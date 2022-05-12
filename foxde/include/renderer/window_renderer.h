#pragma once

#include <stdint.h>
#include <stddef.h>

class window_t {
    public:
        window_t(int64_t x, int64_t y, int64_t width, int64_t height);
        ~window_t();

        int64_t get_x();
        int64_t get_y();
        int64_t get_width();
        int64_t get_height();

        void draw_window();

        void move(int64_t x, int64_t y);
        void resize(int64_t width, int64_t height);

        void set_title(char* title);

    private:
        void calculate_buffer_size();
        void calculate_buffer_position();

        int64_t window_x = 0;
        int64_t window_y = 0;
        int64_t window_width = 0;
        int64_t window_height = 0;

        int64_t buffer_x = 0;
        int64_t buffer_y = 0;
        int64_t buffer_width = 0;
        int64_t buffer_height = 0;

        uint32_t* buffer = 0;
        size_t buffer_size = 0;

        char* window_title = 0;
        uint8_t title_length = 0;
};

typedef struct {
    window_t* window_address = 0;
} __attribute__((packed)) window_list_node_t;

void register_window(window_t* window_address);
void unregister_window(window_t* window_address);
void bring_window_to_front(window_t* window_address);

void draw_windows();
void destroy_all_windows();