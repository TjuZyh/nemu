#include "cpu/exec/template-start.h"

#define instr jne

static void do_execute(){
	print_asm("jne %x",cpu.eip + 1 + DATA_BYTE + op_src->val);
	if(cpu.eflags.ZF == 0) cpu.eip += op_src->val;
}

make_instr_helper(si)

#include "cpu/exec/template-end.h"
