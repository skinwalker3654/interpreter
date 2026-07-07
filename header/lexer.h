#ifndef LEXER_H
#define LEXER_H

typedef struct {
    char *source;
    int pos;
    int line;
} Lexer;

Lexer *lexer_create(char *source);
void lexer_destroy(Lexer *lx);

char lexer_peek(Lexer *lx);
char lexer_advance(Lexer *lx);
void lexer_skip_ws(Lexer *lx);

#endif
