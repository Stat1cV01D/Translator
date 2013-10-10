#pragma once
#include "AstUtils.h"

class AstOptimizer
{
	ParserContext *context;
	AstNode *lastResult;
public:
	AstOptimizer(ParserContext *context)
	{
		this->context = context;
	}

	virtual ~AstOptimizer() {}

	int Optimize(AstNode* node)
	{
		auto result = node->Optimize(this);
		return result;
	}

	void SetResult(AstNode* value)
	{
		lastResult = value;
	}
	AstNode *GetLastOperationResult()
	{
		return lastResult;
	}
};

