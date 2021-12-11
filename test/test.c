#include <stdio.h>
#include <stdlib.h>

#include <sys/spawn.h>

int main(int argc, char* argv[], char* envp[]) {
	printf("Hello world!\n");

	spawn("fat32_0:/bin/terminal.elf", argv, envp);

	while(1) {}

    return 0;
}
