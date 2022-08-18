#pragma once

#include <stdbool.h>

#include <sys/spawn.h>

bool run_command(char* command, char** terminal_envp, bool* should_break, char** stdin, char** stdout);

char* search_executable(char* command);

void load_keymap(char* command);
void keydbg(bool onoff);
void cd(char** argv);
void pwd();
void export(char* command);
void read_(char* command);
void fault(char* command);

task_t* spawn_process(char** argv, char** terminal_envp, pipe stdout, pipe stdin);