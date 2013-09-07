/* 
Распространяется под лицензией zlib,
см. http://www.gzip.org/zlib/zlib_license.html
Разработчик - Иван Якимов.
*/

/************************************/
/*	Блок семантического анализатора	*/
/************************************/

#ifndef _SEMANTIC_ANALYZER_
#define _SEMANTIC_ANALYZER_

#include "ast.h"
#include "HashTable.h"
#include "types.h"
#include "label.h"

extern void yyerror(char *s);

/********************/
/*	Проверка типов	*/
/********************/

/* Проверяет совместимость типов, если типы несовместимы - возвращает FALSE, иначе - TRUE*/
BOOL CheckNodeTypes(NodeAST *n1, NodeAST *n2);
BOOL CheckVarNodeTypes(char varName[NAME_LENGTH], NodeAST *n);
BOOL CheckNodeTypeByType(NodeAST *n, Type *t);
BOOL CheckNodeTypeByTypeId(NodeAST *n, enumTypes id);
BOOL CheckVarTypeByTypeId(TVariable *var, enumTypes id);

/****************************/
/*							*/
/****************************/

/* Проверяет переменную на переопределение. В случае переопределение возвращает TRUE, иначе - FALSE*/
BOOL IsVariableRedefinition(char *varName);
/* Проверяет определена ли переменная. В случае если не определена возвращает TRUE, иначе - FALSE*/
BOOL IsVariableUndeclared(char *varName);
/* Возвращает тип переменной. В случае если переменная не объявлена возвращает typeInvalid */
Type* GetVariableTypeRef(char *varName);
/* Проверяет переопределение базового либо пользовательского типа */
BOOL IsTypeRedefinition(char typeName[NAME_LENGTH]);
/* Проверяет на вложенность определений пользовательских типов */
BOOL OnUserTypeDefinition();
/* Проверяет на переопределение поле структуры */
BOOL IsStructFieldRedefinition(char *name, TypeStruct *t);
/* Проверяет объявлен ли тип */
BOOL IsUndeclaredType(char *name);
/* Проверяет, объявлено ли поле структуры */
BOOL IsUndeclaredStructField(char *name, TypeStruct *t);
/* Проверят, объявлена ли метка */
BOOL IsLabelDeclared(char *name);
/* Проверяет таблицу меток на целостность */
void VerifyLabelTable();
/* Проверяет метку на целостность */
void VerifyLabel(void *label);
/* Проверяет является ли тип пользовательским или базовым */
BOOL IsBaseType(enumTypes typeId);

BOOL IsCorrectOperatorInStack(TOperatorTypeEnum operatorType);
/* Работа с оператором case */
BOOL IsRepeatedKeyVal(NodeAST *keyValNode);
BOOL IsInvalieKeyValNode(NodeAST *keyValNode);

BOOL CanUseContinue();
BOOL CanUseBreak();

#endif /* _SEMANTIC_ANALYZER_ */