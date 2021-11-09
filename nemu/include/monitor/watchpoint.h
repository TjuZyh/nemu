#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int no;
	struct watchpoint *next;
	/* TODO: Add more members if necessary */
	char type; // w 或者 b
	int using; // 正在使用的值
	int value; // 旧值
	int new_value; // 新值
	char expression[32]; // 被监视的表达式
} WP;

int change();
void info_w();
WP* new_wp(char *args);
void free_wp(WP* wp);
void delete_wp(int n);
WP* find_n(int n);

#endif
