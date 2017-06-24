#include "AST.h"

/*
TODO присваивание
  -- x = 6
  -- y = 6 + 1	
*/

/*
предполагается, что каждая строка - законченное выражение
если строка длинная, то при переносе ставить спец символ 
	TODO придумать какой спецсимвол, например слеш, решётка или собака
	
TODO уменьшение стека автоматическое ???
*/


LEX_TOKEN* pop(token_stack* stack) {
	if ( stack->size > 0 ) 
		return &stack->val[--stack->size];
	return NULL;
}
void push(token_stack* stack, LEX_TOKEN* el) {
	if (stack->size == stack->capacity){
		stack->capacity *= EXPANSION_NUM;
		stack->val = realloc(stack->val, stack->capacity*sizeof(LEX_TOKEN));
	}
	stack->val[ stack->size++ ] = *el;
}
LEX_TOKEN* peek(token_stack* stack) {
	if ( stack->size > 0 ) 
		return &stack->val[stack->size-1];
	return NULL;
}

token_stack* make_stack(int capacity) {
	token_stack* stack = malloc(sizeof(token_stack));
	stack->size = 0;
	stack->capacity = capacity;
	stack->val = malloc(capacity * sizeof(LEX_TOKEN));
	return stack;
}

void delete_stack(token_stack* stack) {
	free(stack->val);
	free(stack);
}

void print_stack(FILE* out, token_stack* stack, int n) {
	int num = n;
	for(int i=0; i < stack->size; i++){
		if(stack->val[i].type == EXPR_STACK_TOKEN)
			fprintf(out, "EXPR_%d ", num++);
		else if( stack->val[i].type == TRUE_BODY_TOKEN )
			fprintf(out, "T_BODY_%d ", num++);
		else
			fprintf(out, "%s ", stack->val[i].token);
	}
	fprintf(out, "NULL\n");
	int num_cp = n;
	for(int i=0; i < stack->size; i++)
		if(stack->val[i].type == EXPR_STACK_TOKEN){
			fprintf(out, "EXPR_%d :", num_cp++);
			print_stack( out, ((token_stack*)(stack->val[i].token)), num);
		} else if (stack->val[i].type == TRUE_BODY_TOKEN ) {
			fprintf(out, "T_BODY_%d :", num_cp++);
			print_stack( out, ((token_stack*)(stack->val[i].token)), num);
		}
}

token_stack* stack_pop(stack_stack* stack) {
	if ( stack->size > 0 ) 
		return &stack->val[--stack->size];
	return NULL;
}
void stack_push(stack_stack* stack, token_stack* el) {
	if (stack->size == stack->capacity){
		stack->capacity *= EXPANSION_NUM;
		stack->val = realloc(stack->val, stack->capacity*sizeof(token_stack));
	}
	stack->val[ stack->size++ ] = *el;
}
token_stack* stack_peek(stack_stack* stack) {
	if ( stack->size > 0 ) 
		return &stack->val[stack->size-1];
	return NULL;
}

stack_stack* stack_make_stack(int capacity) {
	stack_stack* stack = malloc(sizeof(stack_stack));
	stack->size = 0;
	stack->capacity = capacity;
	stack->val = malloc(capacity * sizeof(token_stack));
	return stack;
}

void stack_delete_stack(token_stack* stack) {
	free(stack->val);
	free(stack);
}





#define EMPTY 				(100)
#define OPEN_BRACKET		(101)
#define CLOSED_BRACKET		(102)

// x >>= man . age + (2+3**4)
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

void analyze(token_stack*, token_stack*, LEX_TOKEN*);

int func1(token_stack* Moscow, token_stack* Kiev, LEX_TOKEN* token){
	push(Moscow, token);
	return 0;
}
int func2(token_stack* Moscow, token_stack* Kiev, LEX_TOKEN* token){
	push(Kiev, pop(Moscow));
	analyze(Moscow, Kiev, token);
	
	return 0;
}
int func3(token_stack* Moscow, token_stack* Kiev, LEX_TOKEN* token){
	pop(Moscow);
	return 0;
}
int func4(token_stack* Moscow, token_stack* Kiev, LEX_TOKEN* token){
	return 1;
}
int func5(token_stack* Moscow, token_stack* Kiev, LEX_TOKEN* token){
	return -1;//ERROR
}

int (*functions[5])(token_stack*, token_stack*, LEX_TOKEN*) = 
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
								
void analyze(token_stack* op_stack, token_stack* val_stack, LEX_TOKEN* token){
	int last_code = get_op_index( peek(op_stack) );
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
token_stack* generate_stack(token_stack* val_stack, LEX_TOKEN* tokens, int (*not_end)(LEX_TOKEN*)){
	//TODO сделать op_stack постоянным
	token_stack* op_stack = make_stack(START_OPERATION_STACK_SIZE);
	int k = 0;
	
	while( not_end( &tokens[k] ) ){
		if (tokens[k].type != OPERATION_TOKEN) 
			push(val_stack, &tokens[k]);
		else
			analyze(op_stack, val_stack, &tokens[k]);
		k++;		
	}
	
	while( peek(op_stack) ) {
		int ind = get_op_index(peek(op_stack));
		if( ind >= EQUAL && ind <= DOT)
			push(val_stack, pop(op_stack));
		else
			break;
	}
	delete_stack( op_stack );
	return val_stack;
}

//TODO когда сделаешь хеш, измени механизмы поиска с массивов на статические хэши если это даст прирост скорости
static char* deep_words[] = {"while", "if", "else", "elseif", "sub"};

int deep_word(char* word) {
	for(int i = 0; i < sizeof(deep_words)/sizeof(char*); ++i)
		if( !strcmp(word, deep_words[i]) )
			return i+1;
	return 0;
	
}

token_stack* build_AST(ALL_LEX_TOKENS* all_token){
	LEX_TOKEN* tokens = all_token->tokens;
	
	stack_stack* big_stack = stack_make_stack(START_DEEP_STACK_SIZE);
	stack_push( big_stack, make_stack(START_TOKEN_STACK_SIZE) );//добавляем main_stack
	
	token_stack* deep_stack = make_stack(START_DEEP_STACK_SIZE);	
	int cnt_line = 0;
	int deep = 0;
	int where = 0;
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
			token_stack* top = stack_pop( big_stack );
			//кладем его в стек на уровень ниже
			LEX_TOKEN* top_stack_token = malloc(sizeof(token_stack));
			top_stack_token->type = TRUE_BODY_TOKEN;
			top_stack_token->info = 0;
			top_stack_token->token = (char*)top;
				
			push( stack_peek(big_stack), top_stack_token);
			
			//сразу за ним оператор блока (if, while...)
			push( stack_peek( big_stack ), pop(deep_stack));
		}
		int deep_word_num;
		if ( deep_word_num = deep_word(tokens->token) ) {
			if( tokens[cnt_line-1].type != OPERATION_TOKEN || tokens[cnt_line-1].token[0] != ':'){
				//TODO printf -> fprintf
				printf("ERROR: Deep string must end with ':'\nnot empty str %d\n", str_num);
				return NULL;
			} else { //deep string по обоим параметрам (deep word and :)
				push( deep_stack, tokens);
				//TODO возможно нужно избавиться от передачи указателя на функцию
				cnt_for_not_end_line = cnt_line-2;
				token_stack* expr_stack = make_stack( START_EXPR_STACK_SIZE );
				generate_stack(expr_stack, tokens+1, not_end_line );
				
				LEX_TOKEN* expr_token = malloc(sizeof(LEX_TOKEN));
				expr_token->type = EXPR_STACK_TOKEN;
				expr_token->info = deep_word_num;
				expr_token->token = (char*)expr_stack;
				
				push( stack_peek(big_stack), expr_token);
				stack_push( big_stack, make_stack(START_TOKEN_STACK_SIZE) );// тело для if, while, def...
				
			}
		} else {			
			cnt_for_not_end_line = cnt_line;
			generate_stack( stack_peek(big_stack), tokens, not_end_line );
		}
		tokens += all_token->count_tokens[str_num];
	}
	return stack_peek(big_stack);
}




