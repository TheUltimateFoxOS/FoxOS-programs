#include <executables.h>

#include <executables/fexec.h>

executable_format_t get_exec_type(char* name) {
    if (strcmp(name, "bin") == 0) {
        return BIN_FORMAT;
    } else if (strcmp(name, "fexec") == 0) {
        return FEXEC_FORMAT;
    }
    return -1;
}

void write_exec(executable_format_t format, FILE* file, uint8_t* opcodes, uint64_t entry_offset, uint64_t opcode_num) {
    switch (format) {
        case BIN_FORMAT:
            call_exec_write(bin);
            break;
        case FEXEC_FORMAT:
            call_exec_write(fexec);
            break;
        default:
            printf("Unknown executable type!\n");
            break;
    }
}

define_exec_write(bin) {
    fwrite(opcodes, opcode_num, 1, file);
}

define_exec_write(fexec) {
    fexec_header_t header;
    memset((void*) &header, 0, sizeof(fexec_header_t));
    header.magic = FEXEC_MAGIC;
    header.version = 1;
    header.entry = entry_offset;
    header.size = opcode_num;

    fwrite(&header, sizeof(fexec_header_t), 1, file);
    fwrite(opcodes, opcode_num, 1, file);
}