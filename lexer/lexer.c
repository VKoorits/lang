#include <stdio.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

/*
TODO TODO TODO: разбить на lex_analyze на подфункции
TODO защита от длинных строк в программе
TODO реализация INT_TOKEN and FLOAT_TOKEN
TODO отдельный токен под каждый спец символ ? (нужно ли??? может наоборот, как можно больше в один)
													оставить пока так
TODO обработка отступов
TODO приравнивать ; к переносу строки
*/
void mark() { printf("\t==-==\n\t  |\n\t==-==\n");}



void print_token(const LEX_TOKEN* tok, FILE* output_stream){
	char* token_type;
	if(tok->type == UNKNOWN_TOKEN)
		token_type = "UNKNOWN_TOKEN";
	else if(tok->type == INT_NUM_TOKEN)
		token_type = "INT_NUM_TOKEN";
	else if(tok->type == FLOAT_NUM_TOKEN)
		token_type = "FLOAT_NUM_TOKEN";
	else if(tok->type == STRING_TOKEN)
		token_type = "STRING_TOKEN";
	else if(tok->type == IDENT_TOKEN)
		token_type = "IDENT_TOKEN";
	else if(tok->type == OPERATION_TOKEN)
		token_type = "OPERATION_TOKEN";
	else if(tok->type == RESERVED_TOKEN)
		token_type = "RESERVED_TOKEN";
	else token_type = "\n\n\n\nSTRANGE_TOKEN_TYPE\n\n\n\n\a";
	fprintf(output_stream, "%s: %s\n", token_type, tok->token);
}

int isop(int character) {
	static const char operations[] = {'+', '-', '*', '/', '%', '.', ',',
								 	'=', '!', '^', '>', '<', '&', '|',
								 	'(', '[', '{', '}', ']', ')', ';',':' };
	for(int i=0; i < sizeof(operations); i++)
		if(character == operations[i])
			return true;
	return false;
}
int isreserved() {

}


int _get_token_type(const char* token_str, const int was_quote){
	if(was_quote)
		return was_quote;
	return 42;
}	

//#####################

static LEX_TOKEN *tokens;
static int* numerator;
static int count_tokens, len_token, token_type, this_line_pos;
static int tokens_capacity = STARTED_COUNT_TOKENS;
static char* token_str;
static int count_point_in_num = 0;

//#####################

void _add_token(){
	if( len_token == 0 && token_type != STRING_TOKEN)
		return;
	token_str[len_token] = '\0';
	
	if(count_tokens == tokens_capacity){
		tokens_capacity *= EXPANSION_NUM;//может быть изменить коэффициент/ввести доп условия
		tokens = realloc(tokens, sizeof(LEX_TOKEN) * (tokens_capacity) );
	}
	
	

	//перенос накопленных данных
	tokens[count_tokens].token = malloc(len_token+1);
	strcpy( tokens[count_tokens].token, token_str);
	tokens[count_tokens].type = token_type;//_get_token_type(token_str, 0);// <=============-------идентификация токена
	
	count_tokens++;
	len_token = 0;
	numerator[this_line_pos]++;
	token_type = UNKNOWN_TOKEN;
	count_point_in_num = 0;
}



ALL_LEX_TOKENS* lex_analyze(const char* filename, FILE* error_stream){
	FILE* lang_prog = fopen(filename, "r");
	
	if( !lang_prog ){
		fprintf(error_stream, "ERROR: lex_analyze can`t open file %s\n", filename);
		return NULL;
	}
	
	//###############################
	char *str = malloc( MAX_LENGTH_STRING_IN_LANG_PROG );//считывается из файла
	tokens = malloc( sizeof(LEX_TOKEN)* STARTED_COUNT_TOKENS ); // токены для передачи обратно
	int numerator_capacity = STARTED_COUNT_LINES;
	numerator = malloc( sizeof(int) * STARTED_COUNT_LINES);
	numerator[0] = 0;
	this_line_pos = 0;
	//###############################
	count_tokens = 0;
	token_str = malloc(100); // ТОЛЬКО ДЛЯ ПРОТОТИПА
	token_str[99] = '\0';
	int number_str = 0;

	int multyline_comment = false;
	while( fgets( str, MAX_LENGTH_STRING_IN_LANG_PROG, lang_prog ) ){ number_str++;
		char this_char;
		int pos_in_main_str = 0;
		len_token = 0;
		token_type = UNKNOWN_TOKEN;
		int in_quote = 0;
		
		
		while( this_char = str[pos_in_main_str++] ) { // обработка одной строки посимволно
			if(multyline_comment){
				if(this_char == '*' && str[pos_in_main_str] == '/'){
					multyline_comment = false;
					pos_in_main_str++;
				}
				continue;
			}
			if( isalpha( this_char ) ) {
				/*
				мы можем встретить символ в нескольких случаях:
				IDENT_TOKEN or STRING_TOKEN:
					просто запоминаем
				INT_NUM_TOKEN:
					кидаем ошибку
				OPERATION_TOKEN:
					сохраняем токен операций и начинаем записывать новый IDENT_TOKEN
				*/
				if( token_type == INT_NUM_TOKEN ) {//буква не может идти вплотную к числу: [ 42bar ] неверное выражение
					fprintf(error_stream, "ERROR: IDENT_TOKEN can`t start with number:\n  %d:\t%s\n", number_str, str);
					return NULL;
				} else if( token_type == OPERATION_TOKEN ) { // операции могут быть вплотную к словам: [ +=bar ] eq [ += bar ]
					_add_token();
					token_type = IDENT_TOKEN;
				}
				token_str[len_token++] = this_char;
				if(len_token == 1 && token_type != STRING_TOKEN) token_type = IDENT_TOKEN;
			} else if( isdigit(this_char) ) {
				
				/*
				мы можем встретить число в нескольких случаях:
				IDENT_TOKEN  or INT_NUM_TOKEN or STRING_TOKEN: [ bar42 ] or [ 42 ] or [ "42" ]
					просто запоминаем
				OPERATION_TOKEN:
					сохраняем токен операций и начинаем записывать новый INT_NUM_TOKEN
				*/
				if( token_type == OPERATION_TOKEN ) { // операции могут быть вплотную к числам: | +=bar | eq | += bar |
					_add_token();
					token_type = INT_NUM_TOKEN;
				}
				token_str[len_token++] = this_char;
				if(len_token == 1 && token_type != STRING_TOKEN) token_type = INT_NUM_TOKEN;
			} else if ( isspace(this_char) ) {
				/*
				мы можем встретить пробельный символ в двух случаях:
				STRING_TOKEN:
					просто запоминаем
				other :
					сохраняем текущий токен
				*/
				if( token_type == STRING_TOKEN )
					token_str[len_token++] = this_char;
				else
					_add_token();
					
			} else {
				//разнообразные символы===============================
				
				int this_is_quote = NOT_IN_QUOTE;
				if (this_char == '\'')  this_is_quote = IN_QUOTE;
				else if (this_char == '"') this_is_quote = IN_DOUBLE_QUOTE;
				else if ( this_char == '.' && token_type == INT_NUM_TOKEN) {
					count_point_in_num++;
					if(count_point_in_num > 1) {
						fprintf(error_stream, "ERROR: more than 1 point in number %d:\n  %s", number_str, str);
						return NULL;
					}
					token_str[len_token++] = this_char;

				}  else if( this_char == '/' && str[pos_in_main_str] == '/' && in_quote == NOT_IN_QUOTE) { // однострочный комментарий 1.02
					_add_token();
					break;
				} else if (this_char == '/' && str[pos_in_main_str] == '*') {
						multyline_comment = number_str;
						++pos_in_main_str;
				} else if( in_quote && this_char == '\\' && (str[pos_in_main_str] == '\'' || str[pos_in_main_str] == '"') ) {
						mark();
						token_str[len_token++] = str[pos_in_main_str];
						pos_in_main_str++;
						continue;
				} else if( isop(this_char) ){
					if( token_type != OPERATION_TOKEN && token_type != STRING_TOKEN ) {
						_add_token();
						token_type = OPERATION_TOKEN;
					}
					if( token_type != STRING_TOKEN ) token_str[len_token++] = this_char;//иначе в строке будет дублирование
				}
				
				
				if( this_is_quote ) {
					if( this_is_quote == in_quote ){
						in_quote = NOT_IN_QUOTE;
						_add_token();
					} else if(in_quote == NOT_IN_QUOTE) {
						in_quote = this_is_quote;
						_add_token();
						token_type = STRING_TOKEN;
					} else token_str[len_token++] = this_char;
				} else if (this_char == '_' && token_type != INT_NUM_TOKEN) {
					if ( token_type == OPERATION_TOKEN ) {
						_add_token();
						token_type = IDENT_TOKEN;
					}
					token_str[len_token++] = this_char;
				}else if( token_type == STRING_TOKEN ) token_str[len_token++] = this_char;
				
							
			
			}//other characters
		}//one string lex
		if(in_quote){
			fprintf(error_stream, "ERROR: no closing quote in string %d:\n  %s", number_str, str);
			return NULL;
		}
		if( numerator[this_line_pos] ) { this_line_pos++; numerator[this_line_pos] = 0; }
		if(numerator_capacity == this_line_pos + 1) { // расширение numerator`а
			numerator_capacity *= 2;
			numerator = realloc(numerator, sizeof(int) * numerator_capacity);
		}
	
	}//file lex
	
	if(multyline_comment){
			fprintf(error_stream, "ERROR: no closing multyline comment, that opend in str %d\n", multyline_comment);
			return NULL;
	}
	
	free(str);
	free(token_str);
	
	ALL_LEX_TOKENS* result = malloc( sizeof(ALL_LEX_TOKENS) );
	result->tokens = realloc(tokens, sizeof(LEX_TOKEN) * (count_tokens) );
	result->summary_count_tokens = count_tokens;
	result->count_token_lines = this_line_pos;
	result->count_tokens = realloc(numerator, sizeof(int) * this_line_pos);

	
	return result;
}

