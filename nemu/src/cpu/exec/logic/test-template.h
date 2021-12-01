#include "cpu/exec/template-start.h"

#define instr test

static void do_execute(){
    DATA_TYPE ret = op_dest->val & op_src->val;
    int len = (DATA_BYTE << 3) - 1;
    cpu.CF = 0;
    cpu.OF = 0;
    cpu.SF = ret >> len;
    cpu.ZF = !ret;
    ret ^= ret>>4;
    ret ^= ret>>2;
    ret ^= ret>>1;
    cpu.PF = !(ret & 1);
    print_asm_template2();
}

make_instr_helper(i2a);
make_instr_helper(i2rm);
make_instr_helper(r2rm);

#include "cpu/exec/template-end.h"