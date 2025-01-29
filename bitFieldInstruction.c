#include <string.h>
#include <stdlib.h>
#include "assembler.h"
#include "binary_code.h"
// #define MAX_CODE_LINES_ALLOCATIONS 1000
// void *codeImageAllocations[MAX_CODE_LINES_ALLOCATIONS] = {0}; // Array to track allocations
Bool isOnlySingleRegisterlFromTwoOperands(AddressingMethods destMethod, AddressingMethods srcMethod);
Bool isOperandRequireAcodeLine(AddressingMethods operandMethod);

InstructionBinaryData copyInitialBinaryImage(InstructionName name, CodeLine *ptr);
void storeTwoCodeLinesWithSingleOperand(InstructionData *instructionModel,
                                        char *destinationOperand);
void addImmediateCodeLine(char *num);

static int alloc_count = 0;
int getCodeImageAlloc_count()
{
    return alloc_count;
}
void resetCodeImage()
{
    CodeLine **ptr = getCodeImage();
    *ptr = NULL;
}
CodeLine **getCodeImage()
{
    static CodeLine *codeImage = {0};
    // printf(" getCodeImage codeImage %p  \n", codeImage);
    // printf(" getCodeImage &codeImage %p  \n", &codeImage);

    return &codeImage;
}

CodeLine *initCodeImage()
{
    printf("initCodeImage  \n ");
    // if (alloc_count >= MAX_CODE_LINES_ALLOCATIONS)
    // {
    //     fprintf(stderr, "Too many CODE_LINES_ALLOCATIONS!\n");
    //     exit(1);
    // }
    // if(codeImage[0]){
    CodeLine *codeImage_Ptr = (CodeLine *)malloc(1 * sizeof(CodeLine));
    // }
    if (codeImage_Ptr)
    {
        memset(codeImage_Ptr, 0, 1);
        // codeImageAllocations[alloc_count++] = codeImage_Ptr;
    }
    alloc_count++;
    return codeImage_Ptr;
}
void freeCodeImage()
{
    CodeLine **codeImage = getCodeImage();
    if ((*codeImage) == NULL)
    {
        printf("Code image is empty.\n");
        return;
    }
    free(*codeImage);
    resetCodeImage();
    // codeImage = NULL;
    printf("freeCodeImage end .\n");

    alloc_count = 0;
}
CodeLine *reallocCodeImageWithOneMore(CodeLine *codeImage_ptr)
{

    // printf("reallocCodeImageWithOneMore\n");
    // printf("codeImage_ptr beff %p\n", codeImage_ptr);
    //   printf("codeImage_ptr p start %p\n", codeImage_ptr);

   
    if (codeImage_ptr == NULL)
    {
        return initCodeImage(1);
    }
    //   printf("codeImage_ptr p %p\n", codeImage_ptr);

    // printf("codeImage_ptr alloc_count= %d!!\n", alloc_count);
    // printf("sizeof(CodeLine)= %d!!\n", sizeof(CodeLine));
    // printf("sizeof(char **)= %d!!\n", sizeof(char **));

    codeImage_ptr = (CodeLine *)realloc(codeImage_ptr, (alloc_count + 1) * sizeof(CodeLine));

    // printf("codeImage_ptr afte!!\n");

    alloc_count++;
    if (!codeImage_ptr)
    {
        fprintf(stderr, "Failed to realloc memory for code image!!\n");
    }
    return codeImage_ptr;
}

void storeSingleLineInstructionsToCodeArray(InstructionData *instructionModel, char *sourceOperand, char *destinationOperand)
{
    printf(" storeSingleLineInstructionsToCodeArray  \n");

    int *ic;
    // if(instructionModel->name!=stop && instructionModel->name!=rts){
    // fprintf(stderr, "Attempted to allocate code line of a single line intr!\n");
    // }
    // printf(" instructionModel  \n", instructionModel->name);
    CodeLine **ptr = getCodeImage();
    // printf(" reallocCodeImageWithOneMore bef %p  \n", ptr);
    // printf(" reallocCodeImageWithOneMore bef *ptr %p  \n", *ptr);

    *ptr = reallocCodeImageWithOneMore(*ptr);
    // printf(" reallocCodeImageWithOneMore after %p  \n", ptr);

    // printf(" reallocCodeImageWithOneMore after *ptr %p  \n", *ptr);

    // printf(" reallocCodeImageWithOneMore %p  \n", *ptr);
    (*ptr)[alloc_count - 1].type = 0;
    ic = getIc();
    (*ptr)[alloc_count - 1].ic = *ic;
    updateIc(1);
    // ptr[alloc_count - 1].data.instructionLine.binaryFields.ARE = 4;
    // ptr[alloc_count - 1].data.instructionLine.binaryFields =
    // printf(" reallocCodeImageWithOneMore bef alloc_count %d  \n", alloc_count);
    // printf(" reallocCodeImageWithOneMore   ptr %p  \n", *ptr);
    // printf(" reallocCodeImageWithOneMore   ptr+1-1 %p  \n", (*ptr) + 1 - 1);

    copyInitialBinaryImage(instructionModel->name, ((*ptr) + alloc_count - 1));

    int regNum;
    printf(" sourceOperand copyInitialBinaryImage %s  \n", sourceOperand);

    if (!sourceOperand && destinationOperand)
    { // single opernad
        printf(" !sourceOperand && destinationOperand %s  \n", sourceOperand);
        regNum = charToInt(destinationOperand[1]);
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destReg = regNum;
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceAddress = 0;
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceReg = 0;

        return;
    }
    // below dual operand
    if (sourceOperand)
    {
        printf(" sourceOperand %s  \n", sourceOperand);
        regNum = charToInt(sourceOperand[1]);
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceReg = regNum;
    }
    if (destinationOperand)
    {
        regNum = charToInt(destinationOperand[1]);
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destReg = regNum;
    }
    // printf(" reallocCodeImageWithOneMore ptr->data.instructionLine.binaryFields.ARE %d  \n", (*ptr)->data.instructionLine.binaryFields.ARE);
    // printf(" reallocCodeImageWithOneMore ptr->data.instructionLine.binaryFields.opcode %d  \n", (*ptr)->data.instructionLine.binaryFields.opcode);
}

// void storeTwoCodeLinesInstructionsToCodeArray(InstructionData *instructionModel, char *sourceOperand,
//                                               char *destinationOperand)
// {
//     if (sourceOperand)
//     {
//         //! write a function to handle 2 code linens with 2 opernads
//         return;
//     }
//     else
//     {
//         storeTwoCodeLinesWithSingleOperand(instructionModel, sourceOperand, destinationOperand);
//     }
// }
CodeLine **defineCommonAttributesOfLabelOrImmediateCodeLine()
{
    int *ic;

    CodeLine **ptr = getCodeImage();
    *ptr = reallocCodeImageWithOneMore(*ptr);
    (*ptr)[alloc_count - 1].type = 1;
    ic = getIc();
    (*ptr)[alloc_count - 1].ic = *ic;
    updateIc(1);
    (*ptr)[alloc_count - 1].data.labelOrImmediateLine.ARE = 4;
    return ptr;
}
void addLabelCodeLineInFirstTransform(char *str)
{
    printf("addLabelCodeLineInFirstTransform\n");
    int size;
    char **ptr_str;

    CodeLine **ptr = defineCommonAttributesOfLabelOrImmediateCodeLine();
    printf("defineCommonAttributesOfLabelOrImmediateCodeLine OUT\n");

    (*ptr)[alloc_count - 1].data.labelOrImmediateLine.isLabel = 1;
    size = strlen(str);
    printf("size = strlen(str)= %d\n", size);
    printf("str= %s\n", str);

    printf("labelOrImmediateLine.isLabel = 1 OUT\n");
    // labelOrImmediateLine.ARE = 4; is the default- so no need to assign if 'directLabel'
    if (str[0] != '&')
    { // direct addressing method
        // printf("str[0] != '&'\n");

        (*ptr)[alloc_count - 1].data.labelOrImmediateLine.ARE = 2;
        // size--;
        // printf("alloc_count - 1].data.labelOrImmediateLine.ARE = 2\n");

        ptr_str = addOperandLabels(size + 1); //+1 for null terminator
        // printf("after ptr_str = addOperandLabels(size);\n");

        // strcpy((*ptr_str), str);
        strncpy((*ptr_str), str, size + 1);
    }
    else
    {
        ptr_str = addOperandLabels(size + 1);     //! remember- consider fix to -1 minus 1,
                                                  //  +1 for null terminator
                                                  // strcpy((*ptr_str), (str + 1)); // to ommit the & character
        strncpy((*ptr_str), (str + 1), size + 1); // to ommit the & character
    }
    //  printCharArrayWithNulls(ptr_str, 19);
    //  printCharArrayWithNulls((*ptr_str), 18);
    // printf("addOperandLabels if (*ptr_str)) %s\n", (*ptr_str));

    (*ptr)[alloc_count - 1].data.labelOrImmediateLine.nameOrImmediate.labelName = ptr_str;
    //   free(ptr_str);
    // (*ptr)[alloc_count - 1].data.labelOrImmediateLine.ARE
}
void addImmediateCodeLine(char *num)
{
    // int *ic;
    // printf(" addImmediateCodeLine num: %s \n", num);
    // CodeLine **ptr = getCodeImage();
    // *ptr = reallocCodeImageWithOneMore(*ptr);
    // (*ptr)[alloc_count - 1].type = 1;
    // ic = getIc();
    // (*ptr)[alloc_count - 1].ic = *ic;
    // updateIc(1);
    // (*ptr)[alloc_count - 1].data.labelOrImmediateLine.ARE = 4;
    CodeLine **ptr = defineCommonAttributesOfLabelOrImmediateCodeLine();
    (*ptr)[alloc_count - 1].data.labelOrImmediateLine.nameOrImmediate.size = stringToInt(num);
    (*ptr)[alloc_count - 1].data.labelOrImmediateLine.isLabel = 0;
}
void storeThreeCodeLinesFromTwoOperandInstruction(InstructionData *instructionModel, char *destinationOperand, char *sourceOperand)
{
    int *ic;
    int regNum;
    int alloc_count_copy;

    // printf("storeThreeCodeLinesFromTwoOperandInstruction\n");
    CodeLine **ptr = getCodeImage();
    *ptr = reallocCodeImageWithOneMore(*ptr);
    (*ptr)[alloc_count - 1].type = 0;
    ic = getIc();
    (*ptr)[alloc_count - 1].ic = *ic;
    updateIc(1);
    copyInitialBinaryImage(instructionModel->name, ((*ptr) + alloc_count - 1));
    (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destReg = 0;
    (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceReg = 0;
    alloc_count_copy = alloc_count;
    if (sourceOperand[0] == '#')
    {
        (*ptr)[alloc_count_copy - 1].data.instructionLine.binaryFields.sourceAddress = immediateMethodVal;
        addImmediateCodeLine((sourceOperand + 1));
    }
    else
    { // it must be a directLabel
        // printf("sourceOperand[0] !! '#'\n");

        (*ptr)[alloc_count_copy - 1].data.instructionLine.binaryFields.sourceAddress = directLabelMethodVal;
        // printf("directLabelMethodVal %d '#'\n", directLabelMethodVal);
        //! alloc_count will change!
        addLabelCodeLineInFirstTransform(sourceOperand);
    }
    if (destinationOperand[0] == '#')
    {
        (*ptr)[alloc_count_copy - 1].data.instructionLine.binaryFields.destAddress = immediateMethodVal;
        addImmediateCodeLine((destinationOperand + 1));
    }
    else
    { // it must be a directLabel
        // printf("destinationOperand[0] !! '#'\n");
        // printf(" destinationOperand: %s\n", destinationOperand);

        (*ptr)[alloc_count_copy - 1].data.instructionLine.binaryFields.destAddress = directLabelMethodVal;

        // printf("(*ptr)[alloc_count_copy - 1].data.instructionLine.binaryFields.destAddress %d '#'\n", (*ptr)[alloc_count_copy - 1].data.instructionLine.binaryFields.destAddress);
        // printf("(*ptr)[alloc_count_copy - 1]  %p \n", (*ptr)[alloc_count_copy - 1]);
        // printf(" [alloc_count - 1]  %d\n", alloc_count_copy - 1);

        addLabelCodeLineInFirstTransform(destinationOperand);
    }
}
void storeTwoCodeLinesFromTwoOperandInstruction(InstructionData *instructionModel, char *destinationOperand, char *sourceOperand)
{
    static int test = 0;
    int *ic;
    int regNum;

    // printf("storeTwoCodeLinesFromTwoOperandInstruction\n");
    CodeLine **ptr = getCodeImage();
    *ptr = reallocCodeImageWithOneMore(*ptr);
    // printf("storeTwoCodeLinesFromTwoOperandInstruction test %d\n", test);
    test++;
    (*ptr)[alloc_count - 1].type = 0;
    ic = getIc();
    (*ptr)[alloc_count - 1].ic = *ic;
    updateIc(1);
    copyInitialBinaryImage(instructionModel->name, ((*ptr) + alloc_count - 1));

    if (isRegister(destinationOperand))
    {
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destAddress = directRegisterMethodVal;
        regNum = charToInt(destinationOperand[1]);
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destReg = regNum;
        // printf("storeTwoCodeLinesFromTwoOperandInstruction sourceOperand[0] %c \n", sourceOperand[0]);

        if (sourceOperand[0] == '#')
        {
            // printf("storeTwoCodeLinesFromTwoOperandInstruction sourceOperand[0] == '#'\n");

            (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceAddress = immediateMethodVal;
            (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceReg = 0;
            addImmediateCodeLine((sourceOperand + 1));
            return;
        }
        if (sourceOperand[0] != '&')
        { // this handles direct label
            (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceAddress = directLabelMethodVal;
        }
        else
        {
            (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceAddress = relativeLabelMethodVal;
        }
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceReg = 0;
        addLabelCodeLineInFirstTransform(sourceOperand);
        return;
    }

    if (isRegister(sourceOperand))
    {
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceAddress = directRegisterMethodVal;
        regNum = charToInt(sourceOperand[1]);
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceReg = regNum;

        if (destinationOperand[0] == '#')
        {
            (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destAddress = immediateMethodVal;
            (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destReg = 0;
            addImmediateCodeLine((destinationOperand + 1));
            return;
        }
        if (destinationOperand[0] != '&')
        { // this handles direct label
            (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destAddress = directLabelMethodVal;
        }
        else
        {
            (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destAddress = relativeLabelMethodVal;
        }
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destReg = 0;
        // printf("storeTwoCodeLinesFromTwoOperandInstruction bef addLabelCodeLineInFirstTransform");
        addLabelCodeLineInFirstTransform(destinationOperand);
        return;
    }
}
void storeTwoCodeLinesWithSingleOperand(InstructionData *instructionModel,
                                        char *destinationOperand)
{
    int *ic;
    // printf("storeTwoCodeLinesWithSingleOperand\n");
    CodeLine **ptr = getCodeImage();
    *ptr = reallocCodeImageWithOneMore(*ptr);
    (*ptr)[alloc_count - 1].type = 0;
    ic = getIc();
    (*ptr)[alloc_count - 1].ic = *ic;
    updateIc(1);
    copyInitialBinaryImage(instructionModel->name, ((*ptr) + alloc_count - 1));
    // printf(" storeTwoCodeLinesWithSingleOperand after copyInitialBinaryImage \n");

    (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceAddress = 0;
    (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceReg = 0;
    // printf(" storeTwoCodeLinesWithSingleOperand     (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.sourceReg = 0; \n");

    if (destinationOperand[0] == '#')
    { // this handles prn with an immediate

        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destAddress = 0;
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destReg = 0;

        // printf("binaryFields.destReg = 0\n");
        addImmediateCodeLine((destinationOperand + 1));
        return;
    }

    if (destinationOperand[0] != '&')
    { // this handles direct label
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destAddress = directLabelMethodVal;
    }
    else
    {
        (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destAddress = relativeLabelMethodVal;
    }
    (*ptr)[alloc_count - 1].data.instructionLine.binaryFields.destReg = 0;
    addLabelCodeLineInFirstTransform(destinationOperand);
}
InstructionBinaryData copyInitialBinaryImage(InstructionName name, CodeLine *ptr)
{
    // printf(" copyInitialBinaryImage \n");
    // printf(" copyInitialBinaryImage ptr %p\n", ptr);

    for (size_t i = 0; i < INSTRUCTION_BINARY_MODELS_COUNT; i++)
    {
        if (name == instructionsBinaryImage[i].name)
        {
            // printf(" name==instructionsBinaryImage found \n");
            ptr->data.instructionLine.binaryFields.ARE = instructionsBinaryImage[i].binary.ARE;
            // printf(" ptr->data.instructionLine \n");

            ptr->data.instructionLine.binaryFields.destAddress = instructionsBinaryImage[i].binary.destAddress;
            ptr->data.instructionLine.binaryFields.destReg = instructionsBinaryImage[i].binary.destReg;
            ptr->data.instructionLine.binaryFields.funct = instructionsBinaryImage[i].binary.funct;
            ptr->data.instructionLine.binaryFields.opcode = instructionsBinaryImage[i].binary.opcode;

            ptr->data.instructionLine.binaryFields.sourceAddress = instructionsBinaryImage[i].binary.sourceAddress;
            ptr->data.instructionLine.binaryFields.sourceReg = instructionsBinaryImage[i].binary.sourceReg;
        }
    }
    // printf(" copyInitialBinaryImage end \n");
}
void storeInImageCodeArray(InstructionData *instructionModel,
                           AddressingMethods sourceOperandAddressingChosenMethod, AddressingMethods destinationOperandAddressingChosenMethod,
                           char *sourceOperand,
                           char *destinationOperand)

{
    // static int i = 0;
    // printf(" storeInImageCodeArray static int i % d\n ", i
    // );
    // i++;
    // printf(" storeInImageCodeArray instructionModel->name %s\n", InstructionsStrings[instructionModel->name]);

    // first if handles the option of a single code line, with or without operands
    if ((!sourceOperandAddressingChosenMethod && !destinationOperandAddressingChosenMethod)                                             //=no operand instruction
        || (sourceOperandAddressingChosenMethod == directRegister && destinationOperandAddressingChosenMethod == directRegister)        //=dual operand instruction
        || (destinationOperandAddressingChosenMethod == directRegister && sourceOperandAddressingChosenMethod == nullAddressingMethods) //=single operand instruction

    )
    {
        storeSingleLineInstructionsToCodeArray(instructionModel, sourceOperand,
                                               destinationOperand);
    }
    // second if handles the option of a 2 code lines, with a single operand
    if ((destinationOperandAddressingChosenMethod == directLabel && sourceOperandAddressingChosenMethod == nullAddressingMethods) || (destinationOperandAddressingChosenMethod == immediate && sourceOperandAddressingChosenMethod == nullAddressingMethods) ||
        (destinationOperandAddressingChosenMethod == relativeLabel && sourceOperandAddressingChosenMethod == nullAddressingMethods))
    {
        storeTwoCodeLinesWithSingleOperand(instructionModel,
                                           destinationOperand);
        return;
    }
    // third if handles the option of a 2 code lines, with a 2 operands
    if (isOnlySingleRegisterlFromTwoOperands(destinationOperandAddressingChosenMethod, sourceOperandAddressingChosenMethod))
    {
        storeTwoCodeLinesFromTwoOperandInstruction(instructionModel,
                                                   destinationOperand, sourceOperand);
        return;
    }
    // 4th if handles the option of a 3 code lines, with a 2 operands
    if (isOperandRequireAcodeLine(destinationOperandAddressingChosenMethod) && isOperandRequireAcodeLine(sourceOperandAddressingChosenMethod))
    {
        storeThreeCodeLinesFromTwoOperandInstruction(instructionModel,
                                                     destinationOperand, sourceOperand);
        return;
    }
}

Bool isOperandRequireAcodeLine(AddressingMethods operandMethod)
{
    return operandMethod != directRegister && operandMethod != nullAddressingMethods;
}
Bool isOnlySingleRegisterlFromTwoOperands(AddressingMethods destMethod, AddressingMethods srcMethod)
{
    return (destMethod == directRegister && srcMethod != directRegister && srcMethod != nullAddressingMethods) ||
           (destMethod != directRegister && destMethod != nullAddressingMethods && srcMethod == directRegister);
}
void printCodeLine(const CodeLine *codeLine)
{
    if (codeLine->type == 0)
    {
        /* InstructionCodeLine */
        printf("Type: Instruction\n");
        printf("  ARE: %u\n", codeLine->data.instructionLine.binaryFields.ARE);
        printf("  Funct: %u\n", codeLine->data.instructionLine.binaryFields.funct);
        printf("  DestReg: %u\n", codeLine->data.instructionLine.binaryFields.destReg);
        printf("  DestAddress: %u\n", codeLine->data.instructionLine.binaryFields.destAddress);
        printf("  SourceReg: %u\n", codeLine->data.instructionLine.binaryFields.sourceReg);
        printf("  SourceAddress: %u\n", codeLine->data.instructionLine.binaryFields.sourceAddress);
        printf("  Opcode: %u\n", codeLine->data.instructionLine.binaryFields.opcode);
        printf("  IC: %d\n", codeLine->ic);
    }
    else if (codeLine->type == 1)
    {
        /* LabelOrImmediateBinaryCodeLine */
        printf("Type: Label/Immediate\n");
        printf("  ARE: %u\n", codeLine->data.labelOrImmediateLine.ARE);
        printf("codeLine->data.labelOrImmediateLine.isLabel");
        printf("%d", codeLine->data.labelOrImmediateLine.isLabel);

        if (codeLine->data.labelOrImmediateLine.isLabel)
        {
            printf("  labelName is");
            printf("  labelName: %s\n", (*codeLine->data.labelOrImmediateLine.nameOrImmediate.labelName));
        }
        else
        {
            printf("  Size is");

            printf("  Size: %u\n", codeLine->data.labelOrImmediateLine.nameOrImmediate.size);
        }
    }
    else
    {
        printf("Type: Unknown\n");
    }
}
void assignOperandLabelAREtoExternalIfNotInFile()
{
    Bool isFound;
    int codeLInesSize, symbolsSize;
    CodeLine **codeImage;
    LabelData *symbolsFinal;
    symbolsFinal = getSymbol_table();
    symbolsSize = getLabelsFileCounter();
    codeImage = getCodeImage();
    codeLInesSize = getCodeImageAlloc_count();
    if ((*codeImage) == NULL)
    {
        printf("Code image is empty.\n");
        return;
    }
    int index = 0;
    isFound = false;
    /* Iterate through the codeImage array until the terminator (type = -1) */
    while (index < codeLInesSize)
    {
        if ((*codeImage)[index].type == 1)
        {
            if ((*codeImage)[index].data.labelOrImmediateLine.isLabel)
            {
                for (size_t i = 0; i < symbolsSize && !isFound; i++)
                {
                    // printf("assignOperandLabelAREtoExternalIfNotInFile codeImage name  %s\n", *(*codeImage)[index].data.labelOrImmediateLine.nameOrImmediate.labelName);

                    // printf("assignOperandLabelAREtoExternalIfNotInFile symbolsFinal[i].name  %s\n", symbolsFinal[i].name);

                    if (strcmp(*(*codeImage)[index].data.labelOrImmediateLine.nameOrImmediate.labelName, symbolsFinal[i].name) == 0)
                    {
                        isFound = true;
                    }
                }
                if (!isFound)
                {
                    (*codeImage)[index].data.labelOrImmediateLine.ARE = 1;
                }
            }
        }
        index++;
        isFound = false;
    }
}
void printAllCodeLines()
{
    printf("printAllCodeLines\n");

    CodeLine **codeImage = getCodeImage();

    if ((*codeImage) == NULL)
    {
        printf("Code image is empty.\n");
        return;
    }

    int index = 0;

    /* Iterate through the codeImage array until the terminator (type = -1) */
    while (index < alloc_count)
    {

        printf("Code Line %d:\n", index + 100);

        if ((*codeImage)[index].type == 0)
        {
            /* InstructionCodeLine */
            printf("  Type: Instruction\n");
            printf("    ARE: %u\n", (*codeImage)[index].data.instructionLine.binaryFields.ARE);
            printf("    Funct: %u\n", (*codeImage)[index].data.instructionLine.binaryFields.funct);
            printf("    DestReg: %u\n", (*codeImage)[index].data.instructionLine.binaryFields.destReg);
            printf("    DestAddress: %u\n", (*codeImage)[index].data.instructionLine.binaryFields.destAddress);
            printf("    SourceReg: %u\n", (*codeImage)[index].data.instructionLine.binaryFields.sourceReg);
            printf("    SourceAddress: %u\n", (*codeImage)[index].data.instructionLine.binaryFields.sourceAddress);
            printf("    Opcode: %u\n", (*codeImage)[index].data.instructionLine.binaryFields.opcode);
            printf("    IC: %d\n", (*codeImage)[index].ic);
        }
        else if ((*codeImage)[index].type == 1)
        {
            /* LabelOrImmediateBinaryCodeLine */
            printf("  Type: Label/Immediate\n");
            printf("    ARE: %u\n", (*codeImage)[index].data.labelOrImmediateLine.ARE);
            // printf("  (*codeImage)[index].data.labelOrImmediateLine.isLabel is  \n");

            // printf("  (*codeImage)[index].data.labelOrImmediateLine.isLabel is %d\n", (*codeImage)[index].data.labelOrImmediateLine.isLabel);

            if ((*codeImage)[index].data.labelOrImmediateLine.isLabel)
            {
                //    printf("  labelName: %s\n", (*codeImage)[index].data.labelOrImmediateLine.nameOrImmediate.labelName);
                printf("  labelName: %s\n", *(*codeImage)[index].data.labelOrImmediateLine.nameOrImmediate.labelName);

                //   printf("  labelName: %s\n", (*((*codeImage)[index].data.labelOrImmediateLine.nameOrImmediate.labelName)));//original

                //  printf("  labelName: %s\n", (((*codeImage)[index].data.labelOrImmediateLine.nameOrImmediate.labelName)));// (null)
                //  printf("  labelName: %s\n", (((*codeImage)[index].data.labelOrImmediateLine.nameOrImmediate.labelName)));
            }
            else
            {
                printf("    Size: %u\n", (*codeImage)[index].data.labelOrImmediateLine.nameOrImmediate.size);
            }

            printf("    IC: %d\n", (*codeImage)[index].ic);
        }
        else
        {
            printf("  Unknown CodeLine type: %d\n", (*codeImage)[index].type);
        }

        printf("\n");
        index++;
    }
}
