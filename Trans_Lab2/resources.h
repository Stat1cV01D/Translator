/* 
Распространяется под лицензией zlib,
см. http://www.gzip.org/zlib/zlib_license.html
Разработчик - Иван Якимов.
*/

#ifndef _RESOURCES_H_
#define _RESOURCES_H_

#include "definitions.h"
#ifdef LANG_ENG

/* Сообщения об ошибках семантического анализатора */

#define UNDECLARED_VARIABLE_ERROR		"undeclared variable"
#define UNDECLARED_TYPE					"undeclared type"
#define TYPES_INCOMPATIBLE_ERROR		"types incompatible"
#define VARIABLE_REDEFINITION_ERROR		"redefinition of the variable"
#define UNEXPECTED_BREAK_ERROR			"unexpected break"
#define UNEXPECTED_CONTINUE_ERROR		"unexpected continue"
#define TYPE_REDEFINITION_ERROR			"type redefinition"
#define INVALID_ASSIGN_L_VALUE			"expression on left of ':=' must be a base type variable"
#define ERROR_DECLARATIONS_FIRST		"variable declarations should go before any statements in the block"
/* structs */
#define UNDECLARED_STRUCT_FIELD			"undeclared struct field"
#define STRUCT_FIELD_REDEFINITION		"struct field redefinition"
#define EMBEDDED_USER_TYPE_DEFINITION	"embedded user type definition"
#define USER_TYPE_STRUCT_FIELD			"user type struct field"
#define INVALID_STRUCT_FIELD_L_VALUE	"expression on left of '.' must be a struct or union"
#define INVALID_STRUCT_FIELD_TYPE		"invalid struct field type"
/* labels */
#define LABEL_IS_ALREADY_DECLARED		"the label is already declared"
/* arrays */
#define INVALID_ARRAY_DECLARATION		"array dimensions must be defined with constants"
#define ARRAY_STRUCT_FIELD				"array struct field"
#define INVALID_ARRAY_ITEM				"expression on left of '[' must be an array"
#define USER_TYPE_ARRAY					"user type array"
/* case */
#define REPEATED_CASE_KEY_VALUE			"case label key values mustn't be repeated"
#define INVALID_CASE_KEY_VAL_TYPE		"case label key value must be an integer constant"
#define INVALID_SWITCH_KEY_TYPE			"switch key must be an integer type"

/* Синтаксические ошибки */

#define EXPECTED_OPEN_PARANTHESIS		"expected '('"
#define EXPECTED_CLOSE_PARANTHESIS		"expected ')'"
#define EXPECTED_OPEN_BRACE				"expected '{'"
#define EXPECTED_CLOSE_BRACE			"expected '}'"
#define EXPECTED_OPEN_BRACKET			"expected '['"
#define EXPECTED_CLOSE_BRACKET			"expected ']'"
#define EXPECTED_SEPARATOR				"expected ';'"

/* Некоторые внутренние ошибки */
#define BAD_OPERATOR					"internal error: bad operator in stack\n"

#endif /* LANG_ENG */

#endif /* _RESOURCES_H_ */