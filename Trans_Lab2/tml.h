/* Макроопределения, типы данных и прототипы функций,
   используемых для интерпретатора языка псевдомашины ToyMachine.
      
   Распространяется под лицензией zlib,
     см. http://www.gzip.org/zlib/zlib_license.html

   Разработчик - Александр Кузнецов.
   Проект начат 25.05.2005, модифицирован 02.11.2008, 10.05.2013, 11.05.2013

   Обязательно включение этого заголовочного файла
     в исходный текст интерпретатора языка псевдомашины ToyMachine.
   Пример:
   #include "tml.h"

   Модифицирован под задание 7-го варианта 07-08.2013
*/

#ifndef _TML_H
#define _TML_H
#include <stdio.h>
#include <stdint.h>

/* System Instruction Set
 * Suffixes used below:
     I - for integers,
     F - for floating point numbers (single precision).
 */

#define BIN_ARITH(SUFFIX) \
	ADD ## SUFFIX, \
	SUB ## SUFFIX, \
	MUL ## SUFFIX, \
	DIV ## SUFFIX, \
	MOD ## SUFFIX, 

#define ACCUMULATOR_OPS(SUFFIX) \
	LD ## SUFFIX, \
	ST ## SUFFIX, \

#define COMPARE_OPS(SUFFIX) \
	CMP ## SUFFIX,
	
#define UNARY_OPS(SUFFIX) \
	NEG ## SUFFIX, \
	ABS ## SUFFIX, \
	TST ## SUFFIX, 

#define IO_OPS(SUFFIX) \
	IN ## SUFFIX, \
	OUT ## SUFFIX, 

#define POW_OP(SUFFIX) \
	POW ## SUFFIX, 

#define CONVERT_OPS(TO, FROM1, FROM2, FROM3) \
	CV ## FROM1 ## TO, \
	CV ## FROM2 ## TO, \
	CV ## FROM3 ## TO, 

#define INSERTCOMMANDS(SUFFIX) \
	SUFFIX ## _START, \
	BIN_ARITH(SUFFIX) \
	ACCUMULATOR_OPS(SUFFIX) \
	COMPARE_OPS(SUFFIX) \
	POW_OP(SUFFIX) \
	SUFFIX ## _NOARGS, \
	UNARY_OPS(SUFFIX) \
	IO_OPS(SUFFIX) 

/* Operation codes */
enum TMLCOMMAND : uint8_t
{
	/* Halt processor */
	HALT,
	// general command names
	INSERTCOMMANDS(_) 
	// typed command names
	INSERTCOMMANDS(I)
	INSERTCOMMANDS(F)
	INSERTCOMMANDS(D)
	INSERTCOMMANDS(LD)
	_TYPED_COMMANDS_END,

	/* Conversions */
	CONVERT_OPS(I, F, D, LD)
	CONVERT_OPS(F, I, D, LD)
	CONVERT_OPS(D, I, F, LD)
	CONVERT_OPS(LD, I, F, D)
	/* Increment and decrement integer value in Accumulator */
	INC,
	DEC,
	/* (Un-)conditional branches */
	_JUMP_COMMANDS_,
	JMP,
	JMZ,
	JMNZ,
	JMG,
	JML,
	_JUMP_COMMANDS_END_,
	/* Stack operations */
	PUSH,
	POP,
	LAST_OPCODE = POP, 
};

/* 256-bit memory cell */
typedef union
{
	unsigned long int  Content;
	long double ldContent;
} TMemoryCell;

/* Machine status word */
typedef struct
{
	unsigned char Z : 1; /* Zero flag */
	unsigned char G : 1; /* Greater flag */
	unsigned char L : 1; /* Lesser flag */
	unsigned char RESERVED : 5;
} TMachineStatus;

/* Size of instruction arguments, in bytes */
#define COMMAND_ARGS_MAX_SIZE 10

/* variant 7 addition */
union STACKDATA
{
	char data[COMMAND_ARGS_MAX_SIZE];
	int iData;
	float fData;
	TMemoryCell cellData;
};

/* variant 7 addition */
typedef struct STACK
{
	STACKDATA data;
	struct STACK* prev;
} TStack;

/* Addressing modes */
#define ABSOLUTE_MODE			0x00
#define DIRECT_MODE				0x01
#define INDIRECT_MODE			0x02
#define LAST_MODE				0x02

/* Machine instruction */
typedef struct
{
    uint8_t OpCode;   /* Operation code. See above */
    uint8_t AddrMode; /* Addressing mode. See above */
    uint8_t Args[COMMAND_ARGS_MAX_SIZE]; /* Instruction arguments */
} MachineInstruction;

const char g_MandatoryHeaderPart[4] = {'.', 'C', 'M', 'L'};

/* Machine instruction */
typedef struct
{
	char signature[4];
	uint16_t codeSegmentSize;
	uint16_t dataSegmentSize;
} TMLHeader;

void PrintFatalError(char* message);
void PrintError(const char* errorMessage, unsigned int instructionNumber);

unsigned char GetOperationCode(unsigned char currentCodeLine[]);
unsigned char GetAddresingMode(unsigned char currentCodeLine[]);

/* variant 7 additions */
void Push(TMemoryCell Data);
STACKDATA Pop(void);

int OpenProgramFile(char* inputFilename);
int CloseProgramFile(void);
int LoadProgramFileHeader(FILE *inputFile, unsigned short* dataSegmentSize, unsigned short* codeSegmentSize);
#endif
