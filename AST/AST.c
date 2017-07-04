#include "AST.h"

/*
TODO придумать какой спецсимвол, например слеш, решётка или собака
TODO сделать обработку скобок, их можно вставлять куда и сколько угодно 
TODO избавиться от образования пустого стэка при обработке 'else'
*/

//##################### GLOBAL_VARIABLES
hash_t* functions = NULL;
//#####################

void print_stack(FILE* out, stack_t* stack, int n) {
	if( !stack ){
		fprintf(out, "EMPTY_STACK\n");
		return;
	}
  #define TOKEN_I ((LEX_TOKEN*)stack->val[i])
	int num = n;
	for(int i=0; i < stack->size; i++){
		if(TOKEN_I->type == EXPR_STACK_TOKEN)
			fprintf(out, "EXPR_%d ", num++);
		else if( TOKEN_I->type == TRUE_BODY_TOKEN )
			fprintf(out, "T_BODY_%d ", num++);
		else if( TOKEN_I->type == FALSE_BODY_TOKEN )
			fprintf(out, "F_BODY_%d ", num++);
		else
			fprintf(out, "%s ", TOKEN_I->token);
	}
	fprintf(out, "NULL\n");
	int num_cp = n;
	for(int i=0; i < stack->size; i++)
		if(TOKEN_I->type == EXPR_STACK_TOKEN){
			fprintf(out, "EXPR_%d :", num_cp++);
			print_stack( out, (stack_t*)TOKEN_I->token, num);
		} else if (TOKEN_I->type == TRUE_BODY_TOKEN ) {
			fprintf(out, "T_BODY_%d :", num_cp++);
			print_stack( out, (stack_t*)TOKEN_I->token, num);
		} else if (TOKEN_I->type == FALSE_BODY_TOKEN ) {
			fprintf(out, "F_BODY_%d :", num_cp++);
			print_stack( out, (stack_t*)TOKEN_I->token, num);
		}
}
void print_function(FILE* out, function_t* func){
	fprintf(out, "FUNCTION: \n");
	fprintf(out, "\thead: \n");
	print_stack(out, &(func->head), 0 );
	fprintf(out, "\tbody: \n");
	print_stack(out, &(func->body), 0 );
	fprintf(out, "END\n");
}

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


void new_function(stack_t* head, stack_t* body){
//TODO Обработка хедера функции
/*

*/
	function_t* func = malloc(sizeof(function_t));
	func->head = *head;
	func->body = *body;
	hash_set(
		functions,
		((LEX_TOKEN*)(head->val[0]))->token,
		func
	);	
}

void init_global_var(){
	functions = hash_new();
}



stack_t* build_AST(ALL_LEX_TOKENS* all_token){
	init_global_var();

	LEX_TOKEN* tokens = all_token->tokens;

	stack_t* big_stack = stack_new();
	st_push( big_stack, stack_new() );//добавляем main_stack

	stack_t* deep_stack = stack_new();
	int cnt_line = 0;
	int deep = 0;
	int can_else = 0;
	for(int str_num = 0; str_num < all_token->count_token_lines; ++str_num) {
		cnt_line = all_token->count_tokens[str_num];


		//уменьшение отступа
		while(deep_stack->size > all_token->deeper[ str_num ] ){
			//снимем верхний стек
			stack_t* top = st_pop( big_stack );
			//кладем его в стек на уровень ниже
			LEX_TOKEN* top_stack_token = malloc(sizeof(stack_t));
			top_stack_token->type = TRUE_BODY_TOKEN;
			top_stack_token->info = 0;
			top_stack_token->token = (char*)top;

			st_push( st_peek(big_stack), top_stack_token);

			//сразу за ним оператор блока (if, while...)
			LEX_TOKEN* tok = st_pop(deep_stack);
			
			if( !strcmp(tok->token, "if") )
				can_else = 1;
			//TODO!!! сделать эти операции до добавления, а то гоняешь туда-обратно
			if( !strcmp(tok->token, "else") ) {
				LEX_TOKEN* false_body = st_pop( st_peek(big_stack) );
				false_body->type = FALSE_BODY_TOKEN;
				st_pop( st_peek(big_stack) );//else expr
				LEX_TOKEN* if_token = st_pop( st_peek(big_stack) );
				
				st_push( st_peek(big_stack), false_body );
				st_push( st_peek(big_stack), if_token );
			//TODO убрать символьную константу 'sub'				
			} else if( !strcmp(tok->token, "sub") ) {
				new_function(
							(stack_t*)( (LEX_TOKEN*)st_pop( st_peek(big_stack) ) )->token,
							(stack_t*)( (LEX_TOKEN*)st_pop( st_peek(big_stack) ) )->token
				);
			} else
				st_push(st_peek( big_stack ), tok);
		}
		int deep_word_num;
		if ( deep_word_num = deep_word(tokens->token) ) {
			if( tokens[cnt_line-1].type != OPERATION_TOKEN || tokens[cnt_line-1].token[0] != ':'){
				//TODO printf -> fprintf
				printf("ERROR: Deep string must end with ':'\nnot empty str %d\n", str_num);
				return NULL;
			} else { //deep string по обоим параметрам (deep word and :)
				if(deep_word_num == ELSE_ID ) {
					if( !can_else ) {
						printf("ERROR: expectedd 'if' before 'else' construction\nnot empty str %d\n", str_num);
						return NULL;
					} else if ( cnt_line != 2 ){ // если строка содержит else, то : и ничего более 
						printf("ERROR: wrong 'else' construction\nnot empty str %d\n", str_num);
						return NULL;
					}
				 } else if(deep_word_num == FUNCTION_ID ){
					if( deep_stack->size != 0) { // NULL, а не sub NULL, т.к. push(sub) позже 
						printf("WARNING: I can`t work with deep_function\nnot empty str %d\n", str_num);
						return NULL;
					}
				}
				
				st_push( deep_stack, tokens);
				//TODO возможно нужно избавиться от передачи указателя на функцию
				stack_t* expr_stack = stack_new();
				generate_stack(
					expr_stack, tokens+1, cnt_line-2,
					deep_word_num != FUNCTION_ID //с хедером функции отдельный разговор
				);

				LEX_TOKEN* expr_token = malloc(sizeof(LEX_TOKEN));
				expr_token->type = EXPR_STACK_TOKEN;
				expr_token->info = deep_word_num;
				expr_token->token = (char*)expr_stack;

				st_push( st_peek(big_stack), expr_token);
				st_push( big_stack, stack_new() );// тело для if, while, def...
				

			}
		} else {
			generate_stack( st_peek(big_stack), tokens, cnt_line, 1);
		}
		can_else = 0;
		tokens += all_token->count_tokens[str_num];
	}
	
	
	print_function(stdout, hash_get(functions, "factorial") );
	return st_peek(big_stack);	
}




