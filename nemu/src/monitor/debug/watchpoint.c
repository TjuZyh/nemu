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
WP *new_wp(char *args){ // 新增一个监视点
	if(!free_)
		assert(0);
	
	WP* using_wp = free_;
	free_ = free_ -> next;
	
	using_wp -> type = 'w';
	using_wp -> using = 1;
	strcpy(using_wp->expression, args);
	bool success;
	using_wp -> value = expr(args, &success);
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

/*
void free_wp(WP* wp){ // 删除一个监视点
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
			qwq = qwq->next; 
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
*/

void free_wp(WP *wp){
	WP *q, *p;
	p = free_;
	if(p == NULL){
		free_ = wp;
		p = free_;
	}
	else{
		while(p->next != NULL)
			p = p->next;
		p -> next = wp;
	}
	q = head;
	if(head == NULL)assert(0);
	if(head->no == wp->no){
		head = head->next;
	}
	else{
		while(q->next->no != wp->no && q->next!=NULL){
			q=q->next;
		}
		if(q->next == NULL && q->no==wp->no)
			printf("Wrong!");
		else if(q->no == wp->no)
		 q->next = q->next->next;
		 else 
		 	assert(0);
	}
	wp->next = NULL;
	wp->value = 0;
	 wp->using = 0;
	wp->expression[0] = '\0';
}

void delete_wp(int n){
	WP *q;
	q=&wp_pool[n];
	free_wp(q);
}

WP* find_n(int n){
	if((n < 0) || (NR_WP <= n)) {
		printf("命令错误。\n"); 
		return 0;
	}
	if(wp_pool[n].using == 0) {
		printf("这个监视点未被使用。\n"); 
		return 0;
	}
	printf("%d号监视点删除成功。\n", n);
	return &wp_pool[n];
}

void info_w(){ // 打印监视点
	printf("No\tType\tValue\tExpression\n");
	int i;
	for(i = 0; i < NR_WP; ++i){
		if(wp_pool[i].using) 
			printf("%2d\t%c\t0x%x\t%s\n", wp_pool[i].no, wp_pool[i].type, wp_pool[i].value, wp_pool[i].expression);
	}
}