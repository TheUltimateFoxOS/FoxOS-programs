#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/delete.h>
#include <sys/open.h>
#include <sys/env.h>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("Usage: %s <file>\n", argv[0]);
		return 1;
	}

	char* cwd = (char*) env(ENV_GET_CWD);

	char tmp[256];
	memset(tmp, 0, sizeof(tmp));

	strcpy(tmp, argv[1]);

	char* colon = strchr(tmp, ':');
	if (colon == NULL) {
		if (cwd[strlen(cwd) - 1] == '/') {
			cwd[strlen(cwd) - 1] = '\0';
		}

		if (tmp[0] == '/') {
			printf("Unsupported path: '%s'\n", tmp);
			return 1;
		}

		if (tmp[strlen(tmp) - 1] == '/') {
			tmp[strlen(tmp) - 1] = '\0';
		}

		strcat(cwd, "/");
		strcat(cwd, tmp);
	} else {
		memset(cwd, 0, sizeof(cwd));
		strcpy(cwd, tmp);
	}

	int fd = open(cwd);
	
	if (fd == -1) {
		printf("Error: Could not open file %s\n", cwd);
		return 1;
	}

	delete_(fd);

	return 0;
}