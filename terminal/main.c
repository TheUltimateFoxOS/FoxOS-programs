#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sys/spawn.h>
#include <sys/env.h>
#include <sys/open.h>
#include <sys/close.h>
#include <sys/touch.h>
#include <errno.h>

#include <keyboard_helper.h>
#include <argv_tools.h>
#include <commands.h>

#include <script.h>

#define max_buffer_size 2048

char** terminal_envp;

bool command_received(char* command, bool* should_break, char* stdin) {
	int token_pos;
	bool double_pipe_symbol;
	int cmd_type = get_command_type(command, &token_pos, &double_pipe_symbol);

	printf("%d\n", cmd_type);

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
		run_script(argv[1], max_buffer_size);
		return 0;
	} else if (argc != 1) {
		printf("Usage: terminal [script?]\n");
		return 1;
	}

	printf("\nFoxOS %s > ", env(ENV_GET_CWD));

	char* buffer = (char*) calloc(max_buffer_size, sizeof(char));
	int buffer_len = 0;

	while (true) {
		char input = getchar();

		if (input == '\b') {
			buffer[buffer_len] = 0;
			buffer_len--;
			if (buffer_len == -1) {
				printf(" ");
				buffer_len = 0;
			}
		} else if (input == '\n') {
			if (buffer_len == 0) {
				printf("FoxOS %s > ", env(ENV_GET_CWD));
			} else if (is_quote_open(buffer)) {
				printf(" quote> ");
			} else {
				bool should_break;
				command_received(buffer, &should_break, NULL); //This should block while command is running.
				if (should_break) {
					break;
				}

				memset(buffer, 0, sizeof(char) * max_buffer_size);
				buffer_len = 0;
				printf("\nFoxOS %s > ", env(ENV_GET_CWD));
			}
		} else if (buffer_len == max_buffer_size) {
			printf("\b");
		} else {
			buffer[buffer_len] = input;
			buffer_len++;
		}
	}

	printf("\n[TERMINAL PROCESS ENDED]\n");
	free(buffer);
	return 0;
}
