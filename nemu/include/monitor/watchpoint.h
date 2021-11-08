#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	char type; // w 或者 b
	int using_value; // 正在使用的值
	int old_value; // 旧值
	int new_value; // 新值
	char expression[32]; // 被监视的表达式

	/* TODO: Add more members if necessary */


} WP;

int change();
void info_w();
WP* new_wp(char *args);
void free_wp(WP* wp);
WP* find_wp(int n);

#endif
