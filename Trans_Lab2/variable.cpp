#include "variable.h"
#include <string>

int TVariable::g_wordsCount = 0;

TVariable* CreateVariable(BaseTypeInfo *typeTableRef, std::string& name)
{
	return new TVariable(name, typeTableRef);
}

std::string GenerateVariableName(std::string& prefix, unsigned int number)
{
	char convert_buf[10];
	sprintf(convert_buf, "%d", number);
	std::string result(convert_buf);
	return (prefix + result);
}