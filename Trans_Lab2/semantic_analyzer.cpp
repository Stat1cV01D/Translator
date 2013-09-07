#include "semantic_analyzer.h"

BOOL CheckNodeTypes(NodeAST *n1, NodeAST *n2)
{
	Type *typeOp1, *typeOp2;
	typeOp1 = n1->valueType;
	typeOp2 = n2->valueType;
	if(typeOp1->id != typeOp2->id)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL CheckVarNodeTypes(char *varName, NodeAST *n)
{
	Type *varTypeRef = GetVariableTypeRef(varName);
	if(varTypeRef == NULL)
		return NULL;
	else
	{
		if(((Type*)(varTypeRef))->id != n->valueType->id)
		{
			return FALSE;
		}
		{
			return TRUE;
		}
	}
}

BOOL CheckNodeTypeByType(NodeAST *n, Type* t)
{
	Type *typeOp;
	typeOp = n->valueType;
	if(typeOp->id != t->id)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL CheckNodeTypeByTypeId(NodeAST *n, enumTypes id)
{
	Type *typeOp;
	typeOp = n->valueType;
	if(typeOp->id != id)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL CheckVarTypeByTypeId(TVariable *var, enumTypes id)
{
	int varTypeId = var->typeTableRef->id;
	if(varTypeId != id)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL IsVariableRedefinition(char *varName)
{
	TVariable *item;
	item = SearchHashTableItem(varName, g_variableTable);
	if(item == NULL) /* Переменная не объявлена */
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL IsVariableUndeclared(char *varName)
{
	return (!IsVariableRedefinition(varName));
}

Type* GetVariableTypeRef(char *varName)
{
	TVariable *varRef;
	varRef = (TVariable *)SearchHashTableItem(varName, g_variableTable);
	if(varRef == NULL) /* Переменная не объявлена */
	{
		return NULL;
	}
	else
	{
		return ((TVariable *)varRef)->typeTableRef;
	}
}

BOOL IsTypeRedefinition(char typeName[NAME_LENGTH])
{
	Type *typeRef = NULL;
	// Проверка на переопределение типа
	typeRef = SearchHashTableItem(typeName, g_typeTable);
	if(typeRef != NULL)
		return TRUE;
	else
		return FALSE;
}

BOOL OnUserTypeDefinition()
{
	Type *t;
	t = PopUserType();
	PushUserType(t);
	if(t != NULL)
		return TRUE;
	else 
		return FALSE;
}

BOOL IsStructFieldRedefinition(char *name, TypeStruct *t)
{
	if(SearchListItem(name, t->fieldList))
		return TRUE;
	else 
		return FALSE;
}

BOOL IsUndeclaredType(char *name)
{
	g_typeTable->st_get(name)
		return FALSE;
	else
		return TRUE;
}

BOOL IsUndeclaredStructField(char *name, StructType *t)
{
	return (t->GetField(std::string(name)) != nullptr);
}

BOOL IsLabelDeclared(char *name)
{
	TLabel *label;
	return (((label = g_labelTable->st_get(name)) != nullptr) && label->isDeclared);
}

void VerifyLabelTable()
{
	g_labelTable->ProcessHashTable(
		[=] (TLabel*& label) -> bool
		{
			if(!label->isDeclared())
			{
				printf("%d: error: label %s is not declared\n", label->usepointLineNumber, label->name);
			}
			else if(!label->isUsed())
			{
				printf("%d: warning: label %s is declared but not used\n", label->declarationLineNumber, label->name);
			}
		}
	);
}


BOOL IsRepeatedKeyVal(NodeAST *keyValNode)
{
	TSwitchOperator *switchOp = PopOperator();
	BOOL result;
	if(NULL != SearchListItemByPredicate(keyValNode, CheckCaseKeyVal, switchOp->caseList))
	{
		result = TRUE;
	}
	else
	{
		result = FALSE;
	}
	PushOperator(switchOp);
	return result;
}

BOOL IsInvalieKeyValNode(NodeAST *keyValNode)
{
	if(!CheckNodeTypeByTypeId(keyValNode, TYPE_INT) || keyValNode->nodetype != ASTN_CONST)
		return TRUE;
	else 
		return FALSE;
}

BOOL CanUseContinue()
{
	BOOL result;
	TOperator *op = PopOperator();
	if(IsLoopOperator(op))
		result = TRUE;
	else
		result = FALSE;
	PushOperator(op);
	return result;
}

BOOL CanUseBreak()
{
	BOOL result;
	TOperator *op = PopOperator();
	if(IsLoopOperator(op))
		result = TRUE;
	else if(IsSwitchOperator(op))
		result = TRUE;
	else 
		result = FALSE;
	PushOperator(op);
	return result;
}

BOOL IsCorrectOperatorInStack(TOperatorTypeEnum operatorType)
{
	BOOL result;
	TOperator *op = PopOperator();
	if(op->type != operatorType)
		result = FALSE;
	else
		result = TRUE;
	PushOperator(op);
	return result;
}
