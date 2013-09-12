#include "label.h"

TLabel *CreateLabel(std::string& name)
{
	return new TLabel(name);
}

std::string GenerateLabelName(unsigned int number)
{
	char buf[10];
	itoa(number, buf, 10);
	std::string result("$L");
	return (result += std::string(buf));
}

void TLabel::InitLabel(unsigned int instructionIndex)
{
	this->instructionIndex = instructionIndex;
}

TJumpIndex *CreateJumpIndex(unsigned int jumpIndex)
{
	return new TJumpIndex(jumpIndex);
}

void FreeJumpIndex(void *jump)
{
	TJumpIndex *j = (TJumpIndex *)jump;
	delete j;
}

void PrintJumpIndex(FILE* stream, void *jump)
{
	TJumpIndex *j = (TJumpIndex *)jump;
	fprintf(stream, "(%s)", j->GetName());
}

void AddJumpIndex(TLabel *label, unsigned int jumpIndex)
{
	TJumpIndex *jump = CreateJumpIndex(jumpIndex);
	label->jumpList.emplace_back(jump);
}