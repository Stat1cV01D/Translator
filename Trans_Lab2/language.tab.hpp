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
     TOK_ARITHMETICAL_OP = 259,
     TOK_COMP_OP = 260,
     TOK_ASSIGN_OP = 261,
     TOK_U_ARITHMETICAL_OP = 262,
     TOK_B_LOGICAL_OP = 263,
     TOK_U_LOGICAL_OP = 264,
     TOK_IF = 265,
     TOK_ELSE = 266,
     TOK_ENDEXPR = 267,
     TOK_OPENPAR = 268,
     TOK_CLOSEPAR = 269,
     TOK_OPENBR = 270,
     TOK_CLOSEBR = 271,
     TOK_OPENSQ = 272,
     TOK_CLOSESQ = 273,
     TOK_DOT = 274,
     TOK_DOUBLEDOT = 275,
     TOK_ROM_DECL = 276,
     TOK_INT_DECL = 277,
     TOK_FLOAT_DECL = 278,
     TOK_FOR_DECL = 279,
     TOK_WHILE_DECL = 280,
     TOK_DO_DECL = 281,
     TOK_ROM_CONST = 282,
     TOK_INT_CONST = 283,
     TOK_FLOAT_CONST = 284,
     TOK_PRINT = 285,
     TOK_READ = 286,
     TOK_BREAK = 287,
     TOK_CONTINUE = 288,
     TOK_GOTO = 289,
     TOK_STRUCT = 290,
     TOK_UNION = 291,
     TOK_SWITCH = 292,
     TOK_CASE = 293,
     TOK_DEFAULT = 294,
     TMP_VAR = 295,
     LABEL = 296,
     NAME = 297,
     INT_CONST = 298,
     FLOAT_CONST = 299,
     LITERAL = 300,
     IFFALSE = 301,
     IFTRUE = 302,
     ASSIGN = 303,
     OR = 304,
     AND = 305,
     XOR = 306,
     NOT = 307,
     INPUT = 308,
     OUTPUT = 309,
     GOTO = 310,
     CMP = 311,
     EXPR_ERROR = 312,
     STMNT_BLOCK_ERROR = 313,
     IF_WITHOUT_ELSE = 314,
     TOK_U_LOGICAL = 315,
     TOK_U_ARITHMETICAL = 316
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

