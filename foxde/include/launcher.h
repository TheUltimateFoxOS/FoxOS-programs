#pragma once

#include <buildin/list.h>
#include <sys/spawn.h>

struct running_program_t {
	task_t* task;
	bool on_exit;
	char** argv;
};

extern list<running_program_t*>* running_programs;

void launcher_init();
void launcher_run(char* program);
void launcher_tick();