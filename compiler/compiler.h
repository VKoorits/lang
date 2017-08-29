#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <stdio.h>
#include "../lexer/lexer.h"
#include "../AST/AST.h"
#include "../data_struct/data_struct.h"
#include <stdlib.h>

/*
перед началом кода идет секция констант
* сначала идет 4-байтное число: смещение, с которого начинается код
* потом идет 4 байтное чило: количество констант
* каждая константа имеет следующее представление:
* 	?-байтный тип константы
* 	?-байтная длина константной записи
*   константа
*/

int compile(char*, FILE*, stack_t*, hash_t*);
void write_op_codes(FILE*, char*);

#endif
