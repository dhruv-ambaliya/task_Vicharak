#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LENGTH 100
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
                i++;
            } else { // Consume next '='
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
            i--; // Step back for next character

            writeTk(strcmp(buffer, "int") == 0 ? TOKEN_INT : strcmp(buffer, "if") == 0 ? TOKEN_IF : isdigit(buffer[0]) ? TOKEN_NUMBER : TOKEN_IDENTIFIER, buffer, opFile);
        }
    }
}

int main() {
    FILE *ipFile = fopen("input.txt", "r");
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