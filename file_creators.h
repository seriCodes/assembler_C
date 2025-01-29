#include "assembler.h"
#include "binary_code.h"

#ifndef FILE_CREATOR_H
#define FILE_CREATOR_H
#define FILE_FIRST_LINE 100
#define ADDRESS_DIGITS_AMOUNT 7

typedef struct
{
    char name[MAX_LABEL_LIMIT];
    int line;

} ImageLabelLineData;
typedef struct
{
    char binary[WORD_SIZE + 1]; //+1 for null termination
    int line;

} ImageObjectLineData;

void entFileHandler();
void sortLabelsByLine(ImageLabelLineData *labels, size_t count);
void createLabelFile(const ImageLabelLineData *labels, const char *InputFile, int labelsCounted);
void extFileHandler(nameOutputFile);
void obFileHandler(const char *outputFile);
void createObFile(const ImageObjectLineData *lines, const char *InputFile, int codeLinesAmount, int dataLinesAmount);
#endif
