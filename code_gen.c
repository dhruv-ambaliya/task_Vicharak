#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

void genCode(FILE *ipFile, FILE *opFile) {
    char line[MAX_LINE_LENGTH], currentVar[10], identifier[10], operator[3], conditionVar[10];
    int value, conditionValue;

    while (fgets(line, sizeof(line), ipFile)) {
        char *trimmed = line;
        while (*trimmed == ' ') trimmed++;

        if (strncmp(trimmed, "Declaration:", 12) == 0) continue;
        else if (strncmp(trimmed, "Assignment:", 11) == 0) {
            fgets(line, sizeof(line), ipFile);
            sscanf(line, " identifier=%s", currentVar);
        } else if (sscanf(trimmed, "Number(%d)", &value) == 1) {
            fprintf(opFile, "ldi A, %d\nsta %s\n", value, currentVar);
        } else if (sscanf(trimmed, "Identifier(%[^)])", identifier) == 1) {
            fprintf(opFile, "lda %s\n", identifier);
        } else if (sscanf(trimmed, "Operator(%[^)])", operator) == 1) {
            if (strcmp(operator, "+") == 0) 
            fprintf(opFile, "add\n");
            else if (strcmp(operator, "-") == 0) 
            fprintf(opFile, "sub\n");
            else if (strcmp(operator, "*") == 0) 
            fprintf(opFile, "mov B, A\nldi A, 0\nmul_loop:\nadd B\nsta A\nlda B\ndec\njnz mul_loop\n");
            else if (strcmp(operator, "/") == 0) 
            fprintf(opFile, "mov B, A\nldi A, 0\ndiv_loop:\nsub B\njnc div_end\ninc A\njmp div_loop\ndiv_end:\n");
        } else if (strncmp(trimmed, "IfStatement:", 12) == 0) {
            fgets(line, sizeof(line), ipFile);
            fgets(line, sizeof(line), ipFile);
            sscanf(line, " Identifier(%[^)])", conditionVar);
            fgets(line, sizeof(line), ipFile);
            fgets(line, sizeof(line), ipFile);
            sscanf(line, " Number(%d)", &conditionValue);

            fprintf(opFile, "lda %s\nldi B, %d\ncmp\njz else_label\n", conditionVar, conditionValue);

            while (fgets(line, sizeof(line), ipFile)) {
                trimmed = line;
                while (*trimmed == ' ') trimmed++;
                if (strncmp(trimmed, "Assignment:", 11) == 0) {
                    fgets(line, sizeof(line), ipFile);
                    sscanf(line, " identifier=%s", currentVar);
                } else if (sscanf(trimmed, "Number(%d)", &value) == 1) {
                    fprintf(opFile, "ldi A, %d\nsta %s\n", value, currentVar);
                } else if (sscanf(trimmed, "Identifier(%[^)])", identifier) == 1) {
                    fprintf(opFile, "lda %s\n", identifier);
                } else if (sscanf(trimmed, "Operator(%[^)])", operator) == 1) {
                    if (strcmp(operator, "+") == 0) 
                    fprintf(opFile, "add\n");
                    else if (strcmp(operator, "-") == 0) 
                    fprintf(opFile, "sub\n");
                    else if (strcmp(operator, "*") == 0) 
                    fprintf(opFile, "mov B, A\nldi A, 0\nmul_loop:\nadd B\nsta A\nlda B\ndec\njnz mul_loop\n");
                    else if (strcmp(operator, "/") == 0) 
                    fprintf(opFile, "mov B, A\nldi A, 0\ndiv_loop:\nsub B\njnc div_end\ninc A\njmp div_loop\ndiv_end:\n");
                } else if (strncmp(trimmed, "then:", 5) == 0 || strncmp(trimmed, "Assignment:", 11) == 0) continue;
                else break;
            }
            fprintf(opFile, "jmp end_if\nelse_label:\nend_if:\n");
        }
    }
}

int main() {
    FILE *ipFile = fopen("output.ast", "r");
    FILE *opFile = fopen("output.asm", "w");

    if (!ipFile || !opFile) {
        fprintf(stderr, "Error opening files.\n");
        return 1;
    }

    printf("Code generating Start ...\n");
    genCode(ipFile, opFile);

    printf("Code generated success ...\n");
    fclose(ipFile);
    fclose(opFile);
    return 0;
}
