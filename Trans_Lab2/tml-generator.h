/* 
Распространяется под лицензией zlib,
см. http://www.gzip.org/zlib/zlib_license.html
Разработчик - Иван Якимов.
*/

#ifndef _TML_GENERATOR_H_
#define _TML_GENERATOR_H_

#include "definitions.h"
#include "tml.h"
#include "ast.h"
#include "types.h"
#include "variable.h"
#include "HashTable.h"
#include "label.h"
#include <stdio.h>
/*******************************************************************
	Объявления и макроопределения, обеспечивающие интерфейс 
	между синтаксическим анализатором и генератором машинного кода
********************************************************************/

#include "basic-ops.h"

/* Генерирует заголовок файла */
void TMLWriteHeader(FILE *outputFile);
/* Генерирует размеры участка кода и данных*/
void TMLSegmentSize(FILE *outputFile, unsigned short codeSegmentSize, unsigned short dataSegmentSize);
/* Генерирует бинарную арифметическую операцию */
void TMLBinaryArith(FILE* outputFile, opEnum op, AstNode* leftOperand, AstNode* rightOperand);
void TMLBinaryArith(FILE* outputFile, char op, AstNode* leftOperand, AstNode* rightOperand);
/* Генерирует бинарную логическую операцию */
void TMLBinaryLogical(FILE* outputFile, opEnum op, AstNode* leftOperand, AstNode* rightOperand);
/* Генерирует унарную арифметическую операцию */
void TMLUnaryArith(FILE* outputFile, opEnum op, AstNode* operand);
/* Генерирует унарную логическую операцию */
void TMLUnaryLogical(FILE* outputFile, opEnum op, AstNode* operand);
/* Генерирует операции ввода-вывода */
void TMLInputOutput(FILE* outputFile, opEnum op, AstNode *operand);
/* Генерирует операции сохранения-загрузки */
void TMLSaveLoad(FILE* outputFile, opEnum op, AstNode* operand);
/* Сохранение и загрузка элементов массивов*/
void TMLSaveLoadArrayItem(FILE* outputFile, opEnum op, AstNode* arr, AstNode* index);
/* Сохранение и загрузка элементов структуры */
void TMLSaveLoadStructItem(FILE* outputFile, opEnum op, AstNode* str, AstNode* field);
/* Генерирует iffalse */
void TMLIf(FILE* outputFile, opEnum op, AstNode* condition, AstNode* label);
/* Генерирует операцию сравнения */
void TMLComparison(FILE* outputFile, opEnum op, AstNode* leftOperand, AstNode* rightOperand);
/* Генерирует goto */
void TMLGoto(FILE* outputFile, opEnum op, AstNode* label);
/* Заполнение памяти данными */
void TMLFillDataSegment(FILE *outputFile);

/*******************************************************************
	Внутренние объявления и макроопределения, обеспечивающие работу
	генератора машинного кода
********************************************************************/

#define TML_TRUE	"1"
#define TML_FALSE	"0"

extern THashTable<std::string, TLabel*> g_labelTable;
extern THashTable<std::string, TVariable*> g_tmpVarsTable;
extern THashTable<std::string, TVariable*> g_literalTable;

/* Генерирует машинную инструкцию без операндов */ 
MachineInstruction GenerateNoArgInstruction(TMLCOMMAND op, enumTypes typeId);
/* Генерирует машинную инструкцию с одним операндом */
MachineInstruction GenerateSingleArgInstruction(TMLCOMMAND op, AstNode *operand);
/* Генерирует машинную инструкцию перехода */
MachineInstruction GenerateJumpInstruction(unsigned char opCode);
/* Генерирует машинную команду для работы со стеком */
MachineInstruction GeneratePushPopInstruction(unsigned char opCode);
/* Генерирует машинную инструкцию сохранения-загрузки элемента массива */
MachineInstruction GenerateSaveLoadArrayItemInstruction(TMLCOMMAND op, AstNode* arr, AstNode* index);
/* Генерирует машинную инструкцию сохранения-загрузки элемента структуры */
MachineInstruction GenerateSaveLoadStructFieldInstruction(TMLCOMMAND op, AstNode* structNode, AstNode* fieldNode);
/* Пишет в выходной файл набор иструкций для бинарной логической операции над содержимым аккумулятора и операндом */
void WriteBinaryLogicalInstructionSet(FILE* outputFile, opEnum op, AstNode* operand);
/* Пишет в выходной файл набор иструкций для унарной логической операции над содержимым аккумулятора */
void WriteUnaryLogicalInstructionSet(FILE* outputFile, opEnum op, enumTypes typeId);
/* Возвращает код типизированной реализации оператора */
unsigned char GetTypedOperator(TMLCOMMAND op, enumTypes typeId);
/* Добавляет в инструкцию режим адресации и аргументы */
MachineInstruction AddAddressingModeAndArgs(MachineInstruction instruction, AstNode *operand);
/* Пишет инструкцию в выходной поток */
void WriteInstruction(FILE *outputFile, MachineInstruction instruction);

#endif