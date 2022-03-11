#include <stdio.h>
#include <term.h>
#include <sys/env.h>
#include <render.h>

int main() {
	setup_terminal();

	int max_tasks = calculate_max_task_count();

	while (true) {
		task_t* tasks[max_tasks];
		memset(tasks, 0, sizeof(task_t*) * max_tasks);

		env_set2(ENV_READ_RUNNING_TASKS, 512, tasks);

		render_list(tasks, max_tasks);

		char input[512] = { 0 };
		gets(input);

		if (strcmp(input, "exit") == 0) {
			break;
		} else if (strncmp(input, "lock ", 5) == 0) {
			lock_task(input + 5, tasks, max_tasks);
		} else if (strncmp(input, "unlock ", 7) == 0) {
			unlock_task(input + 7, tasks, max_tasks);
		} else if (strncmp(input, "kill ", 5) == 0) {
			kill_task(input + 5, tasks, max_tasks);
		}
	}

	restore_terminal();
}