#include <sys/write.h>
#include <sys/read.h>
#include <sys/open.h>
#include <sys/close.h>
#include <sys/get_file_size.h>
#include <sys/env.h>

#include <string.h>

#include <stdio.h>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("Usage: %s <filename>\n", argv[0]);
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

	size_t size = get_file_size(fd);

	char buffer[size];
	memset(buffer, 0, size);

	read(fd, buffer, size, 0);
	write(STDOUT, buffer, size, 0);

	close(fd);

	write(STDOUT, "\n", 1, 0);

	return 0;
}