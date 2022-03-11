#pragma once

#include <sys/spawn.h>

void write_rainbow(char* what);
void printf_rainbow(char* format, ...);

void setup_terminal();
void restore_terminal();

void render_list(task_t** tasks, int count);
int calculate_max_task_count();