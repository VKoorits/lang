#ifndef _AST_H_
#define _AST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lexer/lexer.h"

#define START_TOKEN_STACK_SIZE (32)
#define START_OPERATION_STACK_SIZE (16)


typedef struct AST_root {
	void* branches;
	int cnt_branches;
} AST_root;


typedef struct AST_leaf{
	// ####	
} AST_leaf;

AST_root* build_AST(ALL_LEX_TOKENS*);

#endif
