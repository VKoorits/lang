#include "interpretator.h"

#define GET_INT (*((int*)code));code+=sizeof(int);
#define GET_BYTE (*((char*)code));code+=sizeof(char);
#define BREAK run_programm=0;break;

//TODO memory_leak устранить
//TODO less and more работа с double

//GLOBAL_VAR ######################################
//CONSTANTS #####################
int const_count;
var_t* constants;
//###############################

//VAR_STACK #####################
var_t* var_stack;
int var_stack_size = 0;
int var_stack_capacity = STARTED_COUNT_VAR_IN_MAIN_STACK;
//###############################

// основной стек, на котором происходят все операции
stack_t* main_stack;

//STANDART LIBRARY
extern void (*std_functions[COUNT_STD_FUNCTION])(int);
//#################################################


static var_t* read_const_from_byte_code(FILE* out, char* const code_begin, unsigned int size_code, int* count_const) {
	char* code = code_begin;
	var_t* constants;
	{
		//в первых 4 байтах кода записано смещение к константам
		int offset_to_const = *((int*)code);
		
		code += offset_to_const;
		*count_const = GET_INT;
		
		constants = calloc(sizeof(var_t), *count_const);
		for(int i=0; i<*count_const; i++) {
			constants[i].type = GET_INT;
			int const_len = GET_INT;
			
			constants[i].val = malloc(const_len+1);
			strncpy(constants[i].val, code, const_len);
			code += const_len;
			((char*)constants[i].val)[const_len] = '\0';
			
			void* old_val = constants[i].val;
			//TODO! это должно делаться на этапе компиляции
			switch(constants[i].type) {
				case INT_NUM_TOKEN:
					constants[i].val = (void*)atoi(old_val);
					free(old_val);
				  break;
				case FLOAT_NUM_TOKEN:
					constants[i].val = malloc(sizeof(double));
					(*(double*)constants[i].val) = atof(old_val);
					free(old_val);
			}
		}		
	}
	
	return constants;
}

static int interpretate(FILE* out, char* const code_begin, unsigned int size_code) {

	constants = read_const_from_byte_code(out, code_begin, size_code, &const_count);	

	var_stack = calloc(sizeof(var_t), STARTED_COUNT_VAR_IN_MAIN_STACK);
	var_stack_size = 0;
	var_stack_capacity = STARTED_COUNT_VAR_IN_MAIN_STACK;

	main_stack = stack_new();
	
	
	
	char* code = code_begin + 4;
	
	char byte;
	int arg;
	int i;
	
	int run_programm = 1;
	while(run_programm) {
		byte = GET_BYTE;
		switch(byte) {
			case _EXIT: {
				run_programm = 0;
				break;
			}
			case _NEW_VAR: {
				// 1:подгоняем размер; 2:записываем новые переменные; 3:увеличиваем счетчик
				arg = GET_INT;//количество новых переменных
				
				if(var_stack_capacity < var_stack_size + arg){
					var_stack = realloc(var_stack, sizeof(var_t)*(var_stack_size + arg) );
					var_stack_capacity = var_stack_size + arg;
				}
				
				for(i = var_stack_size; i < var_stack_size + arg; ++i){
					var_stack[i].type = NONE_TYPE;
					var_stack[i].val = NULL;
				}
				
				var_stack_size += arg;
			  break;
			}
			case _DELETE_VAR: {
				var_stack_size -= GET_INT;
			  break;
			}
			case _PUSH: {
				arg = GET_INT;
				st_push(main_stack, var_stack + arg - 1);//(-1) потому что ты начал номера с единицы
				break;
			}
			case _PUSH_CONST: {
				arg = GET_INT;
				st_push(main_stack, constants + arg - 1);//(-1) потому что ты начал номера с единицы
				break;
			}
			case _STORE: {
				var_t* from = st_pop(main_stack);
				var_t* where = st_pop(main_stack);
				
				where->type = from->type;
				switch(where->type) {
					case NONE_TYPE:
					case INT_TYPE:
					case BOOL_TYPE:
						where->val = from->val;
					  break;
					case FLOAT_TYPE:
						where->val = malloc(sizeof(double));
						*(double*)where->val = *(double*)from->val;
					  break;
					case STRING_TYPE:
						printf("TODO: do sting data_struct!!!\n");
					  BREAK;
					default:
						printf("UNDEFINED VAR_TYPE in STORE line:%d\n", __LINE__);
					  BREAK;
				}
				
				break;
			}
			case _BINARY_ADD: {
				//MEMORY_LEAK: res помещается на стек, но память не освобождается при удалении со стека
				var_t* res = malloc(sizeof(var_t));
				var_t* first = st_pop(main_stack);
				var_t* second = st_pop(main_stack);
				
				if(first->type != second->type) {
					//TODO доп проверки, на совместимость типов для сложения					
					printf("UNCORRECT TYPE OPERATORS FOR BINARY_ADD %d and %d\n", first->type, second->type);
					BREAK;										
				}
				
				res->type = first->type;
				switch(res->type) {
					case NONE_TYPE: {
						printf("ERROR: NONE_TYPE + NONE_TYPE == ERROR\n");
						BREAK;
					}
					case INT_TYPE: {
						res->val = (void*)( (int)second->val + (int)first->val );
					  break;
					}
					case FLOAT_TYPE: {
						res->val = malloc(sizeof(double));
						*(double*)res->val = *(double*)second->val + *(double*)first->val;
					  break;
					}
					case STRING_TYPE: {
						printf("TODO: do sting data_struct!!!\n");
					  BREAK;
					}
					default:
						printf("UNDEFINED VAR_TYPE in BINARY_ADD line:%d\n", __LINE__);
					  BREAK;
				}
				
				st_push(main_stack, res);
				break;
			}
			case _BINARY_SUB: {
				//MEMORY_LEAK: res помещается на стек, но память не освобождается при удалении со стека
				var_t* res = malloc(sizeof(var_t));
				var_t* first = st_pop(main_stack);
				var_t* second = st_pop(main_stack);
				
								
				if(first->type != second->type) {
					//TODO доп проверки, на совместимость типов для сложения					
					printf("UNCORRECT TYPE OPERATORS FOR BINARY_SUB %d and %d\n", first->type, second->type);
					BREAK;										
				}
				
				res->type = first->type;
				switch(res->type) {
					case NONE_TYPE: {
						printf("ERROR: NONE_TYPE - NONE_TYPE == ERROR\n");
						BREAK;
					}
					case INT_TYPE: {
						res->val = (void*)( (int)second->val - (int)first->val );
					  break;
					}
					case FLOAT_TYPE: {
						res->val = malloc(sizeof(double));
						*(double*)res->val = *(double*)second->val - *(double*)first->val;
					  break;
					}
					default:
						printf("UNDEFINED VAR_TYPE in BINARY_SUB line:%d\n", __LINE__);
					  BREAK;
				}
				
				st_push(main_stack, res);
				break;
			}
			case _MULTIPLY: {
				//MEMORY_LEAK: res помещается на стек, но память не освобождается при удалении со стека
				var_t* res = malloc(sizeof(var_t));
				var_t* first = st_pop(main_stack);
				var_t* second = st_pop(main_stack);
				
				if(first->type != second->type) {
					//TODO доп проверки, на совместимость типов для сложения					
					printf("UNCORRECT TYPE OPERATORS FOR MULT %d and %d\n", first->type, second->type);
					BREAK;										
				}
				
				res->type = first->type;
				switch(res->type) {
					case NONE_TYPE: {
						printf("ERROR: NONE_TYPE * NONE_TYPE == ERROR\n");
						BREAK;
					}
					case INT_TYPE: {
						res->val = (void*)( (int)second->val * (int)first->val );
					  break;
					}
					case FLOAT_TYPE: {
						res->val = malloc(sizeof(double));
						*(double*)res->val = *(double*)second->val * *(double*)first->val;
					  break;
					}
					default:
						printf("UNDEFINED VAR_TYPE in MULT line:%d\n", __LINE__);
					  BREAK;
				}
				
				st_push(main_stack, res);
				break;
			}
			case _DIVISION: {
				//MEMORY_LEAK: res помещается на стек, но память не освобождается при удалении со стека
				var_t* res = malloc(sizeof(var_t));
				var_t* first = st_pop(main_stack);
				var_t* second = st_pop(main_stack);
				
				if(first->type != second->type) {
					//TODO доп проверки, на совместимость типов для сложения					
					printf("UNCORRECT TYPE OPERATORS FOR DIVISION %d and %d\n", first->type, second->type);
					BREAK;										
				}
				
				res->type = first->type;
				switch(res->type) {
					case NONE_TYPE: {
						printf("ERROR: NONE_TYPE / NONE_TYPE == ERROR\n");
						BREAK;
					}
					case INT_TYPE: {
						res->val = (void*)( (int)second->val / (int)first->val );
					  break;
					}
					case FLOAT_TYPE: {
						res->val = malloc(sizeof(double));
						*(double*)res->val = *(double*)second->val / *(double*)first->val;
					  break;
					}
					default:
						printf("UNDEFINED VAR_TYPE in DIVISION line:%d\n", __LINE__);
					  BREAK;
				}
				
				st_push(main_stack, res);
				break;
			}
			case _LESS: {
				//MEMORY_LEAK: res помещается на стек, но память не освобождается при удалении со стека
				var_t* res = malloc(sizeof(var_t));
				var_t* first = st_pop(main_stack);
				var_t* second = st_pop(main_stack);
				
				if(first->type != second->type) {
					//TODO доп проверки, на совместимость типов для сложения					
					printf("UNCORRECT TYPE OPERATORS FOR LESS %d and %d\n", first->type, second->type);
					BREAK;										
				}
				
				res->type = BOOL_TYPE;
				switch(first->type) {
					case NONE_TYPE: {
						res->val = 0;
					  break;
					}
					case INT_TYPE: {
						res->val = (void*)( (int)second->val < (int)first->val );
					  break;
					}
					case FLOAT_TYPE: {
						res->val = malloc(sizeof(double));
						res->val = (void*)(*(double*)second->val < *(double*)first->val);
					  break;
					}
					default:
						printf("UNDEFINED VAR_TYPE %d in LESS line:%d\n", res->type, __LINE__);
					  BREAK;
				}
				
				st_push(main_stack, res);
				break;
			}
			case _MORE: {
				//MEMORY_LEAK: res помещается на стек, но память не освобождается при удалении со стека
				var_t* res = malloc(sizeof(var_t));
				var_t* first = st_pop(main_stack);
				var_t* second = st_pop(main_stack);
				
				if(first->type != second->type) {
					//TODO доп проверки, на совместимость типов для сложения					
					printf("UNCORRECT TYPE OPERATORS FOR MORE %d and %d\n", first->type, second->type);
					BREAK;										
				}
				
				res->type = BOOL_TYPE;
				switch(first->type) {
					case NONE_TYPE: {
						res->val = 0;
					  break;
					}
					case INT_TYPE: {
						res->val = (void*)( (int)second->val > (int)first->val );
					  break;
					}
					case FLOAT_TYPE: {
						res->val = malloc(sizeof(double));
						res->val = (void*)(*(double*)second->val > *(double*)first->val);
					  break;
					}
					default:
						printf("UNDEFINED VAR_TYPE %d in MORE line:%d\n",res->type, __LINE__);
					  BREAK;
				}
				
				st_push(main_stack, res);
				break;
			}			
			case _JUMP_IF_NOT: {
				arg = GET_INT;
				//MEMORY_LEAK: res помещается на стек, но память не освобождается при удалении со стека
				var_t* bool = st_pop(main_stack);
				int jump;

				switch(bool->type) {
					case NONE_TYPE: {
						jump = 0;
					  break;
					}
					case INT_TYPE:
					case BOOL_TYPE: {
						jump = (int)bool->val;
					  break;
					}
					case FLOAT_TYPE: {
						printf("WHAT with (bool)<double> ??\n");
					  BREAK;
					}
					default:
						printf("UNDEFINED VAR_TYPE %d in JUMP_IF_NOT line:%d\n", bool->type, __LINE__);
					  BREAK;
				}
				//JUMP_IF_NOT
				if( !jump )
					code = code_begin + arg;
				
				break;
			}
			case _GOTO: {
				arg = GET_INT;
				code = code_begin + arg;
			  break;
			}
			case _CALL_STD: {
				int num_function = GET_INT;
				int count_arg = GET_INT;
				
				std_functions[num_function](count_arg);
				
			  break;
			}

			default:
				printf("UNDEFUNED BYTE CODE %d\n", byte);
			  BREAK;
		}
		
	}
	
	
}

int run_byte_code(FILE* out, char* filename) {
	FILE* code_file = fopen(filename, "r");
	if( code_file == NULL ) {
		fprintf(out, "ERROR: interpretator can`t open file '%s'\n", filename);
		return ERROR_OPEN_BYTE_CODE_FILE;
	}
		fseek(code_file, 0, SEEK_END);
		unsigned int size_code = (unsigned int)ftell(code_file);
		fseek(code_file, 0, SEEK_SET);
		
		char* code = malloc(size_code);
		if( !code ) {
			fprintf(out, "ERROR: interpretator very simple and it can`t save all byte code in memory\n");
			return NO_MEMORY_FOR_SAVE_CODE_IN_MEMORY;
		}
		fread(code, size_code, 1, code_file);
		
		interpretate(out, (char*)code, size_code);
		
		free(code);		
	fclose(code_file);
	return 0;
}
