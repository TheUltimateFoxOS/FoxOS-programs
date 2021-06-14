#include <sys/env.h>
#include <stddef.h>

char** env(int mode) {
	char** env_ptr = NULL;
	__asm__ __volatile__ ("int $0x30" : "=c" (env_ptr) : "a" (SYS_ENV), "b" (mode));

	return env_ptr;
}
