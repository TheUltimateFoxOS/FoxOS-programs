#pragma once

#include <foxos/psf1_font.h>
#include <foxos/term.h>
#include <foxos/g_syscalls.h>

#define font_width 8
#define font_height 16

#define background_colour 0xff00ff00

#define util_bar_padding 5
#define util_bar_height font_height + (util_bar_padding * 2)
#define util_bar_colour 0x000000

#define task_bar_offset_y util_bar_height

#define task_bar_width 50
#define task_bar_icon_padding 5
#define task_bar_icon_size task_bar_width - (task_bar_icon_padding * 2)
#define task_bar_colour 0x404040

#define window_bar_padding 5
#define window_bar_colour 0xFFFFFF
#define window_bar_text_colour 0x000000
#define window_bar_height font_height + (window_bar_padding * 2)
#define window_bar_button_size window_bar_height

#define window_min_width window_bar_button_size * 2 + 2
#define window_min_height window_bar_height * 2 + 3

#define window_background_colour 0xFFFF00
#define window_border_colour 0x000000

#define util_bar_text "FoxOS DE"
#define new_window_text "FoxOS Window"

typedef struct {
    int64_t width = 0;
    int64_t height = 0;
} screen_data_t;


extern mouse_position_t mouse_pos;
extern int mouse_button_down;

extern screen_data_t screen_size;
extern psf1_font_t screen_font;
