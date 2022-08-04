#include <commands.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/spawn.h>
#include <sys/env.h>
#include <sys/open.h>
#include <sys/close.h>
#include <sys/write.h>
#include <sys/read.h>
#include <errno.h>

#include <foxos/keyboard_helper.h>
#include <argv_tools.h>

char* term_stdout = NULL;
int term_stdout_size = 1;

char* term_stdin = NULL;
int term_stdin_size = 1;
int stdin_pos = 0;

void append_stdout(char* str, uint64_t size) {
	int old_size = term_stdout_size;
	term_stdout_size += size;

	term_stdout = realloc(term_stdout, term_stdout_size);
	memcpy(&term_stdout[old_size - 1], str, size);
	term_stdout[term_stdout_size - 1] = '\0';
}

void append_stdin(char* buffer, uint64_t size) {
	int can_copy = (term_stdin_size - 1) - stdin_pos;
	if (size > can_copy) {
		int left_over = size - can_copy;
		if (can_copy != 0) {
			memcpy(buffer, &term_stdin[stdin_pos], can_copy);
			stdin_pos += can_copy;
		}

		// env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0);

		// char input[left_over];
		// read(STDIN, input, left_over, 0);
		// memcpy(buffer + can_copy, input, left_over);

		// env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1);

		memset(buffer + can_copy, 0, left_over);
	} else {
		memcpy(buffer, &term_stdin[stdin_pos], size);
		stdin_pos += size;
	}
}

int term_printf(const char *fmt, ...) {
	char printf_buf[1024 * 4];
	va_list args;
	int printed;

	va_start(args, fmt);
	printed = vsprintf(printf_buf, fmt, args);
	va_end(args);

	if (term_stdout == NULL) {
		write(STDOUT, printf_buf, printed, 0);
	} else {
		append_stdout(printf_buf, printed);
	}

	return printed;
}

bool command_received(char* command, bool* should_break, char* stdin);
void system_command_handler(char* in) {
	bool should_break = false;
	command_received(in, &should_break, NULL);
}

bool run_command(char* command, char** terminal_envp, bool* should_break, char** stdin, char** stdout) {
	if (stdout != NULL) {
		term_stdout = *stdout;
		term_stdout_size = strlen(term_stdout) + 1;
	}

	if (stdin != NULL) {
		term_stdin = *stdin;
		term_stdin_size = strlen(term_stdin) + 1;
		stdin_pos = 0;
	}

	if (strncmp(command, (char*)"loadkeymap ", 11) == 0) {
		load_keymap(command);
	} else if (strcmp(command, (char*)"keydbg on") == 0) {
		keydbg(true);
	} else if (strcmp(command, (char*)"keydbg off") == 0) {
		keydbg(false);
	} else if (strncmp(command, (char*)"cd ", 3) == 0) {
		char** argv = argv_split(command);
		argv = argv_env_process(argv);

		cd(argv);

		free_argv(argv);
	} else if (strncmp(command, (char*)"pwd ", 4) == 0) {
		pwd();
	} else if (strncmp(command, (char*)"export ", 7) == 0) {
		char* argv_str = read_env(command);
		export(argv_str);
		free(argv_str);
	} else if (strncmp(command, (char*)"read ", 5) == 0) {
		read_(command);
	} else if (strcmp(command, (char*)"exit") == 0) {
		*should_break = true;
	} else {
		char** argv = argv_split(command);
		argv = argv_env_process(argv);

		pipe stdout_pipe = NULL;
		if (term_stdout != NULL) {
			stdout_pipe = append_stdout;
		}

		pipe stdin_pipe = NULL;
		if (term_stdin != NULL) {
			stdin_pipe = append_stdin;
		}

		task_t* task = spawn_process(argv, terminal_envp, stdout_pipe, stdin_pipe);
		if (task == NULL) {
			return false;
		}

		free_argv(argv);
	}

	if (stdout != NULL) {
		*stdout = term_stdout;
	}
	term_stdout = NULL;
	term_stdout_size = 1;

	if (stdin != NULL) {
		*stdin = term_stdin;
	}
	term_stdin = NULL;
	term_stdin_size = 1;
	stdin_pos = 0;

	return true;
}

char* search_executable(char* command) {
	char* path = getenv("PATH");
	char* cwd = (char*) env(ENV_GET_CWD);

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

	char* executable = malloc(strlen(cwd) + strlen(command) + 2);
	memset(executable, 0, strlen(cwd) + strlen(command) + 2);
	strcpy(executable, cwd);
	strcat(executable, "/");
	strcat(executable, command);

	int fd;
	if ((fd = open(executable)) != -1) {
		close(fd);
		return executable;
	}

	free(executable);

	char* command_copy = malloc(strlen(command) + 1);
	memset(command_copy, 0, strlen(command) + 1);
	strcpy(command_copy, command);
	return command_copy;
}

void load_keymap(char* command) {
	if (command[11] == 0) {
		term_printf("No keymap specified!\n");
		return;
	} else {
		char* keymap_name = &command[11];
		term_printf("Loading keymap %s!\n", keymap_name);

		foxos_set_keymap(keymap_name);
	}
}

void keydbg(bool onoff) {
	if (onoff) {
		foxos_set_keyboard_debug(true);
		term_printf("Keyboard debugging enabled!\n");
	} else {
		foxos_set_keyboard_debug(false);
		term_printf("Keyboard debugging disabled!\n");
	}
}

void cd(char** argv) {
	int argc = 0;
	while (argv[argc] != NULL) {
		argc++;
	}

	char path_buf[256];
	memset(path_buf, 0, 256);
	bool cancd = false;

	if (argc == 1) {
		char* env = getenv("ROOT_FS");
		if (env != NULL) {
			cancd = resolve(env, path_buf);
		} else {
			term_printf("No root filesystem specified!\n");
			return;
		}
	} else if (argc == 2) {
		cancd = resolve(argv[1], path_buf);
	} else {
		term_printf("Too many arguments.");
		return;
	}

	if (!cancd) {
		term_printf("No such file or directory: %s\n", path_buf);
		return;
	}

	int fd = open(path_buf);
	if (fd != -1) {
		term_printf("You can only change to a folder!\n");
		close(fd);
		return;
	}

	env_set(ENV_SET_CWD, path_buf);
}

extern char** terminal_envp;

void export(char* command) {
	if (strlen(command) <= 7) {
		term_printf("No environment variable specified! Try like this: export MYVAR=value\n");
		return;
	}

	char* env_var = command + 7;

	char* env_name_tmp = malloc(strlen(env_var) + 1); //Make a copy of the string to operate on it
	memset(env_name_tmp, 0, strlen(env_var) + 1);
	strcpy(env_name_tmp, env_var);
	
	char* env_name = strtok(env_name_tmp, "=");
	if (strcmp(env_var, env_name) == 0) {
		term_printf("No environment variable value specified! Try like this: export MYVAR=value\n");
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

void read_(char* command) {
	if (strlen(command) <= 5) {
		term_printf("No env var specified! Try like this: read output_var_name\n");
		return;
	}

	char* env_var = command + 5;
	char in[512] = { 0 };
	gets(in);

	char export_cmd[512] = { 0 };
	sprintf(export_cmd, "export %s=%s", env_var, in);
	export(export_cmd);
}

void pwd() {
	char* cwd = (char*) env(ENV_GET_CWD);
	term_printf("%s\n", cwd);
}

task_t* spawn_process(char** argv, char** terminal_envp, pipe stdout, pipe stdin) {
	char* executable = search_executable((char*) argv[0]);
	const char** envp = (const char**) terminal_envp; //Maybe use actual enviromental vars?

	errno = 0;
	task_t* task = spawn(executable, (const char**) argv, envp, true);

	if (task == NULL) {
		return NULL;
	}

	bool task_exit = false;
	int task_exit_code = 0;
	task->on_exit = &task_exit;
	task->exit_code = &task_exit_code;

	if (stdout != NULL) {
		task->stdout_pipe = stdout;
		task->pipe_enabled = true;
	}

	if (stdin != NULL) {
		task->stdin_pipe = stdin;
		task->pipe_enabled = true;
	}

	task->system_method = system_command_handler;

	while (!task_exit) {
		__asm__ __volatile__("pause" :: : "memory");
	}

	char export_cmd[64];
	sprintf(export_cmd, "export ?=%d", task_exit_code);
	export(export_cmd);

	free(executable);
	return task;
}