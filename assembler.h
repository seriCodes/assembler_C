#include <stdio.h>

#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#define MAX_ARG_STRING_LENGTH 130
#define MAX_STRING_LENGTH 100
#define CURRENT_LINE 4
#define INSTRUCTIONS_INITIAL_ADDRESS 100
#define MAX_LABEL_LIMIT 31
#define MAX_SYMBOLS_ALLOCATIONS 150
#define MACRO_KEY_WORD "mcro"
#define MACROEND_KEY_WORD "mcroend"

extern const char *KeyStrings[];
extern char *invalidName;
extern char *macroString;

typedef enum
{
    nill,
    external,
    entryAttrib,
    code,
    dataAttrib
} LabelAttribute;
typedef struct
{
    char *name;
    int *ic_or_dc;
    LabelAttribute *attrib;
    int *address; // make sure, but this is not in use

} LabelData;
typedef enum
{
    mcro,
    mcroend,
    right,
    left,
    KEY_THREE,
    KEY_COUNT
} Key;
typedef enum
{
    SUCCESS,
    FAILURE
} isSuccess;
typedef enum
{
    false,
    true
} Bool;
typedef enum
{
    notDir = -1,
    data,
    string,
    entry,
    externDirective,
    invalidDirective // For unsupported directives
} Directive;
typedef enum
{
    invalidWord,
    label,
    instruction,
    DirectiveWord,
    invalidLabel,
    comment
} ProcessedWord;
typedef enum
{
    nullInstruction, // Null or undefined value
    mov,
    cmp,
    add,
    sub,
    lea,
    clr,
    not,
    inc,
    dec,
    jmp,
    bne,
    jsr,
    red,
    prn,
    rts,
    stop
} InstructionName;
typedef struct
{
    InstructionName instructionName; // Instruction name
    int funct;                       // funct field
    int opcode;                      // Opcode field
    int sourceOperandMethods[4];     // Legal addressing modes for the source operand
    int destOperandMethods[4];       // Legal addressing modes for the destination operand
} Instruction;
typedef struct
{
    ProcessedWord word;
    InstructionName instructionName;
    Directive dirType;
} wordType;
// extern Bool shouldNotCreateFile;

int main_preassembler(int argc, char *argv[]);
Instruction *getInstruction(int index);
Directive getDirectiveType(const char *directive);
Bool isLastCharacter(const char *str, char c);
void setLineErrorReport();
void setStartErrorReport(char *str);
void updateShouldNotCreateFile(Bool newValue, Bool isMainFunction);
void printCharArrayWithNulls(const char *array, size_t size);
void forwardPointerToNullTerminator(char **str);
char *copyCharArrayWithExtraSpace(const char *source, int extarSpace);
void forwardPointerToNullTerminator(char **str);
void removeTrailingWhitespaces(char *str);
char *copyCharArrayWithExtraSpace(const char *source, int extarSpace);
void moveToNextLine(FILE *fd);
void resetCharArray(char *array, size_t size);
void test();
char skipOverFollowingNewLinesAndTabsAndSpacesAndReturnCarriage(FILE *fd);
char skipOverTabsAndSpaces(FILE *fd);
void remove_all_whitespace(char *str);
void *fetchNoneWhiteCharachtersString(FILE *fd, char **str);
int stringAccumulationDetectionWithRemoveWhiteSpaces(char stringAccumulator[], Key key);
long saveFilePosition(FILE *fd);
void restoreFilePosition();
int isAtStartOfFile(FILE *fd);
int extern myExternVar;
void moveFilePointerToTheStartOfTheLine(FILE *fd);
int lineRemainingWordLimit(FILE *left_Char, FILE *right_Char, int amount);
void *malloc_wrapper(size_t size);
void free_wrapper(void *ptr);
void free_all();
void free_wrapper(void *ptr);
void *realloc_wrapper(void *ptr, size_t new_size);
char *setGetFileName(char *argv, Bool isMainFunction);

void removeStringEndingWhitespace(char *str);
int isOnlyWhitespaceOrNull(char *str);
void replacsTabsWithSpaces(const char *filename);
void removesConsecutiveWhitspacesWithInLine(const char *filename);
void removesConsecutiveWhitspacesCrossLines(const char *filename);
void copy_left_of_last_dot(const char *src, char *dest);
Bool doesStringExistInFile(const char *filename, const char *searchString);
int getLineNum();
void setLineNum(int value);
void updateLineNum(int delta);
void verifyFileLineCharsLimit(char *filename, int limit);
int getInstructionCount();
void setInstructionCount(int value);
void updateInstructionCount(int delta);
Bool isLetter(char letter);
char **getLabels();
void setLabel(char *value);
void initLabels();
void updateAllocCount(int num);
void printLabels();
void setSymbol_table(int *ic, Directive dirType, int labelAmount);
void resetLineLabels();
void freeLineLabels();
void printSymbolTable();
void reset_line_labels_elements();
int getLabelsFileCounter();
void freeSymbolTableAndLabelsDataPointers();
Bool isDuplicatedLabel(char *stringAccumulator);
extern const char *InstructionsStrings[];
int countCharacterInString(const char *str, char ch);
void removeSidesWhiteSpaces(char *str);
void removeTrailingWhitespaceFixed(char *str);
void removeLeadingWhitespace(char *str);
void splitAndWriteArraysByOpernadDelimiter(char array1[80], char array2[80], const char *str);
int isContainsWhitespace(const char *str);
Bool is_label_name_valid(char *name, Bool isReportError);
int charToInt(char ch);
void charToString(char c, char *str);
void removeWhiteSpaces(char *str);
Bool isFirstOrLastCharMatch(const char *str, char ch);
Bool isStringComposedOfCharSet(const char *str, const char *charSet);
Bool hasAdjacentRepeats(const char *str, const char *charSet);
Bool validateStringOrder(const char *str, char rightChar, char unallowdLeftChar, char unallowdLeftChar_2);
Bool isLastCharMatch(const char *str, char ch);
Bool isFirstAndLastCharMatch(const char *str, char ch);
void eraseFirstAndLast(char *str);
Bool getShouldNotCreateFile();
Bool isKeyword(char *input);
int main_first_transformation(int argc, char *argv[]);
#endif
