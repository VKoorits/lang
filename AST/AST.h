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

typedef struct AST_root {
	void* branches;
	int cnt_branches;
} AST_root;


typedef struct AST_leaf{
	// ####	
} AST_leaf;

AST_root* build_AST(ALL_LEX_TOKENS*);

#endif
