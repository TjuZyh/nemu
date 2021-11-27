#include "cpu/exec/template-start.h"

#define instr xor

static void do_execute () {
	DATA_TYPE result = op_dest->val ^ op_src->val;
	OPERAND_W(op_dest, result);

	/* TODO: Update EFLAGS. */
	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	cpu.eflags.ZF = !result;
    cpu.eflags.SF = result >> ((DATA_BYTE << 3) - 1);
	result ^= result >> 4;
    result ^= result >> 2;
    result ^= result >> 1;
    result &= 1;
    cpu.eflags.PF = !result;

	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
