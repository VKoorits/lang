#include "data_struct.h"

//TODO Добавить функционал в vector

void* vec_pop(vector_t* this) {
	if ( this->size > 0 ) 
		return this->val[--this->size];
	return NULL;
}

void vec_push(vector_t* this, void* el) {
	if (this->size == this->capacity){
		this->capacity *= VECTOR_EXPANSION_NUM;
		this->val = realloc(this->val, this->capacity*sizeof(void*));
	}
	this->val[ this->size++ ] = el;
}

void* vec_peek(vector_t* this) {
	if ( this->size > 0 ) 
		return this->val[this->size-1];
	return NULL;
}

vector_t* vector_new() {
	vector_t* this = malloc(sizeof(vector_t));

	this->capacity = START_vector_SIZE;

	this->size = 0;
	this->val = malloc(START_vector_SIZE * sizeof(void*));
	return this;
}

void delete_vector(vector_t* this) {
	free(this->val);
	free(this);
}
