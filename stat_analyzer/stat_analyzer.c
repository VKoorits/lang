#include "stat_analyzer.h"

/*
TODO проверить на существование все функции
TODO продумать устройство переменных
TODO проверить на существование все переменные

*/

stack_t* stat_analyze(FILE* out, stack_t* big_stack, hash_t* functions) {	

	for(int i=0; i < big_stack->size; i++){
	#define TOKEN_I ((LEX_TOKEN*)big_stack->val[i])
		int recursion = 0;
		stack_t* recursive_stack = NULL;
		if( TOKEN_I->type == FUNC_CALL_TOKEN ){
			char* sub_name = ((LEX_TOKEN*)(((stack_t*)(TOKEN_I->token))->val[1]))->token;

			if( !hash_get(functions, sub_name) ){
				fprintf(out, "ERROR: undeclared function '%s'\n", sub_name );
				return NULL;
			}

			recursion = 1;
			recursive_stack = (stack_t*)(((LEX_TOKEN*)(((stack_t*)(TOKEN_I->token))->val[0]))->token);
		} else if (
			TOKEN_I->type > MIN_STACK_TOKEN && TOKEN_I->type < MAX_STACK_TOKEN
		  ) {
			recursion = 1;
			recursive_stack = (stack_t*)(TOKEN_I->token);

		}		


		if(recursion){
			stack_t* res = stat_analyze(out, recursive_stack, functions);
			if( !res) return NULL;
		}
	#undef TOKEN_I
	}
	return big_stack;
}
