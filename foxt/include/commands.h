#pragma once

#include <sys/spawn.h>

void lock_task(char* in, task_t** tasks, int max_tasks);
void unlock_task(char* in, task_t** tasks, int max_tasks);
void kill_task(char* in, task_t** tasks, int max_tasks);
