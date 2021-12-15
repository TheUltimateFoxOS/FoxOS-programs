#include <string.h>
#include <stdio.h>

#include <stdbool.h>
#include <sys/spawn.h>
#include <sys/env.h>

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

	const char* envp_for_terminal[] = {
		path_for_envp,
		root_fs,
		NULL
	};

	//printf("CWD: %s\n", env(ENV_GET_CWD));

	task* terminal_task = spawn(terminal_path, argv_for_terminal, envp_for_terminal, true);

	if (terminal_task == NULL) {
		printf("Could not spawn terminal with path: %s\n", terminal_path);
		return -1;
	}

	while (true) {
		__asm__ __volatile__("hlt");
	}

	return 0;
}
