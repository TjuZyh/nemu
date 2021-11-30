#include "cpu/exec/template-start.h"

#define instr jg

static void do_execute(){
    DATA_TYPE_S dis = op_src->val;
    print_asm("jg %x", cpu.eip + 1 + DATA_BYTE + dis);
    if (cpu.ZF == 0 && cpu.SF == cpu.OF) cpu.eip += dis;
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"