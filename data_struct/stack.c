#include "data_struct.h"

void* st_pop(stack_t* this) {
	if ( this->size > 0 ) 
		return this->val[--this->size];
	return NULL;
}
void st_push(stack_t* this, void* el) {
	if (this->size == this->capacity){
		this->capacity *= STACK_EXPANSION_NUM;
		this->val = realloc(this->val, this->capacity*sizeof(void*));
	}
	this->val[ this->size++ ] = el;
}
void* st_peek(stack_t* this) {
	if ( this->size > 0 ) 
		return this->val[this->size-1];
	return NULL;
}

stack_t* stack_new() {
	stack_t* this = malloc(sizeof(stack_t));

	this->capacity = START_STACK_SIZE;

	this->size = 0;
	this->val = malloc(START_STACK_SIZE * sizeof(void*));
	return this;
}

void delete_stack(stack_t* this) {
	free(this->val);
	free(this);
}
