#include <assembler.h>
#include <asmutils.h>
#include <regutils.h>
#include <fexec.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

void parse_source_lvl1(char* source, lvl1_line_t** lines, int* line_num) {
    char* buffer = malloc(4096);
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
            case ',':
                if (was_code) {
                    buffer[buffer_index++] = 0;
                    was_code = false;
                } else if (current == ',') {
                    printf("Error: Unexpected comma on line %d\n", line_number + 1);
                    exit(1);
                }
                if (current == ';') {
                    comment = true;
                }
                break;
            case '\n': //On a new line, we can add a lvl1_line_t to our list
                line_number++;

                comment = false;
                was_code = false;

                if (buffer_index == 0) {
                    break;
                }
                buffer_index++;

                *lines = realloc(*lines, sizeof(lvl1_line_t) * (*line_num + 1));
                char* tmp_buffer = malloc(buffer_index);
                memcpy(tmp_buffer, buffer, buffer_index);
                (*lines)[*line_num].buffer = tmp_buffer;
                (*lines)[*line_num].buffer_size = buffer_index;
                (*lines)[*line_num].line_num = line_number;
                (*lines)[*line_num].part_num = sub_index;
                (*lines)[*line_num].mnemonic = (char*) ((uint64_t) tmp_buffer + mnemonic);
                (*lines)[*line_num].operands = malloc(sizeof(char*) * sub_index);
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
            default: //By default, we just add the character to the buffer
                if (comment) {
                    break;
                }

                if (!was_code) { //If this is the start of a new operand, we need to set this index as one
                    was_code = true;

                    if (sub_index >= MAX_OPERANDS) {
                        printf("Error: Too many operands on line %d\n", line_number + 1);
                        exit(1);
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

    free(buffer);
}

uint64_t* process_number(char* str, int line_num, bool can_be_register) {
    uint64_t value;

    if (str[0] == '0' && str[1] == 'x') { //Hexadecimal
        str += 2;
        str_to_upper(str);
          
        if (!is_hex(str)) {
            printf("Error: Invalid hexadecimal number on line %d\n", line_num);
            exit(1);
        }

        value = hex_str_to_num(str);
    } else if (str[0] == '0' && str[1] == 'b') { //Binary
        str += 2;
        str_to_upper(str);
            
        if (!is_binary(str)) {
            printf("Error: Invalid binary number on line %d\n", line_num);
            exit(1);
        }

        value = binary_str_to_num(str);
    } else { //Decimal
        str_to_upper(str);

        if (!is_digits(str)) {
            if (can_be_register) {
                return 0;
            }

            printf("Error: Invalid decimal number on line %d\n", line_num);
            exit(1);
        }

        value = str_to_num(str);
    }

    uint64_t* return_value = malloc(sizeof(uint64_t));
    *return_value = value;
    return return_value;
}

register_t process_register(char* name, int line_num) {
    register_t return_reg = get_register(name);
    if (return_reg.size == 0) {
        printf("Error: Invalid register on line %d\n", line_num);
        exit(1);
    }

    return return_reg;
}

operand_t process_operand(char* operand, int line_num) {
    operand_t return_op;

    if (operand[0] == '$') { //Operand is a label
        printf("Error: Labels are not supported yet (line %d)!\n", line_num);
        exit(1);

        operand++;
        //return_op.type = ;

        str_to_upper(operand);

        //TODO: Support labels (position independent addresses must be made somehow)
    } else if (operand[0] == '#') { //Operand is a number
        operand++;
        return_op.type = IMMEDIATE_OPERAND;

        if (operand[0] == '-') {
            //TODO: Support negative numbers????

            printf("Error: Negative numbers are not supported (yet?) (line %d)!\n", line_num);
            exit(1);
        }

        return_op.size = sizeof(uint64_t);
        return_op.value = process_number(operand, line_num, false);
    } else if (operand[0] == '\"' || operand[0] == '\'') { //Operand is a string
        printf("Error: Strings are not yet supported on line %d\n", line_num);
        exit(1);

        char delim = operand[0];
        operand++;
        //return_op.type = STRING_OPERAND;

        char* end = find_string_end(operand, delim);
        if (!end) {
            printf("Error: Unterminated string on line %d\n", line_num);
            exit(1);
        }

        int size = end - operand;
        char* string = malloc(size + 1);
        memcpy(string, operand, size);
        string[size] = 0;

        //TODO: Support strings (depends on labels else we can't get the address of the string)
    } else { //Operand is a register or an address
        uint64_t* address = process_number(operand, line_num, true);
        if (address) {
            return_op.type = ADDRESS_OPERAND;
            return_op.size = sizeof(uint64_t);
            return_op.value = address;
            return return_op;
        }

        //We made sure that this isn't an address, so is has to be a register
        return_op.type = REGISTER_OPERAND;

        str_to_upper(operand);

        int size = strlen(operand);
        char* register_name = malloc(size + 1);
        memcpy(register_name, operand, size);
        register_name[size] = 0;
        
        register_t reg = process_register(register_name, line_num);
        free(register_name);

        return_op.size = sizeof(register_t);
        return_op.value = malloc(sizeof(register_t));
        memcpy(return_op.value, &reg, sizeof(register_t));
    }

    return return_op;
}

void parse_source_lvl2(lvl1_line_t** lines, int line_num, lvl2_line_t** lines_out) {
    *lines_out = malloc(sizeof(lvl2_line_t) * line_num);

    //Go through the split lines and process them to extract the operands and their types
    for (int i = 0; i < line_num; i++) {
        lvl1_line_t* line = &(*lines)[i];
        if (line->part_num < 1) {
            printf("Error: Lexer got empty line at %d for some reason!\n", line->line_num);
            exit(1);
        }

        str_to_upper(line->mnemonic);
        if (!is_letters(line->mnemonic)) {
            printf("Error: Invalid mnemonic on line %d\n", line->line_num);
            exit(1);
        }

        //Process the operands one by one
        operand_t operands[MAX_OPERANDS];
        for (int j = 0; j < line->part_num - 1; j++) {
            operands[j] = process_operand(line->operands[j], line->line_num);
        }

        //Copy the processed line to the output
        lvl2_line_t* new_line = &(*lines_out)[i];
        new_line->line_num = line->line_num;
        new_line->mnemonic = line->mnemonic;
        new_line->operand_num = line->part_num;
        new_line->operands = malloc(sizeof(operand_t) * line->part_num);
        memcpy(new_line->operands, operands, sizeof(operand_t) * line->part_num);
    }
}

void lex_source(lvl2_line_t** lines, int line_num, uint8_t** opcodes, int* opcode_num) {
    //TODO: Convert the operands to opcodes
}

void assemble(FILE* source, FILE* output) {
    char* source_buffer = malloc(source->size + 1);
    fread(source_buffer, source->size, 1, source);
    source_buffer[source->size] = '\n';

    int line_num = 0;

    //Level 1 parse
    lvl1_line_t* lines_lvl1 = 0;
    parse_source_lvl1(source_buffer, &lines_lvl1, &line_num);

    free(source_buffer);

    //Level 2 parse
    lvl2_line_t* lines_lvl2 = 0;
    parse_source_lvl2(&lines_lvl1, line_num, &lines_lvl2);

    for (int i = 0; i < line_num; i++) {
        free(lines_lvl1[i].buffer);
    }
    free(lines_lvl1);

    //--- START DEBUG ---
    for (int i = 0; i < line_num; i++) {
        lvl2_line_t* line = &lines_lvl2[i];
        printf("Line %d: M: \"%s\" ", line->line_num, line->mnemonic);
        for (int j = 0; j < line->operand_num - 1; j++) {
            operand_t* operand = &line->operands[j];
            printf("O%d: ", j);
            switch (operand->type) {
                case REGISTER_OPERAND:
                    register_t reg = *((register_t*) operand->value);
                    printf("REG (%d bytes) %d; ", reg.size, reg.code);
                    break;
                case IMMEDIATE_OPERAND:
                    printf("IMM \"%d\"; ", *((uint64_t*) operand->value));
                    break;
                case ADDRESS_OPERAND:
                    printf("ADR \"0x%x\"; ", *((uint64_t*) operand->value));
                    break;
            }
        }
        printf("\n");
    }
    //--- END DEBUG ---

    //Lex to produce opcodes
    uint8_t* opcodes = 0;
    int opcode_num = 0;
    lex_source(&lines_lvl2, line_num, &opcodes, &opcode_num);

    for (int i = 0; i < line_num; i++) {
        free(lines_lvl2[i].mnemonic);
        for (int j = 0; j < lines_lvl2[i].operand_num - 1; j++) {
            free(lines_lvl2[i].operands[j].value);
            free(&lines_lvl2[i].operands[j]);
        }
    }
    free(lines_lvl2);

    //Write the output
    fexec_header_t header;
    header.magic = FEXEC_MAGIC;
    header.version = 1;
    header.entry = 0; //TODO: Maybe add an entry label?
    header.size = opcode_num;

    fwrite(&header, sizeof(fexec_header_t), 1, output);
    fwrite(opcodes, opcode_num, 1, output);

    free(opcodes);
}
