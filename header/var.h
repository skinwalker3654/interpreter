#ifndef VAR_H
#define VAR_H

#include "parser.h"

typedef struct {
    char *name;
    Expr *ex;
} Variable;

typedef struct {
    Variable *vars;
    int counter;
    int capacity;
} Variable_list;

Variable_list *variable_list_init();
void variable_list_free(Variable_list *list);

int get_variable_index(Variable_list *list, char *varname);
int push_variable_value(Variable_list *list, char *varname, Expr *ex);
int set_variable_value(Variable_list *list, char *varname, Expr *ex);

#endif
