#include <stdio.h>
#include <stdlib.h>

#include <sys/delete.h>
#include <sys/open.h>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("Usage: %s <file>\n", argv[0]);
		return 1;
	}

	int fd = open(argv[1]);
	delete_(fd);

	return 0;
}