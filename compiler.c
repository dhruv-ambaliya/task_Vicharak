#include <stdio.h>

int lex_main(int argc, char *argv[]);
int parse_main(int argc, char *argv[]);
int codegen_main(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    printf("Step 1: Lexical Analysis\n");
    if (lex_main(argc, argv) != 0) {
        fprintf(stderr, "Lexical analysis failed.\n");
        return 1;
    }

    printf("Step 2: Parsing\n");
    if (parse_main(argc, argv) != 0) {
        fprintf(stderr, "Parsing failed.\n");
        return 1;
    }

    printf("Step 3: Code Generation\n");
    if (codegen_main(argc, argv) != 0) {
        fprintf(stderr, "Code generation failed.\n");
        return 1;
    }

    printf("Compilation process completed successfully.\n");
    return 0;
}
