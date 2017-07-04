#include "AST.h"

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
	return -1;//ERROR
}

static int (*functions[5])(stack_t*, stack_t*, LEX_TOKEN*) = 
								{ func1, func2, func3, func4, func5 };



void analyze(stack_t* op_stack, stack_t* val_stack, LEX_TOKEN* token){
	int last_code = get_op_index( st_peek(op_stack) );
	int this_code = get_op_index( token );

	int variant = get_condition(last_code, this_code);
	functions[variant-1](op_stack, val_stack, token);
}



//https://master.virmandy.net/perevod-iz-infiksnoy-notatsii-v-postfiksnuyu-obratnaya-polskaya-zapis/
stack_t* generate_stack(stack_t* val_stack, LEX_TOKEN* tokens, int cnt_tokens, int Polish){
	//TODO сделать op_stack постоянным
	stack_t* op_stack = stack_new();
	int k = 0;
	if(Polish) {

		while( cnt_tokens-->0 ){
			if (tokens[k].type != OPERATION_TOKEN)
				st_push(val_stack, &tokens[k]);
			else
				analyze(op_stack, val_stack, &tokens[k]);
			k++;
		}

		while( st_peek(op_stack) ) {
			int ind = get_op_index(st_peek(op_stack));
			if( ind >= EQUAL && ind <= DOT)
				st_push(val_stack, st_pop(op_stack));
			else
				break;
		}
		delete_stack( op_stack );
	} else {
		while( cnt_tokens-->0 )
			st_push(val_stack, &tokens[k++]);
	}
	return val_stack;
}

