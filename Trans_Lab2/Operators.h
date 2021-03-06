#pragma once
#include <vector>
#include <functional>
#include "BaseTypeClass.h"
#include "variable.h"

class AstNode;
class TLabel;

enum enumOperatorType
{
	OT_IF_ELSE,
	OT_WHILE,
	OT_DO_WHILE,
	OT_FOR,
	OT_SWITCH,
	OT_CASE,
	OT_DEFAULT,
	OT_FUNCTION,
	
	//optimization-related types
	OT_PUSH,
	OT_POP
};

class TOperator
{
private:
	enumOperatorType type;
protected:
	TOperator(enumOperatorType type) 
	{
		this->type = type;
	}

	TOperator() {}
public:

	virtual ~TOperator() {}

	enumOperatorType GetType() { return type; }
};

class TSimpleOperator: public TOperator
{
private:
	TLabel *entranceLabel;
	TLabel *outLabel;
public:
	TSimpleOperator(enumOperatorType type, TLabel *entranceLabel, TLabel *outLabel)
		: TOperator(type) 
	{
		this->entranceLabel = entranceLabel;
		this->outLabel = outLabel;
	}

	virtual ~TSimpleOperator() {}
	TLabel *GetEntranceLabel() { return entranceLabel; }
	TLabel *GetOutLabel() { return outLabel; }
};

class TDefaultOperator: public TOperator
{
protected:
	TLabel *label;				// ����� ��� �������� � ����� ���������
	
	TDefaultOperator(enumOperatorType type, TLabel *label)
		: TOperator(type) 
	{
		this->label = label;
	}

public:
	TDefaultOperator(TLabel *label)
		: TOperator(OT_DEFAULT) 
	{
		this->label = label;
	}
	virtual ~TDefaultOperator() {}

	TLabel *GetLabel() { return label; }
};

class TCaseOperator: public TDefaultOperator
{
private:
	AstNode *keyVal;				// ��������� �� ���� ast-������, � ������� �������� �������� �����
public:
	TCaseOperator(AstNode *keyVal, TLabel *label): TDefaultOperator(OT_CASE, label)
	{
		this->keyVal = keyVal;
	}

	virtual ~TCaseOperator() {}

	AstNode *GetKey() { return keyVal; }
};

class TSwitchOperator: public TOperator
{
private:
	TLabel *controlFlowLabel;	// L0
	TLabel *endLabel;			// ����� ������
	AstNode *key;					// ����
	std::vector<TCaseOperator *> caseList;			// ������ ���������� case
	TDefaultOperator *defOp;
public:
	typedef std::tr1::function<bool (TCaseOperator *)> CallbackFunc;

	TSwitchOperator(AstNode *key, TLabel *controlFlowLabel, TLabel *endLabel)
		: TOperator(OT_SWITCH)
		, caseList()
	{
		this->key = key;
		this->controlFlowLabel = controlFlowLabel;
		this->endLabel = endLabel;
	}

	virtual ~TSwitchOperator()
	{
		for (auto it = caseList.begin(); it != caseList.end(); it++)
			delete (*it);
	}

	void AddCase(TCaseOperator *op)
	{
		caseList.emplace_back(op);
	}

	void AddDefaultOp(TDefaultOperator *op)
	{
		defOp = op;
	}

	void ProcessCaseList(CallbackFunc func)
	{
		for (auto it = caseList.begin(); it != caseList.end(); it++)
			if (!func((*it)))
				break;
	}

	TDefaultOperator *GetDefaultOp() const
	{
		return defOp;
	}

	TLabel *GetEndLabel() { return endLabel; }
};


class TPushOperator: public TOperator
{
private:
	int codeSegmentOffset;
public:
	TPushOperator(int codeSegmentOffset)
		: TOperator(OT_PUSH)
	{
		this->codeSegmentOffset = codeSegmentOffset;
	}

	virtual ~TPushOperator() {}

	int GetCodeSegmentOffset()
	{
		return codeSegmentOffset;
	}
};

class TPopOperator: public TOperator
{
public:
	TPopOperator()
		: TOperator(OT_POP)
	{
		
	}
	virtual ~TPopOperator() {}
};

class TFunctionOperator: public TOperator
{
protected:
	TVariable *returnValue;			// return var type label
	TLabel *enterLabel;				// function start label
	std::string name;
	std::string blockNameSpace;
	std::vector<TVariable*> parameters;
	bool isUsed; // Has the function been called? (optimization-related)
	TLabel *returnLabel;
public:
	TFunctionOperator(TVariable *returnValue, std::string &name, std::string &nameSpace, TLabel *enterLabel, TLabel *returnLabel)
		: TOperator(OT_FUNCTION) 
	{
		this->returnValue = returnValue;
		this->enterLabel = enterLabel;
		this->name = name;
		this->blockNameSpace = nameSpace;
		this->returnLabel = returnLabel;
	}
	virtual ~TFunctionOperator() {}

	TLabel *GetStart() { return enterLabel; }
	BaseTypeInfo *GetResultType() { return (returnValue == nullptr ? new VoidType() : returnValue->GetType()); }
	std::string GetName() { return name; }
	std::string GetBlockNameSpace() { return blockNameSpace; }
	TLabel* GetReturnLabel() { return returnLabel; }

	std::vector<TVariable*> GetParametersList() { return parameters; }
	void SetParametersList(std::vector<TVariable*> &parameters)
	{
		this->parameters = parameters;
	}

	void SetUsed(bool value)
	{
		isUsed = value;
	}
	bool GetUsed()
	{
		return isUsed;
	}

	void SetReturnValue(TVariable *value)
	{
		returnValue = value;
	}
	TVariable *GetReturnValue()
	{
		return returnValue;
	}	
};

class OperatorStack
{
private:
	int m_stackTop;
	std::vector<TOperator *> g_operatorStack;

public:
	typedef std::tr1::function<bool (TOperator *)> CallbackFunc;

	OperatorStack(void);
	virtual ~OperatorStack(void);

	static bool IsLoopOperator(TOperator *op);
	static bool IsConditionalOperator(TOperator *op);
	static bool IsSwitchOperator(TOperator *op);

	void Push(TOperator *op);
	TOperator *Pop();
	TOperator *Top();

	bool IsEmpty();

	void ProcessOperatorsStack(CallbackFunc func)
	{
		for (auto it = g_operatorStack.begin(); it != g_operatorStack.end(); it++)
			if (!func((*it)))
				break;
	}
};

