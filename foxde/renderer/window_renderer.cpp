#include <renderer/window_renderer.h>

#include <foxos/list.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/spawn.h>
#include <config.h>
#include <launcher.h>

#include <renderer/task_bar_renderer.h>

list_t<window_list_node_t>* window_list = new list_t<window_list_node_t>();

void register_window(standard_foxos_window_t* window_address) {
    list_t<window_list_node_t>::node* found_window = window_list->find<standard_foxos_window_t*>([](standard_foxos_window_t* window_address, list_t<window_list_node_t>::node* node) {
        return node->data.window_address == window_address;
    }, window_address);
    if (found_window != nullptr) { //Don't register the window again if it already exists
        return;
    }

    window_list_node_t new_window_node;
    memset(&new_window_node, 0, sizeof(window_list_node_t));
    new_window_node.window_address = window_address;

    window_list->add(new_window_node);
}

void unregister_window(standard_foxos_window_t* window_address) {
    list_t<window_list_node_t>::node* found_window = window_list->find<standard_foxos_window_t*>([](standard_foxos_window_t* window_address, list_t<window_list_node_t>::node* node) {
        return node->data.window_address == window_address;
    }, window_address);
    if (found_window == nullptr) { //The window isn't registered
        return;
    }

    window_list->remove(found_window);
}

void bring_window_to_front(standard_foxos_window_t* window_address) {
    list_t<window_list_node_t>::node* found_window = window_list->find<standard_foxos_window_t*>([](standard_foxos_window_t* window_address, list_t<window_list_node_t>::node* node) {
        return node->data.window_address == window_address;
    }, window_address);
    if (found_window == nullptr) { //The window isn't registered
        return;
    }

    window_list_node_t window_node;
    memcpy(&window_node, &found_window->data, sizeof(window_list_node_t));

    window_list->remove(found_window);
    window_list->add(window_node);
}

int get_window_number() {
    return window_list->get_length();
}

standard_foxos_window_t* get_window_by_index(int index) {
    list_t<window_list_node_t>::node* node = window_list->get(index);
    if (node == nullptr) {
        return nullptr;
    }

    return node->data.window_address;
}

standard_foxos_window_t* get_front_window() {
    list_t<window_list_node_t>::node* node = window_list->get(window_list->get_length() - 1);
    if (node == nullptr) {
        return nullptr;
    }

    return node->data.window_address;
}

void draw_window(window_list_node_t* window_node) {
    standard_foxos_window_t* window = window_node->window_address;

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

	if (window->exit_button) {
        window_node->exit_button_x = window->get_x() + (window->get_width() - (window_bar_button_size + window_bar_padding));
		window_node->exit_button_y = tmp_y;

		fox_draw_rect_outline(&graphics_buffer_info, window_node->exit_button_x, window_node->exit_button_y, window_bar_button_size, window_bar_button_size, 0xff0000);
		fox_draw_char(&graphics_buffer_info, window_node->exit_button_x + ((font_height - font_width) / 2), window_node->exit_button_y, 'X', 0xff0000, &screen_font);
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
    window_list->foreach([](list_t<window_list_node_t>::node* node) {
        draw_window(&node->data);
    });
}

char* program_spawn_terminal_argv[3];

void mouse_handle_windows(int64_t mouse_x, int64_t mouse_y, mouse_buttons_e mouse_button) {
	for (int i = 0; i < num_icons; i++) {
		if (mouse_x >= icons[i].x && mouse_x <= icons[i].x + icons[i].width && mouse_y >= icons[i].y && mouse_y <= icons[i].y + icons[i].height) {
			if (mouse_button == MOUSE_BUTTON_LEFT) {
				launcher_run(icons[i].name);
			}
			return;
		}
	}

    struct lambda_callback_data_t {
        int64_t mouse_x;
        int64_t mouse_y;
        mouse_buttons_e mouse_button;
    };

    lambda_callback_data_t data;
    data.mouse_x = mouse_x;
    data.mouse_y = mouse_y;
    data.mouse_button = mouse_button;

	window_list->foreach([](list_t<window_list_node_t>::node* node, void* lambda_data) {
        window_list_node_t* window_node = &node->data;
        standard_foxos_window_t* window = window_node->window_address;

        lambda_callback_data_t* data = (lambda_callback_data_t*) lambda_data;

		// check if exit button is pressed
		if (window->exit_button) {
			if (data->mouse_x >= window_node->exit_button_x && data->mouse_x <= window_node->exit_button_x + window_bar_button_size &&
				data->mouse_y >= window_node->exit_button_y && data->mouse_y <= window_node->exit_button_y + window_bar_button_size) {
				window->exit_button = false;
				window->should_exit = true;
			}
		}

		// if the mouse is inside of the window then translate the mouse cords to the window cords
		if (data->mouse_x >= window->get_x() && data->mouse_x <= window->get_x() + window->get_width() && data->mouse_y >= window->get_y() && data->mouse_y <= window->get_y() + window->get_height()) {
			int tmp_x = window->get_x() + window_buffer_offset_x;
			int tmp_y = window->get_y() + window_buffer_offset_y;
			int tmp_width = window->get_buffer_width();
			int tmp_height = window->get_buffer_height();

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
				data->mouse_x -= tmp_x;
				data->mouse_y -= tmp_y;
				window->send_click(data->mouse_x, data->mouse_y, data->mouse_button);
			}
		}
	}, &data);
}

void destroy_all_windows() {
    delete window_list;

    window_list = new list_t<window_list_node_t>();
}