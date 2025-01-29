#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assembler.h"
#include "binary_code.h"
#include "file_creators.h"
#include "binary_data.h"
void printImageObjectLineData(const ImageObjectLineData *data, int size);
void sortImageObjectLineData(ImageObjectLineData *data, int size);
void binaryToHexadecimal(const char *binaryStr, char *hexStr);
/* Function to convert two integers into a 24-character binary string */
void combineIntegersToBinaryString(int left, int right, char binaryString[WORD_SIZE + 1])
{
    // Ensure the array has enough space for 24 bits + null terminator
    if (binaryString == NULL)
    {
        return;
    }

    // Clear the binary string
    memset(binaryString, '0', WORD_SIZE);
    binaryString[WORD_SIZE] = '\0'; // Null-terminate the string

    // Convert the left integer to binary (21 bits for the leftmost part)
    for (int i = 20; i >= 0; i--)
    { // 21 bits for the left integer
        if ((left >> i) & 1)
        {
            binaryString[20 - i] = '1';
        }
    }

    // Convert the right integer to binary (3 bits for the rightmost part)
    for (int i = 2; i >= 0; i--)
    { // 3 bits for the right integer
        if ((right >> i) & 1)
        {
            binaryString[23 - i] = '1';
        }
    }
}

char *convertBinaryFieldsToBinaryString(InstructionCodeLine codeLine, char *str)
{
    // printf("codeLine.binaryFields  ARE %d\n", codeLine.binaryFields.ARE);
    if (str == NULL)
    {
        return NULL;
    }

    // Start converting fields, starting from `opcode` (leftmost) to `ARE` (rightmost)
    int idx = 0; // Index to store bits in the string

    // Convert each field to binary and append it to the string
    for (int i = 5; i >= 0; i--)
    { // opcode: 6 bits
        str[idx++] = ((codeLine.binaryFields.opcode >> i) & 1) + '0';
    }
    for (int i = 1; i >= 0; i--)
    { // sourceAddress: 2 bits
        str[idx++] = ((codeLine.binaryFields.sourceAddress >> i) & 1) + '0';
    }
    for (int i = 2; i >= 0; i--)
    { // sourceReg: 3 bits
        str[idx++] = ((codeLine.binaryFields.sourceReg >> i) & 1) + '0';
    }
    for (int i = 1; i >= 0; i--)
    { // destAddress: 2 bits
        str[idx++] = ((codeLine.binaryFields.destAddress >> i) & 1) + '0';
    }
    for (int i = 2; i >= 0; i--)
    { // destReg: 3 bits
        str[idx++] = ((codeLine.binaryFields.destReg >> i) & 1) + '0';
    }
    for (int i = 4; i >= 0; i--)
    { // funct: 5 bits
        str[idx++] = ((codeLine.binaryFields.funct >> i) & 1) + '0';
    }
    for (int i = 2; i >= 0; i--)
    { // ARE: 3 bits
        str[idx++] = ((codeLine.binaryFields.ARE >> i) & 1) + '0';
    }
    printf("convertBinaryFieldsToBinaryString final idx %d\n", idx);

    str[idx] = '\0'; // Null-terminate the string
    return str;
}
/* Function to convert ARE and size to a binary string */
void convertLabelBinaryCodeLinetoBinaryString(LabelOrImmediateBinaryCodeLine codeLine, char *binaryString, int currentIc)
{
    int symbol_table_size, line;
    int *icf;
    Bool isFound; // to erase isFound and make sure an external label is codeLine.ARE == 0
    LabelData *file_symbol_table;
    icf = getIc();
    symbol_table_size = getLabelsFileCounter();
    file_symbol_table = getSymbol_table();
    isFound = false;
    if (codeLine.ARE == 1)
    { // external lable
        line = 0;
        combineIntegersToBinaryString(line, codeLine.ARE, binaryString);
        // printf("after codeLine.ARE == 1: %s\n", binaryString);

        return;
    }

    line = FILE_FIRST_LINE;

    for (size_t i = 0; i < symbol_table_size; i++)
    {
        if (strcmp(file_symbol_table[i].name, *codeLine.nameOrImmediate.labelName) == 0)
        {
            isFound = true;
            // printf("file_symbol_table[i].name %d: %s\n", i, file_symbol_table[i].name);
            // printf("file_symbol_table[i].attrib  %d: %d\n", i, *file_symbol_table[i].attrib);

            if (*file_symbol_table[i].attrib == dataAttrib)
            {
                line = line + *icf + (*file_symbol_table[i].ic_or_dc);
                // printf("file_symbol_table[i].attrib == dataAttrib \n");
                // printf("line= %d\n", line);
            }
            else if (*file_symbol_table[i].attrib == code)
            { //! need to make sure it can be only "code" or "dataAttrib". and if so, the above if is useless
                line = line + (*file_symbol_table[i].ic_or_dc);
                // printf("file_symbol_table[i].attrib == code line: \n");
                // printf("%d\n", line);
            }
            if (codeLine.ARE == 4)
            { // relativeAdrressing
                line = line - currentIc - FILE_FIRST_LINE;
                // printf("codeLine.ARE == 4 \n");
                // printf("%d\n", line);
            }
            // else if (codeLine.ARE ==2)
            // { //! need to make sure it can be only "4" or "2". and if so, the above if is useless
            // }
        }
    }
    // if (!isFound)
    // { // this is an external label. needs to write all zeros besides a single 1 in the right
    //     line = 0;
    // }
    // printf("bef combineIntegersToBinaryString: %s\n", binaryString);

    combineIntegersToBinaryString(line, codeLine.ARE, binaryString);
    // printf("after combineIntegersToBinaryString: %s\n", binaryString);
}
void intTo24BitBinary(int num, char *binaryStr)
{
    // Ensure the input buffer is large enough
    if (binaryStr == NULL)
    {
        return;
    }

    // Create the binary representation
    for (int i = 0; i < WORD_SIZE; i++)
    {
        // Extract each bit from the most significant bit (MSB) to the least significant bit (LSB)
        binaryStr[WORD_SIZE - 1 - i] = (num & (1 << i)) ? '1' : '0';
    }

    // Null terminate the string
    binaryStr[WORD_SIZE] = '\0';
}

void convertImmediateBinaryCodeLinetoBinaryString(LabelOrImmediateBinaryCodeLine codeLine, char *binaryString)
{
    if (binaryString == NULL)
    {
        return;
    }

    int idx = 0; // Index to store bits in the string

    // Convert size (21 bits) to binary
    for (int i = 20; i >= 0; i--)
    {
        binaryString[idx++] = ((codeLine.nameOrImmediate.size >> i) & 1) + '0';
    }
    // Convert ARE (3 bits) to binary
    for (int i = 2; i >= 0; i--)
    {
        binaryString[idx++] = ((codeLine.ARE >> i) & 1) + '0';
    }

    binaryString[idx] = '\0'; // Null-terminate the string
}
void storeObjectImageWithDataImage(ImageObjectLineData *lines, int storingAmount, int initialElement)
{
    int toConvert, index;
    int *icf;
    char binaryString[WORD_SIZE + 1] = {0}; // +1 for null termination
    DataBinaryLine **ptr = getDataImage();
    if ((*ptr) == NULL)
    {
        printf("Data image is empty.\n");
        return;
    }
    icf = getIc();
    // printf("storeObjectImageWithDataImage icf= %d\n", *icf);
    index = 0;
    for (size_t i = initialElement; i < initialElement + storingAmount; i++)
    {
        // printf("storeObjectImageWithDataImage (*ptr)[index].dc= %d\n", (*ptr)[index].dc);

        lines[i].line = (*ptr)[index].dc + *icf;
        // printf("storeObjectImageWithDataImage dc= %d\n", (*ptr)[index].dc);
        if (!(*ptr)[index].type)
        {
            // printf("storeObjectImageWithDataImage this is a char \n");

            toConvert = (int)((*ptr)[index].intOrChar.c);
            // printf("storeObjectImageWithDataImage toConvert inside %d\n", toConvert);
        }
        else
        {
            toConvert = (*ptr)[index].intOrChar.size;
        }
        // printf("storeObjectImageWithDataImage toConvert= %d\n", toConvert);
        intTo24BitBinary(toConvert, binaryString);
        // printf("storeObjectImageWithDataImage binaryString= %s\n", binaryString);
        strncpy(lines[i].binary, binaryString, WORD_SIZE);
        index++;
    }
}
void storeObjectImageWithCodeImage(ImageObjectLineData *lines, int storingAmount)
{
    char binaryString[WORD_SIZE + 1] = {0}; // +1 for null termination
    CodeLine **codeImage = getCodeImage();
    if ((*codeImage) == NULL)
    {
        printf("Code image is empty.\n");
        return;
    }
    for (size_t i = 0; i < storingAmount; i++)
    {
        lines[i].line = (*codeImage)[i].ic;
        // printf("lines[i].line %d\n", (*codeImage)[i].ic);

        if ((*codeImage)[i].type == 0) // instruction type
        {
            convertBinaryFieldsToBinaryString((*codeImage)[i].data.instructionLine, binaryString);
            // printf("binaryString final %d: %s\n", i, binaryString);
        }
        else
        {
            // printf("(*codeImage)[i].data.labelOrImmediateLine.isLabel %d\n ", (*codeImage)[i].data.labelOrImmediateLine.isLabel);

            if ((*codeImage)[i].data.labelOrImmediateLine.isLabel)
            { // label handling
                convertLabelBinaryCodeLinetoBinaryString((*codeImage)[i].data.labelOrImmediateLine, binaryString, (*codeImage)[i].ic);
            }
            else
            { // immediate handling
                convertImmediateBinaryCodeLinetoBinaryString((*codeImage)[i].data.labelOrImmediateLine, binaryString);
                // printf("immediate found convertImmediateBinaryCodeLinetoBinaryString\n");
            }
        }
        strncpy(lines[i].binary, binaryString, WORD_SIZE);
        // printf("lines[i].binary %d: %s\n", i, lines[i].binary);
    }
}

void obFileHandler(const char *outputFile)
{
    int line, icf, linesAmount, codeLinesAmount, dataLinesAmount;
    ImageObjectLineData *lines;
    // printf("  obFileHandler\n");
    //

    // printf("  obFileHandler outputFile %s \n", outputFile);
    dataLinesAmount = getDataImageAllocCount();
    codeLinesAmount = getCodeImageAlloc_count();
    // printf("  obFileHandler dataLinesAmount %d \n", dataLinesAmount);
    // printf("  obFileHandler codeLinesAmount %d \n", codeLinesAmount);
    linesAmount = codeLinesAmount + dataLinesAmount;
    // printf("obFileHandler linesAmount %d \n", linesAmount);

    if (getShouldNotCreateFile())
    {
        // printf("obFileHandler  getShouldNotCreateFile is true\n");
        return;
    }
    // printf("before malloc(linesAmount * sizeof(ImageObjectLineData\n");
    // printf(" obFileHandler sizeof(ImageObjectLineData) %d\n", sizeof(ImageObjectLineData));

    lines = (ImageObjectLineData *)calloc(linesAmount, sizeof(ImageObjectLineData));

    // printf("u after malloc(linesAmount * sizeof(ImageObjectLineData\n");

    storeObjectImageWithCodeImage(lines, codeLinesAmount);
    storeObjectImageWithDataImage(lines, dataLinesAmount, codeLinesAmount);
    // it seem the above sorted the array correctly
    // printf("Before sorting: linesAmount = %d\n", linesAmount);
    // printImageObjectLineData(lines, linesAmount);

    // sortImageObjectLineData(lines, linesAmount);

    // printf("\nAfter sorting:\n");
    // printImageObjectLineData(lines, linesAmount);
    createObFile(lines, outputFile, codeLinesAmount, dataLinesAmount);
    free(lines);
}
void printImageObjectLineData(const ImageObjectLineData *data, int size)
{
    printf("printImageObjectLineData \n ");

    for (int i = 0; i < size; i++)
    {
        printf("Line: %d, Binary: %s\n", data[i].line, data[i].binary);
    }
}

void sortImageObjectLineData(ImageObjectLineData *data, int size)
{
    // Use Bubble Sort for simplicity (can be replaced with a more efficient algorithm)
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            if (data[j].line > data[j + 1].line)
            {
                // Swap the two structs
                ImageObjectLineData temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
}
