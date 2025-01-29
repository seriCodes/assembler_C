#include "assembler.h"

#ifndef BINARY_DATA_H
#define BINARY_DATA_H
#define MAX_INTEGERS_IN_LiNE 40

typedef struct
{
    int type; // 0 for char, 1 for other
    int dc;
    union
    {
        int size;
        char c;
    } intOrChar;

} DataBinaryLine;
DataBinaryLine **getDataImage();
void resetDataImage();
DataBinaryLine *initDataImage();
void storeIntegersInImageDataArray();
void freeDataImage();
DataBinaryLine *reallocDataImage(DataBinaryLine *dataImage_ptr, int size, int data[]);
void parseStringToIntegers(const char *str, int *array);
void updateCurrentLineDataCounter(int s);
int *getCurrentLineDataCounter();
void resetCurrentLineDataCounter();
int *getCurrentFileDataCounter();
void updateCurrentFileDataCounter(int s);
void resetCurrentFileDataCounter();

#endif
