#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define MAX_LENGTH 100
#define MAX_LINE_LENGTH 256
// TokenType enumeration
typedef enum {
    TOKEN_INT, TOKEN_IF, TOKEN_IDENTIFIER, TOKEN_NUMBER,
    TOKEN_ASSIGN, TOKEN_EQUAL, TOKEN_PLUS, TOKEN_MINUS,
    TOKEN_MULTIPLY, TOKEN_DIVIDE, TOKEN_SEMICOLON,
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_UNKNOWN
} TkType;

// String representations
const char *typeStr[] = {
    "TOKEN_INT", "TOKEN_IF", "TOKEN_IDENTIFIER", "TOKEN_NUMBER",
    "TOKEN_ASSIGN", "TOKEN_EQUAL", "TOKEN_PLUS", "TOKEN_MINUS",
    "TOKEN_MULTIPLY", "TOKEN_DIVIDE", "TOKEN_SEMICOLON",
    "TOKEN_LPAREN", "TOKEN_RPAREN", "TOKEN_LBRACE", "TOKEN_RBRACE",
    "TOKEN_UNKNOWN"
};
// Token type and value
typedef struct {
    char type[MAX_LINE_LENGTH];
    char value[MAX_LINE_LENGTH];
} Token;

// lexer functions

void writeTk(TkType type, const char *lexeme, FILE *opFile) {
    printf("%s, %s\n", typeStr[type], lexeme);
    fprintf(opFile, "%s, %s\n", typeStr[type], lexeme);
}

void lexAnalysis(const char *input, FILE *opFile) {
    for (int i = 0, len = strlen(input); i < len; i++) {
        char c = input[i];
        if (isspace(c)) continue;

        if (strchr(";(){}+-*/=", c)) {
            if (c == '=' && input[i + 1] == '=') {
                writeTk(TOKEN_EQUAL, "==", opFile);
                i++; // Consume next '='
            } else {
                TkType type = (c == ';') ? TOKEN_SEMICOLON :
                                 (c == '(') ? TOKEN_LPAREN :
                                 (c == ')') ? TOKEN_RPAREN :
                                 (c == '{') ? TOKEN_LBRACE :
                                 (c == '}') ? TOKEN_RBRACE :
                                 (c == '+') ? TOKEN_PLUS :
                                 (c == '-') ? TOKEN_MINUS :
                                 (c == '*') ? TOKEN_MULTIPLY :
                                 (c == '/') ? TOKEN_DIVIDE : TOKEN_ASSIGN;
                writeTk(type, (char[]){c, '\0'}, opFile);
            }
            continue;
        }

        if (isalnum(c)) {
            char buffer[MAX_LENGTH];
            int bufIndex = 0;
            while (isalnum(input[i]) && i < len) buffer[bufIndex++] = input[i++];
            buffer[bufIndex] = '\0';
            i--;// Step back for next character

            writeTk(strcmp(buffer, "int") == 0 ? TOKEN_INT : strcmp(buffer, "if") == 0 ? TOKEN_IF : isdigit(buffer[0]) ? TOKEN_NUMBER : TOKEN_IDENTIFIER, buffer, opFile);
        }
    }
}

// parser functions

int readTk(FILE *ipFile, Token *token) {
    char line[MAX_LINE_LENGTH];
    return fgets(line, sizeof(line), ipFile) && sscanf(line, "%[^,], %[^\n]", token->type, token->value) == 2;
}

// Add indented line in output and file
void givIndent(FILE *opFile, int indentLevel, const char *format, ...) {
    for (int i = 0; i < indentLevel; i++) fprintf(opFile, "  "), printf("  ");
    va_list args;
    va_start(args, format);
    vfprintf(opFile, format, args);
    va_end(args);
    fprintf(opFile, "\n");
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}
// parse expressions identifiers, numbers, operators etc...
void parseExp(FILE *ipFile, FILE *opFile, int indentLevel) {
    Token token;
    while (readTk(ipFile, &token)) {
        if (strcmp(token.type, "TOKEN_SEMICOLON") == 0 || strcmp(token.type, "TOKEN_RPAREN") == 0) 
        break;
        givIndent(opFile, indentLevel, "%s(%s)", strcmp(token.type, "TOKEN_IDENTIFIER") == 0 ? "Identifier" : strcmp(token.type, "TOKEN_NUMBER") == 0 ? "Number" : "Operator", token.value);
    }// we can add things for more expressions accordingly
}

// parse statements declarations, assignments, etc..)
void parseStatement(FILE *ipFile, FILE *opFile, Token *token, int indentLevel) {
    if (strcmp(token->type, "TOKEN_INT") == 0) {// for int
        readTk(ipFile, token);
        givIndent(opFile, indentLevel, "Declaration: type=int, identifier=%s", token->value);
        readTk(ipFile, token);
    } else if (strcmp(token->type, "TOKEN_IDENTIFIER") == 0) {// for identifier
        char identifier[MAX_LINE_LENGTH];
        strcpy(identifier, token->value);
        readTk(ipFile, token);
        givIndent(opFile, indentLevel, "Assignment:");
        givIndent(opFile, indentLevel + 1, "identifier=%s", identifier);
        givIndent(opFile, indentLevel + 1, "expression:");
        parseExp(ipFile, opFile, indentLevel + 2);
    } else if (strcmp(token->type, "TOKEN_IF") == 0) { // for if
        readTk(ipFile, token);
        givIndent(opFile, indentLevel, "IfStatement:");
        givIndent(opFile, indentLevel + 1, "condition:");
        parseExp(ipFile, opFile, indentLevel + 2);
        readTk(ipFile, token);
        givIndent(opFile, indentLevel + 1, "then:");
        while (readTk(ipFile, token) && strcmp(token->type, "TOKEN_RBRACE") != 0) 
        parseStatement(ipFile, opFile, token, indentLevel + 2);
        printf("if statement parsed success....\n");
    } else {
        fprintf(stderr, "Error: Not expected '%s'\n", token->value);
        exit(1);
    }// we can add other statements accordingly
}

// code generation functions

void genCode(FILE *astFile, FILE *opFile) {
    char line[MAX_LINE_LENGTH], currentVar[10], identifier[10], operator[3], conditionVar[10];
    int value, conditionValue;

    while (fgets(line, sizeof(line), astFile)) {
        char *trim = line;
        while (*trim == ' ') trim++;

        if (strncmp(trim, "Declaration:", 12) == 0) continue;
        else if (strncmp(trim, "Assignment:", 11) == 0) { // get current variable
            fgets(line, sizeof(line), astFile);
            sscanf(line, " identifier=%s", currentVar); 
        } else if (sscanf(trim, "Number(%d)", &value) == 1) { // get value bumber
            fprintf(opFile, "ldi A, %d\nsta %s\n", value, currentVar);
        } else if (sscanf(trim, "Identifier(%[^)])", identifier) == 1) { // get identifier
            fprintf(opFile, "lda %s\n", identifier);
        } else if (sscanf(trim, "Operator(%[^)])", operator) == 1) { // get operator
            if (strcmp(operator, "+") == 0) 
            fprintf(opFile, "add\n");
            else if (strcmp(operator, "-") == 0) 
            fprintf(opFile, "sub\n");
            else if (strcmp(operator, "*") == 0) 
            fprintf(opFile, "mov B, A\nldi A, 0\nmul_loop:\nadd B\nsta A\nlda B\ndec\njnz mul_loop\n");
            else if (strcmp(operator, "/") == 0) 
            fprintf(opFile, "mov B, A\nldi A, 0\ndiv_loop:\nsub B\njnc div_end\ninc A\njmp div_loop\ndiv_end:\n");
        } else if (strncmp(trim, "IfStatement:", 12) == 0) { // get if statement
            fgets(line, sizeof(line), astFile);
            fgets(line, sizeof(line), astFile);
            sscanf(line, " Identifier(%[^)])", conditionVar); // cheking condition (here we can make this to handle different conditions)
            fgets(line, sizeof(line), astFile);
            fgets(line, sizeof(line), astFile);
            sscanf(line, " Number(%d)", &conditionValue);

            fprintf(opFile, "lda %s\nldi B, %d\ncmp\njz else_label\n", conditionVar, conditionValue);

            while (fgets(line, sizeof(line), astFile)) { // loop for inside if statement block
                trim = line;
                while (*trim == ' ') trim++;
                if (strncmp(trim, "Assignment:", 11) == 0) { // get current variable
                    fgets(line, sizeof(line), astFile);
                    sscanf(line, " identifier=%s", currentVar);
                } else if (sscanf(trim, "Number(%d)", &value) == 1) {
                    fprintf(opFile, "ldi A, %d\nsta %s\n", value, currentVar);
                } else if (sscanf(trim, "Identifier(%[^)])", identifier) == 1) {
                    fprintf(opFile, "lda %s\n", identifier);
                } else if (sscanf(trim, "Operator(%[^)])", operator) == 1) {
                    if (strcmp(operator, "+") == 0) 
                    fprintf(opFile, "add\n");
                    else if (strcmp(operator, "-") == 0) 
                    fprintf(opFile, "sub\n");
                    else if (strcmp(operator, "*") == 0) 
                    fprintf(opFile, "mov B, A\nldi A, 0\nmul_loop:\nadd B\nsta A\nlda B\ndec\njnz mul_loop\n");
                    else if (strcmp(operator, "/") == 0) 
                    fprintf(opFile, "mov B, A\nldi A, 0\ndiv_loop:\nsub B\njnc div_end\ninc A\njmp div_loop\ndiv_end:\n");
                } else if (strncmp(trim, "then:", 5) == 0 || strncmp(trim, "Assignment:", 11) == 0) continue;
                else break;
            }
            fprintf(opFile, "jmp end_if\nelse_label:\nend_if:\n");
        }
    }
}

int main() {
    // Lexical Analysis
    FILE *lexinputFile = fopen("input.txt", "r");
    FILE *lexOpFile = fopen("output.l", "w");
    if (!lexinputFile || !lexOpFile) {
        fprintf(stderr, "lexical File error ...\n");
        return 1;
    }
    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), lexinputFile)) {
        lexAnalysis(line, lexOpFile);
    }
    fclose(lexinputFile);
    fclose(lexOpFile);
    printf("Lexical Success ...\n");

    // Parsing
    FILE *parsinputFile = fopen("output.l", "r");
    FILE *parsOpFile = fopen("output.ast", "w");
    if (!parsinputFile || !parsOpFile) {
        fprintf(stderr, "Parse File error ...\n");
        return 1;
    }
    printf("************************ Token parsing start ****************************\n");
    givIndent(parsOpFile, 0, "Program");
    Token token;
    while (readTk(parsinputFile, &token)) parseStatement(parsinputFile, parsOpFile, &token, 1);
    printf("Token parsing success..\n");
    fclose(parsinputFile);
    fclose(parsOpFile);

    // Code Generation
    FILE *astFile = fopen("output.ast", "r");
    FILE *opFile = fopen("output.asm", "w");
    if (!astFile || !opFile) {
        fprintf(stderr, "Code gen File error ...\n");
        return 1;
    }
    printf("*************************** Code generating start **************************\n");
    genCode(astFile, opFile);
    printf("code generated Success ...\n");
    fclose(astFile);
    fclose(opFile);

    return 0;
}
