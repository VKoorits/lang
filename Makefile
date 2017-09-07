all:
	gcc main.c lexer/lexer.c test/test.c AST/AST.c AST/to_postfix.c data_struct/stack.c data_struct/hash/hash.c syntax/syntax.c stat_analyzer/stat_analyzer.c compiler/compiler.c compiler/to_op_codes.c interpretator/interpretator.c std/std.c std/lang_io.c -o lang
