#pragma once
#include "AstUtils.h"

class AstOptimizer
{
	ParserContext *context;
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
};

