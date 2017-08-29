#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "lexer/lexer.h"
#include "test/test.h"
#include "AST/AST.h"
#include "stat_analyzer/stat_analyzer.h"
#include "compiler/compiler.h"

int create_prog(FILE* out, char* filename) {
	ALL_LEX_TOKENS* all_token = lex_analyze(filename, out);
	if(all_token){
		//write_tokens(stdout, all_token);
		hash_t* functions = hash_new();
		stack_t* big_stack = build_AST(all_token, out, functions);	
		if(big_stack) {
			stack_t* analyze_res = stat_analyze(out, big_stack, functions);
			if( analyze_res ){
				int name_len = strlen(filename);
				char* filename_code = malloc(name_len + 2 );
				strcpy(filename_code, filename);
				filename_code[name_len] = 'c';
				filename_code[name_len+1] = '\0';
				
				compile(filename_code, stderr, analyze_res, functions);
			}
		}
	}
}


int main(int argc, char* argv[]){
	FILE* out = stderr;
	if ( argc > 1 ) {
		if ( !strcmp( argv[1], "-test") ) {
			test();
		} else if( !strcmp( argv[1], "-to_op_code") ) {
			if(argc <= 2)
				fprintf(out, "ERROR: no file for decompile\n");
			else{
				write_op_codes(stdout, argv[2]);
			}
		} else {
			create_prog(out, argv[1]);
		}
	} else {
		fprintf(out, "ERROR: compiller called without parametrs\n");
	}
}

