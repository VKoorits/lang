#include "data_struct.h"
#include <stdio.h>

string_t* string_new() {
	string_t* this = malloc(sizeof(string_t));

	this->capacity = START_STRING_CAPACITY;

	this->len = 0;
	this->str = malloc(START_STRING_CAPACITY * sizeof(char) );
	return this;
}


void delete_string(string_t* this) {
	free(this->str);
	free(this);
}

void string_add(string_t* first, const string_t* second) {
	if( first->capacity < first->len + second->len) {
		first->str = realloc( first->str, first->len + second->len + 1 );
		first->capacity = first->len + second->len + 1;
	}
	
	strcpy(first->str+first->len, second->str);
	first->len = first->len + second->len;
}
string_t* string_cat(string_t* first, const string_t* second) {
	string_t* res = malloc(sizeof(string_t));
	res->str = malloc(first->len + second->len + 1);
	res->capacity = first->len + second->len + 1;
	
	strcpy(res->str, first->str);
	strcpy(res->str+first->len-1, second->str);
	
	return res;
}


string_t* string_copy(string_t* str) {
	string_t* copy = malloc(sizeof(string_t));

	copy->capacity = str->capacity;

	copy->len = str->len;
	copy->str = malloc( str->capacity );
	strcpy(copy->str, str->str);
	return str;
}

string_t* string_make(char* str, int len) {
	string_t* copy = malloc(sizeof(string_t));

	copy->capacity = len+1;

	copy->len = len;
	copy->str = malloc( len+1 );
	strcpy(copy->str, str);
	return copy;
}
