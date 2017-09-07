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
			printf("%d", (int)var->val);
		  break;
		case FLOAT_TYPE:
			printf("%f", *((double*)var->val) );
		  break;
		case STRING_TYPE:
			printf("%s", (char*)var->val);
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
		printf("\n");
	}
}
