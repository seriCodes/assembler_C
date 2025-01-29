#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assembler.h"
#include "file_creators.h"

/* Comparator function for qsort */
int compareByLine(const void *a, const void *b)
{
    const ImageLabelLineData *labelA = (const ImageLabelLineData *)a;
    const ImageLabelLineData *labelB = (const ImageLabelLineData *)b;

    // Compare by line
    return labelA->line - labelB->line;
}
void sortLabelsByLine(ImageLabelLineData *labels, size_t count)
{
    if (labels == NULL || count == 0)
    {
        fprintf(stderr, "Invalid input: labels array is NULL or count is 0.\n");
        return;
    }

    qsort(labels, count, sizeof(ImageLabelLineData), compareByLine);
}
void createLabelFile(const ImageLabelLineData *labels, const char *InputFile, int labelsCounted)
{
    int currentDigitsAmount;
    FILE *fOutpt;
    char *strCopy;
    printf(" createLabelFile strncpy InputFile %s \n", InputFile);
    fOutpt = fopen((InputFile), "w");
    if (fOutpt == NULL)
    {
        printf("Error: Could not open the label files to output.\n");
        return;
    }
    for (size_t i = 0; i < labelsCounted; i++)
    {
        fprintf(fOutpt, labels[i].name);
        fprintf(fOutpt, "    ");
        currentDigitsAmount = countDigits(labels[i].line);
        // for (size_t i = 0; i < ADDRESS_DIGITS_AMOUNT - 3; i++)
        for (size_t i = 0; i < ADDRESS_DIGITS_AMOUNT - currentDigitsAmount; i++)
        {
            fprintf(fOutpt, "0");
        }
        fprintf(fOutpt, "%d", labels[i].line);
        if (i == labelsCounted - 1)
        {
            continue; // to avoid a new line in the end of the file
        }
        fprintf(fOutpt, "\n");
    }
    if (fOutpt != NULL)
    {
        fclose(fOutpt);
        fOutpt = NULL; // Safeguard against reuse
    }
}

void binaryToHexadecimal(const char *binaryStr, char *hexStr)
{
    if (binaryStr == NULL || hexStr == NULL)
    {
        return;
    }

    // Verify the input binary string is 24 characters + null terminator
    if (strlen(binaryStr) != 24)
    {
        printf("Error: Input binary string must be exactly 24 characters long.\n");
        return;
    }

    unsigned int decimalValue = 0;

    // Convert the binary string to a decimal integer
    for (int i = 0; i < 24; i++)
    {
        if (binaryStr[i] != '0' && binaryStr[i] != '1')
        {
            printf("Error: Input string contains non-binary characters.\n");
            return;
        }
        decimalValue = (decimalValue << 1) | (binaryStr[i] - '0');
    }

    // Convert the decimal value to a hexadecimal string
    snprintf(hexStr, 7, "%06X", decimalValue); // 6 digits + null terminator
}

//! use below after havg the fuul ImageObjectLineData
void createObFile(const ImageObjectLineData *lines, const char *InputFile, int codeLinesAmount, int dataLinesAmount)
{
    char hexStr[7]; // 6 hex digits + null terminator
    int currentDigitsAmount, iterations;
    FILE *fOutpt;
    char *strCopy;
    printf(" createLabelFile strncpy InputFile %s \n", InputFile);
    fOutpt = fopen((InputFile), "w");
    if (fOutpt == NULL)
    {
        printf("Error: Could not open the object file to output.\n");
        return;
    }
    fprintf(fOutpt, "         ");

    fprintf(fOutpt, "%d", codeLinesAmount);
    fprintf(fOutpt, "  ");
    fprintf(fOutpt, "%d", dataLinesAmount);

    fprintf(fOutpt, "\n");

    iterations = codeLinesAmount + dataLinesAmount;
    for (size_t i = 0; i < iterations; i++)
    {
        currentDigitsAmount = countDigits(lines[i].line);

        for (size_t i = 0; i < ADDRESS_DIGITS_AMOUNT - currentDigitsAmount; i++)
        {
            fprintf(fOutpt, "0");
        }
        fprintf(fOutpt, "%d", lines[i].line + FILE_FIRST_LINE);
        fprintf(fOutpt, "    ");
             //   printf("Error: Could not open the object file to output.\n");

        binaryToHexadecimal(lines[i].binary, hexStr);
        fprintf(fOutpt, hexStr);

        if (i == iterations - 1)
        {
            continue; // to avoid a new line in the end of the file
        }
        fprintf(fOutpt, "\n");
    }
    fclose(fOutpt);
}