#include "assembler.h"
#include "binary_data.h"

#define DATA_PARAMETERS_DELIMITER ','
#define PLUS_SIGN '+'
#define MINUS_SIGN '-'

Bool isValidStringSignsOrderForDataDirective(const char *str)
{
    // Iterate over the string to check validity
    for (int i = 1; str[i] != '\0'; i++)
    {
        if (str[i] == PLUS_SIGN || str[i] == MINUS_SIGN)
        {
            // '+' or '-' must be either at the start str[0] or right after a comma
            if (str[i - 1] != DATA_PARAMETERS_DELIMITER)
            {
                return false;
            }
        }
    }
    return true;
}

const char DataValidCharacters[] = {
    ',',
    '+',
    '-',
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '\0',
};
const char DataParameterNoneAdjacentCharacters[] = {
    ',',
    '+',
    '-',
};

// Bool isDataStringCorrect(char *str)
// {
// }
void directiveDataHandler(wordType result, FILE *fd, int lastCharacter)
{
    char error[MAX_ARG_STRING_LENGTH] = {0};
    int data[MAX_INTEGERS_IN_LiNE] = {0};
    printf("directiveDataHandler\n");
    char parameters[80];
    Bool resError;
    resError = isMIssingParameter(lastCharacter);
    // printf(" directiveLabelHandler: res mistake %d\n", resError);
    if (resError)
    {
        return;
    }
    fgets(parameters, sizeof(parameters), fd);
    // printf(" directiveLabelHandler: parameters bef %s\n", parameters);

    removeWhiteSpaces(parameters);
    // printf(" directiveLabelHandler: parameters removeWhiteSpaces %s\n", parameters);

    if (isFirstOrLastCharMatch(parameters, DATA_PARAMETERS_DELIMITER))
    {

        updateShouldNotCreateFile(true, false);
        setStartErrorReport("");
        setLineErrorReport();
        getErrorReportCopy(error);
        strcat(error, " data directive can not start or end with a delimiter");
        printf(" %s\n", error);
        updateLineNum(1);
        return;
    }
    if (isLastCharMatch(parameters, PLUS_SIGN) || isLastCharMatch(parameters, MINUS_SIGN))
    {
        updateShouldNotCreateFile(true, false);
        setStartErrorReport("");
        setLineErrorReport();
        getErrorReportCopy(error);
        strcat(error, " data directive can not end with a + or - sign");
        printf(" %s\n", error);
        updateLineNum(1);
        return;
    }

    if (!isStringComposedOfCharSet(parameters, DataValidCharacters))
    {
        updateShouldNotCreateFile(true, false);
        setStartErrorReport("");
        setLineErrorReport();
        getErrorReportCopy(error);
        strcat(error, " data directive parameter contains only 0-9, +, - and the delimiter comma signs");
        printf(" %s\n", error);
        updateLineNum(1);
        return;
    }
    if (hasAdjacentRepeats(parameters, DataParameterNoneAdjacentCharacters))
    {
        updateShouldNotCreateFile(true, false);
        setStartErrorReport("");
        setLineErrorReport();
        getErrorReportCopy(error);
        strcat(error, "data directive parametr musn't have same character of +, - or the ',' sign, adjacent");
        printf(" %s\n", error);
        updateLineNum(1);
        return;
    }
    if (!validateStringOrder(parameters, '+', '-', ',') || !validateStringOrder(parameters, '-', '+', ','))
    {
        updateShouldNotCreateFile(true, false);
        setStartErrorReport("");
        setLineErrorReport();
        getErrorReportCopy(error);
        strcat(error, " data directive parametrs can not contain illogical signs adjacency");
        printf(" %s\n", error);
        updateLineNum(1);
        return;
    }
    if (!isValidStringSignsOrderForDataDirective(parameters))
    {
        updateShouldNotCreateFile(true, false);
        setStartErrorReport("");
        setLineErrorReport();
        getErrorReportCopy(error);
        strcat(error, " data directive parameters can not +,- signs between digits");
        printf(" %s\n", error);
        updateLineNum(1);
        return;
    }
    // above all possible errors being handles
    parseStringToIntegers(parameters, data);
    storeIntegersInImageDataArray(data);
    // DataBinaryLine **ptr = getDataImage();


    updateLineNum(1);
}
