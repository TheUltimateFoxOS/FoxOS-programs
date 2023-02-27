#include <assembler.h>

#include <asmutils.h>
#include <mnemonics.h>
#include <executables.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

bool parse_source_lvl1(char* source, lvl1_line_t** lines, int* line_num) {
    bool errors_found = false;

    char* buffer = (char*) malloc(4096);
    memset(buffer, 0, 4096);
    int buffer_index = 0;
    int line_number = 0;

    int sub_index = 0;
    int mnemonic = 0;
    int operands[MAX_OPERANDS];

    bool was_code = false;
    bool comment = false;

    while (*source) {
        char current = *source++;

        switch (current) {
            case ' ': //These are characters that separate operands
            case '\t':
            case ';':
            case ',': {
                if (was_code) {
                    buffer[buffer_index++] = 0;
                    was_code = false;
                } else if (current == ',') {
                    printf("Error line %d: Unexpected comma\n", line_number + 1);
                    errors_found = true;
                    break;
                }
                
                if (current == ';') {
                    comment = true;
                }
                break;
            }
            case '\n': { //On a new line, we can add a lvl1_line_t to our list
                line_number++;

                comment = false;
                was_code = false;

                if (buffer_index == 0) {
                    break;
                }
                buffer_index++;

                *lines = (lvl1_line_t*) realloc(*lines, sizeof(lvl1_line_t) * (*line_num + 1));
                char* tmp_buffer = (char*) malloc(buffer_index); //The string is already null terminated
                memcpy(tmp_buffer, buffer, buffer_index);
                (*lines)[*line_num].buffer = tmp_buffer;
                (*lines)[*line_num].buffer_size = buffer_index;
                (*lines)[*line_num].line_num = line_number;
                (*lines)[*line_num].part_num = sub_index;
                (*lines)[*line_num].mnemonic = (char*) ((uint64_t) tmp_buffer + mnemonic);
                (*lines)[*line_num].operands = (char**) malloc(sizeof(char*) * sub_index);
                for (int i = 0; i < sub_index; i++) {
                    (*lines)[*line_num].operands[i] = (char*) ((uint64_t) tmp_buffer + operands[i]);
                }
                (*line_num)++;

                memset(buffer, 0, 4096);
                buffer_index = 0;
                sub_index = 0;
                mnemonic = 0;
                memset(operands, 0, sizeof(operands));
                break;
            }
            default: { //By default, we just add the character to the buffer
                if (comment) {
                    break;
                }

                if (!was_code) { //If this is the start of a new operand, we need to set this index as one
                    was_code = true;

                    if (sub_index >= MAX_OPERANDS) {
                        printf("Error line %d: Too many operands\n", line_number + 1);
                        errors_found = true;
                        break;
                    }

                    if (sub_index == 0) {
                        mnemonic = buffer_index;
                    } else {
                        operands[sub_index - 1] = buffer_index;
                    }
                    sub_index++;
                }

                buffer[buffer_index++] = current;
                break;
            }
        }
    }

    free(buffer);

    return errors_found;
}

uint64_t* process_number(char* str, int line_num, bool can_be_register, bool* errors_found) {
    uint64_t value;

    if (str[0] == '0' && str[1] == 'x') { //Hexadecimal
        str += 2;
        str_to_upper(str);
          
        if (!is_hex(str)) {
            printf("Error line %d: Invalid hexadecimal number \"%s\"\n", line_num, str);
            *errors_found = true;
            return 0;
        }

        value = hex_str_to_num(str);
    } else if (str[0] == '0' && str[1] == 'b') { //Binary
        str += 2;
        str_to_upper(str);
            
        if (!is_binary(str)) {
            printf("Error line %d: Invalid binary number \"%s\"\n", line_num, str);
            *errors_found = true;
            return 0;
        }

        value = binary_str_to_num(str);
    } else { //Decimal
        if (!is_digits(str)) {
            if (can_be_register) {
                return 0;
            }

            printf("Error line %d: Invalid decimal number \"%s\"\n", line_num, str);
            *errors_found = true;
            return 0;
        }

        value = str_to_num(str);
    }

    uint64_t* return_value = (uint64_t*) malloc(sizeof(uint64_t));
    *return_value = value;
    return return_value;
}

operand_t process_operand(char* operand, int line_num, bool* errors_found) {
    operand_t return_op;

    if (operand[0] == '$') { //Operand is a label
        printf("Error line %d: Labels are not supported yet\n", line_num);
        *errors_found = true;
        return_op.type = UNDEFINED_OPERAND;
        return return_op;

        operand++;
        //return_op.type = ;

        str_to_upper(operand);

        //TODO: Support labels (position independent addresses must be made somehow)
    } else if (operand[0] == '\"' || operand[0] == '\'') { //Operand is a string
        printf("Error line %d: Strings are not yet supported\n", line_num);
        *errors_found = true;
        return_op.type = UNDEFINED_OPERAND;
        return return_op;

        char delim = operand[0];
        operand++;
        //return_op.type = STRING_OPERAND;

        char* end = find_string_end(operand, delim);
        if (!end) {
            printf("Error line %d: Unterminated string\n", line_num);
            *errors_found = true;
            return return_op;
        }

        int size = end - operand;
        char* string = (char*) malloc(size + 1);
        memcpy(string, operand, size);
        string[size] = 0;

        //TODO: Support strings (depends on labels else we can't get the address of the string)
    } else { //Operand is a register or an address
        uint64_t* address = process_number(operand, line_num, true, errors_found);
        if (address) {
            return_op.type = IMMEDIATE_OPERAND;
            return_op.size = sizeof(uint64_t);
            return_op.value = address;
            return return_op;
        }

        //We made sure that this isn't an address, so is has to be a register
        return_op.type = REGISTER_OPERAND;

        str_to_upper(operand);

        if (operand[0] == '[' && operand[strlen(operand) - 1] == ']') { //Register used as an address
            return_op.indirect = true;
            operand[strlen(operand) - 1] = 0;
            operand++;
        } else {
            return_op.indirect = false;
        }
        
        cpu_register_t reg = get_register(operand);
        if (reg.size == 0) {
            printf("Error line %d: Invalid register \"%s\"\n", line_num, operand);
            *errors_found = true;
            return return_op;
        }

        return_op.size = sizeof(cpu_register_t);
        return_op.value = malloc(sizeof(cpu_register_t));
        memcpy(return_op.value, &reg, sizeof(cpu_register_t));
    }

    return return_op;
}

bool parse_source_lvl2(lvl1_line_t** lines, int line_num, lvl2_line_t** lines_out) {
    bool errors_found = false;

    *lines_out = (lvl2_line_t*) malloc(sizeof(lvl2_line_t) * line_num);

    //Go through the split lines and process them to extract the operands and their types
    for (int i = 0; i < line_num; i++) {
        lvl1_line_t* line = &(*lines)[i];
        if (line->part_num < 1) {
            printf("Error: Lexer got empty line at %d for some reason!\n", line->line_num);
            errors_found = true;
        }

        str_to_upper(line->mnemonic);
        if (!is_letters(line->mnemonic)) {
            printf("Error line %d: Invalid mnemonic \"%s\"\n", line->line_num, line->mnemonic);
            errors_found = true;
        }

        //Process the operands one by one
        operand_t operands[MAX_OPERANDS];
        for (int j = 0; j < line->part_num - 1; j++) {
            operands[j] = process_operand(line->operands[j], line->line_num, &errors_found);
        }

        //Copy the processed line to the output
        lvl2_line_t* new_line = &(*lines_out)[i];
        new_line->line_num = line->line_num;

        int msize = strlen(line->mnemonic);
        new_line->mnemonic = (char*) malloc(msize + 1);
        memcpy(new_line->mnemonic, line->mnemonic, msize);
        new_line->mnemonic[msize] = 0;
        
        new_line->operand_num = line->part_num - 1;
        new_line->operands = (operand_t*) malloc(sizeof(operand_t) * line->part_num);
        memcpy(new_line->operands, &operands, sizeof(operand_t) * line->part_num);
    }

    return errors_found;
}

bool lex_source(lvl2_line_t** lines, int line_num, uint8_t** opcodes, int* opcode_num) {
    bool errors_found = false;

    for (int i = 0; i < line_num; i++) {
        lvl2_line_t* line = &(*lines)[i];

        uint8_t line_opcodes[16]; //An instruction can't be more than 4 bytes long, and then there are max 3 operands of 4 bytes each (4 + 12 = 16)
        int line_opcode_num = 0;

        if (strcmp(line->mnemonic, "NOP") == 0) {
            line_opcodes[line_opcode_num++] = 0x90;
        }
        check_mnemonic(INT)
        check_mnemonic(MOV)
        else {
            printf("Error line %d: Unknown mnemonic \"%s\"\n", line->line_num, line->mnemonic);
            #warning Uncomment this when the assembler is finished
            //errors_found = true;
        }
    
        //Append the opcodes to the output        
        if (line_opcode_num == 0) {
            printf("Error line %d: No opcode(s) generated\n", line->line_num);
            #warning Remove this debug line when the assembler is finished
            continue;
        }
        *opcodes = (uint8_t*) realloc(*opcodes, *opcode_num + line_opcode_num);
        memcpy((void*) ((uint64_t) *opcodes + *opcode_num), line_opcodes, line_opcode_num);
        *opcode_num += line_opcode_num;
    }

    return errors_found;
}

bool assemble(FILE* source, FILE* output, executable_format_t format) {
#ifdef FOXOS
    size_t file_size = source->size;
#else
    fseek(source, 0, SEEK_END);
    size_t file_size = ftell(source);
    fseek(source, 0, SEEK_SET); //Reset the file pointer
#endif

    char* source_buffer = (char*) malloc(file_size + 2);
    fread(source_buffer, file_size, 1, source);
    source_buffer[file_size] = '\n';
    source_buffer[file_size + 1] = '\0';

    int line_num = 0;

    //Level 1 parse
    lvl1_line_t* lines_lvl1 = 0;
    bool errors_found = parse_source_lvl1(source_buffer, &lines_lvl1, &line_num);
    if (errors_found) {
        return false;
    }

    free(source_buffer);

    //Level 2 parse
    lvl2_line_t* lines_lvl2 = 0;
    errors_found = parse_source_lvl2(&lines_lvl1, line_num, &lines_lvl2);
    if (errors_found) {
        return false;
    }

    for (int i = 0; i < line_num; i++) {
        free(lines_lvl1[i].buffer);
    }
    free(lines_lvl1);

    //Lex to produce opcodes
    uint8_t* opcodes = 0;
    int opcode_num = 0;
    errors_found = lex_source(&lines_lvl2, line_num, &opcodes, &opcode_num);
    if (errors_found) {
        return false;
    }

    for (int i = 0; i < line_num; i++) {
        free(lines_lvl2[i].mnemonic);
        for (int j = 0; j < lines_lvl2[i].operand_num - 1; j++) {
            free(lines_lvl2[i].operands[j].value);
        }
        free(lines_lvl2[i].operands);
    }
    free(lines_lvl2);

    if (opcodes == 0) {
        printf("Error: No code generated!\n");
        return false;
    }

    uint64_t entry_offset = 0; //TODO: Maybe add this?
    write_exec(format, output, opcodes, entry_offset, opcode_num);

    free(opcodes);

    return true;
}
