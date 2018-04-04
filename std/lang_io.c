#include "std.h"



//GLOBAL_VAR ######################################
//CONSTANTS #####################
extern int const_count;
extern var_t* constants;
//###############################

//VAR_STACK #####################
extern var_t* var_stack;
extern int var_stack_size;
extern int var_stack_capacity;
//###############################

// основной стек, на котором происходят все операции
extern stack_t* main_stack;
//#################################################

void print_var(var_t* var) {
	switch(var->type) {
		case NONE_TYPE:
			printf("NONE_TYPE");
		  break;
		case INT_TYPE:
			printf("%li", (long)var->val);
		  break;
		case FLOAT_TYPE:
			printf("%f", *((double*)var->val) );
		  break;
		case STRING_TYPE:
			printf("%s", ((string_t*)var->val)->str );
		  break;
		case BOOL_TYPE:
			if(var->val)
				printf("true");
			else
				printf("false");
		  break;
		default:
			printf("UNKNOWN VAR TYPE %d (print_var():%d)", var->type, __LINE__ );
	}
}

void std_print(int cnt_arg) {
	for(int i = main_stack->size - cnt_arg; i < main_stack->size; ++i) {
		print_var((var_t*)(main_stack->val[i]) );
	}
}
void std_println(int cnt_arg) {
	std_print(cnt_arg);
	printf("\n");
}

void std_scan(int cnt_arg) {
	//if( cnt_arg )
	//	printf("ERROR: scan get %d arg (expected 0)\n", cnt_arg);

	st_push(main_stack, malloc(sizeof(var_t)) );

	((var_t*)st_peek(main_stack))->type = STRING_TYPE;
	char* buff = malloc(100);
	fgets(buff, 100, stdin);

	((var_t*)st_peek(main_stack))->val = (void*)string_make(buff, strlen(buff));
	free(buff);
}
