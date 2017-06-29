#include "AST.h"


/*
предполагается, что каждая строка - законченное выражение
если строка длинная, то при переносе ставить спец символ
	TODO придумать какой спецсимвол, например слеш, решётка или собака

TODO уменьшение стека автоматическое ???
*/





void print_stack(FILE* out, stack_t* stack, int n) {
  #define TOKEN_I ((LEX_TOKEN*)stack->val[i])
	int num = n;
	for(int i=0; i < stack->size; i++){
		if(TOKEN_I->type == EXPR_STACK_TOKEN)
			fprintf(out, "EXPR_%d ", num++);
		else if( TOKEN_I->type == TRUE_BODY_TOKEN )
			fprintf(out, "T_BODY_%d ", num++);
		else
			fprintf(out, "%s ", TOKEN_I->token);
	}
	fprintf(out, "NULL\n");
	int num_cp = n;
	for(int i=0; i < stack->size; i++)
		if(TOKEN_I->type == EXPR_STACK_TOKEN){
			fprintf(out, "EXPR_%d :", num_cp++);
			print_stack( out, (stack_t*)TOKEN_I->token, num);//*/*/*
		} else if (TOKEN_I->type == TRUE_BODY_TOKEN ) {
			fprintf(out, "T_BODY_%d :", num_cp++);
			print_stack( out, (stack_t*)TOKEN_I->token, num);
		}
}


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

int (*functions[5])(stack_t*, stack_t*, LEX_TOKEN*) = 
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

int not_end_logic_expr(LEX_TOKEN* token){
	return !(token->type == OPERATION_TOKEN && token->token[0] == ':');
}

static int cnt_for_not_end_line;
int not_end_line(LEX_TOKEN* token){
	return cnt_for_not_end_line--;
}

//https://master.virmandy.net/perevod-iz-infiksnoy-notatsii-v-postfiksnuyu-obratnaya-polskaya-zapis/
stack_t* generate_stack(stack_t* val_stack, LEX_TOKEN* tokens, int (*not_end)(LEX_TOKEN*)){
	//TODO сделать op_stack постоянным
	stack_t* op_stack = stack_new();
	int k = 0;

	while( not_end( &tokens[k] ) ){
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
	return val_stack;
}



stack_t* build_AST(ALL_LEX_TOKENS* all_token){
	LEX_TOKEN* tokens = all_token->tokens;

	stack_t* big_stack = stack_new();
	st_push( big_stack, stack_new() );//добавляем main_stack

	stack_t* deep_stack = stack_new();
	int cnt_line = 0;
	int deep = 0;
	for(int str_num = 0; str_num < all_token->count_token_lines; ++str_num) {
		cnt_line = all_token->count_tokens[str_num];

		/*
			deep string - строка, которая увеличивает отступ, глубину
			например
			  --if a < b :
			  --	print a
			deep dtring  должна оканчиваться двоеточием
			deep word - одно из ключевых слов с которого
			должна начинаться deep string (while, if, else...)
		*/

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
			st_push( st_peek( big_stack ), st_pop(deep_stack));
		}
		int deep_word_num;
		if ( deep_word_num = deep_word(tokens->token) ) {
			if( tokens[cnt_line-1].type != OPERATION_TOKEN || tokens[cnt_line-1].token[0] != ':'){
				//TODO printf -> fprintf
				printf("ERROR: Deep string must end with ':'\nnot empty str %d\n", str_num);
				return NULL;
			} else { //deep string по обоим параметрам (deep word and :)
				st_push( deep_stack, tokens);
				//TODO возможно нужно избавиться от передачи указателя на функцию
				cnt_for_not_end_line = cnt_line-2;
				stack_t* expr_stack = stack_new();
				generate_stack(expr_stack, tokens+1, not_end_line );

				LEX_TOKEN* expr_token = malloc(sizeof(LEX_TOKEN));
				expr_token->type = EXPR_STACK_TOKEN;
				expr_token->info = deep_word_num;
				expr_token->token = (char*)expr_stack;

				st_push( st_peek(big_stack), expr_token);
				st_push( big_stack, stack_new() );// тело для if, while, def...

			}
		} else {
			cnt_for_not_end_line = cnt_line;
			generate_stack( st_peek(big_stack), tokens, not_end_line );
		}
		tokens += all_token->count_tokens[str_num];
	}
	return st_peek(big_stack);
}




