#include "../header/parser.h"
#include "../header/token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* OWNER - SHIP*/
Parser *parser_init(char *source) {
    Parser *new_parser = malloc(sizeof(Parser));
    if(!new_parser) {
        printf("Error: Failed to create the parser\n");
        return NULL;
    }

    new_parser->lx = lexer_create(source);
    if(!new_parser->lx) {
        printf("Failed to initialize the lexer\n");
        free(new_parser);
        return NULL;
    }

    new_parser->current = token_get_next(new_parser->lx);
    if(!new_parser->current) {
        printf("Failed to get the first token\n");
        lexer_destroy(new_parser->lx);
        free(new_parser);
        return NULL;
    }

    return new_parser;
}

void parser_destroy(Parser *ps) {
    if(!ps) return;

    lexer_destroy(ps->lx);
    token_destroy(ps->current);

    free(ps);
    ps = NULL;
}

/* PARSER - API */
static int match(Parser *ps, Tok_type type) {
    if(!ps) return 0;
    return ps->current->type == type;
}

static int advance(Parser *ps) {
    token_destroy(ps->current);
    ps->current = token_get_next(ps->lx);
    if(ps->current == NULL) return 0;
    return 1;
}

static int consum(Parser *ps, Tok_type type, const char *msg) {
    if(!match(ps,type)) {
        printf("Line %d: Parse error: %s\n",ps->lx->line,msg);
        return 0;
    }

    if(advance(ps)==0) return 0;
    return 1;
}

static Expr *parse_additive(Parser *ps);
static Expr *parse_multiplicative(Parser *ps);
static Expr *parse_primary(Parser *ps);

static Expr *parse_primary(Parser *ps) {
    if(match(ps, TOK_NUM_LIT)) {
        Expr *ex = expr_new_int(atol(ps->current->value));
        advance(ps);
        return ex;
    }

    if(match(ps,TOK_LPAR)) {
        advance(ps);
        Expr *ex = parser_parse_expr(ps);
        if(!consum(ps,TOK_RPAR,"Expected ')'")) {
            expr_destroy(ex);
            return NULL;
        }

        return ex;
    }

    if(match(ps,TOK_IDENT_LIT)) {
        Expr *ex = expr_new_ident(ps->current->value);
        advance(ps);
        return ex;
    }

    if(match(ps,TOK_STR_LIT)) {
        Expr *ex = expr_new_str(ps->current->value);
        advance(ps);
        return ex;
    }

    if(match(ps,TOK_DIABASE)) {
        advance(ps);

        if(!match(ps,TOK_ARI8MO) && !match(ps,TOK_MHNHMA)) {
            printf("Error line %d: Expected 'ari8mo' or 'mhnhma' before getting the input",ps->lx->line);
            return NULL;
        }

        Read_type type;
        if(match(ps,TOK_ARI8MO))
            type = READ_NUM;
        else 
            type = READ_BUFF;

        advance(ps);
        if(!match(ps,TOK_STR_LIT)) {
            printf("Error line %d: Expected 'prompt' in a string form\n",ps->lx->line);
            return NULL;
        }

        char *prompt = strdup(ps->current->value);
        advance(ps);

        Expr *ex = expr_new_read(prompt,type);
        free(prompt);
        return ex;
    }

    if(match(ps,TOK_DIABASE_ARXEIO)) {
        advance(ps);

        Expr *ex = parser_parse_expr(ps);
        if(!ex) return NULL;

        if(ex->type == EXPR_INT || ex->type == EXPR_BIN || ex->type == EXPR_READ_FILE || ex->type == EXPR_READ) {
            printf("Error line %d: Cannot pass integers at diabase_arxeio\n",ps->lx->line);
            expr_destroy(ex);
            return NULL;
        }

        return expr_new_read_file(ex);
    }

    printf("Error line %d: Expected an expression\n",ps->lx->line);
    return NULL;
}

static Expr *parse_additive(Parser *ps) {
    Expr *left = parse_multiplicative(ps);
    if(!left) return NULL;

    while(match(ps,TOK_PLUS) || match(ps,TOK_MINUS)) {
        Binop_type op;
        if(match(ps,TOK_PLUS)) {
            op = OP_ADD;
        } else if(match(ps,TOK_MINUS)) {
            op = OP_SUB;
        } else {
            printf("Error line %d: Invalid arithmetic symbol '%c'\n",ps->lx->line,ps->lx->source[ps->lx->pos-1]);
            expr_destroy(left);
            return NULL;
        }

        advance(ps);
        Expr *right = parse_multiplicative(ps);
        if(!right) {
            expr_destroy(left);
            return NULL;
        }

        left = expr_new_bin(left, op, right);
        if(!left) {
            expr_destroy(right);
            return NULL;
        }
    }

    return left;
}

static Expr *parse_multiplicative(Parser *ps) {
    Expr *left = parse_primary(ps);
    if(!left) return NULL;

    while(match(ps,TOK_STAR) || match(ps,TOK_SLASH)) {
        Binop_type op;
        if(match(ps,TOK_STAR)) {
            op = OP_MUL;
        } else if(match(ps,TOK_SLASH)) {
            op = OP_DIV;
        } else {
            printf("Error line %d: Invalid arithmetic symbol '%c'\n",ps->lx->line,ps->lx->source[ps->lx->pos-1]);
            expr_destroy(left);
            return NULL;
        }

        advance(ps);
        Expr *right = parse_primary(ps);
        if(!right) {
            expr_destroy(left);
            return NULL;
        }

        left = expr_new_bin(left, op, right);
        if(!left) {
            expr_destroy(right);
            return NULL;
        }
    }

    return left;
}

Expr *parser_parse_expr(Parser *ps) {
    return parse_additive(ps);
}

/* COND - API */
static Cond_type tok_type_to_cond(Parser *ps) {
    switch(ps->current->type) {
        case TOK_EQEQ:
            return COND_EQEQ;
        case TOK_GE:
            return COND_GE;
        case TOK_GT:
            return COND_GT;
        case TOK_LE:
            return COND_LE;
        case TOK_LT:
            return COND_LT;
        case TOK_NOT_EQ:
            return COND_NOT_EQ;
        default:
            return -1;
    }
}

Condition *parser_parse_cond(Parser *ps) {
    Condition *cond;

    Expr *left = parser_parse_expr(ps);
    if(!left) {
        expr_destroy(left);
        return NULL;
    }

    if(ps->current->type != TOK_EQEQ 
        && ps->current->type != TOK_GE
        && ps->current->type != TOK_GT 
        && ps->current->type != TOK_LE
        && ps->current->type != TOK_LT
        && ps->current->type != TOK_NOT_EQ) {
        printf("Parser error line %d: invalid condition type. ",ps->lx->line);
        expr_destroy(left);
        return cond;
    }

    Cond_type type = tok_type_to_cond(ps);
    advance(ps);

    Expr *right = parser_parse_expr(ps);
    if(!right) {
        expr_destroy(left);
        expr_destroy(right);
        return NULL;
    }

    return cond_new(left, type, right);
}

/* AST - API */
Ast *parser_parse_metablhth(Parser *ps) {
    if(!consum(ps,TOK_METABLHTH,"expected 'metablhth'"))
        return NULL;

    if(!match(ps,TOK_IDENT_LIT)) {
        printf("Error line %d: expected variable name\n",ps->lx->line);
        return NULL;
    }

    char *name = strdup(ps->current->value);
    if(!name) return NULL;

    advance(ps);

    if(!consum(ps,TOK_ASSIGN,"expected '='")) {
        free(name);
        return NULL;
    }

    Expr *ex = parser_parse_expr(ps);
    if(!ex) return NULL;

    if(!consum(ps,TOK_SEMI,"expected ';' at the end of metablhth")) {
        expr_destroy(ex);
        free(name);
        return NULL;
    }

    Ast *new = ast_new_metablhth(name,ex);
    free(name);

    return new;
}

Ast *parser_parse_var_assign(Parser *ps) {
    if(!match(ps,TOK_IDENT_LIT)) {
        printf("Error line %d: expected variable name\n",ps->lx->line);
        return NULL;
    }

    char *name = strdup(ps->current->value);
    if(!name) return NULL;

    advance(ps);

    if(!consum(ps,TOK_ASSIGN,"expected '='")) {
        free(name);
        return NULL;
    }

    Expr *ex = parser_parse_expr(ps);
    if(!ex) return NULL;

    if(!consum(ps,TOK_SEMI,"expected ';' at the end of metablhth")) {
        expr_destroy(ex);
        free(name);
        return NULL;
    }

    Ast *new = ast_new_var_assign(name,ex);
    free(name);

    return new;
}

Ast *parser_parse_an(Parser *ps) {
    if(!consum(ps,TOK_AN,"expected 'an'")) 
        return NULL;

    Condition *cond = parser_parse_cond(ps);

    if(!consum(ps,TOK_LBRA,"expected '{' at the condition")) {
        cond_destroy(cond);
        return NULL;
    }

    Ast *body = NULL;
    while(!match(ps,TOK_RBRA)) {
        Ast *stmt = parser_parse_stmt(ps);
        if(!stmt) {
            ast_destroy(body);
            cond_destroy(cond);
            return NULL;
        }
        
        ast_append(&body,stmt);
    }

    if(!consum(ps,TOK_RBRA,"expected '}' at the end of an")) {
        ast_destroy(body);
        return NULL;
    }

    return ast_new_an(cond,body);
}

Ast *parser_parse_gia(Parser *ps) {
    if(!consum(ps,TOK_GIA,"expected 'gia'")) 
        return NULL;

    Expr *from = parser_parse_expr(ps);
    if(from->type == EXPR_STR) {
        printf("Error line %d: cannot put strings on gia\n",ps->lx->line);
        return NULL;
    }

    if(!consum(ps,TOK_EOS,"expected 'eos' at gia")) {
        expr_destroy(from);
        return NULL;
    }

    Expr *to = parser_parse_expr(ps);
    if(from->type == EXPR_STR) {
        printf("Error line %d: cannot put strings on gia\n",ps->lx->line);
        expr_destroy(from);
        return NULL;
    }

    if(!consum(ps,TOK_LBRA,"expected '{' at the condition")) {
        expr_destroy(from);
        expr_destroy(to);
        return NULL;
    }

    Ast *body = NULL;
    while(!match(ps,TOK_RBRA)) {
        Ast *stmt = parser_parse_stmt(ps);
        if(!stmt) {
            ast_destroy(body);
            expr_destroy(from);
            expr_destroy(to);
            return NULL;
        }
        
        ast_append(&body,stmt);
    }

    if(!consum(ps,TOK_RBRA,"expected '}' at the end of an")) {
        ast_destroy(body);
        expr_destroy(from);
        expr_destroy(to);
        return NULL;
    }

    return ast_new_gia(from,to,body);
}

Ast *parser_parse_print(Parser *ps) {
    if(!consum(ps,TOK_PRINT,"expected 'print'\n"))
        return NULL;

    Expr *ex = parser_parse_expr(ps);
    if(!ex) return NULL;

    if(ex->type == EXPR_READ_FILE || ex->type == EXPR_READ) {
        printf("Error line %d: cannot put diabase_arxeio or diabse on printent\n",ps->lx->line);
        expr_destroy(ex);
        return NULL;
    }

    if(!consum(ps,TOK_SEMI,"expected ';'"))  {
        expr_destroy(ex);
        return NULL;
    }
    
    return ast_new_print(ex);
}

Ast *parser_parse_printent(Parser *ps) {
    if(!consum(ps,TOK_PRINTENT,"expected 'printent'\n"))
        return NULL;

    Expr *ex = parser_parse_expr(ps);
    if(!ex) return NULL;

    if(ex->type == EXPR_READ_FILE || ex->type == EXPR_READ) {
        printf("Error line %d: cannot put diabase_arxeio or diabse on printent\n",ps->lx->line);
        expr_destroy(ex);
        return NULL;
    }

    if(!consum(ps,TOK_SEMI,"expected ';'"))  {
        expr_destroy(ex);
        return NULL;
    }
    
    return ast_new_printent(ex);
}

Ast *parser_parse_perimene(Parser *ps) {
    if(!consum(ps,TOK_PERIMENE,"expected 'perimene'\n"))
        return NULL;

    Expr *ex = parser_parse_expr(ps);
    if(!ex) return NULL;

    if(ex->type == EXPR_STR || ex->type == EXPR_READ || ex->type == EXPR_READ_FILE) {
        printf("Error line %d: cannot put string\n",ps->lx->line);
        expr_destroy(ex);
        return NULL;
    }

    if(!consum(ps,TOK_SEMI,"expected ';'"))  {
        expr_destroy(ex);
        return NULL;
    }
    
    return ast_new_perimene(ex);
}


Ast *parser_parse_sbyse(Parser *ps) {
    if(!consum(ps,TOK_SBYSE,"expected 'sbyse'\n"))
        return NULL;

    Expr *ex = parser_parse_expr(ps);
    if(!ex) return NULL;

    if(ex->type == EXPR_INT || ex->type == EXPR_READ || ex->type == EXPR_READ_FILE || ex->type == EXPR_BIN) {
        printf("Error line %d: cannot put number\n",ps->lx->line);
        expr_destroy(ex);
        return NULL;
    }

    if(!consum(ps,TOK_SEMI,"expected ';'"))  {
        expr_destroy(ex);
        return NULL;
    }
    
    return ast_new_sbyse(ex);
}


Ast *parser_parse_treje(Parser *ps) {
    if(!consum(ps,TOK_TREJE,"expected 'treje'\n"))
        return NULL;

    Expr *ex = parser_parse_expr(ps);
    if(!ex) return NULL;

    if(ex->type == EXPR_INT || ex->type == EXPR_READ || ex->type == EXPR_READ_FILE || ex->type == EXPR_BIN) {
        printf("Error line %d: cannot put number\n",ps->lx->line);
        expr_destroy(ex);
        return NULL;
    }

    if(!consum(ps,TOK_SEMI,"expected ';'"))  {
        expr_destroy(ex);
        return NULL;
    }
    
    return ast_new_treje(ex);
}

Ast *parser_parse_neosfakelos(Parser *ps) {
    if(!consum(ps,TOK_NEOSFAKELOS,"expected 'treje'\n"))
        return NULL;

    Expr *ex = parser_parse_expr(ps);
    if(!ex) return NULL;

    if(ex->type == EXPR_INT || ex->type == EXPR_READ || ex->type == EXPR_READ_FILE || ex->type == EXPR_BIN) {
        printf("Error line %d: cannot put number\n",ps->lx->line);
        expr_destroy(ex);
        return NULL;
    }

    if(!consum(ps,TOK_SEMI,"expected ';'"))  {
        expr_destroy(ex);
        return NULL;
    }
    
    return ast_new_neosfakelos(ex);
}

Ast *parser_parse_telosprograma(Parser *ps) {
    if(!consum(ps,TOK_TELOS_PROGRAMA,"expected 'telosprograma'\n"))
        return NULL;

    if(!consum(ps,TOK_SEMI,"expected ';'")) {
        return NULL;
    }
    
    return ast_new_telosprograma();
}


Ast *parser_parse_ka8arise(Parser *ps) {
    if(!consum(ps,TOK_KA8ARISE,"expected 'ka8arise'\n"))
        return NULL;

    if(!consum(ps,TOK_SEMI,"expected ';'"))  {
        return NULL;
    }
    
    return ast_new_ka8arise();
}

Ast *parser_parse_stmt(Parser *ps) {
    if(match(ps,TOK_METABLHTH)) {
        return parser_parse_metablhth(ps);
    } else if(match(ps,TOK_IDENT_LIT)) {
        return parser_parse_var_assign(ps);
    } else if(match(ps,TOK_AN)) {
        return parser_parse_an(ps);
    } else if(match(ps,TOK_GIA)) {
        return parser_parse_gia(ps);
    } else if(match(ps,TOK_PRINT)) {
        return parser_parse_print(ps);
    } else if(match(ps,TOK_PRINTENT)) {
        return parser_parse_printent(ps);
    } else if(match(ps,TOK_PERIMENE)) {
        return parser_parse_perimene(ps);
    } else if(match(ps,TOK_SBYSE)) {
        return parser_parse_sbyse(ps);
    } else if(match(ps,TOK_TREJE)) {
        return parser_parse_treje(ps);
    } else if(match(ps,TOK_NEOSFAKELOS)) {
        return parser_parse_neosfakelos(ps);
    } else if(match(ps,TOK_TELOS_PROGRAMA)) {
        return parser_parse_telosprograma(ps);
    } else if(match(ps,TOK_KA8ARISE)) {
        return parser_parse_ka8arise(ps);
    } else {
        printf("Error: Inexprid value to start '%s'\n",ps->current->value);
        advance(ps);
        return NULL;
    }
}

Ast *parser_parse_program(Parser *ps) {
    Ast *head = NULL;
    while(ps->current->type != TOK_EOF) {
        Ast *new = parser_parse_stmt(ps);
        if(!new) {
            ast_destroy(head);
            return NULL;
        }

        ast_append(&head, new);
    }

    return head;
}
