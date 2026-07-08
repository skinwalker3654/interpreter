#include "../header/ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Expr *expr_new_ident(char *ident) {
    Expr *ex = malloc(sizeof(Expr));
    if(!ex) {
        printf("expr_new_ident: Failed to create the expression\n");
        return NULL;
    }

    ex->type = EXPR_IDENT;
    ex->ident = strdup(ident);
    if(!ex->ident) {
        printf("expr_new_ident: Failed to create the ident value\n");
        free(ex);
        return NULL;
    }

    return ex;
}

Expr *expr_new_int(int num) {
    Expr *ex = malloc(sizeof(Expr));
    if(!ex) {
        printf("expr_new_int: Failed to create the expression\n");
        return NULL;
    }

    ex->type = EXPR_INT;
    ex->int_value = num;

    return ex;
}

Expr *expr_new_str(char *str) {
    Expr *ex = malloc(sizeof(Expr));
    if(!ex) {
        printf("expr_new_str: Failed to create the expression\n");
        return NULL;
    }

    ex->type = EXPR_STR;
    ex->str_value = strdup(str);
    if(!ex->str_value) {
        printf("expr_new_str: Failed to create the string value\n");
        free(ex);
        return NULL;
    }

    return ex;
}

Expr *expr_new_read(char *promt, Read_type type) {
    Expr *ex = malloc(sizeof(Expr));
    if(!ex) {
        printf("expr_new_read: Failed to create the expression\n");
        return NULL;
    }

    ex->type = EXPR_READ;
    ex->read.type = type;
    ex->read.prompt = strdup(promt);
    if(!ex->str_value) {
        printf("expr_new_str: Failed to create the string value\n");
        free(ex);
        return NULL;
    }

    return ex;
}

Expr *expr_new_read_file(Expr *expr) {
    Expr *ex = malloc(sizeof(Expr));
    if(!ex) {
        printf("expr_new_read: Failed to create the expression\n");
        return NULL;
    }

    ex->type = EXPR_READ_FILE;
    ex->read_file.expr = expr;

    return ex;
}

Expr *expr_new_mhkos(Expr *expr) {
    Expr *ex = malloc(sizeof(Expr));
    if(!ex) {
        printf("expr_new_mhkos: Failed to create the expression\n");
        return NULL;
    }

    ex->type = EXPR_MHKOS;
    ex->mhkos.expr = expr;

    return ex;
}


Expr *expr_new_bin(Expr *left, Binop_type op, Expr *right) {
    Expr *ex = malloc(sizeof(Expr));
    if(!ex) {
        printf("expr_new_read: Failed to create the expression\n");
        return NULL;
    }

    ex->type = EXPR_BIN;
    ex->bin.left = left;
    ex->bin.op = op;
    ex->bin.right = right;

    return ex;
}

Expr *expr_copy_expr(Expr *ex) {
    switch(ex->type) {
        case EXPR_INT:
            return expr_new_int(ex->int_value);
        case EXPR_IDENT:
            return expr_new_ident(ex->ident);
        case EXPR_STR:
            return expr_new_str(ex->str_value);
        case EXPR_READ:
            return expr_new_read(ex->read.prompt,ex->read.type);
        case EXPR_READ_FILE:
            return expr_new_read_file(expr_copy_expr(ex->read_file.expr));
        case EXPR_MHKOS:
            return expr_new_mhkos(expr_copy_expr(ex->mhkos.expr));
        case EXPR_BIN:
            return expr_new_bin(expr_copy_expr(ex->bin.left), ex->bin.op, expr_copy_expr(ex->bin.right));
    }

    return NULL;
}

void expr_destroy(Expr *ex) {
    if(!ex) return;

    switch(ex->type) {
        case EXPR_INT: 
            break;
        case EXPR_IDENT:
            free(ex->ident);
            break;
        case EXPR_STR:
            free(ex->str_value);
            break;
        case EXPR_READ:
            free(ex->read.prompt);
            break;
        case EXPR_READ_FILE:
            expr_destroy(ex->read_file.expr);
            break;
        case EXPR_MHKOS:
            expr_destroy(ex->mhkos.expr);
            break;
        case EXPR_BIN:
            expr_destroy(ex->bin.left);
            expr_destroy(ex->bin.right);
            break;
    }

    free(ex);
}

Condition *cond_new(Expr *left, Cond_type type, Expr *right) {
    Condition *new_cond = malloc(sizeof(Condition));
    if(!new_cond) {
        printf("cond_new: Failed to create the condition\n");
        return NULL;
    }

    new_cond->left = left;
    new_cond->type = type;
    new_cond->right = right;

    return new_cond;
}

void cond_destroy(Condition *cond) {
    expr_destroy(cond->left);
    expr_destroy(cond->right);
    free(cond);
}

Ast *ast_new_metablhth(char *varname, Expr *ex) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_METABLHTH;
    new_ast->data.metablhth.expr = ex;

    new_ast->data.metablhth.varname = strdup(varname);
    if(!new_ast->data.metablhth.varname) {
        printf("Failed to create the exariable ast\n");
        free(new_ast);
        return NULL;
    }
    
    new_ast->next = NULL;
    return new_ast;
}

Ast *ast_new_var_assign(char *varname, Expr *ex) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_VAR_ASSIGN;
    new_ast->data.var_assign.expr = ex;

    new_ast->data.var_assign.varname = strdup(varname);
    if(!new_ast->data.metablhth.varname) {
        printf("Failed to create the exariable ast\n");
        free(new_ast);
        return NULL;
    }
    
    new_ast->next = NULL;
    return new_ast;
}

Ast *ast_new_print(Expr *ex) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_PRINT;
    new_ast->data.print.expr = ex;

    new_ast->next = NULL;
    return new_ast;
}

Ast *ast_new_printent(Expr *ex) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_PRINTENT;
    new_ast->data.print.expr = ex;

    new_ast->next = NULL;
    return new_ast;
}

Ast *ast_new_perimene(Expr *ex) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_PERIMENE;
    new_ast->data.perimene.expr = ex;

    new_ast->next = NULL;
    return new_ast;
}

Ast *ast_new_an(Condition *cond, Ast *body) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast)  {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_AN;
    new_ast->data.an.cond = cond;
    new_ast->data.an.body = body;

    new_ast->next = NULL;
    return new_ast;
}

Ast *ast_new_gia(Expr *from, Expr *to, Ast *body) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast)  {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_GIA;
    new_ast->data.gia.from = from;
    new_ast->data.gia.to = to;
    new_ast->data.gia.body = body;

    new_ast->next = NULL;
    return new_ast;
}

Ast *ast_new_sbyse(Expr *ex) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_SBYSE;
    new_ast->data.sbyse.expr = ex;

    new_ast->next = NULL;
    return new_ast;
}

Ast *ast_new_treje(Expr *ex) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_TREJE;
    new_ast->data.treje.expr = ex;

    new_ast->next = NULL;
    return new_ast;
}

Ast *ast_new_neosfakelos(Expr *ex) {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_NEOSFAKELOS;
    new_ast->data.neosfakelos.expr = ex;

    new_ast->next = NULL;
    return new_ast;
}

Ast *ast_new_ka8arise() {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_KA8ARISE;
    new_ast->next = NULL;

    return new_ast;
}

Ast *ast_new_telosprograma() {
    Ast *new_ast = malloc(sizeof(Ast));
    if(!new_ast) {
        printf("Failed to create the ast node\n");
        return NULL;
    }

    new_ast->type = AST_TELOSPROGRAMA;
    new_ast->next = NULL;

    return new_ast;
}

void ast_append(Ast **head, Ast *node) {
    if(*head == NULL) {
        *head = node;
        return;
    }

    Ast *temp = *head;
    while(temp->next != NULL)
        temp = temp->next;
    temp->next = node;
}

void ast_destroy(Ast *ast) {
    if(!ast) return;

    Ast *temp;
    while(ast != NULL) {
        temp = ast;
        ast = ast->next;
        switch(temp->type) {
            case AST_METABLHTH:
                free(temp->data.metablhth.varname);
                expr_destroy(temp->data.metablhth.expr);
                break;
            case AST_VAR_ASSIGN:
                free(temp->data.var_assign.varname);
                expr_destroy(temp->data.var_assign.expr);
                break;
            case AST_AN:
                cond_destroy(temp->data.an.cond);
                ast_destroy(temp->data.an.body);
                break;
            case AST_GIA:
                expr_destroy(temp->data.gia.from);
                expr_destroy(temp->data.gia.to);
                ast_destroy(temp->data.gia.body);
                break;
            case AST_PRINT:
                expr_destroy(temp->data.print.expr);
                break;
            case AST_PRINTENT:
                expr_destroy(temp->data.printent.expr);
                break;
            case AST_PERIMENE:
                expr_destroy(temp->data.perimene.expr);
                break;
            case AST_SBYSE:
                expr_destroy(temp->data.sbyse.expr);
                break;
            case AST_TREJE:
                expr_destroy(temp->data.treje.expr);
                break;
            case AST_NEOSFAKELOS:
                expr_destroy(temp->data.neosfakelos.expr);
                break;
            default:
                break;
        }

        free(temp);
    }
}
