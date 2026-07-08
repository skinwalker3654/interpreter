#ifndef TOKEN_H
#define TOKEN_H

#include "lexer.h"

typedef enum {
    /* keywords */
    TOK_METABLHTH,
    TOK_GIA,
    TOK_AN,
    TOK_EOS,
    TOK_ARI8MO,
    TOK_MHNHMA,
    TOK_MHKOS,

    /* values */
    TOK_NUM_LIT,
    TOK_STR_LIT,
    TOK_IDENT_LIT,

    /* functions */
    TOK_PRINT,
    TOK_PRINTENT,
    TOK_PERIMENE,
    TOK_KA8ARISE,
    TOK_SBYSE,
    TOK_DIABASE_ARXEIO,
    TOK_DIABASE,
    TOK_TELOS_PROGRAMA,
    TOK_TREJE,
    TOK_NEOSFAKELOS,

    /* symbols */
    TOK_SEMI,
    TOK_LPAR,
    TOK_RPAR,
    TOK_LBRA,
    TOK_RBRA,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_ASSIGN,

    /* conditions */
    TOK_EQEQ,
    TOK_GE,
    TOK_GT,
    TOK_LE,
    TOK_LT,
    TOK_NOT_EQ,

    /* extra */
    TOK_EOF,
    TOK_ERR,
} Tok_type;

typedef struct {
    Tok_type type;
    char *value;
} Token;

Token *token_create(Tok_type type, char *value);
void token_destroy(Token *tk);
Token *token_get_next(Lexer *lx);

#endif
