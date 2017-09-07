#ifndef _DATA_STRUCT_H_
#define _DATA_STRUCT_H_

#include <stdlib.h>
#include "hash/hash.h"

#define START_STACK_SIZE (16)
#define STACK_EXPANSION_NUM (1.5)
#define VECTOR_EXPANSION_NUM (1.5)

#define START_STRING_CAPACITY (8)

typedef struct stack_s {
	void** val;
	int size;
	int capacity;
} stack_t;
void* st_pop(stack_t*);
void st_push(stack_t* self, void*);
void* st_peek(stack_t*);
stack_t* stack_new();
void delete_stack(stack_t*);

typedef struct string_s {
	char* str;
	int capacity;
	int len;
} string_t;
string_t* string_new();
void delete_string(string_t*);
void string_add(string_t*, const string_t*);
string_t* string_cat(string_t*, const string_t*);
string_t* string_copy(string_t*);
string_t* string_make(char*, int);


typedef struct vector_s {
	void** val;
	int size;
	int capacity;
} vecctor_t;

typedef struct var_s {
		int type;
		void* val;
} var_t;




#endif
