#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute(){
    DATA_TYPE ret = op_dest->val - op_src->val;
    int len = (DATA_BYTE << 3)-1;
    cpu.CF = op_dest->val < op_src->val;
    cpu.SF = ret >> len;
    cpu.OF = ((op_dest->val >> len) != (op_src->val >> len)) && ((op_src->val >>len) == cpu.SF);
    cpu.ZF = !ret;
    OPERAND_W(op_dest, ret);
    ret ^= ret>>4;
    ret ^= ret>>2;
    ret ^= ret>>1;
    cpu.PF = !(ret & 1);
    print_asm_template2();
}

#if DATA_BYTE ==2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"