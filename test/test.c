#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "test.h"
/*
TODO: убрать ограничение на длину строки MAX_LENGTH_STRING_IN_TOKEN_FILE, т.к токеном может быть большая строка
*/

#define MAX_LENGTH_STRING_IN_TOKEN_FILE (512)
static const char* TEST_PATH = "test/test_data/";

char* cat_path(char* path, char* dir, char* filename, char* extension){
	int len = strlen(TEST_PATH) +
						strlen(path) +
						strlen(dir) +
						strlen(filename) +
						1 + // for '.'
						strlen(extension)+1;

	char* res = malloc( len	);
	strcpy(res, TEST_PATH);
	strcat(res, path);
	strcat(res, dir);
	strcat(res, filename);
	strcat(res, ".");
	strcat(res, extension);
	res[len] = '\0';

	return res;
}



void write_tokens(FILE* f, ALL_LEX_TOKENS* all_token){
	int k=0;
	for(int i=0; i< (all_token->count_token_lines); i++){
		for(int j=0; j < (all_token->count_tokens[i]); j++)
			print_token(&all_token->tokens[k++], f);
		fprintf(f,"#\n");
	}
	fprintf(f,"%d\n", all_token->summary_count_tokens);
}

char** file_eq(char* got_path, char* expected_path) {
	FILE* got = fopen(got_path, "r");
	char** res = malloc(3*sizeof(char*)); // если оба файла откроются, то res изменится
	res[0] = (char*)-1;
	if (got) {
		FILE* exp = fopen(expected_path, "r");
		if(exp){
			//#### если оба файла открыты
			long num_str = 1;
			char got_str[MAX_LENGTH_STRING_IN_TOKEN_FILE],
				 exp_str[MAX_LENGTH_STRING_IN_TOKEN_FILE];

			//построчное сравнивание содержимого файлов
			res[0] = (char*)0; //если есть несовпадение, то res изменится
			while(!feof(got) && !feof(exp)){
				fgets(got_str, MAX_LENGTH_STRING_IN_TOKEN_FILE, got);
				fgets(exp_str, MAX_LENGTH_STRING_IN_TOKEN_FILE, exp);
				if( strcmp(got_str, exp_str) ) {
					res[0] = (char*)num_str;
					res[1] = got_str;
					res[2] = exp_str;
				  fclose(got);
				  fclose(exp);
					break;
				}
				num_str++;
			}
			//####
		} else
			fprintf(stderr, "TEST ERROR: in sub'file_eq' Can`t open file1 %s\n", expected_path);
	} else
		fprintf(stderr, "TEST ERROR: in sub'file_eq' Can`t open file2 %s\n", got_path);
	return res;
}

void test_lexer(char* filename, FILE* out){
	ALL_LEX_TOKENS* all_token = lex_analyze( filename, out);
	if(all_token){
		write_tokens(out, all_token);
	}
}
void test_AST_builder(char* filename, FILE* out){
	ALL_LEX_TOKENS* all_token = lex_analyze( filename, out);
	if(all_token){
		hash_t* functions = hash_new();
		stack_t* big_stack = build_AST(all_token, out, functions);
		if(big_stack){
			print_stack( out, big_stack, 0);
			hash_each_val( functions, {
				print_function(out, val);
			});
		}
	}
}

void test_stat_analyze(char* filename, FILE* out) {
	ALL_LEX_TOKENS* all_token = lex_analyze( filename, out);
	if(all_token){
		hash_t* functions = hash_new();
		stack_t* big_stack = build_AST(all_token, out, functions);
		if(big_stack){
			stack_t* analyze_res = stat_analyze(out, big_stack, functions);
			if( analyze_res )
				print_stack(out, analyze_res, 0);
		}
	}
}

void test_compiler(char* filename, FILE* out) {
	ALL_LEX_TOKENS* all_token = lex_analyze(filename, out);
	if(all_token){
		//write_tokens(stdout, all_token);
		hash_t* functions = hash_new();
		stack_t* big_stack = build_AST(all_token, out, functions);
		if(big_stack) {
			stack_t* analyze_res = stat_analyze(out, big_stack, functions);
			if( analyze_res ){
				//добавляем символ 'c' в конце расширения файла
				int name_len = strlen(filename);
				char* filename_code = malloc(name_len + 2 );
				strcpy(filename_code, filename);
				filename_code[name_len] = 'c';
				filename_code[name_len+1] = '\0';

				compile(filename_code, out, analyze_res, functions);
				write_op_codes(out, filename_code);
				remove(filename_code);
			}
		}
	}
}


int fio_test(char* path, char* filename, void (*tested_func)(char*,FILE*) ) {
	/*
		file input ouput _ test
		вывод перенаправляется в файл и сравнивается с ожидаемым файлом
	*/
	char* full_input_filename = cat_path(path, "input/", filename, "l");
	char* full_output_filename = cat_path(path, "output/", filename, "txt");
	FILE* out = fopen(full_output_filename, "w"); // для записи ошибок
	if( !out ){
		fprintf(stderr, "TEST ERROR: in sub'file_eq' Can`t open file %s\n", full_output_filename);
		return 0;
	}

	tested_func(full_input_filename, out);
	fclose(out);


	char* full_expected_filename = cat_path(path, "expected/", filename, "txt");
	char ** res = file_eq(full_output_filename, full_expected_filename);
	if( (long)res[0] == -1 ) {
		printf("\n\tTEST ERROR: can`t open file in sub'fio_test'");
		return 0;
	} else if( (long)res[0] > 0 ) {
		printf("\n\tThe files differ in str %li\n", (long)res[0]);
		printf("\tGot     :%s", res[1]);
		printf("\tExpected:%s", res[2]);
		return 0;
	}

	return 1;
}

int test() {
	int cnt_test_ok = 0;

	char* lexer_filenames[] = {
		"normal_script1",
		"two_point_in_num",
		"unreal_file",
		"no_closing_quote",
		"unclosed_comment",
		"wrong_ident_token",
		"tab_and_space",
		"wrong_space",
		"space_and_tab",
		"big_shift",
		"shift_start"
	};
	printf("LEXER_TEST:\n");
	for(int i = 0; i < sizeof(lexer_filenames) / sizeof(char*); i++) {
		printf("	TEST %25s: ", lexer_filenames[i]);
		int res = fio_test("lexer/", lexer_filenames[i], test_lexer);
		if(res == 1){ printf("OK\n"); ++cnt_test_ok; }
	}
	for(int i = 0; i<40; i++) printf("="); printf("\n");

	//AST
	char* AST_filenames[] = {
		"normal_script",
		"with_func1_call",
		"with_func_many_call",
		"no_ident_header",
		"wrong_brackets_header",
		"wrong_deep_end",
		"else_without_if",
		"wrong_else",
		"no_open_bracket",
		"unclosed_bracket_expr",
		"wrong_bracket_seq",
		"unclosed_bracket_func",
		"func_like_arg",
		"normal_data_struct",
		"expected_fat_comma",
		"comma_between_pair",
		"fat_comma_in_arr",
		"empty_element",
		"wrong_hash_size",
		"two_operations",
		"num_and_str_near",
		"init_var"
	};

	printf("AST_TEST:\n");
	for(int i = 0; i < sizeof(AST_filenames) / sizeof(char*); i++) {
		printf("	TEST %25s: ", AST_filenames[i]);
		int res = fio_test("AST/", AST_filenames[i], test_AST_builder);
		if(res == 1){ printf("OK\n"); ++cnt_test_ok; }
	}
	for(int i = 0; i<40; i++) printf("="); printf("\n");

	char* stat_analyze_filenames[] = {
		//"undeclared_function",
		//"undeclared_function2",
		"normal_var_script",
		"no_var_declaration",
		"other_namespace",
		"no_var_from_prev_space"
	};

	printf("STAT_ANALYZE_TEST:\n");

	for(int i = 0; i < sizeof(stat_analyze_filenames) / sizeof(char*); i++) {
		printf("	TEST %25s: ", stat_analyze_filenames[i]);
		int res = fio_test("stat_analyze/", stat_analyze_filenames[i], test_stat_analyze);

		if(res == 1){ printf("OK\n"); ++cnt_test_ok; }
	}
	for(int i = 0; i<40; i++) printf("="); printf("\n");

	char* compiler_filenames[] = {
		"simple_file",
		"if_block",
		"while_block",
		"else_block",
		"multy_block"
	};
	printf("COMPILER_TEST:\n");
	for(int i = 0; i < sizeof(compiler_filenames) / sizeof(char*); i++) {
		printf("	TEST %25s: ", compiler_filenames[i]);
		int res = fio_test("compiler/", compiler_filenames[i], test_compiler);
		if(res == 1){ printf("OK\n"); ++cnt_test_ok; }
	}
	for(int i = 0; i<40; i++) printf("="); printf("\n");


	printf("\n");
	int count_test = (
		sizeof(lexer_filenames) +
		sizeof(AST_filenames) +
		sizeof(stat_analyze_filenames) +
		sizeof(compiler_filenames)
	)/sizeof(char*);
	if( count_test == cnt_test_ok )
		printf("All %d tests successfully passed :D\n", count_test);
	else
		printf("fail %d test from %d :G\n", count_test-cnt_test_ok, count_test);
}
