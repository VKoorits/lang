#ifndef _LEXER_H_
#define _LEXER_H_

typedef struct LEX_TOKEN {
	int index;
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


#endif
