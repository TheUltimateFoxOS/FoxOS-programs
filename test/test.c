#include <sys/write.h>

void main() {
	char str[] = "Hello c world!";
	write(STDOUT, str, sizeof(str));
}
