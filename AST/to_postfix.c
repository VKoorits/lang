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




// https://master.virmandy.net/perevod-iz-infiksnoy-notatsii-v-postfiksnuyu-obratnaya-polskaya-zapis/
stack_t* generate_stack(stack_t* val_stack, LEX_TOKEN* tokens,
						int(*NOT_END)(LEX_TOKEN*), int Polish, int* delta){
	//OPTIMIZATE сделать op_stack постоянным
	stack_t* op_stack = stack_new();
	int k = 0;
		if(Polish) {
		int last_type = UNKNOWN_TOKEN;

		while( NOT_END( tokens + k ) ){
			
			//BAG TODO tokens[k].token[0] == '(' подойдет и для строки начинающейся со скобки!!!!!!!!
			if( tokens[k].token[0] == '('){
				if( last_type == IDENT_TOKEN) { //вызов функции: 'print (...'
					end_by_bracket_deep = 0;
					
					LEX_TOKEN* name_func = st_pop(val_stack);					
					
					LEX_TOKEN* func_args = malloc( sizeof(LEX_TOKEN) );
					int delta_func = 0;
					func_args->token = (char*)generate_stack(
						stack_new(), tokens+k, end_by_bracket, 1, &delta_func);
					
					func_args->type = ARGS_TOKEN;
					
					//TODO ручное создание стека, так точно известно, что элементов 2
					LEX_TOKEN* func_call = malloc( sizeof(LEX_TOKEN) );
					func_call->token = (char*)stack_new();
					func_call->type = FUNC_CALL_TOKEN;
				
					
					st_push( (stack_t*)(func_call->token), func_args);
					st_push( (stack_t*)(func_call->token), name_func);
					st_push(val_stack, func_call);
					

					k+=delta_func+1; continue;
					
					
				}
			}
			
			if (tokens[k].type != OPERATION_TOKEN)
				st_push(val_stack, &tokens[k]);
			else
				analyze(op_stack, val_stack, &tokens[k]);
			last_type = tokens[k].type;
			k++;
		}
		++end_by_count_cnt;
		
		while( st_peek(op_stack) ) {
			int ind = get_op_index(st_peek(op_stack));
			if( ind >= EQUAL && ind <= DOT)
				st_push(val_stack, st_pop(op_stack));
			else
				break;
		}

		delete_stack( op_stack );
	} else {
		while( NOT_END( tokens + k ) )
			st_push(val_stack, &tokens[k++]);	
	}
	
	
	*delta = k;
	
	return val_stack;
}

