#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h> // for size_t
#include "assembler.h"
#include "binary_code.h"
#include "directives.h"
#include "binary_data.h"
#include "file_creators.h"
#define LINE_CHAR_LIMIT 80
// static int labelsLineSize = 0;
//  Encapsulated integer variable

wordType verifyInstructionDirectiveOrLabelOrCommentCorrectFormat(char *stringAccumulator)
{
    //  printf("verifyInstructionDirectiveOrLabelOrCommentCorrectFormat  stringAccumulator %s\n", stringAccumulator);
    Bool isSaveLabel;
    char error[MAX_ARG_STRING_LENGTH] = {0};
    Directive currentDirType;
    wordType result;
    int lastChar = strlen(stringAccumulator) - 1;
    //   int dsf = getInstructionCount();
    int localVariable; // A local variable to use as a reference
    unsigned char *stackPointer = (unsigned char *)&localVariable;
    void *last_return_address;
    void *return_address_location;

    if (stringAccumulator[0] == '.')
    { // directive
        currentDirType = getDirectiveType(stringAccumulator);
        if (currentDirType == invalidDirective)
        {
            updateShouldNotCreateFile(true, false);
            setStartErrorReport("");
            setLineErrorReport();
            getErrorReportCopy(error);
            strcat(error, " directive type is invalid");
            printf(" %s\n", error);
        }
        result.dirType = currentDirType;
        result.word = DirectiveWord;
        result.instructionName = nullInstruction;
        //   printf("directive %d\n", result.dirType);
        return result;
    }
    else if (stringAccumulator[lastChar] == ':' && stringAccumulator[0] != ';') // a label whch is not a comment
    {                                                                           // label
        //    printf("label \n");
        if (is_label_name_valid(stringAccumulator, true))
        {
            // printf("label detected: first process %s \n", stringAccumulator);
            result.word = label;
        }
        else
        {
            result.word = invalidLabel;
        }
        result.dirType = notDir;
        result.instructionName = nullInstruction;

        return result;
    }
    else if (result.instructionName = identifyInstruction(stringAccumulator))
    {
        result.dirType = notDir;
        result.word = instruction;

        //   printf("instruction found %d\n", result.instructionName);
        return result;
    }
    else if (stringAccumulator[0] == ';')
    { // comment line
        result.word = comment;
        result.instructionName = nullInstruction;
        result.dirType = notDir;
        return result;
    }
    // handle unknown word error
    //   printf("endddd\n");
    // printf("handle unknown instruction found %s\n", stringAccumulator);

    result.dirType = 0;
    result.word = 0;
    result.instructionName = 0;
    return result;
}
static void initTransforms(const char *amSuffixInputFile)
{
    char nameOutputFile[100] = {0};
    int namelength;
    //  int ic = 0;
    int *ic;
    int dc = 0;
    // int icf;//intruction count final
    int labelsLineSize = 0;
    // initSymbol_table(); //! maybe it is futile. maybe everything in setSymbol_table
    char error[MAX_ARG_STRING_LENGTH] = {0};

    wordType result;
    /* const char *original = "#.....12.3.4.";
     char exclude = '.';
     char *tmp = copyWithoutChar(original, exclude);
     printf("tmp %s  \n", tmp);
     free(tmp);*/
    FILE *fd;
    int charCounter = 0;
    int ch;
    char stringAccumulator[MAX_STRING_LENGTH] = {0};
    verifyFileLineCharsLimit(amSuffixInputFile, LINE_CHAR_LIMIT);
    namelength = strlen(amSuffixInputFile);
    if (!(fd = fopen(amSuffixInputFile, "r+"))) // REMEMBER to fclose(fd);
    {
        fprintf(stderr, "cannot open file %s\n", amSuffixInputFile);
        return;
        // exit(0);
    }
    int lineNum = getLineNum();
    // printf("OUT WILE first_process %d \n", lineNum);
    initLabels();

    while (!feof(fd))
    {

        // printf("while (!feof(fd) %s \n", stringAccumulator);
        ch = fgetc(fd);
        // printf("ch = fgetc(fd); 7%c7 \n", ch);
        // printf("ch = fgetc(fd) digit; %d \n", ch);

        // printCharWithNulls(ch);
        if (
            /*!isspace((unsigned char)ch)*/ // when you have the time convert it to isspace(for unknown reason it does not work)
                ch == 32 ||
            ch == 9 || ch == 10 || ch == EOF)
        { // space, horizontal tab and new line detection
          //  printf("whitespace detected\n");

            if (charCounter != 0)
            {
                result = verifyInstructionDirectiveOrLabelOrCommentCorrectFormat(stringAccumulator); // before saving labels to table.
                // printf("result.word afte %d \n", result.word);
                if (result.word == label)
                {
                    Bool isDuplicated = isDuplicatedLabel(stringAccumulator);
                    if (!isDuplicated)
                    {
                        labelsLineSize++;
                        setLabel(stringAccumulator);
                        // printf("result.word==label \n");
                        // setSymbol_table(1, result.dirType, labelsLineSize);
                        // resetLineLabels(); //! temporarily for development
                    }
                }
                else if (result.word == DirectiveWord)
                { // handle directive
                  // printf("after result.word == DirectiveWord labelsLineSize %d \n", labelsLineSize);
                    printf("directive is: %s \n", stringAccumulator);
                    printf("result.dirType: %d \n", result.dirType);
                    printLabels();
                    if (labelsLineSize != 0 && (result.dirType == data || result.dirType == string))
                    {
                        dc = getDc(); //! need to adjust to lines per instruction
                        printf("directive is to save in table \n");

                        setSymbol_table(dc, result.dirType, labelsLineSize);
                        labelsLineSize = 0;
                    }
                    /*handle labels if .data or .string. store at symbol-table with dc*/

                    /*if .entry or .extern ignore, regarding the symbol-table and 'Data_Image' and reset line labels*/
                    /*below is reset directives*/
                    labelsLineSize = 0;
                    resetLineLabels();
                    /* handle directive itself */
                    // handleDirective(result, fd, ch);
                    if (result.dirType == externDirective)
                    {
                        printf("directive externDirective or read \n");
                        directiveLabelHandler(result, fd, ch, createExternalLabelDoublePointer);
                    }
                    if (result.dirType == entry)
                    {
                        printf("directive entry read \n");
                        directiveLabelHandler(result, fd, ch, createEntrylLabelDoublePointer);
                    }
                    if (result.dirType == data)
                    {
                        printf("data directive read \n");

                        directiveDataHandler(result, fd, ch);
                        printf("data directive end \n");
                    }
                    if (result.dirType == string)
                    {
                        printf("string directive read \n");

                        directiveStringHandler(result, fd, ch);
                        printf("string directive end \n");
                    }
                }
                else if (result.word == instruction)
                { // instruction
                    ic = getIc();
                    printf("ic first process %d \n", *ic);
                    // printLabels();
                    // printf("after result.word == instruction: labelsLineSize %d \n", labelsLineSize);
                    printf("instruction is: %s \n", stringAccumulator);
                    if (labelsLineSize != 0)
                    {
                        printf("labelsLineSize != 0\n");

                        setSymbol_table(ic, result.dirType, labelsLineSize);
                        labelsLineSize = 0;
                        resetLineLabels();
                    }
                    /* handle instruction */
                    // if (ch == 10)
                    // { // new line right after the end of an instruction
                    //     printf("new line right after the end of an instruction \n");
                    //     handleInsrtuction(result);
                    // }
                    printf("bef handleInsrtuction \n");
                    handleInsrtuction(result, fd, ch);
                    //! after handleInsrtuction is finished. fd will move to the next line and the lineNum increases in 1.
                }
                else if (result.word == comment)
                {
                    /*advance fd to the first char of the next line*/
                    printf("result.word == comment\n");
                    if (ch != 10)
                    { // if the comment is multiple words
                        moveToNextLine(fd);
                        ch = 10; // to update the lineNum
                    }
                    // if the comment is a single word- fd already mved to the next line
                }

                else if (!result.word)
                {
                    updateShouldNotCreateFile(true, false);
                    setStartErrorReport("");
                    setLineErrorReport();
                    getErrorReportCopy(error);
                    strcat(error, " unknown word is ");
                    strcat(error, stringAccumulator);

                    printf(" %s\n", error);
                    resetLineLabels();
                }
                else
                {
                    printf("ERROR! something went wrong, nothing shoud get here!\n");
                }

                //   FILE *fd2;
                //  restoreFilePosition(fd2, currentPosition);
                //    fseek(fd2, -2, SEEK_CUR);
                //  int ch_2;
                //  ch_2 = fgetc(fd2);
                //   printf("charCounter != 0 end\n");
            }
            // resetCharArray(stringAccumulator, charCounter);
            charCounter = 0;
            memset(stringAccumulator, '\0', MAX_STRING_LENGTH);
            printf("stringAccumulator, '\0', MAX_STRING_LENGTH\n");
        }
        else
        {
            //   printCharWithNulls(ch);
            stringAccumulator[charCounter] = ch;
            charCounter++;
        }
        if (ch == 10)
        {

            printf("ch == 10 in first\n");
            updateLineNum(1);
            lineNum = getLineNum();
            setLineErrorReport();
            printf("ch == 10 lineNum first_process %d \n", lineNum);
        }

        // printf("after ch == 10\n");
    }
    rewind(fd);
    // to reset after EOF is met in the end of the code.
    resetCharArray(stringAccumulator, charCounter); //  100
    charCounter = 0;
    printSymbolTable();
    // printAllCodeLines();
    //! here execute second transform
    assignOperandLabelAREtoExternalIfNotInFile();
    // printAllCodeLines();

    strncpy(nameOutputFile, amSuffixInputFile, namelength - 2);
    strcat(nameOutputFile, "ent");
    printf("nameOutputFile strcat(nameOutputFile %s\n", nameOutputFile);

    nameOutputFile[strlen(nameOutputFile)] = '\0';
    printf("nameOutputFile ende %s\n", nameOutputFile);

    entFileHandler(nameOutputFile);
    // strncpy(nameOutputFile, amSuffixInputFile, namelength - 3);
    printf("nameOutputFile %s\n", nameOutputFile);
    nameOutputFile[strlen(nameOutputFile) - 2] = 'x';
    printf("nameOutputFile %s\n", nameOutputFile);

    extFileHandler(nameOutputFile);
    nameOutputFile[strlen(nameOutputFile) - 3] = 'o';
    nameOutputFile[strlen(nameOutputFile) - 2] = 'b';
    nameOutputFile[strlen(nameOutputFile) - 1] = '\0';

    printf("nameOutputFile %s\n", nameOutputFile);
    obFileHandler(nameOutputFile);

    freeLineLabels();
    freeSymbolTableAndLabelsDataPointers();
    printExternLabels();
    freeExternLabels();
    // printExternLabels();
    ///
    // printEntryLabels();
    freeEntryLabels();
    // printEntryLabels();
    freeCodeImage();
    //! temporarily! eventually, call only after second tranformation done
    //! after second tranformation:
    resetDc();
    resetIc();
    // printDataBinaryLineArray();

    freeDataImage();
    //  printDataBinaryLineArray();

    resetCurrentFileDataCounter();

    fclose(fd);
}

int main_first_transformation(int argc, char *argv[])
{

    // printf("first_process.c,  \n");

    char nameInputFile[100];
    int maxInputFileName = 100;
    for (int i = 1; i < argc; i++)
    {
        memset(nameInputFile, '\0', maxInputFileName);
        printf("first_process.c, loop  \n");

        printf("Argument command line %d: %s\n", i, argv[i]);
        printf("Argument command line *(argv + i) %d: %s\n", i, (*(argv + i)));
        updateShouldNotCreateFile(false, true);
        if (i > 0)
        {
            strcat(nameInputFile, *(argv + i));
            strcat(nameInputFile, ".am");
            setGetFileName(nameInputFile, true);
            char *tmp = setGetFileName("", false);

            // printf("first process main setGetFileName %s\n", tmp);

            // printCharArrayWithNulls(nameInputFile, 40);
            setLineNum(1);

            // writeFixedBinaryToInstructionModel(mov);
            removesConsecutiveWhitspaecesFromEndOfFile(nameInputFile);

            initTransforms(nameInputFile);
            // printOperandLabels();
            resetLabelOperandCounterAndFreeLabelOperandCounter();
            // printOperandLabels();
        }
    }
}