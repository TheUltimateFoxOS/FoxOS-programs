#pragma once

#include <stdint.h>
#include <stddef.h>

struct framebuffer_t {
	void* base_address;
	size_t buffer_size;
	uint32_t width;
	uint32_t height;
};

struct framebuffer_t fb_info();

void copy_to_fb(void* addr);
void copy_from_fb(void* addr);