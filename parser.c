#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 256
#define MAX_IDENTIFIERS 100

typedef struct {
    char type[MAX_LINE_LENGTH];
    char value[MAX_LINE_LENGTH];
} Token;

int lineNumber = 0;
char decIdenti[MAX_IDENTIFIERS][MAX_LINE_LENGTH];
int decCount = 0;

// check if an identifier is declared
bool isDeclared(const char *identi) {
    for (int i = 0; i < decCount; i++) {
        if (strcmp(decIdenti[i], identi) == 0) {
            return true;
        }
    }
    return false;
}

// adding a declared identifier
void addIdenti(const char *identi) {
    if (decCount < MAX_IDENTIFIERS) {
        strcpy(decIdenti[decCount++], identi);
    } else {
        printf("Error: out off limit idetifier declaration !!\n");
        exit(1);
    }
}

// Reads a token from the input file and checks if it matches the expected types
int readTk(FILE *ipFile, Token *token, const char *expectedType) {
    char line[MAX_LINE_LENGTH];
    if (fgets(line, sizeof(line), ipFile) && sscanf(line, "%[^,], %[^\n]", token->type, token->value) == 2) {
        lineNumber++;
        if (expectedType && strcmp(token->type, expectedType) != 0) {
            printf("\nSyntax error on line %d: expected '%s' but got '%s'\n\n", lineNumber, expectedType, token->value);
            exit(1);
        }
        return 1;
    }
    return 0;
}

// Parses expression
void parseExp(FILE *ipFile, char *expression, int *isArithmetic) {
    Token token;
    readTk(ipFile, &token, NULL);
    if (strcmp(token.type, "TOKEN_IDENTIFIER") != 0 && strcmp(token.type, "TOKEN_NUMBER") != 0) {
        printf("\nSyntax error on line %d: expected 'TOKEN_IDENTIFIER' or 'TOKEN_NUMBER' but got '%s'\n\n", lineNumber, token.value);
        exit(1);
    }
    if (strcmp(token.type, "TOKEN_IDENTIFIER") == 0 && !isDeclared(token.value)) {
        printf("\nSyntax error on line %d: identifier '%s' not declared\n\n", lineNumber, token.value);
        exit(1);
    } // already declared
    sprintf(expression + strlen(expression), "%s(%s) ", strcmp(token.type, "TOKEN_IDENTIFIER") == 0 ? "Identifier" : "Number", token.value);

    readTk(ipFile, &token, NULL);
    if (strcmp(token.type, "TOKEN_SEMICOLON") == 0) {
        *isArithmetic = 0;
        return;
    } //if ; then its normal assignment

    if (strcmp(token.type, "TOKEN_PLUS") != 0 && strcmp(token.type, "TOKEN_MINUS") != 0 &&
        strcmp(token.type, "TOKEN_MULTIPLY") != 0 && strcmp(token.type, "TOKEN_DIVIDE") != 0) {
        printf("\nSyntax error on line %d: expected 'Operator' but got '%s'\n\n", lineNumber, token.value);
        exit(1);
    }
    sprintf(expression + strlen(expression), "Operator(%s) ", token.value);
    *isArithmetic = 1;

    readTk(ipFile, &token, NULL);
    if (strcmp(token.type, "TOKEN_IDENTIFIER") != 0 && strcmp(token.type, "TOKEN_NUMBER") != 0) {
        printf("\nSyntax error on line %d: expected 'TOKEN_IDENTIFIER' or 'TOKEN_NUMBER' but got '%s'\n\n", lineNumber, token.value);
        exit(1);
    }
    if (strcmp(token.type, "TOKEN_IDENTIFIER") == 0 && !isDeclared(token.value)) {
        printf("\nSyntax error on line %d: identifier '%s' not declared\n", lineNumber, token.value);
        exit(1);
    } // already declared
    sprintf(expression + strlen(expression), "%s(%s) ", strcmp(token.type, "TOKEN_IDENTIFIER") == 0 ? "Identifier" : "Number", token.value);

    readTk(ipFile, &token, NULL);
    if (strcmp(token.type, "TOKEN_SEMICOLON") != 0) {
        printf("\nSyntax error on line %d: expected 'TOKEN_SEMICOLON' but got '%s'\n\n", lineNumber, token.value);
        exit(1);
    } 
}

void parseComp(FILE *ipFile, FILE *outputFile) {
    Token token;
    readTk(ipFile, &token, NULL);
    if (strcmp(token.type, "TOKEN_IDENTIFIER") != 0 && strcmp(token.type, "TOKEN_NUMBER") != 0) {
        printf("\nSyntax error on line %d: expected 'TOKEN_IDENTIFIER' or 'TOKEN_NUMBER' but got '%s'\n\n", lineNumber, token.value);
        exit(1);
    }
    if (strcmp(token.type, "TOKEN_IDENTIFIER") == 0 && !isDeclared(token.value)) {
        printf("\nSyntax error on line %d: identifier '%s' not declared\n", lineNumber, token.value);
        exit(1);
    }
    fprintf(outputFile, "%s(%s) ", strcmp(token.type, "TOKEN_IDENTIFIER") == 0 ? "Identifier" : "Number", token.value);

    readTk(ipFile, &token, NULL);
    if (strcmp(token.type, "TOKEN_EQUAL") != 0 && strcmp(token.type, "TOKEN_LESS") != 0 &&
        strcmp(token.type, "TOKEN_GREATER") != 0 && strcmp(token.type, "TOKEN_LESS_EQUAL") != 0 &&
        strcmp(token.type, "TOKEN_GREATER_EQUAL") != 0 && strcmp(token.type, "TOKEN_NOT_EQUAL") != 0) {
        printf("\nSyntax error on line %d: expected 'Comparison' but got '%s'\n\n", lineNumber, token.value);
        exit(1);
    }
    fprintf(outputFile, "Comparison(%s) ", token.value);

    readTk(ipFile, &token, NULL);
    if (strcmp(token.type, "TOKEN_IDENTIFIER") != 0 && strcmp(token.type, "TOKEN_NUMBER") != 0) {
        printf("\nSyntax error on line %d: expected 'TOKEN_IDENTIFIER' or 'TOKEN_NUMBER' but got '%s'\n\n", lineNumber, token.value);
        exit(1);
    }
    if (strcmp(token.type, "TOKEN_IDENTIFIER") == 0 && !isDeclared(token.value)) {
        printf("\nSyntax error on line %d: identifier '%s' not declared\n\n", lineNumber, token.value);
        exit(1);
    }
    fprintf(outputFile, "%s(%s)\n", strcmp(token.type, "TOKEN_IDENTIFIER") == 0 ? "Identifier" : "Number", token.value);
}

// Parses a statement
void parseStmt(FILE *ipFile, FILE *outputFile, Token *token) {
    if (strcmp(token->type, "TOKEN_INT") == 0) {
        readTk(ipFile, token, "TOKEN_IDENTIFIER");  // Read the identifier
        if (isDeclared(token->value)) {
            printf("\nSyntax error on line %d: identifier '%s' already declared\n\n", lineNumber, token->value);
            exit(1);
        }
        addIdenti(token->value);
        fprintf(outputFile, "Declaration: type=int, identifier=%s\n", token->value);
        readTk(ipFile, token, "TOKEN_SEMICOLON");  // skip semicolon
    } else if (strcmp(token->type, "TOKEN_IDENTIFIER") == 0) {
        if (!isDeclared(token->value)) {
            printf("\nSyntax error on line %d: identifier '%s' not declared\n\n", lineNumber, token->value);
            exit(1);
        }
        char identi[MAX_LINE_LENGTH];
        strcpy(identi, token->value);
        readTk(ipFile, token, "TOKEN_ASSIGN");  // Expect =
        int isArithmetic = 0;
        char expression[MAX_LINE_LENGTH] = "";
        parseExp(ipFile, expression, &isArithmetic);
        fprintf(outputFile, "%s: identifier=%s expression: %s\n", isArithmetic ? "Arithmetic" : "Assignment", identi, expression);
    } else if (strcmp(token->type, "TOKEN_OUT") == 0) {
        readTk(ipFile, token, "TOKEN_LPAREN");  // Read the identifier
        readTk(ipFile, token, NULL);
        if (strcmp(token->type, "TOKEN_IDENTIFIER") != 0 && strcmp(token->type, "TOKEN_NUMBER") != 0) {
            printf("\nSyntax error on line %d: expected 'TOKEN_IDENTIFIER' or 'TOKEN_NUMBER' but got '%s'\n\n", lineNumber, token->value);
            exit(1);
        }
        if (strcmp(token->type, "TOKEN_IDENTIFIER") == 0 && !isDeclared(token->value)) {
            printf("\nSyntax error on line %d: identifier '%s' not declared\n\n", lineNumber, token->value);
            exit(1);
        }
        fprintf(outputFile, "Output: %s(%s)\n", strcmp(token->type, "TOKEN_IDENTIFIER") == 0 ? "Identifier" : "Number", token->value);
        readTk(ipFile, token, "TOKEN_RPAREN");
        readTk(ipFile, token, "TOKEN_SEMICOLON"); 
    }    else if (strcmp(token->type, "TOKEN_IF") == 0) {
        readTk(ipFile, token, "TOKEN_LPAREN");  // Expect (
        fprintf(outputFile, "IfStatement: condition: ");
        parseComp(ipFile, outputFile);
        readTk(ipFile, token, "TOKEN_RPAREN");  // Expect )
        fprintf(outputFile, "then:\n");

        // Check for '{'
        if (!readTk(ipFile, token, NULL) || strcmp(token->type, "TOKEN_LBRACE") != 0) {
            printf("\nSyntax error on line %d: expected '{' at the start of if block\n\n", lineNumber);
            exit(1);
        }

        while (readTk(ipFile, token, NULL) && strcmp(token->type, "TOKEN_RBRACE") != 0) {
            parseStmt(ipFile, outputFile, token);
        }

        // Check for '}'
        if (strcmp(token->type, "TOKEN_RBRACE") != 0) {
            printf("\nSyntax error on line %d: expected '}' at the end of if block\n\n", lineNumber);
            exit(1);
        }

        fprintf(outputFile, "endif\n");
        long pos = ftell(ipFile); // Save current file position
        if (readTk(ipFile, token, NULL) && strcmp(token->type, "TOKEN_ELSE") == 0) {
            fprintf(outputFile, "Else:\n");
            // Check for {
            if (!readTk(ipFile, token, NULL) || strcmp(token->type, "TOKEN_LBRACE") != 0) {
                printf("\nSyntax error on line %d: expected '{' at the start of else block\n\n", lineNumber);
                exit(1);
            }
            while (readTk(ipFile, token, NULL) && strcmp(token->type, "TOKEN_RBRACE") != 0) {
                parseStmt(ipFile, outputFile, token);
            }
            if (strcmp(token->type, "TOKEN_RBRACE") != 0) {
                printf("\nSyntax error on line %d: expected '}' at the end of else block\n\n", lineNumber);
                exit(1);
            }
            fprintf(outputFile, "endelse\n");
        } else {
            fseek(ipFile, pos, SEEK_SET); // Rewind if no else
        }
    } else {
        printf("\nSyntax error on line %d: unexpected token '%s'\n\n", lineNumber, token->value);
        exit(1);
    }
}

int parse_main() {
    FILE *ipFile = fopen("output.l", "r");
    FILE *outputFile = fopen("output.ast", "w");

    if (!ipFile || !outputFile) {
        fprintf(stderr, "File error...\n");
        return 1;
    }

    printf("Parsing tokens...\n");
    Token token;

    while (readTk(ipFile, &token, NULL)) {
        parseStmt(ipFile, outputFile, &token);
    }

    printf("Token parsing successful.\n");

    fclose(ipFile);
    fclose(outputFile);

    return 0;
}
