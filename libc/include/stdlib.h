#pragma once
#include <extern.h>
#include <stddef.h>


#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

EXPOSEC void abort(void);
EXPOSEC void exit(int status);
EXPOSEC void _Exit(int status);

EXPOSEC void __libc_exit(int code);

EXPOSEC void* __libc_malloc(size_t size);
EXPOSEC void __libc_free(void* address);