#include "cpu/exec/template-start.h"

#define instr sbb

static void do_execute(){
    DATA_TYPE src = op_src->val;
    if (op_src->size == 1 && op_dest->size != 1){
        op_src->val = (int8_t)op_src->val;
    }
    src += cpu.CF;
    DATA_TYPE ret = op_dest->val - src;
    int len = (DATA_BYTE << 3)-1;
    cpu.CF = op_dest->val < src;
    cpu.SF = ret >> len;
    int f1 = op_dest->val>>len;
    int f2 = src>>len;
    cpu.OF = (f1 != f2 && f2 == cpu.SF);
    cpu.ZF = !ret;
    OPERAND_W(op_dest, ret);
    ret ^= ret>>4;
    ret ^= ret>>2;
    ret ^= ret>>1;
    cpu.PF = !(ret & 1);
    print_asm_template2();
}

#if DATA_BYTE == 2|| DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"