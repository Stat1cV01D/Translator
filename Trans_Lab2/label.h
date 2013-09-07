/* 
Распространяется под лицензией zlib,
см. http://www.gzip.org/zlib/zlib_license.html
Разработчик - Иван Якимов.
*/

#ifndef _LABEL_H_
#define _LABEL_H_

/******************************************************************
	Функции, переменные и макроопределения по работе с метками
******************************************************************/

#include <vector>
#include "definitions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "tml.h"
#include <string>

extern unsigned int g_lastInstructionIndex;
extern unsigned short g_wordsCount;

class TJumpIndex
{
	unsigned int index;
public:
	std::string GetName() 
	{	
		char buf[10];
		itoa(index, buf, 10);
		return std::string(buf);
	}

	unsigned int GetIndex()
	{
		return index;
	}

	TJumpIndex(unsigned int index)
	{
		this->index = index;
	}
};

TJumpIndex *CreateJumpIndex(unsigned int jumpIndex);
void FreeJumpIndex(void *jump);
void PrintJumpIndex(FILE* stream, void *jump);

class TLabel
{
	unsigned int instructionIndex;
	std::string name;
	bool isUsed;
	bool isDeclared;
	uint32_t usepointLineNumber;
	uint32_t declarationLineNumber;

	void Init()
	{
		this->instructionIndex = 0;
		this->name = name;
		this->usepointLineNumber = 0;
		this->declarationLineNumber = 0;
	}
public:
	
	std::vector<TJumpIndex*> jumpList;
	TLabel(std::string& name)
	{
		Init();
	}

	TLabel(unsigned int number)
	{
		char buf[10];
		
		Init();
		itoa(number, buf, 10);
		std::string result("$L");
		this->name = result += std::string(buf);
	}

	~TLabel()
	{}

	void InitLabel(unsigned int instructionIndex);
	unsigned int GetInstructionIndex() { return instructionIndex; }
	std::string GetName() { return name; }

	bool IsUsed() { return (this->usepointLineNumber != 0); }
	bool IsDeclared() { return (this->declarationLineNumber != 0); }

	uint32_t GetUsedLine() { return usepointLineNumber; }
	uint32_t GetDeclaredLine() { return declarationLineNumber; }

	void SetUsedLine(uint32_t value) { usepointLineNumber = value; }
	void SetDeclaredLine(uint32_t value) { declarationLineNumber = value; }
};

TLabel *CreateLabel(std::string& name);
std::string GenerateLabelName(unsigned int number);
void InitLabel(TLabel *label, unsigned int instructionIndex);
void AddJumpIndex(TLabel *label, unsigned int jumpIndex);
void WriteLabelIndexToJumpInstructions(FILE *stream, void* label);

#endif