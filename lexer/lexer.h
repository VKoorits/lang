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
#define STRING_TOKEN (3)
	//все константные типы гарантированно меньше CONST_BORDER, все названия и операции больше
	#define CONST_BORDER (4)
#define IDENT_TOKEN (5)
#define OPERATION_TOKEN (6)
#define RESERVED_TOKEN (7)
#define EXPR_STACK_TOKEN (8)
#define TRUE_BODY_TOKEN (9)
#define FALSE_BODY_TOKEN (10)
#define LIST_TOKEN (11)
#define LIST_EL_TOKEN (12)
#define ARRAY_TOKEN (13)
#define HASH_TOKEN (14)
#define FUNC_CALL_TOKEN (15)
#define UNKNOWN_TOKEN (16)


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
