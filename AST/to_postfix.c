#include "AST.h"

extern int end_by_count_cnt;
extern int end_by_bracket_deep;


int get_condition(int last_code, int this_code) {

	if( last_code == EMPTY ) {
		if(this_code == CLOSED_BRACKET)
			return 5;
		else return 1;
	}else if (last_code == OPEN_BRACKET){
		if( this_code == EMPTY )
			return 5;
		else if( this_code == CLOSED_BRACKET)
			return 3;
		else return 1;
	}

	if( this_code == OPEN_BRACKET)
		return 1;
	else if ( this_code == CLOSED_BRACKET)
		return 2;


	if ( last_code > this_code )
		return 2;
	else return 1	;
}

void analyze(stack_t*, stack_t*, LEX_TOKEN*);

int func1(stack_t* Moscow, stack_t* Kiev, LEX_TOKEN* token){
	st_push(Moscow, token);
	return 0;
}
int func2(stack_t* Moscow, stack_t* Kiev, LEX_TOKEN* token){
	st_push(Kiev, st_pop(Moscow));
	analyze(Moscow, Kiev, token);

	return 0;
}
int func3(stack_t* Moscow, stack_t* Kiev, LEX_TOKEN* token){
	st_pop(Moscow);
	return 0;
}
int func4(stack_t* Moscow, stack_t* Kiev, LEX_TOKEN* token){
	return 1;
}
int func5(stack_t* Moscow, stack_t* Kiev, LEX_TOKEN* token){
	printf("ERROR: condition 5, func5!!\n");
	exit(7);
}

static int (*functions[5])(stack_t*, stack_t*, LEX_TOKEN*) = 
								{ func1, func2, func3, func4, func5 };


int get_op_index(LEX_TOKEN* top) {
	int index = EMPTY;
	if(top) {
		index = top->info;
		if(index == -1){
			if(top->token[0] == '(')
				index = OPEN_BRACKET;
			else if ( top->token[0] == ')')
				index = CLOSED_BRACKET;
		}
	}
	return index;
}

void analyze(stack_t* op_stack, stack_t* val_stack, LEX_TOKEN* token){
	int last_code = get_op_index( st_peek(op_stack) );
	int this_code = get_op_index( token );

	int variant = get_condition(last_code, this_code);
	functions[variant-1](op_stack, val_stack, token);
}


LEX_TOKEN* create_token_stack(int token_type) {
	LEX_TOKEN* tok = malloc( sizeof(LEX_TOKEN) );
	tok->token = (char*)stack_new();
	tok->type = token_type;
	return tok;
}

//TODO! обработка ошибок, т.к там они могут возникать
void end_Polish_expr(stack_t* val_stack, stack_t* op_stack){
	while( st_peek(op_stack) ) {
			int ind = get_op_index(st_peek(op_stack));
			if( ind >= EQUAL && ind <= DOT)
				st_push(val_stack, st_pop(op_stack));
			else
				break;
		}
}

// https://master.virmandy.net/perevod-iz-infiksnoy-notatsii-v-postfiksnuyu-obratnaya-polskaya-zapis/
stack_t* generate_stack_recursive(stack_t* val_stack, LEX_TOKEN* tokens,
						int(*NOT_END)(LEX_TOKEN*), int Polish, int* delta, int read_list){
	//OPTIMIZATE сделать op_stack постоянным
	stack_t* op_stack = stack_new();
	
	stack_t* for_push_val_stack = val_stack;
	if ( read_list )
		for_push_val_stack = (stack_t*)( ((LEX_TOKEN*)st_peek(val_stack))->token );
		
		
	int k = 0;
		if(Polish) {
		int last_type = UNKNOWN_TOKEN;
		while( NOT_END( tokens + k ) ){
			
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
					end_by_bracket_deep = 0;
					func_args->token = (char*)generate_stack_recursive(
						args_stack, tokens+k, end_by_bracket, 1, &delta_func, 1);

					func_args->type = LIST_TOKEN;
					
					//TODO ручное создание стека, так точно известно, что элементов 2
					LEX_TOKEN* func_call = create_token_stack( FUNC_CALL_TOKEN );
				
					
					st_push( (stack_t*)(func_call->token), func_args);
					st_push( (stack_t*)(func_call->token), name_func);
					st_push(val_stack, func_call);
					

					k+=delta_func+1; continue;
					
					
				}
			} else if( !strcmp( tokens[k].token, ",") ){	
				if( read_list ){
					end_Polish_expr(for_push_val_stack, op_stack);
					
					st_push( val_stack, create_token_stack( LIST_EL_TOKEN ) );
					for_push_val_stack = (stack_t*)( ((LEX_TOKEN*)st_peek(val_stack))->token );
				} else {
					printf("ERROR: zpt no in list\n\n");
					mark();
					return NULL;
				}
				
				
			}


			
			if (tokens[k].type != OPERATION_TOKEN)
				st_push(for_push_val_stack, &tokens[k]);
			else
				analyze(op_stack, for_push_val_stack, &tokens[k]);
			last_type = tokens[k].type;
			k++;
		}
		++end_by_count_cnt;
		/*
		while( st_peek(op_stack) ) {
			int ind = get_op_index(st_peek(op_stack));
			if( ind >= EQUAL && ind <= DOT)
				st_push(for_push_val_stack, st_pop(op_stack));
			else
				break;
		}
		*/
		end_Polish_expr(for_push_val_stack, op_stack);
		delete_stack( op_stack );
	} else {
		while( NOT_END( tokens + k ) )
			st_push(for_push_val_stack, &tokens[k++]);			
	}
	
	
	*delta = k;
	
	return val_stack;
}


stack_t* generate_stack(stack_t* val_stack, LEX_TOKEN* tokens,
						int(*NOT_END)(LEX_TOKEN*), int Polish){
	
	int delta = 0;
	// 0 => чтение начинается с неспискового
	return generate_stack_recursive(val_stack, tokens, NOT_END, Polish, &delta, 0);	
}
