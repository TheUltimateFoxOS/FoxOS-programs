#include <stdio.h>
#include <assert.h>

#include <setjmp.h>

jmp_buf buf;

void main() {
	printf("Hello world %d\n", 100);

	int x = 5;

	setjmp(buf);

	printf("Loop %d!\n", x);
	x--;

	assert(x != 0);

	longjmp(buf, 1);
}
