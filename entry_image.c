#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "directives.h"
#include "binary_code.h"

#define MAX_ENTRY_LABELS 200

static int labelEntryCounter = 0;


char **getEntryLabels()
{
    static char *EntryLabels[MAX_ENTRY_LABELS]; // Array of double pointers to track allocations and create Entry file image
    return EntryLabels;
}
int getLabelEntryCounter()
{
    return labelEntryCounter;
}

char **createEntrylLabelDoublePointer(int size)
{
    char **str_pointers_arr;
    printf("createEntrylLabelDoublePointer 2\n");
    char **ptr = (char **)malloc(sizeof(char *));
    if (!ptr)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    *ptr = (char *)malloc(size * sizeof(char));
    if (!*ptr)
    {
        fprintf(stderr, "Memory allocation for EntryLabels failed\n");
        free(ptr);
        exit(1);
    }
    if (labelEntryCounter >= MAX_ENTRY_LABELS)
    {
        fprintf(stderr, "Too many allocations of EntryLabels labels\n");
        exit(1);
    }
    if (ptr)
    {
        memset(*ptr, 0, size);
        printf("EntryLabels if (ptr)\n");
        str_pointers_arr = getEntryLabels();
        str_pointers_arr[labelEntryCounter++] = *ptr;
    }
    printf("EntryLabels end\n");

    return ptr;
}

void printEntryLabels()
{
    printf("printEntryLabels  \n");
    if (labelEntryCounter == 0)
    {
        printf("printEntryLabels empty \n");
    }
    char **str_pointers_arr = getEntryLabels();

    for (size_t i = 0; i < labelEntryCounter; i++)
    {
        printf(" EntryLabels: %s \n", str_pointers_arr[i]);
    }
}
void freeEntryLabels()
{
    // printf("freeEntryLabels labelEntryCounter= %d \n", labelEntryCounter);
    char **str_pointers_arr = getEntryLabels();
    for (size_t i = labelEntryCounter; 0 < labelEntryCounter; i++)
    {
        // printf(" freeEntryLabels: %s \n", str_pointers_arr[labelEntryCounter - 1]);
        free(str_pointers_arr[labelEntryCounter - 1]);
        labelEntryCounter--;
    }
    // printf("freeEntryLabels str_pointers_arr= %p \n", str_pointers_arr);
}