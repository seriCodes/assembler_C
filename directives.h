#ifndef DIRECTIVES_H
#define DIRECTIVES_H
void directiveLabelHandler(wordType result, FILE *fd,
                           int lastCharacter, char **createCustomDoublePointer(int size));
char **getExternLabels();
char **addExternalLabelStringItemToArray(int size);
char **createExternalLabelDoublePointer(int size);
char **createEntrylLabelDoublePointer(int size);
int getLabelExternCounter();
void directiveDataHandler(wordType result, FILE *fd, int ch);
void directiveStringHandler(wordType result, FILE *fd, int ch);
#endif
