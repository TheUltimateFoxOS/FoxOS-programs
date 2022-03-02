#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/dir_at.h>
#include <sys/env.h>
#include <sys/open.h>
#include <sys/close.h>

int main(int argc, char *argv[]) {
	char ls_path[256];
	memset(ls_path, 0, sizeof(ls_path));

#ifndef USE_ENV_VAR_ROOT_FS
	strcpy(ls_path, (char*) env(ENV_GET_CWD));
#else
	strcpy(ls_path, getenv("ROOT_FS"));
#endif

	if (argc == 2) {
		bool canresolve = resolve_check(argv[1], ls_path, true);
		if (!canresolve) {
			printf("Error: No such file or directory: %s\n", argv[1]);
			return 1;
		}
	}

	int fd = open(ls_path);
	if (fd != -1) {
		printf("Error: You can't list a file\n", ls_path);
		close(fd);
		return 1;
	}

	dir_t dir = dir_at(0, ls_path);
	do {
		printf("%s (%s)\n", dir.name, dir.type == ENTRY_FILE ? "file" : "dir");
		dir = dir_at(dir.idx + 1, ls_path);
	} while (!dir.is_none);

	return 0;
}