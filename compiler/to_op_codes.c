#include "compiler.h"
#include "byte_codes.h"


void write_op_codes(FILE* output, char* filename) {
	FILE* code = fopen(filename, "rb");
	int offset_to_constant, count_const;
	fread(&offset_to_constant, sizeof(int), 1, code);
		
	fseek(code, offset_to_constant, SEEK_SET);
	fread(&count_const, sizeof(int), 1, code);
	
	fprintf(output, "CONSTANTS:\n");
	for(int i = 1; i <= count_const; i++ ){
		int type, const_len;
		fread(&type, sizeof(int), 1, code);
		fread(&const_len, sizeof(int), 1, code);
		char* token = malloc(const_len+1);
		fread(token, const_len, 1, code);
		token[const_len] = '\0';
		
		fprintf(output, "\t%d: %s\n", i, token);
		
	}
	
	fseek(code, sizeof(int), SEEK_SET);
	
	fprintf(output, "CODE:\n");
	int cn = 0;//command_number
	while(1){
		++cn;
		char byte;
		if( ftell(code) > offset_to_constant ) break;
		fread(&byte, sizeof(char), 1, code);	
		
		if( byte == PUSH ) {
			int arg;
			fread(&arg, sizeof(int), 1, code); 
			  if( ftell(code) > offset_to_constant ) break;
			fprintf(output, "%d:\tPUSH %d\n", cn, arg);
		} else if ( byte == PUSH_CONST ) {
			int arg;
			fread(&arg, sizeof(int), 1, code); 
			  if( ftell(code) > offset_to_constant ) break;
			fprintf(output, "%d:\tPUSH_CONST %d\n", cn, arg);
		} else if ( byte == STORE ) {
			fprintf(output, "%d:\tSTORE\n", cn);
		} else if ( byte == BINARY_ADD) {
			fprintf(output, "%d:\tBINARY_ADD\n", cn);
		} else if ( byte == BINARY_SUB) {
			fprintf(output, "%d:\tBINARY_SUB\n", cn);
		} else if ( byte == NEW_VAR ) {
			int arg;
			fread(&arg, sizeof(int), 1, code);
			  if( ftell(code) > offset_to_constant ) break;
			fprintf(output, "%d:\tNEW_VAR %d\n", cn, arg);
		} else if ( byte == DELETE_VAR ) {
			int arg;
			fread(&arg, sizeof(int), 1, code); 
			  if( ftell(code) > offset_to_constant ) break;
			fprintf(output, "%d:\tDELETE_VAR %d\n", cn, arg);
		} else if ( byte == JUMP_IF_NOT) {
			int arg;
			fread(&arg, sizeof(int), 1, code); 
			  if( ftell(code) > offset_to_constant ) break;
			fprintf(output, "%d:\tJUMP_IF_NOT 0x%d\n", cn, arg);
		} else  {
			printf("UNKNOWN BYTE CODE %d\n", byte);
			break;
		}
	};
	
}




















