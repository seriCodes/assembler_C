#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "directives.h"
#include "binary_code.h"

#define MAX_EXTERN_LABELS 200

 static int labelExternCounter = 0;

int getLabelExternCounter()
{
    return labelExternCounter;
}
char **getExternLabels()
{
    static char *ExternLabels[MAX_EXTERN_LABELS]; // Array of double pointers to track allocations and create extern file image
    return ExternLabels;
}
char **createExternalLabelDoublePointer(int size)
{
    char **str_pointers_arr;
    printf("createExternalLabelDoublePointer 2\n");
    char **ptr = (char **)malloc(sizeof(char *));
    if (!ptr)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    *ptr = (char *)malloc(size * sizeof(char));
    if (!*ptr)
    {
        fprintf(stderr, "Memory allocation for ExternLabels failed\n");
        free(ptr);
        exit(1);
    }
    if (labelExternCounter >= MAX_EXTERN_LABELS)
    {
        fprintf(stderr, "Too many allocations of ExternLabels labels\n");
        exit(1);
    }
    if (ptr)
    {
        memset(*ptr, 0, size);
        printf("ExternLabels pointer count labelExternCounter= %d\n", labelExternCounter);
        str_pointers_arr = getExternLabels();
        str_pointers_arr[labelExternCounter++] = *ptr;
        printf("ExternLabels pointer count labelExternCounter after labelExternCounter++= %d\n", labelExternCounter);
    }
    printf("ExternLabels end\n");

    return ptr;
}
void printExternLabels()
{
    printf("printExternLabels  \n");
    if (labelExternCounter == 0)
    {
        printf("printExternLabels empty \n");
    }
    char **str_pointers_arr = getExternLabels();

    for (size_t i = 0; i < labelExternCounter; i++)
    {
        printf(" ExternLabels: %s \n", str_pointers_arr[i]);
    }
}
void freeExternLabels()
{
    printf("freeExternLabels labelExternCounter= %d \n", labelExternCounter);
    char **str_pointers_arr = getExternLabels();
    for (size_t i = labelExternCounter; 0 < labelExternCounter; i++)
    {
        printf(" freeExternLabels: %s \n", str_pointers_arr[labelExternCounter - 1]);
        free(str_pointers_arr[labelExternCounter - 1]);
        labelExternCounter--;
    }
    printf("freeExternLabels str_pointers_arr= %p \n", str_pointers_arr);
}