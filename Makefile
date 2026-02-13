SRC = src/main.c src/lexer.c src/parser.c
TAR = run
GREEKTAR = marion
GCC = gcc
GREEKLISH = greeklish/lexer.c greeklish/main.c greeklish/parser.c

all: $(SRC)
	@echo "compiling the compiler..."
	$(GCC) $^ -o  $(TAR) 

greeklish: $(GREEKLISH)
	@echo "copmiling the greeklish compiler..."
	$(GCC) $^ -o $(GREEKTAR)

clean:
	@echo "Deleting executables..."
	rm -rf $(TAR) $(GREEKTAR)

.PHONY: all clean script greeklish
