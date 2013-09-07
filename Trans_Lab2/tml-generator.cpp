#include "tml-generator.h"
#include <functional>
#include <stdint.h>

void TMLWriteHeader(FILE* outputFile)
{
	fwrite(g_MandatoryHeaderPart, strlen(g_MandatoryHeaderPart), 1, outputFile);
}

void TMLSegmentSize(FILE *outputFile, unsigned short codeSegmentSize, unsigned short dataSegmentSize)
{
    /* Перейдем в позицию записи размеров сегмента кода и стека */
    // fseek(outputFile, sizeof(g_MandatoryHeaderPart-1), SEEK_SET); // It was commented By A.K.
    fseek(outputFile, sizeof(g_MandatoryHeaderPart)-1, SEEK_SET); // Changed by A.K.
    fwrite(&codeSegmentSize, sizeof(codeSegmentSize), 1, outputFile);
    fwrite(&dataSegmentSize, sizeof(dataSegmentSize), 1, outputFile);
}

void TMLFillDataSegment(FILE *outputFile)
{
	//TODO: выделить функцию вычисления начала сегмента данных!
	long int offset;
	unsigned short codeSegmentSize;
	unsigned char zeroByte[256] = {0};
	codeSegmentSize = (unsigned short)(g_lastInstructionIndex * sizeof(MachineInstruction));
	offset = 8 + (unsigned long)codeSegmentSize;
	fseek(outputFile, offset, SEEK_SET);
	fwrite(zeroByte, sizeof(unsigned char), g_wordsCount * sizeof(TMemoryCell), outputFile);
}

unsigned char GetTypedOperator(TMLCOMMAND op, enumTypes typeId)
{
	int offset = op - __START;
	// calculate the typed operator

	switch(typeId)
	{
	case INT_TYPE:
		return I_START + offset;
	case FLOAT_TYPE:
		return F_START + offset;
	case DOUBLE_TYPE:
		return D_START + offset;
	case LONGDOUBLE_TYPE:
		return LD_START + offset;
	default:
		fprintf(stderr, "error: unknown type\n");
		return HALT;
	}
}

/*
void TMLIf(FILE* outputFile, opEnum op, AstNode* condition, AstNode* label)
{
	MachineInstruction loadInstruction, compareInstruction, jumpInstruction;
	enumTypes typeId = condition->GetResultType()->getID();
	// Операция загрузки
	loadInstruction = GenerateSingleArgInstruction(LD_, condition);
	// Операция сравнения аккумулятора с нулем
	compareInstruction = GenerateNoArgInstruction(TST_, typeId);
	switch(op)
	{
	case OP_IFFALSE:
		{
			// Операция условного перехода
			jumpInstruction = GenerateJumpInstruction(JMZ);
		}
		break;
	case OP_IFTRUE:
		{
			// Операция условного перехода
			jumpInstruction = GenerateJumpInstruction(JMNZ);
		}
		break;
	default:
		fprintf(stderr, "error: invalid operator\n");
		return;
	}
	WriteInstruction(outputFile, loadInstruction);
	WriteInstruction(outputFile, compareInstruction);
	AddJumpIndex(dynamic_cast<LabelAstNode*>(label)->GetLabel(), g_lastInstructionIndex);
	WriteInstruction(outputFile, jumpInstruction);
}

void TMLGoto(FILE* outputFile, opEnum op, AstNode* label)
{
	switch(op)
	{
	case OP_GOTO:
		{
			MachineInstruction jumpInstruction;
			jumpInstruction = GenerateJumpInstruction(JMP);
			AddJumpIndex(dynamic_cast<LabelAstNode*>(label)->GetLabel(), g_lastInstructionIndex);
			WriteInstruction(outputFile, jumpInstruction);
		}	
		break;
	}
}

void TMLHalt(FILE* outputFile)
{
	MachineInstruction haltInstruction;
	memset(&haltInstruction, 0, sizeof(MachineInstruction));
	haltInstruction.OpCode = HALT;
	WriteInstruction(outputFile, haltInstruction);
}

MachineInstruction GenerateJumpInstruction(unsigned char opCode)
{
	MachineInstruction jumpInstruction;
	char args[4] = {0};
	jumpInstruction.OpCode = opCode;
	jumpInstruction.AddrMode = ABSOLUTE_MODE;
	memcpy(&jumpInstruction.Args, &args, sizeof(TMemoryCell));
	return jumpInstruction;
}

//TODO: то же что и GenerateJumpInstruction!
MachineInstruction GeneratePushPopInstruction(unsigned char opCode)
{
	return GenerateJumpInstruction(opCode);
}

MachineInstruction GenerateSingleArgInstruction(TMLCOMMAND op, AstNode *operand)
{
	MachineInstruction instruction;
	// Сформировать код оператора
	instruction.OpCode = GetTypedOperator(op, operand->GetResultType()->getID());
	// Сформировать режим адресации и аргументы
	instruction = AddAddressingModeAndArgs(instruction, operand);
	return instruction;
}

MachineInstruction GenerateNoArgInstruction(TMLCOMMAND op, enumTypes typeId)
{
	MachineInstruction instruction;
	memset(&instruction, 0, sizeof(MachineInstruction));
	instruction.OpCode = GetTypedOperator(op, typeId);
	return instruction;
}

MachineInstruction GenerateSaveLoadArrayItemInstruction(TMLCOMMAND op, AstNode* arr, AstNode* index)
{
	MachineInstruction instruction;
	//instruction.OpCode = GetOperatorImplementation(op, (arr->valueType->id));
	uint16_t memOffset = dynamic_cast<VarAstNode*>(arr)->GetTableReference()->GetMemoryOffset();
	instruction.OpCode = op; //GetOperatorImplementation(, var->GetType()->itemTypeRef->id);
	switch(index->GetASTType())
	{
	case CONST_NODE:
		{
			//TODO: Внимание! На самом деле адрес массива это int!
			uint16_t arrIndex = (unsigned short)dynamic_cast<NumValueAstNode*>(index)->ToInt();
			instruction.AddrMode = DIRECT_MODE;
			memcpy(&instruction.Args[0], &memOffset, sizeof(memOffset));
			memcpy(&instruction.Args[2], &arrIndex, sizeof(arrIndex));
		}
		break;
	case VARIABLE_NODE:
		{
			uint16_t i_memOffset = dynamic_cast<VarAstNode*>(index)->GetTableReference()->GetMemoryOffset();
			instruction.AddrMode = ABSOLUTE_MODE;
			memcpy(&instruction.Args[0], &memOffset, sizeof(memOffset));
			memcpy(&instruction.Args[2], &i_memOffset, sizeof(i_memOffset));
		}
		break;
	default:
		fprintf(stderr, "error: invalid index type\n");
		break;
	}
	return instruction;
}

MachineInstruction GenerateSaveLoadStructFieldInstruction(TMLCOMMAND op, AstNode* structNode, AstNode* fieldNode)
{
	MachineInstruction instruction;
	std::string fieldName = dynamic_cast<VarAstNode*>(fieldNode)->GetTableReference()->GetName();
	enumTypes fieldTypeId = dynamic_cast<VarAstNode*>(fieldNode)->GetTableReference()->GetType()->getID();
	TVariable *var = dynamic_cast<VarAstNode*>(structNode)->GetTableReference();
	unsigned short var_memOffset = var->GetMemoryOffset();
	unsigned short offset = dynamic_cast<StructType*>(var->GetType())->Offset(fieldName);

	instruction.OpCode = GetTypedOperator(op, fieldTypeId);
	instruction.AddrMode = DIRECT_MODE;
	memcpy(&instruction.Args[0], &var_memOffset, sizeof(unsigned short));
	memcpy(&instruction.Args[2], &offset, sizeof(unsigned short));
	return instruction;
}

void WriteBinaryLogicalInstructionSet(FILE* outputFile, opEnum op, AstNode* operand)
{
	MachineInstruction 
		testInstruction,
		jumpCondInstruction,
		loadOperandInstruction,
		loadTrueInstruction,
		jumpUncondInstruction,
		loadFalseInstructon;
	TLabel *l1, *l2;
	AstNode *trueNode, *falseNode;

	switch(operand->GetResultType()->getID())
	{
	case INT_TYPE:
		trueNode = new NumValueAstNode(TML_TRUE, new IntType());
		falseNode = new NumValueAstNode(TML_FALSE, new IntType());
		break;
	case FLOAT_TYPE:
		trueNode = new NumValueAstNode(TML_TRUE, new FloatType());
		falseNode = new NumValueAstNode(TML_FALSE, new FloatType());
		break;
	}

	l1 = new TLabel(++g_LastLabelNumber);
	l2 = new TLabel(++g_LastLabelNumber);

	testInstruction = GenerateNoArgInstruction(TST_, operand->GetResultType()->getID());
	loadOperandInstruction = GenerateSingleArgInstruction(LD_, operand);
	jumpUncondInstruction = GenerateJumpInstruction(JMP);

	switch(op)
	{
	case OP_AND:
		{
			jumpCondInstruction = GenerateJumpInstruction(JMZ);
			loadTrueInstruction = GenerateSingleArgInstruction(LD_, trueNode);
			loadFalseInstructon = GenerateSingleArgInstruction(LD_, falseNode);
		}
		break;
	case OP_OR:
		{
			jumpCondInstruction = GenerateJumpInstruction(JMNZ);
			loadTrueInstruction = GenerateSingleArgInstruction(LD_, falseNode);
			loadFalseInstructon = GenerateSingleArgInstruction(LD_, trueNode);
		}
		break;
	}

	// TODO: Make it more laconic 
	WriteInstruction(outputFile, testInstruction);			// TST
	AddJumpIndex(l1, g_lastInstructionIndex);
	WriteInstruction(outputFile, jumpCondInstruction);		// JM[N]Z L1
	WriteInstruction(outputFile, loadOperandInstruction);	// LD right
	WriteInstruction(outputFile, testInstruction);			// TST
	AddJumpIndex(l1, g_lastInstructionIndex);
	WriteInstruction(outputFile, jumpCondInstruction);		// JM[N]Z L1
	WriteInstruction(outputFile, loadTrueInstruction);		// LD "1"
	AddJumpIndex(l2, g_lastInstructionIndex); 
	WriteInstruction(outputFile, jumpUncondInstruction);	// JMP L2
	InitLabel(l1, g_lastInstructionIndex);					// L1:
	WriteInstruction(outputFile, loadFalseInstructon);		// LD "0"
	InitLabel(l2, g_lastInstructionIndex);					// L2:

	g_labelTable.st_put(l1->GetName(), l1);
	g_labelTable.st_put(l2->GetName(), l2);

	delete (trueNode);
	delete (falseNode);
}

void WriteUnaryLogicalInstructionSet(FILE* outputFile, opEnum op, enumTypes typeId)
{
	MachineInstruction
		testInstruction,
		jumpCondInstruction,
		loadTrueInstruction,
		jumpUnCondInstructon,
		loadFalseInstruction;

	TLabel *l1, *l2;
	AstNode *trueNode, *falseNode;

	switch(typeId)
	{
	case INT_TYPE:
		trueNode = new NumValueAstNode(TML_TRUE, new IntType());
		falseNode = new NumValueAstNode(TML_FALSE, new IntType());
		break;
	case FLOAT_TYPE:
		trueNode = new NumValueAstNode(TML_TRUE, new FloatType());
		falseNode = new NumValueAstNode(TML_FALSE, new FloatType());
		break;
	}

	l1 = new TLabel(++g_LastLabelNumber);
	l2 = new TLabel(++g_LastLabelNumber);

	testInstruction = GenerateNoArgInstruction(TST_, typeId);
	jumpCondInstruction = GenerateJumpInstruction(JMZ);
	loadTrueInstruction = GenerateSingleArgInstruction(LD_, trueNode);
	loadFalseInstruction = GenerateSingleArgInstruction(LD_, falseNode);
	jumpUnCondInstructon = GenerateJumpInstruction(JMP);
	
	WriteInstruction(outputFile, testInstruction);
	AddJumpIndex(l1, g_lastInstructionIndex);
	WriteInstruction(outputFile, jumpCondInstruction);
	WriteInstruction(outputFile, loadFalseInstruction);
	AddJumpIndex(l2, g_lastInstructionIndex);
	WriteInstruction(outputFile, jumpUnCondInstructon);
	InitLabel(l1, g_lastInstructionIndex);
	WriteInstruction(outputFile, loadTrueInstruction);
	InitLabel(l2, g_lastInstructionIndex);

	g_labelTable.st_put(l1->GetName(), l1);
	g_labelTable.st_put(l2->GetName(), l2);

}

void WriteInstruction(FILE *outputFile, MachineInstruction instruction)
{
	unsigned short int i = 0;
	fwrite(&instruction.OpCode, sizeof(instruction.OpCode), 1, outputFile);
	fwrite(&instruction.AddrMode, sizeof(instruction.AddrMode), 1, outputFile);
	for(i; i < COMMAND_ARGS_MAX_SIZE; i++)
		fwrite(&instruction.Args[i], sizeof(instruction.Args[i]), 1, outputFile);
	g_lastInstructionIndex++;
}
*/