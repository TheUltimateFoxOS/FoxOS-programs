#include <sys/write.h>
#include <sys/read.h>
#include <sys/open.h>
#include <sys/close.h>
#include <sys/get_file_size.h>

#include <foxos/term.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <buildin/rainbow.h>
int cidx = 0;

void write_rainbow(char* what) {
	for (int i = 0; i < strlen(what); i++) {
		uint32_t color = __libc_rainbow(++cidx);
		set_color(color);
		putchar(what[i]);
	}
}

void printf_rainbow(char* format, ...) {
	va_list args;
	va_start(args, format);

	char buffer[1024] = { 0 };
	vsprintf(buffer, format, args);

	write_rainbow(buffer);

	va_end(args);
}

void cat_mode(char* file) {
	char file_to_open[256];
	memset(file_to_open, 0, 256);
	bool canresolve = resolve(file, file_to_open);

	if (!canresolve) {
		printf_rainbow("Error: No such file or directory: %s\n", file);
		abort();
	}

	int fd = open(file_to_open);
	if (fd == -1) {
		printf_rainbow("Error: Could not open file %s\n", file_to_open);
		abort();
	}

	size_t size = get_file_size(fd);

	char buffer[size];
	memset(buffer, 0, size);

	read(fd, buffer, size, 0);
	write_rainbow(buffer);

	close(fd);
}

void stdin_mode() {
	char in[2] = { 0 };
	while ((in[0] = getchar()) != 0) {
		write_rainbow(in);
	}
}

int main(int argc, char* argv[], char* envp[]) {
	uint32_t old_color = get_color();

	if (argc == 1) {
		stdin_mode();
	} else if (argc == 2) {
		cat_mode(argv[1]);
	} else {
		printf_rainbow("Usage: %s [file]\n", argv[0]);
	}

	set_color(old_color);
	return 0;
}