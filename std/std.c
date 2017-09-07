#include "std.h"

char* std_functions_lst[COUNT_STD_FUNCTION] = {
	//Input-Output
	"print",
	"println",
	"scan"
};

void (*std_functions[COUNT_STD_FUNCTION])(int) = {
	std_print,
	std_println,
	std_scan
};
