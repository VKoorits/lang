all:
	gcc main.c lexer/lexer.c test/test.c AST/AST.c -o lang
