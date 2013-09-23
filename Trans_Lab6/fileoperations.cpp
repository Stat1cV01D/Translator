#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "..\Trans_Lab2\tml.h"

extern FILE* g_InputFile;

int OpenProgramFile(char* inputFilename)
{
    g_InputFile = fopen(inputFilename, "rb");
    if (NULL == g_InputFile)
        return !0;
    return 0;
}

int CloseProgramFile(void)
{
	if (g_InputFile)
		fclose(g_InputFile);
    g_InputFile = (FILE *)NULL;
    return 0;
}

int LoadProgramFileHeader(FILE* inputFile, uint16_t* dataSegmentSize, uint16_t* codeSegmentSize)
{
    char headerText[4];

    if (0 == fread(headerText, sizeof(g_MandatoryHeaderPart), 1, inputFile))
    {
        return -1;
    }
    
    if (0 != memcmp(headerText, g_MandatoryHeaderPart, sizeof(g_MandatoryHeaderPart)))
    {
        return -2;
    }
    if (0 == fread(codeSegmentSize, sizeof(uint16_t), 1, inputFile))
    {
        return -3;
    }
    if (0 == fread(dataSegmentSize, sizeof(uint16_t), 1, inputFile))
    {
        return -4;
    }

    return 0;
}
