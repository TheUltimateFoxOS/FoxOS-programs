#include <renderer/window_renderer.h>

#include <foxos/fox_graphics.h>

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <config.h>

window_t::window_t(int64_t x, int64_t y, int64_t width, int64_t height) {
    this->window_id = current_window_id++;

    this->window_x = x;
    this->window_y = y;
    this->window_width = width;
    this->window_height = height;

    window_list_t* current_window = window_list;
    if (current_window) {
        while (current_window->next) {
            current_window = current_window->next;
        }

        current_window->next = (window_list_t*) malloc(sizeof(window_list_t));
        current_window = current_window->next;
    } else {
        window_list = (window_list_t*) malloc(sizeof(window_list_t));
        current_window = window_list;
    }

    current_window->window = this;
    current_window->next = 0;
}

window_t::~window_t() {
    free(this->buffer);

    window_list_t* current_window = window_list;
    window_list_t* previous_window = 0;

    while (current_window) {
        if (current_window->window->window_id == this->window_id) {
            if (previous_window) {
                previous_window->next = current_window->next;
            } else {
                window_list = current_window->next;
            }

            break;
        }

        previous_window = current_window;
        current_window = current_window->next;
    }
}

void window_t::init() {
    if (this->buffer != 0 || this->window_title != 0) {
        return;
    }

    this->calculate_buffer_data();

    this->set_title((char*) new_window_text);
}

int window_t::get_window_id() {
    return this->window_id;
}

void window_t::calculate_buffer_data() {
    this->calculate_buffer_position();
    this->calculate_buffer_size();
}

void window_t::calculate_buffer_size() {
    this->buffer_width = this->window_width - 2;
    this->buffer_height = this->window_height - (window_bar_height + 3);

    size_t new_size = this->buffer_width * this->buffer_height;
    if (new_size == this->buffer_size) {
        return;
    }
    this->buffer_size = this->buffer_width * this->buffer_height;

    if (this->buffer) {
        free(this->buffer);
    }

    this->buffer = (uint32_t*) malloc(sizeof(uint32_t) * this->buffer_size);
    if (this->buffer) {
        for (uint32_t i = 0; i < this->buffer_size; i++) {
            this->buffer[i] = window_background_colour;
        }
    }
}

void window_t::calculate_buffer_position() {
    this->buffer_x = this->window_x + 1;
    this->buffer_y = this->window_y + window_bar_height + 2;
}

int64_t window_t::get_x() {
    return this->window_x;
}

int64_t window_t::get_y() {
    return this->window_y;
}

int64_t window_t::get_width() {
    return this->window_width;
}

int64_t window_t::get_height() {
    return this->window_height;
}

void window_t::draw_window() {
    int64_t tmp_x = this->window_x;
    int64_t tmp_y = this->window_y;
    int64_t tmp_width = this->window_width;
    int64_t tmp_height = this->window_height;

    int64_t seperator_bar_y = this->window_y + window_bar_height + 1;

    if (tmp_x >= screen_size.width || tmp_y >= screen_size.height) {
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

    if (tmp_x + tmp_width > screen_size.width) {
		tmp_width = screen_size.width - tmp_x;
        draw_right_bar = false;
    }
	if (tmp_y + tmp_height > screen_size.height) {
		tmp_height = screen_size.height - tmp_y;
        draw_bottom_bar = false;
	}
    if (seperator_bar_y >= screen_size.height) {
        draw_seperator_bar = false;
    }

    //Draw the horizontal bars
    if (draw_top_bar) {
        for (int64_t i = 0; i < tmp_width; i++) {
            fox_set_px_unsafe(tmp_x + i, tmp_y, window_border_colour);
        }
    }
    if (draw_seperator_bar) {
        for (int64_t i = 0; i < tmp_width; i++) {
            fox_set_px_unsafe(tmp_x + i, seperator_bar_y, window_border_colour);
        }
    }
    if (draw_bottom_bar) {
        int64_t bottom_bar_y = tmp_y + tmp_height - 1;
        if (bottom_bar_y >= 0) {
            for (int64_t i = 0; i < tmp_width; i++) {
                fox_set_px_unsafe(tmp_x + i, bottom_bar_y, window_border_colour);
            }
        }
    }

    //Draw the vertical bars
    if (draw_left_bar) {
        for (int64_t i = 0; i < tmp_height; i++) {
            fox_set_px_unsafe(tmp_x, tmp_y + i, window_border_colour);
        }
    }
    if (draw_right_bar) {
        int64_t right_bar_x = tmp_x + tmp_width - 1;
        if (right_bar_x >= 0) {
            for (int64_t i = 0; i < tmp_height; i++) {
                fox_set_px_unsafe(right_bar_x, tmp_y + i, window_border_colour);
            }
        }
    }

    //Fill the seperator
    tmp_x = this->window_x + 1;
    tmp_y = this->window_y + 1;
    tmp_width = this->window_width - 2;
    tmp_height = window_bar_height;

    if (tmp_x < 0) {
        tmp_width += tmp_x; //x is negative
        tmp_x = 0;
    }
    if (tmp_y < 0) {
        tmp_height += tmp_y; //y is negative
        tmp_y = 0;
    }

    if (tmp_x + tmp_width > screen_size.width) {
        tmp_width = screen_size.width - tmp_x;
    }
    if (tmp_y + tmp_height > screen_size.height) {
        tmp_height = screen_size.height - tmp_y;
    }

    if (tmp_width > 0 && tmp_height > 0) {
        fox_draw_rect_unsafe(tmp_x, tmp_y, tmp_width, tmp_height, window_bar_colour);

        tmp_x += window_bar_padding;
        tmp_y += window_bar_padding;
        tmp_width = tmp_width - (window_bar_padding * 2 + window_bar_button_size);

        if (tmp_width > 0) {
            int offset = 0;
            while (offset < this->title_length && (offset + 1) * font_width < tmp_width) {
                fox_draw_char(tmp_x + offset * font_width, tmp_y, this->window_title[offset], window_bar_text_colour, &screen_font);
                offset++;
            }
        }
    }

    if (!this->buffer) {
        return;
    }

    tmp_x = this->buffer_x;
    tmp_y = this->buffer_y;
    tmp_width = this->buffer_width;
    tmp_height = this->buffer_height;

	if (tmp_x < 0) {
        tmp_width += tmp_x; //x is negative
        tmp_x = 0;
    }
    if (tmp_y < 0) {
        tmp_height += tmp_y; //y is negative
        tmp_y = 0;
    }
    
    if (tmp_x + tmp_width > screen_size.width) {
        tmp_width = screen_size.width - tmp_x;
    }
    if (tmp_y + tmp_height > screen_size.height) {
        tmp_height = screen_size.height - tmp_y;
    }

	for (int64_t j = 0; j < tmp_height; j++) {
        int64_t offset = j * tmp_width;
		for (int64_t i = 0; i < tmp_width; i++) {
			fox_set_px_unsafe(i + tmp_x, j + tmp_y, this->buffer[offset + i]);
		}
	}
}

void window_t::move(int64_t x, int64_t y) {
    this->window_x = x;
    this->window_y = y;

    this->calculate_buffer_position();
}

void window_t::resize(int64_t width, int64_t height) {
    if (width < window_min_width) {
        this->window_width = window_min_width;
    } else {
        this->window_width = width;
    }

    if (height < window_min_height) {
        this->window_height = window_min_height;
    } else {
        this->window_height = height;
    }

    this->calculate_buffer_size();
}

void window_t::set_title(char* title) {
    if (this->window_title) {
        free(this->window_title);
    }

    this->title_length = strlen(title);

    this->window_title = (char*) malloc(this->title_length + 1);
    memset(this->window_title, 0, this->title_length + 1);
    strcpy(this->window_title, title);
}