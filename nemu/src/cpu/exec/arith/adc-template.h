#include "cpu/exec/template-start.h"

#define instr adc

static void do_execute () {
	DATA_TYPE result = op_dest -> val + op_src -> val + cpu.eflags.CF;
	OPERAND_W(op_dest, result);

	/* TODO: Update EFLAGS. */
    cpu.eflags.ZF = !result;
    cpu.eflags.SF = result >> ((DATA_BYTE << 3) - 1);
    cpu.eflags.CF = (result < op_dest -> val);
    int tmp1 = (op_dest -> val) >> ((DATA_BYTE << 3) - 1);
    int tmp2 = (op_src -> val) >> ((DATA_BYTE << 3) - 1);
    cpu.eflags.OF = (tmp1 == tmp2 && tmp1 != cpu.eflags.SF);
    result ^= result >> 4;
    result ^= result >> 2;
    result ^= result >> 1;
    result &= 1;
    cpu.eflags.PF = !result;
	print_asm_template2();
}

make_instr_helper(i2a);
make_instr_helper(i2rm);
make_instr_helper(r2rm);
make_instr_helper(rm2r);

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

#include "cpu/exec/template-end.h"
