#include <string.h>
#include <stdio.h>

#include <stdbool.h>
#include <sys/spawn.h>
#include <sys/env.h>
#include <keyboard_helper.h>
#include <stdlib.h>

#include <jsmn.h>

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start && strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int main(int argc, char* argv[], char* envp[]) {
	char argv_0[256];
	memset(argv_0, 0, 256);
	strcpy(argv_0, argv[0]);

	int num_toks = 0;
	char* tok = strtok(argv_0, "/");
	while (tok != NULL) {
		num_toks++;
		tok = strtok(NULL, "/");
	}

	strcpy(argv_0, argv[0]);

	char path_buf[256];
	memset(path_buf, 0, 256);

	char* tok_2 = strtok(argv_0, "/");
	int i = 0;
	while (i < num_toks - 1) {
		strcat(path_buf, tok_2);
		strcat(path_buf, "/");
		tok_2 = strtok(NULL, "/");
		i++;
	}

	// remove last slash
	path_buf[strlen(path_buf) - 1] = '\0';

	//printf("Got path: '%s'\n", path_buf);

	char path_for_envp[256];
	memset(path_for_envp, 0, 256);

	strcpy(path_for_envp, "PATH=");
	strcat(path_for_envp, path_buf);

	const char* argv_for_terminal[] = {
		"terminal.elf",
		NULL
	};

	char terminal_path[256];
	memset(terminal_path, 0, 256);

	strcpy(terminal_path, path_buf);
	strcat(terminal_path, "/terminal.elf");

	char root_fs[256];
	memset(root_fs, 0, 256);

	strcpy(argv_0, argv[0]);

	char* root_fs_path = strtok(argv_0, ":");

	strcpy(root_fs, "ROOT_FS=");
	strcat(root_fs, root_fs_path);
	strcat(root_fs, ":");

	char new_cwd[256];
	memset(new_cwd, 0, 256);

	strcpy(new_cwd, root_fs_path);
	strcat(new_cwd, ":/");
	env_set(ENV_SET_CWD, new_cwd);

	char config_file_path[256] = {0};
	bool canresolve = resolve("FOXCFG/cfg.fox", config_file_path);
	if (canresolve) {
		FILE* config_file = fopen(config_file_path, "r");
		if (config_file != NULL) {
			// printf("Loading config file...\n");
			char* config_data = malloc(config_file->size + 1);
			memset(config_data, 0, config_file->size + 1);
			fread(config_data, config_file->size, 1, config_file);

			jsmn_parser p;
			jsmntok_t t[128]; // We expect no more than 128 JSON tokens

			jsmn_init(&p);
			int r = jsmn_parse(&p, config_data, strlen(config_data), t, sizeof(t) / sizeof(t[0]));
			if (r < 0) {
				printf("Failed to parse JSON: %d\n", r);
				return 1;
			}

			for (int i = 0; i < r; i++) {
				if (jsoneq(config_data, &t[i], "keyboard_layout") == 0) {
					char keyboard_layout[8] = { 0 };
					strncpy(keyboard_layout, config_data + t[i + 1].start, t[i + 1].end - t[i + 1].start);

					set_keymap(keyboard_layout);
				} else if (jsoneq(config_data, &t[i], "keyboard_debug") == 0) {
					bool keyboard_debug;
					if (strncmp(config_data + t[i + 1].start, "true", 4) == 0) {
						keyboard_debug = true;
					} else {
						keyboard_debug = false;
					}
					// printf("Got keyboard_debug: %s\n", keyboard_debug ? "true" : "false");
					set_keyboard_debug(keyboard_debug);
				}
			}

		} else {
			printf("WARNING: Could not open config (cfg.fox) file.\n");
		}
	} else {
		printf("WARNING: Could not resolve config (cfg.fox) file.\n");
	}

	const char* envp_for_terminal[] = {
		path_for_envp,
		root_fs,
		NULL
	};

	char auto_exec_path[256] = { 0 };
	canresolve = resolve("FOXCFG/start.fox", auto_exec_path);
	if (canresolve) {
		// printf("Executing auto-exec file...\n");
		char* argv_for_auto_exec[] = {
			"terminal.elf",
			auto_exec_path,
			NULL
		};

		task_t* autoexec_task = spawn(terminal_path, (char**) argv_for_auto_exec, envp_for_terminal, true);

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
	terminal_task = spawn(terminal_path, (char**) argv_for_terminal, envp_for_terminal, true);

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
