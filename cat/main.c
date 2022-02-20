#include <sys/write.h>
#include <sys/read.h>
#include <sys/open.h>
#include <sys/close.h>
#include <sys/get_file_size.h>
#include <sys/env.h>

#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	char file_to_open[256];
	memset(file_to_open, 0, 256);
	bool canresolve = resolve(argv[1], file_to_open);

	if (!canresolve) {
		printf("No such file or directory: %s", argv[1]);
		return 1;
	}

	int fd = open(file_to_open);

	if (fd == -1) {
		printf("Error: Could not open file %s\n", file_to_open);
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