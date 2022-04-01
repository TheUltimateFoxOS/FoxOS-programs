#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef bool (*is_format_func)(uint8_t* buff);
typedef int (*get_width_func)(uint8_t* buff);
typedef int (*get_height_func)(uint8_t* buff);
typedef uint32_t (*get_pix)(uint8_t* buff, int x, int y);

typedef struct {
	is_format_func is_format;
	get_width_func get_width;
	get_height_func get_height;
	get_pix get_pix;
} image_reader_t;

extern image_reader_t bmp_reader;
extern image_reader_t fpic_reader;

image_reader_t* image_reader_detect(uint8_t* buff);