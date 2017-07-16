#ifndef _AST_H_
#define _AST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lexer/lexer.h"
#include "../data_struct/data_struct.h"

#define START_DEEP (8)

#define START_TOKEN_STACK_SIZE (32)
#define START_OPERATION_STACK_SIZE (16)
#define START_EXPR_STACK_SIZE (8)
#define START_DEEP_STACK_SIZE (8)

// WHERE
#define IN_SPACE	(1)
#define IN_EXPR 	(2)
#define IN_TRUE		(3)
#define IN_FALSE	(4)

#define EMPTY (100)
//BASE_BRACKET > 6 !
// см функцию is_bracket (to_postfix.c)
#define NUM_MORE_CNT_BRACKET (10)
#define BASE_BRACKET (100) 
#define OPEN_ROUND_BRACKET (101)
#define OPEN_SQUARE_BRACKET (102)
#define OPEN_FIGURE_BRACKET (103)
#define CLOSED_ROUND_BRACKET (104)
#define CLOSED_SQUARE_BRACKET (105)
#define CLOSED_FIGURE_BRACKET (106)


stack_t* build_AST(ALL_LEX_TOKENS*, FILE*);
void print_stack(FILE*, stack_t*, int);

stack_t* generate_stack(FILE* out, stack_t*, LEX_TOKEN*, int(*)(LEX_TOKEN*), int);
int get_op_index(LEX_TOKEN*);

//END functions
int end_by_count(LEX_TOKEN*);
int end_by_round_bracket(LEX_TOKEN*);
int end_by_square_bracket(LEX_TOKEN*);
int end_by_figure_bracket(LEX_TOKEN*);

#endif
