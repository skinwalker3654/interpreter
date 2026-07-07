#include "../header/token.h"
#include "../header/lexer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

Token *token_create(Tok_type type, char *value) {
    Token *new_tok = malloc(sizeof(Token));
    if(!new_tok) {
        printf("Failed to create the token\n");
        return NULL;
    }

    new_tok->type = type;
    
    if(type != TOK_IDENT_LIT && type != TOK_NUM_LIT && type != TOK_STR_LIT) 
        return new_tok;

    new_tok->value = strdup(value);
    if(!new_tok) {
        printf("Failed to put the value into the token\n");
        free(new_tok);
        return NULL;
    }

    return new_tok;
}

void token_destroy(Token *tk) {
    if(!tk) return;

    if(tk->type == TOK_IDENT_LIT || tk->type == TOK_NUM_LIT || tk->type == TOK_STR_LIT)
        free(tk->value);

    free(tk);
    tk = NULL;
}

static int string_push_char(char **string, char ch) {
    if(!*string) {
        *string = malloc(2);
        if(!*string) return -1;

        (*string)[0] = ch;
        (*string)[1] = '\0';
        return 0;
    }

    int size = strlen(*string);
    char *temp =  realloc(*string,size+2);
    if(!temp) return -1;

    *string = temp;
    (*string)[size] = ch;
    (*string)[size+1] = '\0';

    return 0;
}

static Token *get_ident(Lexer *lx) {
    char *buffer = NULL;
    while(isalnum(lexer_peek(lx)) || lexer_peek(lx) == '_') {
        string_push_char(&buffer,lexer_advance(lx));
    }

    string_push_char(&buffer,'\0');
    if(strcmp(buffer,"metablhth")==0 || strcmp(buffer,"set")==0) {
        free(buffer);
        return token_create(TOK_METABLHTH,NULL);
    } else if(strcmp(buffer,"gia")==0 || strcmp(buffer,"for")==0) {
        free(buffer);
        return token_create(TOK_GIA,NULL);
    } else if(strcmp(buffer,"an")==0 || strcmp(buffer,"if")==0) {
        free(buffer);
        return token_create(TOK_AN,NULL);
    } else if(strcmp(buffer,"eos")==0 || strcmp(buffer,"to")==0) {
        free(buffer);
        return token_create(TOK_EOS,NULL);
    } else if(strcmp(buffer,"ari8mo")==0 || strcmp(buffer,"num")==0) {
        free(buffer);
        return token_create(TOK_ARI8MO,NULL);
    } else if(strcmp(buffer,"mhnhma")==0 || strcmp(buffer,"str")==0) {
        free(buffer);
        return token_create(TOK_MHNHMA,NULL);
    } else if(strcmp(buffer,"print")==0) {
        free(buffer);
        return token_create(TOK_PRINT,NULL);
    } else if(strcmp(buffer,"printent")==0 || strcmp(buffer,"println")==0) {
        free(buffer);
        return token_create(TOK_PRINTENT,NULL);
    } else if(strcmp(buffer,"perimene")==0 || strcmp(buffer,"wait")==0) {
        free(buffer);
        return token_create(TOK_PERIMENE,NULL);
    } else if(strcmp(buffer,"ka8arise")==0 || strcmp(buffer,"clear")==0) {
        free(buffer);
        return token_create(TOK_KA8ARISE,NULL);
    } else if(strcmp(buffer,"sbyse")==0 || strcmp(buffer,"remove")==0) {
        free(buffer);
        return token_create(TOK_SBYSE,NULL);
    } else if(strcmp(buffer,"diabase_arxeio")==0 || strcmp(buffer,"read_file")==0) {
        free(buffer);
        return token_create(TOK_DIABASE_ARXEIO,NULL);
    } else if(strcmp(buffer,"diabase")==0 || strcmp(buffer,"read")==0) {
        free(buffer);
        return token_create(TOK_DIABASE,NULL);
    } else if(strcmp(buffer,"telosprograma")==0 || strcmp(buffer,"endprogram")==0) {
        free(buffer);
        return token_create(TOK_TELOS_PROGRAMA,NULL);
    } else if(strcmp(buffer,"treje")==0 || strcmp(buffer,"execute")==0) {
        free(buffer);
        return token_create(TOK_TREJE,NULL);
    } else if(strcmp(buffer,"neosfakelos")==0 || strcmp(buffer,"mkdir")==0) {
        free(buffer);
        return token_create(TOK_NEOSFAKELOS,NULL);
    } else {
        Token *tk = token_create(TOK_IDENT_LIT,buffer);
        free(buffer);
        return tk;
    }
}

static Token *get_number(Lexer *lx) {
    char *buffer = NULL;

    while(isdigit(lexer_peek(lx))) {
        string_push_char(&buffer, lexer_advance(lx));
    }

    string_push_char(&buffer,'\0');

    Token *tk = token_create(TOK_NUM_LIT,buffer);
    free(buffer);

    return tk;
}

static Token *get_string(Lexer *lx) {
    lexer_advance(lx);

    char *buffer = NULL;
    while(lexer_peek(lx) != '"' && lexer_peek(lx) != '\0') {
        string_push_char(&buffer, lexer_advance(lx));
    }

    if(lexer_peek(lx) == '\0') {
        printf("Error %d: String %s never closed",lx->line,buffer);
        return NULL;
    }

    lexer_advance(lx);

    Token *tk = token_create(TOK_STR_LIT,buffer);
    free(buffer);

    return tk;
}

/* TOKEN_GET_NEXT FUNCTION */
Token *token_get_next(Lexer *lx) {
    lexer_skip_ws(lx);
    char current = lexer_peek(lx);

    if(current == '\0') {
        return token_create(TOK_EOF,NULL);
    }

    if(current == '#') {
        while(lexer_peek(lx) != '\n') lexer_advance(lx);
        return token_get_next(lx);
    }

    if(isalpha(current)) {
        return get_ident(lx);
    }

    if(isdigit(current)) {
        return get_number(lx);
    }

    if(current == '"') {
        return get_string(lx);
    }

    char doubles[3] = {current,lx->source[lx->pos+1],'\0'};
    if(strcmp(doubles,"==")==0) {
        lexer_advance(lx);
        lexer_advance(lx);
        return token_create(TOK_EQEQ,NULL);
    } else if(strcmp(doubles,">=")==0) {
        lexer_advance(lx);
        lexer_advance(lx);
        return token_create(TOK_GE,NULL);
    } else if(strcmp(doubles,"<=")==0) {
        lexer_advance(lx);
        lexer_advance(lx);
        return token_create(TOK_LE,NULL);
    } else if(strcmp(doubles,"!=")==0) {
        lexer_advance(lx);
        lexer_advance(lx);
        return token_create(TOK_NOT_EQ,NULL);
    }

    char symbol[2] = {current,'\0'};
    lexer_advance(lx);

    switch(symbol[0]) {
        case ';': return token_create(TOK_SEMI,NULL);
        case '(': return token_create(TOK_LPAR,NULL);
        case ')': return token_create(TOK_RPAR,NULL);
        case '{': return token_create(TOK_LBRA,NULL);
        case '}': return token_create(TOK_RBRA,NULL);
        case '<': return token_create(TOK_LT,NULL);
        case '>': return token_create(TOK_GT,NULL);
        case '+': return token_create(TOK_PLUS,NULL);
        case '-': return token_create(TOK_MINUS,NULL);
        case '*': return token_create(TOK_STAR,NULL);
        case '/': return token_create(TOK_SLASH,NULL);
        case '=': return token_create(TOK_ASSIGN,NULL);
    }

    printf("Error %d: Invalid character '%s'\n",lx->line,symbol);
    return token_create(TOK_ERR,NULL);
}
