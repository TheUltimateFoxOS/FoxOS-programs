#include <renderer/window_renderer.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <config.h>

window_list_node_t* window_list = 0;
int window_number = 0;
int current_window_id = 0;

void register_window(standard_foxos_window_t* window_address) {
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

void unregister_window(standard_foxos_window_t* window_address) {
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

void bring_window_to_front(standard_foxos_window_t* window_address) {
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

void draw_window(standard_foxos_window_t* window) {
    //Draw the window bars
    int64_t tmp_x = window->get_x();
    int64_t tmp_y = window->get_y();
    int64_t tmp_width = window->get_width();
    int64_t tmp_height = window->get_height();

    int64_t seperator_bar_y = window->get_y() + window_bar_height + 1;

    if (tmp_x >= graphics_buffer_info.width || tmp_y >= graphics_buffer_info.height) {
		return;
	}

    bool draw_top_bar = true;
    bool draw_bottom_bar = true;
    bool draw_left_bar = true;
    bool draw_right_bar = true;
    bool draw_seperator_bar = true;

    if (tmp_x < 0) {
        tmp_width += tmp_x; //x is negative
        tmp_x = 0;
        draw_left_bar = false;
    }
    if (tmp_y < 0) {
        tmp_height += tmp_y; //y is negative
        tmp_y = 0;
        draw_top_bar = false;
    }
    if (seperator_bar_y < 0) {
        draw_seperator_bar = false;
    }

    if (tmp_x + tmp_width > graphics_buffer_info.width) {
		tmp_width = graphics_buffer_info.width - tmp_x;
        draw_right_bar = false;
    }
	if (tmp_y + tmp_height > graphics_buffer_info.height) {
		tmp_height = graphics_buffer_info.height - tmp_y;
        draw_bottom_bar = false;
	}
    if (seperator_bar_y >= graphics_buffer_info.height) {
        draw_seperator_bar = false;
    }

    //Draw the horizontal bars
    if (draw_top_bar) {
        for (int64_t i = 0; i < tmp_width; i++) {
            fox_set_px_unsafe(&graphics_buffer_info, tmp_x + i, tmp_y, window_border_colour);
        }
    }
    if (draw_seperator_bar) {
        for (int64_t i = 0; i < tmp_width; i++) {
            fox_set_px_unsafe(&graphics_buffer_info, tmp_x + i, seperator_bar_y, window_border_colour);
        }
    }
    if (draw_bottom_bar) {
        int64_t bottom_bar_y = tmp_y + tmp_height - 1;
        if (bottom_bar_y >= 0) {
            for (int64_t i = 0; i < tmp_width; i++) {
                fox_set_px_unsafe(&graphics_buffer_info, tmp_x + i, bottom_bar_y, window_border_colour);
            }
        }
    }

    //Draw the vertical bars
    if (draw_left_bar) {
        for (int64_t i = 0; i < tmp_height; i++) {
            fox_set_px_unsafe(&graphics_buffer_info, tmp_x, tmp_y + i, window_border_colour);
        }
    }
    if (draw_right_bar) {
        int64_t right_bar_x = tmp_x + tmp_width - 1;
        if (right_bar_x >= 0) {
            for (int64_t i = 0; i < tmp_height; i++) {
                fox_set_px_unsafe(&graphics_buffer_info, right_bar_x, tmp_y + i, window_border_colour);
            }
        }
    }

    //Fill the seperator
    tmp_x = window->get_x() + 1;
    tmp_y = window->get_y() + 1;
    tmp_width = window->get_width() - 2;
    tmp_height = window_bar_height;

    if (tmp_x < 0) {
        tmp_width += tmp_x; //x is negative
        tmp_x = 0;
    }
    if (tmp_y < 0) {
        tmp_height += tmp_y; //y is negative
        tmp_y = 0;
    }

    if (tmp_x + tmp_width > graphics_buffer_info.width) {
        tmp_width = graphics_buffer_info.width - tmp_x;
    }
    if (tmp_y + tmp_height > graphics_buffer_info.height) {
        tmp_height = graphics_buffer_info.height - tmp_y;
    }

    if (tmp_width > 0 && tmp_height > 0) {
        fox_draw_rect_unsafe(&graphics_buffer_info, tmp_x, tmp_y, tmp_width, tmp_height, window_bar_colour);

        //Draw the bar title
        tmp_x += window_bar_padding;
        tmp_y += window_bar_padding;
        tmp_width = tmp_width - (window_bar_padding * 2 + window_bar_button_size);

        if (tmp_width > 0) {
            int offset = 0;
            char* window_title = window->get_title();
            while (offset < window->get_title_length() && (offset + 1) * font_width < tmp_width) {
                fox_draw_char(&graphics_buffer_info, tmp_x + offset * font_width, tmp_y, window_title[offset], window_bar_text_colour, &screen_font);
                offset++;
            }
        }
    }

    if (!window->buffer) { //If there is no buffer then we can't fill it.
        return;
    }

    tmp_x = window->get_x() + window_buffer_offset_x;
    tmp_y = window->get_y() + window_buffer_offset_y;
    tmp_width = window->get_buffer_width();
    tmp_height = window->get_buffer_height();

	if (tmp_x < 0) {
        tmp_width += tmp_x; //x is negative
        tmp_x = 0;
    }
    if (tmp_y < 0) {
        tmp_height += tmp_y; //y is negative
        tmp_y = 0;
    }
    
    if (tmp_x + tmp_width > graphics_buffer_info.width) {
        tmp_width = graphics_buffer_info.width - tmp_x;
    }
    if (tmp_y + tmp_height > graphics_buffer_info.height) {
        tmp_height = graphics_buffer_info.height - tmp_y;
    }

	if (window->frame_ready) {
		for (int64_t j = 0; j < tmp_height; j++) {
			int64_t offset = j * tmp_width;
			for (int64_t i = 0; i < tmp_width; i++) {
				window->old_frame[offset + i] = window->buffer[offset + i];
				fox_set_px_unsafe(&graphics_buffer_info, i + tmp_x, j + tmp_y, window->buffer[offset + i]);
			}
		}
	} else {
		for (int64_t j = 0; j < tmp_height; j++) {
			int64_t offset = j * tmp_width;
			for (int64_t i = 0; i < tmp_width; i++) {
				fox_set_px_unsafe(&graphics_buffer_info, i + tmp_x, j + tmp_y, window->old_frame[offset + i]);
			}
		}
	}

	window->frame_ready = false;
}

void draw_windows() {
    if (!window_list) { //Don't draw if there are no windows
        return;
    }

    for (int i = 0; i < window_number; i++) {
        draw_window(window_list[i].window_address);
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