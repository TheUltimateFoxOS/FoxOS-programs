#pragma once

#include <stdint.h>
#include <stddef.h>

struct framebuffer_t {
	void* base_address;
	size_t buffer_size;
	uint32_t width;
	uint32_t height;
};

struct mouse_position_t {
	int x;
	int y;
};

enum mouse_buttons_e {
	MOUSE_BUTTON_LEFT = 1,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE
};

struct framebuffer_t fb_info();

void copy_to_fb(void* addr);
void copy_from_fb(void* addr);

void mouse_reset();
struct mouse_position_t mouse_position();
int mouse_button();