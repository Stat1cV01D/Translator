#include <string.h>
#include <stdlib.h>
#include "context.h"
#include "common.h"

TBlockContext *TBlockContext::bl_context = nullptr;
bool TBlockContext::EarlyFunctionDefPush = false;

void TBlockContext::Init()
{
	bl_context = new TBlockContext(nullptr, std::string(""));
}

void TBlockContext::Push()
{
	if (EarlyFunctionDefPush)
	{
		EarlyFunctionDefPush = false;
	}
	else
	{
		char SubBlockID[50];
		itoa(GetCurrent()->curSubBlock, SubBlockID, 10);
		TBlockContext *newContext = new TBlockContext(bl_context, GetCurrent()->GetBlockNamepace() + std::string(SubBlockID) + std::string(":"));
		bl_context = newContext;
	}
}

void TBlockContext::Push_FunctionParametersDef(std::string &funcName)
{
	EarlyFunctionDefPush = true;

	char SubBlockID[50];
	itoa(GetCurrent()->curSubBlock, SubBlockID, 10);
	TBlockContext *newContext = new TBlockContext(bl_context, funcName + std::string(SubBlockID) + std::string(":"));
	bl_context = newContext;
}

void TBlockContext::Pop()
{
	TBlockContext *oldContext = bl_context;
	bl_context = bl_context->parent;
	++(bl_context->curSubBlock);
	delete (oldContext);
}

TBlockContext *TBlockContext::GetCurrent()
{
	return bl_context;
}

std::string TBlockContext::GetBlockNamepace()
{
	return this->ns;
}

TBlockContext *TBlockContext::GetParent()
{
	return this->parent;
}
