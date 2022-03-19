#include <stdio.h>
#include <stdlib.h>
#include <term.h>
#include <string.h>

char* input_buffer;
char* filename;
char* file_type;
FILE* f;
char* mode;
bool is_edited = false;
bool is_in_insert_mode = true;
uint32_t old_color;

int current_size = 1;

unsigned int ln_cnt = 1;
unsigned int char_cnt = 0;

unsigned int buffer_idx = 0;

char* get_file_extension(const char* filename) {
    char* chr_ptr = strchr(filename, '.');
    return ++chr_ptr;
}

void render_status_bar() {
    set_color(0);
    clear_screen();
    set_color(old_color);
    
    char edited = '-';
    if (is_edited) {
        edited = '+';
    }
    
    struct point_t screen_size = get_screen_size();
    set_cursor((struct point_t) { 0, screen_size.y - 16 });
    printf("File: %s [%c]      Mode: --%s--                                  Line: %d,%d   Type: [%s]", filename, edited, mode, ln_cnt, char_cnt, file_type);
    set_cursor((struct point_t) {0, 16});    

    for (int i = 0; i < current_size; i++) {
        printf("%c", input_buffer[i]);
    }
}

void listen_input(FILE* f) {
    char input = getchar();

    if (!is_in_insert_mode) {
        switch (input) {
            case 'q':
                set_color(0);
                clear_screen();
                set_color(old_color);
                exit(1);
            case '\e':
                is_in_insert_mode = !is_in_insert_mode;
                mode = "INSERT";
                break;
            case 'a':
                // move left one char
                if (!buffer_idx <= 0) {
                    buffer_idx -= 1;
                }
                break;
            case '+':
                // write save
                fwrite(input_buffer, sizeof(char), current_size * sizeof(char), f);
                break;
            case '-':
                // discard changes
                set_color(0);
                clear_screen();
                set_color(old_color);

                printf("Discard Changes (y/n)? ");

                char discard_input = getchar();
                if (discard_input == 121 || discard_input == 89) {
                    // reallocate to whats currently in the file
                    buffer_idx = f->size;
                    current_size = f->size;
                    input_buffer = (char*) realloc((void*) input_buffer, sizeof(char) * f->size);
                    fread((void*) input_buffer, sizeof(char), f->size * sizeof(char), f);
                    is_edited = false;
                    render_status_bar();
                }
                break;
            default:
                printf("");
                break;
        }
    } else {
        switch (input) {
            case '\b': {
                if (input_buffer[buffer_idx] == '\n') {
                    ln_cnt--;
                } else {
                    char_cnt--;
                }

                if (buffer_idx - 1 < 0 || current_size - 1 < 0) {
                } else {
                if (buffer_idx == current_size) {
                } else {
                    for (int i = buffer_idx + 1; i < current_size; i++) {
                        input_buffer[i - 1] = input_buffer[i];
                    }
                    //input_buffer = (char*) memcpy((void*) &input_buffer[buffer_idx - 1], (void*) &input_buffer[buffer_idx + 1], (current_size-buffer_idx) * sizeof(input_buffer[0]));
                }
                input_buffer = (char*) realloc((void*) input_buffer, --current_size);
                buffer_idx--;
                }
            }
            break;
            case '\e': {
                is_in_insert_mode = !is_in_insert_mode;
                mode = "EDIT";
                break;
            }


            default: {
                if (input == '\n') {
                    ln_cnt++;
                } else {
                    char_cnt++;
                }

                is_edited = true;
                input_buffer = (char*) realloc((void*) input_buffer, ++current_size);
                memcpy(input_buffer + buffer_idx + 1, input_buffer + buffer_idx, current_size - buffer_idx/* - 1*/);
                input_buffer[buffer_idx] = input;
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
    file_type = "";
    file_type = get_file_extension(argv[1]);
    mode = "INSERT";

    old_color = get_color();
    set_color(0);
    clear_screen();
    set_color(old_color);

    f = fopen(argv[1], "w");
    if (f->size != 0) {
        input_buffer = (char*) malloc(sizeof(char) * f->size);
    }
    current_size = f->size;
    buffer_idx = f->size;
    fread((void*) input_buffer, sizeof(char), f->size * sizeof(char), f);

    while (true) {
        render_status_bar();
        listen_input(f);
    }

    fclose(f);
    set_color(old_color);
}
