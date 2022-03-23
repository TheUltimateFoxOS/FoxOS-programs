#include <stdio.h>
#include <stdlib.h>
#include <term.h>
#include <string.h>
#include <sys/env.h>
#include <sys/write.h>
#define CHAR_SIZE 16

char* input_buffer;
char* filename;
char* file_type;
FILE* f;
char* mode;
bool is_edited = false;
bool is_in_insert_mode = true;
uint32_t old_color;

int current_size = 1;

int possible_lines_to_draw;

unsigned int ln_cnt = 1;
unsigned int char_cnt = 0;

unsigned int buffer_ln_idx = 0;
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

    set_cursor((struct point_t) { 0, screen_size.y - CHAR_SIZE });
    printf("File: %s [%c]      Mode: --%s--              Current Line: %d    Line: %d,%d   Type: [%s]", filename, edited, mode, buffer_ln_idx, ln_cnt, char_cnt, file_type);
    set_cursor((struct point_t) {0, CHAR_SIZE});
    
    int j = 0;
    int allready_drawn = 0;
    for (int i = 0; i < current_size; i++) {
        if ((ln_cnt - 1 < possible_lines_to_draw || j >= buffer_ln_idx) && allready_drawn <= possible_lines_to_draw) {
            if (i == buffer_idx) {
                set_color(0x33cccc);
                putchar('|');
                set_color(old_color);
            }
            putchar(input_buffer[i]);
            if (input_buffer[i] == '\n') allready_drawn++;
        } else {
            if (input_buffer[i] == '\n') j++; 
        }
    }
}

bool listen_input(FILE* f) {
    char input = getchar();

    if (!is_in_insert_mode) {
        switch (input) {
            case 'q':
				return true;
            case '\e':
                is_in_insert_mode = !is_in_insert_mode;
                mode = "INSERT";
                break;
            case 'a':
                // move left one char
                if (!buffer_idx <= 0) {
                    if (input_buffer[buffer_idx - 1] == '\n') buffer_ln_idx--;
                    buffer_idx -= 1;
                }
                break;
            case 'd':
                // move right one char
                if (buffer_idx < current_size) {
                    if (input_buffer[buffer_idx] == '\n') buffer_ln_idx++;
                    buffer_idx += 1;
                }
                break;
            case 'w': {
                    if (buffer_ln_idx <= 0 || buffer_idx <= 0) {
                    } else {
                        // move one line up
                        int prev_buff = buffer_idx;

                        for (int i = buffer_idx; i > 0; i--) {
                            buffer_idx--;
                            if (input_buffer[i - 1] == '\n' || buffer_idx < 0) {
                                break;
                            }
                        }
                        if (buffer_idx < 0) {
                            buffer_idx = prev_buff;
                        } else {
                            buffer_ln_idx--;
                        }
                    }
                }
                break;
            case 's': {
                    if (buffer_ln_idx >= ln_cnt - 1 || buffer_idx >= current_size) {
                    } else {
                        // move one line up
                        int prev_buff = buffer_idx;

                        for (int i = buffer_idx; i < current_size; i++) {
                            buffer_idx++;
                            if (input_buffer[i] == '\n' || buffer_idx > current_size) {
                                break;
                            }
                        }
                        if (buffer_idx > current_size) {
                            buffer_idx = prev_buff;
                        } else {
                            buffer_ln_idx++;
                        }
                    }
                }
                break;
            case 'W': {
                    buffer_idx = 0;
                    buffer_ln_idx = 0;
                }
                break;
            case 'S': {
                    buffer_idx = current_size;
                    buffer_ln_idx = ln_cnt - 1;
                }
                break;
            case '+':
                // write save
                fseek(f, 0, SEEK_SET);
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
                if (buffer_idx - 2 < 0 || current_size - 1 < 0) {
                } else {
                    if (input_buffer[buffer_idx - 1] == '\n') buffer_ln_idx--;

                    if (buffer_idx == current_size) {
                    } else {
                       memmove((void*) &input_buffer[buffer_idx - 1], (void*) &input_buffer[buffer_idx], (current_size - buffer_idx) * sizeof(char));
                    }
                    if (input_buffer[buffer_idx] == '\n') {
                        ln_cnt--;
                    } else {
                        char_cnt--;
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
                    buffer_ln_idx++;
                } else {
                    char_cnt++;
                }

                is_edited = true;
                input_buffer = (char*) realloc((void*) input_buffer, ++current_size);
                memmove((void*) &input_buffer[buffer_idx+1], (void*) &input_buffer[buffer_idx], (current_size - buffer_idx) * sizeof(char));
                //memcpy(input_buffer + buffer_idx + 1, input_buffer + buffer_idx, current_size - buffer_idx/* - 1*/);
                input_buffer[buffer_idx] = input;
                buffer_idx++;
            }
            break;
        }
    }

	return false;
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
    disable_print_char();
    
    struct point_t screen_size = get_screen_size();
    possible_lines_to_draw = (screen_size.y / CHAR_SIZE) - 4;

    // set keyboard input print to false
    // env_set(ENV_NO_PRINT_ON_INPUT, (void*) true);

    f = fopen(argv[1], "w");
    if (f->size != 0) {
        input_buffer = (char*) malloc(sizeof(char) * f->size);
    }
    current_size = f->size;
    buffer_idx = f->size;
    fread((void*) input_buffer, sizeof(char), f->size * sizeof(char), f);

    for (int i = 0; i < current_size; i++) {
        char_cnt++;
        if (input_buffer[i] == '\n') {
            ln_cnt++;
            buffer_ln_idx++;
            char_cnt--;
        }
    } 

    while (true) {
        render_status_bar();
        if (listen_input(f)) {
			break;
		}
    }

    fclose(f);
	set_color(0);
	clear_screen();
    set_color(old_color);
    enable_print_char();
}
