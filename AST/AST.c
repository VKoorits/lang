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

typedef struct token_stack {
	LEX_TOKEN* val;
	int size;
	int capacity;
} token_stack;

LEX_TOKEN* pop(token_stack* stack) {
	if ( stack->size > 0 ) 
		return &stack->val[--stack->size];
	return NULL;
}
void push(token_stack* stack, LEX_TOKEN* el) {
	if (stack->size == stack->capacity){
		stack->capacity *= EXPANSION_NUM;
		stack->val = realloc(stack->val, stack->capacity);
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

void print_stack(token_stack* stack) {
	for(int i=0; i<stack->size; i++)
		printf("%s ", stack->val[i].token);
	printf("NULL\n");
}


#define EMPTY 			(0)
	#define MIN_OPERATION (1)
#define EQUAL			(1)
#define ADD				(2)  
#define SUB				(3) 
#define MUL				(4) 
#define DIV				(5)
#define REST_DIV		(6)
#define DOT				(7)
	#define MAX_OPERATION (7)
#define OPEN_BRACKET	(8)
#define CLOSED_BRACKET	(9)

//	  |  =  +  -  *  /  %  .  (  )
const int variants[9][10] = {
	{ 4, 1, 1, 1, 1, 1, 1, 1, 1, 5 },// |
	{ 2, 2, 1, 1, 1, 1, 1, 1, 1, 2 },// =
	{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 2 },// +
	{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 2 },// -
	{ 2, 2, 2, 2, 2, 2, 2, 1, 1, 2 },// *
	{ 2, 2, 2, 2, 2, 2, 2, 1, 1, 2 },// /
	{ 2, 2, 2, 2, 2, 2, 2, 1, 1, 2 },// %
	{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },// .
	{ 5, 1, 1, 1, 1, 1, 1, 1, 1, 3 },// (
};

int get_op_index(const char* op) {
	switch(op[0]){
		case '=': return EQUAL;
		case '+': return ADD;
		case '-': return SUB;
		case '/': return DIV;
		case '*': return MUL;
		case '%': return REST_DIV;
		case '.': return DOT;
		case '(': return OPEN_BRACKET;
		case ')': return CLOSED_BRACKET;
	}
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
								
void analyze(token_stack* op_stack, token_stack* val_stack, LEX_TOKEN* token){
	int i = 0, j = get_op_index( token->token );
	LEX_TOKEN* top = peek(op_stack);
	if(top) {
		i = get_op_index( top->token );
	}
	
	int variant = variants[i][j];	
					
	functions[variant-1](op_stack, val_stack, token);
}


//https://master.virmandy.net/perevod-iz-infiksnoy-notatsii-v-postfiksnuyu-obratnaya-polskaya-zapis/
AST_root* build_AST(ALL_LEX_TOKENS* all_token){
	token_stack* val_stack = make_stack(START_TOKEN_STACK_SIZE);
	token_stack*  op_stack = make_stack(START_OPERATION_STACK_SIZE);
	LEX_TOKEN* tokens = all_token->tokens;
	int k = 0;
	for(int str_num = 1; str_num <= all_token->count_token_lines; ++str_num) {
		for(int tok_num = 1; tok_num <= all_token->count_tokens[str_num-1]; ++tok_num, ++k){
			if (tokens[k].type != OPERATION_TOKEN) 
				push(val_stack, &tokens[k]);
			else
				analyze(op_stack, val_stack, &tokens[k]);			
			
		}
		
		while( peek(op_stack) ) {
			int ind = get_op_index(peek(op_stack)->token);
			if( ind >= MIN_OPERATION && ind <= MAX_OPERATION)
				push(val_stack, pop(op_stack));
			else
				break;
			
		}
		print_stack(val_stack);
		
		val_stack->size = 0;
		op_stack->size = 0;
	}
}
