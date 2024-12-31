#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAX_LINE_LENGTH 256

typedef struct {
    char type[MAX_LINE_LENGTH];
    char value[MAX_LINE_LENGTH];
} Token;

int readTk(FILE *ipFile, Token *token) {
    char line[MAX_LINE_LENGTH];
    return fgets(line, sizeof(line), ipFile) && sscanf(line, "%[^,], %[^\n]", token->type, token->value) == 2;
}

void givIndent(FILE *outputFile, int indentLevel, const char *format, ...) {
    for (int i = 0; i < indentLevel; i++) fprintf(outputFile, "  "), printf("  ");
    va_list args;
    va_start(args, format);
    vfprintf(outputFile, format, args);
    va_end(args);
    fprintf(outputFile, "\n");
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

void parseExp(FILE *ipFile, FILE *outputFile, int indentLevel) {
    Token token;
    while (readTk(ipFile, &token)) {
        if (strcmp(token.type, "TOKEN_SEMICOLON") == 0 || strcmp(token.type, "TOKEN_RPAREN") == 0) break;
        givIndent(outputFile, indentLevel, "%s(%s)", strcmp(token.type, "TOKEN_IDENTIFIER") == 0 ? "Identifier" : strcmp(token.type, "TOKEN_NUMBER") == 0 ? "Number" : "Operator", token.value);
    }
}

void parseStatement(FILE *ipFile, FILE *outputFile, Token *token, int indentLevel) {
    if (strcmp(token->type, "TOKEN_INT") == 0) {
        readTk(ipFile, token);
        givIndent(outputFile, indentLevel, "Declaration: type=int, identifier=%s", token->value);
        readTk(ipFile, token);
    } else if (strcmp(token->type, "TOKEN_IDENTIFIER") == 0) {
        char identifier[MAX_LINE_LENGTH];
        strcpy(identifier, token->value);
        readTk(ipFile, token);
        givIndent(outputFile, indentLevel, "Assignment:");
        givIndent(outputFile, indentLevel + 1, "identifier=%s", identifier);
        givIndent(outputFile, indentLevel + 1, "expression:");
        parseExp(ipFile, outputFile, indentLevel + 2);
    } else if (strcmp(token->type, "TOKEN_IF") == 0) {
        readTk(ipFile, token);
        givIndent(outputFile, indentLevel, "IfStatement:");
        givIndent(outputFile, indentLevel + 1, "condition:");
        parseExp(ipFile, outputFile, indentLevel + 2);
        readTk(ipFile, token);
        givIndent(outputFile, indentLevel + 1, "then:");
        while (readTk(ipFile, token) && strcmp(token->type, "TOKEN_RBRACE") != 0) parseStatement(ipFile, outputFile, token, indentLevel + 2);
        printf("if statement parsed success....\n");
    } else {
        fprintf(stderr, "Error: Not expected '%s'\n", token->value);
        exit(1);
    }
}

int main() {
    FILE *ipFile = fopen("output.l", "r");
    FILE *outputFile = fopen("output.ast", "w");
    if (!ipFile || !outputFile) {
        fprintf(stderr, "File error ...\n");
        return 1;
    }
    givIndent(outputFile, 0, "Program");
    printf("Token parsing start...\n");
    Token token;
    while (readTk(ipFile, &token)) parseStatement(ipFile, outputFile, &token, 1);
    printf("Token parsing success..\n");
    fclose(ipFile);
    fclose(outputFile);
    return 0;
}