#include <mnemonics.h>

//See Vol 2A chapter 3 page 516 of the Intel manual for more information

define_mnemonic(INT) {
    if (line->operand_num != 1) {
        throw_error("Only 1 operand is allowed");
    }

    operand_t* operand = &line->operands[0];
    if (operand->type != IMMEDIATE_OPERAND) {
        throw_error("The operand must be an immediate");
    }

    uint64_t immediate = *((uint64_t*) operand->value);
    if (immediate > UINT8_MAX) {
        throw_error("The immediate must be 8 bits (max 0xFF)");
    }

    append_opcode(0xCD);
    append_opcode((uint8_t) immediate);
}
