#pragma once
#include "ast.h"
#include "AstUtils.h"
#include <hash_map>
/*
class AstOptimizer
{
	ParserContext *context;
	AstNode *lastResult;

	std::hash_map<TVariable*, std::vector<std::pair<AstNode*, AstNode*>>> UseDefChainsTable;
	std::vector<AstNode*> UseDefStack;

	int BuildUseDefChainsRecursive(AstNode* _node)
	{
		auto blockNode = dynamic_cast<StatementBlockAstNode *>(_node);
		if (blockNode == nullptr)
		{	
			AstNode *node;
			if ((node = dynamic_cast<VarAstNode *>(_node)) != nullptr)
			{
				auto opNode = (VarAstNode*)node;
				if (opNode->GetOpID() == OP_ASSIGN)

			}
		}
		else	
		{
			blockNode->ProcessStatements(
				[&result](AstNode *node) -> int
			{
				auto blockAstNode = dynamic_cast<StatementBlockAstNode*>(node);
				if (blockAstNode != nullptr)
				{
					auto _result = BuildUseDefChainsRecursive(blockAstNode);
					for(auto it = _result.begin(); it != _result.end(); it++)
					{
						result.emplace_back(*it);
					}
				}
				else
				{
					result.emplace_back(node);
				}
				return 0;
			}
			);
		}

		return result;
	}

public:
	AstOptimizer(ParserContext *context)
	{
		this->context = context;
	}

	virtual ~AstOptimizer() {}

	int BuildUseDefChains(AstNode* node)
	{
		


	}

	IOptimizable::OptResult Optimize(AstNode* node)
	{
		IOptimizable *optNode = dynamic_cast<IOptimizable *>(node);
		if (nullptr != optNode)
		{
			return optNode->Optimize(this);
		}
		else
		{
			return IOptimizable::optOK;
		}
	}

	ParserContext *GetContext() { return context; }

	void SetResult(AstNode* value)
	{
		lastResult = value;
	}
	AstNode *GetLastOperationResult()
	{
		return lastResult;
	}
};

*/