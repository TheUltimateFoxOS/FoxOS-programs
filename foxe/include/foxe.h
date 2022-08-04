#pragma once

#include <stdio.h>
#include <stdint.h>
#include <foxos/fox_graphics.h>

#define CHAR_SIZE 16
#define CHAR_WIDTH 8
#define GRAPHICS_RENDERING_MODE
#define CURSOR_COLOR 0x33cccc
#define LINE_NUMBER_COLOR 0xa9a9a9
#define SPACE_BETWEEN_LINE_NUMBER_TEXT 1 * CHAR_WIDTH

extern char* copy_buffer;
extern bool copy_buffer_is_set;
extern int copy_buffer_length;

extern char* input_buffer;
extern char* filename;
extern char* file_type;
extern  FILE* f;
extern char* mode;
extern bool is_edited;
extern bool is_in_insert_mode;
extern uint32_t old_color;
extern int max_length_before_line_wrap;

extern int current_size;

extern int possible_lines_to_draw;

extern unsigned int ln_cnt;
extern unsigned int char_cnt;

extern unsigned int buffer_ln_idx;
extern unsigned int buffer_idx;

extern psf1_font_t font;

extern graphics_buffer_info_t graphics_buffer_info;
