#ifndef _LEXER_H_
#define _LEXER_H_

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
#define UNKNOWN_TOKEN (64)


#define NOT_IN_QUOTE (0)
#define IN_QUOTE (1)
#define IN_DOUBLE_QUOTE (2)


#define true 1
#define false 0

typedef struct LEX_TOKEN {
	short type;
	short info;
	char* token;
} LEX_TOKEN;

typedef struct ALL_LEX_TOKENS {
	unsigned int summary_count_tokens;
	unsigned int count_token_lines;
	unsigned int* count_tokens; // в строке кода
	LEX_TOKEN* tokens;
} ALL_LEX_TOKENS;

void print_token(const LEX_TOKEN*, FILE*);
ALL_LEX_TOKENS* lex_analyze(const char*, FILE*);
void mark(void);


#endif
