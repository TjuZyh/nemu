#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_i_, SUFFIX)) {
    // 根据SUFFIX进行译码，len为取出来的参数的字节数
	int len = concat(decode_i_, SUFFIX)(cpu.eip + 1);

    // 堆栈操作
    reg_l(R_ESP) -= DATA_BYTE;
    swaddr_write(reg_l(R_ESP), 4, cpu.eip + len + 1);

    // imm为偏移量的值
    DATA_TYPE_S imm = op_src -> val;

    // 打印指令
    print_asm("call\t%x",cpu.eip + 1 + len + imm);
    
    // 当前地址eip加上偏移量
    cpu.eip += imm;

    return len + 1;
} 

/*
make_helper(concat(call_rm_, SUFFIX)){
    int len = concat(decode_rm_, SUFFIX)(cpu.eip + 1);
	reg_l(R_ESP) -= DATA_BYTE;
	swaddr_write(reg_l(R_ESP) , 4, cpu.eip + len + 1);
	DATA_TYPE_S imm = op_src -> val;
	print_asm("call %x",imm);
	cpu.eip = imm - len - 1;
	return len + 1;
}
*/


#include "cpu/exec/template-end.h"