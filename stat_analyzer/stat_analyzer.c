#include "stat_analyzer.h"

/*
TODO проверить на существование все функции+-
TODO продумать устройство переменных
TODO проверить на существование все переменные

*/

stack_t* stat_analyze_recursive(FILE* out, stack_t* big_stack, hash_t* functions, stack_t* namespace, hash_t* std_functions) {	
	for(int i=0; i < big_stack->size; i++){
	#define TOKEN_I ((LEX_TOKEN*)big_stack->val[i])
		if( TOKEN_I->type == FUNC_CALL_TOKEN ){//проверка наличия вызываемой функции
			char* sub_name = ((LEX_TOKEN*)(((stack_t*)(TOKEN_I->token))->val[1]))->token;
			if( hash_get(functions, sub_name) ) {
				TOKEN_I->info = USER_FUNCTION;
			} else if ( hash_get(std_functions, sub_name) ) {
				TOKEN_I->info = COMPILED_FUNCTION;
			} else {
				fprintf(out, "ERROR: undeclared function '%s'\n", sub_name );
				return NULL;
			}
			
		} else if (//если TOKEN_I->token это тоже стек
			TOKEN_I->type > MIN_STACK_TOKEN && TOKEN_I->type < MAX_STACK_TOKEN
		  ) {
			//новый блок кодв
			if(TOKEN_I->type == TRUE_BODY_TOKEN || TOKEN_I->type == FALSE_BODY_TOKEN)
				st_push(namespace, hash_new() );
			
			stack_t* recursive_stack = (stack_t*)(TOKEN_I->token);			
			stack_t* res = stat_analyze_recursive(out, recursive_stack, functions, namespace, std_functions);
			if( !res ) return NULL;
			//выходим из блока кода
			if(TOKEN_I->type == TRUE_BODY_TOKEN || TOKEN_I->type == FALSE_BODY_TOKEN)
				hash_free( st_pop(namespace ))	;

		} else if(TOKEN_I->type == INIT_VAR_TOKEN){
			
			for(int j = 0; j < ((stack_t*)(TOKEN_I->token))->size; j++){
				if( ((LEX_TOKEN*)(((stack_t*)(TOKEN_I->token))->val[j]))->type == IDENT_TOKEN ){
					hash_t* this_space = (hash_t*)st_peek(namespace);
					char* var_name = ((LEX_TOKEN*)(((stack_t*)(TOKEN_I->token))->val[j]))->token;
					if( hash_get(this_space, var_name) ){
						fprintf(out, "ERROR: redefinition variable '%s'\n", var_name );
						return NULL;
					} else {
						hash_set( this_space, var_name, out );
					}				
				} else {
					stack_t* recursive_stack = (stack_t*)(((LEX_TOKEN*)(((stack_t*)(TOKEN_I->token))->val[j]))->token);

					stack_t* res = stat_analyze_recursive(out, recursive_stack, functions, namespace, std_functions);
					if( !res ) return NULL;
				}
			}
				
		} else if(TOKEN_I->type == IDENT_TOKEN && TOKEN_I->info == 0){
			//проверка на существование переменных
			int finded = 0;
			for(int j=namespace->size-1; j>=0; --j){
				if( hash_get(namespace->val[j], TOKEN_I->token) ){
					finded = 1;
					break;
				}
			}
			
			if(!finded) {
				fprintf(out, "ERROR: undeclared variable '%s'\n", TOKEN_I->token );
				return NULL;
			}
		}


	#undef TOKEN_I
	}
	return big_stack;
}

char* std_functions_lst[] = {};
static hash_t* make_std_function_hash() {
	hash_t* std = hash_new();
	int F = 1;
	for(int i = 0; i < sizeof(std_functions_lst)/sizeof(char*); i++) {
		hash_set(std, std_functions_lst[i], (void*)(i+1));
	}
	
	return std;
}

stack_t* stat_analyze(FILE* out, stack_t* big_stack, hash_t* functions) {
		stack_t* namespace = stack_new();
		st_push(namespace, hash_new());
		hash_t* std_func = make_std_function_hash();
		
		if( !stat_analyze_recursive(out, big_stack, functions, namespace, std_func) )
			return NULL;
			
		hash_each_val( functions, {
			if( !stat_analyze_recursive(out, &(((function_t*)val)->body), functions, namespace, std_func))
				return NULL;
		});
		return big_stack;
}

