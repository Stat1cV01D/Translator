%{
#include "definitions.h"
#include "ast.h"
#include "tml-generator.h"
#include "language.tac.hpp"
#if defined _WIN32
#include <io.h>			// Для isatty
#elif defined _WIN64
#include <io.h>			// Для isatty
#endif
#ifdef MSVC
#define  isatty _isatty		// В VC isatty назван _isatty
#endif
%}

%option yylineno
%option noyywrap
%option nounistd
%option prefix="tac"

EXP	([Ee][-+]?[0-9]+)

%{
#define TAC_REACTION(token, return_type) \
					yylval._node = createLexemNode(token); \
					yylloc.first_column = yylloc.last_column; \
					yylloc.last_column = yylloc.first_column + yyleng; \
					return return_type;
%}

%%
":="			{
                    return ASSIGN;
                }
"or"			{
					return OR;
				}
"and"			{
					return AND;
				}
"xor"			{
					return XOR;
				}
"not"			{
					return NOT;
				}
"input"			{
					return INPUT;
				}
"output"		{
					return OUTPUT;
				}
"goto"			{
					return GOTO;
				}
"iffalse"		{
					return IFFALSE;
				}
"iftrue"		{
					return IFTRUE;
				}
":"				|
"."				|
"["				|
"]"				|
"+"				|
"-"				|
"*"				|
"/"				{
					return tactext[0];
				}
">="			|
"<="			|
"<"				|
">"				|
"="				|
"<>"			{
					TAC_REACTION(tactext, CMP);
				}
"$t"[0-9]+		{
					TAC_REACTION(tactext, TMP_VAR);
				}
"$L"[0-9]+		{
					TAC_REACTION(tactext, LABEL);
				}
0[0-7]+			{
					TAC_REACTION(tactext, INT_CONST);
				}
[0-9]+			{
					TAC_REACTION(tactext, INT_CONST);
                }
([0-9]*\.[0-9]+|[0-9]+\.){EXP}? | //  { taclval._d = atof(tactext); return _FLOAT_VAL; }
[0-9]+{EXP}						{ TAC_REACTION(tactext, FLOAT_CONST); }

[a-zA-Z][a-z0-9A-Z]*	{
					// TODO: Обработка ограничения длины имени переменной
					TAC_REACTION(tactext, NAME);
                }
[ \t]			{;}	
.               { printf("Lexical error: \'%s\' in line  %d\n", tactext, taclineno); }
%%
