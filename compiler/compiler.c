#include "compiler.h"
#include "byte_codes.h"

#define PBC if(0)


stack_t* compile_recursive(FILE* code, FILE* out, stack_t* big_stack, hash_t* functions, int deep,
							int* var_count, int* const_count, stack_t* constants, hash_t* var_id, hash_t* const_id, int new_namespace){
	if( !constants) constants = stack_new();
	if( !var_id ) var_id = hash_new();
	if( !const_id ) const_id = hash_new();
	
	int count_var_in_namespace = *var_count;
	long int position_NEW;
	
	if(new_namespace) {
		fwrite(&NEW_VAR, sizeof(char), 1, code);
		long int position_NEW = ftell(code);
		fwrite(var_count, sizeof(int), 1, code);
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
					PBC printf("PUSH %d\n", *var_count-1);
					
					compile_recursive(code, out, val, functions, deep+1, var_count, const_count, constants, var_id, const_id, 0);			
					
					fwrite(&STORE, sizeof(char), 1, code);
					PBC printf("STORE\n");
					
					prev_type = LIST_EL_TOKEN;
				}
			}
		} else if ( TOKEN_I->type == IDENT_TOKEN && TOKEN_I->info == 0) {//переменная
			int var_index = (int)hash_get(var_id, TOKEN_I->token);
			fwrite(&PUSH, sizeof(char), 1, code);
			fwrite(&var_index, sizeof(int), 1, code);
			PBC printf("PUSH %d\t\t//var(%s)_id\n", (int)hash_get(var_id, TOKEN_I->token), TOKEN_I->token );
		} else if ( TOKEN_I->type == OPERATION_TOKEN ) {
			if( TOKEN_I->token[1] == '\0' ){
				switch(TOKEN_I->token[0]) {
					case '+':
						fwrite(&BINARY_ADD, sizeof(char), 1, code);
						PBC printf("BINARY_ADD\n"); 
						break;
					case '-': 
						fwrite(&BINARY_SUB, sizeof(char), 1, code);
						PBC printf("BINARY_SUB\n");
						break;
					case '=':
						fwrite(&STORE, sizeof(char), 1, code);
						PBC printf("STORE\n");
						break;
					default :
						PBC printf("OPERATION\n") ;
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
				//st_push(constants, (void*)*const_count );
				//END
				++*const_count;
			}
			int const_num = (int)hash_get(const_id, unique_val );
			fwrite(&PUSH_CONST, sizeof(char), 1, code);
			fwrite(&const_num, sizeof(int), 1, code);
			
			PBC printf("PUSH_CONST %d\t\t//const=%s\n", const_num, TOKEN_I->token );
		}
	#undef TOKEN_I
	}
	
	if(new_namespace) {
		fwrite(&DELETE_VAR, sizeof(char), 1, code);
		count_var_in_namespace = *var_count - count_var_in_namespace;
		fwrite(&count_var_in_namespace, sizeof(int), 1, code);
		
		int delta = ftell(code) - position_NEW;
		fseek(code, -delta, SEEK_CUR);
		fwrite(&count_var_in_namespace, sizeof(int), 1, code);
		fseek(code, delta, SEEK_CUR);
		
	}
	
	return constants;
}




int compile(char* code_filename, FILE* out, stack_t* big_stack, hash_t* functions){
	PBC printf("@@@@@@@@@@@@@@@@\n");
	
	FILE* code = fopen("file.bc", "wb+");
		int var_count = 1, const_count = 1;
		stack_t* constants = compile_recursive(code, out, big_stack, functions, 0, &var_count, &const_count, NULL, NULL, NULL, 1);	
		
		long int without_constant_size = ftell(code);
		fseek(code, 0, SEEK_SET);
				
		//копируем в память сгенерированный байт-код
		//TODO: копирование файла
		void* byte_code = malloc( without_constant_size );
		fread(byte_code, without_constant_size, 1, code);
	fclose(code);
		
	FILE* all_code = fopen(code_filename, "wb+");
		//о структуре секции констант написанов compiler.h
		int offset_to_code = 0;
		fwrite(&offset_to_code, sizeof(int), 1, all_code);
		fwrite(&(constants->size), sizeof(int), 1, all_code);
		PBC printf("count_constant: %d\n", constants->size);
		
		//сами константы
		for(int i=0; i<constants->size; i++){			
			LEX_TOKEN* tok = constants->val[i];	
			int token_len = strlen(tok->token);
			
			fwrite(&tok->type,  sizeof(int), 1, all_code);
			fwrite(&token_len, sizeof(int), 1, all_code);//длина строкового представления константы
			fwrite(tok->token, sizeof(char), token_len, all_code);
			
			PBC printf("len: %d, tok: %s\n", token_len, tok->token);
		}
		offset_to_code = ftell(all_code);
			
		//дозаписываем байт-код
		//TODO копирование файла
		fwrite(byte_code, without_constant_size, 1, all_code);
		
		fseek(all_code, 0, SEEK_SET);
		fwrite(&offset_to_code, sizeof(int), 1, all_code);

		PBC {
			fseek(all_code, 0, SEEK_END);
			without_constant_size = ftell(all_code);
			fseek(all_code, 0, SEEK_SET);
			free(byte_code);
			byte_code = malloc( without_constant_size );
			fread(byte_code, without_constant_size, 1, all_code);
			for(int i=0; i < without_constant_size; i++){
				char ch = *((char*)byte_code + i);
				printf("%c ", ch );
			}
			printf("\nEND\n");
		}
		free(byte_code);
	fclose(all_code);
	remove("file.bc");
	return 0;
}












