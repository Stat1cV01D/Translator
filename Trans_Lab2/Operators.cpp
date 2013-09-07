#include "Operators.h"


OperatorStack::OperatorStack(void)
	: g_operatorStack()
{
	m_stackTop = 0;
}


OperatorStack::~OperatorStack(void)
{
}

void PrintOperator(FILE *stream, void *item)
{
	//TODO:
}
/*
void WriteCaseGoto(FILE *stream, void *item)
{
	TCaseOperator *caseOp = (TCaseOperator *)item;
	TSwitchOperator *switchOp = caseOp->switchOp;
	NodeAST *assignNode = HandleBinaryOperation(OP_EQ, switchOp->key, caseOp->keyVal);
	CodegenGoto(stream, OP_IFTRUE, caseOp->label, assignNode);
	// Заглушка - нельзя высвобождать через FreeAST, т.к. в этом случае будут удалены узлы key и keyVal
	// Замечание - хотя данная заглушка и разрушает целостность работы с ast, но она вполне корректна
	if(assignNode)
		free(assignNode);
}
*/

void OperatorStack::Push(TOperator *op)
{
	this->g_operatorStack.emplace_back(op);
}

TOperator *OperatorStack::Pop()
{
	auto result = Top();
	g_operatorStack.erase(g_operatorStack.end());
	return result;
}

TOperator *OperatorStack::Top()
{
	return (*(this->g_operatorStack.end()));
}

bool OperatorStack::IsEmpty()
{
	return g_operatorStack.empty();
}

bool OperatorStack::IsLoopOperator(TOperator *op)
{
	switch(op->GetType())
	{
	case OT_FOR:
	case OT_WHILE:
	case OT_DO_WHILE:
		return true;
	}
	return false;
}

bool OperatorStack::IsConditionalOperator(TOperator *op)
{
	switch(op->GetType())
	{
	case OT_IF_ELSE:
		return true;
	}
	return false;
}

bool OperatorStack::IsSwitchOperator(TOperator *op)
{
	switch(op->GetType())
	{
	case OT_SWITCH:
		return true;
	}
	return false;
}
