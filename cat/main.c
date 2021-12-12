#include <sys/write.h>
#include <sys/read.h>
#include <sys/open.h>
#include <sys/close.h>
#include <sys/get_file_size.h>

#include <string.h>

#include <stdio.h>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	int fd = open(argv[1]);

	if (fd == -1) {
		printf("Error: Could not open file %s\n", argv[1]);
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