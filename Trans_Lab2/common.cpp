#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "parser.h"
#include "ast.h"
#include "pt.h"

extern char* yytext;

Node *createNode(AstNode *astNode, PtNode *ptNode)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->astNode = astNode;
    node->ptNode = ptNode;
    return node;
}

Node *createLexemNode(const char *typeName)
{
    PtNode *ptNode = createPtNode(typeName);
    setPtNodeText(ptNode, yytext);
    Node *node = createNode(NULL, ptNode);
    return node;
}

char *strcatn(int n, ...)
{
    va_list args;
    va_start(args, n);
    int i;
    int len = 0;
    for (i=0; i<n; i++)
    {
        len += strlen(va_arg(args, const char *));
    }
    va_end(args);
    va_start(args, n);
    char *result = (char *)malloc(len + 1);
    result[0] = 0;
    for (i=0; i<n; i++)
    {
        strcat(result, va_arg(args, const char *));
    }
    va_end(args);
    return result;
}

const char *typeToStr(enumTypes type)
{
    return TypeStrings[type];
}
