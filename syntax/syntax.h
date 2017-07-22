#ifndef _SYNTAX_H_
#define _SYNTAX_H_

#include <string.h>
#include "../lexer/lexer.h"
#include "../data_struct/data_struct.h"

#define CNT_DEEP_WORD (4)
#define CNT_KEY_WORDS (6)
#define IF_ID (1)
#define ELSE_ID (2)
#define WHILE_ID (3)
#define FUNCTION_ID (4)
//#####################
#define VAR_ID (5)


int is_key_word(char*);

/*
приоритет операторов (от наибольшего к наименьшему)
	. (оператор доступа)
	in
	! not
	**
	* / // %
	+ -
	<< >>
	eq < <= >= >
	& ^
	|
	&& and ^^ xor
	|| or
	= += -= *= /= //= **= %= 
*/


int isop(char);
int is_char_operator(char* );
int get_op(int*, const int, char*, int*);


typedef struct function_s {
	stack_t head;
	stack_t body;
} function_t;



#endif
