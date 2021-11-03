#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
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

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args){
    if(args ==NULL)return 0;
    bool suc = true;
    uint32_t res = expr(args,&suc);
    if(suc==false)return 0;
    printf(" %d : 0x%x\n",res,res);
    return 0;
}

static int cmd_w(char *args){
	if(args==NULL)
	{
		printf("Error, arguments are not complete!\n");
		return 0;
	}
	WP* tmp=NULL;
	tmp=new_wp(args);
	if(tmp==0)
	{
		printf("Error, wrong expression!\n");
		return 0;
	}
	if(tmp==NULL)
	{
		printf("Sorry, no more watchpoints can be set!\n");
		return 0;
	}
	printf("Watchpoint set successfully!\n");
	return 0;
}

static int cmd_d(char *args){
	char *arg=strtok(NULL," ");
	if(arg==NULL)
	{
		printf("Error, arguments are not complete!\n");
		return 0;
	}
    
	int n;
	sscanf(arg,"%d",&n);
	WP* test=find_n(n);
	if(test)free_wp(test);

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
	{ "si", "Execute the sequence", cmd_si},
	{ "info", "Print SUBCMD info", cmd_info},
	{ "x", "Scan the memory", cmd_x},
	{ "p", "Evaluate the expression", cmd_p},
	{ "w", "Set a monitor point", cmd_w},
	{ "d", "Delete monitor point", cmd_d},

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

static int cmd_si(char *args){
	char *arg = strtok(NULL, " ");
	int step = 1;

	if(arg == NULL) {
		// no argument given
	}
	else {
		sscanf(args, "%d", &step);
	}
	printf("Execute %d sequence\n", step);
	cpu_exec(step);
	return 0;
}

static int cmd_info(char *args){
	char *arg = strtok(NULL, " ");
	char cmd = 1;

	if(arg == NULL) {
		// no argument given
		printf("the number of subcom is not true\n");
	}
	else {
		sscanf(args, "%s", &cmd);
		if(cmd == 'r'){
			printf("Print register status\n");
			int i;
			for(i = R_EAX; i <= R_EDI; i++){
				printf("$%s\t0x%08x\n",regsl[i],reg_l(i));
			}
			printf("$eip\t0x%08x\n",cpu.eip);
		}
		else if(cmd == 'w'){
			printf("Print monitoring point information\n");
			info_w();
		}
	}
	return 0;
}

static int cmd_x(char *args){
	char *arg = strtok(NULL, " ");
	if(arg == NULL){
		printf("the number of subcom is not true\n");
		return 0;
	}
	int n;
	lnaddr_t exprs;
	//sscanf(args, "%d%x", &n, &exprs);
	sscanf(arg, "%d", &n);  
    sscanf(arg, "%x", &exprs);  
	int i;
	
	for(i = 0; i < n; i++){
		if(i % 4 == 0)
			printf("0x%08x", exprs + i*4);
		printf("   0x%08x", swaddr_read(exprs + i*4, 4));
		if((i+1) % 4 == 0) 
			printf("\n");	
	}
	if((i+1) % 8) printf("\n");
	return 0;
	/*
	for(i = 0; i < n; i++){
		printf("0x%08x   0x%08x\n", exprs + i*4, swaddr_read(exprs + i*4, 4));
	}
	*/
	
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
