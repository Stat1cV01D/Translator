#pragma once
#include "ast.h"
#include "Operators.h"
#include "label.h"
#include "Variable.h"
#include "types.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include "language.tab.hpp"
#include "context.h"
#include "parser.h"
#include "HashTable.h"

class AstNode;
class OperatorStack;
class TMLWriter;
class AstPrintInfo;
class AstWriter;
class TACWriter;

// Holds all the data required for 
// programming language and TAC parsing
class ParserContext
{
protected:
	AstWriter *owner;
	int m_LastLabelNumber;
	int m_LastTmpIndex;
	int m_LastLiteralIndex;

	THashTable<std::string, TLabel*> *g_labelTable;
	THashTable<std::string, TVariable*> *g_tmpVarsTable;
	THashTable<std::string, TVariable*> *g_VariableTable;
	THashTable<std::string, BaseTypeInfo*> *g_TypeTable;
	THashTable<std::string, TVariable*> *g_LiteralTable;
	THashTable<std::string, TFunctionOperator*> *g_FunctionsTable;

	OperatorStack OpStack;

	std::vector<BaseTypeInfo*> g_userTypeStack;
	static const int USER_TYPE_STACK_SIZE = 1;	

public:
	ParserContext()
		: OpStack()
	{
		auto HashFunction = [=] (std::string& arg) -> int // Hash function here
		{
			uint32_t sum = 0;
			int	len = arg.size();
			int i = 0;

			// NOTE: for() loop doesn't work in global scoped lambdas for VS2010
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
		g_LiteralTable = new THashTable<std::string, TVariable*>(HashFunction, CompareFunction);
		g_FunctionsTable = new THashTable<std::string, TFunctionOperator*>(HashFunction, CompareFunction);

		m_LastLabelNumber = 0;
		m_LastTmpIndex = 0;
	}

	virtual ~ParserContext()
	{
		delete g_labelTable;
		delete g_tmpVarsTable;
		delete g_TypeTable;
		delete g_VariableTable;
		delete g_LiteralTable;
		delete g_FunctionsTable;
	}

	/// <summary>
	/// Returns the next available label number.
	/// </summary>
	/// <returns>Next label number</returns>
	int GetNextLabelNumber() { return ++m_LastLabelNumber; }
	/// <summary>
	/// Returns the index of the next temporary variable.
	/// </summary>
	/// <returns>The index of the next temporary variable</returns>
	int GetNextTmpVarIndex() { return ++m_LastTmpIndex; }
	/// <summary>
	/// Returns the index of the next literal variable.
	/// </summary>
	/// <returns>The index of the next literal variable</returns>
	int GetNextLiteralIndex() { return ++m_LastLiteralIndex; }

	/// <summary>
	/// Gets the last used (current) label number.
	/// </summary>
	/// <returns>The last used (current) label number</returns>
	int GetLastUsedLabelNumber() { return m_LastLabelNumber; }
	/// <summary>
	/// Gets the last (current) index of the used temporary variable.
	/// </summary>
	/// <returns>The last (current) index of the used temporary variable</returns>
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

	TVariable *GenerateNewTmpVar(BaseTypeInfo *type, bool isDescribed = false)
	{
		TVariable *result = CreateVariable(type, GenerateVariableName(std::string("$t"), this->GetNextTmpVarIndex()));
		if (!isDescribed) // means we are generating code now.
		{
			result->ReserveMemory();
		}
		g_tmpVarsTable->st_put(result->GetName(), result);
		return result;
	}
	
	//TVariable *AddLiteral(std::string& str_literal)
	//{
	//	if (g_LiteralTable->st_exist(str_literal))
	//	{
	//		return g_LiteralTable->st_get(str_literal);
	//	}
	//	else
	//	{
	//		TVariable *result = CreateVariable(new LiteralType(str_literal.size()), str_literal);
	//		result->ReserveMemory();
	//		g_LiteralTable->st_put(result->GetName(), result);
	//		return result;
	//	}
	//}


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
		if (nullptr != Struct)
		{
			g_TypeTable->st_put(Struct->GetTypeName(), type);
		}
	}

	bool IsLabelDeclared(char *name)
	{
		TLabel *label;
		return (((label = GetLabel(name)) != nullptr) && label->IsDeclared());
	}

	TLabel *GetLabel(char *name)
	{
		auto labelName = std::string(name);
		if (g_labelTable->st_exist(labelName))
			return g_labelTable->st_get(labelName);
		else
			return nullptr;
	}

	StructType *GetUserType(char *name)
	{
		auto typeName = std::string(name);
		if (g_TypeTable->st_exist(typeName))
			return dynamic_cast<StructType*>(g_TypeTable->st_get(typeName));
		else
			return nullptr;
	}

	TLabel *MakeLabel(char *name)
	{
		auto result = CreateLabel(std::string(name));
		g_labelTable->st_put(result->GetName(), result);
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
		return IsNodeOfType(operatorType);
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
		g_userTypeStack.erase(--g_userTypeStack.end());
		return result;
	}

	BaseTypeInfo *TopUserType()
	{
		if (g_userTypeStack.size() == 0)
			return nullptr;
		else
			return g_userTypeStack.back();
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
		var->ReserveMemory();
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
		if (nameWithNs == std::string(""))
		{
			print_error(strcatn(3, "Identifier \"", name, "\" doesn't exist in this context."), loc);
			return NULL;
		}
		TVariable *var = g_VariableTable->st_get(nameWithNs);
	
		//check array dimensions addressing (ignore if dimensions_list is NULL)
		if (dimensions_list != nullptr && dimensions_list->GetExpr() != nullptr)
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

	typedef std::tr1::function<bool (TVariable *)> BlockVarCallbackFunc;

	void ProcessFunctionBlockVariables(TFunctionOperator *funcOp, BlockVarCallbackFunc func)
	{
		// might not very optimal: we'll have to find all variables in all blocks
		// instead of allocating the block-dependent varibles separately
		auto blockNameSpace = funcOp->GetBlockNameSpace();
		g_VariableTable->ProcessHashTable(
			[&func, &blockNameSpace](TVariable* &var) -> bool
			{
				if (var->GetName().find_first_of(blockNameSpace) != std::string::npos)
				{
					return func(var);
				}
				else
					return true;
			}
		);
	}

	bool OnFunctionDefinition()
	{
		bool result = false;
		OpStack.ProcessOperatorsStack(
			[=, &result](TOperator *op) -> bool
		{
			auto funcDefOp = dynamic_cast<TFunctionOperator*>(op);
			return !(result = (funcDefOp != nullptr));
		}
		);
		return result;
	}

	bool IsFunctionDefined(std::string& funcName)
	{
		return g_FunctionsTable->st_exist(funcName);
	}

	TFunctionOperator *GetFunction(std::string& funcName)
	{
		if (!IsFunctionDefined(funcName))
			return nullptr;
		else
			return g_FunctionsTable->st_get(funcName);
	}

	void AddFunction(TFunctionOperator *func)
	{
		g_FunctionsTable->st_put(func->GetName(), func);
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
		WriteFormat("%s\n", code);
	}

	long FSeek(long offset, std::ios::seek_dir mode)
	{
		fseek(m_output, offset, mode);
		return ftell(m_output);
	}

	/// <summary>
	/// Writes the binary code to the output file.
	/// </summary>
	/// <param name="code">The code.</param>
	/// <param name="size">The size.</param>
	void BinaryWrite(const void *code, size_t size)
	{
		fwrite(code, sizeof(char), size, m_output);
	}

public:
	AstWriter(AstWriter& copy)
	{
		this->m_CallLevel = copy.m_CallLevel;
		this->m_output = copy.m_output;
	}

	AstWriter(ParserContext *context, FILE* output)
	{
		m_CallLevel = 0;
		this->context = context;
		this->m_output = output;
	}

	virtual ~AstWriter() {}

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

	TACWriter(ParserContext *context, FILE* out): AstWriter(context, out) {}
	
	virtual ~TACWriter() {}

	int CodeGen(AstNode* node)
	{
		m_CallLevel++;
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
	AstPrintInfo(ParserContext *context, FILE* out): AstWriter(context, out) {}
	virtual ~AstPrintInfo() {}

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
	AstNode *lastOpResultNode;

	OperatorStack PushInstructionsStack;
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

	TMLCOMMAND GetTypedOperator(TMLCOMMAND op, enumTypes typeId)
	{
		int8_t offset = (uint8_t)op - (uint8_t)__START;
		// calculate the typed operator

		switch(typeId)
		{
		case INT_TYPE:
		case ROM_TYPE: // NOTE: roman types are converted to int automatically during compilation
			return (TMLCOMMAND)((uint8_t)I_START + offset);
		case FLOAT_TYPE:
			return (TMLCOMMAND)((uint8_t)F_START + offset);
		case DOUBLE_TYPE:
			return (TMLCOMMAND)((uint8_t)D_START + offset);
		case LONGDOUBLE_TYPE:
			return (TMLCOMMAND)((uint8_t)LD_START + offset);
		case LITERAL_TYPE:
			return (TMLCOMMAND)((uint8_t)S_START + offset);
		default:
			fprintf(stderr, "error: unknown type for command\n");
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
				this->Serialize(operand);
				operand = this->GetLastOperationResult();

				instruction.AddrMode = DIRECT_MODE;
				switch(operand->GetResultType()->getID())
				{
				case ROM_TYPE:
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
		case STRUCT_ITEM_NODE:
		case UNION_TYPE:
			{
				this->Serialize(operand);
				operand = this->GetLastOperationResult();

				uint32_t index;
				instruction.AddrMode = ABSOLUTE_MODE;
				index = dynamic_cast<IValueHolderNode*>(operand)->CalculateMemoryOffset();
				memcpy(&instruction.Args, &index, sizeof(uint32_t));
			}
			break;		
		case ARRAY_ITEM_NODE:
			{
				WriteInstruction(PUSH); // Save what's been in the VM's accumulator register
				this->Serialize(operand);
				WriteInstruction(POP); // Restore it

				operand = this->GetLastOperationResult();

				instruction.AddrMode = ABSOLUTE_POINTER_MODE;
				uint32_t index = dynamic_cast<IValueHolderNode*>(operand)->CalculateMemoryOffset();
				memcpy(&instruction.Args, &index, sizeof(uint32_t));
			}
			break;
		default:
			fprintf(stderr, "error: invalid nodetype in function %s\n", __FUNCTION__);
			break;
		}
		return instruction;
	}

	void TMLWriteHeader()
	{
		TMLHeader header;
		memcpy(&header.signature, g_MandatoryHeaderPart, sizeof(g_MandatoryHeaderPart));
		header.codeSegmentSize = (uint16_t)((g_lastInstructionIndex+1)*sizeof(MachineInstruction));
		
		int size = (TVariable::GetWordsCount() * sizeof (TMemoryCell)) - varBuffer.str().size();
		auto preallocateBuf = new char[size];
		VarBinaryWrite(preallocateBuf, size);
		
		header.dataSegmentSize = varBuffer.str().size();

		FSeek(0, SEEK_SET);
		BinaryWrite(&header, sizeof(header));
	}

	std::stringstream varBuffer;
	
	void TMLFillDataSegment()
	{
		FSeek(sizeof(TMLHeader) + ((g_lastInstructionIndex+1)*sizeof(MachineInstruction)), SEEK_SET);
		auto Str = varBuffer.str();
		BinaryWrite(Str.c_str(), Str.size());
	}
	
	int VarFSeek(int pos, std::ios::seek_dir mode)
	{
		//2, std::ios::end
		varBuffer.seekp(pos, mode);
		return varBuffer.tellp();
	}

	void VarBinaryWrite(const void *data, int size)
	{
		varBuffer.write((char*)data, size);
	}

public:
	TMLWriter(ParserContext *context, FILE* out)
		: AstWriter(context, out)
		, PushInstructionsStack()
		, varBuffer(std::ios::binary | std::ios::in | std::ios::out)
	{
		TMLWriteHeader();
		//TMLFillDataSegment(); // allocate var space beforehand!
		int offset = FSeek(0, SEEK_CUR);
		g_lastInstructionIndex = -1; // so that first increment returns 0

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
		instruction.OpCode = op;
		instruction.AddrMode = ABSOLUTE_MODE;
		WriteInstructionBase(instruction);
	}

/*
	void WritePushInstruction()
	{
		WriteInstruction(PUSH);
		PushInstructionsStack.Push(new TPushOperator(FileSeek(0, SEEK_CUR)));
	}

	void WritePopInstruction()
	{
		auto pushOp = dynamic_cast<TPushOperator*>(PushInstructionsStack.Pop());

		WriteInstruction(POP);
		
	}
*/

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
		jumpInstruction.OpCode = op;
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

	void InitVariableData(VarAstNode *varNode)
	{
		//int oldPos = this->FSeek(0, SEEK_CUR);
		this->VarFSeek(varNode->CalculateMemoryOffset()*sizeof(TMemoryCell), std::ios::beg);

		switch (varNode->GetResultType()->getID())
		{
		case ROM_TYPE:
		case INT_TYPE:
			{
				auto var = varNode->GetTableReference()->ValueToInt();
				this->VarBinaryWrite(&var, sizeof(TMemoryCell)*varNode->GetTableReference()->SizeOf());
			}
			break;
		case FLOAT_TYPE:
			{
				auto var = varNode->GetTableReference()->ValueToDouble();
				this->VarBinaryWrite(&var, sizeof(TMemoryCell)*varNode->GetTableReference()->SizeOf());
			}
			break;
		case LITERAL_TYPE:
			{
				auto var = varNode->GetTableReference()->ValueToString();
				this->VarBinaryWrite(var.c_str(), sizeof(TMemoryCell)*varNode->GetTableReference()->SizeOf());
			}
			break;
		}
		
		//this->FSeek(oldPos, SEEK_SET);
	}

	void SetResult(AstNode *resultNode)
	{
		lastOpResultNode = resultNode;
	}

	AstNode *GetLastOperationResult()
	{
		return lastOpResultNode;
	}

	void AfterProcess()
	{
		WriteInstruction(HALT); // Mark the end of the program

		// Rewrite the header and the data segment with updated info and data
		GetContext()->ProcessLabelTable(
			[=] (TLabel*& label) -> bool
		{
			for (auto it = label->jumpList.begin(); it != label->jumpList.end(); ++it)
			{
				long int offset;
				MachineInstruction i;
				int jumpIndex = (*it)->GetIndex();
				offset = 
					sizeof(TMLHeader) +
					(jumpIndex * sizeof(MachineInstruction)) +
					sizeof(i.OpCode) +
					sizeof(i.AddrMode);

				this->FSeek(offset, SEEK_SET);
				unsigned int instruction_index = label->GetInstructionIndex();
				this->BinaryWrite(&instruction_index, sizeof(uint32_t));
			}
			return true;
		}
		);
		
		// Write segment sizes
		TMLWriteHeader();
		// Write data segment 
		TMLFillDataSegment();
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
	PtPrintInfo(ParserContext *context, FILE* out): AstWriter(context, out) {}
	virtual ~PtPrintInfo() {}

	int Print(PtNode *node)
	{
		m_CallLevel++;
		print_ptNode(node);
		m_CallLevel--;
		return 0;
	}

	void PtWriteFormat(const char *FormatString, ...)
	{
		va_list args;
		va_start(args, FormatString);
		print_tab();
		WriteFormatVA(FormatString, args);
	}

	void PtWriteLine(const char *code)
	{
		print_tab();
		WriteLine(code);
	}
};