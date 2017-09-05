#include "interpretator.h"

#define GET_INT (*((int*)code));code+=sizeof(int);
#define GET_BYTE (*((char*)code));code+=sizeof(char);


static void print_var(var_t* var) {
	printf("TYPE = %d\t", var->type);
	switch(var->type) {
		case INT_NUM_TOKEN:
			printf("%d", (int)var->val);
		  break;
		case FLOAT_NUM_TOKEN:
			printf("%f", *((double*)var->val) );
		  break;
		case STRING_TOKEN:
			printf("%s", (char*)var->val);
		  break;
		default:
			printf("UNKNOWN VAR TYPE %d (print_var():%d)", var->type, __LINE__ );
	}
}

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
	int const_count;
	var_t* constants = read_const_from_byte_code(out, code_begin, size_code, &const_count);
	char* code = code_begin + 4;
	
	char byte;
	while(1) {
		byte = GET_BYTE;
		break;
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
