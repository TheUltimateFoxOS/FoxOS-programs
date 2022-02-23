#pragma once

#include <stdbool.h>

char* search_executable(char* command);

void load_keymap(char* command);
void keydbg(bool onoff);
void cd(char* command);
void pwd();
void export(char* command);

void spawn_process(char* command, char** terminal_envp);