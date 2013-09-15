/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* "%code requires" blocks.  */


#include "parser.h"
#include "common.h"
struct Node;




/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOK_IDENTIFIER = 258,
     TOK_B_AND = 259,
     TOK_B_OR = 260,
     TOK_B_XOR = 261,
     TOK_B_NOT = 262,
     TOK_AR_PLUS = 263,
     TOK_AR_MINUS = 264,
     TOK_AR_MUL = 265,
     TOK_AR_DIV = 266,
     TOK_AR_MOD = 267,
     TOK_ASSIGN_OP = 268,
     TOK_COMP_OP = 269,
     TOK_IF = 270,
     TOK_ELSE = 271,
     TOK_ENDEXPR = 272,
     TOK_OPENPAR = 273,
     TOK_CLOSEPAR = 274,
     TOK_OPENBR = 275,
     TOK_CLOSEBR = 276,
     TOK_OPENSQ = 277,
     TOK_CLOSESQ = 278,
     TOK_DOT = 279,
     TOK_COMMA = 280,
     TOK_DOUBLEDOT = 281,
     TOK_ROM_DECL = 282,
     TOK_INT_DECL = 283,
     TOK_FLOAT_DECL = 284,
     TOK_FOR_DECL = 285,
     TOK_WHILE_DECL = 286,
     TOK_DO_DECL = 287,
     TOK_ROM_CONST = 288,
     TOK_INT_CONST = 289,
     TOK_FLOAT_CONST = 290,
     TOK_PRINT = 291,
     TOK_READ = 292,
     TOK_BREAK = 293,
     TOK_CONTINUE = 294,
     TOK_GOTO = 295,
     TOK_STRUCT = 296,
     TOK_UNION = 297,
     TOK_SWITCH = 298,
     TOK_CASE = 299,
     TOK_DEFAULT = 300,
     TMP_VAR = 301,
     LABEL = 302,
     NAME = 303,
     INT_CONST = 304,
     FLOAT_CONST = 305,
     LITERAL = 306,
     IFFALSE = 307,
     IFTRUE = 308,
     ASSIGN = 309,
     OR = 310,
     AND = 311,
     XOR = 312,
     NOT = 313,
     INPUT = 314,
     OUTPUT = 315,
     GOTO = 316,
     CMP = 317,
     EXPR_ERROR = 318,
     STMNT_BLOCK_ERROR = 319,
     IF_WITHOUT_ELSE = 320,
     NOTX = 321,
     UMINUS = 322
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{


	Node *_node;
	AstNode *_AstNode;



} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;

