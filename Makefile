SRC = src/main.c src/lexer.c src/parser.c
TAR = main
GCC = gcc

all: $(SRC)
	@echo "compiling the compiler..."
	$(GCC) $^ -o  $(TAR) 

script:
	@echo "Running the script...."
	./$(TAR) script

clean:
	@echo "Deleting executables..."
	rm -rf $(TAR) 

.PHONY: all clean script
