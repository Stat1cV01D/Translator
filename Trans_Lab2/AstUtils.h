#pragma once
#include "ast.h"
#include "Operators.h"
#include "label.h"
#include "Variable.h"
#include "types.h"
#include <vector>
#include "language.tab.hpp"
#include "context.h"
#include "parser.h"
#include "HashTable.h"

class AstNode;
class OperatorStack;

// Holds all the data required for 
// programming language and TAC parsing
class ParserContext
{
protected:
	int m_LastLabelNumber;
	int m_LastTmpIndex;

	THashTable<std::string, TLabel*> *g_labelTable;
	THashTable<std::string, TVariable*> *g_tmpVarsTable;
	THashTable<std::string, TVariable*> *g_VariableTable;
	THashTable<std::string, BaseTypeInfo*> *g_TypeTable;

	OperatorStack OpStack;

	std::vector<BaseTypeInfo*> g_userTypeStack;
	static const int USER_TYPE_STACK_SIZE = 1;	

public:
	ParserContext()
		: OpStack()
	{
		g_lastInstructionIndex = 0;

		auto HashFunction = [=] (std::string& arg) -> int // Hash function here
		{
			uint32_t sum = 0;
			int	len = arg.size();
			int i = 0;

			// NOTE: for() loop doesn't work in global scoped lambdas 
			// http://connect.microsoft.com/VisualStudio/feedback/details/693671
			while(i < len)
				sum += (uint32_t)(arg[i++]);

			return sum % THashTable<int, int>::TABLE_SIZE;
		};

		auto CompareFunction = [=] (std::string& arg1, std::string& arg2) -> int // Hash function here
		{
			return arg1.compare(arg2);
		};

		g_labelTable = new THashTable<std::string, TLabel*>(HashFunction, CompareFunction);
		g_tmpVarsTable = new THashTable<std::string, TVariable*>(HashFunction, CompareFunction);
		g_VariableTable = new THashTable<std::string, TVariable*>(HashFunction, CompareFunction);
		g_TypeTable = new THashTable<std::string, BaseTypeInfo*>(HashFunction, CompareFunction);

		m_LastLabelNumber = 0;
		m_LastTmpIndex = 0;
	}

	virtual ~ParserContext()
	{
		delete g_labelTable;
		delete g_tmpVarsTable;
		delete g_TypeTable;
		delete g_VariableTable;
	}

	/// <summary>
	/// Returns the next available label number.
	/// </summary>
	/// <returns>Next label number</returns>
	int GetNextLabelNumber() { return ++m_LastLabelNumber; }
	/// <summary>
	/// Returns the index of the next temporary variable.
	/// </summary>
	/// <returns></returns>
	int GetNextTmpVarIndex() { return ++m_LastTmpIndex; }

	/// <summary>
	/// Gets the last used (current) label number.
	/// </summary>
	/// <returns></returns>
	int GetLastUsedLabelNumber() { return m_LastLabelNumber; }
	/// <summary>
	/// Gets the last (current) index of the used temporary variable.
	/// </summary>
	/// <returns></returns>
	int GetLastUsedTmpVarIndex() { return m_LastTmpIndex; }

	void ProcessLabelTable(THashTable<std::string, TLabel*>::ItemCallbackFunc PrintItem)
	{
		g_labelTable->ProcessHashTable(PrintItem);
	}

	TLabel *GenerateNewLabel()
	{
		TLabel *result = new TLabel(GetNextLabelNumber());
		g_labelTable->st_put(result->GetName(), result);
		return result;
	}

	TVariable *GenerateNewTmpVar(BaseTypeInfo *type, bool describeOnly = false)
	{
		TVariable *result = CreateVariable(type, GenerateVariableName(std::string("$t"), this->GetNextTmpVarIndex()));
		if (!describeOnly)
		{
			result->StaticMalloc();
		}
		g_tmpVarsTable->st_put(result->GetName(), result);
		return result;
	}

	TVariable *GetLastUsedTmpVar()
	{
		return g_tmpVarsTable->st_get(GenerateVariableName(std::string("$t"), this->GetLastUsedTmpVarIndex()));
	}

	// semantic analyzer functions
	bool IsVariableDefined(char *varName)
	{
		return g_VariableTable->st_exist(std::string(varName));
	}

	// semantic analyzer functions
	bool IsTypeDefined(char *typeName)
	{
		return (GetUserType(typeName) != nullptr);
	}

	void AddUserTypeToTable(BaseTypeInfo *type)
	{
		StructType *Struct = dynamic_cast<StructType *>(type);
		if (NULL != Struct)
		{
			g_TypeTable->st_set(Struct->GetTypeName(), type);
		}
	}

	bool IsLabelDeclared(char *name)
	{
		TLabel *label;
		return (((label = GetLabel(name)) != nullptr) && label->IsDeclared());
	}

	TLabel *GetLabel(char *name)
	{
		return g_labelTable->st_get(std::string(name));
	}

	StructType *GetUserType(char *name)
	{
		return dynamic_cast<StructType*>(g_TypeTable->st_get(std::string(name)));
	}

	TLabel *MakeLabel(char *name)
	{
		auto result = CreateLabel(std::string(name));
		g_labelTable->st_set(result->GetName(), result);
		return result;
	}

	bool OnUserTypeDefinition()
	{
		return (TopUserType() != nullptr);
	}

	void VerifyLabelTable()
	{
		g_labelTable->ProcessHashTable(
			[=] (TLabel*& label) -> bool
		{
			if(!(label->IsDeclared()))
			{
				printf("%d: error: label %s is not declared\n", label->GetUsedLine(), label->GetName());
			}
			else if(!(label->IsUsed()))
			{
				printf("%d: warning: label %s is declared but not used\n", label->GetDeclaredLine(), label->GetName());
			}
		}
		);
	}

	void OperatorStackPush(TOperator *op)
	{
		OpStack.Push(op);
	}

	TOperator *OperatorStackPop()
	{
		return OpStack.Pop();
	}

	TOperator *OperatorStackTop()
	{
		return OpStack.Top();
	}

	bool IsRepeatedCaseKeyVal(AstNode *keyValNode)
	{
		auto switchOp = dynamic_cast<TSwitchOperator*>(OperatorStackTop());
		if (nullptr != switchOp)
		{
			bool result = false;
			switchOp->ProcessCaseList(
				[&keyValNode, &result](TCaseOperator* caseOp) -> bool
				{
					switch(keyValNode->GetASTType())
					{
					case CONST_NODE:
						result = (dynamic_cast<NumValueAstNode*>(caseOp->GetKey())->ToInt() == dynamic_cast<NumValueAstNode*>(keyValNode)->ToInt());
						return !result;
					default:
						fprintf(stderr, "can't check case key val because invalid nodetype\n");
						return false;
					}
				}	
				);
			return result;
		}
		else
		{
			return false;
		}
	}

	// TODO: move this switch_tail grammar to AstNode's 3AC generation procedure!
	void WriteCaseJumps()
	{
		auto switchOp = dynamic_cast<TSwitchOperator*>(OperatorStackTop());
		if (nullptr != switchOp)
		{
			//TSwitchOperator *switchOp = Context.OperatorStackPop();
			//CodegenGoto(g_Output3acFile, OP_GOTO, switchOp->endLabel, NULL);
			//CodegenLabel(g_Output3acFile, switchOp->controlFlowLabel);

			switchOp->ProcessCaseList(
				[=](TCaseOperator* caseOp) -> bool
			{
				//AstNode *assignNode = HandleBinaryOperation(OP_EQ, switchOp->key, caseOp->keyVal);
				//CodegenGoto(stream, OP_IFTRUE, caseOp->label, assignNode);
			}	
			);
			//if(switchOp->defOp)
			//	CodegenGoto(g_Output3acFile, OP_GOTO,switchOp->defOp->label, NULL);
			//CodegenLabel(g_Output3acFile, switchOp->endLabel);
		}
	}

	bool IsValidKeyValNode(AstNode *keyValNode)
	{
		return (keyValNode->IsOfType(INT_TYPE) && keyValNode->GetASTType() == CONST_NODE);
	}

	bool CanUseContinue()
	{
		return (OpStack.IsLoopOperator(OperatorStackTop()));
	}

	bool CanUseBreak()
	{
		return (OpStack.IsLoopOperator(OperatorStackTop()) || OpStack.IsSwitchOperator(OperatorStackTop()));
	}

	bool IsCorrectOperatorInStack(enumOperatorType operatorType)
	{
		return (OperatorStackTop()->GetType() == operatorType);
	}

	bool IsNodeOfType(enumOperatorType operatorType)
	{
		return (OperatorStackTop()->GetType() == operatorType);
	}

	void PushUserType(BaseTypeInfo* userType)
	{
		if(g_userTypeStack.size() > USER_TYPE_STACK_SIZE)
		{
			fprintf(stderr, "fatal error: user type definition stack overflow\n");
			return;
		}
		g_userTypeStack.emplace_back(userType);
	}

	BaseTypeInfo *PopUserType()
	{
		auto result = TopUserType();
		g_userTypeStack.erase(g_userTypeStack.end());
		return result;
	}

	BaseTypeInfo *TopUserType()
	{
		return *(g_userTypeStack.end());
	}

	void DeclVar(const char *name, BaseTypeInfo *type, YYLTYPE loc)
	{
		auto ns = TBlockContext::GetCurrent()->GetBlockNamepace();
		std::string nameWithNs = ns + std::string(name);

		if (g_VariableTable->st_exist(nameWithNs))
		{
			print_error(strcatn(5, "Identifier \"", name, "\" is already declared at namespace \"",
								ns.c_str(), "\"."), loc);
			return;
		}
		TVariable *var = new TVariable(nameWithNs, type);

		g_VariableTable->st_put(nameWithNs, var);
	}

	std::string findName(const std::string name)
	{
		TBlockContext *curContext;
		for (curContext = TBlockContext::GetCurrent(); curContext != nullptr; curContext = curContext->GetParent())
		{
			std::string nameWithNs = curContext->GetBlockNamepace() + name;
			if (g_VariableTable->st_exist(nameWithNs))
			{
				return nameWithNs;
			}	
		}
		return std::string("");
	}

	TVariable *getVar(const char *name, int secure, DimensionAstNode *dimensions_list, YYLTYPE loc)
	{
		std::string nameWithNs = findName(std::string(name));
		if (nameWithNs == "")
		{
			print_error(strcatn(3, "Identifier \"", name, "\" doesn't exist in this context."), loc);
			return NULL;
		}
		TVariable *var = g_VariableTable->st_get(nameWithNs);
	
		//check array dimensions addressing (ignore if dimensions_list is NULL)
		if (dimensions_list)
		{
			int *dimensionSizes = NULL;
			int numDimensions = GetDimensionInfo(dimensions_list, &dimensionSizes);
		
			ArrayType *varType = dynamic_cast<ArrayType*>(var->GetType());

			if (varType == nullptr)
			{
				print_error("Variable is not of array type", loc);
				return NULL;
			}
		
			int numDimensionsVar = varType->GetSizes().size();

			if (dynamic_cast<ArrayType *>(var->GetType()) == nullptr)
			{
				char Err[100];
				sprintf(Err, "Number of dimensions for \"%s\" does not match. Got %d, expected %d", name, numDimensions, numDimensionsVar);
				print_error(Err, loc);
				return NULL;
			}

			/* NOTE: don't use it - the addressing might not always be NumValue
			int i = 0;
			for (i = 0; i < numDimensions; i++)
			{
				if (var->dimensionSizes[i] >= dimensionSizes[i])
				{
					char Err[100];
					sprintf(Err, "Dimension %d size for \"%s\" is too big. Got %d, expected %d", i, name, numDimensions, var->numDimensions);
					print_error(Err, loc);
					return NULL;
				}
			}
			*/
		}
		if (secure && !(var->IsInitialized()))
		{
			print_warning(strcatn(3, "Identifier \"", nameWithNs.c_str(), "\" is not initialized."), loc);
		}
		return var;
	}
};

/// <summary>
/// The general writer class
/// Takes AstNode's as input,
/// handles writing to FILE*.
/// </summary>
class AstWriter
{
private:
	FILE *m_output;

protected:
	int m_CallLevel; //contains number of levels of main class function calls
	ParserContext *context;

	/// <summary>
	/// Prints the tabulations depending on the current m_CallLevel state.
	/// </summary>
	void print_tab()
	{
		for (int i=0; i<m_CallLevel*2; ++i)
		{
			Write(" ");
		}
	}

	/// 
	/// <summary>
	/// Writes the specified code directly with no additions or changes.
	/// </summary>
	/// <param name="code">The code.</param>
	void Write(const char *code)
	{
		fprintf(m_output, "%s", code);
	}

	/// <summary>
	/// Writes the formatted code.
	/// </summary>
	/// <param name="FormatString">The format string.</param>
	/// <param name="...">Format parameters.</param>
	void WriteFormatVA(const char *FormatString, va_list args)
	{
		vfprintf(m_output, FormatString, args);
	}

	/// <summary>
	/// Writes the formatted code.
	/// </summary>
	/// <param name="FormatString">The format string.</param>
	/// <param name="...">Format parameters.</param>
	void WriteFormat(const char *FormatString, ...)
	{
		va_list args;
		va_start(args, FormatString);
		WriteFormatVA(FormatString, args);
	}

	/// <summary>
	/// Writes the code line.
	/// </summary>
	/// <param name="code">The code.</param>
	void WriteLine(const char *code)
	{
		fprintf(m_output, "%s\n", code);
	}

	int FileSeek(int offset, int mode)
	{
		return fseek(m_output, offset, mode);
	}

	/// <summary>
	/// Writes the binary code to the output file.
	/// </summary>
	/// <param name="code">The code.</param>
	/// <param name="size">The size.</param>
	void BinaryWrite(const void *code, int size)
	{
		fwrite(code, size, sizeof(char), m_output);
	}

public:
	AstWriter(AstWriter& copy)
	{
		this->m_CallLevel = copy.m_CallLevel;
		this->m_output = copy.m_output;
	}

	AstWriter(ParserContext *context, FILE *output)
	{
		if (output == nullptr)
			output = stdout;
		m_output = output;
		m_CallLevel = 0;
		this->context = context;
	}

	ParserContext *GetContext() const { return context; }
};

class TACWriter: public AstWriter
{
	std::string lastUsedValueName;
public:

	TACWriter(TACWriter &copy)
		: AstWriter(copy),
		lastUsedValueName()
	{}

	TACWriter(ParserContext *context, FILE *out = nullptr): AstWriter(context, out) {}

	int CodeGen(AstNode* node)
	{
		m_CallLevel++;
		// TODO: These should exclude "call from call" numbers used!
		//result.FirstLabelNumber = GetLastUsedLabelNumber();
		//result.FirstTmpVarIndex = GetLastUsedTmpVarIndex();
		auto result = node->Print3AC(this);
		m_CallLevel--;

		return result;
	}

	void CodeWrite(const char *code)
	{
		Write(code);
	}

	void CodeWriteFormat(const char *FormatString, ...)
	{
		// TODO [SV] 14.08.13 16:09: check the correctness here!
		va_list args;
		va_start(args, FormatString);
		WriteFormatVA(FormatString, args);
	}

	void CodeWriteLine(const char *code)
	{
		WriteLine(code);
	}

	// Used by Push 3AC command
	void SetLastUsedValueName(std::string& name)
	{
		lastUsedValueName = name;
	}

	std::string& GetLastUsedValueName() 
	{
		return lastUsedValueName;
	}
};

// The ASTree text representation class.
// Takes AstNode's as input,
// produces printed representation of AST to FILE*.
class AstPrintInfo: public AstWriter
{
public:
	AstPrintInfo(ParserContext *context, FILE *in = nullptr): AstWriter(context, in) {}

	int Print(AstNode *node)
	{
		m_CallLevel++;
		auto result = node->PrintASTree(this);
		m_CallLevel--;
		return result;
	}

	void AstWriteFormat(const char *FormatString, ...)
	{
		// TODO [SV] 14.08.13 16:09: check the correctness here!
		va_list args;
		va_start(args, FormatString);
		print_tab();
		WriteFormatVA(FormatString, args);
	}

	void AstWriteLine(const char *code)
	{
		print_tab();
		WriteLine(code);
	}
};

/// <summary>
/// TML binary code writer, serializes AST nodes into binary TML code
/// </summary>
class TMLWriter: public AstWriter
{
private:
	int g_lastInstructionIndex;

protected:	

	/// <summary>
	/// Returns the index of the next instruction.
	/// </summary>
	/// <returns></returns>
	int GetNextInstructionIndex() { return ++g_lastInstructionIndex; }

	/// <summary>
	/// Gets the last written (current) instruction index.
	/// </summary>
	/// <returns></returns>
	int GetLastWrittenInstructionIndex() { return g_lastInstructionIndex; }

	void WriteInstructionBase(const MachineInstruction instruction)
	{
		GetNextInstructionIndex();
		BinaryWrite(&instruction, sizeof(instruction));
	}

	uint8_t GetTypedOperator(TMLCOMMAND op, enumTypes typeId)
	{
		int8_t offset = op - __START;
		// calculate the typed operator

		switch(typeId)
		{
		case INT_TYPE:
			return (I_START + offset);
		case FLOAT_TYPE:
			return (F_START + offset);
		case DOUBLE_TYPE:
			return (D_START + offset);
		case LONGDOUBLE_TYPE:
			return (LD_START + offset);
		default:
			fprintf(stderr, "error: unknown type\n");
			return HALT;
		}
	}

	MachineInstruction AddAddressingModeAndArgs(const MachineInstruction _instruction, AstNode *operand)
	{
		auto instruction = _instruction;
		switch(operand->GetASTType()) 
		{
		case CONST_NODE:
			{
				instruction.AddrMode = DIRECT_MODE;
				switch(operand->GetResultType()->getID())
				{
				case INT_TYPE:
					{
						auto value = dynamic_cast<NumValueAstNode *>(operand)->ToInt();
						memcpy(&instruction.Args, &value, sizeof(value));
					}	
					break;
				case FLOAT_TYPE:
					{
						auto value = dynamic_cast<NumValueAstNode *>(operand)->ToDouble();
						memcpy(&instruction.Args, &value, sizeof(value));
					}	
					break;
				}
			}
			break;
		case ID_NODE:
		case TMP_ID_NODE:
		case VARIABLE_NODE:
		case TMP_VAR_NODE:
			{
				uint32_t index;
				instruction.AddrMode = ABSOLUTE_MODE;
				index = dynamic_cast<VarAstNode*>(operand)->GetTableReference()->GetMemoryOffset();
				memcpy(&instruction.Args, &index, sizeof(unsigned short));
			}
			break;
		case ARRAY_ITEM_NODE:
		case STRUCT_TYPE:
		case UNION_TYPE:
			{
				instruction.AddrMode = INDIRECT_MODE;

				// serialize the variable offset to the last used temporary variable
				this->Serialize(operand);

				uint32_t index = GetContext()->GetLastUsedTmpVar()->GetMemoryOffset(); // The result tmp var with the pointer (offset) to the var
				memcpy(&instruction.Args, &index, sizeof(unsigned short));
			}
			break;
		default:
			fprintf(stderr, "error: invalid nodetype in function %s\n", __FUNCTION__);
			break;
		}
		return instruction;
	}

public:
	TMLWriter(ParserContext *context, FILE *out = nullptr)
		: AstWriter(context, out)
	{
	}

	virtual ~TMLWriter()
	{
	}

	int Serialize(AstNode *node)
	{
		return node->Serialize(this);
	}

	void WriteInstruction(TMLCOMMAND op)
	{
		MachineInstruction instruction;
		memset(&instruction, 0, sizeof(instruction));
		instruction.OpCode = (uint8_t) op;
		instruction.AddrMode = ABSOLUTE_MODE;
		WriteInstructionBase(instruction);
	}

	/// <summary>
	/// Writes the typed instruction taking the operand as a parameter or just
	/// takes the type information (depending on the instruction)
	/// </summary>
	/// <param name="op">The operation code.</param>
	/// <param name="operand">The operand.</param>
	void WriteTypedInstruction(TMLCOMMAND op, AstNode* operand)
	{
		if (op <= TMLCOMMAND::__START || op >= TMLCOMMAND::_TYPED_COMMANDS_END)
			return;
		MachineInstruction instruction;
		memset(&instruction, 0, sizeof(instruction));

		// —формировать код оператора
		instruction.OpCode = GetTypedOperator(op, operand->GetResultType()->getID());
		if (op < __NOARGS)
		{
			// —формировать режим адресации и аргументы
			instruction = AddAddressingModeAndArgs(instruction, operand);
		}
		else
		{
			instruction.AddrMode = DIRECT_MODE;
		}
		WriteInstructionBase(instruction);
	}

	/// <summary>
	/// Writes the instruction to jump to the label (on condition).
	/// </summary>
	/// <param name="op">The jump condition op code.</param>
	/// <param name="label">The label to jump to if condition met.</param>
	void WriteJumpInstruction(TMLCOMMAND op, TLabel* label)
	{
		if (op <= TMLCOMMAND::_JUMP_COMMANDS_ || op >= TMLCOMMAND::_JUMP_COMMANDS_END_)
			return;

		MachineInstruction jumpInstruction;
		memset(&jumpInstruction, 0, sizeof(jumpInstruction));
		jumpInstruction.OpCode = (uint8_t) op;
		jumpInstruction.AddrMode = ABSOLUTE_MODE;
		WriteInstructionBase(jumpInstruction);
		AddJumpIndex(label, GetLastWrittenInstructionIndex());
	}

	/// <summary>
	/// Binds label to the last written instruction.
	/// </summary>
	/// <param name="label">The label.</param>
	void BindLabelToLastWrittenInstruction(TLabel* label)
	{
		label->InitLabel(GetLastWrittenInstructionIndex());
	}

	/// <summary>
	/// Binds label to the instruction that is next to be written.
	/// </summary>
	/// <param name="label">The label.</param>
	void BindLabelToNextWrittenInstruction(TLabel* label)
	{
		// NOTE: Make "+1" here so we do not increase instructions number!
		label->InitLabel(GetLastWrittenInstructionIndex() + 1);
	}

	void AfterProcess()
	{
		GetContext()->ProcessLabelTable(
			[=] (TLabel*& label) -> bool
		{
			for (auto it = label->jumpList.begin(); it != label->jumpList.end(); ++it)
			{
				long int offset;
				// TODO: i should be initialized?
				MachineInstruction i;
				int jumpIndex = (*it)->GetIndex();
				offset = 
					sizeof(TMLHeader) +
					(int32_t) (jumpIndex * sizeof(MachineInstruction)) +
					(int32_t) sizeof(i.OpCode) +
					(int32_t) sizeof(i.AddrMode);

				this->FileSeek(offset, SEEK_SET);
				unsigned int instruction_index = label->GetInstructionIndex();
				this->BinaryWrite(&instruction_index, sizeof(uint32_t));
			}
			return true;
		}
		);
		auto codeSegmentSize = (unsigned short)(g_lastInstructionIndex*sizeof(MachineInstruction));
		auto dataSegmentSize = (unsigned short)(g_wordsCount * sizeof (TMemoryCell));
		
		// Write segment sizes
		this->FileSeek(sizeof(g_MandatoryHeaderPart)-1, SEEK_SET); // Changed by A.K.
		this->BinaryWrite(&codeSegmentSize, sizeof(codeSegmentSize));
		this->BinaryWrite(&dataSegmentSize, sizeof(dataSegmentSize));
	}
};

#include "pt.h"

class PtPrintInfo: public AstWriter
{
protected:

	void print_ptNode(const PtNode *ptNode)
	{
		print_tab();
		WriteFormat("%s", ptNode->type);
		if (ptNode->text != nullptr) 
			WriteFormat(" [%s]", ptNode->text);
		WriteFormat("\n");

		for (PtNode *cur = ptNode->firstChild; cur != nullptr; cur = cur->next)
		{
			Print(cur);		
		}
	}

public:
	PtPrintInfo(ParserContext *context, FILE *in = nullptr): AstWriter(context, in) {}

	int Print(PtNode *node)
	{
		m_CallLevel++;
		print_ptNode(node);
		m_CallLevel--;
		return 0;
	}

	void PtWriteFormat(const char *FormatString, ...)
	{
		// TODO [SV] 14.08.13 16:09: check the correctness here!
		va_list args;
		va_start(args, FormatString);
		print_tab();
		WriteFormat(FormatString, args);
	}

	void PtWriteLine(const char *code)
	{
		print_tab();
		WriteLine(code);
	}
};

