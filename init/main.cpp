#include <string.h>
#include <stdio.h>

#include <stdbool.h>
#include <sys/spawn.h>
#include <sys/env.h>
#include <foxos/keyboard_helper.h>
#include <stdlib.h>
#include <cfg.h>

int main(int argc, char* argv[], char* envp[]) {
	char argv_0[256];
	memset(argv_0, 0, 256);
	strcpy(argv_0, argv[0]);

	int num_toks = 0;
	char* tok = strtok(argv_0, (char*) "/");
	while (tok != NULL) {
		num_toks++;
		tok = strtok((char*) NULL, (char*) "/");
	}

	strcpy(argv_0, argv[0]);

	char path_buf[256];
	memset(path_buf, 0, 256);

	char* tok_2 = strtok(argv_0, (char*) "/");
	int i = 0;
	while (i < num_toks - 1) {
		strcat(path_buf, tok_2);
		strcat(path_buf, (char*) "/");
		tok_2 = strtok((char*) NULL, (char*) "/");
		i++;
	}

	// remove last slash
	path_buf[strlen(path_buf) - 1] = '\0';

	//printf("Got path: '%s'\n", path_buf);

	char path_for_envp[256];
	memset(path_for_envp, 0, 256);

	strcpy(path_for_envp, (char*) "PATH=");
	strcat(path_for_envp, path_buf);

	const char* argv_for_terminal[] = {
		(const char*) "terminal.elf",
		(const char*) NULL
	};

	char terminal_path[256];
	memset(terminal_path, 0, 256);

	strcpy(terminal_path, path_buf);
	strcat(terminal_path, (char*) "/terminal.elf");

	char root_fs[256];
	memset(root_fs, 0, 256);

	strcpy(argv_0, argv[0]);

	char* root_fs_path = strtok(argv_0, ":");

	strcpy(root_fs, (char*) "ROOT_FS=");
	strcat(root_fs, root_fs_path);
	strcat(root_fs, ":");

	char new_cwd[256];
	memset(new_cwd, 0, 256);

	strcpy(new_cwd, root_fs_path);
	strcat(new_cwd, ":/");
	env_set(ENV_SET_CWD, new_cwd);

	char config_file_path[256] = {0};
	bool canresolve = resolve((char*)(char*) "FOXCFG/init.cfg", config_file_path);
	if (canresolve) {
		FILE* config_file = fopen(config_file_path, "r");
		if (config_file != NULL) {
			// printf("Loading config file...\n");
			char* config_data = (char*) malloc(config_file->size + 1);
			memset(config_data, 0, config_file->size + 1);
			fread(config_data, config_file->size, 1, config_file);

			config_loader* config = new config_loader(config_data);

			char* keyboard_layout = config->get_key((char*) "keyboard_layout");
			if (keyboard_layout != NULL) {
				foxos_set_keymap(keyboard_layout);
			} else {
				printf("WARNING: key 'keyboard_layout' not found in config file.\n");
			}

			char* keyboard_debug = config->get_key((char*) "keyboard_debug");
			if (keyboard_debug != NULL) {
				bool debug = false;
				if (strcmp(keyboard_debug, "true") == 0) {
					debug = true;
				} else if (strcmp(keyboard_debug, "false") == 0) {
					debug = false;
				} else {
					printf("WARNING: value for key 'keyboard_debug' is not 'true' or 'false'. Defaulting to 'false'.\n");
				}
				foxos_set_keyboard_debug(debug);
			} else {
				printf("WARNING: key 'keyboard_debug' not found in config file.\n");
			}

			delete config;
			free(config_data);
			fclose(config_file);

		} else {
			printf("WARNING: Could not open config (init.cfg) file.\n");
		}
	} else {
		printf("WARNING: Could not resolve config (init.cfg) file.\n");
	}

	const char* envp_for_terminal[] = {
		path_for_envp,
		root_fs,
		(const char*) NULL
	};

	char auto_exec_path[256] = { 0 };
	canresolve = resolve((char*) "FOXCFG/start.fox", auto_exec_path);
	if (canresolve) {
		// printf("Executing auto-exec file...\n");
		char* argv_for_auto_exec[] = {
			(char*) "terminal.elf",
			auto_exec_path,
			(char*) NULL
		};

		task_t* autoexec_task = spawn(terminal_path, (const char**) argv_for_auto_exec, envp_for_terminal, true);

		bool autoexec_task_exit = false;
		autoexec_task->on_exit = &autoexec_task_exit;

		while (!autoexec_task_exit) {
			__asm__ __volatile__("pause");
		}

		// printf("Auto-exec task exited.\n");

	} else {
		printf("WARNING: Could not resolve auto-exec (start.fox) file.\n");
	}

	//printf("CWD: %s\n", env(ENV_GET_CWD));
	task_t* terminal_task;

respawn:
	terminal_task = spawn(terminal_path, (const char**) argv_for_terminal, envp_for_terminal, true);

	if (terminal_task == NULL) {
		printf("Could not spawn terminal with path: %s\n", terminal_path);
		return -1;
	}

	bool task_exit = false;
	int exit_code = 0;
	terminal_task->on_exit = &task_exit;
	terminal_task->exit_code = &exit_code;

	while (!task_exit) {
		__asm__ __volatile__("hlt");
	}

	printf("Terminal exited (code %d). Respawning...\n", exit_code);
	goto respawn;

	return 0;
}
