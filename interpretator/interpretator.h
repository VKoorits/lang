#ifndef _INTERPRETATOR_H_
#define _INTERPRETATOR_H_

#include <stdio.h>
#include "../lexer/lexer.h"
#include "../AST/AST.h"
#include "../data_struct/data_struct.h"
#include "../compiler/byte_codes.h"
#include "stdlib.h"
#include "../std/std.h"


#define ERROR_OPEN_BYTE_CODE_FILE (1)
#define NO_MEMORY_FOR_SAVE_CODE_IN_MEMORY (2)

#define STARTED_COUNT_VAR_IN_MAIN_STACK (8)


int run_byte_code(FILE*, char*);



#endif //_INTERPRETATOR_H_
