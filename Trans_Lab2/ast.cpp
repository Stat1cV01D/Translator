#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "AstUtils.h"
#include "tml-generator.h"

int ConditionalAstNode::Print3AC(TACWriter* output)
{
	int labelNumber_false = output->GetContext()->GetNextLabelNumber();
	int labelNumber_end = labelNumber_false;
	if (this->false_block != nullptr)
		labelNumber_end = output->GetContext()->GetNextLabelNumber();

	// 1. generate the code for condition, return result to A
	output->CodeGen(cond);
	// 2. check the result and process true_block (TODO [SV] 14.08.13 16:14: with possible optimization|elimination?)
	output->CodeWriteFormat("\tiffalse\t$A\tL%d\n", labelNumber_false);
	output->CodeGen(true_block);

	if (this->false_block != nullptr)
	{
		output->CodeWriteFormat("\tgoto\t$L%d\n", labelNumber_end); // for the iftrue block to end
		output->CodeGen(false_block);
	}

	output->CodeWriteFormat("$L%d:\n", labelNumber_end);
	return 0;
}
int ConditionalAstNode::PrintASTree(AstPrintInfo* output)
{
	output->AstWriteLine("if_stmnt");
	output->AstWriteLine("cond:");
	output->Print(cond);
	output->AstWriteLine("if_true:");
	output->Print(true_block);
	if (this->false_block != nullptr)
	{
		output->AstWriteLine("else:");
		output->Print(false_block);
	}
	return 0;
}
int ConditionalAstNode::Serialize(TMLWriter* output)
{
	auto labelNumber_false = output->GetContext()->GenerateNewLabel();
	auto labelNumber_end = labelNumber_false;
	if (this->false_block != nullptr)
		labelNumber_end = output->GetContext()->GenerateNewLabel();

	// 1. generate the code for condition, return result to A
	output->Serialize(cond);
	// 2. check the result and process true_block (TODO [SV] 14.08.13 16:14: with possible optimization|elimination?)
	OperatorAstNode op(OP_IFFALSE, new LabelAstNode(labelNumber_end));
	output->Serialize(&op);
	output->Serialize(true_block);

	if (this->false_block != nullptr)
	{
		OperatorAstNode op_goto(OP_GOTO, new LabelAstNode(labelNumber_end));
		output->Serialize(&op_goto);

		output->Serialize(false_block);
	}

	output->BindLabelToNextWrittenInstruction(labelNumber_end);
	return 0;
}

int OperatorAstNode::Print3AC(TACWriter* output)
{
	switch (this->GetOpID())
	{
	case OP_ARRAY_ITEM:
		{
			if (dynamic_cast<DimensionAstNode *>(this->right) == nullptr)
				return 1; // Error!

			// The dimensions will be pushed to stack
			output->CodeGen(right);

			// Now we need to get number of dimensions and the sizes
			VarAstNode *astNode = dynamic_cast<VarAstNode *>(this->left);
			if (astNode == nullptr)
				return 1; // Error!

			ArrayType *varType = dynamic_cast<ArrayType*>(astNode->GetTableReference()->GetType());
			if (varType == nullptr)
				return 1; // Error!

			auto it_end = varType->GetSizes().end();

			int PopTmpVar = output->GetContext()->GetNextTmpVarIndex();
			int SumTmpVar = output->GetContext()->GetNextTmpVarIndex();
			
			for (auto it = varType->GetSizes().begin(); it != it_end; it++)
			{
				output->CodeWriteFormat("\tPop\t$t%d\n", PopTmpVar);
				output->CodeWriteFormat("\t$t%d\t=\t$t%d\t*\t%d\n", PopTmpVar, PopTmpVar, (*it));
				output->CodeWriteFormat("\t$t%d\t=\t$t%d\t+\t$t%d\n", SumTmpVar, SumTmpVar, PopTmpVar);
			}
			output->CodeWriteFormat("\t$t%d\t=", output->GetContext()->GetNextTmpVarIndex());
			output->CodeGen(left);
			// $A = $A + $B (original array pointer + offset)
			output->CodeWriteFormat("\t+\t$t%d\n", SumTmpVar); // TODO [SV] 15.08.13 14:41: Make this a pointer?
		}
		break;
	case OP_STRUCT_ITEM:
		break;
	case OP_ASSIGN:
		output->CodeGen(left);
		output->CodeWriteFormat("\t%s\t", this->op_3ac_name.c_str());
		output->CodeGen(right);
		output->CodeWrite("\n");
		break;
	case OP_PLUS:
	case OP_MINUS:
	case OP_MULT:
	case OP_DIV:
	case OP_UMIN:
	case OP_OR:
	case OP_AND:
	case OP_XOR:
	case OP_NOT:
	case OP_EQ:
	case OP_NOT_EQ:
	case OP_LARGER:
	case OP_LARGER_OR_EQ:
	case OP_SMALLER:
	case OP_SMALLER_OR_EQ:
		{
			output->CodeWriteFormat("\t$t%u\t=", output->GetContext()->GetNextTmpVarIndex());
			output->CodeGen(left);
			output->CodeWriteFormat("\t%s\t", this->op_3ac_name.c_str());
			output->CodeGen(right);
			output->CodeWrite("\n");
		}
		break;
	case OP_BREAK: // The OP_GOTO synonym
		{
			// TODO: make switch|loop reaction here
		}
		break;
	case OP_CONTINUE: // The OP_GOTO synonym
		{
			//output->
		}
		break;
	default:
		fprintf(stderr, "error: invalid 3ac operator code");
		break;
	}

	return 0;
}
int OperatorAstNode::PrintASTree(AstPrintInfo* output)
{
	//printing the type
	output->AstWriteFormat("%s oper [%s]\n", this->GetResultType()->GetName(), this->GetOpName());
	output->Print(left);
	if (right != nullptr)
		output->Print(right);
	return 0;
}
int OperatorAstNode::Serialize(TMLWriter *output)
{
	if (left != nullptr) // Left operand is allowed to be null (means stuff is already loaded into accumulator)
	{
		switch (operation)
		{
		case OP_GOTO:
		case OP_BREAK: // The OP_GOTO synonym
		case OP_CONTINUE: // The OP_GOTO synonym
			break;
		default:
			output->WriteTypedInstruction(LD_, left);			// LD left
		}
	}
	return SerializeProcessor(output);
}
int OperatorAstNode::SerializeProcessor(TMLWriter* output)
{
	opEnum op = this->operation;
	switch(op)
	{
	// Binary arithmetics
	case OP_PLUS:
		output->WriteTypedInstruction(ADD_, right); // ADD A
		break;
	case OP_MINUS:
		output->WriteTypedInstruction(SUB_, right); // SUB A
		break;
	case OP_MULT:
		output->WriteTypedInstruction(MUL_, right); // MUL A
		break;
	case OP_DIV:
		output->WriteTypedInstruction(DIV_, right); // DIV A
		break;

	// Unary arithmetics
	case OP_UMIN:
		output->WriteTypedInstruction(NEG_, left);	// NEG A
		break;

	// Unary logical
	case OP_NOT:
		{
			TLabel *L1, *L2;
			BaseTypeInfo *type;
			switch(right->GetResultType()->getID())
			{
			case INT_TYPE:
				type = new IntType();
				break;
			case FLOAT_TYPE:
				type = new FloatType();
				break;
			}

			NumValueAstNode trueNode(TML_TRUE, type), 
							falseNode(TML_FALSE, type);

			L1 = output->GetContext()->GenerateNewLabel();
			L2 = output->GetContext()->GenerateNewLabel();

			output->WriteTypedInstruction(TST_, left);			// TST A
			output->WriteJumpInstruction(JMZ, L1);				// JMZ L1
			output->WriteTypedInstruction(LD_, &falseNode);		// LD "1"
			output->WriteJumpInstruction(JMP, L2);				// JMP L2
			output->BindLabelToNextWrittenInstruction(L1);		// L1:
			output->WriteTypedInstruction(LD_, &trueNode);		// LD "0"
			output->BindLabelToNextWrittenInstruction(L2);		// L2:
		}
		break;

	// Binary logical
	case OP_AND:
	case OP_OR:
		{
			TLabel *L1, *L2;
			BaseTypeInfo *type;
			switch(right->GetResultType()->getID())
			{
			case INT_TYPE:
				type = new IntType();
				break;
			case FLOAT_TYPE:
				type = new FloatType();
				break;
			}

			NumValueAstNode trueNode(TML_TRUE, type), 
							falseNode(TML_FALSE, type);

			L1 = output->GetContext()->GenerateNewLabel();
			L2 = output->GetContext()->GenerateNewLabel();

			output->WriteTypedInstruction(TST_, left);			// TST A
			output->WriteJumpInstruction((op == OP_AND ? JMZ : JMNZ), L1); // JM[N]Z L1
			output->WriteTypedInstruction(LD_, right);			// LD right
			output->WriteTypedInstruction(TST_, right);			// TST A
			output->WriteJumpInstruction((op == OP_AND ? JMZ : JMNZ), L1); // JM[N]Z L1
			output->WriteTypedInstruction(LD_, &trueNode);		// LD "1"
			output->WriteJumpInstruction(JMP, L2);				// JMP L2
			output->BindLabelToNextWrittenInstruction(L1);		// L1:
			output->WriteTypedInstruction(LD_, &falseNode);		// LD "0"
			output->BindLabelToNextWrittenInstruction(L2);		// L2:
		}
		break;
	case OP_XOR:
		{
			// Проинициализируем временную переменную
			TVariable *tempVar = output->GetContext()->GenerateNewTmpVar(left->GetResultType());
			VarAstNode tempVarNode(true, tempVar);

			//**************************************//
			//		x xor y = (!x or !y)(x or y)	//
			//**************************************//

			// (!x or !y)
			OperatorAstNode NotX(OP_NOT, left);
			output->Serialize(&NotX);
			output->WriteInstruction(PUSH);

			OperatorAstNode NotY(OP_NOT, right);
			output->Serialize(&NotY);
			output->WriteTypedInstruction(ST_, &tempVarNode);	
			
			output->WriteInstruction(POP);
			OperatorAstNode XorY1(OP_OR, nullptr, &tempVarNode, tempVarNode.GetResultType());
			// NOTE: call SerializeProcessor instead of output->Serialize(&XorY1);
			// so that not to overwrite the accumulator value
			XorY1.SerializeProcessor(output); 
	
			output->WriteInstruction(PUSH);

			//(x or y)
			OperatorAstNode XorY2(OP_OR, left, right);
			output->Serialize(&XorY2);
			output->WriteTypedInstruction(ST_, &tempVarNode);

			//(!x or !y)(x or y)
			output->WriteInstruction(POP);
			OperatorAstNode EndingAnd(OP_AND, nullptr, &tempVarNode, tempVarNode.GetResultType());
			output->Serialize(&EndingAnd);
		}
		break;

	case OP_EQ:
	case OP_NOT_EQ:
	case OP_LARGER:
	case OP_SMALLER:
	case OP_LARGER_OR_EQ:
	case OP_SMALLER_OR_EQ:
		{
			TLabel *L1, *L2;

			BaseTypeInfo *type;
			switch(right->GetResultType()->getID())
			{
			case INT_TYPE:
				type = new IntType();
				break;
			case FLOAT_TYPE:
				type = new FloatType();
				break;
			}

			NumValueAstNode trueNode(TML_TRUE, type), 
							falseNode(TML_FALSE, type);

			L1 = output->GetContext()->GenerateNewLabel();
			L2 = output->GetContext()->GenerateNewLabel();

			// Запись инструкций в выходной файл и привязка меток:
			output->WriteTypedInstruction(LD_, left);
			output->WriteTypedInstruction(CMP_, right);
			switch(op)
			{
			case OP_EQ:
				output->WriteJumpInstruction(JMZ, L1);
				break;
			case OP_NOT_EQ:
				output->WriteJumpInstruction(JMNZ, L1);
				break;
			case OP_LARGER:
				output->WriteJumpInstruction(JMG, L1);
				break;
			case OP_SMALLER:
				output->WriteJumpInstruction(JML, L1);
				break;
			case OP_LARGER_OR_EQ:
				output->WriteJumpInstruction(JMG, L1);
				output->WriteJumpInstruction(JMZ, L1);
				break;
			case OP_SMALLER_OR_EQ:
				output->WriteJumpInstruction(JML, L1);
				output->WriteJumpInstruction(JMZ, L1);
				break;
			}
			output->WriteTypedInstruction(LD_, &falseNode);		// LD "0"
			output->WriteJumpInstruction(JMP, L2);
			output->BindLabelToNextWrittenInstruction(L1);
			output->WriteTypedInstruction(LD_, &trueNode);		// LD "1"
			output->BindLabelToNextWrittenInstruction(L2);
		}
		break;
	case OP_INPUT:
		{
			// Операция ввода
			output->WriteTypedInstruction(IN_, left);
			// Операция сохранения
			output->WriteTypedInstruction(ST_, left);
		}
		break;
	case OP_OUTPUT:
		{
			// Операция загрузки
			output->WriteTypedInstruction(LD_, left);
			// Операция вывода
			output->WriteTypedInstruction(OUT_, left);
		}
		break;
	case OP_SAVE:
		{
			output->WriteTypedInstruction(ST_, left);
		}
		break;
	case OP_LOAD:
		{
			output->WriteTypedInstruction(LD_, left);
		}
		break;
	case OP_IFTRUE:
	case OP_IFFALSE:
		{
			// Операция сравнения аккумулятора с нулем
			output->WriteTypedInstruction(TST_, left);
			switch(op)
			{
			case OP_IFFALSE:
				{
					// Операция условного перехода
					output->WriteJumpInstruction(JMZ, dynamic_cast<LabelAstNode*>(right)->GetLabel());
				}
				break;
			case OP_IFTRUE:
				{
					// Операция условного перехода
					output->WriteJumpInstruction(JMNZ, dynamic_cast<LabelAstNode*>(right)->GetLabel());
				}
				break;
			}
		}
		break;
	case OP_GOTO:
	case OP_BREAK: // The OP_GOTO synonym
	case OP_CONTINUE: // The OP_GOTO synonym
		{
			LabelAstNode *labelNode = dynamic_cast<LabelAstNode *>(left);
			if (NULL != labelNode)
			{
				output->WriteJumpInstruction(JMP, labelNode->GetLabel());
			}
			else
			{
				throw std::string("Error: GOTO node invalid!");
			}
		}
		break;
	}
}

int VarAstNode::Print3AC(TACWriter* output)
{
	TVariable *var = this->GetTableReference();
	output->CodeWriteFormat("\t$id%s", var->GetName().c_str());
	return 0;
}
int VarAstNode::PrintASTree(AstPrintInfo* output)
{
	output->AstWriteFormat("%s identifier %s\n", GetResultType()->GetName().c_str(), GetTableReference()->GetName().c_str());
	return 0;
}
int VarAstNode::Serialize(TMLWriter* _output)
{
	return 0;
}

int NumValueAstNode::Print3AC(TACWriter* output)
{
	output->CodeWriteFormat("\t$c%s", this->value.c_str());
	return 0;
}
int NumValueAstNode::PrintASTree(AstPrintInfo* output)
{
	output->AstWriteFormat("const %s %s\n", GetResultType()->GetName().c_str(), value.c_str());
	return 0;
}
int NumValueAstNode::Serialize(TMLWriter* _output)
{
	return 0;
}

int LoopAstNode::Print3AC(TACWriter* output)
{
	// decl
	if (this->cond->GetInit_expr() != nullptr)
	{
		// Generate initial expression code
		output->CodeGen(this->cond->GetInit_expr());
	}

	auto labelNumber_start = output->GetContext()->GenerateNewLabel();
	auto labelNumber_loop = output->GetContext()->GenerateNewLabel();
	auto labelNumber_end = output->GetContext()->GenerateNewLabel();
	
	// We'll write the post expression first (before the start)
	// because when using "continue" operator
	// this way generates less jumps
	if (this->cond->GetPost_expr() != nullptr)
	{
		// first time it passes straight to start
		// but as loop iterates it will return to labelNumber_loop, not labelNumber_start
		output->CodeWriteFormat("\tgoto\t%s\n", labelNumber_start->GetName());
		output->CodeWriteFormat("%s:\n", labelNumber_loop->GetName());
		output->CodeGen(this->cond->GetPost_expr());
		output->CodeWriteFormat("%s:\n", labelNumber_start->GetName());
	}
	else
	{
		output->CodeWriteFormat("%s:\n", labelNumber_loop->GetName());
	}
	
	// The continuation condition
	if (this->cond->GetB_expr() != nullptr && !this->post_check)
	{
		output->CodeGen(this->cond->GetB_expr());
		// Assuming last used TmpVar index contains the result
		output->CodeWriteFormat("\tiffalse\t$t%d\t%s\n", output->GetContext()->GetLastUsedTmpVarIndex(), labelNumber_end->GetName());
	}

	// The loop body
	if (this->block != nullptr)
	{
		//LoopCodeGenInfo LoopCGI(*output, labelNumber_loop, labelNumber_end);
		// TODO: Make Push to operator stack!
		//LoopCGI.CodeGen(this->block);
		TSimpleOperator op(OT_FOR, labelNumber_loop, labelNumber_end);
		output->GetContext()->OperatorStackPush(&op);
 		output->CodeGen(this->block);
		output->GetContext()->OperatorStackPop();
	}

	// The continuation condition post check (for do{}while() loops)
	if (this->cond->GetB_expr() != nullptr && this->post_check)
	{
		output->CodeGen(this->cond->GetB_expr());
		// Assuming last used TmpVar index contains the result
		output->CodeWriteFormat("\tiftrue\t$t%d\t%s\n", output->GetContext()->GetLastUsedTmpVarIndex(), labelNumber_loop->GetName());
	}
	else
	{
		output->CodeWriteFormat("\tgoto\t%s\n", labelNumber_loop->GetName());
	}
	output->CodeWriteFormat("%s:\n", labelNumber_end->GetName());
	return 0;
}
int LoopAstNode::PrintASTree(AstPrintInfo* output)
{
	output->AstWriteFormat("%sloop\n", (post_check ? "post-test " : ""));
	output->AstWriteLine("Conditions:");
	output->Print(cond);
	if (block != nullptr)
	{
		output->AstWriteLine("block:");
		output->Print(block);
	}
	return 0;
}
int LoopAstNode::Serialize(TMLWriter* output)
{
	return 0;
}

int LoopConditionAstNode::PrintASTree(AstPrintInfo* output)
{
	if (init_expr != nullptr)
	{
		output->AstWriteLine("initial:");
		output->Print(init_expr);
	}

	if (b_expr != nullptr)
	{
		output->AstWriteLine("condition:");
		output->Print(b_expr);
	}

	if (post_expr != nullptr)
	{
		output->AstWriteLine("post-iteration:");
		output->Print(post_expr);
	}

	return 0;
}

int DimensionAstNode::Print3AC(TACWriter* output)
{
	if (this->next_dimension != nullptr)
		output->CodeGen(this->next_dimension);

	// Проинициализируем временную переменную
	auto tempVarName = GenerateVariableName(std::string("$t"), output->GetContext()->GetNextTmpVarIndex());
	//VarAstNode tempVarNode(true, tempVar);
	//output->CodeGen(this->GetExpr());

	// NOTE: dimension values are generated into INT values!
	output->CodeWriteFormat("\tPush\t%d\n", tempVarName);

	return 0;
}
int DimensionAstNode::Serialize(TMLWriter* output)
{
	if (this->next_dimension != nullptr)
		output->Serialize(this->next_dimension);

	// TODO: serialize result to TmpVar and then push 
	output->Serialize(this->GetExpr());

	// NOTE: dimension values are generated into INT values!
	output->WriteInstruction(PUSH);

	return 0;
}

int ArrayAddressAstNode::Serialize(TMLWriter* output)
{
	// Push desired addresses for the dimensions
	output->Serialize(this->dimensions);

	auto *varType = dynamic_cast<ArrayType*>(var->GetTableReference()->GetType());
	auto it_end = varType->GetSizes().end();

	auto SumTmpVar = output->GetContext()->GenerateNewTmpVar(new IntType());
	VarAstNode SumTmpVarNode(true, SumTmpVar);

	for (auto it = varType->GetSizes().begin(); it != it_end; it++)
	{
		output->WriteInstruction(POP);
		NumValueAstNode DimSizeNode(*it);
		OperatorAstNode Mul(OP_MULT, nullptr, &DimSizeNode, DimSizeNode.GetResultType());
		output->Serialize(&Mul);
		
		OperatorAstNode Plus(OP_PLUS, nullptr, &SumTmpVarNode, SumTmpVarNode.GetResultType());
		output->Serialize(&Plus);
		output->WriteTypedInstruction(ST_, &SumTmpVarNode);
	}

	// SumTmpVarNode is the result var
	OperatorAstNode ArrayItemOffset(OP_PLUS, this->var, &SumTmpVarNode);
	output->WriteTypedInstruction(ST_, &SumTmpVarNode);
	
	return 0;
}

int StructAddressAstNode::Serialize(TMLWriter* output)
{
	NumValueAstNode VarOffsetNode(GetStruct()->GetMemoryOffset());
	std::string fieldName = GetField()->GetName();
	NumValueAstNode FieldOffsetNode((int)dynamic_cast<StructType*>(GetStruct()->GetType())->Offset(fieldName));

	// SumTmpVarNode is the result var
	OperatorAstNode ArrayItemOffset(OP_PLUS, &VarOffsetNode, &FieldOffsetNode);
	
	auto SumTmpVar = output->GetContext()->GenerateNewTmpVar(new IntType());
	VarAstNode SumTmpVarNode(true, SumTmpVar);
	output->WriteTypedInstruction(ST_, &SumTmpVarNode);
	
	return 0;
}
