all: assembler 
assembler: assembler.o preassembler.o assembler_utils.o lineNum.o first_process.o secondTransform.o fileCreationDecider.o post-macros-generic-errors.o instructionCount.o symbols_prep.o symbols_table.o bitFieldInstruction.o binary_code_utils.o code_image.o extern_image.o entry_image.o IcManager.o DcManager.o operand_label_manager.o directiveslabelsExecuter.o data_image.o labelsFilesHandler.o file_creators.o objectsFileHandler.o
	gcc -g -ansi -Wall -pedantic assembler.o preassembler.o assembler_utils.o lineNum.o first_process.o secondTransform.o fileCreationDecider.o post-macros-generic-errors.o instructionCount.o symbols_prep.o symbols_table.o bitFieldInstruction.o binary_code_utils.o code_image.o extern_image.o entry_image.o IcManager.o DcManager.o operand_label_manager.o directiveslabelsExecuter.o data_image.o dataDirectiveManager.o dataStringManager.o labelsFilesHandler.o file_creators.o objectsFileHandler.o -o assembler -lm
assembler.o:assembler.c  assembler.h directives.h
	gcc -g -c -Wall -pedantic assembler.c
secondTransform.o: secondTransform.c assembler_utils.o lineNum.o fileCreationDecider.o assembler.h
	gcc -g -c -Wall -pedantic secondTransform.c
first_process.o: first_process.c assembler_utils.o post-macros-generic-errors.o lineNum.o  fileCreationDecider.o symbols_prep.o symbols_table.o bitFieldInstruction.o binary_code_utils.o code_image.o IcManager.o DcManager.o operand_label_manager.o extern_image.o entry_image.o directiveslabelsExecuter.o dataDirectiveManager.o objectsFileHandler.o dataStringManager.o labelsFilesHandler.o assembler.h binary_code.h directives.h file_creators.h
	gcc -g -c -Wall -pedantic first_process.c
preassembler.o: preassembler.c assembler_utils.o lineNum.o fileCreationDecider.o assembler.h
	gcc -g -c -Wall -pedantic preassembler.c 
assembler_utils.o: assembler_utils.c lineNum.o fileCreationDecider.o assembler.h   
	gcc -g -c -Wall -pedantic assembler_utils.c
post-macros-generic-errors.o: post-macros-generic-errors.c lineNum.o fileCreationDecider.o assembler.h 
	gcc -g -c -Wall -pedantic post-macros-generic-errors.c
lineNum.o: lineNum.c  
	gcc -g -c -Wall -pedantic lineNum.c
instructionCount.o: instructionCount.c  
	gcc -g -c -Wall -pedantic instructionCount.c
fileCreationDecider.o: fileCreationDecider.c
	gcc -g -c -Wall -pedantic fileCreationDecider.c
symbols_prep.o: symbols_prep.c assembler_utils.o assembler.h 
	gcc -g -c -Wall -pedantic symbols_prep.c
symbols_table.o:symbols_table.c assembler_utils.o assembler.h 
	gcc -g -c -Wall -pedantic symbols_table.c
bitFieldInstruction.o:bitFieldInstruction.c assembler_utils.o binary_code_utils.o assembler.h binary_code.h 
	gcc -g -c -Wall -pedantic bitFieldInstruction.c
binary_code_utils.o: binary_code_utils.c binary_code.h 
	gcc -g -c -Wall -pedantic binary_code_utils.c
code_image.o: code_image.c post-macros-generic-errors.o binary_code.h assembler.h 
	gcc -g -c -Wall -pedantic code_image.c
IcManager.o: IcManager.c assembler.h 
	gcc -g -c -Wall -pedantic IcManager.c
operand_label_manager.o: operand_label_manager.c 
	gcc -g -c -Wall -pedantic operand_label_manager.c
DcManager.o: DcManager.c assembler.h 
	gcc -g -c -Wall -pedantic DcManager.c
extern_image.o: extern_image.c post-macros-generic-errors.o assembler.h directives.h 
	gcc -g -c -Wall -pedantic extern_image.c
directiveslabelsExecuter.o: directiveslabelsExecuter.c post-macros-generic-errors.o assembler.h directives.h 
	gcc -g -c -Wall -pedantic directiveslabelsExecuter.c
entry_image.o:entry_image.c post-macros-generic-errors.o assembler.h directives.h 
	gcc -g -c -Wall -pedantic entry_image.c
data_image.o: data_image.c DcManager.o assembler.h binary_data.h
	gcc -g -c -Wall -pedantic data_image.c
dataDirectiveManager.o:dataDirectiveManager.c data_image.o DcManager.o binary_data.h
	gcc -g -c -Wall -pedantic dataDirectiveManager.c
dataStringManager.o:dataStringManager.c data_image.o DcManager.o binary_data.h
	gcc -g -c -Wall -pedantic dataStringManager.c
labelsFilesHandler.o:labelsFilesHandler.c data_image.o DcManager.o IcManager.o binary_data.h assembler.h file_creators.h
	gcc -g -c -Wall -pedantic labelsFilesHandler.c
file_creators.o:file_creators.c file_creators.h
	gcc -g -c -Wall -pedantic file_creators.c
objectsFileHandler.o:objectsFileHandler.c data_image.o code_image.o DcManager.o IcManager.o file_creators.h  binary_data.h assembler.h 
	gcc -g -c -Wall -pedantic objectsFileHandler.c


