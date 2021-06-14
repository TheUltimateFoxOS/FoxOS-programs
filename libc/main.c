#include <stdlib.h>
#include <sys/env.h>
#include <stdio.h>

extern int main(int argc, char* argv[], char* envp[]);

void _start() {
	__libc_init_alloc();

	char** argv = env(ENV_ARGV);
	char** envp = env(ENV_ENVP);
	int argc = 0;

	while(argv[argc] != NULL) {
		argc++;
	}

	int return_code = main(argc, argv, envp);

	exit(return_code);
}