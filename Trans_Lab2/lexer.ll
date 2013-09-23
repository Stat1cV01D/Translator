%{
#include "parser.h"
#include "pt.h"
#if defined _WIN32 || defined _WIN64
	#include <io.h>			// Для isatty
#endif
#ifdef MSVC
	#define  isatty _isatty		// В VC isatty назван _isatty
#endif
%}

%option yylineno
%option noyywrap
%option warn
%option nounistd

endl				\n
whitespace 			[ \t]
digit 				[0-9]
letter 				[a-zA-Z]
assign 				=
add					\+
sub					\-
mul					\*
div					\/
mod					\%
gt					>
lt					<
eq					==
ne					!=
le					<=
ge					>=
end_expr			;
dot					\.
comma				\,
doubledot			\:
open_par 			\(
close_par 			\)
open_br				\{
close_br			\}
open_sq				\[
close_sq			\]
if					if
else				else

rom_const			(M{0,3})(D?C{0,3}|C[DM])([L]?X{0,3}|X[LC])([V]?I{0,3}|I[VX])

b_and				and
b_or				or
b_xor				xor
b_not				not
ar_plus				{add}
ar_minus			{sub}
ar_mul				{mul}
ar_div				{div}
ar_mod				{mod}
comp_op				{gt}|{lt}|{eq}|{ge}|{le}|{ne}
assign_op			{assign}
punctuator			{end_expr}|{open_par}|{close_par}|{open_br}|{close_br}
keyword				{if}|{else}
identifier 			{letter}({letter}|{digit})*
		
int_const			{digit}+
float_const			({digit}+\.{digit}*)|({digit}*\.{digit}+)
rom_decl			rom
int_decl			int
float_decl			float
string_literal		L?\"(\\.|[^\\"])*\"

for_decl			for
while_decl			while
do_decl				do

%{
#define REACTION(token, return_type) \
					yylval._node = createLexemNode(#token); \
					yylloc.first_column = yylloc.last_column; \
					yylloc.last_column = yylloc.first_column + yyleng; \
					return return_type;
%}

%%
{whitespace}		{
					 yylloc.first_column = yylloc.last_column;
					 yylloc.last_column = yylloc.first_column + yyleng;
					}
{endl}				{
					 yylloc.first_column = 1;
					 yylloc.first_line++;
					 yylloc.last_column = 1;
					 yylloc.last_line++;
					}
{int_const}			{ REACTION(int_const,		TOK_INT_CONST) }
{rom_const}			{ REACTION(rom_const,		TOK_ROM_CONST) }
{float_const}		{ REACTION(float_const,		TOK_FLOAT_CONST) }

{b_and}				{ REACTION(b_and,			TOK_B_AND) }
{b_or}				{ REACTION(b_or,			TOK_B_OR) }
{b_xor}				{ REACTION(b_xor,			TOK_B_XOR) }
{b_not}				{ REACTION(b_not,			TOK_B_NOT) }
{ar_plus}			{ REACTION(ar_plus,			TOK_AR_PLUS) }
{ar_minus}			{ REACTION(ar_minus,		TOK_AR_MINUS) }
{ar_mul}			{ REACTION(ar_mul,			TOK_AR_MUL) }
{ar_div}			{ REACTION(ar_div,			TOK_AR_DIV) }
{ar_mod}			{ REACTION(ar_mod,			TOK_AR_MOD) }

{doubledot}			{ REACTION(dot,				TOK_DOUBLEDOT) }
{dot}				{ REACTION(dot,				TOK_DOT) }
{comma}				{ REACTION(dot,				TOK_COMMA) }
{comp_op}			{ REACTION(comp_op,			TOK_COMP_OP) }
{assign_op}			{ REACTION(assign_op,		TOK_ASSIGN_OP) }
{end_expr}			{ REACTION(end_expr,		TOK_ENDEXPR) }
{open_par}			{ REACTION(open_par,		TOK_OPENPAR) }
{close_par}			{ REACTION(close_par,		TOK_CLOSEPAR) }
{open_br}			{ REACTION(open_br,			TOK_OPENBR) }
{close_br}			{ REACTION(close_br,		TOK_CLOSEBR) }
{open_sq}			{ REACTION(open_sq,			TOK_OPENSQ) }
{close_sq}			{ REACTION(close_sq,		TOK_CLOSESQ) }
{if}				{ REACTION(if,				TOK_IF) }
{else}				{ REACTION(else,			TOK_ELSE) }

{rom_decl}			{ REACTION(rom_decl,		TOK_ROM_DECL) }
{int_decl}			{ REACTION(int_decl,		TOK_INT_DECL) }
{float_decl}		{ REACTION(float_decl,		TOK_FLOAT_DECL) }
{for_decl}			{ REACTION(for_decl,		TOK_FOR_DECL) }
{while_decl}		{ REACTION(while_decl,		TOK_WHILE_DECL) }
{do_decl}			{ REACTION(do_decl,			TOK_DO_DECL) }

print				{ REACTION(print,			TOK_PRINT) }
break				{ REACTION(break,			TOK_BREAK) }
continue			{ REACTION(continue,		TOK_CONTINUE) }
goto				{ REACTION(goto,			TOK_GOTO) }
struct				{ REACTION(struct,			TOK_STRUCT) }
union				{ REACTION(union,			TOK_UNION) }
switch				{ REACTION(switch,			TOK_SWITCH) }
case				{ REACTION(case,			TOK_CASE) }
default				{ REACTION(default,			TOK_DEFAULT) }
{string_literal}	{ 
						yytext = &(yytext[1]);
						yytext[ strlen(yytext)-1 ] = 0;
						REACTION(string_literal,	TOK_STRING_LITERAL)
						// unquote the string
						
					}

{identifier}		{ REACTION(identifier,		TOK_IDENTIFIER) }
.					{
					 yylloc.first_column = yylloc.last_column;
					 yylloc.last_column = yylloc.first_column + yyleng;
					 print_error("Not in alphabet.", yylloc);
					}
%%
