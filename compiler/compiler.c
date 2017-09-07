#include "compiler.h"
#include "byte_codes.h"

// GLOBAL VAR ###################
extern hash_t* std_func;
//###############################



stack_t* compile_recursive(FILE* code, FILE* out, stack_t* big_stack, hash_t* functions, int deep,
							int* var_count, int* const_count, stack_t* constants, hash_t* var_id, hash_t* const_id, int new_namespace){
	if( !constants) constants = stack_new();
	if( !var_id ) var_id = hash_new();
	if( !const_id ) const_id = hash_new();
	
	int count_var_in_namespace = *var_count;
	long int position_NEW;
	int some_var = 666;//какая-то переменная
//#############################################
	//goto var
	//нет возможности вкладывадь блоки друг в друга ??????
	int begin_expr = 0;//первая команда условия
	int begin_true_body = 0;//первая команда true_body
	int end_true_body = 0;//первая команда после true_body
	
//##################################################333
	
	
	if(new_namespace) {
		fwrite(&NEW_VAR, sizeof(char), 1, code);
		position_NEW = ftell(code);		
		fwrite(&some_var, sizeof(int), 1, code);
	}
	
	for(int i=0; i < big_stack->size; i++){
	#define TOKEN_I ((LEX_TOKEN*)big_stack->val[i])
		if( TOKEN_I->type == INIT_VAR_TOKEN) {
			int prev_type = UNKNOWN_TOKEN;
			for(int j = 0; j < ((stack_t*)(TOKEN_I->token))->size; j++){
				if( ((LEX_TOKEN*)(((stack_t*)(TOKEN_I->token))->val[j]))->type == IDENT_TOKEN ){
					char* var_name = ((LEX_TOKEN*)(((stack_t*)(TOKEN_I->token))->val[j]))->token;					
						
					hash_set(var_id, var_name, (void*)*var_count );
					++*var_count;					
						
					prev_type = IDENT_TOKEN;
				} else {
					stack_t* val = (stack_t*)(((LEX_TOKEN*)(((stack_t*)(TOKEN_I->token))->val[j]))->token);
									
					//кладем переменную, которой принадлежит значение
					fwrite(&PUSH, sizeof(char), 1, code);
					--*var_count;
					fwrite(var_count, sizeof(int), 1, code);
					++*var_count;
					
					compile_recursive(code, out, val, functions, deep+1, var_count, const_count, constants, var_id, const_id, 0);			
					
					fwrite(&STORE, sizeof(char), 1, code);
					
					prev_type = LIST_EL_TOKEN;
				}
			}
		} else if ( TOKEN_I->type == IDENT_TOKEN && TOKEN_I->info == 0) {//переменная
			int var_index = (int)hash_get(var_id, TOKEN_I->token);
			fwrite(&PUSH, sizeof(char), 1, code);
			fwrite(&var_index, sizeof(int), 1, code);
		} else if ( TOKEN_I->type == OPERATION_TOKEN ) {
			if( TOKEN_I->token[1] == '\0' ){
				switch(TOKEN_I->token[0]) {
					case '+':
						fwrite(&BINARY_ADD, sizeof(char), 1, code);
						break;
					case '-': 
						fwrite(&BINARY_SUB, sizeof(char), 1, code);
						break;
					case '*':
						fwrite(&MULTIPLY, sizeof(char), 1, code);
						break;
					case '/':
						fwrite(&DIVISION, sizeof(char), 1, code);
						break;
					case '%':
						fwrite(&MOD, sizeof(char), 1, code);
						break;
					case '<':
						fwrite(&LESS, sizeof(char), 1, code);
						break;
					case '>':
						fwrite(&MORE, sizeof(char), 1, code);
						break;
					case '=':
						fwrite(&STORE, sizeof(char), 1, code);
						break;
					default :
						break;					
				}
			}
		} else if ( 
			TOKEN_I->type == STRING_TOKEN ||
			TOKEN_I->type == INT_NUM_TOKEN ||
			TOKEN_I->type == FLOAT_NUM_TOKEN
		) {
			char* unique_val = malloc( 6 + strlen(TOKEN_I->token) );//6 - max длина номера типа + 3( '@' + '#' + '\0')
			sprintf(unique_val, "@%d#%s", TOKEN_I->type, TOKEN_I->token);			
			
			if( (int)hash_get(const_id, unique_val ) == 0 ){//новая константа
				hash_set(const_id, unique_val, (void*)*const_count );
				//переменные, значения которых известны сразу BEGIN
				st_push(constants, TOKEN_I);
				++*const_count;
			}
			int const_num = (int)hash_get(const_id, unique_val );
			fwrite(&PUSH_CONST, sizeof(char), 1, code);
			fwrite(&const_num, sizeof(int), 1, code);
		} else if (TOKEN_I->type == IDENT_TOKEN) {
			//DANGER: может попасть не ключевое слово, а что-то другое(function_name)
			switch(TOKEN_I->info) {
				case IF_ID:
					fseek(code, begin_true_body+1, SEEK_SET);// (+1) для записи JUMP_IF_NOT
					fwrite(&end_true_body, sizeof(int), 1, code);
					fseek(code, 0, SEEK_END);
				  break;
				case WHILE_ID:
					fwrite(&GOTO, 1, 1, code);
					fwrite(&begin_expr, sizeof(int), 1, code);
					end_true_body += 1 + sizeof(int);
					
					fseek(code, begin_true_body+1, SEEK_SET);// (+1) для записи JUMP_IF_NOT
					fwrite(&end_true_body, sizeof(int), 1, code);
					fseek(code, 0, SEEK_END);
				  break;
			}
		} else if (TOKEN_I->type == EXPR_STACK_TOKEN ) {
			begin_expr = (int)ftell(code);
			compile_recursive(code, out, (stack_t*)TOKEN_I->token, functions, deep+1,
							var_count, const_count, constants, var_id, const_id, 0);					
		} else if (TOKEN_I->type == TRUE_BODY_TOKEN) {
			begin_true_body  = (int)ftell(code);

			fwrite(&JUMP_IF_NOT, 1, 1, code);
			long int jump_from = ftell(code);
			fwrite(&some_var, sizeof(int), 1, code);
			
			compile_recursive(code, out, (stack_t*)TOKEN_I->token, functions, deep+1,
							var_count, const_count, constants, var_id, const_id, 1);
			
			end_true_body = (int)ftell(code);		
		} else if (TOKEN_I->type == FALSE_BODY_TOKEN) {
			fwrite(&GOTO, 1, 1, code);
			fwrite(&some_var, sizeof(int), 1, code);//переход к первой инструкции находящейся после FALSE_BODY
			
			
			compile_recursive(code, out, (stack_t*)TOKEN_I->token, functions, deep+1,
							var_count, const_count, constants, var_id, const_id, 1);
			
			int after_else = (int)ftell(code);
			fseek(code, end_true_body+1, SEEK_SET);//(+1) так как sizeof(GOTO) = 1
			fwrite(&after_else, sizeof(int), 1, code);
			fseek(code, 0, SEEK_END);
			
			end_true_body += 1 + sizeof(int);// GOTO + $arg
			
		} else if (TOKEN_I->type == FUNC_CALL_TOKEN ) {
			stack_t* list_arg = (stack_t*)((LEX_TOKEN*)(((stack_t*)(TOKEN_I->token))->val[0]))->token;
			for(int i=0; i<list_arg->size; i++) {
				//заносим аргументы функции на стек
				stack_t* elem = (stack_t*)(((LEX_TOKEN*)list_arg->val[i])->token);
				compile_recursive(code, out, elem, functions, deep+1,
							var_count, const_count, constants, var_id, const_id, 0);
				
			}
			 
			
			
			int sub_index = -1;
			
			if( TOKEN_I->info == STD_FUNCTION ) {
				char* sub_name = ((LEX_TOKEN*)(((stack_t*)(TOKEN_I->token))->val[1]))->token;
				sub_index = (int)hash_get(std_func, sub_name) - 1;
			}
			
			fwrite(&CALL_STD, 1, 1, code);
			fwrite(&sub_index, sizeof(int), 1, code);
			fwrite(&list_arg->size, sizeof(int), 1, code);	
			
		} else {
			printf("UNKNOWN_TOKEN in compiler %d\n", TOKEN_I->type);
		}
	#undef TOKEN_I
	}
	
	
	if(new_namespace) {
		fwrite(&DELETE_VAR, sizeof(char), 1, code);
		count_var_in_namespace = *var_count - count_var_in_namespace;
		fwrite(&count_var_in_namespace, sizeof(int), 1, code);

		fseek(code, position_NEW, SEEK_SET);
		fwrite(&count_var_in_namespace, sizeof(int), 1, code);
		fseek(code, 0, SEEK_END);
		
	}
	
	if(deep == 0) {
		fwrite(&EXIT, 1, 1, code);
	}
	
	return constants;
}




int compile(char* code_filename, FILE* out, stack_t* big_stack, hash_t* functions){
	FILE* code = fopen(code_filename, "wb+");
		int var_count = 1, const_count = 1;
		int offset_to_const = 0;
		fwrite(&offset_to_const, sizeof(int), 1, code);//будет перезаписано
		stack_t* constants = compile_recursive(code, out, big_stack, functions, 0, &var_count, &const_count, NULL, NULL, NULL, 1);
		offset_to_const = (int)ftell(code);
		
				
		fseek(code, 0, SEEK_SET);
		fwrite(&offset_to_const, sizeof(int), 1, code);
		fseek(code, offset_to_const, SEEK_SET);
		
		//о структуре секции констант написанов compiler.h
		fwrite(&(constants->size), sizeof(int), 1, code);
		//сами константы
		for(int i=0; i<constants->size; i++){			
			LEX_TOKEN* tok = constants->val[i];	
			int token_len = strlen(tok->token);
			
			fwrite(&tok->type,  sizeof(int), 1, code);
			fwrite(&token_len, sizeof(int), 1, code);//длина строкового представления константы
			fwrite(tok->token, sizeof(char), token_len, code);
		}
	fclose(code);
	return 0;
}












