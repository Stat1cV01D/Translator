#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "pt.h"

PtNode *createPtNode(const char *typeName)
{
    PtNode *ptNode = (PtNode *)malloc(sizeof(PtNode));
    memset(ptNode, 0, sizeof(PtNode));
    ptNode->type = strdup(typeName);
    return ptNode;
}

void setPtNodeText(PtNode *ptNode, char *text)
{
    ptNode->text = strdup(text);
}

void setPtNodeChildren(PtNode *ptNode, int childrenCount, va_list children)
{
	int i;
	PtNode *cur = va_arg(children, PtNode *);
	ptNode->firstChild = cur;
	for (i=0; i<childrenCount-1; i++)
	{
		PtNode *next = va_arg(children, PtNode *);
		cur->next = next;
		cur = next;
	}
}

void setPtNodeChildren(PtNode *ptNode, int childrenCount, ...)
{
    int i;
    va_list children;
    va_start(children, childrenCount);
    setPtNodeChildren(ptNode, childrenCount, children);
    va_end(children);
}

PtNode *createPtNodeWithChildren(const char *typeName, int childrenCount, ...)
{
	auto result = createPtNode(typeName);
	va_list children;
	va_start(children, childrenCount);
	setPtNodeChildren(result, childrenCount, children);
	va_end(children);
	return result;
}