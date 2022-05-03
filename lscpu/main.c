#include <stdio.h>
#include <stdlib.h>
#include <sys/sm32_get_cpu_info.h>

int main() {
	cpu_info_t* cpu_info = (cpu_info_t*) malloc(sizeof(cpu_info_t) * 64);

	int len = sm32_get_cpu_info(cpu_info);

	for (int i = 0; i < len; i++) {
		printf("%d: %s %s @%d\n", cpu_info[i].id, cpu_info[i].manufacture, cpu_info[i].version, cpu_info[i].speed);
	}

	free(cpu_info);
}