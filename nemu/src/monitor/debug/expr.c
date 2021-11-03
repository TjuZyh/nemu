#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
	NOTYPE = 256,
	REG = 400,
	Number = 301,
	Hex = 300,
	EQ = 80,
	NEQ = 81,
	AND = 82,
	OR = 201,
	NEG = 50,
	DEREF = 51

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +", NOTYPE}, // spaces

	{"\\$[a-z]{2,3}", REG}, //register name
	//{"[a-zA-Z0-9_]+", ID},			//identifier

	{"[0-9]{1,10}", Number},   // adecimal number
	{"0x[0-9,a-f]{1,8}", Hex}, // hexadecimal number

	{"\\+", '+'}, // plus
	{"\\-", '-'}, // minus
	{"\\*", '*'}, // multipy
	{"/", '/'},	  // divison
	{"\\(", '('}, // left parenthesis
	{"\\)", ')'}, // right parenthesis

	{"==", EQ},	 // equal
	{"!=", NEQ}, // not equal
	{"&&", AND}, // logical and
	{"||", OR},	 // logical or
	{"!", '!'},	 // logical not
				 //{"\\%", '%'},            		//mod
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				if (substr_len > 32)
					assert(0);
				switch (rules[i].token_type)
				{
				case 80:
					tokens[nr_token++].type = EQ;
					break;
				case 81:
					tokens[nr_token++].type = NEQ;
					break;
				case 256: //空格
					break;
				case '+': //+
					tokens[nr_token++].type = '+';
					break;
				case '-': //-
					tokens[nr_token++].type = '-';
					break;
				case '*': //*
					tokens[nr_token++].type = '*';
					break;
				case '/': // /
					tokens[nr_token++].type = '/';
					break;
				case '(': //左括号
					tokens[nr_token++].type = '(';
					break;
				case ')': //右括号
					tokens[nr_token++].type = ')';
					break;
				case 200:
					tokens[nr_token++].type = 200;
					break;
				case 201:
					tokens[nr_token++].type = 201;
					break;
				case '!':
					tokens[nr_token++].type = '!';
					break;
				case 300: //HEXEXP
					tokens[nr_token].type = 300;
					init_str(nr_token);
					strncpy(tokens[nr_token].str, substr_start, substr_len);
					tokens[nr_token].str[substr_len] = '\0';
					nr_token++;
					break;
				case 301: //NUMBER
					tokens[nr_token].type = 301;
					init_str(nr_token);
					strncpy(tokens[nr_token].str, substr_start, substr_len);
					tokens[nr_token].str[substr_len] = '\0';
					nr_token++;
					break;
				case 400: //REG
					tokens[nr_token].type = 400;
					init_str(nr_token);
					strncpy(tokens[nr_token].str, substr_start, substr_len);
					tokens[nr_token].str[substr_len] = '\0';
					nr_token++;
					break;
				default:
					panic("please implement me");
				}

				switch(rules[i].token_type) {
					default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

//匹配
int check_parentheses(int p, int q)
{
	char stack[40];
	int i = p;
	int top = 0;
	bool flag = 0;
	for (i = p; i <= q; i++)
	{
		if (tokens[i].type == '(')
		{
			if (top == 0)
			{
				if (i != p)
					flag = 1; //合法
			}
			stack[top++] = '(';
		}
		else if (tokens[i].type == ')')
		{
			if (top == 0)
				return 0;
			else
				top--;
		}
		else
			continue;
	}
	if (top != 0) //栈未清空不合法
		return 0;
	//下面都是合法
	if (flag == 1)
		return 1;
	if (tokens[p].type != '(' || tokens[q].type != ')')
		return 1;
	return 2;
}

int dominant_operator(int p, int q)
{
	int level[220];
	level['!'] = 1;
	level['+'] = level['-'] = 3;
	level['*'] = level['/'] = 2;
	level[AND] = 5;
	level[OR] = 6;
	level[NEG] = 1;
	level[DEREF] = 1;
	level[EQ] = level[NEQ] = 4;
	int i, maxx = 0;
	int pos = p;
	//如果左括号多于右括号，说明tokens[i]出现在某一对括号中
	int lp = 0, rp = 0;
	for (i = p; i <= q; i++)
	{
		//是数字一定不是运算符
		if (tokens[i].type == Hex || tokens[i].type == Number || tokens[i].type == REG)
			continue;
		//括号也不算是运算符，但要参与括号的计数
		if (tokens[i].type == '(')
		{
			lp++;
			continue;
		}
		if (tokens[i].type == ')')
		{
			rp++;
			continue;
		}
		//如果是！的话，由于它是单目运算符，它最后一定是！x这样的形式出现，那么主运算符就是第一个token串
		if (tokens[i].type == '!' || tokens[i].type == NEG || tokens[i].type == DEREF)
			continue;
		//运算符
		if (lp > rp)
			continue; //被括号包围的运算符不是主运算符
		else
		{
			if (level[tokens[i].type] >= maxx)
			{
				maxx = level[tokens[i].type];
				pos = i;
			}
		}
	}
	return pos;
}

int eval(int p, int q, bool *success)
{
	char reg_32[8][4] = {"eax", "edx", "ecx", "ebx", "ebp", "esi", "edi", "esp"};
	char reg_16[8][3] = {"ax", "dx", "cx", "bx", "bp", "si", "di", "sp"};
	char reg_8_high[4][3] = {"ah", "dh", "ch", "bh"};
	char reg_8_low[4][3] = {"al", "dl", "cl", "bl"};
	*success = true;
	if (p > q) //错误表达式，单目运算符出现后会有这种情况产生
	{
		*success = false;
		return 0;
	}
	else if (p == q) //数字
	{
		if (tokens[p].type == 301)
		{
			//将字符串转化为int型
			return atoi(tokens[p].str);
		}

		else if (tokens[p].type == 300)
		{
			//代表将str转化为16进制的长整型，当指针指向NULL时（即指向str的末尾）停止读取字符
			return strtol(tokens[p].str, NULL, 16);
		}

		else if (tokens[p].type == 400)
		{
			int i;
			for (i = 0; i < 4; i++)
			{
				if (strcmp(tokens[p].str + 1, reg_8_high[i]) == 0)
					return cpu.gpr[i]._8[0];
				if (strcmp(tokens[p].str + 1, reg_8_low[i]) == 0)
					return cpu.gpr[i]._8[1];
			}

			for (i = 0; i < 8; i++)
			{
				if (strcmp(tokens[p].str + 1, reg_32[i]) == 0)
					return cpu.gpr[i]._32;
				if (strcmp(tokens[p].str + 1, reg_16[i]) == 0)
					return cpu.gpr[i]._16;
			}
			if (strcmp(tokens[p].str + 1, "eip") == 0)
			{
				return cpu.eip;
			}
		}
		*success = false;
		return 0;
	}
	else if (check_parentheses(p, q) == 2)
	{ //是要找的情况
		return eval(p + 1, q - 1, success);
	}
	else if (check_parentheses(p, q) == 0)
	{ //非法
		*success = false;
		return 0;
	}
	else
	{ //不是要找的情况，需要继续拆分子表达式
		int op = dominant_operator(p, q);
		if (op > p)
		{
			int val1 = eval(p, op - 1, success);
			int val2 = eval(op + 1, q, success);
			switch (tokens[op].type)
			{
			case '+':
				return val1 + val2;
			case '-':
				return val1 - val2;
			case '*':
				return val1 * val2;
			case '/':
				return val1 / val2;
			case EQ:
				return val1 == val2;
			case NEQ:
				return val1 != val2;
			case AND:
				return val1 && val2;
			case OR:
				return val1 || val2;
			default:
				assert(0);
			}
		}
		else if (op == p)
		{
			int val2 = eval(op + 1, q, success);
			switch (tokens[op].type)
			{
			case '!':
				return !(val2);
			case NEG:
				return -1 * val2;
			case DEREF:
				return swaddr_read(val2, 4);
			default:
				assert(0);
			}
		}
		else
		{
			*success = false;
			return 0;
		}
	}
	*success = false;
	return 0;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	int j;
	if (tokens[0].type == '-')
		tokens[0].type = NEG;
	for (j = 1; j < nr_token; j++)
	{
		if (tokens[j].type == '-' && tokens[j - 1].type != Hex && tokens[j - 1].type != Number && tokens[j - 1].type != REG && tokens[j - 1].type != ')')
			tokens[j].type = NEG;
	}

	if (tokens[0].type == '*')
		tokens[0].type = DEREF;
	for (j = 1; j < nr_token; j++)
	{
		if (tokens[j].type == '*' && tokens[j - 1].type != Hex && tokens[j - 1].type != Number && tokens[j - 1].type != REG && tokens[j - 1].type != ')')
			tokens[j].type = DEREF;
	}

	bool success_eval = true;
	eval(0, nr_token - 1, &success_eval);
	if (success_eval == true)
	{
		*success = true;
		return eval(0, nr_token - 1, &success_eval);
	}

	panic("please implement me");
	return 0;
}

