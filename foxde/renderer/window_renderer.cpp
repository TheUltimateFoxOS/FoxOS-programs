#include <renderer/window_renderer.h>

#include <config.h>

#include <stdlib.h>
#include <string.h>

window_list_t* window_list = 0;

window_t create_window(uint64_t x, uint64_t y, uint64_t width, uint64_t height) {
    window_t window;
    window.x = x;
    window.y = y;
    window.width = width;
    window.height = height;

    window.buffer_x = x + 1;
    window.buffer_y = y + window_bar_height + 1;
    window.buffer_width = width - 2;
    window.buffer_height = height - window_bar_height - 2;

    window.buffer = (uint32_t*) malloc(sizeof(uint32_t) * window.buffer_width * window.buffer_height);
}