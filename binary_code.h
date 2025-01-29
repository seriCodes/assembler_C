#include "assembler.h"

#ifndef BINARY_CODE_H
#define BINARY_CODE_H
#define ABSOLUTE 4
#define INSTRUCTION_BINARY_MODELS_COUNT 16
#define OPERAND_DELIMITER ','
#define LABEL_IMMEDIATE 1
#define INSTRUCTION_LINE 0
#define WORD_SIZE 24

typedef struct
{
    unsigned int ARE : 3;
    unsigned int funct : 5;
    unsigned int destReg : 3;
    unsigned int destAddress : 2;
    unsigned int sourceReg : 3;
    unsigned int sourceAddress : 2;
    unsigned int opcode : 6;

} InstructionBinaryModel;
typedef struct
{
    unsigned int ARE : 3;
    unsigned int isLabel : 1; // 1 if label. 0 for other
    union
    {
        unsigned int size : 21;
        //  unsigned int : 11;
        char **labelName;
    } nameOrImmediate;

} LabelOrImmediateBinaryCodeLine;
typedef struct
{
    InstructionBinaryModel binaryFields;

} InstructionCodeLine;

/* Tagged Union */
typedef struct
{
    int ic;

    int type; // 0 for instruction, 1 for other
    union
    {
        LabelOrImmediateBinaryCodeLine labelOrImmediateLine;
        InstructionCodeLine instructionLine;
    } data;
} CodeLine;

typedef enum
{
    nullAddressingMethods, // Null or undefined value
    immediate,
    directLabel,
    relativeLabel,
    directRegister
} AddressingMethods;
typedef enum
{
    immediateMethodVal,
    directLabelMethodVal,
    relativeLabelMethodVal,
    directRegisterMethodVal
} AddressingMethodsValues;
typedef struct
{
    AddressingMethods methods[3];
} AddressingMethodsArray;

typedef struct
{
    // InstructionBinaryModel binary;
    InstructionName name;
    int operandAmount;
    const AddressingMethods *sourceAddressingMethods;
    const AddressingMethods *destinationAddressingMethods;

} InstructionData;
extern InstructionData instructionsModels[INSTRUCTION_BINARY_MODELS_COUNT]; // THIS IS MANDATORY FOR THE COMPILER TO CALCUALTE THE sizeof(instructionsModels) in other files
typedef struct
{
    InstructionBinaryModel binary;
    InstructionName name;

} InstructionBinaryData;
extern InstructionBinaryData instructionsBinaryImage[INSTRUCTION_BINARY_MODELS_COUNT]; // THIS IS MANDATORY FOR THE COMPILER TO CALCUALTE THE sizeof(instructionsModels) in other files

void handleInsrtuction(wordType result, FILE *fd, int lastCharacter);
InstructionBinaryModel writeFixedBinaryToInstructionModel(InstructionName name);

Bool isNumericString(const char *str);
void printAllCodeLines();
void assignOperandLabelAREtoExternalIfNotInFile();
int stringToInt(const char *str);
void resetLabelOperandCounterAndFreeLabelOperandCounter();
CodeLine **getCodeImage();
int getCodeImageAlloc_count();
void freeCodeImage();
int *getIc();
void updateIc(int c);
void resetIc();
int *getDc();
void updateDc(int c);
void resetDc();
int countDigits(int num);
#endif
