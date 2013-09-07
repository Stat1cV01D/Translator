/* 
Распространяется под лицензией zlib,
см. http://www.gzip.org/zlib/zlib_license.html
Разработчик - Иван Якимов.
*/

#ifndef _TYPES_H_
#define _TYPES_H_

#include <vector>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include "definitions.h"
#include <stdarg.h> 
#include "variable.h"
#include "BaseTypeClass.h"
//#include "ast.h"

extern void FreeVariable(void *var);
extern void PrintVariable(FILE *stream, void *item);

#define SIZE_OF_INT		1
#define SIZE_OF_FLOAT	1
#define CHAR_IN_WORD	4

class IntType: public BaseTypeInfo
{
public:
	virtual int SizeOf() { return SIZE_OF_INT; }
	virtual enumTypes getID() { return enumTypes::INT_TYPE; }
	IntType() {}
};

class LiteralType: public BaseTypeInfo
{
protected:
	unsigned short length;
public:
	int GetLength() {return length;}
	virtual std::string GetName() 
	{ 
		char convert_buf[10];
		sprintf(convert_buf, " %d", SizeOf());
		return BaseTypeInfo::GetName()+std::string(convert_buf); 
	}

	virtual enumTypes getID() { return enumTypes::LITERAL_TYPE; }

	virtual int SizeOf() 
	{ 			
		if( length % CHAR_IN_WORD)
			return length / CHAR_IN_WORD + 1;
		else
			return length / CHAR_IN_WORD; 
	}

	LiteralType(unsigned short length) {this->length = length; }
};

class FloatType: public BaseTypeInfo
{
public:
	virtual int SizeOf() { return SIZE_OF_FLOAT; }
	virtual enumTypes getID() { return enumTypes::FLOAT_TYPE; }
	FloatType() {}
};

class RomanType: public BaseTypeInfo
{
protected:
	unsigned short length;
public:
	int GetLength() { return length; }

	virtual enumTypes getID() { return enumTypes::ROM_TYPE; }

	virtual int SizeOf() { return GetLength(); }
	RomanType() { this->length = 10; }
};

class InvalidType: public BaseTypeInfo
{
public:
	virtual int SizeOf() { return 0; }
	virtual enumTypes getID() { return enumTypes::INVALID_TYPE; }
	InvalidType() {}
};

class VoidType: public BaseTypeInfo
{
public:
	virtual int SizeOf() { return 0; }
	virtual enumTypes getID() { return enumTypes::VOID_TYPE; }
	VoidType(){}
};

class ArrayType: public BaseTypeInfo
{
	std::vector<int> sizes;
	BaseTypeInfo *itemTypeRef; //The base type of the array
public:
	virtual int SizeOf() {
		int itemsCount = 0;
		for(auto it = sizes.begin(); it != sizes.end(); it++)
			itemsCount += *it;
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

	std::vector<int> GetSizes() const { return sizes; }
	
	BaseTypeInfo *GetBaseType() { return itemTypeRef; }

	ArrayType(BaseTypeInfo *itemTypeRef, int arity, ...):
		sizes()
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

	ArrayType(BaseTypeInfo *itemTypeRef, std::vector<int> &SizesList):
		sizes()
	{
		this->itemTypeRef = itemTypeRef;
		this->sizes = SizesList;
	}

	virtual enumTypes getID() { return enumTypes::ARRAY_TYPE; } 
};

// Struct and Unoin types are in Variable module

#endif /* _TYPES_H_ */