#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "lexer/lexer.h"
#include "test/test.h"
#include "AST/AST.h"
#include "stat_analyzer/stat_analyzer.h"


int main(int argc, char* argv[]){
	if ( argc > 1 ) {
		if ( !strcmp( argv[1], "-test") ) {
			test();
		} else {
			printf("UNKNOWN PARAMETR %s\n", argv[1]);
			printf("THERE ARE PARAMETRS:\n\t-test\n");
			return 0;
		}
	} else {	
		
		FILE* out = stderr;
		ALL_LEX_TOKENS* all_token = lex_analyze("lang_code/file.l", out);
		if(all_token){
			//write_tokens(stdout, all_token);
			hash_t* functions = hash_new();
			stack_t* big_stack = build_AST(all_token, out, functions);	
			if(big_stack) {
				stack_t* analyze_res = stat_analyze(out, big_stack, functions);
				if( analyze_res )
					print_stack(stdout, analyze_res, 0);
				//hash_each_val( functions, {print_function(out, val);});
			}
		}
	}
}

