source = src/ast.c src/lexer.c src/parser.c src/token.c src/var.c src/main.c src/execute.c src/value.c
cc = gcc

target = marion

$(target): $(source)
	@echo "compiling the interpreter..."
	$(cc) $^ -o $@ -g

clean:
	@echo "removing executables..."
	rm $(target)

.PHONY: clean
