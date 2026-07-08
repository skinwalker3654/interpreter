source = src/ast.c src/lexer.c src/parser.c src/token.c src/var.c src/main.c src/execute.c src/value.c
cc = gcc

target = marion

$(target): $(source)
	@echo "compiling the interpreter..."
	$(cc) $^ -o $@ -g

clean:
	@echo "removing executables..."
	rm $(target)

vim:
	@echo "copying configurations for vim..."
	mkdir -p ~/.vim/ftdetect
	mkdir -p ~/.vim/syntax
	cp configs/vim/ftdetect/marion.vim ~/.vim/ftdetect
	cp configs/vim/syntax/marion.vim ~/.vim/syntax
 
vimremove:
	@echo "removing vim config..."
	rm ~/.vim/ftdetect/marion.vim
	rm ~/.vim/syntax/marion.vim

.PHONY: clean vim
