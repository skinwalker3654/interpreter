#ifndef AST_H
#define AST_H

typedef enum {
    AST_METABLHTH,
    AST_VAR_ASSIGN,
    AST_PRINT,
    AST_PRINTENT,
    AST_PERIMENE,
    AST_GIA,
    AST_AN,
    AST_KA8ARISE,
    AST_SBYSE,
    AST_TELOSPROGRAMA,
    AST_TREJE,
    AST_NEOSFAKELOS,
} Ast_type;

typedef enum {
    EXPR_INT,
    EXPR_STR,
    EXPR_IDENT,
    EXPR_READ_FILE,
    EXPR_READ,
    EXPR_BIN,
    EXPR_MHKOS,
} Expr_type;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
} Binop_type;

typedef enum {
    READ_NUM,
    READ_BUFF,
} Read_type;

typedef struct Expr Expr;
typedef struct Expr {
    Expr_type type;
    union {
        int int_value;
        char *str_value;
        char *ident;

        struct {
            Expr *left;
            Binop_type op;
            Expr *right;
        } bin;
        struct {
            char *prompt;
            Read_type type;
        } read;
        struct {
            Expr *expr;
        } mhkos;
        struct {
            Expr *expr; 
        } read_file;
    };
} Expr;

typedef enum {
    COND_EQEQ,
    COND_GE,
    COND_GT,
    COND_LE,
    COND_LT,
    COND_NOT_EQ,
} Cond_type;

typedef struct {
    Expr *left;
    Cond_type type;
    Expr *right;
} Condition;

typedef struct Ast {
    Ast_type type;
    union {
        struct {
            char *varname;
            Expr *expr;
        } metablhth;
        struct {
            char *varname;
            Expr *expr;
        } var_assign;
        struct {
            Expr *expr;
        } print;
        struct {
            Expr *expr;
        } printent;
        struct {
            Expr *expr;
        } perimene;
        struct {
            Expr *from;
            Expr *to;
            struct Ast *body;
        } gia;
        struct {
            Condition *cond;
            struct Ast *body;
        } an;
        struct {
            Expr *expr;
        } sbyse;
        struct {
            Expr *expr;
        } treje;
        struct {
            Expr *expr;
        } neosfakelos;
    } data;
    struct Ast *next;
} Ast;

Expr *expr_new_int(int num);
Expr *expr_new_str(char *str);
Expr *expr_new_ident(char *ident);
Expr *expr_new_read(char *promt, Read_type type);
Expr *expr_new_read_file(Expr *expr);
Expr *expr_new_mhkos(Expr *expr);
Expr *expr_new_bin(Expr *left, Binop_type op, Expr *right);

Expr *expr_copy_expr(Expr *ex);
void expr_destroy(Expr *ex);

Condition *cond_new(Expr *left, Cond_type type, Expr *right);
void cond_destroy(Condition *cond);

Ast *ast_new_metablhth(char *varname, Expr *ex);
Ast *ast_new_var_assign(char *varname, Expr *ex);
Ast *ast_new_print(Expr *ex);
Ast *ast_new_printent(Expr *ex);
Ast *ast_new_perimene(Expr *ex);
Ast *ast_new_an(Condition *cond, Ast *body);
Ast *ast_new_gia(Expr *from, Expr *to, Ast *body);
Ast *ast_new_sbyse(Expr *ex);
Ast *ast_new_treje(Expr *ex);
Ast *ast_new_neosfakelos(Expr *ex);
Ast *ast_new_telosprograma();
Ast *ast_new_ka8arise();

void ast_append(Ast **head,Ast *node);
void ast_destroy(Ast *ast);

#endif
