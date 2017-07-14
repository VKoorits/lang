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
#define OPEN_BRACKET (101)
#define CLOSED_BRACKET (102)


stack_t* build_AST(ALL_LEX_TOKENS*, FILE*);
void print_stack(FILE*, stack_t*, int);

stack_t* generate_stack(stack_t*, LEX_TOKEN*, int(*)(LEX_TOKEN*), int);
int get_op_index(LEX_TOKEN*);

//END functions
int end_by_count(LEX_TOKEN*);
int end_by_bracket(LEX_TOKEN*);

#endif
