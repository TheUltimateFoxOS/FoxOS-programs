#include <stdio.h>
#include <stdlib.h>

#include <sys/dir_at.h>

int main(int argc, char *argv[]) {
	char* ls_path = getenv("ROOT_FS");
	if (argc == 2) {
		ls_path = argv[1];
	}

	if (ls_path == NULL) {
		printf("ls: Could not find environment variable ROOT_FS please specify a path\n");
		return 1;
	}

	dir_t dir = dir_at(0, ls_path);
	while (!dir.is_none) {
		printf("%s\n", dir.name);
		dir = dir_at(dir.idx + 1, ls_path);
	}

	return 0;
}