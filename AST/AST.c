#include "AST.h"

/*
TODO! ошибки при встрече некоторых ключевых слов не в начале строки
TODO придумать какой спецсимвол, например слеш, решётка или собака
*/
////////////////////
int end_by_count_cnt;
int end_by_bracket_deep;
int next_last;
//##################### GLOBAL_VARIABLES

int end_by_count(LEX_TOKEN* tok) { return end_by_count_cnt--; }
int end_by_fat(LEX_TOKEN* tok) {
	--end_by_count_cnt;
	if( end_by_count_cnt < 0) return 0;
	if( tok->type == OPERATION_TOKEN && tok->token[0] == ',') return 0;
	return 1;
}
int end_by_bracket(LEX_TOKEN* tok, char* opened, char* closed){
	if(next_last){
		next_last = 0;
		return 0;
	}
	--end_by_count_cnt;
	if( end_by_count_cnt < 0) return 0;
	
	if( !strcmp( tok->token, closed ) )
		--end_by_bracket_deep;
	else if ( !strcmp( tok->token, opened) ){
		++end_by_bracket_deep;
		next_last = 0;
	}
		
	if( end_by_bracket_deep > 0 ){ 
		next_last = 0;
		return 1;
	} else if (end_by_bracket_deep == 0 ){
		next_last = 1;
		return 1;
	}
	
	return 1;
}
int end_by_round_bracket(LEX_TOKEN* tok){
	return end_by_bracket(tok, "(", ")");
}
int end_by_square_bracket(LEX_TOKEN* tok){
	return end_by_bracket(tok, "[", "]");	
}
int end_by_figure_bracket(LEX_TOKEN* tok){
	return end_by_bracket(tok, "{", "}");
}
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
		else if( TOKEN_I->type == LIST_TOKEN )
			fprintf(out, "LIST_%d ", num++);
		else if( TOKEN_I->type == FUNC_CALL_TOKEN )
			fprintf(out, "CALL_%d ", num++);
		else if( TOKEN_I->type == LIST_EL_TOKEN )
			fprintf(out, "ELEM_%d ", num++);
		else if( TOKEN_I->type == ARRAY_TOKEN )
			fprintf(out, "ARR_%d ", num++);
		else if( TOKEN_I->type == HASH_TOKEN )
			fprintf(out, "HASH_%d ", num++);
		else if( TOKEN_I->type == INIT_VAR_TOKEN )
			fprintf(out, "INIT_%d ", num++);
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
		} else if (TOKEN_I->type == LIST_TOKEN ) {
			fprintf(out, "LIST_%d :", num_cp++);
			print_stack( out, (stack_t*)TOKEN_I->token, num);
		} else if (TOKEN_I->type == FUNC_CALL_TOKEN ) {
			fprintf(out, "CALL_%d :", num_cp++);
			print_stack( out, (stack_t*)TOKEN_I->token, num);
		} else if (TOKEN_I->type == LIST_EL_TOKEN ) {
			fprintf(out, "ELEM_%d :", num_cp++);
			print_stack( out, (stack_t*)TOKEN_I->token, num);
		} else if (TOKEN_I->type == ARRAY_TOKEN ) {
			fprintf(out, "ARR_%d :", num_cp++);
			print_stack( out, (stack_t*)TOKEN_I->token, num);
		} else if (TOKEN_I->type == HASH_TOKEN ) {
			fprintf(out, "HASH_%d :", num_cp++);
			print_stack( out, (stack_t*)TOKEN_I->token, num);
		} else if (TOKEN_I->type == INIT_VAR_TOKEN ) {
			fprintf(out, "INIT_%d :", num_cp++);
			print_stack( out, (stack_t*)TOKEN_I->token, num);
		}
	#undef TOKEN_I
}
void print_function(FILE* out, function_t* func){
	fprintf(out, "FUNCTION: \n");
	fprintf(out, "\thead: \n");
	print_stack(out, &(func->head), 0 );
	fprintf(out, "\tbody: \n");
	print_stack(out, &(func->body), 0 );
	fprintf(out, "END\n");
}
 

static char* new_function(stack_t* head, stack_t* body, hash_t* functions){
//TODO обработка аргументов функции по хэдеру
//TODO улучшить тексты ошибок
	if( ((LEX_TOKEN*)(head->val[0]))->type != IDENT_TOKEN ) {
		return "ERROR: function header must start with IDENT_TOKEN\n";
	} else if(	
		  strcmp( ((LEX_TOKEN*)(head->val[1]))->token, "(") ||
		  strcmp( ((LEX_TOKEN*)(st_peek(head)))->token, ")")  ) {
		return "ERROR: second and last token must be brackets\n";
	}

	function_t* func = malloc(sizeof(function_t));
	func->head = *head;
	func->body = *body;
	hash_set(
		functions,
		((LEX_TOKEN*)(head->val[0]))->token,
		func
	);
	return NULL;
}


static int decrement_deep(FILE* out, stack_t* big_stack, stack_t* deep_stack, int* can_else, hash_t* functions){
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
		*can_else = 1;
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
		char* new_function_error = new_function(
					(stack_t*)( (LEX_TOKEN*)st_pop( st_peek(big_stack) ) )->token,
					(stack_t*)( (LEX_TOKEN*)st_pop( st_peek(big_stack) ) )->token,
					functions
		);				
		if(new_function_error){
			fprintf(out, "%s", new_function_error);
			return 0;
		}
	} else
		st_push(st_peek( big_stack ), tok);
	return 1;
}

LEX_TOKEN* create_token(int type, int info, void* token){
	LEX_TOKEN* tok = malloc(sizeof(LEX_TOKEN));
	tok->type = type;
	tok->info = info;
	tok->token = (char*)token;
	return tok;
}

static stack_t* genrate_var_init_stack(FILE* out, LEX_TOKEN* tokens, int len_str){
	stack_t* init = stack_new();
	LEX_TOKEN* tok = create_token(TRUE_BODY_TOKEN, 0, init);
	int cond = 0;
	/*	cond=0 => состояние считывания новой переменной
		cond=1 => состояние считывания значения переменной
	*/
	while(len_str > 0) {		
		if(!cond ) {
			if(tokens->type != IDENT_TOKEN) {
				fprintf(out, "ERROR: expected varname, get ");
				print_token(tokens, out);
				return NULL;
			}
			
			st_push(init, create_token(IDENT_TOKEN, 0, tokens->token) );
			tokens += 1; len_str--;
			if(!len_str)  break;
			
			if(//если не операция или неподходящая операция
				  tokens->type != OPERATION_TOKEN
				||( strcmp(tokens->token, "=") && strcmp(tokens->token,",") )
			) {
				fprintf(out, "ERROR: expected ',' or '=', get ");
				print_token(tokens, out);
				return NULL;
			}
			
			if( tokens->token[0] == '=' )
				cond = 1;//перейти к считывания выражения

			tokens += 1; len_str--;
			if(!len_str) break;			
		} else {


			end_by_count_cnt = len_str;			
			stack_t* generate_res = generate_stack(out, stack_new(), tokens, end_by_fat, 1);

			if( !generate_res )
					return NULL;
			
			st_push(init, create_token(LIST_EL_TOKEN, 0, generate_res) );
			cond = 0;
			tokens += len_str - end_by_count_cnt;
			len_str = end_by_count_cnt;					
		}
	}
	return init;	 
}


stack_t* build_AST(ALL_LEX_TOKENS* all_token, FILE* out, hash_t* functions){
	LEX_TOKEN* tokens = all_token->tokens;

	stack_t* big_stack = stack_new();
	st_push( big_stack, stack_new() );//добавляем main_stack

	stack_t* deep_stack = stack_new();
	int cnt_line = 0;
	int deep = 0;
	int can_else = 0;
	for(int str_num = 0; str_num < all_token->count_token_lines; ++str_num) {
		cnt_line = all_token->count_tokens[str_num];


		//уменьшение отступа до нужной глубины
		while(deep_stack->size > all_token->deeper[ str_num ] ){
			int decrement_ok = decrement_deep(out, big_stack, deep_stack, &can_else, functions);
			if(!decrement_ok) return NULL;
		}
	
		if(tokens->type !=IDENT_TOKEN) {
			printf("ERROR: every string must start with IDENT_TOKEN\n");
			return NULL;
		}
		
		if ( tokens->info && tokens->info <= CNT_DEEP_WORD ) {
			if( tokens[cnt_line-1].type != OPERATION_TOKEN || tokens[cnt_line-1].token[0] != ':'){
				//TODO printf -> fprintf
				fprintf(out, "ERROR: Deep string must end with ':'\n");
				return NULL;
			} else { //deep string по обоим параметрам (deep word and ':')
				if(tokens->info == ELSE_ID ) {
					if( !can_else ) {
						fprintf(out, "ERROR: expectedd 'if' before 'else' construction\n");
						return NULL;
					} else if ( cnt_line != 2 ){ // если строка содержит else, то : и ничего более 
						fprintf(out, "ERROR: wrong 'else' construction\n");
						return NULL;
					}
				} else if(tokens->info == FUNCTION_ID ){
					if( deep_stack->size != 0) { // NULL, а не sub NULL, т.к. push(sub) позже 
						fprintf(out, "WARNING: I can`t work with deep_function\n");
						return NULL;
					}
				}
				
				st_push( deep_stack, tokens);
				stack_t* expr_stack = stack_new();
				
				end_by_count_cnt = cnt_line-2;
				stack_t* generate_res = generate_stack(
					out, expr_stack, tokens+1, end_by_count,
					tokens->info != FUNCTION_ID//с хедером функции отдельный разговор
				);
				if( !generate_res )
					return NULL;

				LEX_TOKEN* expr_token = malloc(sizeof(LEX_TOKEN));
				expr_token->type = EXPR_STACK_TOKEN;
				expr_token->info = tokens->info;
				expr_token->token = (char*)expr_stack;

				st_push( st_peek(big_stack), expr_token);
				st_push( big_stack, stack_new() );// тело для if, while, def...
				

			}
		} else if ( tokens->info == VAR_ID ) {
			stack_t* init_stack = genrate_var_init_stack(out, tokens+1, cnt_line-1);
			//print_stack(out, init_stack, 0);
			st_push( st_peek(big_stack), create_token(INIT_VAR_TOKEN, 0, init_stack));
		} else {
			end_by_count_cnt = cnt_line;
			stack_t* generate_res = generate_stack(out, st_peek(big_stack), tokens, end_by_count, 1);
			if( !generate_res )
					return NULL;
		}
		can_else = 0;
		tokens += all_token->count_tokens[str_num];
	}


	//уменьшение отступа до нуля
	while(deep_stack->size > 0 ){
			int decrement_ok = decrement_deep(out, big_stack, deep_stack, &can_else, functions);
			if(!decrement_ok) return NULL;
	}
	
	return st_peek(big_stack);	
}




