#ifndef AST_H_INCLUDED
#define AST_H_INCLUDED
#include <vector>
#include <string>
#include <stdint.h>
#include "label.h"
#include "basic-ops.h"
#include "types.h"
#include "variable.h"
#pragma warning(disable : 4996)
#pragma warning(disable : 4482)
#include "Operators.h"
#include "language.tab.hpp"
#include "parser.h"

#define NODETYPE(NAME) NAME ## _NODE,
enum TreeType
{
	LexemeDispatch,
	#include "nodeTypes.inc"
};
#undef NODETYPE

class TACWriter;
class AstPrintInfo;
class TMLWriter;

/// Abstract syntax tree generation base class.
/// Contains general methods description to be overridden in child-classes.
class AstNode
{
    TreeType nodeType;
	BaseTypeInfo *resultType;
public:

	AstNode(TreeType category, BaseTypeInfo *type) 
	{
		nodeType = category;
		resultType = type;
	}

	~AstNode()
	{
		delete resultType;
	}

	virtual BaseTypeInfo * GetResultType() { return resultType; }
	TreeType GetASTType() { return nodeType; }

	bool IsEqual(AstNode *n2)
	{
		return (this->GetResultType()->getID() == n2->GetResultType()->getID());
	}

	bool IsOfType(enumTypes typeID)
	{
		return (this->GetResultType()->getID() == typeID);
	}

	bool IsNodeOfType(BaseTypeInfo *t)
	{
		return (t != nullptr && IsOfType(t->getID()));
	}

	bool IsVarTypeEqual(TVariable *var)
	{
		return (IsNodeOfType(var->GetType()));
	}

	/// <summary> Prints out the Three address code </summary>
	virtual int Print3AC(TACWriter* output) = 0;
	virtual int PrintASTree(AstPrintInfo* output) = 0;
	virtual int Serialize(TMLWriter* output) = 0;
};

class IValueHolderNode
{
public:
	virtual std::string GetValueHolderName() = 0;
	virtual ~IValueHolderNode() {}
};

class StatementAstNode: public AstNode
{
	AstNode* stmnt;
public:
	StatementAstNode(AstNode* _stmnt) 
		: AstNode(STMNT_NODE, (new VoidType())) 
	{ 
		this->stmnt = _stmnt; 
	}

	virtual int Print3AC(TACWriter* output) { return stmnt->Print3AC(output); } 
	virtual int PrintASTree(AstPrintInfo* output) { return stmnt->PrintASTree(output); } 
	virtual int Serialize(TMLWriter* output) { return stmnt->Serialize(output); } 
};

class StatementBlockAstNode: public AstNode
{
	std::vector<AstNode*> stmnts;
public:
	StatementBlockAstNode(): AstNode(STMNT_BLOCK_NODE, new VoidType()) {}
	void AddStatement(AstNode *stmnt) { stmnts.emplace_back(stmnt); }
	~StatementBlockAstNode() { for (auto it = stmnts.begin(); it != stmnts.end(); it++) delete (*it); }

	virtual int Print3AC(TACWriter* output) 
	{
		int result;
		for (auto it = stmnts.begin(); it != stmnts.end(); it++) 
			if ((result = (*it)->Print3AC(output)) != 0)
				return result;
		return 0;
	}

	virtual int PrintASTree(AstPrintInfo* output)
	{
		int result;
		for (auto it = stmnts.begin(); it != stmnts.end(); it++) 
			if ((result = (*it)->PrintASTree(output)) != 0)
				return result;
		return 0;
	}

	/// <summary>
	/// Serializes the AstNode (this) to TML code.
	/// </summary>
	/// <param name="output">The output information.</param>
	/// <returns></returns>
	virtual int Serialize(TMLWriter* output)
	{		
		int result;
		for (auto it = stmnts.begin(); it != stmnts.end(); it++) 
			if ((result = (*it)->Serialize(output)) != 0)
				return result;
		return 0;
	}
};

class ConditionalAstNode: public AstNode
{
	AstNode *cond;
	AstNode *true_block;
	AstNode *false_block;
public:
	ConditionalAstNode(
		AstNode *cond,
		AstNode *true_block,
		AstNode *false_block = nullptr): AstNode(IF_NODE, new VoidType()) 
	{
		this->cond = cond;
		this->true_block = true_block;
		this->false_block = false_block;
	}

	virtual int Print3AC(TACWriter* output);
	virtual int PrintASTree(AstPrintInfo* output);
	virtual int Serialize(TMLWriter* output);

	~ConditionalAstNode()
	{
		if (cond)
			delete cond;
		if (true_block)
			delete true_block;
		if (false_block)
			delete false_block;
	}
};

class LoopConditionAstNode: public AstNode
{
	AstNode *init_expr;
	AstNode *b_expr;
	AstNode *post_expr;
public:
	
	LoopConditionAstNode(
		AstNode *init_expr,
		AstNode *b_expr,
		AstNode *post_expr): AstNode(LOOP_COND_NODE, new VoidType()) 
	{
		this->b_expr = b_expr;
		this->init_expr = init_expr;
		this->post_expr = post_expr;
	}

	// Conditional node can't be used apart from loop node
	// Instead Loop nodes gets *_expr's from this node 
	virtual int Print3AC(TACWriter* output) { return 0; }
	virtual int PrintASTree(AstPrintInfo* output);
	virtual int Serialize(TMLWriter* output) { return 0; }

	AstNode *GetInit_expr() { return init_expr; }
	AstNode *GetB_expr() { return b_expr; }
	AstNode *GetPost_expr() { return post_expr; }

	~LoopConditionAstNode()
	{
		if (init_expr)
			delete init_expr;
		if (b_expr)
			delete b_expr;
		if (post_expr)
			delete post_expr;
	}
};

class LoopAstNode: public AstNode
{
private:
	LoopConditionAstNode *cond;
	AstNode *block;
	bool post_check;

	TLabel *entranceLabel;
	TLabel *outLabel;
public:
	LoopAstNode(
		LoopConditionAstNode *cond,
		AstNode *block,
		TLabel *entranceLabel,
		TLabel *outLabel,
		bool post_check = false): AstNode(LOOP_NODE, new VoidType()) 
	{
		this->cond = cond;
		this->block = block;
		this->post_check = post_check;
		this->entranceLabel = entranceLabel;
		this->outLabel = outLabel;
	}

	virtual int Print3AC(TACWriter* output);
	virtual int PrintASTree(AstPrintInfo* output);
	virtual int Serialize(TMLWriter* output);

	~LoopAstNode()
	{
		delete cond;
		delete block;
	}
};

// Value is placed directly into the TML command argument
class NumValueAstNode: public AstNode, public IValueHolderNode
{
	// TODO: make value storage|cache list so that not to write duplicate values?

	std::string value; //string because of ROM type which is to be converted

	int RomanToInt()
	{
		char romanNums[] = "IVXLCDM";
		int arabicNums[] = {1, 5, 10, 50, 100, 500, 1000};
		int len = value.size();
		int *result = new int[len];
		int total = 0, i = 0, j = 0, n = 0;

		// try convert to int from roman
		for (i = 0; i < len; i++)
		{
			for (j = 0; j < sizeof(romanNums); j++)
			{
				if (value[i] = romanNums[j])
					result[i] = arabicNums[j];
			}
		}

		for (n = 0; n < len; n++)
		{
			if (result[n] >= result[n+1])
			{
				total = total + result[n];

			}
			else if (result[n] < result[n+1])
			{
				total = total + (result[n+1]-result[n]);
				n++;
			}
		}
		return total;
	}
public:
	NumValueAstNode(int value): AstNode(CONST_NODE, new IntType()) 
	{
		char convert_buf[10];
		sprintf(convert_buf, "%d", value);
		this->value = std::string(convert_buf);
	}

	NumValueAstNode(float value): AstNode(CONST_NODE, new FloatType()) 
	{
		char convert_buf[10];
		sprintf(convert_buf, "%f", value);
		this->value = std::string(convert_buf);
	}

	NumValueAstNode(char *value, BaseTypeInfo *type): AstNode(CONST_NODE, type) 
	{
		this->value = std::string(value);
	}

	std::string ToString() 
	{
		return value;
	}

	int ToInt()
	{
		switch (GetResultType()->getID())
		{
		case LITERAL_TYPE:
		case INT_TYPE:
			return atoi(value.c_str());
		case FLOAT_TYPE:
			return (int)atof(value.c_str());
		case ROM_TYPE:
			return RomanToInt();
		default:
			return 0;
		}
	}

	double ToDouble()
	{
		switch (GetResultType()->getID())
		{
		case INT_TYPE:
			return atoi(value.c_str());
		case LITERAL_TYPE:
		case FLOAT_TYPE:
			return atof(value.c_str());
		case ROM_TYPE:
			return RomanToInt();
		default:
			return 0;
		}
	}

	virtual int Print3AC(TACWriter* output);
	virtual int PrintASTree(AstPrintInfo* output);
	virtual int Serialize(TMLWriter* output);

	virtual std::string GetValueHolderName()
	{
		return std::string("$c")+this->ToString();
	}
};

class DimensionAstNode: public AstNode
{
	// Either expression node (addressing) or NumValue node (declaration)
	AstNode *dimension_expr;
	DimensionAstNode *next_dimension;
public:
	DimensionAstNode(BaseTypeInfo *value_type, AstNode *dimension_expr, 
		DimensionAstNode *next_dimension): AstNode(DIMENSION_NODE, value_type)
	{
		this->dimension_expr = dimension_expr;
		this->next_dimension = next_dimension;
	}

	virtual int Print3AC(TACWriter* output);
	virtual int PrintASTree(AstPrintInfo* output);
	virtual int Serialize(TMLWriter* output);

	AstNode *GetExpr() { return dimension_expr; }
	DimensionAstNode *GetNextDim() { return next_dimension; }
	void SetNextDim(DimensionAstNode *next_dimension) { this->next_dimension = next_dimension; }
};

// Value is referenced from the heap into the TML command argument
class VarAstNode: public AstNode, public IValueHolderNode
{
	bool isAllocated;
	TVariable *varTableReference;
public:
	VarAstNode(bool isTemporary, TVariable *varTableReference): 
	  AstNode((isTemporary ? TMP_ID_NODE : ID_NODE), varTableReference->GetType()->Clone()) 
	{
		this->varTableReference = varTableReference;
		this->isAllocated = false;
	}
	TVariable *GetTableReference() { return varTableReference; }
	virtual int Print3AC(TACWriter* output);
	virtual int PrintASTree(AstPrintInfo* output);
	virtual int Serialize(TMLWriter* output);

	virtual std::string GetValueHolderName()
	{
		if (this->GetASTType() == TMP_ID_NODE)
			return this->GetTableReference()->GetName();
		else
			return std::string("$id")+this->GetTableReference()->GetName();
	}
};

class ArrayAddressAstNode: public AstNode, public IValueHolderNode
{
	VarAstNode *var;
	DimensionAstNode *dimensions;

	int GetNumDimensions()
	{
		int dimensions_num = 0;
		for (auto cur = this->dimensions; cur->GetNextDim() != nullptr; cur = cur->GetNextDim())
		{
			dimensions_num++;
		}
		return dimensions_num;
	}

public:
	ArrayAddressAstNode(VarAstNode *var, DimensionAstNode *dimensions)
		: AstNode(ARRAY_ITEM_NODE, var->GetResultType()) 
	{
		this->var = var;
		this->dimensions = dimensions;

		auto varType = dynamic_cast<ArrayType*>(var->GetResultType());

		if (varType->GetSizes().size() < GetNumDimensions())
			throw std::string("Error: too much dimensions for the declared variable");
	}

	~ArrayAddressAstNode()
	{

	}

	// TODO: make it more flexible to assignment checks for:
	// 1) array = var
	// 2) var = array
	// 3) array = array
	virtual BaseTypeInfo *GetResultType()
	{
		int dimensions_num = GetNumDimensions();
		auto varType = dynamic_cast<ArrayType*>(var->GetResultType());
		
		if (varType->GetSizes().size() == dimensions_num)
		{
			return varType->GetBaseType();
		}
		else
		{
			return varType; //same as return AstNode::GetResultType();
		}
	}

	virtual int Print3AC(TACWriter* output);
	virtual int PrintASTree(AstPrintInfo* output);
	virtual int Serialize(TMLWriter* output);

	virtual std::string GetValueHolderName()
	{
		return this->var->GetValueHolderName();
	}
};

class StructAddressAstNode: public AstNode, public IValueHolderNode
{
	VarAstNode *Struct;
	TVariable *Field;
public:
	StructAddressAstNode(VarAstNode *Struct, TVariable *Field): AstNode(STRUCT_ITEM_NODE, Field->GetType()) 
	{
		this->Struct = Struct;
		this->Field = Field;
	}

	~StructAddressAstNode() 
	{
		delete Struct;
		delete Field;
	}

	TVariable *GetStruct() { return Struct->GetTableReference(); }
	TVariable *GetField() { return Field; }

	virtual int Print3AC(TACWriter* output);
	virtual int PrintASTree(AstPrintInfo* output); 
	virtual int Serialize(TMLWriter* output);

	virtual std::string GetValueHolderName()
	{
		return GetStruct()->GetName()+std::string(".")+GetField()->GetName();
	}
};

class DeclIDAstNode: public AstNode
{
	//bool isConstant;
	//std::string name;
public:
	DeclIDAstNode(BaseTypeInfo *Type)
		: AstNode(DECL_ID_NODE, Type) 
	{
	}

	virtual int Print3AC(TACWriter* output) { throw std::string("Not available"); return 0; }
	virtual int PrintASTree(AstPrintInfo* output) { return 0; }
	virtual int Serialize(TMLWriter* output) { return 0; }
};

class LabelAstNode: public AstNode
{
	unsigned int place;
	BaseTypeInfo *valueType;
	TLabel *labelTableReference;
public:
	LabelAstNode(TLabel *labelTableReference): AstNode(LABEL_NODE, new VoidType())
	{
		this->labelTableReference = labelTableReference;
	}

	TLabel *GetLabel() { return labelTableReference; }

	virtual int Print3AC(TACWriter* output);
	virtual int PrintASTree(AstPrintInfo* output) { return 0; }
	virtual int Serialize(TMLWriter* output);
};

/// The core class to work with TML generation process - 
/// writes the basic command that have 2 or 3 arguments 
/// from other high level AST nodes.
/// Otherwise a usual AST node.
class OperatorAstNode: public AstNode
{
	// 3-address code name 
	std::string op_3ac_name;
	opEnum operation;

	// The resulting temporary variable index. Used by primary AST gen algo, NOT by 3ac AST gen.
	int tmp_index; 

	// the left node
	AstNode *left;
	// the right node
	AstNode *right;
	// the result node
	AstNode *result;

	/// <summary> General constructor handler because as of VS10 there's no support for C++11 delegated constructors </summary>
	void Init(opEnum operation, AstNode *left, AstNode *right = nullptr, AstNode* result = nullptr)
	{
		this->left = left;
		this->right = right;
		this->result = result;
		this->operation = operation;
		this->op_3ac_name = std::string(Get3acOperatorText(operation));
	}

	int SerializeProcessor(TMLWriter* output);

public:
	OperatorAstNode(char *op_3ac_name, AstNode *left, AstNode *right = nullptr, AstNode* result = nullptr): 
	  AstNode(OPER_NODE, (result != nullptr ? result->GetResultType()->Clone() : left->GetResultType()->Clone()))
	{
		this->left = left;
		this->right = right;
		this->result = result;
		this->op_3ac_name = std::string(op_3ac_name);
		this->operation = Get3acOperatorCode(op_3ac_name);
	}

	OperatorAstNode(char operation, AstNode *left, AstNode *right = nullptr, AstNode* result = nullptr): 
		AstNode(OPER_NODE, (result != nullptr ?  result->GetResultType()->Clone() : left->GetResultType()->Clone()))
	{
		Init((opEnum)operation, left, right, result);
	}
		
	OperatorAstNode(opEnum operation, AstNode *left, AstNode *right = nullptr, AstNode* result = nullptr): 
		AstNode(OPER_NODE, (result != nullptr ?  result->GetResultType()->Clone() : left->GetResultType()->Clone()))
	{
		Init(operation, left, right, result);
	}

	virtual int Print3AC(TACWriter* output);
	virtual int PrintASTree(AstPrintInfo* output);
	virtual int Serialize(TMLWriter* output);

	opEnum GetOpID() { return operation; }
	const char* GetOpName() { return op_3ac_name.c_str(); }
	AstNode *GetLeftOperand() { return left; }
	AstNode *GetRightOperand() { return right; }
	AstNode *GetResult() { return result; }
};

class SwitchAstNode: public AstNode
{
private:
	AstNode *key;
	AstNode *case_list;
	AstNode *case_default;
	TSwitchOperator *opData;
public:
	SwitchAstNode(AstNode *key, AstNode *case_list, AstNode *case_default, TSwitchOperator *opData): AstNode(SWITCH_STMT_NODE, new VoidType())
	{
		this->key = key;
		this->case_list = case_list;
		this->case_default = case_default;
		this->opData = opData;
	}

	virtual int Print3AC(TACWriter* output);
	virtual int PrintASTree(AstPrintInfo* output);
	virtual int Serialize(TMLWriter* output);
};

class VerboseAstNode: public AstNode
{
private:
	YYLTYPE location;
	std::string message;

public:

	enum enumVerbosityLvl
	{
		LEVEL_INFO,
		LEVEL_WARN,
		LEVEL_ERROR
	};

	VerboseAstNode(enumVerbosityLvl verbosityLevel, char *message, YYLTYPE location): AstNode(ERROR_NODE, new VoidType())
	{
		this->location = location;
		this->message = std::string(message);

		switch(verbosityLevel)
		{
		case LEVEL_INFO:
			//print_error(message, location);
			break;
		case LEVEL_WARN:
			print_warning(message, location);
			break;
		case LEVEL_ERROR:
			print_error(message, location);
			break;
		}
	}

	YYLTYPE GetLocation() { return location; }

	virtual int Print3AC(TACWriter* output) { return 0; }
	virtual int PrintASTree(AstPrintInfo* output) { return 0; }
	virtual int Serialize(TMLWriter* output) { return 0; }

};

#endif // AST_H_INCLUDED
