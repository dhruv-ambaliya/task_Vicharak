#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 256
#define MAX_VARIABLES 100

int ifCount = 0, elseCount = 0, whileCount = 0, divCount = 0, multiCount = 0;

void handleArith(char *line, FILE *outputFile) {
    char lhs[MAX_LINE_LENGTH], rhs1[MAX_LINE_LENGTH], rhs2[MAX_LINE_LENGTH], operator[MAX_LINE_LENGTH];

    if (sscanf(line, "Arithmetic: identifier=%s expression: Identifier(%[^)]) Operator(%[^)]) Identifier(%[^)])", lhs, rhs1, operator, rhs2) == 4 ||
        sscanf(line, "Arithmetic: identifier=%s expression: Identifier(%[^)]) Operator(%[^)]) Number(%[^)])", lhs, rhs1, operator, rhs2) == 4 ||
        sscanf(line, "Arithmetic: identifier=%s expression: Number(%[^)]) Operator(%[^)]) Identifier(%[^)])", lhs, rhs1, operator, rhs2) == 4 ||
        sscanf(line, "Arithmetic: identifier=%s expression: Number(%[^)]) Operator(%[^)]) Number(%[^)])", lhs, rhs1, operator, rhs2) == 4) {

        if (strcmp(operator, "+") == 0 || strcmp(operator, "-") == 0) {
            // Handle + and -
            if (rhs2[0] >= '0' && rhs2[0] <= '9') { // this is for give different instruction for num and identi
                fprintf(outputFile, "\n    ldi A %s\n", rhs2);
            } else {
                fprintf(outputFile, "\n    lda %%%s\n", rhs2); 
            }
            fprintf(outputFile, "    mov B A\n");

            if (rhs1[0] >= '0' && rhs1[0] <= '9') {
                fprintf(outputFile, "    ldi A %s\n", rhs1);
            } else {
                fprintf(outputFile, "    lda %%%s\n", rhs1); 
            }

            fprintf(outputFile, "    %s\n", strcmp(operator, "+") == 0 ? "add" : "sub");
            fprintf(outputFile, "    sta %%%s\n", lhs);
        } else if (strcmp(operator, "*") == 0) {
            // Handle *
            fprintf(outputFile, "\nmulti:\n    ldi A 0\n    sta %%tmp1\n");

            if (rhs1[0] >= '0' && rhs1[0] <= '9') {
                fprintf(outputFile, "    ldi A %s\n", rhs1);
            } else {
                fprintf(outputFile, "    lda %%%s\n", rhs1);
            }
            fprintf(outputFile, "    sta %%tmp0\n");

            if (rhs2[0] >= '0' && rhs2[0] <= '9') {
                fprintf(outputFile, "    ldi A %s\n", rhs2);
            } else {
                fprintf(outputFile, "    lda %%%s\n", rhs2);
            }
            fprintf(outputFile, "    mov B A\n");

            fprintf(outputFile, "multi_loop_%d:\n", multiCount);
            fprintf(outputFile, "    lda %%tmp1\n    add\n    sta %%tmp1\n    lda %%tmp0\n    dec\n    sta %%tmp0\n");
            fprintf(outputFile, "    jnz %%multi_loop_%d\n", multiCount);
            fprintf(outputFile, "    lda %%tmp1\n    sta %%%s\n", lhs);
            multiCount++;
        } else if (strcmp(operator, "/") == 0) {
            // Handle /
            fprintf(outputFile, "\ndiv:\n    ldi A 0\n    sta %%tmp0\n");

            if (rhs2[0] >= '0' && rhs2[0] <= '9') {
                fprintf(outputFile, "    ldi A %s\n", rhs2); 
            } else {
                fprintf(outputFile, "    lda %%%s\n", rhs2);
            }
            fprintf(outputFile, "    mov B A\n");

            if (rhs1[0] >= '0' && rhs1[0] <= '9') {
                fprintf(outputFile, "    ldi A %s\n", rhs1); 
            } else {
                fprintf(outputFile, "    lda %%%s\n", rhs1); 
            }

            fprintf(outputFile, "div_loop_%d:\n", divCount);
            fprintf(outputFile, "    sub\n    sta %%tmp1\n    lda %%tmp0\n    inc\n    sta %%tmp0\n    lda %%tmp1\n    cmp\n");
            fprintf(outputFile, "    jnc %%div_loop_%d\n", divCount);

            fprintf(outputFile, "    lda %%tmp0\n    sta %%%s\n", lhs);
            divCount++;
        }
    }
}
void handleInstruction(char *line, FILE *outputFile, FILE *inFile) {
    char nextLine[MAX_LINE_LENGTH], lhs[MAX_LINE_LENGTH], rhs1[MAX_LINE_LENGTH], rhs2[MAX_LINE_LENGTH], op[MAX_LINE_LENGTH];

    if (strstr(line, "Assignment")) {
        if (sscanf(line, "Assignment: identifier=%s expression: Number(%[^)])", lhs, rhs1) == 2)
            fprintf(outputFile, "\n    ldi A %s\n    sta %%%s\n", rhs1, lhs);
        else if (sscanf(line, "Assignment: identifier=%s expression: Identifier(%[^)])", lhs, rhs1) == 2)
            fprintf(outputFile, "\n    lda %%%s\n    sta %%%s\n", rhs1, lhs);
    } else if (strstr(line, "Arithmetic")) {
        handleArith(line, outputFile);
    } else if (strstr(line, "Output")) {
         if (sscanf(line, "Output: Identifier(%[^)])", rhs1) == 1) {
            fprintf(outputFile, "    lda %%%s\n    out 0\n", rhs1);
        } else{
            fprintf(outputFile, "    ldi A %s\n    out 0\n", rhs1);
        }
    }  else if (strstr(line, "WhileStatement")) { 
        int localWhileCount = whileCount++;
        
        // Print the while loop label
        fprintf(outputFile, "\nwhile_loop_%d:\n", localWhileCount);
        fprintf(outputFile, "whilecondition_%d:\n", localWhileCount);
        
        // Generate code for the comparison
        if (sscanf(line, "WhileStatement: condition: Identifier(%[^)]) Comparison(%[^)]) Identifier(%[^)])", lhs, op, rhs1) == 3) {
            fprintf(outputFile, "    lda %%%s\n    mov B A\n    lda %%%s\n    cmp\n", rhs1, lhs);
        } else if (sscanf(line, "WhileStatement: condition: Identifier(%[^)]) Comparison(%[^)]) Number(%[^)])", lhs, op, rhs1) == 3) {
            fprintf(outputFile, "    ldi A %s\n    mov B A\n    lda %%%s\n    cmp\n", rhs1, lhs);
        } else if (sscanf(line, "WhileStatement: condition: Number(%[^)]) Comparison(%[^)]) Identifier(%[^)])", lhs, op, rhs1) == 3) {
            fprintf(outputFile, "    ldi A %s\n    mov B A\n    lda %%%s\n    cmp\n", lhs, rhs1);
        } else if (sscanf(line, "WhileStatement: condition: Number(%[^)]) Comparison(%[^)]) Number(%[^)])", lhs, op, rhs1) == 3) {
            fprintf(outputFile, "    ldi A %s\n    mov B A\n    ldi A %s\n    cmp\n", lhs, rhs1);
        }
        
        // Generate the conditional jump to exit the loop based on comparison operator
        if (strcmp(op, "!=") == 0) fprintf(outputFile, "    je %%while_end_%d\n", localWhileCount);
        else if (strcmp(op, "==") == 0) fprintf(outputFile, "    jne %%while_end_%d\n", localWhileCount);
        else if (strcmp(op, "<") == 0) fprintf(outputFile, "    jnc %%while_end_%d\n    je %%while_end_%d\n", localWhileCount, localWhileCount);
        else if (strcmp(op, ">") == 0) fprintf(outputFile, "    jc %%while_end_%d\n    je %%while_end_%d\n", localWhileCount, localWhileCount);
        else if (strcmp(op, "<=") == 0) fprintf(outputFile, "    jnc %%while_end_%d\n", localWhileCount);
        else if (strcmp(op, ">=") == 0) fprintf(outputFile, "    jc %%while_end_%d\n", localWhileCount);
        
        // Process the loop body
        fgets(nextLine, sizeof(nextLine), inFile);
        if (strstr(nextLine, "do:")) {
            // Loop body
            while (fgets(nextLine, sizeof(nextLine), inFile)) {
                if (strstr(nextLine, "endwhile")) {
                    // End of loop body
                    break;
                } else {
                    // Process statements in loop body
                    handleInstruction(nextLine, outputFile, inFile);
                }
            }
        }
        
        // Jump back to the beginning of the loop
        fprintf(outputFile, "    jmp %%while_loop_%d\n", localWhileCount);
        fprintf(outputFile, "while_end_%d:\n", localWhileCount);
    } else if (strstr(line, "IfStatement")) { 
        int localIfCount = ifCount++;
        if (sscanf(line, "IfStatement: condition: Identifier(%[^)]) Comparison(%[^)]) Identifier(%[^)])", lhs, op, rhs1) == 3) {
            fprintf(outputFile, "\nifcondition_%d:\n    lda %%%s\n    mov B A\n    lda %%%s\n    cmp\n", localIfCount, rhs1, lhs);
        } else if (sscanf(line, "IfStatement: condition: Identifier(%[^)]) Comparison(%[^)]) Number(%[^)])", lhs, op, rhs1) == 3) {
            fprintf(outputFile, "\nifcondition_%d:\n    ldi A %s\n    mov B A\n    lda %%%s\n    cmp\n", localIfCount, rhs1, lhs);
        } else if (sscanf(line, "IfStatement: condition: Number(%[^)]) Comparison(%[^)]) Identifier(%[^)])", lhs, op, rhs1) == 3) {
            fprintf(outputFile, "\nifcondition_%d:\n    ldi A %s\n    mov B A\n    lda %%%s\n    cmp\n",localIfCount, lhs, rhs1);
        } else if (sscanf(line, "IfStatement: condition: Number(%[^)]) Comparison(%[^)]) Number(%[^)])", lhs, op, rhs1) == 3) {
            fprintf(outputFile, "\nifcondition_%d:\n    ldi A %s\n    mov B A\n    ldi A %s\n    cmp\n",localIfCount, lhs, rhs1);
        }
        // for comparison 
        if (strcmp(op, "!=") == 0) fprintf(outputFile, "    je %%endif_%d\n", localIfCount);
        else if (strcmp(op, "==") == 0) fprintf(outputFile, "    jne %%endif_%d\n", localIfCount);
        else if (strcmp(op, "<") == 0) fprintf(outputFile, "    jnc %%endif_%d\n    je %%endif_%d\n", localIfCount, localIfCount);
        else if (strcmp(op, ">") == 0) fprintf(outputFile, "    jc %%endif_%d\n    je %%endif_%d\n", localIfCount, localIfCount);
        else if (strcmp(op, "<=") == 0) fprintf(outputFile, "    jnc %%endif_%d\n", localIfCount);
        else fprintf(outputFile, "    jc %%endif_%d\n", localIfCount);

        fgets(nextLine, sizeof(nextLine), inFile);
        if (strstr(nextLine, "then:")) {
            fprintf(outputFile, "startif_%d:\n", localIfCount);
        }

        while (fgets(nextLine, sizeof(nextLine), inFile)) {
            if (strstr(nextLine, "endif")) {
                fgets(nextLine, sizeof(nextLine), inFile);
                int hasElse = strstr(nextLine, "Else:") ? 1 : 0;
                if (hasElse) {
                    // If there a Else block, add a jump to the end of Else
                    fprintf(outputFile, "    jmp %%endelse_%d\n", elseCount);
                }
                fprintf(outputFile, "endif_%d:\n", localIfCount);
                if (hasElse) {
                    fprintf(outputFile, "\nstartelse_%d:", elseCount);
                    int localElseCount = elseCount++;
                    while (fgets(nextLine, sizeof(nextLine), inFile)) {
                        if (strstr(nextLine, "endelse")) {
                            fprintf(outputFile, "endelse_%d:\n", localElseCount);
                            break;
                        } else {
                            handleInstruction(nextLine, outputFile, inFile);
                        }
                    }
                }
                break;
            } else {
                handleInstruction(nextLine, outputFile, inFile);
            }
        }
    }
}

void processAST(FILE *inFile, FILE *outFile) {
    char line[MAX_LINE_LENGTH];
    char variables[MAX_VARIABLES][MAX_LINE_LENGTH];
    int varCount = 0;

    // First pass: Handle declarations
    while (fgets(line, sizeof(line), inFile)) {
        if (strstr(line, "Declaration")) {
            sscanf(line, "Declaration: type=int, identifier=%s", variables[varCount]);
            varCount++;
        }
    }

    // .data section
    fprintf(outFile, ".data\n");
    for (int i = 0; i < varCount; i++) {
        fprintf(outFile, "%s = 0\n", variables[i]);
    }
    fprintf(outFile, "tmp0 = 0\ntmp1 = 0\n\n.text\nstart:\n");

    // Second pass: Generate instructions
    rewind(inFile);
    while (fgets(line, sizeof(line), inFile)) {
        if (!strstr(line, "Declaration")) {
            handleInstruction(line, outFile, inFile);
        }
    }

    fprintf(outFile, "\nhlt\n");
}

int codegen_main() {
    const char *inputFile = "output.ast";
    const char *outputFile = "output.asm";

    FILE *inFile = fopen(inputFile, "r");
    FILE *outFile = fopen(outputFile, "w");
    if (!inFile || !outFile) {
        perror("File error");
        return 1;
    }

    processAST(inFile, outFile);

    fclose(inFile);
    fclose(outFile);

    printf("Assembly code has been generated in output.asm\n");
    return 0;
}