#include "cpu/exec/template-start.h"

#define instr scas

make_helper(concat(scas_, SUFFIX)) {
    current_sreg = R_ES;
    uint32_t t2 = MEM_R(reg_l(R_EDI));
    uint32_t t1 = REG(R_EAX);

    uint32_t result = t1 - t2;

    if (cpu.eflags.DF == 0) reg_l(R_EDI) += DATA_BYTE;
    else reg_l(R_EDI) -= DATA_BYTE;

	/* TODO: Update EFLAGS. */
    cpu.eflags.ZF = !result;
    cpu.eflags.SF = result >> ((DATA_BYTE << 3) - 1);
    cpu.eflags.CF = (t1 < t2);
    int tmp1 = (t1) >> ((DATA_BYTE << 3) - 1);
    int tmp2 = (t2) >> ((DATA_BYTE << 3) - 1);
    cpu.eflags.OF = (tmp1 != tmp2 && tmp2 == cpu.eflags.SF);
    result ^= result >> 4;
    result ^= result >> 2;
    result ^= result >> 1;
    result &= 1;
    cpu.eflags.PF = !result;

	print_asm("scas");
    return 1;
}

#include "cpu/exec/template-end.h"