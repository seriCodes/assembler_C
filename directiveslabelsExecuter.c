#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "directives.h"
#include "binary_code.h"

Bool is_label_name_is_used_in_passed_directives(char *param)
{
    char error[MAX_ARG_STRING_LENGTH] = {0};

    char **str_pointers_arr = getExternLabels();
    int amount = getLabelExternCounter();
    printf(" is_label_name_is_used_in_passed_directives amount: %d \n", amount);

    for (size_t i = 0; i < amount; i++)
    {
        printf(" is_label_name_is_used_in_passed_directives i: %d \n", i);
        printf(" is_label_name_is_used_in_passed_directives ExternLabel: %s \n", str_pointers_arr[i]);
        if (strcmp(str_pointers_arr[i], param) == 0)
        {
            updateShouldNotCreateFile(true, false);
            setStartErrorReport("");
            setLineErrorReport();
            getErrorReportCopy(error);
            strcat(error, " can not use the same label name twice in another extern directive.");
            printf(" %s\n", error);
            return true;
        }
    }
    amount = getLabelEntryCounter();
    str_pointers_arr = getEntryLabels();
    for (size_t i = 0; i < amount; i++)
    {
        printf(" is_label_name_is_used_in_passed_directives Entr i: %d \n", i);
        printf(" is_label_name_is_used_in_passed_directives Entr: %s \n", str_pointers_arr[i]);
        if (strcmp(str_pointers_arr[i], param) == 0)
        {
            updateShouldNotCreateFile(true, false);
            setStartErrorReport("");
            setLineErrorReport();
            getErrorReportCopy(error);
            strcat(error, " can not use the same label name twice in another entry directive.");
            printf(" %s\n", error);
            return true;
        }
    }
    return false;
}
void directiveLabelHandler(wordType result, FILE *fd,
                           int lastCharacter, char **createCustomDoublePointer(int size))
{ // handles the directives of entry and extern and its single label parameter
    char parameter[80];
    Bool resError;
    resError = isMIssingParameter(lastCharacter);
    // printf(" directiveLabelHandler: resError %d\n", resError);
    if (resError)
    {
        // updateLineNum(1);
        return;
    }
    fgets(parameter, sizeof(parameter), fd);
    resError = isCharAmountCorrect(parameter, OPERAND_DELIMITER, 0);
    if (!resError)
    {
        // printf("directiveLabelHandler !resError \n");

        updateLineNum(1);

        return;
    }
    removeSidesWhiteSpaces(parameter);
    if (isContainingWhitespacesError(parameter))
    {
        updateLineNum(1);
        // printf("directiveLabelHandler isContainingWhitespacesError(parameter \n");

        return;
    }
    if (!is_label_name_valid(parameter, true))
    {
        // printf("is_label_name_valid  (parameter \n");

        updateLineNum(1);
        return;
    }
    if (is_label_name_is_used_in_passed_directives(parameter))
    {
        // printf("is_label_name_is_used_in_passed_directives  (parameter \n");

        updateLineNum(1);
        return;
    }
    // above all the checks needed
    char **ptr = createCustomDoublePointer(strlen(parameter) + 1); //+1 for null terminator
    // createExternalLabelDoublePointer(strlen(parameter) + 1); //+1 for null terminator
    // printf("createExternalLabelDoublePointer ptr bef %s\n", *ptr);
    strncpy(*ptr, parameter, strlen(parameter) + 1);
    // printf("createExternalLabelDoublePointer ptr after %s\n", *ptr);
    free(ptr);
    // לאסוף את כל השורה. לבדוק תקינות וליצור פוינטר ולשמור במערך שלמעלה
    updateLineNum(1);
    return;
}
