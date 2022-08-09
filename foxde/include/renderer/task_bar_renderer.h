#pragma once

#include <stdint.h>
#include <config.h>

typedef struct icon_t {
	int x;
	int y;
	int width;
	int height;

	uint32_t* img_buffer;
	char* name;
} icon_t;

extern icon_t icons[MAX_ICONS];
extern int num_icons;

void add_icon(char* name);
void draw_task_bar();