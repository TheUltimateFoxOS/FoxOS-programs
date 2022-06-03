#pragma once

#include <stdint.h>

bool is_bmp(uint8_t* bmp_data);
int get_bmp_width(uint8_t* bmp_data);
int get_bmp_height(uint8_t* bmp_data);
uint32_t get_bmp_pixel(uint8_t* bmp_data, int x, int y);