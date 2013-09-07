#include "types.h"
#include "variable.h"

unsigned int StructType::Offset(std::string& fieldName)
{
	unsigned int offset = 0;
	for (auto it = fieldList.begin(); it != fieldList.end(); it++)
	{
		if((*it)->GetName() == fieldName)
			return offset;
		offset += (*it)->SizeOf();
	}
}