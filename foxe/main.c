#include <stdio.h>
#include <stdlib.h>
#include <term.h>
#include <string.h>

char* input_buffer;
char* filename;
char* file_type;
char* mode;
bool is_edited = false;
bool is_in_insert_mode = true;
uint32_t old_color;

int current_size = 0;

unsigned int ln_cnt = 1;
unsigned int char_cnt = 1;

unsigned int buffer_idx = 0;

char* get_file_extension(const char* filename) {
    char* chr_ptr = strchr(filename, '.');
    return ++chr_ptr;
}

void render_status_bar(char* file_name,  bool is_edited, char* mode, unsigned int line_cnt, unsigned int char_count, char* f_type) {
    set_color(0);
    clear_screen();
    set_color(old_color);
    
    char edited = '-';
    if (is_edited) {
        edited = '+';
    }
    
    struct point_t screen_size = get_screen_size();
    set_cursor((struct point_t) { 0, screen_size.y - 16 });
    printf("File: %s [%c]      Mode: --%s--                                  Line: %d,%d   Type: [%s]", file_name, edited, mode, line_cnt, char_count, f_type);
    set_cursor((struct point_t) {0, 16});
    printf("%s", input_buffer);
}

void listen_input(FILE* f) {
    char inpt = getchar();
    if (!is_in_insert_mode) {
        switch (inpt) {
            case 'q':
                set_color(0);
                clear_screen();
                set_color(old_color);
                exit(1);
            case 27:
                is_in_insert_mode = !is_in_insert_mode;
                if (is_in_insert_mode) {
                    mode = "INSERT";
                } else {
                    mode = "EDIT";
                }
                break;
            case 'p':
                // write save
                break;
            default:
                break;
        }
    } else {
        switch (inpt) {
            case '\b':
                // backspace
                if (buffer_idx - 1 > 0) {
                    input_buffer = (char*) realloc((void*) input_buffer, f->size + (--current_size));
                    buffer_idx--;
                    printf("\b");
                }
                break;
            case 27:
                is_in_insert_mode = !is_in_insert_mode;
                if (is_in_insert_mode) {
                    mode = "INSERT";
                } else {
                    mode = "EDIT";
                }
                break;
            default:
                // just add the char at current pos and realloc
                input_buffer = (char*) realloc((void*) input_buffer, f->size + (++current_size));
                if (buffer_idx == f->size + current_size - 1) {
                    input_buffer[f->size + current_size - 1] = inpt;
                    buffer_idx++;
                }
                break;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Not enough arguments. foxe takes 1 argument but 0 were given!\nUsage: foxe <file-name>\n");
        return 1;
    } else if (argc > 2) {
        printf("To many arguments. foxe takes 1 argument but %d were given!\nUsage: foxe <file-name>\n", argc - 1);
        return 1;
    }
    filename = argv[1];
    file_type = get_file_extension(argv[1]);
    mode = "INSERT";

    old_color = get_color();
    set_color(0);
    clear_screen();
    set_color(old_color);

    FILE* f = fopen(argv[1], "w");
    input_buffer = (char*) malloc(sizeof(char) * f->size);

    while (true) {
        render_status_bar(filename, is_edited, mode, ln_cnt, char_cnt, file_type);
        listen_input(f);
    }

    fclose(f);
    set_color(old_color);
}
