/* 
���������������� ��� ��������� zlib,
��. http://www.gzip.org/zlib/zlib_license.html
����������� - ���� ������.
*/

#ifndef _TYPES_H_
#define _TYPES_H_

#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include "definitions.h"
#include <stdarg.h> 
//#include "variable.h"
#include "BaseTypeClass.h"
#include "tml.h"
//#include "ast.h"

extern void FreeVariable(void *var);
extern void PrintVariable(FILE *stream, void *item);

#define SIZE_OF_INT		1 //sizeof(int) < sizeof(TMemoryCell)
#define SIZE_OF_FLOAT	1 //sizeof(float) < sizeof(TMemoryCell)

class BoolType: public BaseTypeInfo
{
public:
	virtual int SizeOf() { return SIZE_OF_INT; }
	virtual enumTypes getID() { return enumTypes::BOOL_TYPE; }
	BoolType() {}
	virtual BaseTypeInfo* Clone() { return new BoolType(*this); }
};

class IntType: public BaseTypeInfo
{
public:
	virtual int SizeOf() { return SIZE_OF_INT; }
	virtual enumTypes getID() { return enumTypes::INT_TYPE; }
	IntType() {}
	virtual BaseTypeInfo* Clone() { return new IntType(*this); }
};

class LiteralType: public BaseTypeInfo
{
protected:
	uint16_t length;
public:
	uint16_t GetLength() { return length; }
	virtual std::string GetName() 
	{ 
		char convert_buf[10];
		sprintf(convert_buf, " %d", SizeOf());
		return BaseTypeInfo::GetName()+std::string(convert_buf); 
	}

	virtual enumTypes getID() { return enumTypes::LITERAL_TYPE; }

	virtual int SizeOf()
	{
		if (length % sizeof(TMemoryCell) != 0)
			return length / sizeof(TMemoryCell) + 1;
		else
			return length / sizeof(TMemoryCell); 
	}

	LiteralType(uint16_t length) { this->length = length; }
	virtual BaseTypeInfo* Clone() { return new LiteralType(*this); }
};

class FloatType: public BaseTypeInfo
{
public:
	virtual int SizeOf() { return SIZE_OF_FLOAT; }
	virtual enumTypes getID() { return enumTypes::FLOAT_TYPE; }
	FloatType() {}
	virtual BaseTypeInfo* Clone() { return new FloatType(*this); }
};

class RomanType: public BaseTypeInfo
{
protected:
	unsigned short length;
public:
	virtual enumTypes getID() { return enumTypes::ROM_TYPE; }

	virtual int SizeOf() { return SIZE_OF_INT; }
	RomanType() { this->length = 10; }
	virtual BaseTypeInfo* Clone() { return new RomanType(*this); }
};

class InvalidType: public BaseTypeInfo
{
public:
	virtual int SizeOf() { return 0; }
	virtual enumTypes getID() { return enumTypes::INVALID_TYPE; }
	InvalidType() {}
	virtual BaseTypeInfo* Clone() { return new InvalidType(*this); }
};

class VoidType: public BaseTypeInfo
{
public:
	virtual int SizeOf() { return 0; }
	virtual enumTypes getID() { return enumTypes::VOID_TYPE; }
	VoidType(){}
	virtual BaseTypeInfo* Clone() { return new VoidType(*this); }
};

class ArrayType: public BaseTypeInfo
{
	std::vector<int> sizes;
	BaseTypeInfo *itemTypeRef; //The base type of the array
public:
	virtual int SizeOf() 
	{
		int itemsCount = 1;
		for(auto it = sizes.begin(); it != sizes.end(); it++)
			itemsCount *= *it;
		return (itemsCount * itemTypeRef->SizeOf());
	}
	
	virtual std::string GetName() 
	{ 
		std::string dimensions;

		for (auto it = sizes.begin(); it != sizes.end(); it++)
		{
			char convert_buf[10];
			sprintf(convert_buf, "[%d]", (*it));
			dimensions += std::string(convert_buf);
		}

		return itemTypeRef->GetName()+std::string(" array")+dimensions;
	}

	std::vector<int>& GetSizes() { return sizes; }
	
	BaseTypeInfo *GetBaseType() { return itemTypeRef; }

	ArrayType(BaseTypeInfo *itemTypeRef, int arity, ...)
		: sizes()
	{
		this->itemTypeRef = itemTypeRef;
		sizes.reserve(arity);

		va_list sizesList;
		va_start(sizesList, arity);
		for (int i=0; i<arity; i++)
		{
			sizes.emplace_back(va_arg(sizesList, int));
		}
		va_end(sizesList);
	}

	ArrayType(BaseTypeInfo *itemTypeRef, std::vector<int> &SizesList)
		: sizes()
	{
		this->itemTypeRef = itemTypeRef;
		this->sizes = SizesList;
	}

	virtual enumTypes getID() { return enumTypes::ARRAY_TYPE; } 
	virtual BaseTypeInfo* Clone() { return new ArrayType(*this); }
};

// Struct and Union types are in Variable module

#endif /* _TYPES_H_ */