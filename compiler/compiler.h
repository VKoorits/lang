#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <stdio.h>
#include "../lexer/lexer.h"
#include "../AST/AST.h"
#include "../data_struct/data_struct.h"
#include <stdlib.h>

/*
 * сначала идет 4-байтное число: смещение, с которого начинается секция констант
 * код
 * потом идет 4 байтное чило: количество констант
 * каждая константа имеет следующее представление:
 * 	4-байтный тип константы
 * 	4-байтная длина константной записи
 *   константа
*/

int compile(char*, FILE*, stack_t*, hash_t*);
void write_op_codes(FILE*, char*);

#endif
