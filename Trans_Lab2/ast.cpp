#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "AstUtils.h"
#include "tml-generator.h"
#include "Operators.h"

int ConditionalAstNode::Print3AC(TACWriter* output)
{
	auto labelNumber_false = output->GetContext()->GenerateNewLabel();
	auto labelNumber_end = labelNumber_false;
	if (this->false_block != nullptr)
		labelNumber_end = output->GetContext()->GenerateNewLabel();

	LabelAstNode endLabelAstNode(labelNumber_end);
	LabelAstNode falseLabelAstNode(labelNumber_false);

	// 1. check the result and process true_block (TODO [SV] 14.08.13 16:14: with possible optimization|elimination?)
	OperatorAstNode op(OP_IFFALSE, cond, &falseLabelAstNode);
	output->CodeGen(&op);

	if (this->true_block != nullptr)
	{
		output->CodeGen(true_block);
	}

	if (this->false_block != nullptr)
	{
		OperatorAstNode op(OP_GOTO, &endLabelAstNode);
		output->CodeGen(&op); // for the iftrue block to end
		
		output->CodeGen(&falseLabelAstNode);
		output->CodeGen(false_block);
	}

	output->CodeGen(&endLabelAstNode);
	return 0;
}
int ConditionalAstNode::PrintASTree(AstPrintInfo* output)
{
	output->AstWriteLine("if_stmnt");
	output->AstWriteLine("cond:");
	output->Print(cond);
	// TODO: think of optimisations here?
	if (this->true_block != nullptr) // might be null if there are no statements (or declarations only)
	{
		output->AstWriteLine("if_true:");
		output->Print(true_block);
	}
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

	LabelAstNode endLabelAstNode(labelNumber_end);
	LabelAstNode falseLabelAstNode(labelNumber_false);

	// 1. check the result and process true_block (TODO [SV] 14.08.13 16:14: with possible optimization|elimination?)
	OperatorAstNode op(OP_IFFALSE, cond, &falseLabelAstNode);
	output->Serialize(&op);
	if (this->true_block != nullptr) // might be null if there are no statements (or declarations only)
	{
		output->Serialize(true_block);
	}

	if (this->false_block != nullptr)
	{
		OperatorAstNode op_goto(OP_GOTO, &endLabelAstNode);
		output->Serialize(&op_goto);
		
		output->Serialize(&falseLabelAstNode);
		output->Serialize(false_block);
	}

	output->Serialize(&endLabelAstNode);
	return 0;
}
int ConditionalAstNode::Optimize(AstOptimizer* output)
{

}

int OperatorAstNode::Print3AC(TACWriter* output)
{
	switch (this->GetOpID())
	{
	/*case OP_ARRAY_ITEM:
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
		break;*/
	case OP_ASSIGN:
		{
			output->CodeGen(left);
			auto leftVarName = output->GetLastUsedValueName();
			
			output->CodeGen(right);
			auto rightVarName = output->GetLastUsedValueName();
			
			output->CodeWriteFormat("\t%s\t%s\t%s\n", 
				leftVarName.c_str(), 
				this->op_3ac_name.c_str(),
				rightVarName.c_str());

			output->SetLastUsedValueName(leftVarName);
		}
		break;
	case OP_UMIN:
	case OP_NOT:
		{
			if (left != nullptr)
			{
				output->CodeGen(left);
			}
			auto leftVarName = output->GetLastUsedValueName();
			
			auto resultVarName = dynamic_cast<IValueHolderNode*>(result)->GetValueHolderName();
			output->CodeWriteFormat("\t%s\t=\t%s\t%s\n",
				resultVarName.c_str(),
				this->op_3ac_name.c_str(),
				leftVarName.c_str());
			
			output->SetLastUsedValueName(resultVarName);
		}
		break;	
	case OP_PLUS:
	case OP_MINUS:
	case OP_MULT:
	case OP_DIV:
	case OP_OR:
	case OP_AND:
	case OP_XOR:
	case OP_EQ:
	case OP_NOT_EQ:
	case OP_LARGER:
	case OP_LARGER_OR_EQ:
	case OP_SMALLER:
	case OP_SMALLER_OR_EQ:
		{
			if (left != nullptr)
			{
				output->CodeGen(left);
			}
			auto leftVarName = output->GetLastUsedValueName();
			
			output->CodeGen(right);
			auto rightVarName = output->GetLastUsedValueName();
			
			auto resultVarName = dynamic_cast<IValueHolderNode*>(result)->GetValueHolderName();
			output->CodeWriteFormat("\t%s\t=\t%s\t%s\t%s\n", 
				resultVarName.c_str(), 
				leftVarName.c_str(), 
				this->op_3ac_name.c_str(),
				rightVarName.c_str());
			
			output->SetLastUsedValueName(resultVarName);
		}
		break;
	case OP_GOTO:
	case OP_BREAK: // The OP_GOTO synonym
	case OP_CONTINUE: // The OP_GOTO synonym
		{
			LabelAstNode *labelNode = dynamic_cast<LabelAstNode *>(left);
			if (nullptr != labelNode)
			{
				auto labelName = labelNode->GetLabel()->GetName();
				output->CodeWriteFormat("\tGoto\t%s\n", labelName.c_str());
			}
			else
			{
				throw std::string("Error: GOTO node invalid!\n");
			}
		}
		break;
	case OP_INPUT:
		{
			if (left != nullptr)
			{
				output->CodeGen(left);
			}
			auto leftVarName = output->GetLastUsedValueName();
			output->CodeWriteFormat("\tSCAN\t%s\n", leftVarName.c_str());
		}
		break;
	case OP_OUTPUT:
		{
			if (left != nullptr)
			{
				output->CodeGen(left);
			}
			auto leftVarName = output->GetLastUsedValueName();
			output->CodeWriteFormat("\tPRINT\t%s\n", leftVarName.c_str());
		}
		break;
	case OP_IFTRUE:
	case OP_IFFALSE:
		{
			output->CodeGen(left);
			auto resultVarName = output->GetLastUsedValueName();
			auto labelName = dynamic_cast<LabelAstNode*>(right)->GetLabel()->GetName();

			switch(this->GetOpID())
			{
			case OP_IFFALSE:
				output->CodeWriteFormat("\tiffalse\t%s\t%s\n", resultVarName.c_str(), labelName.c_str());
				break;
			case OP_IFTRUE:
				output->CodeWriteFormat("\tiftrue\t%s\t%s\n", resultVarName.c_str(), labelName.c_str());
				break;
			}
		}
		break;
	case OP_CASE:
	case OP_LIST: // case list
	case OP_DEFAULT:
		{
			output->CodeGen(left);
			output->CodeGen(right);	
		}
		break;
	default:
		fprintf(stderr, "error: invalid 3ac operator code in %s\n", __FUNCTION__);
		break;
	}

	return 0;
}
int OperatorAstNode::PrintASTree(AstPrintInfo* output)
{
	//printing the type
	// NOTE: gotta make it to separate std::string local var! 
	std::string typeName = this->GetResultType()->GetName(); 
	output->AstWriteFormat("%s oper [%s]\n", typeName.c_str(), this->GetOpName());
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
		case OP_PLUS:
		case OP_MINUS:
		case OP_MULT:
		case OP_DIV:
		case OP_UMIN:
		case OP_NOT:
		case OP_AND:
		case OP_OR:
		case OP_XOR:
		case OP_EQ:
		case OP_NOT_EQ:
		case OP_LARGER:
		case OP_SMALLER:
		case OP_LARGER_OR_EQ:
		case OP_SMALLER_OR_EQ:
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

			NumValueAstNode trueNode(1), falseNode(0);

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

			NumValueAstNode trueNode(1), falseNode(0);

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
			output->Serialize(&NotX); // TODO: make OperatorAstNode ignore option for result var ST_ instruction 
			output->WriteInstruction(PUSH); 

			OperatorAstNode NotY(OP_NOT, right, nullptr, &tempVarNode);
			output->Serialize(&NotY);
			
			output->WriteInstruction(POP);
			OperatorAstNode XorY1(OP_OR, nullptr, &tempVarNode, &tempVarNode);
			output->Serialize(&XorY1); 
	
			output->WriteInstruction(PUSH);

			//(x or y)
			OperatorAstNode XorY2(OP_OR, left, right, &tempVarNode);
			output->Serialize(&XorY2);

			//(!x or !y)(x or y)
			output->WriteInstruction(POP);
			OperatorAstNode EndingAnd(OP_AND, nullptr, &tempVarNode, &tempVarNode);
			output->Serialize(&EndingAnd);
		}
		break;
	case OP_ASSIGN:
		{
			output->Serialize(right);
			output->WriteTypedInstruction(LD_, output->GetLastOperationResult());
			output->WriteTypedInstruction(ST_, left);
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

			NumValueAstNode trueNode(1), falseNode(0);

			L1 = output->GetContext()->GenerateNewLabel();
			L2 = output->GetContext()->GenerateNewLabel();

			// Запись инструкций в выходной файл и привязка меток:
			//output->WriteTypedInstruction(LD_, left);
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
			// Input (variable only!)
			output->WriteTypedInstruction(IN_, left); 
			output->WriteTypedInstruction(ST_, left);
		}
		break;
	case OP_OUTPUT:
		{
			//NOTE: output's left can be an expression, var or num value
			//output->WriteTypedInstruction(LD_, left);
			output->WriteTypedInstruction(OUT_, left);
		}
		break;
	case OP_SAVE:
		{
			// Input (variable only!)
			output->WriteTypedInstruction(ST_, left);
		}
		break;
	case OP_LOAD:
		{
			// Input (variable only!)
			output->WriteTypedInstruction(LD_, left);
		}
		break;
	case OP_IFTRUE:
	case OP_IFFALSE:
		{
			output->Serialize(left);

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
			if (nullptr != labelNode)
			{
				output->WriteJumpInstruction(JMP, labelNode->GetLabel());
			}
			else
			{
				throw std::string("Error: GOTO node invalid!");
			}
		}
		break;
	case OP_CALL:
		{
			LabelAstNode *labelNode = dynamic_cast<LabelAstNode *>(left);
			if (nullptr != labelNode)
			{
				output->WriteJumpInstruction(CALL, labelNode->GetLabel());
			}
			else
			{
				throw std::string("Error: GOTO node invalid!");
			}
		}
		break;
	case OP_RETURN:
		output->WriteInstruction(RET);
		break;
	case OP_CASE:
	case OP_LIST:
	case OP_DEFAULT:
		{
			output->Serialize(left);
			output->Serialize(right);	
		}
		break;
	}

	if (result != nullptr) // NOTE: for arithmetic and logical operations only! 
	{
		output->WriteTypedInstruction(ST_, result);
		output->SetResult(result);
	}

	return 0;
}

int VarAstNode::Print3AC(TACWriter* output)
{
	//auto varName = this->GetTableReference()->GetName();
	//output->CodeWriteFormat("\t$id%s", varName.c_str());
	output->SetLastUsedValueName(this->GetValueHolderName());
	return 0;
}
int VarAstNode::PrintASTree(AstPrintInfo* output)
{
	auto typeName = GetResultType()->GetName();
	auto varName = GetTableReference()->GetName();
	output->AstWriteFormat("%s identifier %s\n", typeName.c_str(), varName.c_str());
	return 0;
}
int VarAstNode::Serialize(TMLWriter* output)
{
	if (this->GetTableReference()->IsInitialized() && !this->isAllocated)
	{
		output->InitVariableData(this);
		this->isAllocated = true;
	}

	output->SetResult(this);
	return 0;
}

int NumValueAstNode::Print3AC(TACWriter* output)
{
	//output->CodeWriteFormat("\t$c%s", this->value.c_str());
	output->SetLastUsedValueName(std::string("$c")+this->value);
	return 0;
}
int NumValueAstNode::PrintASTree(AstPrintInfo* output)
{
	auto typeName = GetResultType()->GetName();
	output->AstWriteFormat("const %s %s\n", typeName.c_str(), value.c_str());
	return 0;
}
int NumValueAstNode::Serialize(TMLWriter* output)
{
	if (this->GetResultType()->getID() == LITERAL_TYPE)
	{
		auto strVar = new VarAstNode(true, output->GetContext()->GenerateNewTmpVar(this->GetResultType()->Clone(), false));
		strVar->GetTableReference()->SetValue(this->ToString());
		output->Serialize(strVar);
	}
	else
		output->SetResult(this);
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

	LabelAstNode labelNumber_start(output->GetContext()->GenerateNewLabel());
	LabelAstNode labelNumber_loop(this->entranceLabel);
	LabelAstNode labelNumber_end(this->outLabel);
	
	// We'll write the post expression first (before the start)
	// because when using "continue" operator
	// this way generates less jumps
	if (this->cond->GetPost_expr() != nullptr)
	{
		// first time it passes straight to start
		// but as loop iterates it will return to labelNumber_loop, not labelNumber_start
		OperatorAstNode opGoto(OP_GOTO, &labelNumber_start);
		output->CodeGen(&opGoto);

		output->CodeGen(&labelNumber_loop);
		output->CodeGen(this->cond->GetPost_expr());
		output->CodeGen(&labelNumber_start);
	}
	else
	{
		output->CodeGen(&labelNumber_loop);
	}
	
	// The continuation condition
	if (this->cond->GetB_expr() != nullptr && !this->post_check)
	{
		OperatorAstNode op(OP_IFFALSE, this->cond->GetB_expr(), &labelNumber_end);
		output->CodeGen(&op);
	}

	// The loop body
	if (this->block != nullptr)
	{
		output->CodeGen(this->block);
	}

	// The continuation condition post check (for do{}while() loops)
	if (this->cond->GetB_expr() != nullptr)
	{
		if (this->post_check)
		{
			OperatorAstNode op(OP_IFTRUE, this->cond->GetB_expr(), &labelNumber_loop);
			output->CodeGen(&op);
		}
		else
		{
			OperatorAstNode op(OP_GOTO,  &labelNumber_loop);
			output->CodeGen(&op);
		}
	}
	output->CodeGen(&labelNumber_end);
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
	// decl
	if (this->cond->GetInit_expr() != nullptr)
	{
		// Generate initial expression code
		output->Serialize(this->cond->GetInit_expr());
	}

	LabelAstNode labelNumber_start(output->GetContext()->GenerateNewLabel());
	LabelAstNode labelNumber_loop(this->entranceLabel);
	LabelAstNode labelNumber_end(this->outLabel);

	// We'll write the post expression first (before the start)
	// because when using "continue" operator
	// this way generates less jumps
	if (this->cond->GetPost_expr() != nullptr)
	{
		// first time it passes straight to start
		// but as loop iterates it will return to labelNumber_loop, not labelNumber_start
		OperatorAstNode opGoto(OP_GOTO, &labelNumber_start);
		output->Serialize(&opGoto);

		output->Serialize(&labelNumber_loop);
		output->Serialize(this->cond->GetPost_expr());
		output->Serialize(&labelNumber_start);
	}
	else
	{
		output->Serialize(&labelNumber_loop);
	}

	// The continuation condition
	if (this->cond->GetB_expr() != nullptr && !this->post_check)
	{
		OperatorAstNode op(OP_IFFALSE, this->cond->GetB_expr(), &labelNumber_end);
		output->Serialize(&op);
	}

	// The loop body
	if (this->block != nullptr)
	{
		output->Serialize(this->block);
	}

	// The continuation condition post check (for do{}while() loops)
	if (this->cond->GetB_expr() != nullptr)
	{
		if (this->post_check)
		{
			OperatorAstNode op(OP_IFTRUE, this->cond->GetB_expr(), &labelNumber_loop);
			output->Serialize(&op);
		}
		else
		{
			OperatorAstNode op(OP_GOTO, &labelNumber_loop);
			output->Serialize(&op);
		}
	}
	output->Serialize(&labelNumber_end);
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

int DimensionAstNode::PrintASTree(AstPrintInfo* output)
{
	output->AstWriteFormat("[\n");
	output->Print(this->GetExpr());
	output->AstWriteFormat("]\n");

	if (this->next_dimension->GetExpr() != nullptr) // means next dimension node is the terminating one
		output->Print(this->next_dimension);
	return 0;
}

int DimensionAstNode::Print3AC(TACWriter* output)
{
	if (this->next_dimension->GetExpr() != nullptr) // means next dimension node is the terminating one
		output->CodeGen(this->next_dimension);

	output->CodeGen(this->GetExpr()); // Might be either 
	auto valName = output->GetLastUsedValueName();
	// NOTE: dimension values are generated into INT values!
	output->CodeWriteFormat("\tPush\t%s\n", valName.c_str());
	// Проинициализируем временную переменную

	return 0;
}
int DimensionAstNode::Serialize(TMLWriter* output)
{
	if (this->next_dimension->GetExpr() != nullptr) // means next dimension node is the terminating one
		output->Serialize(this->next_dimension);

	// TODO: serialize result to TmpVar and then push 
	output->WriteTypedInstruction(LD_, this->GetExpr());	
	// NOTE: dimension values are generated into INT values!
	output->WriteInstruction(PUSH);

	return 0;
}

int ArrayAddressAstNode::Print3AC(TACWriter* output)
{
	// Push desired addresses for the dimensions
	output->CodeGen(this->dimensions);

	auto *varType = dynamic_cast<ArrayType*>(varNode->GetTableReference()->GetType());
	auto sizes = varType->GetSizes();

	auto SumTmpVar = output->GetContext()->GenerateNewTmpVar(new IntType(), true);
	VarAstNode SumTmpVarNode(true, SumTmpVar);
	
	auto MulTmpVar = output->GetContext()->GenerateNewTmpVar(new IntType(), true);
	VarAstNode MulTmpVarNode(true, MulTmpVar);

	//Адрес(table2[i,j]) =
	//	Адрес(table2[low(1),low(1)]) +
	//	(i) * (upp(2) + 1) +
	//	(j)

	// Set initial values
	NumValueAstNode numZero(0);
	OperatorAstNode nullOp(OP_ASSIGN, &SumTmpVarNode, &numZero);
	output->CodeGen(&nullOp);

	auto loop_end = sizes.begin();
	for (auto it = --sizes.end(); it != loop_end; it--)
	{
		output->CodeGen(&MulTmpVarNode);
		auto valName = output->GetLastUsedValueName();
		// NOTE: dimension values are generated into INT values!
		output->CodeWriteFormat("\tPop\t%s\n", valName.c_str());

		NumValueAstNode DimSizeNode(*it);
		OperatorAstNode Mul(OP_MULT, &MulTmpVarNode, &DimSizeNode, &MulTmpVarNode);
		output->CodeGen(&Mul);
		
		OperatorAstNode Plus(OP_PLUS, &SumTmpVarNode, &MulTmpVarNode, &SumTmpVarNode);
		output->CodeGen(&Plus);
	}

	// add last dimension address with no multiplication!
	output->CodeGen(&MulTmpVarNode);
	auto valName = output->GetLastUsedValueName();
	output->CodeWriteFormat("\tPop\t%s\n", valName.c_str());
	OperatorAstNode Plus(OP_PLUS, &MulTmpVarNode, &SumTmpVarNode, &SumTmpVarNode);
	output->CodeGen(&Plus);

	// SumTmpVarNode is the result var
	OperatorAstNode ArrayItemOffset(OP_PLUS, &SumTmpVarNode, this->varNode, &SumTmpVarNode);
	output->CodeGen(&ArrayItemOffset);
	output->SetLastUsedValueName(SumTmpVarNode.GetValueHolderName());
	
	return 0;
}

int ArrayAddressAstNode::PrintASTree(AstPrintInfo* output)
{
	auto varName = this->varNode->GetTableReference()->GetName();
	output->AstWriteFormat("%s\n", varName.c_str());
	output->Print(this->dimensions);
	output->AstWriteLine("");
	return 0;
}

int ArrayAddressAstNode::Serialize(TMLWriter* output)
{
	// Push desired addresses for the dimensions
	output->Serialize(this->dimensions);

	if (this->ArrayOffsetVarNode == nullptr)
	{
		auto SumTmpVar = output->GetContext()->GenerateNewTmpVar(new IntType());
		ArrayOffsetVarNode = new VarAstNode(true, SumTmpVar);
	}
	if (this->MulTmpVarNode == nullptr)
	{
		auto MulTmpVar = output->GetContext()->GenerateNewTmpVar(new IntType());
		this->MulTmpVarNode = new VarAstNode(true, MulTmpVar);
	}

	// Set initial values
	NumValueAstNode numZero(0);
	OperatorAstNode nullOp1(OP_ASSIGN, ArrayOffsetVarNode, &numZero);
	output->Serialize(&nullOp1);

	auto *varType = dynamic_cast<ArrayType*>(varNode->GetTableReference()->GetType());
	auto sizes = varType->GetSizes();

	auto loop_end = sizes.begin();
	for (auto it = --sizes.end(); it != loop_end; it--)
	{
		// A = POP // current dimension address
		// A = A * size
		// MulTmpVar = A
		output->WriteInstruction(POP);
		NumValueAstNode DimSizeNode(*it);
		OperatorAstNode Mul(OP_MULT, nullptr, &DimSizeNode, MulTmpVarNode);
		output->Serialize(&Mul);

		// A = SumTmpVar + MulTmpVar
		// SumTmpVar = A
		OperatorAstNode Plus(OP_PLUS, ArrayOffsetVarNode, MulTmpVarNode, ArrayOffsetVarNode);
		output->Serialize(&Plus);
	}

	// add last dimension address with no multiplication!
	output->WriteInstruction(POP);
	OperatorAstNode Plus(OP_PLUS, nullptr, ArrayOffsetVarNode, ArrayOffsetVarNode);
	output->Serialize(&Plus);

	NumValueAstNode varOffset(this->varNode->GetTableReference()->GetMemoryOffset());

	// SumTmpVarNode is the result var
	OperatorAstNode ArrayItemOffset(OP_PLUS, ArrayOffsetVarNode, &varOffset, ArrayOffsetVarNode);
	output->Serialize(&ArrayItemOffset);
	output->SetResult(ArrayOffsetVarNode);

	return 0;
}

int StructAddressAstNode::Print3AC(TACWriter* output)
{
	output->SetLastUsedValueName(this->GetValueHolderName());
	return 0;
}
int StructAddressAstNode::PrintASTree(AstPrintInfo* output)
{
	auto typeName = GetResultType()->GetName();
	auto structName = GetStruct()->GetName();
	auto fieldName = GetField()->GetName();
	output->AstWriteFormat("%s identifier %s.%s\n", typeName.c_str(), structName.c_str(), fieldName.c_str());
	return 0;
}
int StructAddressAstNode::Serialize(TMLWriter* output)
{
	NumValueAstNode VarOffsetNode(GetStruct()->GetMemoryOffset());
	std::string fieldName = GetField()->GetName();
	NumValueAstNode FieldOffsetNode((int)dynamic_cast<StructType*>(GetStruct()->GetType())->Offset(fieldName));

	if (SumTmpVarNode == nullptr)
	{
		auto SumTmpVar = output->GetContext()->GenerateNewTmpVar(new IntType());
		SumTmpVarNode = new VarAstNode(true, SumTmpVar);
	}
	// SumTmpVarNode is the result var
	OperatorAstNode ArrayItemOffset(OP_PLUS, &VarOffsetNode, &FieldOffsetNode, SumTmpVarNode);
	output->Serialize(&ArrayItemOffset);

	output->SetResult(SumTmpVarNode);
	return 0;
}

int LabelAstNode::Print3AC(TACWriter* output) 
{
	auto labelName = GetLabel()->GetName();
	output->CodeWriteFormat("%s:\n", labelName.c_str()); 
	return 0;
}
int LabelAstNode::Serialize(TMLWriter* output) 
{ 
	output->BindLabelToNextWrittenInstruction(this->GetLabel()); 
	return 0;
}

int SwitchAstNode::Print3AC(TACWriter* output)
{
	this->opData->ProcessCaseList(
		[this, output](TCaseOperator *caseOp) -> bool 
	{
		VarAstNode tmpVar(true, output->GetContext()->GenerateNewTmpVar(this->key->GetResultType()->Clone(), true));
		LabelAstNode caseLabel(caseOp->GetLabel());
		OperatorAstNode opEq(OP_EQ, this->key, caseOp->GetKey(), &tmpVar);
		OperatorAstNode opJmp(OP_IFTRUE, &tmpVar, &caseLabel);
		output->CodeGen(&opEq);
		output->CodeGen(&opJmp);
		return true;
	}
	);

	if (this->case_default != nullptr)
	{
		LabelAstNode caseLabel(this->opData->GetDefaultOp()->GetLabel());
		OperatorAstNode opJmp(OP_GOTO, &caseLabel);
		output->CodeGen(&opJmp);
	}

	output->CodeGen(this->case_list);
	
	if (this->case_default != nullptr)
	{
		output->CodeGen(this->case_default);
	}

	LabelAstNode caseEndLabel(this->opData->GetEndLabel());
	output->CodeGen(&caseEndLabel);

	return 0;
}
int SwitchAstNode::PrintASTree(AstPrintInfo* output)
{
	return 0;
}
int SwitchAstNode::Serialize(TMLWriter* output)
{
	this->opData->ProcessCaseList(
		[this, output](TCaseOperator *caseOp) -> bool 
	{
		VarAstNode tmpVar(true, output->GetContext()->GenerateNewTmpVar(this->key->GetResultType()->Clone()));
		LabelAstNode caseLabel(caseOp->GetLabel());
		OperatorAstNode opEq(OP_EQ, this->key, caseOp->GetKey(), &tmpVar);
		OperatorAstNode opJmp(OP_IFTRUE, &tmpVar, &caseLabel);
		output->Serialize(&opEq);
		output->Serialize(&opJmp);
		return true;
	}
	);

	if (this->case_default != nullptr)
	{
		LabelAstNode caseLabel(this->opData->GetDefaultOp()->GetLabel());
		OperatorAstNode opJmp(OP_GOTO, &caseLabel);
		output->Serialize(&opJmp);
	}

	output->Serialize(this->case_list);

	if (this->case_default != nullptr)
	{
		output->Serialize(this->case_default);
	}

	LabelAstNode caseEndLabel(this->opData->GetEndLabel());
	output->Serialize(&caseEndLabel);

	return 0;
}

int FunctionAstNode::Print3AC(TACWriter* output)
{
	auto exitLabel = output->GetContext()->GenerateNewLabel();
	
	LabelAstNode startLabelNode(this->functionData->GetStart());
	output->CodeGen(&startLabelNode);

	auto parametersList = functionData->GetParametersList();
	if (parametersList.size() > 0)
	{
		// NOTE: last pushed var (parametersList[0]) might be the result var
		VarAstNode varNode(false, parametersList[0]);
		output->CodeGen(&varNode);
		auto varName = output->GetLastUsedValueName();
		output->CodeWriteFormat("\tExch\t%s\n", varName.c_str());
		
		int i = 1;
		for (auto it = ++parametersList.begin(); it != parametersList.end(); i++, it++)
		{
			VarAstNode varNode(false, (*it));
			output->CodeGen(&varNode);
			auto varName = output->GetLastUsedValueName();
			if (i != 1)
				output->CodeWriteFormat("\tExch\t%d\n", i-1);
			output->CodeWriteFormat("\tExch\t%d\n", i);
			output->CodeWriteFormat("\tExch\t%s\n", varName.c_str());
		}
		if (i != 1)
			output->CodeWriteFormat("\tExch\t%d\n", i);
	}
	
	// Get all variables in the function block (and sub-blocks)
	std::vector<TVariable*> blockVars;
	output->GetContext()->ProcessFunctionBlockVariables(this->functionData,
		[&blockVars](TVariable* var) -> bool
		{
			blockVars.emplace_back(var);
			return true;
		}
	);

	for (auto it = blockVars.begin(); it != blockVars.end(); it++)
	{
		auto blockVarName = (*it)->GetName();
		output->CodeWriteFormat("\tPush\t%s\n", blockVarName.c_str());
	}

	output->CodeGen(this->statementsBlock);

	LabelAstNode exitLabelNode(exitLabel);
	output->CodeGen(&exitLabelNode);

	for (auto it = --blockVars.end(); ; it--)
	{
		auto blockVarName = (*it)->GetName();
		output->CodeWriteFormat("\tPop\t%s\n", blockVarName.c_str());
		if (it == blockVars.begin())
			break;
	}

	// Now we gotta reverse iterate vector<TVariable*> for
	// the cosequent pop's
	if (parametersList.size() > 0)
	{
		int i = 1;
		for (auto it = --parametersList.end(); it != parametersList.begin(); it--)
		{
			VarAstNode varNode(false, (*it));
			output->CodeGen(&varNode);
			auto varName = output->GetLastUsedValueName();
			output->CodeWriteFormat("\tPop\t%s\n", varName.c_str());
		}

		VarAstNode varNode(false, (parametersList[0]));
		output->CodeGen(&varNode);
		auto varName = output->GetLastUsedValueName();	
		if (GetResultType()->getID() != VOID_TYPE)
		{
			output->CodeWriteFormat("\tExch\t%s\n", varName.c_str());
		}
		else
		{
			output->CodeWriteFormat("\tPop\t%s\n", varName.c_str());
		}
	}
	output->CodeWriteFormat("\tReturn\n\n"); // NOTE: double '\n' !
	return 0;
}
int FunctionAstNode::PrintASTree(AstPrintInfo* output)
{
	auto typeName = functionData->GetResultType()->GetName();
	auto funcName = functionData->GetName();
	output->AstWriteFormat("function: %s %s(", typeName.c_str(), funcName.c_str());

	auto parametersList = functionData->GetParametersList();
	auto it_end = --parametersList.end();
	for (auto it = parametersList.begin(); it != it_end; it++)
	{
		output->AstWriteFormat("%s, ", (*it)->GetName());
	}
	output->AstWriteFormat("%s)\n", (*it_end)->GetName());

	return 0;
}
int FunctionAstNode::Serialize(TMLWriter* output)
{
	auto exitLabel = output->GetContext()->GenerateNewLabel();

	LabelAstNode startLabelNode(this->functionData->GetStart());
	output->Serialize(&startLabelNode);

	auto parametersList = functionData->GetParametersList();
	if (parametersList.size() > 0)
	{
		// NOTE: last pushed var (parametersList[0]) might be the result var
		VarAstNode varNode(false, parametersList[0]);
		output->WriteTypedInstruction(EXCH, &varNode);

		int i = 1;
		for (auto it = ++parametersList.begin(); it != parametersList.end(); i++, it++)
		{
			VarAstNode varNode(false, (*it));
			NumValueAstNode exchPrev(i-1), exchNext(i);

			if (i != 1)
				output->WriteTypedInstruction(EXCH, &exchPrev);
			output->WriteTypedInstruction(EXCH, &exchNext);
			output->WriteTypedInstruction(EXCH, &varNode);
		}
		if (i != 1)
		{
			NumValueAstNode exchNext(i);
			output->WriteTypedInstruction(EXCH, &exchNext);
		}
	}

	// Get all variables in the function block (and sub-blocks)
	std::vector<TVariable*> blockVars;
	output->GetContext()->ProcessFunctionBlockVariables(this->functionData,
		[&blockVars](TVariable* var) -> bool
	{
		blockVars.emplace_back(var);
		return true;
	}
	);

	for (auto it = blockVars.begin(); it != blockVars.end(); it++)
	{
		VarAstNode varNode(false, (*it));
		output->WriteTypedInstruction(PUSH, &varNode);
	}

	output->Serialize(this->statementsBlock);

	LabelAstNode exitLabelNode(exitLabel);
	output->Serialize(&exitLabelNode);

	for (auto it = --blockVars.end(); ; it--)
	{
		VarAstNode varNode(false, (*it));
		output->WriteTypedInstruction(POP, &varNode);

		if (it == blockVars.begin())
			break;
	}

	// Now we gotta reverse iterate vector<TVariable*> for
	// the consequent pop's
	if (parametersList.size() > 0)
	{
		int i = 1;
		for (auto it = --parametersList.end(); it != parametersList.begin(); it--)
		{
			VarAstNode varNode(false, (*it));
			output->WriteTypedInstruction(POP, &varNode);
		}

		VarAstNode varNode(false, (*(parametersList.begin())));

		if (GetResultType()->getID() != VOID_TYPE)
		{
			output->WriteTypedInstruction(EXCH, &varNode);
		}
		else
		{
			output->WriteTypedInstruction(POP, &varNode);
		}
	}

	// NOTE: OP_RETURN is just a TML command here
	OperatorAstNode opRet(OP_RETURN, nullptr);
	output->Serialize(&opRet);

	return 0;
}

int FunctionCallAstNode::Print3AC(TACWriter* output)
{	
	for (auto it = this->parametersList.begin(); it != this->parametersList.end(); it++)
	{
		output->CodeGen((*it));
		auto valName = output->GetLastUsedValueName();
		output->CodeWriteFormat("\tPush\t%s\n", valName.c_str());
	}

	TVariable *resultVar = output->GetContext()->GenerateNewTmpVar(GetResultType()->Clone(), true);
	VarAstNode resultVarNode(true, resultVar);
		
	if (GetResultType()->getID() != VOID_TYPE)
	{
		output->CodeGen(&resultVarNode);
		auto valName = output->GetLastUsedValueName();
		output->CodeWriteFormat("\tPush\t%s\n", valName.c_str());
	}

	auto funcName = functionData->GetName();
	output->CodeWriteFormat("\tCall %s\n", funcName.c_str());
	
	if (GetResultType()->getID() != VOID_TYPE)
	{
		output->CodeGen(&resultVarNode);
		auto valName = output->GetLastUsedValueName();
		output->CodeWriteFormat("\tPop\t%s\n", valName.c_str());
	}
	output->SetLastUsedValueName(resultVarNode.GetValueHolderName());
	
	return 0;
}
int FunctionCallAstNode::PrintASTree(AstPrintInfo* output)
{
	auto funcTypeName = functionData->GetResultType()->GetName();
	auto funcName = functionData->GetName();
	output->AstWriteFormat("call: %s %s(", funcTypeName.c_str(), funcName.c_str());

	auto parametersList = functionData->GetParametersList();
	auto it_end = --parametersList.end();
	for (auto it = parametersList.begin(); it != it_end; it++)
	{
		auto varTypeName = (*it)->GetType()->GetName();
		auto varName = (*it)->GetName();
		output->AstWriteFormat("%s %s, ", varTypeName.c_str(), varName.c_str());
	}

	auto varTypeName = (*it_end)->GetType()->GetName();
	auto varName = (*it_end)->GetName();
	output->AstWriteFormat("%s %s)\n", varTypeName.c_str(), varName.c_str());

	return 0;
}
int FunctionCallAstNode::Serialize(TMLWriter* output)
{
	for (auto it = this->parametersList.begin(); it != this->parametersList.end(); it++)
	{
		output->Serialize((*it));
		auto valNode = output->GetLastOperationResult();
		output->WriteTypedInstruction(LD_, valNode);
		output->WriteInstruction(PUSH);
	}

	TVariable *resultVar = output->GetContext()->GenerateNewTmpVar(GetResultType()->Clone(), true);
	auto resultVarNode = new VarAstNode(true, resultVar);

	if (GetResultType()->getID() != VOID_TYPE)
	{
		output->WriteTypedInstruction(LD_, resultVarNode);
		output->WriteInstruction(PUSH);
	}

	LabelAstNode funcStartLabel(functionData->GetStart());
	OperatorAstNode opCall(OP_CALL, &funcStartLabel);
	output->Serialize(&opCall);

	if (GetResultType()->getID() != VOID_TYPE)
	{
		output->WriteInstruction(POP);
		output->WriteTypedInstruction(ST_, resultVarNode);
	}
	output->SetResult(resultVarNode);

	return 0;
}