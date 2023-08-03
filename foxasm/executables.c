#include <executables.h>

#include <executables/fexec.h>
#include <executables/macho64.h>

executable_format_t get_exec_type(char* name) {
    if (strcmp(name, "bin") == 0) {
        return BIN_FORMAT;
    } else if (strcmp(name, "fexec") == 0) {
        return FEXEC_FORMAT;
    } else if (strcmp(name, "macho64") == 0) {
        return MACHO64_FORMAT;
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
        case MACHO64_FORMAT:
            call_exec_write(macho64);
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

define_exec_write(macho64) {
    //--- Define the header, commands and data ---
    macho64_header_t header;
    memset((void*) &header, 0, sizeof(macho64_header_t));
    header.magic = 0xfeedfacf; // 64-bit
    header.cputype = 0x01000007; // x86_64
    header.cpusubtype = 0x80000003;
    header.filetype = 0x2; // EXECUTABLE
    header.ncmds = 3;
    header.flags = 0x1;
    // Size of commands will be set later

    macho64_load_command_t pagezero_command;
    pagezero_command.cmd = 0x19; // LC_SEGMENT_64
    pagezero_command.cmdsize = sizeof(macho64_load_command_t) + sizeof(macho64_segment_command_t);

    macho64_segment_command_t pagezero_command_data;
    memset((void*) &pagezero_command_data, 0, sizeof(macho64_segment_command_t));
    strcpy(pagezero_command_data.name, "__PAGEZERO");
    pagezero_command_data.vmsize = 0x1000;

    macho64_load_command_t segment_command;
    segment_command.cmd = 0x19; // LC_SEGMENT_64
    segment_command.cmdsize = sizeof(macho64_load_command_t) + sizeof(macho64_segment_command_t) + sizeof(macho64_section64_t);

    macho64_segment_command_t segment_command_data;
    memset((void*) &segment_command_data, 0, sizeof(macho64_segment_command_t));
    strcpy(segment_command_data.name, "__TEXT");
    segment_command_data.vmaddr = pagezero_command_data.vmsize;
    // File size and vmsize will be set later
    segment_command_data.maxprot = 0x7; // RWX
    segment_command_data.initprot = 0x5; // RX
    segment_command_data.nsects = 1;

    macho64_section64_t text_section;
    memset((void*) &text_section, 0, sizeof(macho64_section64_t));
    strcpy(text_section.sectname, "__text");
    strcpy(text_section.segname, "__TEXT");
    text_section.size = opcode_num;
    // Address and offset will be set later

    macho64_load_command_t unix_thread_command;
    unix_thread_command.cmd = 0x5; // LC_UNIXTHREAD
    unix_thread_command.cmdsize = sizeof(macho64_load_command_t) + sizeof(thread_state64_command_t);

    thread_state64_command_t unix_thread_command_data;
    memset((void*) &unix_thread_command_data, 0, sizeof(thread_state64_command_t));
    unix_thread_command_data.flavor = 0x4; // x86_64_THREAD_STATE
    unix_thread_command_data.count = 42;
    // Code start will be set later

    //--- Set the dynamic values ---
    header.sizeofcmds = pagezero_command.cmdsize + segment_command.cmdsize + unix_thread_command.cmdsize;
    segment_command_data.filesize = sizeof(macho64_header_t) + header.sizeofcmds + opcode_num;
    segment_command_data.vmsize = segment_command_data.filesize;
    text_section.offset = sizeof(macho64_header_t) + header.sizeofcmds;
    text_section.addr = pagezero_command_data.vmsize + text_section.offset;
    unix_thread_command_data.rip = text_section.addr + entry_offset;

    //--- Write the file ---
    fwrite(&header, sizeof(macho64_header_t), 1, file);
    fwrite(&pagezero_command, sizeof(macho64_load_command_t), 1, file);
    fwrite(&pagezero_command_data, sizeof(macho64_segment_command_t), 1, file);
    fwrite(&segment_command, sizeof(macho64_load_command_t), 1, file);
    fwrite(&segment_command_data, sizeof(macho64_segment_command_t), 1, file);
    fwrite(&text_section, sizeof(macho64_section64_t), 1, file);
    fwrite(&unix_thread_command, sizeof(macho64_load_command_t), 1, file);
    fwrite(&unix_thread_command_data, sizeof(thread_state64_command_t), 1, file);
    fwrite(opcodes, opcode_num, 1, file);

    if (segment_command_data.filesize < 0x1000) {
        uint8_t padding[0x1000];
        memset((void*) padding, 0, 0x1000);
        fwrite(padding, 0x1000 - segment_command_data.filesize, 1, file);
    }
}
