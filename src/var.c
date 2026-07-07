#include "../header/var.h"
#include "../header/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Variable_list *variable_list_init() {
    Variable_list *list = malloc(sizeof(Variable_list));
    if(!list) {
        printf("Failed to create the variable list\n");
        return NULL;
    }

    list->vars = malloc(sizeof(Variable)*2);
    if(!list->vars) {
        printf("Failed to initialize the variable list\n");
        free(list);
        return NULL;
    }
    
    list->counter = 0;
    list->capacity = 2;

    return list;
}

void variable_list_free(Variable_list *list) {
    if(!list) return;

    for(int i=0; i<list->counter; i++) {
        free(list->vars[i].name);
        expr_destroy(list->vars[i].ex);
    }

    free(list->vars);
    free(list);
    list = NULL;
}

int get_variable_index(Variable_list *list, char *varname) {
    if(!list) {
        printf("Variable '%s' not found: variable list is empty\n",varname);
        return -1;
    }

    int index;
    int found = 0;
    for(int i=0; i<list->counter; i++) {
        if(strcmp(list->vars[i].name,varname)==0) {
            found = 1;
            index = i;
            break;
        }
    }

    if(found) return index;
    return -1;
}

int push_variable_value(Variable_list *list, char *varname, Expr *ex) {
    if(!list) {
        printf("List is not initialized\n");
        return -1;
    }

    if(list->counter >= list->capacity) {
        list->capacity *= 2;
        Variable *temp = realloc(list->vars, sizeof(Variable) * list->capacity);
        if(!temp) {
            printf("Failed to push the variable into the list\n");
            return -1;
        }

        list->vars = temp;
    }

    list->vars[list->counter].name = strdup(varname);
    if(!list->vars[list->counter].name) {
        printf("Failed to create variable name\n");
        return -1;
    }
    
    list->vars[list->counter].ex = expr_copy_expr(ex);
    list->counter++;

    return 0;
}

int set_variable_value(Variable_list *list, char *varname, Expr *ex) {
    if(!list) {
        printf("List is not initialized\n");
        return -1;
    }

    int index = get_variable_index(list, varname);
    if(index == -1) {
        printf("Error: Variable '%s' not found\n",varname);
        return -1;
    }
    
    expr_destroy(list->vars[index].ex);
    list->vars[index].ex = expr_copy_expr(ex);

    return 0;
}
