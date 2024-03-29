#pragma once

#include <stdint.h>
#include <config.h>

typedef struct task_bar_icon_t {
	int x;
	int y;
	int width;
	int height;

	uint32_t* img_buffer;
	char* name;
};

bool add_icon(char* name);
void draw_task_bar();

bool mouse_handle_icons(int64_t mouse_x, int64_t mouse_y, mouse_buttons_e mouse_button);