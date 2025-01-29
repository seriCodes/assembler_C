#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>    // for size_t
#include "assembler.h" // Include the header file
static void copyStringUntilEncounteringmcroend(FILE *fd, char **str);
static int isMcroOpeningAndClosingLinesCorrect(FILE *fd, int wordsAmount);
static void jumpOverMacroCode(FILE *fd)
{
    char buffer[80]; // Buffer for reading lines

    // printf("jumpOverMacroCode currentPos %d  ;", currentPos);
    while (fgets(buffer, sizeof(buffer), fd)) // advances the position indicator for the stream=fd
    {
        // printf("jumpOverMacroCode in  %s\n", buffer);
        removeTrailingWhitespaces(buffer);

        if (!memcmp(buffer, KeyStrings[mcroend], strlen(KeyStrings[mcroend])))
        {
            // printf("mcroend FOUND\n");
            return;
        }
        // printf("NOT mcroend  \n");
    }
}
typedef struct
{
    char *name;
    char *code;
} MacroData;

void validateLabelNamesNotMacroNames(const char *fileName, MacroData **macros, int macrosCount)
{
    int length = 100;
    // printf("\n validateLabelNamesNotMacroNames %s  \n", fileName);

    char *str[length];
    for (size_t i = 0; i < macrosCount; i++)
    {
        memset(str, '\0', length);

        // printf("\n validateLabelNamesNotMacroNames ((*macros) + i)->name, %s  \n", ((*macros) + i)->name);
        strcat(str, ((*macros) + i)->name);
        strcat(str, ":");
        //   printf("\n ::::::::::: str %s  \n", str);
        setLineNum(1);
        if (doesStringExistInFile(fileName, str))
        {
            setStartErrorReport(macroString);

            setLineErrorReport();
            char tmp[MAX_ARG_STRING_LENGTH];
            getErrorReportCopy(tmp);

            strncat(tmp, " label name can not be identical to a macro name", MAX_ARG_STRING_LENGTH);
            printf("%s\n", tmp);
            updateShouldNotCreateFile(true, false);
        }
    }
}
static void insertMacroNameAndMacroCodeToMacroTable(MacroData **macros, char **name, char **code, int macrosIndex)
{
    //  printf("*name as:%s \n", *name);
    // printf("*code as:%s , strlen(*code)=%d\n", *code, strlen(*code));
    if (*name == NULL || *code == NULL)
    {
        fprintf(stderr, "Error: Name or code pointer is NULL\n");
        exit(1);
    }
    // printCharArrayWithNulls(*code, 10);

    //  printf("insertMacroNameAndMacroCodeToMacroTable macrosIndex: %d\n", macrosIndex);

    if (macrosIndex < 0)
    {
        fprintf(stderr, "Invalid macrosIndex: %d\n", macrosIndex);
        exit(1);
    }
    // printf("Before: *name=%p, *code=%p\n", (void *)*name, (void *)*code);

    (*macros)[macrosIndex].name = *name;
    (*macros)[macrosIndex].code = *code;
    //   printf("After: macros[%d].name=%p, macros[%d].code=%p\n", macrosIndex, (void *)(*macros)[macrosIndex].name, macrosIndex, (void *)(*macros)[macrosIndex].code);

    /*printf("macrosIndex alone %d \n", macrosIndex);

    printf("(*macros)[0].name:%s \n", (*macros)[0].name);
    printf("(*macros)[0].code:%s \n", (*macros)[0].code);

    printf("(*macros)[macrosIndex].name:%s \n", (*macros)[macrosIndex].name);
    printf("(*macros)[macrosIndex].code:%s \n", (*macros)[macrosIndex].code);*/
}
static MacroData *isStringAccumulationIsMacroName(MacroData **macros, char *str, int macrosCount)
{
    //  printf("isStringAccumulationIsMacroName: macros double pointer value, which is the MacroData *  address %p \n", macros);
    // printf("isStringAccumulationIsMacroName: *macros direct pointer value, which is the MacroData *  value *macros %p \n", *macros);
    //  printf("sizeof(MacroData *) %d ", sizeof(MacroData *));

    //   printf("\n isStringAccumulationIsMacroName str %s strlen(str) %d\n", str, strlen(str));
    // printCharArrayWithNulls(str, strlen(str));

    //  macrosCount = 5;
    for (size_t i = 0; i < macrosCount; i++)
    {
        // printf("\ni is %d", i);

        // printf("\n isStringAccumulationIsMacroName *macros[%d].name=%s\n", i, (*macros)[i].name);
        //   printf("\n isStringAccumulationIsMacroName macros[%d].code=%s\n", i, (*macros)[i].code);

        //    printCharArrayWithNulls((*macros)[i].name, strlen((*macros)[i].name));
        //(macros)[i]->name false
        //   printf("\n isStringAccumulationIsMacroName ((*macros) + i)->name, %s  \n", ((*macros) + i)->name);

        // if (strlen(str) == strlen(((macros)[i]->name) && !memcmp(str, (*macros)[i].name, strlen(str)))
        if (strlen(str) == strlen(((*macros) + i)->name) && !memcmp(str, ((*macros) + i)->name, strlen(str)))
        // strlen(str) == strlen((*macros)[i].name) to prevent wrong string identification as a macro name, in case of a shorter or linger string that is similar in is characters to the other. e.g: mcroname1 and mcroname1False
        {
            printf("\n isStringAccumulationIsMacroName macros index %i\n", i);
            //  printf("\n macros)[i]->name %s\n", macros[i]->name);

            // return (*macros)[i];
            return (*macros) + i;
            //   return i;
        }
    }
    // return -1;
    return NULL;
}
static void fetchMacroCode(FILE *fd, char **str)
{
    int i = 0;
    int ch;
    ch = skipOverFollowingNewLinesAndTabsAndSpacesAndReturnCarriage(fd); // first char of the macro code
    fseek(fd, -1, SEEK_CUR);
    // printf("fetchMacroCode  *str %s \n", *str);

    copyStringUntilEncounteringmcroend(fd, str);
}
static void copyStringUntilEncounteringmcroend(FILE *fd, char **str)
{
    printf("before malloc_wrapper, *str:\n%s \n", *str);

    char *str2 = (char *)malloc(80 * sizeof(char));
    *str = (char *)malloc_wrapper(80 * sizeof(char));
    //  printf("after malloc_wrapper, *str:\n%s \n", *str);
    long savedPosition, savedPositionLineAfterMcroend;
    int i = 1;
    while (1)
    {
        *str = (char *)realloc_wrapper(*str, 80 * i * sizeof(char));
        // printf("fgets i:  %d \n", i);
        savedPosition = saveFilePosition(fd);
        fgets(str2, 80, fd); // If the line is bigger than 80 chars it will include the remainder of the line in the next iteration. It includes \n

        // lineNum++;
        updateLineNum(1);
        int lineNum = getLineNum();

        //  printf("ch == 10 lineNum copyStringUntilEncounteringmcroend: %d\n", lineNum);
        printf("fgets(str2 %s \n", str2);
        // printCharArrayWithNulls(str2, 50);
        printf("str2: copyStringUntilEncounteringmcroend %s\n", str2);

        if (!memcmp(str2, KeyStrings[mcroend], strlen(KeyStrings[mcroend])))
        {
            savedPositionLineAfterMcroend = saveFilePosition(fd);
            restoreFilePosition(fd, savedPosition);

            printf("printCharArrayWithNulls: memcmp \n%s \n", *str);
            removeTrailingWhitespaces(*str); // needed for macro spreading without a surplus new line

            printf("printCharArrayWithNulls: memcmp after removeTrailingWhitespaces(*str)\n%s \n", *str);

            isMcroOpeningAndClosingLinesCorrect(fd, 1);
            // lineNum++; // next itearion will be in the outer loop and it does not include the \n char
            updateLineNum(1);
            lineNum = getLineNum();

            restoreFilePosition(fd, savedPositionLineAfterMcroend);

            break;
        }
        if (feof(fd))
        {
            char currentLine[CURRENT_LINE];
            snprintf(currentLine, CURRENT_LINE, "%d", lineNum);
            char *fileName = setGetFileName("", false);

            printf("\nError: At file %s, at line %s: End of file reached before reading mcroend.\n", fileName, currentLine);
            updateShouldNotCreateFile(true, false);
            break;
        }
        // printf("before strncat, *str:\n%s \n", *str);

        strncat(*str, str2, 80);
        i++;
        printf("fgets *str,:\n%s ", *str);
    }
    free(str2);
}
static void preAsmOutput(char *inputFilename, FILE *fd, char *str, int counter, MacroData **macros, int macrosCount)
{
    FILE *fOutpt;
    char *strCopy;
    char strName[100] = {0};
    // strncpy(strName, "output_of_", 100 - 1);
    strcat(strName, inputFilename);
    printf("strncpy strName %s .\n", strName);

    strcat(strName, ".am");

    printf("strcat strName  %s .\n", strName);

    int isAfterAddingLineFeedManually = 0; // 0 means the last iteration did not add \n manually. Helps to prevent an extra \n when regular code is written after a macro name translation to its code.
    // Open a file in write mode
    fOutpt = fopen((strName), "w");
    if (fOutpt == NULL)
    {
        printf("Error: Could not open the file pre_asm_output for appending.\n");
        return 1; // Exit with error code
    }

    MacroData *macroData = NULL;
    int ch = NULL;
    /* fprintf(fOutpt, "32first");
    fprintf(fOutpt, "32second\n");
    fputs("thir fputs", fOutpt);
      printf("ch = fgetc(fd) out;  %c", ch);
  */
    int loopContinued = 1;
    while (loopContinued)
    {
        ch = fgetc(fd);
        // printf("ch = fgetc(fd); in  %c", ch);
        if (
            /*!isspace((unsigned char)ch)*/ // when you have the time convert it to isspace(for unknown reason it does not work)
                ch == 32 ||
            ch == 9 || ch == 10 || ch == -1 /*EOF*/)
        { // space, horizontal tab, new line and EOF detection
            if (!isOnlyWhitespaceOrNull(str))
            {

                if (counter != 0)
                {
                    //  printf("counter != 0, str %s\n", str);

                    // printCharArrayWithNulls(str, 50);
                    strCopy = copyCharArrayWithExtraSpace(str, 0);
                    printf("after strCopy = copyCharArrayWithExtraSpace ch  ;  %s\n", strCopy);

                    if (!stringAccumulationDetectionWithRemoveWhiteSpaces(strCopy, mcro))
                    // returns zero if mcro string is found
                    {
                        //         printf("stringAccumulationDetectionWithRemoveWhiteSpaces  ;  %c", ch);
                        jumpOverMacroCode(fd); // ch is currently a whitespace. will be written in the start of the next line
                        ch = fgetc(fd);        // this prevents including a space in the start  of next line.
                        printf("after jumpOverMacroCode ch  ;  %c \n", ch);
                    }
                    else
                    {

                        macroData = isStringAccumulationIsMacroName(macros, strCopy, macrosCount);
                        if (macroData != NULL) // macroData
                        {
                            // printf("\n macroIndex!=NULL isStringAccumulationIsMacroName completed  %s\n", (*macros)[macroIndex].code);
                            //     printCharArrayWithNulls(macroData->code, 33);
                            //  printf("\n macroData!=NULL isStringAccumulationIsMacroName completed macroData.code: %s\n", macroData->code);
                            // printCharArrayWithNulls((*macros)[macroIndex].code, 19);
                            // strcat(str, "\n");
                            strCopy = copyCharArrayWithExtraSpace(macroData->code, 1);
                            // strCopy = macroData->code;
                            *(strCopy + strlen(strCopy)) = '\n';
                            isAfterAddingLineFeedManually = 1;
                            //   printf("\n macroData != NULL isAfterAddingLineFeedManually: %d\n", isAfterAddingLineFeedManually);
                            fprintf(fOutpt, strCopy);

                            // writeMacroCode();
                        }
                        else
                        {
                            // fprintf(fOutpt, ch);
                            //  counter++;

                            // str[counter - 1] = ch;
                            /* if (ch == '\n')
                            {
                                printf("hexa new line \\x%02X \n", ch);
                             }*/

                            //   printf("hexa \\x%02X \n", ch);
                            /*  counter++;
                              str[counter] = '\0';*/
                            if (str[0] == 10 && !isAfterAddingLineFeedManually)
                            {
                                fprintf(fOutpt, "\n");
                            }

                            remove_all_whitespace(str);
                            // printCharArrayWithNulls(str, 15);
                            //   printf("out ch == '')\n");
                            if (ch == 10) //  \n
                            {
                                //   printf("ch == '' in )");
                                strcat(str, "\n");
                                ch = fgetc(fd);
                            }
                            else
                            {
                                strcat(str, " ");
                                ch = fgetc(fd);
                            }
                            fprintf(fOutpt, str);

                            isAfterAddingLineFeedManually = 0;
                        }
                        macroData = NULL;
                    }

                    free(strCopy);
                }
            }
            resetCharArray(str, counter);
            counter = 0;
            if (ch == -1 /*EOF*/)
            {
                break;
            }
            /*   if (!feof(fd))
               {
                   break;
               }*/
        }
        /*if (!counter && isspace(ch)) // prevents writing whitespaces in the start of the string(after mcroend in specific)
        {
            continue;
        }*/
        str[counter] = ch;
        // strTest[counter] = ch;
        counter++;
        //  printf("counter++, %d", counter);
    }

    // printf("str[counter] in preAsmOutput, %s\n", str);

    //  printf("str[counter] in preAsmOutput, \n");

    // fprintf(fOutpt, ch);
    //   fprintf(fOutpt, "ssssstr");
    //   printf("fd preAsmOutput, %p\n", fd);

    if (fOutpt != NULL)
    {
        fclose(fOutpt);
        fOutpt = NULL; // Safeguard against reuse
    }
}
static int isMcroOpeningAndClosingLinesCorrect(FILE *fd, int wordsAmount)
{ // moves pointer to the start of the current line.
  // verifies if there are only 2 words between two \n
  // printf("isMcroOpeningAndClosingLinesCorrect wordsAmount: %d\n", wordsAmount);
    // int ch = fgetc(fd);
    // printf("isMcroOpeningAndClosingLinesCorrect wordsAmount: %c\n", ch);

    long savedPosition = saveFilePosition(fd);
    FILE *fd2;
    //= fopen("pre_asm_input.txt", "r+");
    char *fileName = setGetFileName("", false);
    // printf("isMcroOpeningAndClosingLinesCorrect fileName: %s\n", fileName);

    if (!(fd2 = fopen(fileName, "r+")))
    {
        fprintf(stderr, "cannot open file at isMcroOpeningAndClosingLinesCorrect %s\n", fileName); // erase isMcroOpeningAndClosingLinesCorrect before submit
        return;
        // exit(0);
    }
    restoreFilePosition(fd2, savedPosition);

    moveFilePointerToTheStartOfTheLine(fd); // fd points to the first char in the line
    moveFilePointerToTheStartOfTheLine(fd2);
    // errorRepor = {0};
    //  printf("before setStartErrorReport \n");
    //  printf("before setStartErrorReport macroString %s\n", macroString);

    setStartErrorReport(macroString);
    if (lineRemainingWordLimit(fd, fd2, wordsAmount))
    {
        //   printf("lineRemainingWordLimit failure \n");
        // restoreFilePosition(fd, savedPosition);//No need for this here because after lineRemainingWordLimit call, fd points to the last nonewhite character in the line. In the oter loop it will skip it.
        return FAILURE;
    }

    //  ch_left = fgetc(fd2);
    //  printf("ch_left = fgetc(fd2) after after moveFilePointerToTheEdgeOfWord %c\n", (unsigned char)ch_left);
    //   ungetc(ch_left, fd2);
    restoreFilePosition(fd, savedPosition);

    if (fd2 != NULL)
    {
        fclose(fd2);
        fd2 = NULL; // Safeguard against reuse
    }

    // fclose(fd2);
    return SUCCESS;
}

static void preassembleInputFile(const char *InputFile)
{
    // printf("preassembleInputFile .\n");
    FILE *fd;
    int ch, macrosCounter = 0;
    MacroData *macros = (MacroData *)malloc_wrapper(macrosCounter * sizeof(MacroData));
    char *mcroName = NULL;
    char *mcroCode = NULL;
    // const char *filename = InputFile;
    //  printf("const char *filename.\n");
    //  printf("preassembleInputFile  %s.\n", InputFile);

    // printf("const char *filename %s.\n", filename);

    int charCounter = 0;
    char stringAccumulator[100] = {0};
    //  printf("(argv[1]) %s\n", (*argv[1]));
    if (!(fd = fopen(InputFile, "r+"))) // REMEMBER to fclose(fd);
    {
        fprintf(stderr, "cannot open file %s\n", InputFile);
        return;
        // exit(0);
    }
    int lineNum = getLineNum();

    while (!feof(fd))
    {
        ch = fgetc(fd);
        // printf("ch = fgetc(fd) %c\n", (unsigned char)ch);
        if (
            /*!isspace((unsigned char)ch)*/ // when you have the time convert it to isspace(for unknown reason it does not work)
                ch == 32 ||
            ch == 9 || ch == 10)
        { // space, horizontal tab and new line detection
            if (ch == 10)
            {
                updateLineNum(1);
                lineNum = getLineNum();
            }

            if (charCounter != 0)
            {                                                                                   // i=0 if this is before the start of a new word. ex:     myWord          mcro macroName
                if (!stringAccumulationDetectionWithRemoveWhiteSpaces(stringAccumulator, mcro)) // returns zero if mcro string is found
                {
                    macrosCounter++;
                    if (ch == 10) //\to display correct line number in case of: "mcro\nmcroend"
                    {
                        updateLineNum(-1);
                        lineNum = getLineNum();

                        fseek(fd, -1, SEEK_CUR);

                        //  printf("ch == 10 lineNum--: %d\n", lineNum);
                    }
                    if (!isMcroOpeningAndClosingLinesCorrect(fd, 2))
                    {
                        fetchNoneWhiteCharachtersString(fd, &mcroName); // fetch macro's name and forwards fd to the next whitespace

                        if (isLastCharacter(mcroName, ':') || isKeyword(mcroName))
                        {
                            printf("out resolveDirectiveType ||resolveInstructionType %s \n", mcroName);
                            setStartErrorReport(macroString);
                            setLineErrorReport();
                            char tmp[MAX_ARG_STRING_LENGTH];
                            getErrorReportCopy(tmp);

                            strncat(tmp, "name conflict. Macro name can not be a keyword and not end in ':'.", MAX_ARG_STRING_LENGTH);
                            // name or \"mcro\",
                            //
                            printf(" %s\n", tmp);

                            updateShouldNotCreateFile(true, false);
                        }
                        //  lineNum++;// probably needed to fix line numbers

                        // printf("mcro name after fetch:%s \n", mcroName);
                    }
                    else
                    {
                        // printf("\n failure at isMcroOpeningAndClosingLinesCorrect \n");
                        // printf("invalidName %s\n", invalidName);
                        mcroName = invalidName;
                        moveToNextLine(fd);
                    }
                    // Reporting on noneWhite Chars in the opening of the mcro(excluding mcro's name) and closing of the mcro. Stopping the program.
                    fetchMacroCode(fd, &mcroCode);
                    //     printf("ch == 10 lineNum after fetchMacroCode: %d\n", lineNum);
                    //  printf("mcroCode mcroCode %s\n\n", mcroCode);
                    macros = (MacroData *)realloc_wrapper(macros, macrosCounter * sizeof(MacroData));
                    if (!macros)
                    {
                        printf("memory allocation failed for macros");
                        exit(0);
                    }
                    //  printf("sizeof(MacroData) %d \n", sizeof(MacroData));

                    //  printf("macrosCounter %d\n", macrosCounter);
                    //   printf("macros point %p\n", macros);

                    insertMacroNameAndMacroCodeToMacroTable(&macros, &mcroName, &mcroCode, macrosCounter - 1);
                }
                // printf("charCounter != 0| charCounter=%d \n", charCounter);
            }
            resetCharArray(stringAccumulator, charCounter);
            charCounter = 0;

            //  printf("charCounter = 0; %d \n", charCounter);
        }
        // printf("stringAccumulator[charCounter] here %c \n", ch);
        // printf("charCounter out; %d \n", charCounter);
        stringAccumulator[charCounter] = ch;
        charCounter++;
        mcroCode = NULL;
        // printf("%c:%d, ", ch, ch);
    }
    rewind(fd);
    // to reset after EOF is met in the end of the code.
    resetCharArray(stringAccumulator, charCounter); //  100
    charCounter = 0;

    validateLabelNamesNotMacroNames(InputFile, &macros, macrosCounter);
    Bool notCreateFile = getShouldNotCreateFile();
    if (!notCreateFile)
    {
        char *originalName[MAX_STRING_LENGTH];
        copy_left_of_last_dot(InputFile, originalName);
        preAsmOutput(originalName, fd, stringAccumulator, charCounter, &macros, macrosCounter);
    }

    //  printf("before fclose(fd\n");
    // printf("fd preAsmOutput, after %p\n", fd);
    if (fd != NULL)
    {
        fclose(fd);
        fd = NULL; // Safeguard against reuse
    }

    // printf("before free all\n");

    // free(macros);
    //  printf("before : free_all call \n");
    free_all();
    // printf("Finished processing file: %s\n", InputFile);
    return 0; // success indication
}

int main_preassembler(int argc, char *argv[])
{
    //  Instruction *ins = getInstruction(16);
    //   printf(" ins  getInstruction %d.\n", ins->instructionName);
    char nameInputFile[100];
    int maxInputFileName = 100;
    for (int i = 1; i < argc; i++)
    {
        memset(nameInputFile, '\0', maxInputFileName);
        printf("main_preassembler \n");

        printf("Argument command line %d: %s\n", i, argv[i]);
        printf("Argument command line *(argv + i) %d: %s\n", i, (*(argv + i)));
        updateShouldNotCreateFile(false, true);
        if (i > 0)
        {
            //  printCharArrayWithNulls(*(argv + i), 33);

            strcat(nameInputFile, *(argv + i));
            // printCharArrayWithNulls(nameInputFile, 33);
            strcat(nameInputFile, ".asm");
            printf("after strcat(nameInputFile,..asm\n");

            // printCharArrayWithNulls(nameInputFile, 33);

            //  printf("nameInputFile %s.\n", nameInputFile);

            setGetFileName(nameInputFile, true);
            //  char *tmp = setGetFileName("", false);

            //  printf("main setGetFileName  %s.\n", tmp);

            //  removesConsecutiveWhitspaecesFromStartOfFile(*(argv + i));
            removesConsecutiveWhitspaecesFromStartOfFile(nameInputFile);

            // removesConsecutiveWhitspaecesFromEndOfFile(*(argv + i));
            removesConsecutiveWhitspaecesFromEndOfFile(nameInputFile);

            //!  removesConsecutiveWhitspacesWithInLine(*(argv + i));
            removesConsecutiveWhitspacesWithInLine(nameInputFile);

            //!  removesConsecutiveWhitspacesCrossLines(*(argv + i));

            removesConsecutiveWhitspacesCrossLines(nameInputFile);

            // preassembleInputFile((*(argv + i)));
            preassembleInputFile(nameInputFile);

            //! check if below is futile
            // Bool notCreateFile = getShouldNotCreateFile();

            // if (!notCreateFile)
            // {
            //     printf("!notCreateFile after preassembleInputFile %d.\n", notCreateFile);
            //     char copy[50];
            //     strcpy(copy, *(argv + i));
            //     strcat(copy, ".ok");
            //     FILE *flagFile = fopen(copy, "w");
            //     if (flagFile)
            //         fclose(flagFile);
            // }

            // printf("notCreateFile out  %d  .\n", notCreateFile);
            //!
        }
        setLineNum(1);
    }

    printf("main_preassembler finished.\n");

    return 0;
}