#ifndef _TEST_H_
#define _TEST_H_

#include "../lexer/lexer.h"
#include "../AST/AST.h"
#include "../stat_analyzer/stat_analyzer.h"
#include "../compiler/compiler.h"

int test();
void write_tokens(FILE*, ALL_LEX_TOKENS*);
#endif
