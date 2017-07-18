#include "AST.h"
//DEBUG
#define IF if(read_list)
extern int end_by_count_cnt;
extern int end_by_bracket_deep;
char brackets_char[6];


int get_condition(int last_code, int this_code) {

	if( last_code == EMPTY ) {
		if(this_code >= CLOSED_ROUND_BRACKET && this_code <= CLOSED_FIGURE_BRACKET)
			return 5;
		else return 1;
	} else if (last_code >= OPEN_ROUND_BRACKET && last_code <= OPEN_FIGURE_BRACKET) {
		if( this_code == EMPTY )
			return 5;
		else if(this_code >= CLOSED_ROUND_BRACKET && this_code <= CLOSED_FIGURE_BRACKET)
			if( last_code % BASE_BRACKET % 3 == this_code % BASE_BRACKET % 3 )
				return 3;
			else 
				return -1*(
					 (last_code % BASE_BRACKET % 3)
					*NUM_MORE_CNT_BRACKET
					+ this_code % BASE_BRACKET % 3
				);
		else return 1;
	}

	if (last_code >= OPEN_ROUND_BRACKET && last_code <= OPEN_FIGURE_BRACKET)
		return 1;
	else if(this_code >= CLOSED_ROUND_BRACKET && this_code <= CLOSED_FIGURE_BRACKET)
		return 2;


	if ( last_code > this_code )
		return 2;
	else return 1;
}

char* analyze(stack_t*, stack_t*, LEX_TOKEN*);

char* func1(stack_t* Moscow, stack_t* Kiev, LEX_TOKEN* token){
	st_push(Moscow, token);
	return NULL;
}
char* func2(stack_t* Moscow, stack_t* Kiev, LEX_TOKEN* token){
	st_push(Kiev, st_pop(Moscow));
	return analyze(Moscow, Kiev, token);
}
char* func3(stack_t* Moscow, stack_t* Kiev, LEX_TOKEN* token){
	st_pop(Moscow);
	return NULL;
}
char* func4(stack_t* Moscow, stack_t* Kiev, LEX_TOKEN* token){
	return NULL;
}
char* func5(stack_t* Moscow, stack_t* Kiev, LEX_TOKEN* token){
	char *template = "ERROR: no opened bracket for '%s'\n";
	char* error_msg = malloc( strlen(template) + strlen( token->token ) );
	sprintf(error_msg, template, token->token);
	return error_msg;
}

static char* (*functions[5])(stack_t*, stack_t*, LEX_TOKEN*) = 
								{ func1, func2, func3, func4, func5 };

int is_bracket(LEX_TOKEN*);
int get_op_index(LEX_TOKEN* top) {
	int index = EMPTY;
	if(top) {
		index = top->info;
		if(index == -1){
			int type = is_bracket(top);
			if(type)
				index = BASE_BRACKET + type;
		}
	}
	return index;
}

char* analyze(stack_t* op_stack, stack_t* val_stack, LEX_TOKEN* token){
	int last_code = get_op_index( st_peek(op_stack) );
	int this_code = get_op_index( token );

	int variant = get_condition(last_code, this_code);
	if( variant > 0) 
		return functions[variant-1](op_stack, val_stack, token);
	else {
		char* template = "ERROR: wrong brackets sequence:\n\texpected: '%c'\n\tgot:      '%c'\n";
		char* error_msg = malloc( strlen(template) );
		char expected = brackets_char[ 2 + (int)(-variant/NUM_MORE_CNT_BRACKET)];
		char got = brackets_char[ 2 + (int)(-variant%NUM_MORE_CNT_BRACKET)];
		sprintf(error_msg, template, expected, got);
		return error_msg;
	}
}


LEX_TOKEN* create_token_stack(int token_type) {
	LEX_TOKEN* tok = malloc( sizeof(LEX_TOKEN) );
	tok->token = (char*)stack_new();
	tok->type = token_type;
	return tok;
}

int is_bracket(LEX_TOKEN* token){
	if( token->type == OPERATION_TOKEN ) {
		switch(token->token[0]){
			case '(': return 1;
			case '[': return 2;
			case '{': return 3;
			case ')': return 4;
			case ']': return 5;
			case '}': return 6;
		}
	}
	return 0;
}


int end_Polish_expr(stack_t* val_stack, stack_t* op_stack){
	while( st_peek(op_stack) ) {
			int ind = get_op_index(st_peek(op_stack));
			if( ind >= EQUAL && ind <= DOT){
				//print_stack(stdout, op_stack, 0);
				st_push(val_stack, st_pop(op_stack));
			} else
				break;
		}
	if( op_stack->size )
		return 0;
	
	return 1;
}



// https://master.virmandy.net/perevod-iz-infiksnoy-notatsii-v-postfiksnuyu-obratnaya-polskaya-zapis/
stack_t* generate_stack_recursive(FILE* out, stack_t* val_stack, LEX_TOKEN* tokens,
						int(*NOT_END)(LEX_TOKEN*), int Polish, int* delta, int read_list){
	
	
	stack_t* op_stack = stack_new();
	stack_t* brackets_stack = stack_new();
	
	stack_t* for_push_val_stack = val_stack;
	if ( read_list )
		for_push_val_stack = (stack_t*)( ((LEX_TOKEN*)st_peek(val_stack))->token );


	int k = 0;
		if(Polish) {
		int last_type = UNKNOWN_TOKEN;
		while(  NOT_END( tokens + k ) > 0 ){
			

			//BEGIN не удалять, полезно при отладке
			IFD printf("token => %s\n read_list => %d\t", tokens[k].token, read_list);
			IFD print_stack(stdout, op_stack, 0);
			//print_stack(stdout, val_stack, 0);
			//printf("********************\n");*/
			
			
			//END
			//DEBUG !read_list &&
			if( !strcmp( tokens[k].token, "(") ){
				
				if( last_type == IDENT_TOKEN) { //вызов функции: 'print (...'		
					LEX_TOKEN* func_args = malloc( sizeof(LEX_TOKEN) );
					LEX_TOKEN* name_func = st_pop(val_stack);	
					// если вызов функции как аргумент функции,
					//то имя не рассмотртся как IDENT_TOKEN, а должно
					if(read_list) { 
						LEX_TOKEN* name = st_peek( (stack_t*)name_func->token );
						delete_stack( (stack_t*)name_func->token );
						name_func = name;
					}
					
					
					stack_t* args_stack = stack_new();
					st_push( args_stack, create_token_stack( LIST_EL_TOKEN ) );				
					
					// первая единица => польская нотация
					//вторая указывает на чтение списка
					int delta_func = 0;
					IFD printf("\tBEFORE RECURSION end_deep => %d\n", end_by_bracket_deep);
					end_by_bracket_deep = 0;
					++end_by_count_cnt;
					func_args->token = (char*)generate_stack_recursive(
						out, args_stack, tokens+k, end_by_round_bracket, 1, &delta_func, read_list + 1);
					end_by_bracket_deep = 1;
					if( func_args->token == NULL )
						return NULL;
					
					func_args->type = LIST_TOKEN;
					
					//TODO ручное создание стека, так точно известно, что элементов 2
					LEX_TOKEN* func_call = create_token_stack( FUNC_CALL_TOKEN );
				
					
					st_push( (stack_t*)(func_call->token), func_args);
					st_push( (stack_t*)(func_call->token), name_func);
					st_push(val_stack, func_call);

					k+=delta_func; continue;
		
				}
			} else if( !strcmp( tokens[k].token, ",") ){	
				if( read_list ){
					int Polish_res = end_Polish_expr(for_push_val_stack, op_stack);
					//ДУБЛИРОВАНИЕ КОДА, см //DOUBLE
					if(!Polish_res) {
						if(!(
							read_list && op_stack->size==1 &&
							( (LEX_TOKEN*)st_peek(op_stack) )->token[0] == '('
						) ){
							if( is_bracket( st_peek(op_stack) ) )
								fprintf(out, "ERROR: no closed bracket for '%s'\n", ( (LEX_TOKEN*)(st_peek(op_stack)) )->token );
							else
								fprintf(out, "ERROR: wrong expression. not null stack;):\n");

							return NULL;
						}
					}
					
					st_push( val_stack, create_token_stack( LIST_EL_TOKEN ) );
					for_push_val_stack = (stack_t*)( ((LEX_TOKEN*)st_peek(val_stack))->token );
				} else {
					fprintf(out, "ERROR: zpt no in list\n");
					return NULL;
				}
				k++; continue;
			}
			
			
			
			if (tokens[k].type != OPERATION_TOKEN)
				st_push(for_push_val_stack, &tokens[k]);
			else{
				char* analyze_msg =  analyze(op_stack, for_push_val_stack, &tokens[k]);
				if( analyze_msg ) {
					fprintf(out, "%s", analyze_msg);
					return NULL;
				}
			}
			last_type = tokens[k].type;
			k++;
		}// while

		
		
		
		IFD printf("RECURSION --\t op_stack => ");
		IFD print_stack(stdout, op_stack, 0);
		int Polish_res = end_Polish_expr(for_push_val_stack, op_stack);
		//ДУБЛИРОВАНИЕ КОДА, см //DOUBLE
		if(!Polish_res) {
				if( is_bracket( st_peek(op_stack) ) )
					fprintf(out, "ERROR: no closed bracket for '%s'\n", ( (LEX_TOKEN*)(st_peek(op_stack)) )->token );
				else {
					fprintf(out, "ERROR: wrong expression. not null stack:\n");
					print_stack(stdout, op_stack, 0);
				}

				return NULL;
			
		}
		
	} else {
		while( NOT_END( tokens + k ) )
			st_push(for_push_val_stack, &tokens[k++]);			
	}
	
	
	*delta = k;
	delete_stack( op_stack );
	return val_stack;
}


stack_t* generate_stack(FILE* out,stack_t* val_stack, LEX_TOKEN* tokens,
						int(*NOT_END)(LEX_TOKEN*), int Polish){
	
	int delta = 0;
	// 0 => чтение начинается с неспискового
	return generate_stack_recursive(out, val_stack, tokens, NOT_END, Polish, &delta, 0);	
}



