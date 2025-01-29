#include <math.h>
#include "binary_code.h"

static const AddressingMethodsArray immediateDirectLabelDirectRegister = {
    .methods = {immediate, directLabel, directRegister}};
static const AddressingMethodsArray directLabelDirectRegister = {
    .methods = {directLabel, directRegister}};

static const AddressingMethodsArray immediateOperand = {
    .methods = {immediate}};
static const AddressingMethodsArray directLabelOperand = {
    .methods = {directLabel}};
static const AddressingMethodsArray directAndRelativeLabe = {
    .methods = {directLabel, relativeLabel}};

/* Function to validate if the string contains only digits or starts with '-' */
Bool isNumericString(const char *str)
{
    // Check if the string is empty
    if (*str == '\0')
    {
        return false; // An empty string is not numeric
    }

    // Check for an optional leading minus sign
    if (*str == '-')
    {
        str++; // Skip the minus sign
    }

    // Ensure the rest of the string contains only digits
    if (*str == '\0')
    {
        return false; // A single '-' without digits is invalid
    }

    while (*str)
    {
        if (!isdigit((unsigned char)*str))
        {
            return false; // Return 0 if a non-digit character is found
        }
        str++; // Move to the next character
    }

    return true; // Return 1 if the string is valid
}

InstructionData instructionsModels[INSTRUCTION_BINARY_MODELS_COUNT] /*INSTRUCTION_BINARY_MODELS_COUNT is not mandatory */ = {
    //-1 for yet unknown. -2 for irrelavent
    {
        // .binary = {4 /*ARE*/, 0, -1, -1, -1, -1, 0},
        .name = mov,
        .operandAmount = 2,
        .sourceAddressingMethods = &immediateDirectLabelDirectRegister,
        .destinationAddressingMethods = &directLabelDirectRegister},
    {//.binary = {4, 0, -1, -1, -1, -1, 1},
     .name = cmp,
     .operandAmount = 2,
     .sourceAddressingMethods = &immediateDirectLabelDirectRegister,
     .destinationAddressingMethods = &immediateDirectLabelDirectRegister},
    {//.binary = {4, 1, -1, -1, -1, -1, 2},
     .name = add,
     .operandAmount = 2,
     .sourceAddressingMethods = &immediateDirectLabelDirectRegister,
     .destinationAddressingMethods = &directLabelDirectRegister},

    {//.binary = {4, 2, -1, -1, -1, -1, 2},
     .name = sub,
     .operandAmount = 2,
     .sourceAddressingMethods = &immediateDirectLabelDirectRegister,
     .destinationAddressingMethods = &directLabelDirectRegister},

    {//.binary = {4, 0, -1, -1, 0, 1, 4},
     .name = lea,
     .operandAmount = 2,
     .sourceAddressingMethods = &directLabelOperand,
     .destinationAddressingMethods = &directLabelDirectRegister},

    {//.binary = {4, 1, -1, -1, -2, -2, 5},
     .name = clr,
     .operandAmount = 1,
     .sourceAddressingMethods = NULL,
     .destinationAddressingMethods = &directLabelDirectRegister},

    {//.binary = {4, 2, -1, -1, -2, -2, 5},
     .name = not,
     .operandAmount = 1,
     .destinationAddressingMethods = &directLabelDirectRegister},

    {//.binary = {4, 3, -1, -1, -2, -2, 5},
     .name = inc,
     .operandAmount = 1,
     .sourceAddressingMethods = NULL,
     .destinationAddressingMethods = &directLabelDirectRegister},
    {//.binary = {4, 4, -1, -1, -2, -2, 5},
     .name = dec,
     .operandAmount = 1,
     .sourceAddressingMethods = NULL,
     .destinationAddressingMethods = &directLabelDirectRegister},

    {//.binary = {4, 1, -1, -1, -2, -2, 9},
     .name = jmp,
     .operandAmount = 1,
     .sourceAddressingMethods = NULL,
     .destinationAddressingMethods = &directAndRelativeLabe},
    {//.binary = {4, 2, -1, -1, -2, -2, 9},
     .name = bne,
     .operandAmount = 1,
     .sourceAddressingMethods = NULL,
     .destinationAddressingMethods = &directAndRelativeLabe},
    {//.binary = {4, 3, -1, -1, -2, -2, 9},
     .name = jsr,
     .operandAmount = 1,
     .sourceAddressingMethods = NULL,
     .destinationAddressingMethods = &directAndRelativeLabe},
    {//.binary = {4, 0, -1, -1, -2, -2, 12},
     .name = red,
     .operandAmount = 1,
     .destinationAddressingMethods = &directLabelDirectRegister},
    {//.binary = {4, 0, -1, -1, -2, -2, 13},
     .name = prn,
     .operandAmount = 1,
     .sourceAddressingMethods = NULL,
     .destinationAddressingMethods = &immediateDirectLabelDirectRegister},
    {//.binary = {4, 0, -2, -2, -2, -2, 14},
     .name = rts,
     .operandAmount = 0,
     .sourceAddressingMethods = NULL,
     .destinationAddressingMethods = NULL},

    {//.binary = {4, 0, -2, -2, -2, -2, 15},
     .name = stop,
     .operandAmount = 0,
     .sourceAddressingMethods = NULL,
     .destinationAddressingMethods = NULL}};

InstructionBinaryData instructionsBinaryImage[INSTRUCTION_BINARY_MODELS_COUNT] = {
    {.binary = {4 /*ARE*/, 0, -1, -1, -1, -1, 0},
     .name = mov},
    {.binary = {4, 0, -1, -1, -1, -1, 1},
     .name = cmp},
    {.binary = {4, 1, -1, -1, -1, -1, 2},
     .name = add},
    {.binary = {4, 2, -1, -1, -1, -1, 2},
     .name = sub},
    {.binary = {4, 0, -1, -1, 0, 1, 4},
     .name = lea},
    {.binary = {4, 1, -1, -1, -2, -2, 5},
     .name = clr},
    {.binary = {4, 2, -1, -1, -2, -2, 5},
     .name = not },
    {.binary = {4, 3, -1, -1, -2, -2, 5},
     .name = inc},
    {.binary = {4, 4, -1, -1, -2, -2, 5},
     .name = dec},
    {.binary = {4, 1, -1, -1, -2, -2, 9},
     .name = jmp},
    {.binary = {4, 2, -1, -1, -2, -2, 9},
     .name = bne

    },
    {.binary = {4, 3, -1, -1, -2, -2, 9},
     .name = jsr

    },
    {.binary = {4, 0, -1, -1, -2, -2, 12},
     .name = red

    },
    {.binary = {4, 0, -1, -1, -2, -2, 13},
     .name = prn

    },
    {.binary = {4, 0, 0, 0, 0, 0, 14},
     .name = rts

    },
    {.binary = {4, 0, 0, 0, 0, 0, 15},
     .name = stop}};
int stringToInt(const char *str)
{
    int result = 0; // To store the final integer value
    int sign = 1;   // To handle negative numbers

    // Check for an optional leading minus sign
    if (*str == '-')
    {
        sign = -1; // Set the sign to negative
        str++;     // Move to the next character
    }

    // Iterate through each character of the string
    while (*str)
    {
        if (*str >= '0' && *str <= '9')
        {
            result = result * 10 + (*str - '0'); // Build the number
        }
        else
        {
            // If a non-digit character is encountered, return an error indicator
            return -1; // Use -1 to indicate invalid input
        }
        str++; // Move to the next character
    }

    return result * sign; // Apply the sign and return the result
}
int countDigits(int num)
{
    if (num == 0)
        return 1; // Special case for 0
    if (num < 0)
        num = -num; // Handle negative numbers
    return (int)log10(num) + 1;
}

//! checks are needed
/* Function to convert a 24-bit binary string to its hexadecimal representation */
void binaryToHex24(const char *binaryStr, char *hexOutput)
{
    if (strlen(binaryStr) != 24)
    {
        fprintf(stderr, "Error: Input string must be exactly 24 binary digits.\n");
        hexOutput[0] = '\0'; // Ensure hexOutput is empty in case of error
        return;
    }

    unsigned int decimalValue = 0;

    // Convert binary string to decimal value
    for (size_t i = 0; i < 24; i++)
    {
        if (binaryStr[i] == '1')
        {
            decimalValue = (decimalValue << 1) | 1; // Add a 1 to the least significant bit
        }
        else if (binaryStr[i] == '0')
        {
            decimalValue = decimalValue << 1; // Add a 0 to the least significant bit
        }
        else
        {
            fprintf(stderr, "Error: Input string contains non-binary character '%c'.\n", binaryStr[i]);
            hexOutput[0] = '\0'; // Ensure hexOutput is empty in case of error
            return;
        }
    }

    // Convert the decimal value to a hexadecimal string
    sprintf(hexOutput, "%06X", decimalValue); // %06X ensures 6 characters, padded with zeros if necessary
}
//! to above
// int main() {
//     const char *binaryStr = "110101100101101111001010"; // Example 24-bit binary string
//     char hexOutput[7]; // 6 characters for the hexadecimal + 1 for null terminator

//     binaryToHex24(binaryStr, hexOutput);

//     if (hexOutput[0] != '\0') { // Only print if conversion was successful
//         printf("Binary: %s\n", binaryStr);
//         printf("Hexadecimal: %s\n", hexOutput);
//     }

//     return 0;
// }
