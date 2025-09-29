#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LENGTH 100

typedef enum {
    TOKEN_INT, TOKEN_IF,TOKEN_ELSE,TOKEN_WHILE, TOKEN_IDENTIFIER, TOKEN_NUMBER,
    TOKEN_ASSIGN, TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE, 
    TOKEN_OUT,TOKEN_SEMICOLON,
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_EQUAL,TOKEN_LESS, TOKEN_GREATER, TOKEN_LESS_EQUAL, TOKEN_GREATER_EQUAL, TOKEN_NOT_EQUAL,
    TOKEN_UNKNOWN
} TkType;

const char *typeStr[] = {
    "TOKEN_INT", "TOKEN_IF","TOKEN_ELSE", "TOKEN_WHILE", "TOKEN_IDENTIFIER", "TOKEN_NUMBER",
    "TOKEN_ASSIGN", "TOKEN_PLUS", "TOKEN_MINUS","TOKEN_MULTIPLY", "TOKEN_DIVIDE", 
    "TOKEN_OUT","TOKEN_SEMICOLON",
    "TOKEN_LPAREN", "TOKEN_RPAREN", "TOKEN_LBRACE", "TOKEN_RBRACE", 
    "TOKEN_EQUAL", "TOKEN_LESS", "TOKEN_GREATER", "TOKEN_LESS_EQUAL", "TOKEN_GREATER_EQUAL", "TOKEN_NOT_EQUAL",
    "TOKEN_UNKNOWN"
}; //array of strings typeStr corresponds TkType

void writeTk(TkType type, const char *lexeme, FILE *opFile) {
    printf("%s, %s\n", typeStr[type], lexeme);
    fprintf(opFile, "%s, %s\n", typeStr[type], lexeme); // printing 
}

void lexAnalysis(const char *input, FILE *opFile) {
    if (input[0] == '/' && input[1] == '/') {
        return; // ignore comments
    }
    for (int i = 0, len = strlen(input); i < len; i++) { 
        char c = input[i];
        if (isspace(c)) continue; // whitespace ignored

        if (strchr(";(){}+-*/=<>!", c)) { // chckes this symbols and give tokens
            TkType type = TOKEN_UNKNOWN;
            char lexeme[3] = {c, '\0', '\0'};
            if (c == '=' && input[i + 1] == '=') { type = TOKEN_EQUAL; lexeme[1] = '='; i++; }
            else if (c == '<' && input[i + 1] == '=') { type = TOKEN_LESS_EQUAL; lexeme[1] = '='; i++; }
            else if (c == '>' && input[i + 1] == '=') { type = TOKEN_GREATER_EQUAL; lexeme[1] = '='; i++; }
            else if (c == '!' && input[i + 1] == '=') { type = TOKEN_NOT_EQUAL; lexeme[1] = '='; i++; }
            else {
                switch (c) {
                    case ';': type = TOKEN_SEMICOLON; break;
                    case '(': type = TOKEN_LPAREN; break;
                    case ')': type = TOKEN_RPAREN; break;
                    case '{': type = TOKEN_LBRACE; break;
                    case '}': type = TOKEN_RBRACE; break;
                    case '+': type = TOKEN_PLUS; break;
                    case '-': type = TOKEN_MINUS; break;
                    case '*': type = TOKEN_MULTIPLY; break;
                    case '/': type = TOKEN_DIVIDE; break;
                    case '<': type = TOKEN_LESS; break;
                    case '>': type = TOKEN_GREATER; break;
                    case '=': type = TOKEN_ASSIGN; break;
                }
            }
            writeTk(type, lexeme, opFile);
            continue;
        }

        if (isalnum(c)) {
            char buffer[MAX_LENGTH];
            int bufIndex = 0;
            while (isalnum(input[i]) && i < len) buffer[bufIndex++] = input[i++];
            buffer[bufIndex] = '\0';
            i--; // Step back for next character
            TkType type = TOKEN_IDENTIFIER; // for other tokens
            if (strcmp(buffer, "int") == 0) type = TOKEN_INT;
            else if (strcmp(buffer, "if") == 0) type = TOKEN_IF;
            else if (strcmp(buffer, "else") == 0) type = TOKEN_ELSE;
            else if (strcmp(buffer, "out") == 0) type = TOKEN_OUT;
            else if (strcmp(buffer, "while") == 0) type = TOKEN_WHILE;
            else if (isdigit(buffer[0])) type = TOKEN_NUMBER;

            writeTk(type, buffer, opFile);
        }
    }
}

int lex_main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *ipFile = fopen(argv[1], "r");
    FILE *opFile = fopen("output.l", "w");

    if (!ipFile || !opFile) {
        fprintf(stderr, "Error opening files.\n");
        return 1;
    }

    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), ipFile)) {
        lexAnalysis(line, opFile);
    }

    fclose(ipFile);
    fclose(opFile);

    printf("Lexical success ...\n");
    return 0;
}