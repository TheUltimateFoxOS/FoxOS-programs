#include <test_system.h>

#include <stdlib.h>

int system_(int argc, char* argv[], char* envp[]) {
	system("ls");
}