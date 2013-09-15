#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int LoadProgramFileHeader(FILE* inputFile, unsigned short* dataSegmentSize, unsigned short* codeSegmentSize)
{
    char headerText[5] = "\0";
    int size = strlen(g_MandatoryHeaderPart);
    if (0 == fread(headerText, size, 1, inputFile))
    {
        return -1;
    }
    
    headerText[size] = 0;
    if (0 != strcmp(headerText, g_MandatoryHeaderPart))
    {
        return -2;
    }
    if (0 == fread(codeSegmentSize, sizeof(unsigned short), 1, inputFile))
    {
        return -3;
    }
    if (0 == fread(dataSegmentSize, sizeof(unsigned short), 1, inputFile))
    {
        return -4;
    }

    return 0;
}
