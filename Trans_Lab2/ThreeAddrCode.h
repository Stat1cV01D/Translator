#ifndef THREEADDRCODE_H_INCLUDED
#define THREEADDRCODE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "ast.h"
#include "parser.h"

extern FILE* g_outputFile;

int form_3addrCode(const AstNode *astNode, int tab);

#endif