/* Реализация интерпретатора языка псевдомашины ToyMachine.
      
   Распространяется под лицензией zlib,
     см. http://www.gzip.org/zlib/zlib_license.html

   Разработчик - Александр Кузнецов.
   Проект начат 25.05.2005, модифицирован 02.11.2008, 10.05.2013, 11.05.2013
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include "..\Trans_Lab2\tml.h"

TMemoryCell*        g_MachineDataSegment = NULL;
MachineInstruction* g_MachineCodeSegment = NULL;

FILE* g_InputFile = NULL;    /* поток для файла с программой на машинном языке */
TStack *g_stackTop = NULL;

TMachineStatus        PSW;

template<typename T> T GetValue(MachineInstruction MI)
{
	T result;
	switch (MI.AddrMode)
	{
	case DIRECT_MODE:
		memcpy(&result, MI.Args, sizeof (T));
		break;
	case ABSOLUTE_MODE:
		{
			uint32_t index; 
			memcpy(&index, MI.Args, sizeof (uint32_t)); 
			memcpy(&result, &g_MachineDataSegment[index], sizeof (T)); 
		}
		break;
	case ABSOLUTE_POINTER_MODE:
		{
			uint32_t index; 
			memcpy(&index, MI.Args, sizeof (index)); 
			memcpy(&index, &g_MachineDataSegment[index], sizeof(index));
			memcpy(&result, &g_MachineDataSegment[index], sizeof (T));
		}
		break;
	}

	return result;
}

template<typename T> int UnaryOperations(TMemoryCell &Accumulator, TMachineStatus &PSW, MachineInstruction MI, T &VAR)
{
	int result = 1;

	uint8_t op = MI.OpCode;
	uint8_t addresingMode = MI.AddrMode;
	auto args = MI.Args;

	switch (op)
	{
	case NEG_: 
		VAR *= (T)(-1.0); 
		memcpy(&Accumulator, &VAR, sizeof (VAR)); 
		break; 
	case ABS_: 
		VAR = (T)fabs((long double)VAR); 
		memcpy(&Accumulator, &VAR, sizeof (VAR)); 
		break; 
	case TST_: 
		PSW.Z = ((T)(0.0) == VAR); 
		PSW.L = ((T)(0.0) > VAR); 
		PSW.G = !(PSW.L || PSW.Z); 
		break; 
	default:
		result = 0;
	}
	return result;
}

// type equality compile time check code
template <typename T, typename U>
struct same_type 
{
static const bool value = false;
};
template <typename T>
struct same_type< T, T >
{
static const bool value = true;
};

template<typename T> int IOOperations(TMemoryCell &Accumulator, MachineInstruction MI, char *str_template, T& VAR)
{
	int result = 1;
	
	uint8_t op = MI.OpCode;
	uint8_t addresingMode = MI.AddrMode;
	auto args = MI.Args;

	switch (op)
	{
	case LD_: 
		{
			T tmpValue = GetValue<T>(MI);
			memcpy(&Accumulator, &tmpValue, sizeof (T)); 
		}
		break; 
	case ST_: 
		if (addresingMode == ABSOLUTE_MODE) 
		{ 
			uint32_t index; 
			memcpy(&index, args, sizeof (uint32_t)); 
			memcpy(&g_MachineDataSegment[index], &Accumulator, sizeof(T)); 
		} 
		else if (addresingMode == ABSOLUTE_POINTER_MODE)
		{
			uint32_t index; 
			memcpy(&index, args, sizeof (index)); 
			memcpy(&index, &g_MachineDataSegment[index], sizeof(index));
			memcpy(&g_MachineDataSegment[index], &Accumulator, sizeof(T));
		}
		break;
	case IN_:
	case OUT_: 
		{
			T *ptrVAR = nullptr;
			switch (MI.AddrMode)
			{
			case DIRECT_MODE:
				ptrVAR = &VAR;
				break;
			case ABSOLUTE_MODE:
				{
					uint32_t index; 
					memcpy(&index, args, sizeof (uint32_t)); 
					ptrVAR = (T*)&g_MachineDataSegment[index];
				}
				break;
			case ABSOLUTE_POINTER_MODE:
				{
					uint32_t index; 
					memcpy(&index, args, sizeof (index)); 
					memcpy(&index, &g_MachineDataSegment[index], sizeof(index));
					ptrVAR = (T*)&g_MachineDataSegment[index];
				}
				break;
			}

			switch (op)
			{
			case IN_: 
				{
					scanf(str_template, ptrVAR);
					memcpy(&Accumulator, ptrVAR, sizeof (*ptrVAR));
				}
				break; 
			case OUT_:
				{
					// NOTE: an unfortunate workaround for the strings
					if (same_type<T, char>::value) 
						printf(str_template, ptrVAR);
					else
						printf(str_template, VAR);
				}
				break; 
			}
		}
	default:
		result = 0;
	}
	return result;
}

template<typename T> int PowOperations(TMemoryCell &Accumulator, MachineInstruction MI, T& VAR)
{
	int result = 1;

	uint8_t op = MI.OpCode;
	uint8_t addresingMode = MI.AddrMode;
	auto args = MI.Args;

	switch (op)
	{
	case POW_: 
		if (addresingMode == DIRECT_MODE) 
		{
			T result = 1.0, times = 0.0; 
			memcpy(&times, args, sizeof (T)); 
			result = (T)pow((long double)VAR, (long double)times); 
			memcpy(&Accumulator, &result, sizeof (T)); 
		} 
		break;
	default:
		result = 0;
	}
	return result;
}

template<typename T> int BinaryOperations(TMemoryCell &Accumulator, MachineInstruction MI, T& VAR)
{
	int result = 1;

	uint8_t op = MI.OpCode;
	uint8_t addresingMode = MI.AddrMode;
	auto args = MI.Args;
	
	T tmpValue = GetValue<T>(MI);

	switch (op)
	{
	case ADD_: 
		VAR = VAR + tmpValue;
		break;
	case SUB_:
		VAR = VAR - tmpValue;
		break;
	case MUL_:
		VAR = VAR * tmpValue;
		break;
	case DIV_:
		VAR = VAR / tmpValue;
		break;
	case MOD_: 
		VAR = (T)fmod((long double)VAR, (long double)tmpValue);
		break;
	case CMP_: 
		{
			PSW.Z = (tmpValue == VAR); 
			PSW.L = (tmpValue >  VAR); 
			PSW.G = !(PSW.L || PSW.Z); 
		}
		break;
	default:
		result = 0;
	}
	if (result)
		memcpy(&Accumulator, &VAR, sizeof(T)); 
	return result;
}

#define CONVERT_OPS(TO, TO_VAR, TO_VARTYPE, \
	FROM1, FROM1_VAR, \
	FROM2, FROM2_VAR, \
	FROM3, FROM3_VAR) \
	case CV ## FROM1 ## TO: \
	TO_VAR = (TO_VARTYPE)FROM1_VAR; \
	memcpy(&Accumulator, & ## TO_VAR, sizeof(TO_VAR)); \
	break; \
	case CV ## FROM2 ## TO: \
	TO_VAR = (TO_VARTYPE)FROM2_VAR; \
	memcpy(&Accumulator, & ## TO_VAR, sizeof(TO_VAR)); \
	break; \
	case CV ## FROM3 ## TO: \
	TO_VAR = (TO_VARTYPE)FROM3_VAR; \
	memcpy(&Accumulator, & ## TO_VAR, sizeof(TO_VAR)); \
	break; 

int main(int argc, char* argv[])
{
    MachineInstruction codeLine;
	TMLCOMMAND operationCode;
    uint8_t addresingMode;
    unsigned char args[COMMAND_ARGS_MAX_SIZE];
    TMemoryCell    Accumulator;
    unsigned long  ProgramCounter;
    unsigned int lastInstructionNumber;

    unsigned char isHaltCommandMet = 0; /* команды HALT не было */

    unsigned short dataSegmentSize = 0, codeSegmentSize = 0;
    const int instructionSize = sizeof (MachineInstruction);
    struct stat programFileState;
    int fileOpeningStatus = 0, fileHeaderLoadingStatus = 0;

    unsigned long programSize = 0;
    int tmpInteger = 0;
    float tmpFloat = 0.0f;
	double tmpDouble = 0.0;
	long double tmpLongDouble = 0.0;
	char tmpCharPtr = 0;

    if (2 != argc)
    {
        PrintFatalError("Run the TML interpreter as \'tml <filename>\'");
    }

    fileOpeningStatus = OpenProgramFile(argv[1]);
    if (0 != fileOpeningStatus)
    {
        char tmp[132] = "\0";
        sprintf(tmp, "File %s doesn't exist", argv[1]);
        PrintFatalError(tmp);
    }

    fileHeaderLoadingStatus = LoadProgramFileHeader(g_InputFile, &dataSegmentSize, &codeSegmentSize);
    if (0 != fileHeaderLoadingStatus)
    {
        switch(fileHeaderLoadingStatus)
        {
        case -1 :
            PrintFatalError("Can't read the mandatory part of the header.");
            break;
        case -2 :
            PrintFatalError("The mandatory part is unknown.");
            break;
        case -3 :
            PrintFatalError("Can't read Code Segment size.");
            break;
        case -4 :
            PrintFatalError("Can't read Data Segment size.");
            break;
        default:
            PrintFatalError("Something wrong.");
        }
    }

    programSize = dataSegmentSize + codeSegmentSize
                  + sizeof(dataSegmentSize) + sizeof(codeSegmentSize)
                  + strlen(g_MandatoryHeaderPart) 
                  ;
    stat(argv[1], &programFileState);
    if (programSize < (unsigned int)programFileState.st_size)
    {
        PrintFatalError("Not enough memory to load the program.");
    }

    g_MachineCodeSegment = (MachineInstruction *) malloc(codeSegmentSize);
    if (NULL == g_MachineCodeSegment
        || 0 == fread(g_MachineCodeSegment, codeSegmentSize, 1, g_InputFile)
       )
    {
        PrintFatalError("Can't read this code segment.");
    }

    g_MachineDataSegment = (TMemoryCell *) malloc(dataSegmentSize);
    if (NULL == g_MachineDataSegment
        || 0 == fread(g_MachineDataSegment, dataSegmentSize, 1, g_InputFile)
        )
    {
        PrintFatalError("Can't read this data segment.");
    }

    CloseProgramFile();

    lastInstructionNumber = codeSegmentSize / instructionSize - 1;
    ProgramCounter = 0;
    Accumulator.Content = 0;
    PSW.Z = PSW.G = PSW.L = 0;

    /* Бесконечный цикл работы загруженной программы.
     * Условие останова в теле цикла 
     */


    while (!0)
    {
        if (ProgramCounter > lastInstructionNumber)
            break;
        memcpy(&codeLine, &g_MachineCodeSegment[ProgramCounter], instructionSize);
        operationCode = codeLine.OpCode;
        addresingMode = codeLine.AddrMode;
        if (LAST_MODE < addresingMode)
		{
			PrintError("Unknown addressing mode", ProgramCounter);
			break;
		}
        memcpy(args, &codeLine.Args, COMMAND_ARGS_MAX_SIZE);
        memcpy(&tmpInteger, &Accumulator, sizeof (int));
        memcpy(&tmpFloat, &Accumulator, sizeof (float));
		memcpy(&tmpDouble, &Accumulator, sizeof (double));
		memcpy(&tmpLongDouble, &Accumulator, sizeof (long double));
		memcpy(&tmpCharPtr, &Accumulator, sizeof (char*));

		// I don't the nature of it yet, but i need that "+2", lol
#define OPS(SUFFIX, TYPE, STR, VAR) \
		if ((uint8_t)operationCode > (uint8_t)SUFFIX ## _START) \
		{ \
			codeLine.OpCode = (TMLCOMMAND)((uint8_t)codeLine.OpCode - (uint8_t)SUFFIX ## _START + (uint8_t)__START); \
			if (PowOperations<TYPE>(Accumulator, codeLine, VAR)); \
			else if (BinaryOperations<TYPE>(Accumulator, codeLine, VAR)); \
			else if (UnaryOperations<TYPE>(Accumulator, PSW, codeLine, VAR)); \
			else if (IOOperations<TYPE>(Accumulator, codeLine, STR, VAR)); \
			codeLine.OpCode = (TMLCOMMAND)((uint8_t)codeLine.OpCode + (uint8_t)SUFFIX ## _START - (uint8_t)__START); \
		} 

		if ((uint8_t)operationCode > (uint8_t)__START  && (uint8_t)operationCode < (uint8_t)_TYPED_COMMANDS_END)
		{
			// The order is important!
			OPS(S,char,"%s", tmpCharPtr)
			else OPS(LD,long double,"%ld", tmpLongDouble)
			else OPS(D,double,"%d", tmpDouble)
			else OPS(F,float,"%f", tmpFloat)
			else OPS(I,int,"%d", tmpInteger)
		}
		else
		{
			switch (operationCode)
			{
			case HALT:
				isHaltCommandMet = !0;
				break;
			case NOP:
				break;
			case INC:
				++tmpInteger;
				memcpy(&Accumulator, &tmpInteger, sizeof (int));
				break;
			case DEC:
				--tmpInteger;
				memcpy(&Accumulator, &tmpInteger, sizeof (int));
				break;


			CONVERT_OPS(I, tmpInteger, int, F, tmpFloat, D, tmpDouble, LD, tmpLongDouble)
			CONVERT_OPS(F, tmpFloat, float, I, tmpInteger, D, tmpDouble, LD, tmpLongDouble)
			CONVERT_OPS(D, tmpDouble, double, I, tmpInteger, F, tmpFloat, LD, tmpLongDouble)
			CONVERT_OPS(LD, tmpLongDouble, long double, I, tmpInteger, F, tmpFloat, D, tmpDouble)
        
			case JMP:
				memcpy(&ProgramCounter, args, sizeof(int));
				continue;
			case JMZ:
				if (PSW.Z)
				{
					memcpy(&ProgramCounter, args, sizeof(int));
					continue;
				}
				break;
			case JMNZ:
				if (!PSW.Z)
				{
					memcpy(&ProgramCounter, args, sizeof(int));
					continue;
				}
				break;
			case JMG:
				if (PSW.G)
				{
					memcpy(&ProgramCounter, args, sizeof(int));
					continue;
				}
				break;
			case JML:
				if (PSW.L)
				{
					memcpy(&ProgramCounter, args, sizeof(int));
					continue;
				}
				break;

			/* variant 7 additions */
			case PUSH:
				{
					Push(Accumulator);
				}
				break;
			case POP:
				{
					Accumulator = Pop().cellData;
				}
				break;
			default:
				PrintError("Unknown code operation", ProgramCounter);
			}
		}
        if (0 != isHaltCommandMet)
            break;
        ++ProgramCounter;
    }

    free(g_MachineCodeSegment);
    free(g_MachineDataSegment);
#ifdef DEBUG
    printf("\nTML program successfully executed.\r");
#endif
    return EXIT_SUCCESS;
}

/* Print error messages */

void PrintFatalError(char* errorMessage)
{
    free(g_MachineCodeSegment);
    free(g_MachineDataSegment);
    CloseProgramFile();

    printf("%s\nExited by failure!\n", errorMessage);
    exit(EXIT_FAILURE);
}

void PrintError(const char* errorMessage, unsigned int instructionNumber)
{
    printf("ERROR: %s at instruction # %d.\n", errorMessage, instructionNumber);
}

void Push(TMemoryCell Data)
{
	STACK *stackItem = (STACK*)malloc(sizeof(STACK));
	memset(stackItem, 0, sizeof(STACK));

	stackItem->data.cellData = Data;
	if (g_stackTop)
		g_stackTop->prev = stackItem;
	else
		g_stackTop = stackItem;
}

STACKDATA Pop(void)
{
	STACKDATA result;
	memset(&result, 0, sizeof(STACKDATA));

	if (g_stackTop)
	{
		result = g_stackTop->data;
		STACK *old = g_stackTop;
		g_stackTop = g_stackTop->prev;
		free(old);
	}
	return result;
}
