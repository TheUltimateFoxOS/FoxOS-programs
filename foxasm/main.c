#include <stdio.h>
#include <stdlib.h>

#include <assembler.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <source> <output>\n", argv[0]);
        return 1;
    }

    FILE* source = fopen(argv[1], "r");
    if (!source) {
        printf("Error: Could not open source file '%s'\n", argv[1]);
        return 1;
    }
    
    FILE* output = fopen(argv[2], "w");
    if (!output) {
        printf("Error: Could not create output file '%s'\n", argv[2]);
        return 1;
    }

    assemble(source, output);

    fclose(source);
    fclose(output);

    return 0;
}
