#pragma once

#include <asmutils.h>

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define define_mnemonic(name) \
    void mnemonic_##name(lvl2_line_t* line, uint8_t* opcodes, int* opcode_num, bool* errors_found)

#define check_mnemonic(name) \
    else if (strcmp(line->mnemonic, #name) == 0) { \
        mnemonic_##name(line, line_opcodes, &line_opcode_num, &errors_found); \
    }

#define throw_error(message, ...) \
    printf("Error line %d: "message"\n", line->line_num, ##__VA_ARGS__); \
    *errors_found = true; \
    return;

#define append_opcode(op) \
    opcodes[(*opcode_num)++] = op

define_mnemonic(INT);
define_mnemonic(MOV);