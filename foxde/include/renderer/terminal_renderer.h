#pragma once

#include <stdint.h>

void scroll_bg_console();
void print_bg_console(char* s, int l);

void load_terminal();
void start_terminal(bool* on_exit, char* envp[]);
void draw_terminal();