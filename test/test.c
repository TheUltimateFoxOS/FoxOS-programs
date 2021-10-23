#include <stdio.h>
#include <stdlib.h>

#include <sys/write.h>

int main(int argc, char* argv[], char* envp[]) {
	printf("Hello world!\n");

	FILE* file = fopen("stivale:test.elf", "r");

	char buffer[1024];

	fseek(file, 1, SEEK_SET);
	fread(buffer, 1, 1024, file);

	write(STDOUT, buffer, 1024);

	printf("\n");

	fclose(file);


    return 0;
}
