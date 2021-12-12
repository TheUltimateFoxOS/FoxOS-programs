#include <sys/write.h>

#include <string.h>

int main(int argc, char* argv[]) {
	for (int i = 1; i < argc; i++) {
		write(STDOUT, argv[i], strlen(argv[i]), 0);
		write(STDOUT, " ", 1, 0);
	}

	write(STDOUT, "\n", 1, 0);

	return 0;
}