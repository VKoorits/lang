#ifndef _SYNTAX_H_
#define _SYNTAX_H_

#include <string.h>
#include "../lexer/lexer.h"

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



#endif
