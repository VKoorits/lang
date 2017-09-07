#include "std.h"

char* std_functions_lst[COUNT_STD_FUNCTION] = {
	//Input-Output
	"print",
	//"scan"
};

void (*std_functions[COUNT_STD_FUNCTION])(int) = {
	std_print
};
