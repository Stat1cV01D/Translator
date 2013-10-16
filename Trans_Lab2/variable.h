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
#include "types.h"
#include "tml.h"

class TVariable
{
	std::string name;
	BaseTypeInfo *typeTableRef;
	uint16_t memoryOffset;
	bool initialized;
	bool isAllocated;

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
				if (sValue[i] = romanNums[j])
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

	void Init(std::string& name, BaseTypeInfo *Type)
	{
		this->name = name;
		this->typeTableRef = Type;
		memoryOffset = 0;
		initialized = false;
		isAllocated = false;
	}

	void SetStrValue(std::string &value, bool typeCheck)
	{
		if (typeCheck && this->typeTableRef->getID() != LITERAL_TYPE)
		{
			throw std::string("Error: type assign mismatch");
		}
		
		sValue = value;
		initialized = true;
	}

	void SetMemoryOffset(int offset)
	{
		memoryOffset = offset;
		isAllocated = true;
	}

#define SETVALUEMACRO(typeID, thisValue) \
	if (this->typeTableRef->getID() == typeID) { \
		initialized = true; \
		this->thisValue = value; \
	} else { \
		throw std::string("Error: type assign mismatch"); \
	}

public:
	TVariable(std::string& name, BaseTypeInfo *Type)
	{
		Init(name, Type);
	}
	
	TVariable(std::string& name, BaseTypeInfo *Type, int offset)
	{
		Init(name, Type);
		SetMemoryOffset(offset);
	}

	~TVariable()
	{
		delete typeTableRef;
	}

	void SetValue(BaseTypeInfo *Type, std::string& initValue)
	{
		delete typeTableRef;
		typeTableRef = Type;
		SetStrValue(initValue, false);
	}

	void SetValue(int value)
	{
		SETVALUEMACRO(INT_TYPE, iValue)
	}
	void SetValue(float value)
	{
		SETVALUEMACRO(FLOAT_TYPE, fValue)
	}
	void SetValue(std::string& value)
	{
		SetStrValue(value, true);
	}
	void SetValue(char *value)
	{
		SetValue(std::string(value));
	}

	static int GetWordsCount()
	{
		return g_wordsCount;
	}

	BaseTypeInfo *GetType() { return typeTableRef; }
	int SizeOf() { return GetType()->SizeOf(); }

	// Reserve memory space for the variable by the offset
	void ReserveMemory()
	{
		if (!isAllocated)
		{
			SetMemoryOffset(g_wordsCount);
			g_wordsCount += GetType()->SizeOf();
		}
	}

	// Get the memory offset
	uint16_t GetMemoryOffset() { return memoryOffset; }

	std::string GetName() { return name; }
	bool IsInitialized() { return initialized; }
	
	void SetConst(bool value) { isConstValue = value; }
	bool IsConst() { return isConstValue; }

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
			default:
				return std::string("Error type converting to string ") + typeTableRef->GetName();
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
