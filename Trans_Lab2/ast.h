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

	BaseTypeInfo * GetResultType() { return resultType; }
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
	LoopConditionAstNode *cond;
	AstNode *block;
	bool post_check;
public:

	/*
	class LoopCodeGenInfo: public TACWriter
	{
		int LoopLabel;
		int EndLabel;
	public:
		LoopCodeGenInfo(const TACWriter& orig_output, int LoopLabel, int EndLabel)
			: TACWriter(orig_output)
		{
			this->LoopLabel = LoopLabel;
			this->EndLabel = EndLabel;
		}
	};
	*/

	LoopAstNode(
		LoopConditionAstNode *cond,
		AstNode *block,
		bool post_check = false): AstNode(LOOP_NODE, new VoidType()) 
	{
		this->cond = cond;
		this->block = block;
		this->post_check = post_check;
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

class NumValueAstNode: public AstNode
{
	bool isConstant;
	std::string value; //string because of ROM type which is to be converted
	TVariable *varTableReference;

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
		switch (varTableReference->GetType()->getID())
		{
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
		switch (varTableReference->GetType()->getID())
		{
		case INT_TYPE:
			return atoi(value.c_str());
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

};

class DimensionAstNode: public AstNode
{
	// Either expression node or NumValue node
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
	virtual int PrintASTree(AstPrintInfo* output) { return 0; }
	virtual int Serialize(TMLWriter* output);

	AstNode *GetExpr() { return dimension_expr; }
	DimensionAstNode *GetNextDim() { return next_dimension; }
	void SetNextDim(DimensionAstNode *next_dimension) { this->next_dimension = next_dimension; }
};

class VarAstNode: public AstNode
{
	TVariable *varTableReference;
public:
	VarAstNode(bool isTemporary, TVariable *varTableReference): 
	  AstNode((isTemporary ? TMP_ID_NODE : ID_NODE), varTableReference->GetType()) 
	{
		this->varTableReference = varTableReference;
	}
	TVariable *GetTableReference() { return varTableReference; }
	virtual int Print3AC(TACWriter* output);
	virtual int PrintASTree(AstPrintInfo* output);
	virtual int Serialize(TMLWriter* _output);

};

class ArrayAddressAstNode: public AstNode
{
	VarAstNode *var;
	DimensionAstNode *dimensions;
public:
	ArrayAddressAstNode(VarAstNode *var, DimensionAstNode *dimensions): 
	  AstNode(ARRAY_ITEM_NODE, var->GetResultType()) 
	  {
		  this->var = var;
		  this->dimensions = dimensions;
	  }

	  virtual int Print3AC(TACWriter* output) { return 0; }
	  virtual int PrintASTree(AstPrintInfo* output) { return 0; }
	  virtual int Serialize(TMLWriter* output);
};

class StructAddressAstNode: public AstNode
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
	virtual int Print3AC(TACWriter* output)
	{
		//TODO [SV] 16.08.13 18:19: complete here!
		return 0;
	}
	virtual int PrintASTree(AstPrintInfo* output) { return 0; }
	virtual int Serialize(TMLWriter* output);
};

class DeclIDAstNode: public AstNode
{
	bool isConstant;
	std::string name;
	DimensionAstNode* dim_list;
public:
	DeclIDAstNode(BaseTypeInfo *Type)
		: AstNode(DECL_ID_NODE, Type) 
	{
		this->dim_list = dim_list;
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

	virtual int Print3AC(TACWriter* output) { throw std::string("Not available"); }
	virtual int PrintASTree(AstPrintInfo* output) { return 0; }
	virtual int Serialize(TMLWriter* output) { throw std::string("Not available"); }
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

	// the left node (assignee)
	AstNode *left;
	// the right node (assigner)
	AstNode *right;

	/// <summary> General constructor handler because as of VS10 there's no support for C++11 delegated constructors </summary>
	void Init(opEnum operation, AstNode *left, AstNode *right = nullptr, BaseTypeInfo* resultType = nullptr)
	{
		this->left = left;
		this->right = right;
		this->operation = operation;
		this->op_3ac_name = std::string(Get3acOperatorText(operation));
	}

	int SerializeProcessor(TMLWriter* output);

public:
	OperatorAstNode(char *op_3ac_name, AstNode *left, AstNode *right = nullptr, BaseTypeInfo* resultType = nullptr): 
		AstNode(OPER_NODE, (resultType == nullptr ? resultType : left->GetResultType()))
	{
		this->left = left;
		this->right = right;
		this->op_3ac_name = std::string(op_3ac_name);
		this->operation = Get3acOperatorCode(op_3ac_name);
	}

	OperatorAstNode(char operation, AstNode *left, AstNode *right = nullptr, BaseTypeInfo* resultType = nullptr): 
		AstNode(OPER_NODE, (resultType == nullptr ? resultType : left->GetResultType()))
	{
		Init((opEnum)operation, left, right, resultType);
	}
		
	OperatorAstNode(opEnum operation, AstNode *left, AstNode *right = nullptr, BaseTypeInfo* resultType = nullptr): 
		AstNode(OPER_NODE, (resultType == nullptr ? resultType : left->GetResultType()))
	{
		Init(operation, left, right, resultType);
	}

	virtual int Print3AC(TACWriter* output);
	virtual int PrintASTree(AstPrintInfo* output);
	virtual int Serialize(TMLWriter* output);

	opEnum GetOpID() { return operation; }
	const char* GetOpName() { return op_3ac_name.c_str(); }
	AstNode *GetLeftOperand() { return left; }
	AstNode *GetRightOperand() { return right; }
};

class SwitchAstNode: public AstNode
{
private:
	AstNode *key;
	AstNode *case_list;
	AstNode *case_default;

public:
	SwitchAstNode(AstNode *key, AstNode *case_list, AstNode *case_default): AstNode(SWITCH_STMT_NODE, new VoidType())
	{
		this->key = key;
		this->case_list = case_list;
		this->case_default = case_default;
	}

	virtual int Print3AC(TACWriter* output) { return 0; }
	virtual int PrintASTree(AstPrintInfo* output) { return 0; }
	virtual int Serialize(TMLWriter* output) { return 0; }
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
