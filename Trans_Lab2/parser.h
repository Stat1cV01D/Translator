#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <stdio.h>
#include "common.h"
#include "context.h"
#include "symbol_table.h"
#include "language.tab.hpp"
//#include "ast.h"
//#include "pt.h"

int yyparse();
int yylex();
void print_error(const char *, YYLTYPE);
void print_warning(const char *, YYLTYPE);

extern char *yytext;
extern FILE *yyin;
extern AstNode *astTree;
extern PtNode *ptTree;

class DimensionAstNode;

Node *addStmntToBlock(Node *block, const Node *stmnt);
BaseTypeInfo *assumeOneOfTypes(const Node *node, YYLTYPE loc, int n, ...);
int GetDimensionInfo(DimensionAstNode *dimensions_list, int **sizes);

#endif // PARSER_H_INCLUDED
