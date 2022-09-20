#pragma once

#include <foxos/window.h>

#include <stdint.h>
#include <stddef.h>

typedef struct {
    standard_foxos_window_t* window_address = 0;

    int64_t exit_button_x;
    int64_t exit_button_y;
} __attribute__((packed)) window_list_node_t;

void init_window_renderer();

void register_window(standard_foxos_window_t* window_address);
void unregister_window(standard_foxos_window_t* window_address);
void bring_window_to_front(standard_foxos_window_t* window_address);
int get_window_number();
standard_foxos_window_t* get_window_by_index(int index);
standard_foxos_window_t* get_front_window();

void draw_windows();
void destroy_all_windows();

bool mouse_handle_windows(int64_t mouse_x, int64_t mouse_y, mouse_buttons_e mouse_button);