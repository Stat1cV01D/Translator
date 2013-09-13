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

	// 1. generate the code for condition, return result
	output->CodeGen(cond);
	auto resultVarName = output->GetLastUsedValueName();
	// 2. check the result and process true_block (TODO [SV] 14.08.13 16:14: with possible optimization|elimination?)
	output->CodeWriteFormat("\tiffalse\t%s\tL%d\n", resultVarName.c_str(), labelNumber_false);
	if (this->true_block != nullptr)
	{
		output->CodeGen(true_block);
	}

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
			output->CodeWriteFormat("\t%s", leftVarName.c_str());
		
			output->CodeWriteFormat("\t%s", this->op_3ac_name.c_str());
		
			output->CodeGen(right);
			auto rightVarName = output->GetLastUsedValueName();
			output->CodeWriteFormat("\t%s\n", rightVarName.c_str());

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
				throw std::string("Error: GOTO node invalid!");
			}
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
		case OP_GOTO:
		case OP_BREAK: // The OP_GOTO synonym
		case OP_CONTINUE: // The OP_GOTO synonym

		case OP_INPUT:
		case OP_OUTPUT:
		case OP_SAVE:
		case OP_LOAD:
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
			// Операция ввода
			output->WriteTypedInstruction(IN_, left);
			// Операция сохранения
			output->WriteTypedInstruction(ST_, left);
		}
		break;
	case OP_OUTPUT:
		{
			// Операция загрузки
			//output->WriteTypedInstruction(LD_, left);
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
	}

	if (result != nullptr) // NOOTE: for arithmetic and logical operations only! 
		output->WriteTypedInstruction(ST_, result);
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
int VarAstNode::Serialize(TMLWriter* _output)
{
	//_output->WriteTypedInstruction(LD_, this);
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
	output->AstWriteFormat("const %s %s\n", GetResultType()->GetName().c_str(), value.c_str());
	return 0;
}
int NumValueAstNode::Serialize(TMLWriter* _output)
{
	//_output->WriteTypedInstruction(LD_, this);
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
	output->Serialize(this->GetExpr());

	// NOTE: dimension values are generated into INT values!
	output->WriteInstruction(PUSH);

	return 0;
}

int ArrayAddressAstNode::Print3AC(TACWriter* output)
{
	// Push desired addresses for the dimensions
	output->CodeGen(this->dimensions);

	auto *varType = dynamic_cast<ArrayType*>(var->GetTableReference()->GetType());
	auto sizes = varType->GetSizes();

	auto SumTmpVar = output->GetContext()->GenerateNewTmpVar(new IntType(), true);
	VarAstNode SumTmpVarNode(true, SumTmpVar);
	
	auto MulTmpVar = output->GetContext()->GenerateNewTmpVar(new IntType(), true);
	VarAstNode MulTmpVarNode(true, MulTmpVar);

	for (auto it = sizes.begin(); it != sizes.end(); it++)
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

	// SumTmpVarNode is the result var
	OperatorAstNode ArrayItemOffset(OP_PLUS, &SumTmpVarNode, this->var);
	output->SetLastUsedValueName(SumTmpVarNode.GetValueHolderName());
	
	return 0;
}

int ArrayAddressAstNode::PrintASTree(AstPrintInfo* output)
{
	auto varName = this->var->GetTableReference()->GetName();
	output->AstWriteFormat("%s\n", varName.c_str());
	output->Print(this->dimensions);
	output->AstWriteLine("");
	return 0;
}

int ArrayAddressAstNode::Serialize(TMLWriter* output)
{
	// Push desired addresses for the dimensions
	output->Serialize(this->dimensions);

	auto *varType = dynamic_cast<ArrayType*>(var->GetTableReference()->GetType());
	auto sizes = varType->GetSizes();

	auto SumTmpVar = output->GetContext()->GenerateNewTmpVar(new IntType());
	VarAstNode SumTmpVarNode(true, SumTmpVar);

	auto MulTmpVar = output->GetContext()->GenerateNewTmpVar(new IntType());
	VarAstNode MulTmpVarNode(true, MulTmpVar);

	for (auto it = sizes.begin(); it != sizes.end(); it++)
	{
		// A = POP // current dimension address
		// A = A * size
		// MulTmpVar = A
		output->WriteInstruction(POP);
		NumValueAstNode DimSizeNode(*it);
		OperatorAstNode Mul(OP_MULT, nullptr, &DimSizeNode, &MulTmpVarNode);
		output->Serialize(&Mul);

		// A = SumTmpVar + MulTmpVar
		// SumTmpVar = A
		OperatorAstNode Plus(OP_PLUS, &SumTmpVarNode, &MulTmpVarNode, &SumTmpVarNode);
		output->Serialize(&Plus);
	}

	// SumTmpVarNode is the result var
	OperatorAstNode ArrayItemOffset(OP_PLUS, &SumTmpVarNode, this->var, &SumTmpVarNode);
	output->Serialize(&ArrayItemOffset);

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

	auto SumTmpVar = output->GetContext()->GenerateNewTmpVar(new IntType());
	VarAstNode SumTmpVarNode(true, SumTmpVar);

	// SumTmpVarNode is the result var
	OperatorAstNode ArrayItemOffset(OP_PLUS, &VarOffsetNode, &FieldOffsetNode, &SumTmpVarNode);
	output->Serialize(&ArrayItemOffset);
	return 0;
}
