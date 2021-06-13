#include <stdlib.h>

extern int main();

void _start() {
	__libc_init_alloc();
	int return_code = main();

	exit(return_code);
}