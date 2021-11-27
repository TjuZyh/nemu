#include "cpu/exec/template-start.h"

#define instr test

static void do_execute () {
	DATA_TYPE result = op_dest -> val & op_src -> val;
    cpu.eflags.SF = result >> ((DATA_BYTE << 3) - 1);
    cpu.eflags.ZF = !result;
    cpu.eflags.CF = 0;
    cpu.eflags.OF = 0;

    result ^= result >> 4;
    result ^= result >> 2;
    result ^= result >> 1;

    result &= 1;
    cpu.eflags.SF = !result;
    print_asm_template2();
}

make_instr_helper(r2rm)
make_instr_helper(i2rm)
make_instr_helper(i2a)

#include "cpu/exec/template-end.h"
