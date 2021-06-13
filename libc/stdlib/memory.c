#include <stdlib.h>
#include <sys/memory.h>

void* __libc_malloc(size_t size) {
	return memory(NULL, size, MEM_ALLOC);
}

void __libc_free(void* address) {
	memory(address, 0, MEM_FREE);
}