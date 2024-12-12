# parameters used
flags = -g
# clears any created files
clean:
	del *.exe
	del *.o
	del test\*.o
# for creating the tests
lexer:
	gcc -c $(flags) "test/lexer.c" -o "test/lexer.o"
	gcc test/lexer.o $(flags) -o lexer.exe
	./lexer.exe
parser:
	gcc -c $(flags) "test/parser.c" -o "test/parser.o"
	gcc test/parser.o $(flags) -o parser.exe
	./parser.exe
evaluator:
	gcc -c $(flags) "test/evaluator.c" -o "test/evaluator.o"
	gcc test/evaluator.o $(flags) -o evaluator.exe
	./evaluator.exe
memory:
	gcc -c $(flags) "test/memory.c" -o "test/memory.o"
	gcc test/memory.o $(flags) -o memory.exe
	./memory.exe