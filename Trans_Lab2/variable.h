/* 
Распространяется под лицензией zlib,
см. http://www.gzip.org/zlib/zlib_license.html
Разработчик - Иван Якимов.
*/

#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include <string>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"
#include "BaseTypeClass.h"
#include "tml.h"

class TVariable
{
	std::string name;
	BaseTypeInfo *typeTableRef;
	unsigned short memoryOffset;
	bool initialized;

	int iValue;
	float fValue;
	std::string sValue;

	bool isConstValue; /* Можно ли использовать как константное r-value */

	static int g_wordsCount;

	int RomanToInt()
	{
		char romanNums[] = "IVXLCDM";
		int arabicNums[] = {1, 5, 10, 50, 100, 500, 1000};
		int len = sValue.size();
		int *result = new int[len];
		int total = 0, i = 0, j = 0, n = 0;

		// try convert to int from roman
		for (i = 0; i < len; i++)
		{
			for (j = 0; j < sizeof(romanNums); j++)
			{
				if (value[i] = romanNums[j])
					result[i] = arabicNums[j];
			}
		}

		for (n = 0; n < len; n++)
		{
			if (result[n] >= result[n+1])
			{
				total = total + result[n];

			}
			else if (result[n] < result[n+1])
			{
				total = total + (result[n+1]-result[n]);
				n++;
			}
		}
		return total;
	}
public:
	static int GetWordsCount()
	{
		return g_wordsCount;
	}

	TVariable(std::string& name, BaseTypeInfo *Type)
	{
		this->name = name;
		this->typeTableRef = Type;
		memoryOffset = 0;
		initialized = false;
	}

	void SetValue(int value)
	{
		this->iValue = value;
	}

	void SetValue(float value)
	{
		this->fValue = value;
	}

	void SetValue(char *value)
	{
		SetValue(std::string(value));
	}

	void SetValue(std::string& value)
	{
		this->sValue = value;
	}

	BaseTypeInfo *GetType() { return typeTableRef; }
	int SizeOf() { return GetType()->SizeOf(); }

	// Reserve memory space for the variable by the offset
	void ReserveMemory()
	{
		memoryOffset = g_wordsCount;
		g_wordsCount += GetType()->SizeOf();
	}

	// Get the memory offset
	unsigned short GetMemoryOffset() { return memoryOffset; }

	std::string GetName() { return name; }
	void SetInitialized(bool flag)
	{
		initialized = flag;
	}
	bool IsInitialized() { return initialized; }
	
	int ValueToInt()
	{
		switch (typeTableRef->getID())
		{
		case LITERAL_TYPE:
			return atoi(sValue.c_str());	
		case INT_TYPE:
			return iValue;
		case FLOAT_TYPE:
			return atof(sValue.c_str());
		case ROM_TYPE:
			return RomanToInt();
		default:
			return 0;
		}
	}

	double ValueToDouble()
	{
		switch (typeTableRef->getID())
		{
		case INT_TYPE:
			return iValue;
		case LITERAL_TYPE:
			return atof(sValue.c_str());
		case FLOAT_TYPE:
			return fValue;
		case ROM_TYPE:
			return RomanToInt();
		default:
			return 0;
		}
	}

	std::string ValueToString()
	{
		char convert_buf[10];
		switch(typeTableRef->getID())
		{
			case INT_TYPE:
				sprintf(convert_buf, "%d", iValue);
				return std::string(convert_buf);
			case FLOAT_TYPE:
				sprintf(convert_buf, "%f", fValue);
				return std::string(convert_buf);
			case ROM_TYPE:
			case LITERAL_TYPE:
				return std::string(sValue);
		}
	}
};

std::string GenerateVariableName(std::string& prefix, unsigned int number);
TVariable* CreateVariable(BaseTypeInfo *typeTableRef, std::string& name);

class StructType: public BaseTypeInfo
{
protected:
	std::vector<TVariable*> fieldList;
	std::string TypeName;
public:
	virtual int SizeOf() 
	{
		int size = 0;
		for (auto it = fieldList.begin(); it != fieldList.end(); it++)
			size += (*it)->SizeOf();
		return size;
	}

	virtual enumTypes getID() { return enumTypes::STRUCT_TYPE; } 

	void AddField(BaseTypeInfo *type, char *name)
	{
		TVariable *field = CreateVariable(type, std::string(name));
		fieldList.emplace_back(field);
		field->ReserveMemory();
	}

	TVariable *GetField(std::string& fieldName)
	{
		for (auto it = fieldList.begin(); it != fieldList.end(); it++)
			if ((*it)->GetName() == fieldName)
				return (*it);
		return nullptr;
	}

	bool IsFieldDefined(std::string& fieldName)
	{
		return (GetField(fieldName) != nullptr);
	}

	virtual unsigned int StructType::Offset(std::string& fieldName);

	void SetTypeName(char *TypeName)
	{
		this->TypeName = std::string(TypeName);
	}

	std::string GetTypeName()
	{
		return this->TypeName;
	}

	StructType()
		: BaseTypeInfo()
		, fieldList()
	{
	}
	~StructType()
	{
	}
	virtual BaseTypeInfo* Clone() { return new StructType(*this); }
};

class UnionType: public StructType
{
public:	
	virtual enumTypes getID() { return enumTypes::UNION_TYPE; } 

	virtual int SizeOf() 
	{
		int maxSize = 0;
		for (auto it = fieldList.begin(); it != fieldList.end(); it++)
		{
			int curSize = (*it)->SizeOf();
			if(curSize > maxSize)
				maxSize = curSize;
		}
		return maxSize;
	}

	UnionType()
	{
	}
	~UnionType()
	{
	}
	virtual BaseTypeInfo* Clone() { return new UnionType(*this); }
};

#endif
