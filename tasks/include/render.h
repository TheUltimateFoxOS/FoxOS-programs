#pragma once

#include <sys/spawn.h>

void write_rainbow(char* what);
void printf_rainbow(char* format, ...);

void setup_terminal();
void restore_terminal();

void render_list(task_t** tasks, int count, uint64_t total_memory_mb, uint64_t free_memory_mb, uint64_t used_memory_mb, uint64_t reserved_memory_mb);
int calculate_max_task_count();