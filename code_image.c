#include <stdio.h>
#include "binary_code.h"
#include "assembler.h"
InstructionData *findInstructionByName(InstructionName name);
Bool isRegister(char *str)
{
    return str[0] == 'r' && str[1] >= '0' && str[1] <= '7' && strlen(str) == 2;
}
Bool isAddressingMethodContains(AddressingMethods *options, AddressingMethods wanted)
{
    printf(" isAddressingMethodContains  \n");

    for (size_t i = 0; i < 3; i++)
    {
        // printf(" options %d\n", options);
        // printf(" options[i] %d\n", options[i]);

        if (options[i] == wanted)
        {
            // printf(" options[i] == wanted\n");

            return true;
        }
    }
    return false;
}

Bool isRelativeAddressingCorrectToInstruction(char *buffer, AddressingMethods *addressingMethodsArrayOptions)
{
    //   printf(" isRelativeAddressingCorrectToInstruction buffer: %s\n", buffer);
    //  printf(" isRelativeAddressingCorrectToInstruction buffer[0]: %c\n", buffer[0]);
    // printf(" buffer[0] == '&': %d\n", buffer[0] == '&');

    // printf(" isRelativeAddressingCorrectToInstruction buffer+2: %s\n", (buffer + 2));

    return buffer[0] == '&' && is_label_name_valid(buffer + 1, true) && isAddressingMethodContains(addressingMethodsArrayOptions, relativeLabel);
}
Bool isDirectAddressingCorrectToInstruction(char *buffer, AddressingMethods *addressingMethodsArrayOptions)
{

    return is_label_name_valid(buffer, true) && isAddressingMethodContains(addressingMethodsArrayOptions, directLabel);
}
Bool isDirectRegisterAddressingCorrectToInstruction(char *buffer, AddressingMethods *addressingMethodsArrayOptions)
{

    return isRegister(buffer) && isAddressingMethodContains(addressingMethodsArrayOptions, directRegister);
}
Bool isImmendiateCorrectToInstruction(char *buffer, AddressingMethods *addressingMethodsArrayOptions)
{
    return buffer[0] == '#' && isAddressingMethodContains(addressingMethodsArrayOptions, immediate) && isNumericString((buffer + 1));
}
AddressingMethods detectAndVerifyIfOperandAddressingMethodCorrect(InstructionData *instructionModel, char *buffer, Bool isSource)
{
    AddressingMethods *addressingMethodsArrayOptions;
    // int firstChar = buffer[0];
    printf(" detectAndVerifyIfOperandAddressingMethodCorrect buffer: %s\n", buffer);
    // printf(" detectAndVerifyIfOperandAddressingMethodCorrect firstChar: %c\n", firstChar);
    if (isSource)
    {
        addressingMethodsArrayOptions = instructionModel->sourceAddressingMethods;
    }
    else
    {
        addressingMethodsArrayOptions = instructionModel->destinationAddressingMethods;
    }

    if (isImmendiateCorrectToInstruction(buffer, addressingMethodsArrayOptions))
    {
        printf(" firstChar == # \n");
        return immediate;
    }
    if (isRelativeAddressingCorrectToInstruction(buffer, addressingMethodsArrayOptions))
    {
        printf(" firstChar == &\n");
        return relativeLabel;
    }
    if (isDirectRegisterAddressingCorrectToInstruction(buffer, addressingMethodsArrayOptions))
    {
        printf(" firstChar == r of register directRegister\n");
        return directRegister;
    }
    if (isDirectAddressingCorrectToInstruction(buffer, addressingMethodsArrayOptions))
    {
        printf(" opernad of label\n");
        return directLabel;
    }
    // operand is not in correct addresing method
    return nullAddressingMethods;
    // else if(){

    // }
}
void handleInsrtuction(wordType result, FILE *fd,
                       int lastCharacter)
{
    char buffer[80]; // Buffer for reading lines
    char leftOperand[80] = {0};
    char rightOperand[80] = {0};
    AddressingMethods methodCatched;
    AddressingMethods methodCatchedRightOperand;

    char error[MAX_ARG_STRING_LENGTH] = {0};
    // printf("start handleInsrtuction\n");
    InstructionData *instructionModel = findInstructionByName(result.instructionName);
    // printf("instructionModel name: %d opernadAmount: %d\n", instructionModel->name, instructionModel->operandAmount);
    if (lastCharacter == 10 || lastCharacter == EOF)
    { // new line right after the end of an instruction was found
      // Considering whitespaces characters, it is only possible to have a newline character in the end of a line, due to the file proccessing in the beginning of the execution.
        printf("lastCharacter == 10\n");
        if (!instructionModel->operandAmount)
        {
            printf(" result.instructionName %s\n", InstructionsStrings[result.instructionName]);
            //! handle "no opernads instruction"- code_image array elemnts-> completed
            storeInImageCodeArray(instructionModel, nullAddressingMethods, nullAddressingMethods, NULL, NULL);
            return;
        }
        else
        {
            updateShouldNotCreateFile(true, false);
            setStartErrorReport("");
            setLineErrorReport();
            getErrorReportCopy(error);
            strcat(error, " too few arguments for instruction");
            printf(" %s\n", error);
            return;
        }
    }
    else if (!instructionModel->operandAmount)
    { // too many arguments for an operator that needs zero operands
        setStartErrorReport("");
        setLineErrorReport();
        getErrorReportCopy(error);
        strcat(error, " too many arguments for instruction: ");
        strcat(error, InstructionsStrings[result.instructionName]);
        printf(" %s\n", error);
        fgets(buffer, sizeof(buffer), fd); // to forward fd to next line
        updateLineNum(1);
        return;
    }
    else
    {
        // printf("handle Insrtuction\n");
        fgets(buffer, sizeof(buffer), fd);
        // printf("buffer: %s\n", buffer);
        int commasAmountInString = countCharacterInString(buffer, OPERAND_DELIMITER);
        // printf("commasAmountInString: %d\n", commasAmountInString);
        if (commasAmountInString > 1 || commasAmountInString == instructionModel->operandAmount)
        {
            // printf("commasAmountInString: %d\n", commasAmountInString);
            setStartErrorReport("");
            setLineErrorReport();
            getErrorReportCopy(error);
            strcat(error, " too many arguments for instruction: ");
            strcat(error, InstructionsStrings[result.instructionName]);
            printf(" %s\n", error);
            updateLineNum(1);

            return;
        }
        else if (instructionModel->operandAmount - commasAmountInString > 1)
        {
            setStartErrorReport("");
            setLineErrorReport();
            getErrorReportCopy(error);
            strcat(error, " too few arguments for instruction or missing comma: ");
            strcat(error, InstructionsStrings[result.instructionName]);
            printf(" %s\n", error);
            updateLineNum(1);
            return;
        }
        // executing below means the operand amount is correct

        if (instructionModel->operandAmount == 1)
        { // handle single operand instructions
            printf(" instructionModel->operandAmount==1\n");
            // printCharArrayWithNulls(buffer, 25);
            removeSidesWhiteSpaces(buffer);
            //  printf(" after removeStartingAndEndingWhiteSpaces\n");

            // printCharArrayWithNulls(buffer, 25);
            // if there is a whitespace in the string report error.
            printf("buffer %s\n", buffer);
            if (isContainsWhitespace(buffer))
            {
                setStartErrorReport("");
                setLineErrorReport();
                getErrorReportCopy(error);
                strcat(error, " operand can not contain a whitespace: ");
                strcat(error, InstructionsStrings[result.instructionName]);
                printf(" %s\n", error);
                updateLineNum(1);
                return;
            }
            // handle single operand instruction
            methodCatched = detectAndVerifyIfOperandAddressingMethodCorrect(instructionModel, buffer, false);

            printf("methodCatched %d\n", methodCatched);
            printf("methodCatched instructionName %d\n", result.instructionName);

            if (!methodCatched)
            {
                setStartErrorReport("");
                setLineErrorReport();
                getErrorReportCopy(error);
                strcat(error, " operand is not the correct addressing method. instruction: ");
                strcat(error, InstructionsStrings[result.instructionName]);
                printf(" %s\n", error);
                updateLineNum(1);
                return;
            }
            if (methodCatched == directRegister)
            { // below is handling an instruction with one opernad with a single code line

                storeInImageCodeArray(instructionModel, nullAddressingMethods, methodCatched, NULL, buffer);
                updateLineNum(1);
                return;
            }
            if (methodCatched == directLabel || methodCatched == immediate || methodCatched == relativeLabel)
            { // below is handling an instruction with one opernad with a 2 code lines

                storeInImageCodeArray(instructionModel, nullAddressingMethods, methodCatched, NULL, buffer);
                updateLineNum(1);
                return;
            }

            printf("something is wrong\n");
            return;
        }

        //! Below is for instructionModel->operandAmount == 2, option. Because all other options were handled above.
        splitAndWriteArraysByOpernadDelimiter(leftOperand, rightOperand, buffer);
        // printf(" leftOperand\n");

        // printCharArrayWithNulls(leftOperand, 25);
        // printf(" rightOperand\n");
        // printCharArrayWithNulls(rightOperand, 25);
        removeSidesWhiteSpaces(leftOperand);
        removeSidesWhiteSpaces(rightOperand);
        // printf(" rightOperand removeSidesWhiteSpaces \n");
        // printCharArrayWithNulls(rightOperand, 25);

        if (isContainsWhitespace(leftOperand) || isContainsWhitespace(rightOperand))
        {
            setStartErrorReport("");
            setLineErrorReport();
            getErrorReportCopy(error);
            strcat(error, " operand can not contain a whitespace: ");
            strcat(error, InstructionsStrings[result.instructionName]);
            printf(" %s\n", error);
            updateLineNum(1);
            return;
        }
        methodCatched = detectAndVerifyIfOperandAddressingMethodCorrect(instructionModel, leftOperand, true);
        if (!methodCatched)
        {
            setStartErrorReport("");
            setLineErrorReport();
            getErrorReportCopy(error);
            strcat(error, "left operand is not the correct addressing method. instruction: ");
            strcat(error, InstructionsStrings[result.instructionName]);
            printf(" %s\n", error);
            updateLineNum(1);
            return;
        }
        methodCatchedRightOperand = detectAndVerifyIfOperandAddressingMethodCorrect(instructionModel, rightOperand, false);
        if (!methodCatchedRightOperand)
        {
            setStartErrorReport("");
            setLineErrorReport();
            getErrorReportCopy(error);
            strcat(error, "right operand is not the correct addressing method. instruction: ");
            strcat(error, InstructionsStrings[result.instructionName]);
            printf(" %s\n", error);
            updateLineNum(1);
            return;
        }
        // not yet written below adjustments
        // storeInImageCodeArray(instructionModel, methodCatched, methodCatchedRightOperand, leftOperand, rightOperand);
        
        storeInImageCodeArray(instructionModel, methodCatched, methodCatchedRightOperand, leftOperand, rightOperand);

        updateLineNum(1);

        return;

        //! handle dual operand intruction code_image array elemnts
    }
}
/* Function to find an InstructionData by name */

InstructionData *findInstructionByName(InstructionName name)
{
    /* Calculate the number of elements in the array */
    size_t instructionCount = sizeof(instructionsModels) / sizeof(instructionsModels[0]);
    printf("size_t instructionCount %d\n", instructionCount);

    /* Iterate over the array */
    for (size_t i = 0; i < instructionCount; i++)
    {
        //  printf("for instructionCount i %d\n", i);

        if (instructionsModels[i].name == name)
        {
            //   printf("for instructionCount name %d\n", name);

            return &instructionsModels[i]; // Return a pointer to the matching element
        }
    }

    return NULL; // Return NULL if no match is found
}
