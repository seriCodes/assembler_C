#include "assembler.h"
static void *labelsDataPointers[MAX_SYMBOLS_ALLOCATIONS] = {0}; // Array to track allocations
static int labelsDataAttributesCounter = 0;
// this file handles the symbol table to forward the symbols from first process to second process
static LabelData *symbol_table;
static int labelsFileCounter = 0;

#define MAX_LABEL_LIMIT_IN_LINE 10
// #define MAX_LABEL_LIMIT_IN_FILE 100
int getLabelsFileCounter()
{
    return labelsFileCounter;
}
LabelData *getSymbol_table()
{
    return symbol_table;
}
Bool isDuplicatedLabel(char *stringAccumulator)
{ // duplicate label error is relavent only cross lines.
  // printf("isDuplicatedLabel: %s\n", stringAccumulator);
    char copy[MAX_LABEL_LIMIT];

    char error[MAX_ARG_STRING_LENGTH] = {0};
    strncpy(copy, stringAccumulator, MAX_LABEL_LIMIT - 1);
    copy[MAX_LABEL_LIMIT - 1] = '\0';           // Ensure null termination
    copy[strlen(stringAccumulator) - 1] = '\0'; // erasing the ":" character
    // printf("isDuplicatedLabel copy  %s\n", copy);
    for (size_t i = 0; i < labelsFileCounter; i++)
    {
        if (strcmp(symbol_table[i].name, copy /*stringAccumulator*/) == 0)
        {
            updateShouldNotCreateFile(true, false);
            setStartErrorReport("");
            setLineErrorReport();
            getErrorReportCopy(error);
            strcat(error, " duplicated Label found: ");
            strcat(error, copy);

            printf(" %s\n", error);
            return true;
        }
    }

    return false;
}

void setSymbol_table(int *ic_or_dc, Directive dirType, int labelsLineSize) // if dirType is not:data, string, entry, externDirective, so the default is code
{

    char **currenLineLabels = getLabels();
    // printf("currenLineLabels = labelsLineSize %d \n", labelsLineSize);

    for (size_t i = 0; i < labelsLineSize; i++)
    {
        printf("currenLineLabels = getLabels %s \n", currenLineLabels[i]);
    }

    int index = labelsLineSize - 1;
    //  printf("setSymbol_table( line_labels %p \n", line_labels);
    //   printf("setSymbol_table( ic_or_dc %d \n", *ic_or_dc);
    //  printf("setSymbol_table address start: null only in the first call: %p \n", symbol_table);
    // printf("setSymbol_table(labelsLineSize  %d \n", labelsLineSize);

    // printf("setSymbol_table(labelsLineSize * sizeof(LabelData *) %d \n", labelsLineSize);
    //  int a= getLabelsFileCounter();
    // printf("setSymbol_table(labelsLineSize * sizeof(labelsFileCounter) %d \n", labelsFileCounter);
    // printf("symbol_table p %p \n", symbol_table);

    symbol_table = (LabelData *)realloc(symbol_table, (labelsFileCounter + labelsLineSize) * sizeof(LabelData));
    // printf("sizeof(allocationsSymbols) %d \n", sizeof(labelsDataPointers));

    //  printf("setSymbol_table( p after %p \n", symbol_table);

    int i = 0;
    //  printf("labelsFileCounter  %d\n", labelsFileCounter);

    for (int k = labelsFileCounter; k < (labelsFileCounter + labelsLineSize); k++, i++)
    {
        //  printf("setSymbol_table( loop name  %s \n", line_labels[k]);
        //    printf("setSymbol_table( k %d \n", k);
        // symbol_table[k] = (LabelData *)malloc_wrapper(sizeof(LabelData));

        symbol_table[k].ic_or_dc = (int *)calloc(1, sizeof(int));
        labelsDataPointers[labelsDataAttributesCounter] = symbol_table[k].ic_or_dc;
        labelsDataAttributesCounter++;
        symbol_table[k].attrib = (int *)calloc(1, sizeof(int));
        labelsDataPointers[labelsDataAttributesCounter] = symbol_table[k].attrib;
        labelsDataAttributesCounter++;
        symbol_table[k].name = (char *)calloc(MAX_LABEL_LIMIT, sizeof(char));
        labelsDataPointers[labelsDataAttributesCounter] = symbol_table[k].name;
        labelsDataAttributesCounter++;

        // printf("after symbol_table[k].ic_or_dc malloc_wrapper %s \n", line_labels[k]);

        // symbol_table[k].ic_or_dc = *ic_or_dc;
        // printf("ic_or_dc address  %p\n", ic_or_dc);
        // symbol_table[k].name = currenLineLabels[i];
        // printf("symbol_table[k].name  %s\n", symbol_table[k].name);
        *(symbol_table[k].ic_or_dc) = *ic_or_dc;
        // printf("ic_or_dc address  %p\n", ic_or_dc);
        strncpy(symbol_table[k].name, currenLineLabels[i], MAX_LABEL_LIMIT - 1); // Correct: Copies the string
        symbol_table[k].name[MAX_LABEL_LIMIT - 1] = '\0';                        // Ensure null termination
        symbol_table[k].name[strlen(currenLineLabels[i]) - 1] = '\0';            // erasing the ":" character
        // printf("setSymbol_table symbol_table[k].name  %s\n", symbol_table[k].name);

        if (dirType == data || dirType == string)
        {
            *(symbol_table[k].attrib) = dataAttrib;
        }
        else if (dirType == entry)
        {
            *(symbol_table[k].attrib) = entryAttrib;
        }
        else if (dirType == externDirective)
        {
            *(symbol_table[k].attrib) = external;
        }
        else
        {
            printf("attrib = code\n");
            *(symbol_table[k].attrib) = code;
            //    printf("attrib = code after\n");
        }
    }
    // printf("labelsFileCounter bef %d\n", labelsFileCounter);
    // printf("labelsLineSize bef %d\n", labelsLineSize);
    labelsFileCounter = labelsFileCounter + labelsLineSize;
}
// Getter function to retrieve the value
void initSymbol_table()
{
    //   printf("initSymbol_table\n");
    char *name = NULL;
    //!  symbol_table = (LabelData **)malloc_wrapper_with_passed_storage(sizeof(LabelData *), allocationsSymbols);
    printf("symbol_table malloc_wrapper\n");
    // Allocate memory for the first LabelData structure
    //!  symbol_table[0] = (LabelData *)malloc_wrapper(sizeof(LabelData));

    // for (int k = 0; k < 1; k++)
    //  {
    // printf("initline_labels loop\n");

    //!   name = (char *)malloc_wrapper((MAX_LABEL_LIMIT + 1) * sizeof(char));
    // symbol_table[0]->name = (char *)malloc_wrapper((MAX_LABEL_LIMIT + 1) * sizeof(char));
    printf("symbol_table bef symbol_table)[0].name = name\n");
    //!  symbol_table[0]->name = name;
    // (*symbol_table)[0].name = name;
    printf("symbol_table name\n");

    //!  symbol_table[0]->ic = (int *)malloc_wrapper(sizeof(int));
    printf("symbol_table ic\n");

    //!   symbol_table[0]->attrib = (LabelAttribute *)malloc_wrapper(sizeof(LabelAttribute));
    printf("symbol_table attrib\n");

    //  }
}
// Setter function to set the value
void printSymbolTable()
{
    printf(" Symbol table START.2\n");

    if (symbol_table == NULL)
    {
        printf("Symbol table is empty.\n");
        return;
    }

    for (int i = 0; i < labelsFileCounter; i++)
    {
        // if (symbol_table[i] == NULL)
        // {
        //     printf("Symbol table[%d] is NULL.\n", i);
        //     continue;
        // }

        printf("Symbol table[%d]:\n", i);

        // Print name
        if (symbol_table[i].name != NULL)
        {
            printf("Name: %s\n", symbol_table[i].name);
        }
        else
        {
            printf("  Name: (NULL)\n");
        }

        // Print ic_or_dc
        // if (symbol_table[i]->ic_or_dc != NULL)
        // {
        printf(" ic_or_dc: %d\n", *(symbol_table[i].ic_or_dc));
        // }
        // else
        // {
        // printf("  ic_or_dc: (NULL)\n");
        // }

        // Print attrib (assuming LabelAttribute is printable or NULL-safe)
        if (symbol_table[i].attrib != NULL)
        {

            printf("Attribute:          %s\n", *(symbol_table[i].attrib) == 3 ? "code" : *(symbol_table[i].attrib) == 4 ? "data"
                                                                                     : *(symbol_table[i].attrib) == 2   ? "entry"
                                                                                                                        : "extern"); // should not print extern or entry
        }
        else
        {
            printf("Attribute: (NULL)\n");
        }
    }
}
void freeSymbolTableAndLabelsDataPointers()
{
    freeLabelsDataPointers();
    freeSymbol_table();
    labelsFileCounter = 0;
}
void freeSymbol_table()
{
    free(symbol_table);
    symbol_table = NULL;
}
void freeLabelsDataPointers()
{
    for (int i = 0; i < labelsDataAttributesCounter; i++)
    {
        if (labelsDataPointers[i])
        {
            //   printf(" labelsDataPointers[i] %d\n", i);

            free(labelsDataPointers[i]);
            labelsDataPointers[i] = NULL;
        }
    }
    labelsDataAttributesCounter = 0;
}
