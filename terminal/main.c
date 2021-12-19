#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sys/spawn.h>
#include <sys/env.h>
#include <sys/open.h>
#include <sys/close.h>
#include <errno.h>

#include <keyboard_helper.h>
#include <argv_tools.h>
#include <commands.h>

#define max_buffer_size 2048

char** terminal_envp;

bool command_received(char* command) {
	if (strncmp(command, (char*)"loadkeymap", 10) == 0) {
		load_keymap(command);
	} else if (strcmp(command, (char*)"keydbg on") == 0) {
		keydbg(true);
	} else if (strcmp(command, (char*)"keydbg off") == 0) {
		keydbg(false);
	} else if (strncmp(command, (char*)"cd", 2) == 0) {
		cd(command);
	} else if (strncmp(command, (char*)"pwd", 3) == 0) {
		pwd();
	} else if (strncmp(command, (char*)"exit", 4) == 0) {
		return true;
	} else {
		spawn_process(command, terminal_envp);
	}
	return false;
}

bool is_quote_open(char* command) {
	bool quote_open = false;
	bool double_quote_open = false;
	bool special_char_next = false;

	int len = strlen(command);

	for (int i = 0; i < len; i++) {
		if (command[i] == '\\') {
			special_char_next = true;
		} else if (command[i] == '\"') {
			if (special_char_next || double_quote_open) {
				special_char_next = false;
			} else {
				quote_open = !quote_open;
			}
		} else if (command[i] == '\'') {
			if (special_char_next || quote_open) {
				special_char_next = false;
			} else {
				double_quote_open = !double_quote_open;
			}
		}
	}

	return double_quote_open || quote_open;
}

int main(int argc, char* argv[], char* envp[]) {
	printf("\nTerminal initialising...\n");

	//env_set3(ENV_KEYBOARD_DEBUG, 1);
	//env_set3(ENV_KEYMAP, keymap_de_e);

	terminal_envp = envp;

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
				printf("\nFoxOS %s > ", env(ENV_GET_CWD));
			} else if (is_quote_open(buffer)) {
				printf(" quote> ");
			} else {
				bool should_break = command_received(buffer); //This should block while command is running.
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
