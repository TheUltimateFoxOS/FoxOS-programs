#include <stdio.h>
#include <stdlib.h>
#include <sys/insmod.h>

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Usage: %s <module>\n", argv[0]);
		return 1;
	}
	
	FILE* f = fopen(argv[1], "r");
	if (!f) {
		printf("Could not open module file %s\n", argv[1]);
		return 1;
	}

	void* module = malloc(f->size);
	fread(module, f->size, 1, f);

	insmod(module, f->size);

	fclose(f);
	free(module);
	
	return 0;
}