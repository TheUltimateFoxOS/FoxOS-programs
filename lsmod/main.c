#include <stdio.h>
#include <sys/env.h>
#include <string.h>

#define max_modules 512
int main() {
	char* modules[max_modules];
	memset(modules, 0, sizeof(char*) * max_modules);

	env_set2(ENV_GET_LOADED_MODULES, max_modules, modules);

	for (int i = 0; i < max_modules && modules[i] != 0; i++) {
		printf("%s\n", modules[i]);
	}
}