#include "assembler.h"
void verifyFileLineCharsLimit(char *filename, int limit)
{
    printf("verifyFileLineCharsLimit first");

    if (filename == NULL || limit == NULL)
    {
        return false; // Invalid input
    }
    printf("verifyFileLineCharsLimit fopen");

    // Open the file in read mode
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file in doesStringExistInFile");
        return false;
    }

    // Buffer to store each line of the file
    char buffer[100] = {0};
    //   printf("\n searchString %s  \n", searchString);
    int len;
    // Search for the string in the file line by line

    printf("while bef");

    while (fgets(buffer, sizeof(buffer), file))
    {
        len = strlen(buffer);
        //  printf("%s length %d line %d\n", buffer, len - 1, getLineNum());
        if (strlen(buffer) - 1 > limit) // minus 1 to exclude \n
        {
            updateShouldNotCreateFile(true, false);
            setStartErrorReport("");

            setLineErrorReport();
            char tmp[MAX_ARG_STRING_LENGTH];
            getErrorReportCopy(tmp);

            strncat(tmp, " no more than 80 characters in a single line, excluding \\n", MAX_ARG_STRING_LENGTH);
            printf(" %s\n", tmp);
            // printf("\n updateShouldNotCreateFile line NUm %d \n", getLineNum());
        }
        updateLineNum(1);
    }
    setLineNum(1);
    fclose(file);
    printf("verifyFileLineCharsLimit end");
}

Bool is_label_name_valid(char *name, Bool isReportError)
{
    char copy[32];
    strncpy(copy, name, 32);
    // printf(" is_label_name_valid name: %s\n", name);
    // printf(" is_label_name_valid copy: %s\n", copy);
    copy[strlen(copy)] = '\0';
    if (copy[strlen(copy) - 1] == ':')
    {
        copy[strlen(copy) - 1] = '\0';
    }

    printf(" is_label_name_valid copy modi: %s\n", copy);

    int res;
    res = isLetter(name[0]);
    char error[MAX_ARG_STRING_LENGTH] = {0};
    setStartErrorReport("");
    setLineErrorReport();
    getErrorReportCopy(error);
    if (!res)
    {
        if (isReportError)
        {
            updateShouldNotCreateFile(true, false);
            strcat(error, "label must start with a letter");
            printf(" %s\n", error);
        }

        return false;
    }
    else if (strlen(name) > MAX_LABEL_LIMIT)
    {
        if (isReportError)
        {
            updateShouldNotCreateFile(true, false);
            strcat(error, "label must be at most 31 characters");
            printf(" %s\n", error);
        }

        return false;
    }
    else if (isRegister(copy)) // an addition to the teacher instructions
    {
        if (isReportError)
        {
            updateShouldNotCreateFile(true, false);
            strcat(error, "label can not be a register name");
            printf(" %s\n", error);
        }

        return false;
    }
    else if (isKeyword(copy)) // an addition to the teacher instructions
    {
        if (isReportError)
        {
            updateShouldNotCreateFile(true, false);
            strcat(error, "label can not be a keyword");
            printf(" %s\n", error);
        }

        return false;
    }
    return true;
}
InstructionName identifyInstruction(const char *stringAccumulator)
{
    if (stringAccumulator == NULL)
    {
        return nullInstruction;
    }

    if (strcmp(stringAccumulator, "mov") == 0)
    {
        return mov;
    }
    else if (strcmp(stringAccumulator, "cmp") == 0)
    {
        return cmp;
    }
    else if (strcmp(stringAccumulator, "add") == 0)
    {
        return add;
    }
    else if (strcmp(stringAccumulator, "sub") == 0)
    {
        return sub;
    }
    else if (strcmp(stringAccumulator, "lea") == 0)
    {
        return lea;
    }
    else if (strcmp(stringAccumulator, "clr") == 0)
    {
        return clr;
    }
    else if (strcmp(stringAccumulator, "not") == 0)
    {
        return not ;
    }
    else if (strcmp(stringAccumulator, "inc") == 0)
    {
        return inc;
    }
    else if (strcmp(stringAccumulator, "dec") == 0)
    {
        return dec;
    }
    else if (strcmp(stringAccumulator, "jmp") == 0)
    {
        return jmp;
    }
    else if (strcmp(stringAccumulator, "bne") == 0)
    {
        return bne;
    }
    else if (strcmp(stringAccumulator, "jsr") == 0)
    {
        return jsr;
    }
    else if (strcmp(stringAccumulator, "red") == 0)
    {
        return red;
    }
    else if (strcmp(stringAccumulator, "prn") == 0)
    {
        return prn;
    }
    else if (strcmp(stringAccumulator, "rts") == 0)
    {
        return rts;
    }
    else if (strcmp(stringAccumulator, "stop") == 0)
    {
        return stop;
    }
    else
    {
        return nullInstruction; // Default case for undefined instructions
    }
}
Bool isMIssingParameter(int lastCharacterAfterFetchedWord)
{
    char error[MAX_ARG_STRING_LENGTH] = {0};

    if (lastCharacterAfterFetchedWord == 10 || lastCharacterAfterFetchedWord == EOF)
    { // newlinr or end of file
        updateShouldNotCreateFile(true, false);
        setStartErrorReport("");
        setLineErrorReport();
        getErrorReportCopy(error);
        strcat(error, " too few arguments for directive");
        printf(" %s\n", error);
        return true;
    }
    return false;
}
Bool isContainingWhitespacesError(char *parameter)
{
    char error[MAX_ARG_STRING_LENGTH] = {0};
    // printf("isContainingWhitespacesError  parameter %s\n", parameter);

    if (isContainsWhitespace(parameter))
    {
        updateShouldNotCreateFile(true, false);

        setStartErrorReport("");
        setLineErrorReport();
        getErrorReportCopy(error);
        strcat(error, " directive parameter can not contain a whitespace ");
        printf(" %s\n", error);

        return true;
    }
    return false;
}
Bool isCharAmountCorrect(char *parameter, char c, int limit)
{
    printf(" isCharAmountCorrect  \n");
    char str[2];
    char error[MAX_ARG_STRING_LENGTH] = {0};
    int CharAmountInString = countCharacterInString(parameter, c);
    ;
    printf(" CharAmountInString %d  \n", CharAmountInString);

    if (CharAmountInString > limit)
    {
        updateShouldNotCreateFile(true, false);

        setStartErrorReport("");
        setLineErrorReport();
        getErrorReportCopy(error);
        strcat(error, " too many arguments for Directive. '");
        charToString(c, str);
        strcat(error, str);
        strcat(error, "' is a delimiter.");

        printf(" %s\n", error);

        return false;
    }
    return true;
}