#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    UNDEFINED_OPERAND,
    REGISTER_OPERAND,
    STRING_OPERAND,
    IMMEDIATE_OPERAND,
    LABEL_OPERAND
} operand_type_t;

typedef struct {
    void* value;
    int size;
    bool indirect; //If true, the value is an address in a register
    operand_type_t type;
} operand_t;

typedef struct {
    char* buffer;
    int buffer_size;

    int line_num;
    int part_num;

    char* mnemonic;
    char** operands;
} lvl1_line_t;

typedef struct {
    int line_num;
    char* mnemonic;
    int operand_num;
    operand_t* operands;
} lvl2_line_t;

typedef enum { //Annotated with the 64bit register names
    R0 = 0, //RAX
    R1, //RCX
    R2, //RDX
    R3, //RBX
    R4, //RSP
    R5, //RBP
    R6, //RSI
    R7, //RDI
    R8, //R8
    R9, //R9
    R10, //...
    R11,
    R12,
    R13,
    R14,
    R15
} register_code_t;

typedef struct {
    register_code_t code;
    int size;
} cpu_register_t;

cpu_register_t get_register(char* str);

uint8_t get_two_register_magic(cpu_register_t* dest, cpu_register_t* src);

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