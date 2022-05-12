#include <renderer/window_renderer.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <config.h>

window_list_node_t* window_list = 0;
int window_number = 0;
int current_window_id = 0;

void register_window(window_t* window_address) {
    for (int i = 0; i < window_number; i++) { //Don't register the window again
        if (window_list[i].window_address == window_address) {
            return;
        }
    }

    window_number++;
    window_list = (window_list_node_t*) realloc((void*) window_list, sizeof(window_list_node_t) * window_number);

    if (!window_list) {
        return;
    }
    
    //Get the new node address and set it's data
    window_list_node_t* new_window_node = (window_list_node_t*) ((uint64_t) window_list + (sizeof(window_list_node_t) * (window_number - 1)));
    memset(new_window_node, 0, sizeof(window_list_node_t));
    new_window_node->window_address = window_address;
}

void unregister_window(window_t* window_address) {
    if (!window_list) {
        return;
    }

    int found_index = -1; //Find the index of the window to remove
    for (int i = 0; i < window_number; i++) {
        if (window_list[i].window_address == window_address) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) { //The window was not found
        return;
    }

    window_number--;
    if (window_number == 0) { //If there are no more windows, then just free the list
        free(window_list);
        window_list = 0;
    } else { //If there are still windows, then reallocate the list
        window_list_node_t* new_window_list = (window_list_node_t*) malloc(sizeof(window_list_node_t) * window_number);

        int current_id = 0;
        for (int i = 0; i < window_number + 1; i++) {
            if (i != found_index) {
                new_window_list[current_id] = window_list[i];
                current_id++;
            }
        }

        free(window_list);
        window_list = new_window_list;

        assert(current_id == window_number);
    }  
}

void bring_window_to_front(window_t* window_address) {
    if (!window_list) {
        return;
    }
    if (window_number == 1) {
        return;
    }

    int found_index = -1; //Find the index of the window to bring to the front
    for (int i = 0; i < window_number; i++) {
        if (window_list[i].window_address == window_address) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) { //The window was not found
        return;
    }

    window_list_node_t temp = window_list[found_index]; //Store the window to be moved
    for (int i = found_index + 1; i < window_number; i++) { //Move all the windows after the found window
        window_list[i - 1] = window_list[i];
    }
    window_list[window_number - 1] = temp; //Move the window to the end of the array
}

void draw_windows() {
    if (!window_list) { //Don't draw if there are no windows
        return;
    }

    for (int i = 0; i < window_number; i++) {
        window_list[i].window_address->draw_window();
    }
}

void destroy_all_windows() {
    if (!window_list) {
        return;
    }

    for (int i = 0; i < window_number; i++) { //Delete the window classes
        delete window_list[i].window_address;
    }
    free(window_list); //Free the list

    window_list = 0;
}