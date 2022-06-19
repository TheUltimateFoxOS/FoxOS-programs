#pragma once

#include <foxos/window.h>

#include <stdint.h>
#include <stddef.h>

typedef struct {
    standard_foxos_window_t* window_address = 0;
} __attribute__((packed)) window_list_node_t;

void register_window(standard_foxos_window_t* window_address);
void unregister_window(standard_foxos_window_t* window_address);
void bring_window_to_front(standard_foxos_window_t* window_address);

void draw_windows();
void destroy_all_windows();
void mouse_handle_windows(int mouse_x, int mouse_y, int mouse_button);