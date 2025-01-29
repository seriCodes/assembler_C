#include "assembler.h"

// this file handles the line_labels in a single line: multiple or single
static char **line_labels;
#define MAX_LABEL_LIMIT_IN_LINE 10
// #define MAX_LABEL_LIMIT_IN_FILE 100
// Getter function to retrieve the value
char **getLabels()
{
    return line_labels;
}

// Setter function to set the value
void setLabel(char *value)
{
    for (int k = 0; k < MAX_LABEL_LIMIT_IN_LINE; k++)
    {
       // printf("setLabel( loop %s \n", value);

        if (line_labels[k][0] == '\0')
        { // char array is empty
            strncpy(line_labels[k], value, MAX_LABEL_LIMIT + 1);
            // printf("setLabel(char value %s \n", value);
            return;
        }
    }
}

void initLabels()
{
    //  printf("initLabels \n");

    line_labels = (char **)calloc(MAX_LABEL_LIMIT_IN_LINE, sizeof(char *));
    for (int k = 0; k < MAX_LABEL_LIMIT_IN_LINE; k++)
    {
        // printf("initline_labels loop\n");
        line_labels[k] = (char *)calloc((MAX_LABEL_LIMIT + 1), sizeof(char));
    }
}
void reset_line_labels_elements()
{
    //  printf("initline_labels loop OUT\n");

    for (int k = 0; k < MAX_LABEL_LIMIT_IN_LINE; k++)
    {
        //     printf("initline_labels loop\n");
        line_labels[k] = (char *)calloc((MAX_LABEL_LIMIT + 1), sizeof(char));
    }
}

void freeLineLabels()
{
    for (int i = 0; i < MAX_LABEL_LIMIT_IN_LINE; i++)
    {
        if (line_labels[i])
        {
            //  printf("line_labels[i] %p \n", line_labels[i]);

            free(line_labels[i]);
            line_labels[i] = NULL;
            //   updateAllocCount(-1);
        }
        //  printf("freeline_labels loop \n");
    }
    if (line_labels)
    {
        //  printf("free(line_labels %p \n", line_labels);

        free(line_labels);
        // updateAllocCount(-1);
    }
}

void printLabels()
{
    for (int k = 0; k < MAX_LABEL_LIMIT_IN_LINE; k++)
    {
        //   printf("printLabels loop   \n");

        if (line_labels[k][0] == '\0')
        { // char array is empty
            return;
        }
       // printf("line_labels value %s \n", line_labels[k]);
    }
}
void resetLineLabels()
{
    freeLineLabels();
    initLabels();
}
