#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include "parser.h"
#include "ThreeAddrCode.h"
#include "language.tab.hpp"
#include "AstUtils.h"
#include "ast.h"
#include "pt.h"
#include "tml-generator.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

void print_tab(int tab);
int print_astNode(AstNode *astNode, int tab);
void print_ptNode(const PtNode *ptNode, int tab);
void print_astTree();
void print_ptTree();
void print_3ac();
void MakeTML();

// Это получаем от Bison/Yacc
extern int yyparse(void);
extern FILE *yyin;
extern int yydebug;

extern ParserContext Context;

int main(int argc, char* argv[])
{
    TBlockContext::Init();

    int printPt, printAst, print3AC;
    if (argc < 2)
    {
        printPt = printAst = 1;
    }
    else
    {
        printPt = 0;
        printAst = 1;
        FILE *infile = fopen(argv[argc-1],"r");
        yyin = infile;
        int i;
        for (i=1; i<argc-1; ++i)
        {
            if (!strcmp(argv[i], "-no-tree"))
                printPt = 0;
            else if (!strcmp(argv[i], "-tree"))
                printPt = 1;
            else if (!strcmp(argv[i], "-no-ast"))
                printAst = 0;
            else if (!strcmp(argv[i], "-ast"))
                printAst = 1;
			else if (!strcmp(argv[i], "-no-3ac"))
				print3AC = 0;
			else if (!strcmp(argv[i], "-3ac"))
				print3AC = 1;
            else
                printf("Unknown parameter: %s\n", argv[i]);
        }
    }

	yydebug = 1;
	try
	{
		int errcode = yyparse();

		printf("\n");
		if(errcode == 0)
		{
			if (printAst) print_astTree();
			if (printPt) print_ptTree();
			if (print3AC) print_3ac();
			MakeTML();
		}
		else
		{
			printf("There are syntax errors!");
		}
	}
	catch (BisonException& e)
	{
		; // do nothing here
	}
	getchar();
    exit (0);
}

void print_3ac()
{
	char FN[MAX_PATH];
	sprintf(FN, "_out.txt");
	FILE *g_outputFile = fopen(FN, "w+b");
	
	TACWriter CGInfo(&Context, g_outputFile);
	CGInfo.CodeGen(astTree);

	fclose(g_outputFile);
}

void MakeTML()
{
	char FN[MAX_PATH];
	sprintf(FN, "_out.tml");
	
	FILE *g_OutputTmlFile = fopen(FN, "w+b");

	try
	{
		TMLWriter TmlWrite(&Context, g_OutputTmlFile);
		TmlWrite.Serialize(astTree);
		TmlWrite.AfterProcess();
	}
	catch (std::string e)
	{
		fprintf(stderr, "Error: %s\n", e.c_str());
	}
	fclose(g_OutputTmlFile);
}

void print_astTree()
{
    printf("Abstract syntax tree:\n---------------------\n\n");

	AstPrintInfo PrintInfo(&Context, stdout);
	PrintInfo.Print(astTree);
    printf("\n");
}

void print_ptTree()
{
    printf("Parse tree:\n-----------\n\n");
	
	PtPrintInfo PrintInfo(&Context, stdout);
	PrintInfo.Print(ptTree);

	printf("\n");
}
