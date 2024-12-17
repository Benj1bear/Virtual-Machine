# clears any created files
clean:
	del *.exe
# for creating the tests
lexer:
	gcc -o lexer "test/lexer.c"
	./lexer.exe
parser:
	gcc -o parser "test/parser.c"
	./parser.exe
evaluator:
	gcc -o evaluator "test/evaluator.c"
	./evaluator.exe
memory:
	gcc -o memory "test/memory.c"
	./memory.exe
