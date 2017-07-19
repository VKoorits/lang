#ifndef _SYNTAX_H_
#define _SYNTAX_H_

#include <string.h>
#include "../lexer/lexer.h"
#include "../data_struct/data_struct.h"

#define CNT_DEEP_WORD (4)
#define IF_ID (1)
#define ELSE_ID (2)
#define WHILE_ID (3)
#define FUNCTION_ID (4)


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



int deep_word(char* );
int isop(char);
int is_char_operator(char* );
int get_op(int*, const int, char*, int*);


//TODO? сделать фуекцию структурой данных, вынести в отдельный файл и сделать методы
typedef struct function_s {
	stack_t head;
	stack_t body;
} function_t;



#endif
