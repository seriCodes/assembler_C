#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_OPERNDS_LABELS 200
#define MAX_OPERNDS_LABELS_POINTERS 100

static char *operandLabelsPointers[MAX_OPERNDS_LABELS_POINTERS] = {0};
static char *operandLabels[MAX_OPERNDS_LABELS] = {0}; // Array to track allocations
static int labelOperandCounter = 0;
static int labelOperandPointersCounter = 0;

char **addOperandLabels(int size)
{
    // printf("addOperandLabels\n");
    char **ptr = (char **)malloc(sizeof(char *));
    if (!ptr)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    // printf("addOperandLabels size = %d\n", size);

    *ptr = (char *)malloc(size * sizeof(char));
    if (!*ptr)
    {
        fprintf(stderr, "Memory allocation for label failed\n");
        free(ptr);
        exit(1);
    }
    if (labelOperandCounter >= MAX_OPERNDS_LABELS)
    {
        fprintf(stderr, "Too many allocations of operand labels\n");
        exit(1);
    }
    if (ptr)
    {
        memset(*ptr, 0, size);
        // printf("addOperandLabels if (ptr)\n");

        operandLabels[labelOperandCounter++] = *ptr;
        operandLabelsPointers[labelOperandPointersCounter++] = ptr;
    }
    // printf("addOperandLabels end\n");

    return ptr;
}
// call between files
void resetLabelOperandCounterAndFreeLabelOperandCounter()
{
    // printf("resetLabelOperandCounterAndFreeLabelOperandCounter\n");
    //   printf("labelOperandCounter = %d\n", labelOperandCounter);

    for (size_t i = 0; i < labelOperandCounter; i++)
    {
        // printf("free of operandLabels i: %d : %s \n", i, (operandLabels[i]));

        // free(operandLabels[i]);
        // operandLabels[i] = NULL;
        // printf("end iteration i: %d \n", i);
        if (operandLabels[i] != NULL)
        {
            //   printf("Freeing operandLabels[%zu]: %s\n", i, operandLabels[i]);
            free(operandLabels[i]);  // Free dynamically allocated memory
            operandLabels[i] = NULL; // Avoid dangling pointer
        }
        else
        {
            //    printf("Skipping operandLabels[%zu] as it is NULL\n", i);
        }
        // printf("End iteration i: %zu\n", i);
    }

    // printf("end resetLabelOperandCounterAndFreeLabelOperandCounter\n");

    labelOperandCounter = 0;     
    //  printf("labelOperandPointersCounter %d\n",labelOperandPointersCounter);

    for (size_t i = 0; i < labelOperandPointersCounter; i++)
    {
        if (operandLabelsPointers[i] != NULL)
        {
            // printf("Freeing labelOperandPointersCounter[%zu]: %p\n", i, operandLabelsPointers[i]);
            free(operandLabelsPointers[i]);  // Free dynamically allocated memory
            operandLabelsPointers[i] = NULL; // Avoid dangling pointer
        }
    }
    labelOperandPointersCounter = 0;
}
void printOperandLabels()
{
    printf("printOperandLabels\n");
    printf("labelOperandCounter = %d\n", labelOperandCounter);

    for (size_t i = 0; i < labelOperandCounter; i++)
    {
        printf("operandLabels i: %d : %s \n", i, (operandLabels[i]));
    }
}
