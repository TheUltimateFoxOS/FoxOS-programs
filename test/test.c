#include <stdio.h>
#include <stdlib.h>

#include <sys/spawn.h>

char* global = "global";

int main(int argc, char* argv[], char* envp[]) {
	printf("Hello world!\n");
	printf("global: %s\n", global);

	printf("argc: %d\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("argv[%d]: %s\n", i, argv[i]);
	}

	// spawn("fat32_0:/bin/terminal.elf", argv, envp);

	// while(1) {}

    return 0;
}
