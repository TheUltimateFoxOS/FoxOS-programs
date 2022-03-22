#pragma once

#include <stdint.h>

struct point_t {
	uint64_t x;
	uint64_t y;
};

void set_color(uint32_t color);
uint32_t get_color();

void set_cursor(struct point_t point);
struct point_t get_cursor();

struct point_t get_screen_size();

void clear_screen();
void clear_line();

void enable_print_char();
void disable_print_char();