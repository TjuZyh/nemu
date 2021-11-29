#include "cpu/exec/template-start.h"

#define instr dec

static void do_execute () {
	DATA_TYPE result = op_src->val - 1;
	OPERAND_W(op_src, result);

	/* TODO: Update EFLAGS. */
	cpu.eflags.ZF = !result;
	cpu.eflags.SF = result >> ((DATA_BYTE << 3) - 1);
	cpu.eflags.CF = op_src -> val < 1;
	int tmp1 = (op_src -> val) >> ((DATA_BYTE << 3) - 1);
	int tmp2 = 0;
	cpu.eflags.OF = (tmp1 != tmp2 && tmp2 == cpu.eflags.SF);
	result ^= result >> 4;
	result ^= result >> 2;
	result ^= result >> 1;
	result &= 1;
	cpu.eflags.PF = !result;

	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
