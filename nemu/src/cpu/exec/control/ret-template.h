#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_n_, SUFFIX)){
    cpu.eip = MEM_R(reg_l(R_ESP));
    reg_l(R_ESP) += DATA_BYTE;
    print_asm("ret");
    return 1;
}

make_helper(concat(ret_i_, SUFFIX)){
    int val = instr_fetch(cpu.eip + 1, 2);
    cpu.eip =MEM_R(REG(R_ESP));
    int i;
    if (DATA_BYTE == 2) cpu.eip &= 0xffff;
    REG(R_ESP) += DATA_BYTE;
    for (i = 0; i < val; i+=DATA_BYTE)
        MEM_W(REG(R_ESP)+i, 0);
    REG(R_ESP) += val;
    print_asm("ret %x", val);
    return 1;
}

#include "cpu/exec/template-end.h"