#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: touch <filename>\n");
        return 0;
    }

    FILE* f = fopen(argv[1], "w");
    fclose(f);
    return 0;
}
