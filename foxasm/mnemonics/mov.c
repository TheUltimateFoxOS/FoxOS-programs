#include <mnemonics.h>

//See Vol 2B chapter 4 page 35 of the Intel manual for more information

define_mnemonic(MOV) {
    if (line->operand_num != 2) {
        throw_error("Only 2 operands are allowed");
    }

    operand_t* op1 = &line->operands[0];
    operand_t* op2 = &line->operands[1];

    //Register -> register
    // mov rax, rbx
    if (op1->type == REGISTER_OPERAND && op1->indirect == false && op2->type == REGISTER_OPERAND && op2->indirect == false) {
        cpu_register_t* destinaton = (cpu_register_t*) op1->value;
        cpu_register_t* source = (cpu_register_t*) op2->value;

        if (destinaton->size != source->size) {
            throw_error("The size of the destination register must be the same as the size of the source register");
        }

        if (destinaton->size == 1) {
            append_opcode(0x88);
        } else if (destinaton->size == 2) {
            append_opcode(0x66);
            append_opcode(0x89);
        } else if (destinaton->size == 4) {
            append_opcode(0x89);
        } else {
            append_opcode(0x48);
            append_opcode(0x89);
        }

        //0xC0 is the mod field for a register to register move (0b11 << 6)
        append_opcode(0xC0 + get_two_register_magic(destinaton, source));
    }
    //Number -> register
    // mov rax, 0x12345678
    else if (op1->type == REGISTER_OPERAND && op1->indirect == false && op2->type == IMMEDIATE_OPERAND) {
        union register_value_t {
            uint64_t value;
            uint8_t bytes[8];
        };
        union register_value_t reg_val;
        reg_val.value = *((uint64_t*) op2->value);
        
        cpu_register_t* destinaton = (cpu_register_t*) op1->value;
        
        if (destinaton->size == 1) {
            if (reg_val.value > UINT8_MAX) {
                throw_error("The immediate must be 8 bits (max 0xFF) since the selected register is 8 bit");
            }
            append_opcode(0xB0 + destinaton->code);
            append_opcode(reg_val.bytes[0]);
        } else if (destinaton->size == 2) {
            if (reg_val.value > UINT16_MAX) {
                throw_error("The immediate must be 16 bits (max 0xFFFF) since the selected register is 16 bit");
            }
            append_opcode(0x66);
            append_opcode(0xB8 + destinaton->code);
            append_opcode(reg_val.bytes[0]);
            append_opcode(reg_val.bytes[1]);
        } else {
            int num_size = 4;
            if (reg_val.value > UINT32_MAX) {
                if (destinaton->size != 8) {
                    throw_error("The immediate must be 32 bits (max 0xFFFFFFFF) since the selected register is 32 bit");
                }

                num_size = 8;
                append_opcode(0x48);
            }

            append_opcode(0xB8 + destinaton->code);

            for (int i = 0; i < num_size; i++) {
                append_opcode(reg_val.bytes[i]);
            }
        }
    }
    //Number -> address in register
    // mov [rax], 0x12345678
    else if (op1->type == REGISTER_OPERAND && op1->indirect && op2->type == IMMEDIATE_OPERAND) {
        union register_value_t {
            uint64_t value;
            uint8_t bytes[8];
        };
        union register_value_t reg_val;
        reg_val.value = *((uint64_t*) op2->value);

        if (reg_val.value > UINT32_MAX) {
            throw_error("The immediate must be 32 bits (max 0xFFFFFFFF)");
        }

        cpu_register_t* destinaton = (cpu_register_t*) op1->value;
        if (destinaton->size == 1 || destinaton->size == 2) {
            throw_error("The destination register must be 32 or 64 bit");
        }

        #warning This may end up doing weird things if the rest of the register is not set to 0s
        if (reg_val.value <= UINT8_MAX) {
            if (destinaton->size == 4) {
                append_opcode(0x67);
            }
            append_opcode(0xC6);
            append_opcode(destinaton->code);
            append_opcode(reg_val.bytes[0]);
        } else if (reg_val.value <= UINT16_MAX) {
            append_opcode(0x66);
            if (destinaton->size == 4) {
                append_opcode(0x67);
            }
            append_opcode(0xC7);
            append_opcode(destinaton->code);
            append_opcode(reg_val.bytes[0]);
            append_opcode(reg_val.bytes[1]);
        } else {
            if (destinaton->size == 4) {
                append_opcode(0x67);
            }
            append_opcode(0xC7);
            append_opcode(destinaton->code);
            for (int i = 0; i < 4; i++) {
                append_opcode(reg_val.bytes[i]);
            }
        }
    }
    //Register -> address in register
    // mov [rax], rbx
    else if (op1->type == REGISTER_OPERAND && op1->indirect && op2->type == REGISTER_OPERAND && op2->indirect == false) {
        cpu_register_t* destinaton = (cpu_register_t*) op1->value;
        cpu_register_t* source = (cpu_register_t*) op2->value;

        if (destinaton->size == 1 || destinaton->size == 2) {
            throw_error("The destination register must be 32 or 64 bit");
        }

        if (source->size == 2) {
            append_opcode(0x66);
        }
        if (destinaton->size == 4) {
            append_opcode(0x67);
        }
        if (source->size == 8) {
            append_opcode(0x48);
        }

        if (source->size == 1) {
            append_opcode(0x88);
        } else {
            append_opcode(0x89);
        }

        //In this case, the mod field is 0 because the address is specified with only the register, with displacement
        append_opcode(get_two_register_magic(destinaton, source));
    }
    //Address in register -> register
    // mov rax, [rbx]
    else if (op1->type == REGISTER_OPERAND && op1->indirect == false && op2->type == REGISTER_OPERAND && op2->indirect) {
        cpu_register_t* destinaton = (cpu_register_t*) op1->value;
        cpu_register_t* source = (cpu_register_t*) op2->value;

        if (source->size == 1 || source->size == 2) {
            throw_error("The source register must be 32 or 64 bit");
        }

        if (destinaton->size == 2) {
            append_opcode(0x66);
        }
        if (source->size == 4) {
            append_opcode(0x67);
        }
        if (destinaton->size == 8) {
            append_opcode(0x48);
        }

        if (destinaton->size == 1) {
            append_opcode(0x8A);
        } else {
            append_opcode(0x8B);
        }

        //In this case, the mod field is 0 because the address is specified with only the register, with displacement
        append_opcode(get_two_register_magic(source, destinaton));
    } else {
        throw_error("Invalid operands, MOV instruction was not understood");
    }
}
