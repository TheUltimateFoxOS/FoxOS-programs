#include <commands.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/spawn.h>
#include <sys/env.h>
#include <sys/open.h>
#include <sys/close.h>
#include <errno.h>

#include <keyboard_helper.h>
#include <argv_tools.h>

char* search_executable(char* command) {
	char* path = getenv("PATH");

	if (path == NULL) {
		abort();
	}

	char* path_copy = malloc(strlen(path) + 1);
	memset(path_copy, 0, strlen(path) + 1);
	strcpy(path_copy, path);
	char* path_token = strtok(path_copy, ";");

	while (path_token != NULL) {
		char* executable = malloc(strlen(path_token) + strlen(command) + 2);
		memset(executable, 0, strlen(path_token) + strlen(command) + 2);
		strcpy(executable, path_token);
		strcat(executable, "/");
		strcat(executable, command);

		int fd;
		if ((fd = open(executable)) != -1) {
			close(fd);
			free(path_copy);
			return executable;
		}

		free(executable);

		char* executable2 = malloc(strlen(path_token) + strlen(command) + strlen(".elf") + 2);
		memset(executable2, 0, strlen(path_token) + strlen(command) + strlen(".elf") + 2);
		strcpy(executable2, path_token);
		strcat(executable2, "/");
		strcat(executable2, command);
		strcat(executable2, ".elf");

		if ((fd = open(executable2)) != -1) {
			close(fd);
			free(path_copy);
			return executable2;
		}

		free(executable2);
		path_token = strtok(NULL, ";");
	}

	free(path_copy);

	char* command_copy = malloc(strlen(command) + 1);
	memset(command_copy, 0, strlen(command) + 1);
	strcpy(command_copy, command);
	return command_copy;
}

void load_keymap(char* command) {
	if (command[11] == 0) {
		printf("No keymap specified!\n");
		return;
	} else {
		char* keymap_name = &command[11];
		printf("Loading keymap %s!\n", keymap_name);

		set_keymap(keymap_name);
	}
}

void keydbg(bool onoff) {
	if (onoff) {
		set_keyboard_debug(true);
		printf("Keyboard debugging enabled!\n");
	} else {
		set_keyboard_debug(false);
		printf("Keyboard debugging disabled!\n");
	}
}

void cd(char** argv) {
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}

	if (argc == 1) {
		char* env = getenv("ROOT_FS");
		if (env != NULL) {
			char path_buf[256];
			memset(path_buf, 0, 256);
			bool cancd = resolve(env, path_buf);

			if (cancd) {
				env_set(ENV_SET_CWD, path_buf);
			} else {
				printf("The specified root filesystem doesn't exist!\n");
			}
		} else {
			printf("No root filesystem specified!\n");
		}
	} else if (argc == 2) {
		char path_buf[256];
		memset(path_buf, 0, 256);
		bool cancd = resolve(argv[1], path_buf);

		if (cancd) {
			env_set(ENV_SET_CWD, path_buf);
		} else {
			printf("No such file or directory: %s\n", path_buf);
		}
	} else {
		printf("Too many arguments.");
	}
}

extern char** terminal_envp;

void export(char* command) {
	if (strlen(command) <= 7) {
		printf("No environment variable specified! Try like this: export MYVAR=value\n");
		return;
	}

	char* env_var = command + 7;

	char* env_name_tmp = malloc(strlen(env_var) + 1); //Make a copy of the string to operate on it
	memset(env_name_tmp, 0, strlen(env_var) + 1);
	strcpy(env_name_tmp, env_var);
	
	char* env_name = strtok(env_name_tmp, "=");
	if (strcmp(env_var, env_name) == 0) {
		printf("No environment variable value specified! Try like this: export MYVAR=value\n");
		free(env_name_tmp); //Make sure to free the memory allocated for strtok
		return;
	}
	size_t env_name_len = strlen(env_name);

	bool found = false;
	int next_empty_env_var = 0;
	for (int i = 0; terminal_envp[i] != NULL; i++) {
		if (strncmp(env_name, terminal_envp[i], env_name_len) == 0) {
			found = true;

			free(terminal_envp[i]); //Delete the old environment variable

			terminal_envp[i] = malloc(strlen(env_var) + 1); //Replace it with the new one
			memset(terminal_envp[i], 0, strlen(env_var) + 1);
			strcpy(terminal_envp[i], env_var);

			break;
		}
		next_empty_env_var = i + 1;
	}

	if (!found) { //We haven't found the environment variable, so we need to add it
		terminal_envp[next_empty_env_var] = malloc(strlen(env_var) + 1);
		memset(terminal_envp[next_empty_env_var], 0, strlen(env_var) + 1);
		strcpy(terminal_envp[next_empty_env_var], env_var);
		terminal_envp[next_empty_env_var + 1] = NULL;
	}

	free(env_name_tmp);
}

void pwd() {
	char* cwd = (char*) env(ENV_GET_CWD);
	printf("%s\n", cwd);
}


bool spawn_process(char** argv, char** terminal_envp) {
	char* executable = search_executable((char*) argv[0]);
	const char** envp = (const char**) terminal_envp; //Maybe use actual enviromental vars?

	errno = 0;
	task* t = spawn(executable, (char**) argv, envp, true);

	if (t == NULL) {
		return false;
	}

	bool task_exit = false;
	int task_exit_code = 0;
	t->on_exit = &task_exit;
	t->exit_code = &task_exit_code;

	while (!task_exit) {
		__asm__ __volatile__("pause" :: : "memory");
	}

	char export_cmd[64];
	sprintf(export_cmd, "export ?=%d", task_exit_code);
	export(export_cmd);

	free(executable);
	return true;
}