#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct Parser {
    Lexer lexer;
    Token current_token;
} Parser;

typedef enum {
    INT,
    STRING,
} Variable_type;

typedef enum {
    PRINT,
    PRINTLN,
} Print_type;

typedef struct Variables {
    char stringvalue[200][256];
    int intvalue[200];
    char variablename[200][256];
    Variable_type type[200];
    int counter;
} Variables;

void add_variable(Variables *ptr,char *varname,void *value,Variable_type type);
int print_variable(Variables *ptr,char *varname,Print_type type);
void wait_seconds(int seconds);
int parse_code(Parser *ptr,Variables *var);

#endif
