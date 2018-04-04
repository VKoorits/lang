#include "lexer.h"

/*
TODO TODO TODO: разбить на lex_analyze на подфункции
TODO защита от длинных строк в программе

TODO приравнивать ; к переносу строки
*/


//##################### GLOBAL_VARIABLES

static LEX_TOKEN *tokens;
static int* numerator;
static int* deeper;
static int count_tokens, len_token, token_type, this_line_pos;
static int tokens_capacity = STARTED_COUNT_TOKENS;
static char* token_str;
static int count_point_in_num = 0;
static int priorety; //для записи приоритета оператора
//#####################


void print_token(const LEX_TOKEN* tok, FILE* output_stream){
	if(tok) {
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
		fprintf(output_stream, "%s: %s, info => %d\n", token_type, tok->token, tok->info);
	} else {
		fprintf(output_stream, "NULL POINTER TO LEX_TOKEN\n");
	}
}


//TODO переделать в стэк
//добавляет токен в стэк
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

		int char_op_priorety = is_char_operator(token_str);
	if( token_type == IDENT_TOKEN  &&  char_op_priorety ) {//символьный оператор
		token_type = OPERATION_TOKEN;
		tokens[count_tokens].info = char_op_priorety;
	} else if( token_type == INT_NUM_TOKEN && count_point_in_num > 0 )//число с точкой
		token_type = FLOAT_NUM_TOKEN;
	else if(token_type == OPERATION_TOKEN) // обычная операция
		tokens[count_tokens].info = priorety;
	else {
		if( token_type == IDENT_TOKEN) {
			//номер+1 ключевого слова в массиве key_words(syntax.c)
			tokens[count_tokens].info = is_key_word(token_str);
		} else
			tokens[count_tokens].info = 0;
	}

	tokens[count_tokens].type = token_type;

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
	char *str = malloc( MAX_LENGTH_STRING_IN_LANG_PROG + 2 );//считывается из файла
	/*	В некоторых местах проверяется 1-2 следующих символа.
		Чтобы не делать проверки добавим два пробела в конце
		TODO: избавиться от этого костыля и сделать возможность считывания строк любой длины
			  или сделать ограничение длины строки (программа будет посимпатичнее)
	*/
	str[MAX_LENGTH_STRING_IN_LANG_PROG - 1] = ' ';
	str[MAX_LENGTH_STRING_IN_LANG_PROG ] = ' ';
	str[MAX_LENGTH_STRING_IN_LANG_PROG + 1] = '\n';
	tokens = malloc( sizeof(LEX_TOKEN)* STARTED_COUNT_TOKENS ); // токены для передачи обратно
	int numerator_capacity = STARTED_COUNT_LINES;
	numerator = malloc( sizeof(int) * STARTED_COUNT_LINES);
	deeper = malloc(sizeof(int) * STARTED_COUNT_LINES);
	deeper[0] = 0;
	numerator[0] = 0;
	this_line_pos = 0;
	//###############################
	count_tokens = 0;
	token_str = malloc(2000); // ТОЛЬКО ДЛЯ ПРОТОТИПА
	token_str[99] = '\0';
	int number_str = 0;
	int deep;

	int multyline_comment = false;
	while( fgets( str, MAX_LENGTH_STRING_IN_LANG_PROG, lang_prog ) ){ number_str++;
		char this_char;
		int pos_in_main_str = 0;
		len_token = 0;
		token_type = UNKNOWN_TOKEN;
		int in_quote = 0;

		deep = 0;
		if(!multyline_comment){ // считаем величину отступа
				if( str[pos_in_main_str] == ' ') {
					while (str[pos_in_main_str] == ' ') pos_in_main_str++;
					if( str[pos_in_main_str] == '\n') continue;
					if( pos_in_main_str % COUNT_SPACE_IN_DEEP != 0 ) {
						//если используются пробелы, то их кол-во должно быть пропорционально COUNT_SPACE_IN_DEEP
						fprintf(error_stream, "ERROR: wrong count space in shift of string:\n  %d:\t%s\n", number_str, str);
						return NULL;
					}
					if( str[pos_in_main_str] == '\t'){
						fprintf(error_stream, "ERROR: space and tab in shift of string:\n  %d:\t%s\n", number_str, str);
						return NULL;
					}
					deep = pos_in_main_str / 4;
				} else if( str[pos_in_main_str] == '\t') {
					while (str[pos_in_main_str] == '\t') pos_in_main_str++;
					if( str[pos_in_main_str] == '\n') continue;
					if( str[pos_in_main_str] == ' '){
						fprintf(error_stream, "ERROR: tab and space in shift of string:\n  %d:\t%s\n", number_str, str);
						return NULL;
					}
					deep = pos_in_main_str;
				}
		}

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

				}  else if( this_char == '#' && in_quote == NOT_IN_QUOTE) { // однострочный комментарий 1.02
					_add_token();
					break;
				} else if (this_char == '/' && str[pos_in_main_str] == '*') {
						multyline_comment = number_str;
						++pos_in_main_str;
				} else if( in_quote && this_char == '\\' && (str[pos_in_main_str] == '\'' || str[pos_in_main_str] == '"') ) {
						token_str[len_token++] = str[pos_in_main_str];
						pos_in_main_str++;
						continue;
				} else if( priorety = isop(this_char) ){
					if( token_type != OPERATION_TOKEN && token_type != STRING_TOKEN ) {
						_add_token();
						//printf("Started operator: %c", this_char);
						//TODO проверка на выход за пределы
						//TODO не только здесь

						//функция get_op() изменяет token_str
						token_str[0] = this_char; token_str[1] = str[pos_in_main_str];
						token_str[2] = str[pos_in_main_str+1]; token_str[3] = '\0';
						priorety = get_op(&pos_in_main_str, priorety, token_str, &len_token);

						token_type = OPERATION_TOKEN;
						_add_token();
					}
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
					if ( token_type == OPERATION_TOKEN || token_type == UNKNOWN_TOKEN ) {
						_add_token();
						token_type = IDENT_TOKEN;
					}
					token_str[len_token++] = this_char;
				}else if( token_type == STRING_TOKEN ) token_str[len_token++] = this_char;



			}//other characters
		}//one string lex
		if(in_quote){
			//TODO сделать возможность введения больших строк
			fprintf(error_stream, "ERROR: no closing quote in string %d:\n  %s", number_str, str);
			return NULL;
		}
		if( numerator[this_line_pos] ) {
			if(this_line_pos == 0){
				if (deep > 0){
					fprintf(error_stream, "ERROR: file start with shift %d:\n  %s", number_str, str);
					return NULL;
				}
			}else if (deep - deeper[this_line_pos-1] > 1) {
				fprintf(error_stream, "ERROR: very big shift %d:\n  %s", number_str, str);
				return NULL;
			}
			deeper[this_line_pos] = deep;
			this_line_pos++;
			numerator[this_line_pos] = 0;
		}
		if(numerator_capacity == this_line_pos + 1) { // расширение numerator`а
			numerator_capacity *= EXPANSION_NUM;
			numerator = realloc(numerator, sizeof(int) * numerator_capacity);
			deeper = realloc(deeper, sizeof(int) * numerator_capacity);
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
	result->deeper = realloc(deeper, sizeof(int) * this_line_pos);

	return result;
}
