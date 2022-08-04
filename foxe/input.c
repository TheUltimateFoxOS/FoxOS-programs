#include <foxe.h>
#include <sys/env.h>
#include <stdint.h>
#include <render.h>
#include <foxos/term.h>

void move_up() {
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

void move_down() {
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

void move_left() {
	if (!buffer_idx <= 0) {
		if (input_buffer[buffer_idx - 1] == '\n') buffer_ln_idx--;
		buffer_idx -= 1;
	}
}

void move_right() {
	if (buffer_idx < current_size) {
		if (input_buffer[buffer_idx] == '\n') buffer_ln_idx++;
		buffer_idx += 1;
	}
}

void arrow_keys_sighandler(uint8_t signum) {
	special_keys_down_t* keys = (special_keys_down_t*) env(ENV_GET_SPECIAL_KEYS);

	if (keys->up_arrow) {
		move_up();
	} else if (keys->down_arrow) {
		move_down();
	} else if (keys->left_arrow) {
		move_left();
	} else if (keys->right_arrow) {
		move_right();
	}

	render_tui();
}

bool listen_input() {
	char input = getchar();

	if (!is_in_insert_mode) {
		switch (input) {
			case 'q': {
					return true;
				}
			case '\e': {
					is_in_insert_mode = !is_in_insert_mode;
					mode = "INSERT";
				}
				break;

			case 'a': {
					// move left one char
					move_left();
				}
				break;
			case 'd': {
					// move right one char
					move_right();
				}
				break;
			case 'w': {
					// move up one line
					move_up();
				}
				break;
			case 's': {
					// move down one line
					move_down();
				}
				break;

			case 'A': {
					for (;buffer_idx > 0; buffer_idx--) {
						if (input_buffer[buffer_idx - 1] == '\n') {
							break;
						}
					}
				}
				break;
			case 'D': {
					for (; buffer_idx < current_size; buffer_idx++) {
						if (input_buffer[buffer_idx + 1] == '\n') {
							break;
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

			case 'r': {
				// r + char -> replace char
				char c = getchar();
				if (c < 32 || c > 126 && c != 10) break; // if not a printable character
				input_buffer[buffer_idx] = c;
				}
				break;

			case 'v': {
					if (copy_buffer_is_set) {
						input_buffer = (char*) realloc((void*) input_buffer, (current_size + copy_buffer_length) * sizeof(char));
						current_size += copy_buffer_length;
						memmove((void*) &input_buffer[buffer_idx + copy_buffer_length], (void*) &input_buffer[buffer_idx + 1], (current_size - copy_buffer_length - buffer_idx)* sizeof(char));

						for (int i = 0; i < copy_buffer_length; i++) {
							input_buffer[buffer_idx] = copy_buffer[i];
							buffer_idx++;
						}

						free(copy_buffer);
						copy_buffer_length = 0;
						copy_buffer_is_set = false;
					}
				}
				break;

			case 'C': {
					if (copy_buffer_is_set) break;
					// move to beginning of the line
					for (;buffer_idx > 0; buffer_idx--) {
						if (input_buffer[buffer_idx - 1] == '\n') {
							break;
						}
					}

					for (; buffer_idx < current_size; buffer_idx++) {
						copy_buffer_length++;
						
						if (copy_buffer_length == 1) {
							copy_buffer = (char*) malloc(copy_buffer_length * sizeof(char));
						} else {
							copy_buffer = (char*) realloc((void*) copy_buffer, copy_buffer_length * sizeof(char));
						}
						copy_buffer[copy_buffer_length - 1] = input_buffer[buffer_idx]; // maybe do this with memcpy later

						if (input_buffer[buffer_idx + 1] == '\n') {
							break;
						}
					}
					copy_buffer_is_set = true;
				}
				break;
			case 'X': {
					if (copy_buffer_is_set) break;
					// move to beginning of the line
					for (;buffer_idx > 0; buffer_idx--) {
						if (input_buffer[buffer_idx - 1] == '\n') {
							break;
						}
					}

					for (; buffer_idx < current_size; buffer_idx++) {
						copy_buffer_length++;
						
						if (copy_buffer_length == 1) {
							copy_buffer = (char*) malloc(copy_buffer_length * sizeof(char));
						} else {
							copy_buffer = (char*) realloc((void*) copy_buffer, copy_buffer_length * sizeof(char));
						}
						copy_buffer[copy_buffer_length - 1] = input_buffer[buffer_idx]; // maybe do this with memcpy later
						if (input_buffer[buffer_idx + 1] == '\n') {
							break;
						}
					}
					copy_buffer_is_set = true;
					memmove((void*) &input_buffer[buffer_idx - copy_buffer_length], (void*) &input_buffer[buffer_idx], (current_size - buffer_idx) * sizeof(char));
					current_size -= copy_buffer_length;
					buffer_idx -= copy_buffer_length;
				}
				break;

			case '+': {
					// write save
					fseek(f, 0, SEEK_SET);
					fwrite(input_buffer, sizeof(char), current_size * sizeof(char), f);
					is_edited = false;
				}
				break;
			case '-': {
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
						fseek(f, 0, SEEK_SET);
						fread((void*) input_buffer, sizeof(char), f->size * sizeof(char), f);
						is_edited = false;
						render_tui();
					}
				}
				break;
			default: {
					break;
				}
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