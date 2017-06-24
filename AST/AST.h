#ifndef _AST_H_
#define _AST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lexer/lexer.h"

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

typedef struct token_stack {
	LEX_TOKEN* val;
	int size;
	int capacity;
} token_stack;

//TODO вынеси универсвльный стэк в отдельную папку и переделай вё на него
typedef struct stack_stack {
	token_stack* val;
	int size;
	int capacity;
} stack_stack;

token_stack* build_AST(ALL_LEX_TOKENS*);
void print_stack(FILE*, token_stack*, int);

#endif
