#ifndef _STD_H_
#define _STD_H_

#include <stdio.h>
#include "../data_struct/data_struct.h"
#include "../interpretator/interpretator.h"

void print_var(var_t*);

#define COUNT_STD_FUNCTION (3)

#define NONE_TYPE (0)
#define INT_TYPE (1)
#define FLOAT_TYPE (2)
#define STRING_TYPE (3)
#define BOOL_TYPE (4)


void std_print(int);
void std_println(int);
void std_scan(int);




#endif
