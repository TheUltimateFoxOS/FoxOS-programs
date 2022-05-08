#pragma once

#include <stdint.h>

typedef struct {
    uint64_t x;
    uint64_t y;
    uint64_t width;
    uint64_t height;

    uint64_t buffer_x;
    uint64_t buffer_y;
    uint64_t buffer_width;
    uint64_t buffer_height;

    uint32_t* buffer;
} __attribute__((packed)) window_t;

typedef struct _window_list {
    window_t window;
    _window_list* next;
} __attribute__((packed)) window_list_t;

extern window_list_t* window_list;

window_t create_window(uint64_t x, uint64_t y, uint64_t width, uint64_t height);