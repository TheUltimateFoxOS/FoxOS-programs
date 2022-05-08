#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sys/spawn.h>
#include <sys/env.h>
#include <sys/open.h>
#include <sys/close.h>
#include <sys/touch.h>
#include <sys/dir_at.h>
#include <errno.h>

#include <foxos/term.h>

#include <keyboard_helper.h>
#include <argv_tools.h>
#include <commands.h>
#include <script.h>

#define MAX_BUFFER_SIZE 2048

char** terminal_envp;

bool command_received(char* command, bool* should_break, char* stdin) {
	*should_break = false;

	int token_pos = 0;
	bool double_pipe_symbol = false;
	int cmd_type = get_command_type(command, &token_pos, &double_pipe_symbol);

	//printf("%d\n", cmd_type);

	char** command_stdin = NULL;
	if (stdin != NULL) {
		command_stdin = &stdin;
	}

	if (cmd_type == NORMAL) { //Normal command, just run it and send the stdin
		int found_command = run_command(command, terminal_envp, should_break, command_stdin, NULL);
		if (!found_command) {
			printf("Error: Command not found: '%s'\n", command);
		}
		return found_command;
	} else if (cmd_type == AND_RUN || cmd_type == PIPE_PROC || cmd_type == PIPE_FILE || cmd_type == PIPE_FILE_APPEND) {
		char* current_command = process_line(command, false); //This is the first command that needs to be run
		current_command[token_pos - (double_pipe_symbol ? 1 : 0)] = '\0';
		char* next_command = process_line(&current_command[token_pos + 1], false); //This could be a file name, or another command

		char* stdout = NULL; //Set to NULL by default, so that stdout isn't send if it isn't needed
		if (cmd_type == PIPE_PROC || cmd_type == PIPE_FILE || cmd_type == PIPE_FILE_APPEND) { //If the command is a pipe, we need to set the stdout to a buffer
			stdout = malloc(1);
			memset(stdout, 0, 1);
		}

		bool found_command = run_command(current_command, terminal_envp, should_break, command_stdin, &stdout); //Run the first command with the stdin, and stdout if there is one
		if (!found_command) {
			printf("Error: command not found: %s\n", current_command);
			return false;
		}

		bool call_next_command = true;
		if (cmd_type == PIPE_FILE || cmd_type == PIPE_FILE_APPEND) { //If the command is a pipe to a file, we need to open the file and write the stdout to it
			int old_cmd_type = cmd_type;
			current_command = next_command; //There may be another command after this one, so we will need to run it if so
			cmd_type = get_command_type(current_command, &token_pos, &double_pipe_symbol);
			if (cmd_type == NORMAL) {
				call_next_command = false;
			} else {
				current_command[token_pos - (double_pipe_symbol ? 1 : 0)] = '\0';
				call_next_command = true;
				next_command = process_line(&current_command[token_pos + 1], false);
			}

			char file_to_create[256];
			memset(file_to_create, 0, 256);
			bool canresolve = resolve_check(current_command, file_to_create, false);
			if (!canresolve) {
				printf("Error: No such file or directory to create output file in: %s\n", current_command);
				return false;
			}

			FILE* file = fopen(file_to_create, "w");
			if (file == NULL) {
				printf("Error: Failed to open file: %s\n", file_to_create);
				return false;
			}

			if (old_cmd_type == PIPE_FILE_APPEND) {
				fseek(file, 0, SEEK_END);
			}

			fwrite(stdout, strlen(stdout), 1, file);
			fclose(file);
		}

		if (call_next_command) { //If the command is a pipe to another command (or there is another command after the file), we need to run the next command with the stdout as the stdin
			found_command = command_received(next_command, should_break, stdout);
		}

		if (stdout != NULL) { //Make sure to free the stdout if it was allocated
			free(stdout);
		}

		return found_command;
	} else {
		printf("Error: Got an unknown command type, this shouldn't happen. %s\n", command);
	}
}

void on_shutdown() {
	for (int i = 0; terminal_envp[i] != NULL; i++) {
		free(terminal_envp[i]);
	}

	free(terminal_envp);

	enable_print_char();
}

void tab_complete(char* command, char* extra, int extra_size) {
	struct dir_node_t {
		char name[MAX_DIR_NAME_LENGTH + 1];
		void* next;
	};

	struct dir_node_t* root_node = malloc(sizeof(struct dir_node_t)); //Create the root node to start the list
	memset(root_node, 0, sizeof(struct dir_node_t));
	root_node->name[0] = 0;
	root_node->next = NULL;

	int check_dir_count = 2;
	char check_dirs[check_dir_count][MAX_DIR_NAME_LENGTH + 1];
	memset(check_dirs, 0, check_dir_count * (MAX_DIR_NAME_LENGTH + 1));

	char* cwd = (char*) env(ENV_GET_CWD);
	strcpy(check_dirs[0], cwd); //Check the current working directory

	char* path = getenv("PATH");
	strcpy(check_dirs[1], path); //Check the PATH

	#warning Check for / in command, so that we can check the subdirectories

	int did_complete = -1;

	for (int check_dir_i = 0; check_dir_i < check_dir_count; check_dir_i++) {
		char* check_dir = check_dirs[check_dir_i];

		dir_t dir = dir_at(0, check_dir);
		struct dir_node_t* current_node = root_node;

		while (!dir.is_none) {
			if (strncmp(command, dir.name, strlen(command)) == 0) { //See if the directory name starts with the command
				struct dir_node_t* data = malloc(sizeof(struct dir_node_t)); //Allocate memory for the new node
				memset(data, 0, sizeof(struct dir_node_t));

				strcpy(data->name, dir.name);
				if (dir.type == ENTRY_DIR) {
					strcat(data->name, "/");
				}
				data->next = NULL;

				current_node->next = data; //Set the next node to the new node
				current_node = data; //Move to the new node
			}

			dir = dir_at(dir.idx + 1, check_dir);
		}

		if (root_node->next != NULL) { //Check if there is actually anything to try
			bool exit = false;
			int start_pos = strlen(command);
			int current_pos = start_pos;

			while (current_pos < MAX_DIR_NAME_LENGTH) {
				current_node = root_node->next;
				char current_char = current_node->name[current_pos]; //Set the current character to check

				while (current_node->next != NULL) { //Make sure that there is a next node to check
					current_node = current_node->next;
					char check_char = current_node->name[current_pos]; //Set the character to check

					if (check_char != current_char) { //If the current character doesn't match on one of the subsequent nodes, we can exit
						exit = true;
						break;
					}
				}

				if (exit || current_pos >= extra_size) { //Make sure that the command buffer has space to accomodate the new character
					break;
				} else {
					extra[current_pos - start_pos] = current_char;
					current_pos++;
					did_complete = check_dir_i;
				}
			}
		}

		current_node = root_node->next; //Get the first node
		while (current_node != NULL) {
			struct dir_node_t* next_node = current_node->next;
			free(current_node);
			current_node = next_node;
		}

		if (did_complete != -1) {
			break;
		}
	}

	if (did_complete == 1) {
		int extra_len = strlen(extra);
		if (extra[extra_len - 4] == '.' && extra[extra_len - 3] == 'e' && extra[extra_len - 2] == 'l' && extra[extra_len - 1] == 'f') { //Remove the .elf of files from /BIN
			extra[extra_len - 4] = 0;
			extra[extra_len - 3] = 0;
			extra[extra_len - 2] = 0;
			extra[extra_len - 1] = 0;

			// add space at the end of the command
			extra[extra_len - 4] = ' ';
		}
	}

	free(root_node); //Free the root node
}

int main(int argc, char* argv[], char* envp[]) {
	// printf("\nTerminal initialising...\n");

	//env_set3(ENV_KEYBOARD_DEBUG, 1);
	//env_set3(ENV_KEYMAP, keymap_de_e);

	terminal_envp = (char**) malloc(sizeof(char*) * 0xff);
	for (int i = 0; envp[i] != NULL; i++) {
		terminal_envp[i] = malloc(strlen(envp[i]) + 1);
		memset(terminal_envp[i], 0, strlen(envp[i]) + 1);
		memcpy(terminal_envp[i], envp[i], strlen(envp[i]));
		terminal_envp[i + 1] = NULL;
	}
	env_set(ENV_ENVP_SET, terminal_envp);

	__libc_set_shutdown_hook(on_shutdown);

	if (argc == 2) {
		run_script(argv[1]);
		return 0;
	} else if (argc != 1) {
		printf("Usage: terminal [script?]\n");
		return 1;
	}

	disable_print_char();

	printf("\nFoxOS %s > ", env(ENV_GET_CWD));

	char buffer[MAX_BUFFER_SIZE + 1];
	memset(buffer, 0, MAX_BUFFER_SIZE + 1);
	int buffer_len = 0;

	while (true) {
		char input = getchar();
		if (input >= 0x20 && input <= 0x7E || input == '\n'|| input == '\b') {
			printf("%c", input);
		}

		if (input == '\b') {
			buffer_len--;
			if (buffer_len == -1) {
				printf(" ");
				buffer_len = 0;
				buffer[buffer_len] = 0;
			} else {
				buffer[buffer_len] = 0;
			}
		} else if (input == '\t') {
			if (buffer_len == 0) { //Don't tab complete empty commands
				continue;
			}
			if (is_quote_open(buffer)) { //Don't tab complete if there is a quote open
				continue;
			}

			char buffer2[MAX_BUFFER_SIZE + 1]; //Create a clone of the buffer to do operations on
			memset(buffer2, 0, MAX_BUFFER_SIZE + 1);
			strcpy(buffer2, buffer);

			char* current_command = process_line(buffer2, false); //Remove trailing spaces and get the current command

			char* tmp = strtok(current_command, " ");
			while (tmp != NULL) {
				current_command = tmp;
				tmp = strtok(NULL, " ");
			}

			int extra_size = (MAX_BUFFER_SIZE - buffer_len); //Must be only enough to fill the rest of the buffer, not more
			char extra[extra_size + 1]; //The tab complete buffer
			memset(extra, 0, extra_size + 1);
			tab_complete(current_command, extra, extra_size);

			if (extra[0] != 0) {
				for (int i = 0; i < strlen(extra); i++) { //Append the data to the screen and the buffer
					buffer[buffer_len] = extra[i];
					printf("%c", extra[i]);
					buffer_len++;
				}
			}
		} else if (input == '\n') {
			if (buffer_len == 0) {
				printf("FoxOS %s > ", env(ENV_GET_CWD));
			} else if (is_quote_open(buffer)) {
				printf(" quote> ");
			} else {
				enable_print_char();
				bool should_break;
				command_received(buffer, &should_break, NULL); //This should block while command is running.
				disable_print_char();
				if (should_break) {
					break;
				}

				memset(buffer, 0, MAX_BUFFER_SIZE + 1);
				buffer_len = 0;
				printf("\nFoxOS %s > ", env(ENV_GET_CWD));
			}
		} else if (buffer_len >= MAX_BUFFER_SIZE) {
			printf("\b");
		} else if (input == 0) {
			break;
		} else if (input == 27 /* esc */) {
			memset(buffer, 0, MAX_BUFFER_SIZE + 1);
			buffer_len = 0;
			printf("\nFoxOS %s > ", env(ENV_GET_CWD));
		} else {
			buffer[buffer_len] = input;
			buffer_len++;
		}
	}

	printf("\n[TERMINAL PROCESS ENDED]\n");
	return 0;
}
