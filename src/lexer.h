#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_PRINT=0,
    TOKEN_SET,
    TOKEN_WAIT,
    TOKEN_STRING,
    TOKEN_VARIABLE,
    TOKEN_NUMBER,
    TOKEN_PRINTLN,
    TOKEN_FOR,
    TOKEN_ENDFOR,
    TOKEN_INCREASE,
    TOKEN_DECREASE,
    TOKEN_EQUAL,
    TOKEN_COLON,
    TOKEN_IF,
    TOKEN_EQUAL_EQUAL,
    TOKEN_LESS,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS_EQUAL,
    TOKEN_ENDIF,
    TOKEN_TO,
    TOKEN_MKDIR,
    TOKEN_REMOVE,
    TOKEN_EOF,
    TOKEN_CLEAR,
    TOKEN_PLUS_EQUAL,
    TOKEN_MINUS_EQUAL,
    TOKEN_SEMICOLON,
    TOKEN_ERROR,
} TokenType;

typedef struct Lexer {
    char *source;
    int pos;
    int line;
    int column;
} Lexer;

Lexer lexer_init(char *source);
char peek(Lexer *ptr);
char advance(Lexer *ptr);
void skip_white_spaces(Lexer *ptr);

typedef struct Token {
    TokenType type;
    char value[256];
    int line;
    int column;
} Token;

Token make_token(TokenType type,char value[],int line,int column);
Token get_next_token(Lexer *ptr);

#endif
