#pragma once

#include <foxos/psf1_font.h>
#include <foxos/term.h>

#define font_width 8
#define font_height 16

#define background_colour 0xFFFFFF

#define util_bar_padding 5
#define util_bar_height font_height + (util_bar_padding * 2)
#define util_bar_colour 0x000000

#define task_bar_offset_y util_bar_height

#define task_bar_width 50
#define task_bar_icon_padding 5
#define task_bar_icon_size (task_bar_width - (task_bar_icon_padding * 2))
#define task_bar_colour 0x404040

#define window_bar_padding 5
#define window_bar_height font_height + (window_bar_padding * 2)

extern point_t screen_size;
extern psf1_font_t screen_font;