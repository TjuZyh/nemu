#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].no = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP *new_wp(char *args){
	if(!free_)
		assert(0);
	
	WP* using_wp = free_;
	free_ = free_ -> next;
	
	using_wp -> type = 'w';
	using_wp -> using_value = 1;
	strcpy(using_wp->expression, args);
	bool success;
	using_wp -> old_value = expr(args, &success);
	using_wp -> next = NULL;

	if(!head)
		head = using_wp;
	else{
		WP *qwq = head;
		while(qwq->next) // qwq是上一个监视点
			qwq = qwq -> next;
		qwq -> next = using_wp;
	}
	printf("监视点序号为：%d\n", using_wp->no);
	return using_wp;
}

void free_wp(WP* wp){
	if(wp == 0)
		return;
	if(head == wp){ // wp是第一个监视点
		head = wp -> next;
		wp -> next = 0;
		if(!free_)
			free_ = wp;
		else{
			WP *qwq = free_;
			while(qwq->next)
				qwq = qwq -> next;  // qwq是当前最后一个free_的wp
			qwq -> next = wp;
		}
	}
	else{
		WP *qwq = head;
		while (qwq->next != wp) 
			qwq = qwq->next; 	// qwq->next == wp
		qwq->next = wp->next;
		wp->next = 0;
		if(!free_) 
			free_ = wp;
		else {
			qwq = free_;
			while (qwq->next) 
				qwq = qwq->next;  // qwq是当前最后一个free_的wp	
			qwq->next = wp;
		}
	}
}