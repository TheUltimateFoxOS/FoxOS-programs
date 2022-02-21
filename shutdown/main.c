#include <stdio.h>
#include <sys/env.h>

int main() {
	printf("Shuttin down...\n");
	env2(ENV_ACPI_SHUTDOWN);

	return 0;
}