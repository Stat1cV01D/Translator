/* 
Распространяется под лицензией zlib,
см. http://www.gzip.org/zlib/zlib_license.html
Разработчик - Иван Якимов.
*/

#ifndef _3AC_OP_H_
#define _3AC_OP_H_

#include <string.h>
#include "definitions.h"

/**************************************************************************************/
/*	Функции макроопределения и переменные для работы с опереторами трехадресного кода */
/**************************************************************************************/

/* перечисление операторов трехадресного кода */
typedef enum
{
	OP_PLUS = '+',
	OP_MINUS = '-',
	OP_MULT = '*',
	OP_DIV = '/',
	OP_UMIN,
	OP_OR,
	OP_AND,
	OP_XOR,
	OP_NOT,
	OP_INPUT,
	OP_OUTPUT,
	OP_ASSIGN,
	OP_IFFALSE,
	OP_IFTRUE,
	OP_GOTO,
	OP_EQ,
	OP_NOT_EQ,
	OP_LARGER,
	OP_SMALLER,
	OP_LARGER_OR_EQ,
	OP_SMALLER_OR_EQ,
	OP_ARRAY_ITEM,
	OP_STRUCT_ITEM,
	/* Дополнительные операторы для tml */
	OP_SAVE,
	OP_LOAD,
	/* Операторы, не относящиеся к трехадресному коду: */
	OP_LIST,
	OP_BREAK,
	OP_CONTINUE,
	OP_RETURN,
	OP_SWITCH,
	OP_CASE,
	OP_DEFAULT,
	/* Служебные операторы */
	OP_ERROR,
	OP_INVALID
}opEnum;

#define _3AC_OP_NAME_LENGTH 10

/* Данная структура хранит описание оператора трехадресного кода */
typedef struct
{
	opEnum op;
	char text[_3AC_OP_NAME_LENGTH];
}T3acOpDescription;

static T3acOpDescription g_3acOpDescriptions[] = 
{
	{OP_PLUS, "+"},
	{OP_MINUS, "-"},
	{OP_MULT, "*"},
	{OP_DIV, "/"},
	{OP_UMIN, "-"},
	{OP_OR, "or"},
	{OP_AND, "and"},
	{OP_XOR, "xor"},
	{OP_NOT, "not"},
	{OP_EQ, "=="},
	{OP_NOT_EQ, "!="},
	{OP_LARGER, ">"},
	{OP_SMALLER, "<"},
	{OP_LARGER_OR_EQ, ">="},
	{OP_SMALLER_OR_EQ, "<="},
	{OP_ASSIGN, "="},
	{OP_INPUT, "input"},
	{OP_OUTPUT, "output"},
	{OP_STRUCT_ITEM, "."},
	{OP_IFFALSE, "iffalse"},
	{OP_IFTRUE, "iftrue"},
	{OP_GOTO, "goto"},
	/* Операторы, не относящиеся к трехадресному коду: */
	{OP_LIST, "L"},
	{OP_BREAK, "break"},
	{OP_CONTINUE, "continue"},
	/* Служебные операторы: */
	{OP_INVALID, "###"},
	{OP_ERROR, "error"}
};

/* Возвращает текст оператора */
char *Get3acOperatorText(opEnum op);

/* Возвращает код оператора */
opEnum Get3acOperatorCode(const char *op);

#endif