#ifndef _LEXER_H_
#define _LEXER_H_

#include <stdio.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>

//##############################
#define COUNT_SPACE_IN_DEEP (4)
//##############################

#define MAX_LENGTH_STRING_IN_LANG_PROG (256)
#define STARTED_COUNT_TOKENS (64)
#define STARTED_COUNT_LINES (16)
#define EXPANSION_NUM (2)

#define INT_NUM_TOKEN (1)
#define FLOAT_NUM_TOKEN (2)
#define STRING_TOKEN (4)
	//все константные типы гарантированно меньше CONST_BORDER, все названия и операции меньше
	#define CONST_BORDER (6)
#define IDENT_TOKEN (8)
#define OPERATION_TOKEN (16)
#define RESERVED_TOKEN (32)
#define EXPR_STACK_TOKEN (64)
#define TRUE_BODY_TOKEN (128)
#define FALSE_BODY_TOKEN (256)
#define ARGS_TOKEN (512)
#define FUNC_CALL_TOKEN (1024)
#define UNKNOWN_TOKEN (2048)


#define NOT_IN_QUOTE (0)
#define IN_QUOTE (1)
#define IN_DOUBLE_QUOTE (2)


#define true 1
#define false 0


#define EQUAL 	(1)
#define LOG_OR	(2)
#define LOG_AND (3)
#define BIT_OR	(4)
#define BIT_XOR	(6)
#define BIT_AND	(6)
#define CMP		(7)
#define SHIFT 	(8)
#define ADD		(9)
#define MUL 	(10)
#define POW 	(11)
#define NOT		(12)
#define IN		(13)
#define DOT		(14)


typedef struct LEX_TOKEN {
	short type;
	short info;//для операций приоритет
	char* token;
} LEX_TOKEN;

typedef struct ALL_LEX_TOKENS {
	unsigned int summary_count_tokens;
	unsigned int count_token_lines;
	unsigned int* count_tokens; // в строке кода
	unsigned int* deeper; //отступ строки
	LEX_TOKEN* tokens;
} ALL_LEX_TOKENS;
#include "../syntax/syntax.h"
void print_token(const LEX_TOKEN*, FILE*);
ALL_LEX_TOKENS* lex_analyze(const char*, FILE*);
void mark(void);


#endif
