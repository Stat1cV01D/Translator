#include "basic-ops.h"

char *Get3acOperatorText(opEnum op)
{
	int i=0;
	for(i; i < sizeof(g_3acOpDescriptions); i++)
	{
		if(g_3acOpDescriptions[i].op == op)
			return g_3acOpDescriptions[i].text;
	}
	return "";
}

opEnum Get3acOperatorCode(const char *op)
{
	int i=0;
	for(i; i < sizeof(g_3acOpDescriptions); i++)
	{
		if(strcmp(g_3acOpDescriptions[i].text, op) == EQ_STRINGS)
			return g_3acOpDescriptions[i].op;
	}
	return OP_INVALID;
}