#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef enum {
    BIN_FORMAT,
    FEXEC_FORMAT,
    MACHO64_FORMAT
} executable_format_t;

executable_format_t get_exec_type(char* name);

void write_exec(executable_format_t format, FILE* file, uint8_t* opcodes, uint64_t entry_offset, uint64_t opcode_num);

#define define_exec_write(format) \
    void write_##format(FILE* file, uint8_t* opcodes, uint64_t entry_offset, uint64_t opcode_num)

#define call_exec_write(format) \
    write_##format(file, opcodes, entry_offset, opcode_num)

define_exec_write(bin);
define_exec_write(fexec);
define_exec_write(macho64);