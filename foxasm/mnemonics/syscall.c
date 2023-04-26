#include <mnemonics.h>

//See Vol 2A chapter 4 page 695 of the Intel manual for more information

define_mnemonic(SYSCALL) {
    if (line->operand_num != 0) {
        throw_error("The SYSCALL instruction does not take any operands");
    }

    append_opcode(0x0F);
    append_opcode(0x05);
}
