#ifndef _STAT_ANALYZER_H_
#define _STAT_ANALYZER_H_

#include <stdio.h>
#include "../lexer/lexer.h"
#include "../AST/AST.h"
#include "../data_struct/data_struct.h"
#include "../std/std.h"

stack_t* stat_analyze(FILE*, stack_t*, hash_t*);
hash_t* make_std_function_hash();



#endif
