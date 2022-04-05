#include <stdio.h>
#include <sys/env.h>

int main() {
	printf("Rebooting...\n");
	env2(ENV_ACPI_REBOOT);

	return 0;
}