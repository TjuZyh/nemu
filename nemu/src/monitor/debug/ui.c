#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "monitor/elf.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */ 
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){ // 单步执行
	char *arg = strtok(NULL, " "); // 以空格为分割符分割字符串，获取命令中的执行步数。
	int step; // 执行步数
	if(arg == NULL) // 输入为空，默认执行1步
		step = 1;
	else
		step = atoi(arg);
	cpu_exec(step);
	return 0;
}

static int cmd_info(char *args){ // 打印寄存器或监视点状态
	char *arg = strtok(NULL, " "); // 以空格为分割符分割字符串，获取命令中的r命令。
	if(arg == NULL || *arg == 'r'){
		int i;
		for(i = 0; i < 8; ++i)
			printf("%s: 0x%x 0x%x\n", regsl[i], cpu.gpr[i]._32, cpu.gpr[i]._16);
		printf("\n");
		for(i = 0; i < 4; ++i)
			printf("%s: 0x%x %s: 0x%x\n", regsb[i], cpu.gpr[i]._8[0], regsb[i+4], cpu.gpr[i]._8[1]);
		
		/*
	 	printf("eax is %x\n",cpu.eax);
        printf("ecx is %x\n",cpu.ecx);
        printf("edx is %x\n",cpu.edx);
        printf("ebx is %x\n",cpu.ebx);
        printf("esp is %x\n",cpu.esp);
        printf("ebp is %x\n",cpu.ebp); 
        printf("esi is %x\n",cpu.esi);
        printf("edi is %x\n",cpu.edi);
		*/
	}
	else if(*arg == 'w'){
		info_w();
	}
	return 0;
}

static int cmd_x(char *args){ // 扫描内存。命令格式 x N EXPR，N是扫描长度，EXPR是扫描的起始地址。
	char *N = strtok(NULL, " "); // 获得命令中的扫描长度
	char *EXPR = strtok(NULL, " "); // 获得命令中的扫描的起始地址
	int len = atoi(N);
	lnaddr_t address;
	sscanf(EXPR, "%x", &address);
	printf("0x%x: ", address);
	int i;
	for(i = 0; i < len; ++i){
		printf("%08x ", lnaddr_read(address, 4));
		address += 4;
	}
	printf("\n");
	return 0;
}

static int cmd_p(char *args){ // 表达式求值
	if(args == NULL)
		printf("未输入表达式。\n");
	bool flag = false;
	uint32_t res = expr(args, &flag);
	if(flag)
		printf("0x%x = %d\n", res, res);
	else
		printf("表达式错误。\n");
	return 0;
}

static int cmd_w(char *args){ // 增加监视点
	new_wp(args);
	return 0;
}

static int cmd_d(char *args){ // 删除监视点
	char* arg = strtok(NULL, " ");
	WP* w = find_n(atoi(arg));
	free_wp(w);
	return 0;
}

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	/* TODO: Add more commands */
	{ "si", "单步执行", cmd_si},
	{ "info", "打印寄存器或监视点状态", cmd_info}, // 输入命令 info r 打印寄存器 
										 // 输入命令 info w 打印监视点状态
	{ "x", "扫描内存", cmd_x},
	{ "p", "表达式求值", cmd_p},
	{ "w", "增加监视点", cmd_w},
	{ "d", "删除监视点", cmd_d},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
