#ifndef _DATA_STRUCT_H_
#define _DATA_STRUCT_H_

#include <stdlib.h>
#include "hash/hash.h"

#define START_STACK_SIZE (16)
#define STACK_EXPANSION_NUM (1.5)
#define VECTOR_EXPANSION_NUM (1.5)

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

typedef struct vector_s {
	void** val;
	int size;
	int capacity;
} vecctor_t;





#endif
