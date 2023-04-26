#include <mnemonics.h>

//See Vol 2A chapter 4 page 695 of the Intel manual for more information

define_mnemonic(DB) {
    if (line->operand_num < 1) {
        throw_error("The DB instruction requires at least one operand");
    }

    for (int i = 0; i < line->operand_num; i++) {
        operand_t* operand = &line->operands[i];
        
        if (operand->type == IMMEDIATE_OPERAND) {
            union register_value_t {
                uint64_t value;
                uint8_t bytes[8];
            };
            union register_value_t reg_val;
            reg_val.value = *((uint64_t*) operand->value);

            int size;
            if (reg_val.value <= UINT8_MAX) {
                size = 1;
            } else if (reg_val.value <= UINT16_MAX) {
                size = 2;
            } else if (reg_val.value <= UINT32_MAX) {
                size = 4;
            } else {
                size = 8;
            }

            for (int i = 0; i < size; i++) {
                append_opcode(reg_val.bytes[i]);
            }
        } else if (operand->type == STRING_OPERAND) {
            char* string = (char*) operand->value;
            for (int i = 0; i < strlen(string); i++) {
                append_opcode(string[i]);
            }
        } else {
            throw_error("The DB instruction only takes immediate and string operands");
        }
    }
}
