#pragma once
#include <vector>
#include <functional>

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
	OT_DEFAULT
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
	TLabel *label;				// Метка для перехода к этому оператору
	
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
	AstNode *keyVal;				// Указатель на узел ast-дерева, в котором хранится значение ключа
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
	TLabel *endLabel;			// метка выхода
	AstNode *key;					// ключ
	std::vector<TCaseOperator *> caseList;			// список операторов case
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

class OperatorStack
{
private:

	int m_stackTop;
	std::vector<TOperator *> g_operatorStack;

public:
	OperatorStack(void);
	virtual ~OperatorStack(void);

	static bool IsLoopOperator(TOperator *op);
	static bool IsConditionalOperator(TOperator *op);
	static bool IsSwitchOperator(TOperator *op);

	void Push(TOperator *op);
	TOperator *Pop();
	TOperator *Top();

	bool IsEmpty();
};

