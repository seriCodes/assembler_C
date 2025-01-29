#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h> // for size_t
#include "assembler.h"
#include "binary_code.h"

#define MAX_ALLOCATIONS 100
void *allocations[MAX_ALLOCATIONS] = {0}; // Array to track allocations

static int alloc_count = 0;
static char errorReport[MAX_ARG_STRING_LENGTH];
char *invalidName = "invalid name";
char *macroString = "Macro ";
char *InstructionString = "Instruction ";
char *error = "error ";
char *atLine = "at line:";
char *atFile = "At file:";
// char *mcroKeyWord = "mcro";

// Instruction nullInstruction = {none, -1, -1, {-1, -1, -1, -1}, {-1, -1, -1, -1}};

Instruction *getInstruction(int index)
{
    static Bool alreadyInitialized = false; // Tracks if modified outside main
                                            //   printf(" instructionHandler  .\n");
    //   static Instruction instructions[16];
    static Instruction instructions[16];

    if (!alreadyInitialized)
    {
        //   printf(" instructionHandler !alreadyInitialized.\n");

        alreadyInitialized = true;
        //   printf("Initializing instructions array.\n");
        instructions[0] = (Instruction){mov, 0, 0, {0, 1, 3, -1}, {1, 3, -1, -1}};
        instructions[1] = (Instruction){cmp, 0, 1, {0, 1, 3, -1}, {0, 1, 3, -1}};
        instructions[2] = (Instruction){add, 2, 2, {0, 1, 3, -1}, {1, 3, -1, -1}};
        instructions[3] = (Instruction){sub, 2, 3, {0, 1, 3, -1}, {1, 3, -1, -1}};
        instructions[4] = (Instruction){lea, 0, 4, {1, -1, -1, -1}, {1, 3, -1, -1}};
        instructions[5] = (Instruction){clr, 1, 5, {-1, -1, -1, -1}, {1, 3, -1, -1}};
        instructions[6] = (Instruction){not, 2, 5, {-1, -1, -1, -1}, {1, 3, -1, -1}};
        instructions[7] = (Instruction){inc, 3, 5, {-1, -1, -1, -1}, {1, 3, -1, -1}};
        instructions[8] = (Instruction){dec, 4, 5, {-1, -1, -1, -1}, {1, 3, -1, -1}};
        instructions[9] = (Instruction){bne, 2, 9, {-1, -1, -1, -1}, {1, 2, -1, -1}};
        instructions[10] = (Instruction){jmp, 1, 9, {-1, -1, -1, -1}, {1, 2, -1, -1}};
        instructions[11] = (Instruction){jsr, 3, 9, {-1, -1, -1, -1}, {1, 2, -1, -1}};
        instructions[12] = (Instruction){red, 0, 12, {-1, -1, -1, -1}, {1, 3, -1, -1}};
        instructions[13] = (Instruction){prn, 0, 13, {-1, -1, -1, -1}, {0, 1, 3, -1}};
        instructions[14] = (Instruction){rts, 0, 14, {-1, -1, -1, -1}, {-1, -1, -1, -1}};
        instructions[15] = (Instruction){stop, 0, 15, {-1, -1, -1, -1}, {-1, -1, -1, -1}};
        //   printf(" instructionHandler out if.\n");
    }
    if (index < 0 || index >= 16)
    {
        return NULL; // Out-of-bounds access
    }

    return &instructions[index]; // Return pointer to the requested instruction
}
const char *InstructionsStrings[] = {
    "nullInstruction",
    "mov",
    "cmp",
    "add",
    "sub",
    "lea",
    "clr",
    "not",
    "inc",
    "dec",
    "jmp",
    "bne",
    "jsr",
    "red",
    "prn",
    "rts",
    "stop"};

char *setGetFileName(char *newValue, Bool isMainFunction)
{
    static char fileNname[MAX_ARG_STRING_LENGTH];
    if (isMainFunction)
    {
        memset(fileNname, '\0', MAX_ARG_STRING_LENGTH);
        strncpy(fileNname, newValue, MAX_ARG_STRING_LENGTH - 1);
    }
    if (!isMainFunction)
    {

        return fileNname;
    }
} // fileNname[MAX_ARG_STRING_LENGTH - 1] = '\0'; // Null-terminate

void *malloc_wrapper(size_t size)
{
    if (alloc_count >= MAX_ALLOCATIONS)
    {
        fprintf(stderr, "Too many allocations!\n");
        exit(1);
    }
    void *ptr = malloc(size);
    //  printf("before memset malloc_wrapper, ptr:\n%p \n", ptr);
    if (ptr)
    {
        memset(ptr, 0, size); // Initialize to zero. Prevents from garbage values to be present at the used heap section. Prevents unwanted characters to be written in the second (or other than the first) output file.

        allocations[alloc_count++] = ptr;
    }
    //    printf("after memset malloc_wrapper, ptr:\n%p \n", ptr);
    return ptr;
}

void *malloc_wrapper_with_passed_storage(size_t size, void *pointers_allocations_storage[])
{
    int len = strlen(pointers_allocations_storage);
    printf("malloc_wrapper_with_passed_storage pointers_allocations_storage len %d.\n", len);

    if (len >= MAX_SYMBOLS_ALLOCATIONS)
    {
        fprintf(stderr, "Too many allocations in pointers_allocations_storage!\n");
        exit(1);
    }
    void *ptr = malloc(size);
    //  printf("before memset malloc_wrapper, ptr:\n%p \n", ptr);
    if (ptr)
    {
        memset(ptr, 0, size); // Initialize to zero. Prevents from garbage values to be present at the used heap section. Prevents unwanted characters to be written in the second (or other than the first) output file.

        pointers_allocations_storage[len] = ptr;
    }
    //    printf("after memset malloc_wrapper, ptr:\n%p \n", ptr);
    return ptr;
}

void free_all()
{
    printf("free_all  \n");

    for (int i = 0; i < alloc_count; i++)
    {
        if (allocations[i])
        {
            free(allocations[i]);
            allocations[i] = NULL;
        }
    }
    alloc_count = 0;
}
void updateAllocCount(int num)
{
    alloc_count = alloc_count + num;
}
void free_wrapper(void *ptr)
{
    for (int i = 0; i < alloc_count; i++)
    {
        if (allocations[i] == ptr)
        {
            free(ptr);
            allocations[i] = NULL;
            return;
        }
    }
    fprintf(stderr, "Attempted to free untracked pointer!\n");
}
void *realloc_wrapper(void *ptr, size_t new_size)
{
    // If the pointer is NULL, behave like malloc
    if (ptr == NULL)
    {
        return malloc_wrapper(new_size);
    }

    // If the new size is 0, behave like free
    if (new_size == 0)
    {
        free_wrapper(ptr);
        return NULL;
    }

    // Find the pointer in the allocations array
    for (int i = 0; i < alloc_count; i++)
    {
        // printf("realloc_wrapper for \n");
        // printf("i is %d \n", i);

        if (allocations[i] == ptr)
        {
            // printf("allocations[i] == ptr \n");
            // printf("new_size %d \n", new_size);
            // printf("ptr %p \n", ptr);

            // Reallocate memory
            void *new_ptr = realloc(ptr, new_size);
            // printf("after realloc\n");

            if (new_ptr)
            {
                allocations[i] = new_ptr; // Update the tracked pointer
                                          // printf("allocations[i] = new_ptr %p \n", new_ptr);
            }
            else
            {
                fprintf(stderr, "Failed to realloc memory!!\n");
            }
            return new_ptr;
        }
    }

    // If the pointer was not found in the tracked list
    fprintf(stderr, "Attempted to realloc untracked pointer!\n");
    return NULL;
}

void *realloc_wrapper_with_passed_storage(void *ptr, size_t new_size, void *pointers_allocations_storage[], int pointers_allocations_storage_counter)
{
    // If the pointer is NULL, behave like malloc
    if (ptr == NULL)
    {
        return malloc_wrapper_with_passed_storage(new_size, pointers_allocations_storage); //!
    }

    // If the new size is 0, behave like free
    if (new_size == 0)
    {
        free_wrapper(ptr); //!
        return NULL;
    }

    int len = sizeof(pointers_allocations_storage);
    printf("realloc_wrapper_with_passed_storage len %d \n", len);
    // Find the pointer in the allocations array
    for (int i = 0; i < sizeof(pointers_allocations_storage); i++)
    {
        printf("realloc_wrapper for \n");
        printf("i is %d \n", i);
        if (pointers_allocations_storage[i] == ptr)
        {
            printf("allocations[i] == ptr \n");
            printf("new_size %d \n", new_size);
            printf("ptr %p \n", ptr);
            // Reallocate memory
            void *new_ptr = realloc(ptr, new_size);
            printf("after realloc\n");

            if (new_ptr)
            {
                pointers_allocations_storage[i] = new_ptr; // Update the tracked pointer
                printf("pointers_allocations_storage[i] = new_ptr %p \n", new_ptr);
            }
            else
            {
                fprintf(stderr, "Failed to realloc memory!!\n");
            }
            return new_ptr;
        }
    }

    // If the pointer was not found in the tracked list
    fprintf(stderr, "Attempted to realloc untracked pointer!\n");
    return NULL;
}

int myExternVar = 59;
const char *KeyStrings[] = {
    "mcro",
    "mcroend",
    "right",
    "left",
    "Value for Key Three"};
const char *DirectiveStrings[] = {
    ".data",
    ".string",
    ".entry",
    ".extern"};

void printCharArrayWithNulls(const char *array, size_t size)
{
    // printf("\n printCharArrayWithNulls \n");
    // printf(" str before loop printCharArrayWithNulls%s\n", array);

    for (size_t i = 0; i < size; i++)
    {
        // printf("printCharArrayWithNulls   in \\x%02X \n", array[i]); // Hexadecimal
        //  printf("for (size_t i %d\n", i);
        if (array[i] == '\0')
        {
            printf("\\0"); // Represent null terminator as \0
        }
        else if (array[i] == '\n')
        {
            printf("\\n"); // Represent newline as \n
        }
        else if (array[i] == '\t')
        {
            printf("\\t"); // Represent tab as \t
        }
        else if (array[i] == ' ')
        {
            printf("\\space"); // Represent space as \space
        }
        else if (array[i] == '\r')
        {
            printf("\\r "); // Represent newline as \n
        }
        else
        {
            printf("%c", array[i]);
            // putchar(array[i]); // Print the character as-is
        }
    }
    printf("\n");
}

void printCharWithNulls(char *ch)
{
    printf("printCharWithNulls\n");

    if (ch == '\0')
    {
        printf("\\0"); // Represent null terminator as \0
    }
    else if (ch == '\n')
    {
        printf("\\n"); // Represent newline as \n
    }
    else if (ch == '\t')
    {
        printf("\\t"); // Represent tab as \t
    }
    else if (ch == ' ')
    {
        printf("\\space"); // Represent space as \space
    }
    else if (ch == '\r')
    {
        printf("\\r "); // Represent newline as \n
    }
    /*else if (ch == 'ÿ')//does not work
    {
        printf("EOF"); // Represent newline as \n
    }*/
    else
    {
        printf("%c", ch);
        // putchar(array[i]); // Print the character as-is
    }

    printf("\n");
}
void test()
{

    printf("Thiss is a shared function.\n");
}
void forwardPointerToNullTerminator(char **str)
{
    // this modifies the caller' pointer

    if (*str == NULL)
    {
        return; // Do nothing for NULL pointer
    }
    // Move to the end of the string
    while (**str != '\0')
    {
        (*str)++;
    }
}
void removeTrailingWhitespaces(char *str)
{

    char *end = str;
    forwardPointerToNullTerminator(&end);
    printf("after forwardPointerToNullTerminator %c\n ", *(end));

    end--;
    //    printf("after  end-- forwardPointerToNullTerminator %c\n ", *(end));

    // Move backward to find the first non-whitespace character
    while (end > str && (isspace((unsigned char)*(end)) || *(end) == '\r'))
    {
        // while (end > str && (isspace((unsigned char)*(end - 1)) || (unsigned char)*(end - 1) == '\r')) {

        //  printf("ace((unsigned cha (end - 1) \n %c", *(end));
        //  printCharArrayWithNulls((end), 12);
        //*end = '\0';
        *(end) = '\0';
        end--;
    }

    // Null-terminate the string after the last non-whitespace character
    // *end = '\0';
}

char *copyCharArrayWithExtraSpace(const char *source, int extarSpace)
{
    if (source == NULL)
    {
        return NULL; // Return NULL for invalid input
    }

    // Allocate memory for the copy, including space for the null terminator
    size_t length = strlen(source) + extarSpace + 1; // +1 for '\0'
    char *copy = (char *)calloc(length, sizeof(char));
    if (copy == NULL)
    {
        perror("Memory allocation failed");
        return NULL; // Return NULL if memory allocation fails
    }

    // Copy the contents of the source array to the new array
    strcpy(copy, source);

    return copy; // Return the copied array
}
void moveToNextLine(FILE *fd)
{
    int ch;
    // Read characters until you find a newline or reach EOF
    while ((ch = fgetc(fd)) != EOF)
    {
        if (ch == '\n')
        {
            break;
        }
    }
}

void resetCharArray(char *array, size_t size)
{
    if (array == NULL)
    {
        return; // Handle null pointers gracefully
    }
    for (size_t i = 0; i < size; i++)
    {
        array[i] = '\0'; // Set each element to '\0'
    }
}
char skipOverFollowingNewLinesAndTabsAndSpacesAndReturnCarriage(FILE *fd)
{
    int ch;
    while ((ch = fgetc(fd)) == '\n' || ch == ' ' || ch == '\t' || ch == '\r')
        ;
    return ch;
}
char skipOverTabsAndSpaces(FILE *fd)
{
    int ch;

    while ((ch = fgetc(fd)) == ' ' || ch == '\t')
        ;
    return ch;
}
void remove_all_whitespace(char *str) // removes all whitespace characters from a null-terminated string in-place
{
    char *src = str, *dst = str;

    while (*src != '\0')
    {
        // If the current character is not a whitespace, copy it
        if (!isspace((unsigned char)*src))
        {
            *dst++ = *src;
        }
        src++;
    }

    // Null-terminate the modified string
    *dst = '\0';
}

void *fetchNoneWhiteCharachtersString(FILE *fd, char **str) // i.e., *str refers to mcroName
{
    int ch, i = 0;
    // char *str;
    ch = skipOverTabsAndSpaces(fd);
    // ch = fgetc(fd);
    //  printf("skipOverTabsAndSpaces  acc ch :%c i,%d\n", ch, i);

    // if (*str == NULL)
    //  {
    *str = (char *)malloc_wrapper(sizeof(char));
    if (*str == NULL)
    {
        perror("Memory allocation failed");
        return NULL;
    }
    //   }

    while (!feof(fd) && ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r') // ch is ' ' or \t or \n in the end of macro name
    {
        //    printf("mcro name  acc ch :%c i:%d\n", ch, i);
        *str = (char *)realloc_wrapper(*str, sizeof(char) * (i + 1));
        (*str)[i] = ch;
        //  printf("before *str == NULL \n");
        if (*str == NULL)
        {
            perror("Memory reallocation failed");
            return NULL;
        }
        //  printf("after *str == NULL \n");

        //  printf("mcro name  acc i,%d\n", i);

        //  printf("mcro name  acc str[i]:%c \n", (*str)[i]);

        i++;
        ch = fgetc(fd);
    }

    *str = (char *)realloc_wrapper(*str, sizeof(char) * (i + 1));
    (*str)[i] = '\0';
    // free(*str);
    // return str;
}
const char *resolveDirectiveType(char *input)
{
    //   printf("in resolveDirectiveType %s \n", input);
    if (strcmp(input, DirectiveStrings[data]) == 0)
    {
        return DirectiveStrings[data];
    }
    else if (strcmp(input, DirectiveStrings[string]) == 0)
    {
        return DirectiveStrings[string];
    }
    else if (strcmp(input, DirectiveStrings[entry]) == 0)
    {
        return DirectiveStrings[entry];
    }
    else if (strcmp(input, DirectiveStrings[externDirective]) == 0)
    {
        return DirectiveStrings[externDirective];
    }
    else
    {
        // Default case
        //    printf(" resolveDirectiveType Default case  \n");

        return NULL;
    }
}
const char *resolveInstructionType(char *input)
{
    printf("in resolveInstructionType %s \n", input);
    //  printf("in resolveInstructionType InstructionsStrings[mov] %s \n", InstructionsStrings[mov]);

    if (strcmp(input, InstructionsStrings[mov]) == 0)
    {
        return InstructionsStrings[mov];
    }
    else if (strcmp(input, InstructionsStrings[cmp]) == 0)
    {
        return InstructionsStrings[cmp];
    }
    else if (strcmp(input, InstructionsStrings[add]) == 0)
    {
        return InstructionsStrings[add];
    }
    else if (strcmp(input, InstructionsStrings[sub]) == 0)
    {
        return InstructionsStrings[sub];
    }
    else if (strcmp(input, InstructionsStrings[lea]) == 0)
    {
        return InstructionsStrings[lea];
    }
    else if (strcmp(input, InstructionsStrings[clr]) == 0)
    {
        return InstructionsStrings[clr];
    }
    else if (strcmp(input, InstructionsStrings[not ]) == 0)
    {
        return InstructionsStrings[not ];
    }
    else if (strcmp(input, InstructionsStrings[inc]) == 0)
    {
        return InstructionsStrings[inc];
    }
    else if (strcmp(input, InstructionsStrings[dec]) == 0)
    {
        return InstructionsStrings[dec];
    }
    else if (strcmp(input, InstructionsStrings[jmp]) == 0)
    {
        return InstructionsStrings[jmp];
    }
    else if (strcmp(input, InstructionsStrings[bne]) == 0)
    {
        return InstructionsStrings[bne];
    }
    else if (strcmp(input, InstructionsStrings[jsr]) == 0)
    {
        return InstructionsStrings[jsr];
    }
    else if (strcmp(input, InstructionsStrings[red]) == 0)
    {
        return InstructionsStrings[red];
    }
    else if (strcmp(input, InstructionsStrings[prn]) == 0)
    {
        return InstructionsStrings[prn];
    }
    else if (strcmp(input, InstructionsStrings[rts]) == 0)
    {
        return InstructionsStrings[rts];
    }
    else if (strcmp(input, InstructionsStrings[stop]) == 0)
    {
        return InstructionsStrings[stop];
    }
    else
    {
        // Default case
        printf(" resolveInstructionType Default case  \n");
        return NULL;
    }
}

void getErrorReportCopy(char *str)
{

    strncpy(str, errorReport, MAX_ARG_STRING_LENGTH);
}
void setStartErrorReport(char *str)
{
    char *fileName = setGetFileName("", false);
    memset(errorReport, '\0', MAX_ARG_STRING_LENGTH);
    strncat(errorReport, atFile, MAX_ARG_STRING_LENGTH);
    strncat(errorReport, fileName, MAX_ARG_STRING_LENGTH);
    strncat(errorReport, ". ", MAX_ARG_STRING_LENGTH);
    strncat(errorReport, str, MAX_ARG_STRING_LENGTH);
    strncat(errorReport, error, MAX_ARG_STRING_LENGTH);
    strncat(errorReport, atLine, MAX_ARG_STRING_LENGTH);

    // printf("errorReport %s \n", errorReport);
}
int charToInt(char ch)
{
    if (ch >= '0' && ch <= '9')
    {
        return ch - '0'; // Convert character to integer
    }
    return -1; // Return -1 for non-digit characters
}

void setLineErrorReport()
{
    // printf("in setStartErrorReport %s \n", str);
    int lineNum = getLineNum();
    char currentLine[CURRENT_LINE];
    snprintf(currentLine, CURRENT_LINE, "%d ", lineNum); // Safely write the integer into the string
    strncat(errorReport, currentLine, MAX_ARG_STRING_LENGTH);

    // printf("errorReport %s \n", errorReport);
}
int stringAccumulationDetectionWithRemoveWhiteSpaces(char stringAccumulator[], Key key)
{
    int res;
    int count = strlen(KeyStrings[key]);
    char *str1 = (char *)malloc((count + 1) * sizeof(char));

    remove_all_whitespace(stringAccumulator); // if I recall correctly the last char will be a whitespace because of the while loop in main.
                                              //  printf("stringAccumulator out %s\n", stringAccumulator);

    if (count != strlen(stringAccumulator)) // to avoid checking code that starts with the string mcro but is not a mcro. E.g mcroend.
    {
        //   printf("\ncount != strlen(stringAccumulator\n");
        free(str1);

        return -1;
    }
    str1[count] = '\0'; // end of string
    for (size_t i = 0; i < count; i++)
    {
        /* if(str1[i]=stringAccumulator[][]){
         }
         ;*/
        str1[i] = stringAccumulator[i];
    }
    // explain here why you made a copy above to yourself. and why you don;t send stringAccumulator to memcmp.
    res = memcmp(str1, KeyStrings[key], count);
    //  printf("res %d \n", res);
    free(str1);

    return res;
}
long saveFilePosition(FILE *fd)
{
    long savedPosition = ftell(fd);
    if (savedPosition == -1L)
    {
        perror("ftell failed");
        return;
    }
    // printf("Saved file position: %ld\n", savedPosition);
    return savedPosition;
}
void restoreFilePosition(FILE *fd, long savedPosition)
{
    if (fseek(fd, savedPosition, SEEK_SET) != 0)
    {
        perror("fseek failed");
        return;
    }
    printf("File position restored to: %ld\n", savedPosition);
}

int isAtStartOfFile(FILE *fd)
{
    //   printf("isAtStartOfFile\n");

    return ftell(fd) == 0;
}
void isTheContinueOfTheLineHasNoneWhiteCharsBeforeLineFeed(FILE *fd, Key key)
{ // tabs and spaces are okay
  //   printf("isTheContinueOfTheLineHasNoneWhiteCharsBeforeLineFeed\n");
    int ch = fgetc(fd);
    if (KeyStrings[key] == "right")
    {
        while (ch != '\n')
        {
            if (isspace((unsigned char)ch))
            {
                ch = fgetc(fd);
                continue;
            }
            //  printf("the mcaro is not correct from %s side\n", KeyStrings[key]);
            break;
            // printf("isspace((unsigned char)ch\n");

            // fseek(fd, step, SEEK_CUR);

            // ungetc(ch, fd);
        }
    }
}
void moveFilePointerToTheStartOfTheLine(FILE *fd)
{
    int ch;
    ch = fgetc(fd); // for the first loop iteration
                    //   printf("moveFilePointerToTheStartOfTheLine start %c \n", ch);

    ungetc(ch, fd);
    if (ch == '\n') // in case fd is the last character of the line. which is \n
    {
        //   printf("moveFilePointerToTheStartOfTheLine if (ch ==  %c \n", ch);
        fseek(fd, -1, SEEK_CUR);
        ch = fgetc(fd);
        ungetc(ch, fd);

        // printf("moveFilePointerToTheStartOfTheLine if (ch ==  end %c \n", ch);
    }
    //  printf("moveFilePointerToTheStartOfTheLine  if (ch ==  out %c \n", ch);

    while (ch != '\n')
    {
        //     printf("moveFilePointerToTheStartOfTheLine %c \n", ch);
        fseek(fd, -1, SEEK_CUR);
        ch = fgetc(fd);
        ungetc(ch, fd);
        if (isAtStartOfFile(fd)) // handling the start of the file
        {
            // printf("moveFilePointerToTheStartOfTheLine isAtStartOfFile %c \n", ch);
            return;
        }
    }
    fgetc(fd);
    // printf("moveFilePointerToTheStartOfTheLine ending %c \n", ch);
}
Bool isLetter(char letter)
{
    if (letter == NULL)
    {
        printf("Input is NULL.\n");
        return;
    }
    return ((letter >= 'A' && letter <= 'Z') || (letter >= 'a' && letter <= 'z'));

    /*if (letter >= 'A' && letter <= 'Z')
    {
        printf("The character '%c' is a letter.\n", letter);
        return true;
    }
    else
    {
        printf("The character '%c' is not letter.\n", letter);
        return false;
    }*/
}

int lineRemainingWordLimit(FILE *left_Char, FILE *right_Char, int amount)
{
    //   printf("lineRemainingWordLimit lineNum: %d \n", lineNum);
    int ch_right, ch_Left, count = 0;
    ch_Left = fgetc(left_Char); // for the first loop iteration
    printf("lineRemainingWordLimit ch_Left %c \n", ch_Left);

    /*if (isAtStartOfFile(left_Char) && !isspace((unsigned char)ch_Left)) // handling the start of the file
     {
         printf("lineRemainingWordLimit isAtStartOfFile ch_Left ch_Left %c \n", ch_Left);
         count++;
     }*/
    count++; // counting the first word.
             //  printf("lineRemainingWordLimit count %d \n", count);

    ch_right = fgetc(right_Char);
    ch_right = fgetc(right_Char);

    printf("lineRemainingWordLimit isAtStartOfFile ch_right %c \n", ch_right);
    setLineErrorReport();
    while (ch_right != '\n' && !feof(right_Char))
    {
        //  printf(" while lineRemainingWordLimit count %d \n", count);
        //  printf(" while lineRemainingWordLimit ch_Left %c \n", ch_Left);
        // printf(" while lineRemainingWordLimit ch_right %c \n", ch_right);

        if ((isspace((unsigned char)ch_Left)) && !isspace((unsigned char)ch_right))
        {
            count++;
        }
        if (count > amount)
        {
            //   printf("\n %d lineNum  \n", lineNum);
            updateShouldNotCreateFile(true, false);

            //   printf("\n %d shouldNotCreateFile in  count > amount\n", shouldNotCreateFile);

            strncat(errorReport, " too many arguments.", MAX_ARG_STRING_LENGTH);
            char *fileName = setGetFileName("", false);

            printf("At file: %s, %s\n", fileName, errorReport);
            // print error and cancel process and move to next file
            // perror

            return FAILURE;
        }
        ch_Left = fgetc(left_Char);
        ch_right = fgetc(right_Char);
    }
    printf(" amount %d \n", amount);
    printf(" count %d \n", count);

    if (count < amount)
    {
        printf(" count < amount\n");
        strncat(errorReport, " not enough arguments.", MAX_ARG_STRING_LENGTH);
        char *fileName = setGetFileName("", false);

        printf("%s \n", errorReport);

        updateShouldNotCreateFile(true, false);

        return FAILURE;
    }
    // updateShouldNotCreateFile(false, false);// will not be here becaise the default is true
    //  printf("count = amount \n");

    return SUCCESS;
}
void removeStringEndingWhitespace(char *str)
{
    if (str == NULL)
    {
        return; // Do nothing if the input is NULL
    }

    char *end = str;

    // Move to the end of the string
    while (*end != '\0')
    {
        end++;
    }

    // Step back to the last character
    end--;

    // Trim trailing whitespaces
    while (end >= str && isspace((unsigned char)*end))
    {
        *end = '\0';
        end--;
    }
}
void removeSidesWhiteSpaces(char *str)
{
    removeStringEndingWhitespace(str);
    // printf(" removeStringEndingWhitespace\n");

    // printCharArrayWithNulls(str, 25);

    // removeTrailingWhitespaceFixed(str);
    removeLeadingWhitespace(str);
}
int isOnlyWhitespaceOrNull(char *str)
{
    if (str == NULL)
    {
        return 1; // Consider NULL strings as "only whitespace or null"
    }

    while (*str != '\0')
    {
        if (!isspace((unsigned char)*str))
        {
            return 0; // Found a non-whitespace, non-null character
        }
        str++;
    }

    return 1; // All characters were whitespace or null
}

void removesConsecutiveWhitspaecesFromStartOfFile(const char *filename)
{
    //  printf("removesConsecutiveWhitspaecesFromStartOfFile  %s .\n", filename);
    FILE *file = fopen(filename, "r+");
    //   FILE *file = fopen(filename, "r+");

    if (file == NULL)
    {
        fprintf(stderr, "Error: Could not open file '%s'.\n", filename);
        return;
    }

    // Create a temporary file to store the updated content
    FILE *tempFile = tmpfile();
    if (tempFile == NULL)
    {
        fprintf(stderr, "Error: Could not create a temporary file.\n");
        fclose(file);
        return;
    }

    // isAtStartOfFile
    int ch; // Use int to handle EOF
    ch = fgetc(file);
    if (ch == EOF)
    {
        fprintf(stderr, " file is empty %s.\n", filename);
        return;
    }
    while (ch != EOF && isspace(ch))
    {
        ch = fgetc(file);
    }
    while (ch != EOF)
    {
        fputc(ch, tempFile);
        ch = fgetc(file);
    }
    rewind(file);
    rewind(tempFile);

    // Write updated content back to the original file
    while ((ch = fgetc(tempFile)) != EOF)
    {
        fputc(ch, file);
    }

    // Truncate the file to remove extra data
    fflush(file);
    ftruncate(fileno(file), ftell(file));

    fclose(file);
    fclose(tempFile);
}
void removesConsecutiveWhitspaecesFromEndOfFile(const char *filename)
{
    //  printf("removesConsecutiveWhitspaecesFromEndOfFile %s  .\n", filename);
    FILE *file = fopen(filename, "r+");

    if (file == NULL)
    {

        fprintf(stderr, "Error: Could not open file '%s'.\n", filename);
        return;
    }
    FILE *tempFile = tmpfile();
    if (tempFile == NULL)
    {
        fprintf(stderr, "Error: Could not create a temporary file.\n");
        fclose(file);
        return;
    }

    // isAtStartOfFile
    fseek(file, -1, SEEK_END); // Go to the last character.char before EOF.
    int endingPostionBeforeWhiteSpaces;
    int i = 0;

    int ch;
    long pos = ftell(file); // Get the current position
                            //   printf("long pos= ending %d  .\n", pos);
    long temp;
    ch = fgetc(file); // Read the current character, char before EOF.  file stepped to point at EOF.
                      // printCharWithNulls((unsigned char)ch);
    if (ch == EOF)
    {
        fprintf(stderr, "the file is empty %s.\n", filename);
        return;
    }
    while (pos >= 0) // skipps over whitespaces
    {
        temp = ftell(file);
        // printf("removesConsecutiveWhitspaecesFromEndOfFile temp %d  .\n", temp);
        // printf("before printCharWithNulls %s  .\n", filename);

        // printCharWithNulls((unsigned char)ch);
        //  printf("after printCharWithNulls %s  .\n", filename);

        if (isspace(ch))
        {
            // printf("while  isspace(ch).\n");
            //  printf("while  pos %d.\n", pos);

            if (pos > 0)
            {
                pos--;
                fseek(file, pos, SEEK_SET);
                ch = fgetc(file);
                printCharWithNulls((unsigned char)ch);
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    // printf("long pos= ending after whitespace skipping %d  .\n", pos);

    rewind(file);
    rewind(tempFile);

    while (i <= pos)
    {
        ch = fgetc(file); // Read the current character
        fputc(ch, tempFile);
        i++;
    }
    rewind(file);
    rewind(tempFile);

    // Write updated content back to the original file
    while ((ch = fgetc(tempFile)) != EOF)
    {
        fputc(ch, file);
    }

    // Truncate the file to remove extra data
    fflush(file);
    ftruncate(fileno(file), ftell(file));

    fclose(file);
    fclose(tempFile);
}
void removesConsecutiveWhitspacesWithInLine(const char *filename)
{

    // printf("removesConsecutiveWhitspaeces fike %s.\n", filename);

    FILE *file = fopen(filename, "r+");
    //   FILE *file = fopen(filename, "r+");

    if (file == NULL)
    {
        fprintf(stderr, "Error: Could not open file '%s'.\n", filename);
        return;
    }

    // Create a temporary file to store the updated content
    FILE *tempFile = tmpfile();
    if (tempFile == NULL)
    {
        fprintf(stderr, "Error: Could not create a temporary file.\n");
        fclose(file);
        return;
    }

    // isAtStartOfFile
    int ch, isLeftCharacterIsWhitespace = 0; // Use int to handle EOF
    ch = fgetc(file);
    if (ch == EOF)
    {
        fprintf(stderr, " file is empty %s.\n", filename);
        return;
    }
    while (ch != EOF)
    { // 4 combinations of for isLeftCharacterIsWhitespace && isspace(ch)
      // printCharWithNulls((unsigned char)ch);
      // printf("while fgetc ch %c.\n", ch);
      // printf("isLeftCharacterIsWhitespace %d\n", isLeftCharacterIsWhitespace);
        if (ch == '\r')
        {
            //   printf("ch=\\r\n");
            isLeftCharacterIsWhitespace = 1;
            ch = fgetc(file);
            continue;
        }
        if (!isLeftCharacterIsWhitespace && !isspace(ch)) // 2 none whitespace chars
        {
            //    printf("2 none whitespace chars\n");
            isLeftCharacterIsWhitespace = 0;
            fputc(ch, tempFile);
        }
        else if (!isLeftCharacterIsWhitespace && isspace(ch)) // left char is none whitespace and the right is whitespace
        {
            //    printf("left char is none whitespace and the right is whitespace\n");

            if (ch == '\n')
            {
                fputc('\n', tempFile);
            }
            else
            {
                //           printf("skip\n");
                // fputc(' ', tempFile);
            }
            isLeftCharacterIsWhitespace = 1;
            //  break;
        }
        else if (isLeftCharacterIsWhitespace && !isspace(ch))
        { // left char is whitespace and the right is none whitespace
            //     printf("left char is  whitespace and the right is none whitespace\n");
            isLeftCharacterIsWhitespace = 0;
            fputc(' ', tempFile);
            fputc(ch, tempFile);
            // break;
        }
        else if (isLeftCharacterIsWhitespace && isspace(ch))
        { // default: 2 cosecutive whitespaces.
          //   printf("default: 2 cosecutive whitespaces\n");
            if (ch == '\n')
            {
                fputc('\n', tempFile);
                isLeftCharacterIsWhitespace = 0;
            }
            else
            {
                isLeftCharacterIsWhitespace = 1;
            }
        }

        ch = fgetc(file);
    }
    // left_ch
    rewind(file);
    rewind(tempFile);
    //  int ch;
    //   ch = fgetc(tempFile);
    // printf("while left_ch = fgetc(tempFile)) oooout %c.\n", ch);
    // Write updated content back to the original file
    while ((ch = fgetc(tempFile)) != EOF)
    {
        // printf("while fputc(ch, file); %c.\n", ch);

        fputc(ch, file);
    }

    // Truncate the file to remove extra data
    fflush(file);
    ftruncate(fileno(file), ftell(file));

    fclose(file);
    fclose(tempFile);
}
void removesConsecutiveWhitspacesCrossLines(const char *filename)
{
    //  printf("removesConsecutiveWhitspacesCrossLines fike %s.\n", filename);

    FILE *file = fopen(filename, "r+");
    //   FILE *file = fopen(filename, "r+");

    if (file == NULL)
    {
        fprintf(stderr, "Error: Could not open file '%s'.\n", filename);
        return;
    }

    // Create a temporary file to store the updated content
    FILE *tempFile = tmpfile();
    if (tempFile == NULL)
    {
        fprintf(stderr, "Error: Could not create a temporary file.\n");
        fclose(file);
        return;
    }

    // isAtStartOfFile
    int ch, isLeftCharacterIsWhitespace = 0; // Use int to handle EOF
    ch = fgetc(file);
    if (ch == EOF)
    {
        fprintf(stderr, " file is empty %s.\n", filename);
        return;
    }
    while (ch != EOF)
    { // 4 combinations of for isLeftCharacterIsWhitespace && isspace(ch)
        // printCharWithNulls((unsigned char)ch);
        //    printf("while fgetc ch %c.\n", ch);
        //    printf("isLeftCharacterIsWhitespace %d\n", isLeftCharacterIsWhitespace);
        if (ch == '\r')
        {
            //    printf("ch=\\r\n");
            isLeftCharacterIsWhitespace = 1;
            ch = fgetc(file);
            continue;
        }
        if (!isLeftCharacterIsWhitespace && !isspace(ch)) // 2 none whitespace chars
        {
            //    printf("2 none whitespace chars\n");
            isLeftCharacterIsWhitespace = 0;
            fputc(ch, tempFile);
        }
        else if (!isLeftCharacterIsWhitespace && isspace(ch)) // left char is none whitespace and the right is whitespace
        {
            //     printf("left char is none whitespace and the right is whitespace\n");

            fputc(ch, tempFile);

            isLeftCharacterIsWhitespace = 1;
            //  break;
        }
        else if (isLeftCharacterIsWhitespace && !isspace(ch))
        { // left char is whitespace and the right is none whitespace
          //   printf("left char is  whitespace and the right is none whitespace\n");
            isLeftCharacterIsWhitespace = 0;
            // fputc(' ', tempFile);
            fputc(ch, tempFile);
            // break;
        }
        else if (isLeftCharacterIsWhitespace && isspace(ch))
        { // default: 2 cosecutive whitespaces.
          //    printf("default: 2 cosecutive whitespaces\n");
            /*if (ch == '\n')
            {
                fputc('\n', tempFile);
                isLeftCharacterIsWhitespace = 0;
            }
            else
            {
                isLeftCharacterIsWhitespace = 1;
            }*/
        }

        ch = fgetc(file);
    }
    // left_ch
    rewind(file);
    rewind(tempFile);
    //  int ch;
    //   ch = fgetc(tempFile);
    // printf("while left_ch = fgetc(tempFile)) oooout %c.\n", ch);
    // Write updated content back to the original file
    while ((ch = fgetc(tempFile)) != EOF)
    {
        // printf("while fputc(ch, file); %c.\n", ch);

        fputc(ch, file);
    }

    // Truncate the file to remove extra data
    fflush(file);
    ftruncate(fileno(file), ftell(file));

    fclose(file);
    fclose(tempFile);
}
void copy_left_of_last_dot(const char *src, char *dest)
{
    const char *dot = strrchr(src, '.'); // Find the last dot in the string
    if (dot)
    {
        size_t length = dot - src;  // Calculate the number of characters to copy
        strncpy(dest, src, length); // Copy the substring
        dest[length] = '\0';        // Null-terminate the destination string
    }
    else
    {
        // If no dot is found, copy the whole string
        strcpy(dest, src);
    }
}
Bool isLastCharacter(const char *str, char c)
{
    // Validate input
    if (str == NULL || *str == '\0')
    {
        return 0; // Empty string or NULL pointer, return false
    }

    // Find the last character in the string
    size_t len = strlen(str);

    // Compare the last character with the given character
    if (str[len - 1] == c)
    {
        return true; // True
    }
    else
    {
        return false; // False
    }
}

Bool doesStringExistInFile(const char *filename, const char *searchString)
{
    //   printf("\n doesStringExistInFile filename %s  \n", filename);

    // Validate input
    if (filename == NULL || searchString == NULL)
    {
        return false; // Invalid input
    }

    // Open the file in read mode
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file in doesStringExistInFile");
        return false;
    }

    // Buffer to store each line of the file
    char buffer[1024];
    //   printf("\n searchString %s  \n", searchString);

    // Search for the string in the file line by line
    while (fgets(buffer, sizeof(buffer), file))
    {
        //    printf("\n buffer %s  \n", buffer);

        if (strstr(buffer, searchString) != NULL)
        {
            //    printf("\n strstr(buffer, searchString\n");

            fclose(file); // Close the file before returning
            return true;  // String found
        }
        else
        {
            updateLineNum(1);
        }
    }

    // Close the file
    fclose(file);

    // String not found
    return 0;
}
char *copyWithoutChar(const char *source, char excludeChar)
{
    if (source == NULL)
    {
        return NULL; // Handle NULL input
    }

    // Calculate the length of the source string
    size_t length = strlen(source);

    // Allocate memory for the new string (max length same as source + null terminator)
    char *result = (char *)malloc_wrapper(length + 1);
    if (result == NULL)
    {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    size_t j = 0; // Index for the new string
    for (size_t i = 0; i < length; i++)
    {
        if (source[i] != excludeChar)
        {
            result[j++] = source[i];
        }
    }

    result[j] = '\0'; // Null-terminate the new string
    return result;    // free it in caller
}
Directive getDirectiveType(const char *directive)
{
    if (strcmp(directive, ".data") == 0)
    {
        return data;
    }
    else if (strcmp(directive, ".string") == 0)
    {
        return string;
    }
    else if (strcmp(directive, ".entry") == 0)
    {
        return entry;
    }
    else if (strcmp(directive, ".extern") == 0)
    {
        return externDirective;
    }
    else
    {
        return invalidDirective; // If the directive doesn't match any known values
    }
}
void charToString(char c, char *str)
{
    str[0] = c;    // Place the character in the first position
    str[1] = '\0'; // Null-terminate the string
}

int countCharacterInString(const char *str, char ch)
{
    int count = 0; // Initialize the count to 0

    // Iterate through the string
    while (*str != '\0')
    {
        if (*str == ch)
        {
            count++; // Increment the count if the character matches
        }
        str++; // Move to the next character in the string
    }

    return count; // Return the total count
}
void removeTrailingWhitespaceFixed(char *str)
{
    int len = strlen(str);

    while (len > 0 && isspace((unsigned char)str[len - 1]))
    {
        str[--len] = '\0'; // Replace trailing whitespace with null terminator
    }
}

/* Function to remove leading whitespace */
void removeLeadingWhitespace(char *str)
{
    char *start = str;

    // Find the first non-whitespace character
    while (*start && isspace((unsigned char)*start))
    {
        start++;
    }

    // Shift the string left if leading whitespace exists
    if (start != str)
    {
        char *dest = str;
        while (*start)
        {
            *dest++ = *start++;
        }
        *dest = '\0'; // Null-terminate the string
    }
}
void splitAndWriteArraysByOpernadDelimiter(char array1[80], char array2[80], const char *str)
{

    int i = 0, j = 0;
    int foundComma = 0;

    // Traverse the input string
    while (*str != '\0')
    {
        if (*str == OPERAND_DELIMITER && !foundComma)
        {
            // Found the first comma
            foundComma = 1;
            str++; // Skip the comma
            continue;
        }

        // if (!isspace((unsigned char)*str))
        //  {
        if (!foundComma)
        {
            // Write to array1 before the comma
            if (i < 79)
            { // Prevent overflow
                array1[i++] = *str;
            }
        }
        else
        {
            // Write to array2 after the comma
            if (j < 79)
            { // Prevent overflow
                array2[j++] = *str;
            }
        }
        //  }

        str++; // Move to the next character
    }

    // Null-terminate both arrays
    array1[i] = '\0';
    array2[j] = '\0';
}
int isContainsWhitespace(const char *str)
{
    while (*str)
    { // Loop until the null terminator
        if (isspace((unsigned char)*str))
        {
            return true; // Return 1 if a whitespace is found
        }
        str++; // Move to the next character
    }
    return false; // Return 0 if no whitespace is found
}

void removeWhiteSpaces(char *str)
{
    if (str == NULL)
    {
        return; // Handle NULL input
    }

    char *src = str;  // Source pointer
    char *dest = str; // Destination pointer

    while (*src != '\0')
    {
        if (!isspace((unsigned char)*src))
        {                 // Check if the character is not a white space
            *dest = *src; // Copy the character to the destination
            dest++;
        }
        src++;
    }

    *dest = '\0'; // Null-terminate the resulting string
}
Bool isFirstOrLastCharMatch(const char *str, char ch)
{
    if (str == NULL || *str == '\0')
    {
        return false; // Handle NULL or empty string
    }

    // Check if the first character matches
    if (str[0] == ch)
    {
        return true;
    }

    // Check if the last character matches
    size_t len = strlen(str);
    if (str[len - 1] == ch)
    {
        return true;
    }

    return false; // No match
}
Bool isFirstAndLastCharMatch(const char *str, char ch)
{
    printf(" isFirstAndLastCharMatch str  %s\n", str);
    printf(" isFirstAndLastCharMatch ch  %c\n", ch);

    if (str == NULL || *str == '\0')
    {
        return false; // Handle NULL or empty string
    }

    // Check if the last character matches
    size_t len = strlen(str);
    if (str[0] == ch && str[len - 1] == ch)
    {
        return true;
    }
    return false; // No match
}
Bool isLastCharMatch(const char *str, char ch)
{
    size_t len = strlen(str);
    if (str[len - 1] == ch)
    {
        return true;
    }

    return false; // No match
}
Bool isStringComposedOfCharSet(const char *str, const char *charSet)
{
    if (str == NULL || charSet == NULL)
    {
        return false; // Invalid input
    }
    // Iterate through the string
    for (const char *s = str; *s != '\0'; ++s)
    {
        // Check if the current character is in the allowed set
        if (strchr(charSet, *s) == NULL)
        {
            return false; // Character not in the allowed set
        }
    }
    return true; // All characters are in the allowed set
}
Bool hasAdjacentRepeats(const char *str, const char *charSet)
{
    if (str == NULL || charSet == NULL)
    {
        return 0; // Invalid input
    }

    while (*str != '\0')
    {
        if (strchr(charSet, *str) != NULL && *(str + 1) == *str)
        {
            // Current character is in the set, and the next character is the same
            return true; // Found adjacent repeats
        }
        str++;
    }

    return false; // No adjacent repeats found
}
Bool validateStringOrder(const char *str, char rightChar, char unallowdLeftChar, char unallowdLeftChar_2)
{
    if (str == NULL)
    {
        return false; // Invalid input
    }

    while (*str != '\0')
    {
        // Check if the current character is the first character
        if (*str == rightChar)
        {
            // Check if the next character is one of the other two characters
            if (*(str + 1) == unallowdLeftChar || *(str + 1) == unallowdLeftChar_2)
            {
                return false; // Invalid string
            }
        }
        str++;
    }

    return true; // Valid string
}
void eraseFirstAndLast(char *str)
{
    int len = strlen(str);

    // Check if the string has at least two characters
    if (len < 2)
    {
        str[0] = '\0'; // If the string is too short, make it empty
        return;
    }

    // Shift the contents of the string one position to the left
    for (int i = 1; i < len - 1; i++)
    {
        str[i - 1] = str[i];
    }

    // Null-terminate the string after shifting
    str[len - 2] = '\0';
}
Bool isKeyword(char *input)
{
    if (resolveDirectiveType(input) || resolveInstructionType(input) || strcmp(MACRO_KEY_WORD, input) == 0 || strcmp(MACROEND_KEY_WORD, input) == 0)
    {
        return true;
    }
    return false;
}