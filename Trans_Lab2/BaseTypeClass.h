#pragma once

#define TYPE(NAME) NAME ## _TYPE,
enum enumTypes
{
#include "IDtypes.inc"
	__TYPE_LAST
};
#undef TYPE

#define TYPE(NAME) #NAME ,
const char TypeStrings[14][15] =
{
#include "IDtypes.inc"
	__TYPE_LAST
};
#undef TYPE

class BaseTypeInfo
{
public:
	virtual int SizeOf() = 0;
	virtual unsigned int Offset(std::string& fieldName) { return 0; /* unions and base types have no field offsets */}

	virtual std::string GetName() { return std::string(TypeStrings[getID()]); }
	virtual enumTypes getID() = 0;
	void PrintType(FILE *stream);
};

