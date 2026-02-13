#include <string.h>
#include <ctype.h>
#include "lexer.h"

Lexer lexer_init(char *source) {
    Lexer new_lexer = {source,0,1,1};
    return new_lexer;
}

char peek(Lexer *ptr) {
    return ptr->source[ptr->pos];
}

char advance(Lexer *ptr) {
    char ch = ptr->source[ptr->pos++];
    if(ch == '\n') {
        ptr->line++;
        ptr->column=1;
        return ch;
    } else {
        ptr->column++;
        return ch;
    }
}

void skip_white_spaces(Lexer *ptr) {
    while(isspace(peek(ptr))) {
        advance(ptr);
    }
}

Token make_token(TokenType type, char *value, int line, int column) {
    Token new_token;
    strcpy(new_token.value,value);
    new_token.type = type;
    new_token.line = line;
    new_token.column = column;
    return new_token;
}

Token get_next_token(Lexer *ptr) {
    skip_white_spaces(ptr);
    char current = peek(ptr);
    int line = ptr->line;

    if(current == '\0') return make_token(TOKEN_EOF,"",line,ptr->column);

    if(isdigit(current)) {
        char buffer[256];
        int counter = 0;
        
        while(isdigit(peek(ptr))) {
            buffer[counter] = advance(ptr);
            counter++;
        }

        buffer[counter] = '\0';
        return make_token(TOKEN_NUMBER,buffer,line,ptr->column);
    }

    if(isalpha(current)) {
        char buffer[256];
        int counter = 0;

        while(isalnum(peek(ptr)) || peek(ptr) == '_') {
            buffer[counter] = advance(ptr);
            counter++;
        }

        buffer[counter] = '\0';
        if(strcmp(buffer,"print")==0) {
            return make_token(TOKEN_PRINT,buffer,line,ptr->column);
        } else if(strcmp(buffer,"printent")==0) {
            return make_token(TOKEN_PRINTLN,buffer,line,ptr->column);
        } else if(strcmp(buffer,"metablhth")==0) {
            return make_token(TOKEN_SET,buffer,line,ptr->column);
        } else if(strcmp(buffer,"perimene")==0) {
            return make_token(TOKEN_WAIT,buffer,line,ptr->column);
        } else if(strcmp(buffer,"an")==0) {
            return make_token(TOKEN_IF,buffer,line,ptr->column);
        } else if(strcmp(buffer,"telosan")==0) {
            return make_token(TOKEN_ENDIF,buffer,line,ptr->column);
        } else if(strcmp(buffer,"telosgia")==0) {
            return make_token(TOKEN_ENDFOR,buffer,line,ptr->column);
        } else if(strcmp(buffer,"neosfakelos")==0) {
            return make_token(TOKEN_MKDIR,buffer,line,ptr->column);
        } else if(strcmp(buffer,"gia")==0) {
            return make_token(TOKEN_FOR,buffer,line,ptr->column);
        } else if(strcmp(buffer,"eos")==0) {
            return make_token(TOKEN_TO,buffer,line,ptr->column);
        } else if(strcmp(buffer,"ka8arise")==0) {
            return make_token(TOKEN_CLEAR,buffer,line,ptr->column);
        } else if(strcmp(buffer,"sbyse")==0) {
            return make_token(TOKEN_REMOVE,buffer,line,ptr->column);
        } else {
            return make_token(TOKEN_VARIABLE,buffer,line,ptr->column);
        }
    }

    if(current == '"') {
        char buffer[256];
        int counter = 0;

        advance(ptr);
        while(peek(ptr) != '"' && peek(ptr) != '\0') {
            buffer[counter] = advance(ptr);
            counter++;
        }

        advance(ptr);
        buffer[counter] = '\0';

        return make_token(TOKEN_STRING,buffer,line,ptr->column);
    }

    char symbols[2] = {current,'\0'};
    char doubles[3] = {current,ptr->source[ptr->pos+1],'\0'};

    if(doubles[0] == '+' && doubles[1] == '+') {
        advance(ptr);
        advance(ptr);
        return make_token(TOKEN_INCREASE,doubles,line,ptr->column);
    } else if(doubles[0] == '-' && doubles[1] == '-') {
        advance(ptr);
        advance(ptr);
        return make_token(TOKEN_DECREASE,doubles,line,ptr->column);
    } else if(doubles[0] == '>' && doubles[1] == '=') {
        advance(ptr);
        advance(ptr);
        return make_token(TOKEN_GREATER_EQUAL,doubles,line,ptr->column); 
    } else if(doubles[0] == '<' && doubles[1] == '=') {
        advance(ptr);
        advance(ptr);
        return make_token(TOKEN_LESS_EQUAL,doubles,line,ptr->column);
    } else if(doubles[0] == '=' && doubles[1] == '=') {
        advance(ptr);
        advance(ptr);
        return make_token(TOKEN_EQUAL_EQUAL,doubles,line,ptr->column);
    } else if(doubles[0] == '+' && doubles[1] == '=') {
        advance(ptr);
        advance(ptr);
        return make_token(TOKEN_PLUS_EQUAL,doubles,line,ptr->column);
    } else if(doubles[0] == '-' && doubles[1] == '=') {
        advance(ptr);
        advance(ptr);
        return make_token(TOKEN_MINUS_EQUAL,doubles,line,ptr->column);
    }

    switch(symbols[0]) {
        case '=': advance(ptr); return make_token(TOKEN_EQUAL,symbols,line,ptr->column);
        case ':': advance(ptr); return make_token(TOKEN_COLON,symbols,line,ptr->column); 
        case '>': advance(ptr); return make_token(TOKEN_GREATER,symbols,line,ptr->column);
        case '<': advance(ptr); return make_token(TOKEN_LESS,symbols,line,ptr->column);
        case ';': advance(ptr); return make_token(TOKEN_SEMICOLON,symbols,line,ptr->column);
    }

    return make_token(TOKEN_ERROR,"Invalid Input",line,ptr->column);
}
