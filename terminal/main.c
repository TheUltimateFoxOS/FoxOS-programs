#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sys/spawn.h>
#include <sys/env.h>
#include <errno.h>

#include <keyboard_helper.h>

#define max_buffer_size 2048

char** terminal_envp;

char** argv_split(char* str) {
	int len = strlen(str);

	int argc = 1;
	bool quote_open = false;
	bool double_quote_open = false;
	bool special_char_next = false;

	for (int i = 0; i < len; i++) {
		if(str[i] == ' ') {
			if (!quote_open && !double_quote_open) {
				argc++;
			}
		} else if (str[i] == '\\') {
			special_char_next = true;
		} else if (str[i] == '\"') {
			if (special_char_next || double_quote_open) {
				special_char_next = false;
			} else {
				quote_open = !quote_open;
			}
		} else if (str[i] == '\'') {
			if (special_char_next || quote_open) {
				special_char_next = false;
			} else {
				double_quote_open = !double_quote_open;
			}
		}
	}

	char** argv = malloc(sizeof(char*) * (argc + 1));

	argc = 1;
	argv[0] = &str[0];

	quote_open = false;
	double_quote_open = false;
	special_char_next = false;

	for (int i = 0; i < len; i++) {
		if(str[i] == ' ') {
			if (!quote_open && !double_quote_open) {
				argv[argc] = &str[i + 1];
				str[i] = 0;
				argc++;
			}
		} else if (str[i] == '\\') {
			special_char_next = true;
		} else if (str[i] == '\"') {
			if (special_char_next || double_quote_open) {
				special_char_next = false;
			} else {
				quote_open = !quote_open;
			}
		} else if (str[i] == '\'') {
			if (special_char_next || quote_open) {
				special_char_next = false;
			} else {
				double_quote_open = !double_quote_open;
			}
		}
	}

	argv[argc] = NULL;

	return argv;
}

void command_received(char* command) {
	if (strcmp(command, (char*)"test") == 0) {
		printf("Yay this terminal works!");
	} else if (strncmp(command, (char*)"loadkeymap", 10) == 0) {
		if (command[11] == 0) {
			printf("No keymap specified!");
			return;
		} else {
			char* keymap_name = &command[11];
			printf("Loading keymap %s!\n", keymap_name);

			if (strcmp(keymap_name, (char*)"de") == 0) {
				set_keymap(0);
			} else if (strcmp(keymap_name, (char*)"us") == 0) {
				set_keymap(1);
			} else if (strcmp(keymap_name, (char*)"fr") == 0) {
				set_keymap(2);
			} else {
				printf("Keymap %s not found!\n", keymap_name);
			}

		}
	} else if (strcmp(command, (char*)"keydbg on") == 0) {
		set_keyboard_debug(true);
		printf("Keyboard debugging enabled!");
	} else if (strcmp(command, (char*)"keydbg off") == 0) {
		set_keyboard_debug(false);
		printf("Keyboard debugging disabled!");
	} else {
		const char** argv = (const char**) argv_split(command);
		const char** envp = (const char**) terminal_envp; //Maybe use actual enviromental vars?

		errno = 0;
		task* t = spawn(argv[0], argv, envp);

		if (t == NULL) {
			goto error;
		}

		bool task_exit = false;
		t->on_exit = &task_exit;

		while (!task_exit) {
			__asm__ __volatile__("pause" :: : "memory");
		}

		goto _exit;

	error:
		free(argv);
		printf("Error: command not found: %s", command);

	_exit:
		return;
	}
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

	printf("FoxOS > ");

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
				printf("FoxOS > ");
			} else if (is_quote_open(buffer)) {
				printf(" quote> ");
			} else {
				command_received(buffer); //This should block while command is running.

				memset(buffer, 0, sizeof(char) * max_buffer_size);
				buffer_len = 0;
				printf("\nFoxOS > ");
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
