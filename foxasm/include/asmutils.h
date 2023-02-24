#pragma once

#include <stdbool.h>
#include <stdint.h>

bool is_digits(char* str);

bool is_hex(char* str);

bool is_binary(char* str);

bool is_letters(char* str);

bool is_alphanumeric(char* str);

void str_to_upper(char* str);

uint64_t hex_str_to_num(char* str);

uint64_t binary_str_to_num(char* str);

uint64_t str_to_num(char* str);

char* find_string_end(char* str, char string_delimiter);