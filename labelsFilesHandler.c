#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assembler.h"
#include "binary_code.h"
#include "file_creators.h"

void entFileHandler(const char *InputFile)
{
    ImageLabelLineData *labelsData;
    char **EntryLabels;
    int labelEntryCounter = getLabelEntryCounter();
    int labelsFileCounter = getLabelsFileCounter();
    LabelData *symbol_table = getSymbol_table();
    int line, icf;
    printf("  entFileHandler\n");
    printf("  entFileHandler labelEntryCounter %d \n", labelEntryCounter);

    EntryLabels = getEntryLabels();

    if (getShouldNotCreateFile() || !labelEntryCounter) //
    {
        printf("getShouldNotCreateFile  entFileHandler\n");
        return;
    }
    labelsData = (ImageLabelLineData *)malloc(labelEntryCounter * sizeof(ImageLabelLineData));

    if (!labelsData)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    icf = *getIc();
    printf("entFileHandler icf= %d\n", icf);

    for (size_t i = 0; i < labelEntryCounter; i++)
    {
        printf("for entFileHandler\n");
        printf("EntryLabels[i] %s\n", (*(EntryLabels + i)));
        for (size_t j = 0; j < labelsFileCounter; j++)
        {
            printf("for labelsFileCounter\n");
            printf("symbol_table[j] %s\n", symbol_table[j].name);
            if (strcmp(symbol_table[j].name, *(EntryLabels + i)) == 0)
            {
                printf("symbol_table[j].ic_or_dc %d\n", *symbol_table[j].ic_or_dc);

                line = (*symbol_table[j].ic_or_dc) + FILE_FIRST_LINE;
                printf("symbol_table[j].attrib =%d\n", *symbol_table[j].attrib);

                if ((*symbol_table[j].attrib) == dataAttrib)
                {
                    printf("symbol_table[j].attrib == dataAttrib\n");

                    line = line + icf;
                }
                strncpy(labelsData[i].name, *(EntryLabels + i), MAX_LABEL_LIMIT - 1);
                labelsData[i].name[MAX_LABEL_LIMIT - 1] = '\0'; // Ensure null termination

                // labelsData[i].name = *(EntryLabels + i);
                labelsData[i].line = line;
                printf("symbol_table[j] line %d\n", line);
                printf("labelsData[i].name %s\n", labelsData[i].name);
                printf("labelsData[i].line %d\n", labelsData[i].line);
            }
        }
    }
    printf("entFileHandler end \n");
    printImageLabelArray(labelsData, labelEntryCounter);
    sortLabelsByLine(labelsData, labelEntryCounter);
    printImageLabelArray(labelsData, labelEntryCounter);
    printf("entFileHandler InputFile %s \n", InputFile);

    createLabelFile(labelsData, InputFile, labelEntryCounter);
    free(labelsData);
}

void extFileHandler(const char *OutPutFile)
{
    printf("extFileHandler\n");

    ImageLabelLineData *labelsData;
    char **ExternLabels;

    //    int currentIc = getIc() - 1;
    //  LabelData *symbol_table = getSymbol_table();
    int line, icf, externLabelsCounter;
    ExternLabels = getExternLabels();
    externLabelsCounter = getLabelExternCounter();
    icf = *getIc();
    int labelsDataIndex = 0;
    CodeLine **codeImage = getCodeImage();
    printf("externLabelsCounter %d\n", externLabelsCounter);
    if (getShouldNotCreateFile()) //
    {
        printf("getShouldNotCreateFile  extFileHandler\n");
        return;
    }
    if (!externLabelsCounter)
    {
        printf("No external labels directive\n");
        return;
    }
    if ((*codeImage) == NULL)
    {
        printf("Code image is empty., can not print ext file\n");
        return;
    }
    labelsData = (ImageLabelLineData *)malloc((icf - 1) * sizeof(ImageLabelLineData));

    if (!labelsData)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    printf("ExternLabels icf %d\n", icf);

    for (size_t i = 0; i < externLabelsCounter; i++)
    {
        printf("*(ExternLabels: extFileHandler %s\n", *(ExternLabels + i));

        for (size_t j = 0; j < icf; j++)
        {
            if ((*codeImage)[j].type == 1)
            {
                /* Label */
                printf("  Type: Label/Immediate\n");
                if ((*codeImage)[j].data.labelOrImmediateLine.isLabel)
                {
                    printf("labelName: extFileHandler %s\n", *(*codeImage)[j].data.labelOrImmediateLine.nameOrImmediate.labelName);
                    if (strcmp(*(ExternLabels + i), *(*codeImage)[j].data.labelOrImmediateLine.nameOrImmediate.labelName) == 0)
                    {
                        printf("match extFileHandler  \n");
                        printf("match extFileHandler (*codeImage)[j].ic %d \n", (*codeImage)[j].ic);

                        line = ((*codeImage)[j].ic) + FILE_FIRST_LINE;
                        strncpy(labelsData[labelsDataIndex].name, *(*codeImage)[j].data.labelOrImmediateLine.nameOrImmediate.labelName, MAX_LABEL_LIMIT - 1);
                        labelsData[labelsDataIndex].name[MAX_LABEL_LIMIT - 1] = '\0'; // Ensure null termination

                        // labelsData[i].name = *(EntryLabels + i);
                        labelsData[labelsDataIndex].line = line;
                        printf("symbol_table[j] line %d\n", line);
                        printf("labelsData[labelsDataIndex].name %s\n", labelsData[labelsDataIndex].name);
                        printf("labelsData[labelsDataIndex].line %d\n", labelsData[labelsDataIndex].line);
                        labelsDataIndex++;
                    }
                }
            }
        }
    }
    printImageLabelArray(labelsData, labelsDataIndex);
    sortLabelsByLine(labelsData, labelsDataIndex);
    printImageLabelArray(labelsData, labelsDataIndex);
    printf("entFileHandler InputFile %s \n", OutPutFile);

    createLabelFile(labelsData, OutPutFile, labelsDataIndex);
    free(labelsData);
}
void printImageLabelArray(const ImageLabelLineData *labels, size_t count)
{
    printf("printImageLabelArray\n");

    if (labels == NULL || count == 0)
    {
        printf("No labels to display.\n");
        return;
    }

    printf("ImageLabelLineData Array:\n");
    for (size_t i = 0; i < count; i++)
    {
        printf("Index %zu: Name = %s, Line = %d\n", i, labels[i].name, labels[i].line);
    }
}
