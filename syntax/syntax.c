#include "syntax.h"

char* deep_words[CNT_DEEP_WORD] = {"if", "else", "while", "sub"};
//deep words расположены в начале, за /**/ идут остальные ключевые слова
char* key_words[CNT_KEY_WORDS] = {"if", "else", "while", "sub",/**/ "var", "return"};

static char* operators_3[] = {"**=", "//=", "<<=", ">>="};
static int 	property_3[] = {EQUAL, EQUAL, EQUAL, EQUAL};

//операторы с приоритетом -1 приравниваются к IDENT токенам,
//т.е могут идти подряд несколько сразу:
//  --for line in lines:
//  --while([...
//но не:
//  --for char in "string":
static char* operators_2[] = {"+=", "-=", "*=", "/=", "%=", "**", "//",">>", "<<",  "&&",   "^^",  "||",  "->", ">=", "<=", "==", "!=", "=>"};
static int 	property_2[] = {EQUAL,EQUAL,EQUAL,EQUAL,EQUAL, POW, MUL, SHIFT,SHIFT,LOG_AND,LOG_AND,LOG_OR,   -2,   CMP,  CMP,  CMP,  CMP,  -1};

static char* char_operators[] = {"not", "and",   "or", "eq", "in", "xor"};
static int		property_char[]= { NOT, LOG_AND, LOG_OR, CMP, IN,  LOG_AND};

static const char operations[] = {'+', '-', '*', '/', '%', '.', ',',  '=',  '!', '^',   '>', '<', '&',    '|', '(', '[', '{', '}', ']', ')', ';',':' };
static int 	property_1[] = 		{ADD, ADD, MUL, MUL, MUL, DOT, -2,  EQUAL, NOT,BIT_XOR,CMP, CMP,BIT_AND,BIT_OR, -1, -1,  -1,  -1,   -1,   -1, -1, -2 };

char brackets_char[6] = {'(', '[', '{', ')', ']', '}' };

//##########################################
//##########################################
//##########################################
int is_key_word(char* word) {
	for(int i = 0; i < sizeof(key_words)/sizeof(char*); ++i)
		if( !strcmp(word, key_words[i]) )
			return i+1;
	return 0;
}

int isop(char c){
	for(int i = 0; i < sizeof(operations)/sizeof(char); ++i)
		if( c == operations[i])
			return property_1[i];
	return 0; 
}

int is_char_operator(char* op){
	for(int i=0; i < sizeof(char_operators)/sizeof(char*); i++)
		if( !strcmp(op, char_operators[i]) )
			return property_char[i];
	return 0;
}

//по трём символам, первый из которых принадлежит operations, определяет приоритет операции
// и изменяет смещение в зависимости от длины
int get_op(int* offset, const int priorety, char* token_str, int* len_token) {
	for(int i = 0; i < sizeof(operators_3)/sizeof(char*); ++i)
		if( !strcmp(operators_3[i], token_str) ) {
			*offset += 2;
			*len_token = 3;
			return property_3[i];
		}
	token_str[2] = '\0';
	for(int i = 0; i < sizeof(operators_2)/sizeof(char*); ++i)
		if( !strcmp(operators_2[i], token_str) ) {
			*offset += 1;
			*len_token = 2;		
			return property_2[i];
		}
	token_str[1] = '\0';
	*len_token = 1;
	return priorety;
}

/*
*/
