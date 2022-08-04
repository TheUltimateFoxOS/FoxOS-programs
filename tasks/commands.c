#include <commands.h>

#include <assert.h>
#include <sys/env.h>
#include <buildin/number_parser.h>

void lock_task(char* in, task_t** tasks, int max_tasks) {
	int number;
	__libc_parse_number(in, &number);

	assert(number < max_tasks);
	assert(tasks[number] != 0);

	tasks[number]->lock = true;
}

void unlock_task(char* in, task_t** tasks, int max_tasks) {
	int number;
	__libc_parse_number(in, &number);

	assert(number < max_tasks);
	assert(tasks[number] != 0);

	tasks[number]->lock = false;
}

void kill_task(char* in, task_t** tasks, int max_tasks) {
	int number;
	__libc_parse_number(in, &number);

	assert(number < max_tasks);
	assert(tasks[number] != 0);

	// if (tasks[number]->signals[SIG_INTR]) {
		// tasks[number]->signals[SIG_INTR](SIG_INTR);
	// } else {
		tasks[number]->kill_me = true;
	// }
}