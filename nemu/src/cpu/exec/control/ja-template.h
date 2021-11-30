#include "cpu/exec/template-start.h"

#define instr ja

static void do_execute(){
    DATA_TYPE_S dis = op_src->val;
    print_asm("ja %x", cpu.eip + 1 + DATA_BYTE + dis);
    if (cpu.ZF == 0 && cpu.CF == 0) cpu.eip += dis;
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"