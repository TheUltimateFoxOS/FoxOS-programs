#include <test_tasks.h>

#include <stdio.h>
#include <sys/env.h>
#include <sys/spawn.h>

int tasks(int argc, char* argv[], char* envp[]) {
	task_t* tasks[512] = { 0 };
	env_set2(ENV_READ_RUNNING_TASKS, 512, tasks);

	for (int i = 0; i < 512; i++) {
		if (tasks[i] == 0) {
			break;
		}

		printf("%d: %s (running on cpu %d)\n", i, tasks[i]->argv[0], tasks[i]->running_on_cpu);
	}
}