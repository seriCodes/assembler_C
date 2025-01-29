#include <stdlib.h>
#include "assembler.h"
#include "binary_data.h"
static int data_alloc_count = 0;
int getDataImageAllocCount()
{
    return data_alloc_count;
}
void resetDataImage()
{
    DataBinaryLine **ptr = getDataImage();
    *ptr = NULL;
}
DataBinaryLine **getDataImage()
{
    static DataBinaryLine *dataImage = {0};
    // printf(" getDataImage dataImage %p  \n", dataImage);
    // printf(" getDataImage &dataImage %p  \n", &dataImage);

    return &dataImage;
}
DataBinaryLine *initDataImage(int size)
{
    printf(" initDataImage size %d  \n", size);

    DataBinaryLine *dataImage_Ptr = (DataBinaryLine *)malloc(size * sizeof(DataBinaryLine));
    if (dataImage_Ptr)
    {
        memset(dataImage_Ptr, 0, 1);
    }
    data_alloc_count = data_alloc_count + size;
    return dataImage_Ptr;
}
void storeIntegersInImageDataArray(int data[])
{
    int *currentFileDataCounter;
    int dataIndex;

    int *dc;
    int *integersAmount;
    DataBinaryLine **ptr = getDataImage();

    // printf(" storeIntegersInImageDataArray  \n");
    // printf(" storeIntegersInImageDataArray *ptr %p \n", *ptr);

    currentFileDataCounter = getCurrentFileDataCounter();
    integersAmount = getCurrentLineDataCounter();
    // printf(" reallocDataImage bef %p  \n", ptr);
    // printf(" reallocDataImage bef *ptr %p  \n", *ptr);

    *ptr = reallocDataImage(*ptr, *integersAmount, data);
    // printf(" reallocDataImage after %p  \n", ptr);
    // printf(" reallocDataImage after *ptr %p  \n", *ptr);

    dataIndex = 0;
    // printf(" storeIntegersInImageDataArray currentFileDataCounter %d \n", *currentFileDataCounter);
    // printf(" storeIntegersInImageDataArray integersAmount %d \n", *integersAmount);

    for (size_t i = *currentFileDataCounter; i < *currentFileDataCounter + *integersAmount; i++)
    {
        // printf(" storeIntegersInImageDataArray loop data[dataIndex] %d \n", data[dataIndex]);
        dc = getDc();
        // printf(" storeIntegersInImageDataArray loop dc %d\n", *dc);

        (*ptr)[i].type = 1;
        // printf(" storeIntegersInImageDataArray loop ptr)[i].type\n");

        (*ptr)[i].dc = *dc;
        // printf(" storeIntegersInImageDataArray intOrChar.size\n");

        (*ptr)[i].intOrChar.size = data[dataIndex];
        updateDc(1);
        dataIndex++;
    }

    // printIntegers(data);
    updateCurrentFileDataCounter(*integersAmount);

    resetCurrentLineDataCounter();
}

void storeCharsInImageDataArray(char *data, int len)
{
    int *currentFileDataCounter;
    int dataIndex;

    int *dc;
    DataBinaryLine **ptr = getDataImage();

    // printf(" storeCharsInImageDataArray \n");
    // printf(" storeCharsInImageDataArray *ptr %p \n", *ptr);

    currentFileDataCounter = getCurrentFileDataCounter();
    // printf(" reallocDataImage bef %p  \n", ptr);
    // printf(" reallocDataImage bef *ptr %p  \n", *ptr);

    *ptr = reallocDataImage(*ptr, len, data);
    // printf(" reallocDataImage after %p  \n", ptr);
    // printf(" reallocDataImage after *ptr %p  \n", *ptr);

    dataIndex = 0;
    // printf(" storeIntegersInImageDataArray currentFileDataCounter %d \n", *currentFileDataCounter);

    for (size_t i = *currentFileDataCounter; i < *currentFileDataCounter + len; i++)
    {
        // printf(" storeIntegersInImageDataArray (*ptr)[i].intOrChar.c start %c \n", (*ptr)[i].intOrChar.c);
        // printf(" storeIntegersInImageDataArray loop data[dataIndex] %c \n", data[dataIndex]);
        dc = getDc();
        // printf(" storeIntegersInImageDataArray loop dc %d\n", *dc);

        (*ptr)[i].type = 0;
        // printf(" storeIntegersInImageDataArray loop ptr)[i].type\n");

        (*ptr)[i].dc = *dc;
        // printf(" storeIntegersInImageDataArray intOrChar.size\n");
        if (i == (*currentFileDataCounter) + len - 1)
        {
            (*ptr)[i].intOrChar.c = '\0';
        }
        else
        {

            (*ptr)[i].intOrChar.c = data[dataIndex];
        }

        updateDc(1);
        dataIndex++;
                // printf(" storeIntegersInImageDataArray (*ptr)[i].intOrChar.c end %c \n", (*ptr)[i].intOrChar.c);

    }

    // printIntegers(data);
    updateCurrentFileDataCounter(len);

    // resetCurrentLineDataCounter();
}
void freeDataImage()
{
    // printf("freeDataImage data_alloc_count start %d.\n", data_alloc_count);

    DataBinaryLine **dataImage = getDataImage();
    if ((*dataImage) == NULL)
    {
        printf("data image is empty.\n");
        return;
    }
    // printf("dataImage after if ((*codeImage) == NULL.\n");
    free(*dataImage); // *dataImage this is a pointer to multiple elements of DataBinaryLine in the heap
                      //! temp
    DataBinaryLine **ptr = getDataImage();
    printf(" freeDataImage storeIntegersInImageDataArray  ptr bef %p \n", ptr);
    printf(" freeDataImage storeIntegersInImageDataArray  *ptr bef %p \n", *ptr);

    resetDataImage(); //! not tmp
    ptr = getDataImage();
    data_alloc_count = 0;
    //! temp
    // printf(" freeDataImage storeIntegersInImageDataArray ptr bef %p \n", ptr);

    // printf("freeDataImage storeIntegersInImageDataArray *ptr %p \n", *ptr);
}
DataBinaryLine *reallocDataImage(DataBinaryLine *dataImage_ptr, int size, int data[])
{

    // printf("reallocDataImage\n");
    // printf("dataImage_ptr beff %p\n", dataImage_ptr);
    // printf("reallocDataImage size %d\n", size);

    if (dataImage_ptr == NULL)
    {
        return initDataImage(size);
    }

    // printf("dataImage_ptr %p\n", dataImage_ptr);

    // printf("dataImage_ptr data_alloc_count= %d!!\n", data_alloc_count);
    // printf("sizeof(DataBinaryLine)= %d!!\n", sizeof(DataBinaryLine));

    dataImage_ptr = (DataBinaryLine *)realloc(dataImage_ptr, (data_alloc_count + size) * sizeof(DataBinaryLine));

    // printf("dataImage_ptr afte!!\n");

    data_alloc_count = data_alloc_count + size;
    if (!dataImage_ptr)
    {
        fprintf(stderr, "Failed to realloc memory for data image!!\n");
    }
    return dataImage_ptr;
}

int *getCurrentFileDataCounter()
{
    static int fileCounter = 0;
    static int *fileCounterPointer;
    fileCounterPointer = &fileCounter;
    return fileCounterPointer;
}
void updateCurrentFileDataCounter(int s)
{
    int *counter = getCurrentFileDataCounter();
    // printf("updateCurrentFileDataCounter  counter %d\n", *counter);

    *counter = (*counter) + s;
    // printf("updateCurrentFileDataCounter  *counter = *counter + s %d\n", *counter);
}
void resetCurrentFileDataCounter()
{
    int *counter = getCurrentFileDataCounter();
    *counter = 0;
}
// these 3 methods below handle the counter for integers and characters for both directives: .data and . string. counting current line parameter. //! it might be that .string directive does not need it. validation needed
int *getCurrentLineDataCounter()
{
    static counter = 0;
    return &counter;
}
void updateCurrentLineDataCounter(int s)
{
    int *counter = getCurrentLineDataCounter();
    *counter = *counter + s;
}
void resetCurrentLineDataCounter()
{
    int *counter = getCurrentLineDataCounter();
    *counter = 0;
}

void parseStringToIntegers(const char *str, int *array)
{
    int index = 0;

    const char *start = str;

    while (*start != '\0' && index < MAX_INTEGERS_IN_LiNE)
    {
        // Find the end of the current number
        char *end;
        long num = strtol(start, &end, 10); // Convert substring to long integer

        // Check if conversion succeeded and if within integer range
        if (start != end)
        {
            array[index++] = (int)num;
            updateCurrentLineDataCounter(1);
        }
        // Move start to the next number (skip comma)
        start = end;
        if (*start == ',')
        {
            start++;
        }
    }
}
void printIntegers(int *array)
{
    printf("printIntegers\n");
    int *counter = getCurrentLineDataCounter();
    printf("printIntegers counter %d\n", *counter);

    for (size_t i = 0; i < *counter; i++)
    {
        printf(" %d\n", array[i]);
    }
}

void printDataBinaryLine(const DataBinaryLine *data)
{
    printf("DataBinaryLine {\n");
    printf("  type: %d\n", data->type);
    printf("  dc: %d\n", data->dc);
    if (data->type == 0)
    {
        if (data->intOrChar.c == '\0')
        {
            printf("  intOrChar.c: %s\n", "\\0");
        }
        else
        {
            printf("  intOrChar.c: %c\n", data->intOrChar.c);
        }
    }
    else
    {
        printf("  intOrChar.size: %d\n", data->intOrChar.size);
    }
    printf("\n");
}

// Function to print an array of DataBinaryLine structures
void printDataBinaryLineArray(
    // const DataBinaryLine *array
)
{
    DataBinaryLine **ptr = getDataImage();
    if (*ptr == NULL)
    {
        printf("printDataBinaryLineArray ptr empty\n");

        return;
    }

    printf("printDataBinaryLineArray  :\n");
    int *integersAmount = getCurrentFileDataCounter();
    printf("getCurrentFileDataCounter %d  :\n", *integersAmount);

    for (int i = 0; i < *integersAmount; i++)
    {
        printf("Element %d:\n", i);
        printDataBinaryLine((*ptr + i));
    }
}
