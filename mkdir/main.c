#include <stdio.h>
#include <stdlib.h>

#include <sys/mkdir.h>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("Usage: %s <dir>\n", argv[0]);
		return 1;
	}

	mkdir(argv[1]);

	return 0;
}