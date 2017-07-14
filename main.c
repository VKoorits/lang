#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "lexer/lexer.h"
#include "test/test.h"
#include "AST/AST.h"


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
		
		FILE* f = stderr;
		ALL_LEX_TOKENS* all_token = lex_analyze("lang_code/file.l", f);
		if(all_token){
			stack_t* big_stack = build_AST(all_token, f);	
			
			//
			if(big_stack)
				print_stack(stdout, big_stack, 0);
			//	
		}
	}
}

