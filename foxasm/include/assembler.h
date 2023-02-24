#pragma once

#include <stdio.h>

#define MAX_OPERANDS 50

typedef enum {
    UNDEFINED_OPERAND,
    REGISTER_OPERAND,
    ADDRESS_OPERAND,
    IMMEDIATE_OPERAND
} operand_type_t;

typedef struct {
    void* value;
    int size;
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

void assemble(FILE* source, FILE* output);