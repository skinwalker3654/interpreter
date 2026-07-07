#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "ast.h"

typedef struct {
    Lexer *lx;
    Token *current;
} Parser;

Parser *parser_init(char *source);
void parser_destroy(Parser *ps);

Expr *parser_parse_expr(Parser *ps);
Condition *parser_parse_cond(Parser *ps);

Ast *parser_parse_matablhth(Parser *ps);
Ast *parser_parse_var_assign(Parser *ps);
Ast *parser_parse_an(Parser *ps);
Ast *parser_parse_gia(Parser *ps);
Ast *parser_parse_print(Parser *ps);
Ast *parser_parse_printent(Parser *ps);
Ast *parser_parse_perimene(Parser *ps);
Ast *parser_parse_sbyse(Parser *ps);
Ast *parser_parse_treje(Parser *ps);
Ast *parser_parse_neosfakelos(Parser *ps);
Ast *parser_parse_telosprograma(Parser *ps);
Ast *parser_parse_ka8arise(Parser *ps);

Ast *parser_parse_stmt(Parser *ps);
Ast *parser_parse_program(Parser *ps);

#endif
