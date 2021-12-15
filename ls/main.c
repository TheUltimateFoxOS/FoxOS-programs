#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/dir_at.h>
#include <sys/env.h>

int main(int argc, char *argv[]) {
	char ls_path[256];
	memset(ls_path, 0, sizeof(ls_path));

#ifndef USE_ENV_VAR_ROOT_FS
	strcpy(ls_path, (char*) env(ENV_GET_CWD));
#else
	strcpy(ls_path, getenv("ROOT_FS"));
#endif

	if (argc == 2) {
		char tmp[256];
		memset(tmp, 0, sizeof(tmp));

		strcpy(tmp, argv[1]);

		char* colon = strchr(tmp, ':');
		if (colon == NULL) {
			if (ls_path[strlen(ls_path) - 1] == '/') {
				ls_path[strlen(ls_path) - 1] = '\0';
			}

			if (tmp[0] == '/') {
				printf("Unsupported path: '%s'\n", tmp);
				return 1;
			}

			if (tmp[strlen(tmp) - 1] == '/') {
				tmp[strlen(tmp) - 1] = '\0';
			}

			strcat(ls_path, "/");
			strcat(ls_path, tmp);
		} else {
			memset(ls_path, 0, sizeof(ls_path));
			strcpy(ls_path, tmp);
		}
	}

	dir_t dir = dir_at(0, ls_path);
	while (!dir.is_none) {
		printf("%s\n", dir.name);
		dir = dir_at(dir.idx + 1, ls_path);
	}

	return 0;
}