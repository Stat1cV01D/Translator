#include <string.h>
#include <stdlib.h>
#include "context.h"
#include "common.h"

TBlockContext *TBlockContext::bl_context = nullptr;

void TBlockContext::Init()
{
	bl_context = new TBlockContext(nullptr, std::string(""));
}

void TBlockContext::Push()
{
	char SubBlockID[50];
	itoa(bl_context->curSubBlock, SubBlockID, 10);
	TBlockContext *newContext = new TBlockContext(bl_context, GetCurrent()->GetBlockNamepace() + std::string(SubBlockID) + std::string(":"));
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
