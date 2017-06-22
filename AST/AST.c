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

void print_stack(token_stack* stack, int deep) {
	for(int i=0; i<deep; i++) printf("\t");
	for(int i=0; i < stack->size; i++)
		printf("%s ", stack->val[i].token);
	printf("NULL\n");
}


#define EMPTY 				(100)
#define OPEN_BRACKET		(101)
#define CLOSED_BRACKET		(102)

// x >>= man . age + (2+3**4)
int get_condition(int last_code, int this_code) {
	/*
	
	//	  |  =  +  -  *  /  %  .  (  )
	const int variants[9][10] = {
		{ 4, 1, 1, 1, 1, 1, 1, 1, 1, 5 },// |
		{ 2, 2, 1, 1, 1, 1, 1, 1, 1, 2 },// =
		{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 2 },// +
		{ 2, 2, 2, 2, 1, 1, 1, 1, 1, 2 },// -
		{ 2, 2, 2, 2, 2, 2, 2, 1, 1, 2 },// *
		{ 2, 2, 2, 2, 2, 2, 2, 1, 1, 2 },// /
		{ 2, 2, 2, 2, 2, 2, 2, 1, 1, 2 },// %
		{ 2, 2, 2, 2, 2, 2, 2, 2, 1, 2 },// .
		{ 5, 1, 1, 1, 1, 1, 1, 1, 1, 3 },// (
	};
	
	*/
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
	//printf("VARIANT: %d\n", variant);
	//printf("Operator => %s this_code => %d, last_code=> %d\n", token->token, this_code, last_code);
	functions[variant-1](op_stack, val_stack, token);
}

int not_end_logic_expr(LEX_TOKEN* token){
	return !(token->type == OPERATION_TOKEN && token->token[0] == ':');
}

static int cnt_for_not_end_line;
int not_end_line(LEX_TOKEN* token){
	return cnt_for_not_end_line--;
}

token_stack* generate_stack(LEX_TOKEN* tokens, int (*not_end)(LEX_TOKEN*)){
	token_stack* val_stack = make_stack(START_TOKEN_STACK_SIZE);
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

//https://master.virmandy.net/perevod-iz-infiksnoy-notatsii-v-postfiksnuyu-obratnaya-polskaya-zapis/
AST_root* build_AST(ALL_LEX_TOKENS* all_token){
	LEX_TOKEN* tokens = all_token->tokens;
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
		if ( deep_word(tokens->token) ) {
			if( tokens[cnt_line-1].type != OPERATION_TOKEN || tokens[cnt_line-1].token[0] != ':'){
				printf("ERROR: Deep string must end with ':'\nnot empty str %d\n", str_num);
				return NULL;
			} else { //deep string по обоим параметрам (deep word and :)
				++deep;
				printf("head|");
				cnt_for_not_end_line = cnt_line-2;
				print_stack(generate_stack( tokens+1, not_end_line ), deep-1);
			}
		} else {			
			cnt_for_not_end_line = cnt_line;
			token_stack* stack = generate_stack( tokens, not_end_line );
			print_stack( stack, deep );
		}
		tokens += all_token->count_tokens[str_num];
	}
	
}



















