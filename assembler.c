#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "assembler.h"
#define LIMIT_OF_CHARACTERSOF_INPUT_FILES 100
#define MAX_ARGS 64 // Maximum number of arguments
static const char *preassembler = "./preassembler";
static const char *first_process = "./first_process";
static const char *secondTransform = "./secondTransform";

int main(int argc, char *argv[])
{
    // char *bbb[] = {"./preassembler", "pre_asm_input_2", NULL}; // Arguments array

    char *fileNames[MAX_ARGS];

    memset(fileNames, NULL, MAX_ARGS);
    fileNames[0] = preassembler;

    int i = 1;
    for (; i < argc && i < MAX_ARGS - 1; i++)
    {
        printf("main assembler c\n");

        printf("Argument command line %d: %s\n", i, argv[i]);
        printf("Argument command line *(argv + i) %d: %s\n", i, (*(argv + i)));
        fileNames[i] = argv[i];
        printf("fileNames[%d]: %s\n", i, fileNames[i]);

        // fileNames[i] = *(argv + i);
        // strcat(fileNames[i], *(argv + i));
        //   strcat(fileNames[i], " ");
    }
    // fileNames[i] = NULL;
    main_preassembler(argc, fileNames);
    main_first_transformation(argc, fileNames);

    /*printf("Arguments passed to execvp:\n");
    for (int j = 0; fileNames[j] != NULL; j++)
    {
        printf("for execvp:\n");

        printf("fileNames[%d]: %s\n", j, fileNames[j]);
    }*/

    // printf("fileNames: %s\n", fileNames);
    //   perror("execvp failed");
    //  exit(1);
    // preassembler child process

    printf("finished  assembler \n");
    // execvp(preassembler, fileNames); // Replace child process with `preassembler`

    return 0;
}