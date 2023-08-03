#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assembler.h>

int main(int argc, char* argv[]) {
    int option_args = 0; //Args to skip later

    executable_format_t output_format = BIN_FORMAT;

    if (argc > 1) {
        while (option_args < argc - 1) {
            char* arg = argv[option_args + 1];

            if (arg[0] != '-') {
                break;
            }
            option_args++;

            if (strcmp(arg, "-h") == 0) {
                printf("Usage: %s [OPTIONS] <source> <output>\n\nOptions:\n -h: Displays this help message.\n -f: Set output executable format.\n", argv[0]);
                return 0;
            } else if (strcmp(arg, "-f") == 0) {
                option_args++;
                if (option_args >= argc) {
                    printf("Error: No output format specified.\n");
                    return 1;
                }

                output_format = get_exec_type(argv[option_args]);
                if (output_format == -1) {
                    printf("Error: Unknown executable format '%s'\n", argv[option_args]);
                    return 1;
                }
            } else {
                printf("Error: Unknown option '%s'\n", arg);
                return 1;
            }
        }
    }

    if (argc - option_args < 3) {
        printf("Usage: %s [OPTIONS] <source> <output>\n", argv[0]);
        return 1;
    }

    char* source_file = argv[option_args + 1];
    char* output_file = argv[option_args + 2];

    FILE* source = fopen(source_file, "r");
    if (!source) {
        printf("Error: Could not open source file '%s'\n", source_file);
        return 1;
    }
    
    FILE* output = fopen(output_file, "w");
    if (!output) {
        printf("Error: Could not create output file '%s'\n", output_file);
        return 1;
    }

    printf("Starting assembly...\n");
    if (assemble(source, output, output_format)) {
        printf("Done! Binary written to %s.\n", output_file);
    } else {
        printf("Assembly failed.\n");
        exit(1);
    }

    fclose(source);
    fclose(output);

    return 0;
}
