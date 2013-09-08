#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "parser.h"
#include "common.h"
#include "ast.h"
#include "pt.h"

Node *addStmntToBlock(Node *block, const Node *stmnt)
{
    PtNode *ptNode = block->ptNode;
    if (!ptNode->firstChild)
    {
        ptNode->firstChild = stmnt->ptNode;
    }
    else
    {
        ptNode = ptNode->firstChild;
        while (ptNode->next) ptNode = ptNode->next;
        ptNode->next = stmnt->ptNode;
    }

    StatementBlockAstNode *astNode = dynamic_cast<StatementBlockAstNode*>(block->astNode);
	if (astNode != nullptr)
	{
		astNode->AddStatement(stmnt->astNode);
	}
    return block;
}

void AssertOneOfTypes( const Node *node, YYLTYPE loc, int n, ... )
{
	va_list args;
	va_start(args, n);
	int i;
	int len = 0;
	for (i=0; i<n; i++)
	{
		if (node->astNode->GetResultType()->getID() == va_arg(args, enumTypes))
			return; // no errors here
	}
	va_end(args);

	va_start(args, n);
	const char *gotType = typeToStr(node->astNode->GetResultType()->getID());
	char errStr[200];
	sprintf(errStr, "Got \'%s\', expected: ", gotType);
	for (i=0; i < n-1; i++)
	{
        const char *expectedType = typeToStr(va_arg(args, enumTypes));
		strcat(errStr, expectedType);
		strcat(errStr, ", ");
    }
	const char *expectedType = typeToStr(va_arg(args, enumTypes));
	strcat(errStr, expectedType);
	strcat(errStr, ".");
	va_end(args);

	print_error(errStr, loc);
}


int GetDimensionInfo(DimensionAstNode *dimensions_list, int **sizes)
{
	DimensionAstNode *cur;
	int numDimensions = 0;
	for (cur = dimensions_list; cur && cur->GetNextDim() != nullptr; cur = cur->GetNextDim())
	{
		numDimensions++;
	}

	if (sizes != NULL && numDimensions > 0)
	{
		DimensionAstNode *cur;
		int i = 0;
		*sizes = new int[numDimensions];
		for (cur = dimensions_list; cur && cur->GetNextDim() != nullptr; cur = cur->GetNextDim())
		{
			auto valueNode = dynamic_cast<NumValueAstNode*>(cur->GetExpr());
			if (valueNode == nullptr)
				break;
			(*sizes)[i++] = valueNode->ToInt();
		}
	}

	return numDimensions;
}

void print_warning(const char *message, YYLTYPE loc)
{
	printf("Warning at %d:%d - %d:%d\n\t%s\n", loc.first_line, loc.first_column,
		loc.last_line, loc.last_column, message);
	//exit(0);
}

void print_error(const char *message, YYLTYPE loc)
{
	printf("Error at %d:%d - %d:%d\n\t%s\n", loc.first_line, loc.first_column,
		loc.last_line, loc.last_column, message);
	//exit(0);
}


