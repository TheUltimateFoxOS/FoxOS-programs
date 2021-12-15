#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/env.h>
#include <sys/mkdir.h>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("Usage: %s <dir>\n", argv[0]);
		return 1;
	}

	char dir_to_create[256];
	memset(dir_to_create, 0, 256);
	resolve(argv[1], dir_to_create);

	mkdir(dir_to_create);

	return 0;
}