all:
	gcc main.c lexer/lexer.c test/test.c AST/AST.c AST/to_postfix.c data_struct/stack.c data_struct/hash/hash.c syntax/syntax.c -o lang
