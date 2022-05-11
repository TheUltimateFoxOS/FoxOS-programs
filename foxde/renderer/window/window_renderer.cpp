#include <renderer/window_renderer.h>

#include <stdlib.h>
#include <string.h>

#include <config.h>

window_list_t* window_list = 0;
uint8_t current_window_id = 1;

void draw_windows() {
    window_list_t* current_window = window_list;

    while (current_window) {
        current_window->window->draw_window();
        current_window = current_window->next;
    }
}

void destroy_all_windows() {
    window_list_t* current_window = window_list;
    window_list_t* previous_window = 0;

    while (current_window) {
        previous_window = current_window;
        current_window = current_window->next;

        delete previous_window;
    }

    window_list = 0;
}