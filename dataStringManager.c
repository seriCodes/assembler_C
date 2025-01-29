#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"
#include "binary_data.h"
#define MAX_CHARS_IN_LiNE 80

#define STRING_PARAMETERS_EDGE '"'
// Function to validate if a string contains only alphabetic characters
Bool isAlphabetic(const char *str)
{
    // Traverse the string until the null terminator
    while (*str)
    {
        // If the current character is not alphabetic, return false
        if (!isalpha((unsigned char)*str))
        {
            return false;
        }
        str++;
    }
    // If all characters are alphabetic, return true
    return true;
}
void directiveStringHandler(wordType result, FILE *fd, int lastCharacter)
{
    char error[MAX_ARG_STRING_LENGTH] = {0};
    int data[MAX_CHARS_IN_LiNE] = {0};
    int len;
    printf("directiveStringHandler\n");
    char parameters[80];
    Bool resError;
    printf("bef fgets fgets  \n");
    resError = isMIssingParameter(lastCharacter);
    if (resError)
    {
        return;
    }

    fgets(parameters, sizeof(parameters), fd);
    printf("bef removeSidesWhiteSpaces parameters %s\n", parameters);

    removeSidesWhiteSpaces(parameters);
    printf(" parameters %s\n", parameters);
    if (!isFirstAndLastCharMatch(parameters, STRING_PARAMETERS_EDGE))
    {

        updateShouldNotCreateFile(true, false);
        setStartErrorReport("");
        setLineErrorReport();
        getErrorReportCopy(error);
        strcat(error, " string directive must start and end with a \"");
        printf(" %s\n", error);
        updateLineNum(1);
        return;
    }
    if (isContainingWhitespacesError(parameters, STRING_PARAMETERS_EDGE))
    {
        updateLineNum(1);
        return;
    }
    len = strlen(parameters);
    if (len < 3)
    {
        updateShouldNotCreateFile(true, false);
        setStartErrorReport("");
        setLineErrorReport();
        getErrorReportCopy(error);
        strcat(error, " string must have at least one character");
        printf(" %s\n", error);
        updateLineNum(1);
        return;
    }

    printf("bef eraseFirstAndLast %s\n", parameters);

    eraseFirstAndLast(parameters);
    len = len - 2;

    printf("af eraseFirstAndLast %s\n", parameters);
    if (!isAlphabetic(parameters))
    {
        updateShouldNotCreateFile(true, false);
        setStartErrorReport("");
        setLineErrorReport();
        getErrorReportCopy(error);
        strcat(error, " string must have only a-z or A-Z");
        printf(" %s\n", error);
        updateLineNum(1);
        return;
    }
    // All possible errors handled above
    storeCharsInImageDataArray(parameters, len + 1); //+1 for '\0'
    updateLineNum(1);
}
