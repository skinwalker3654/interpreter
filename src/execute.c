#include "../header/execute.h"
#include "../header/value.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

static Value eval_expr(Variable_list *list, Expr *expr);

static Value eval_binary(Variable_list *list, Expr *expr) {
    Value left = eval_expr(list, expr->bin.left);
    Value right = eval_expr(list, expr->bin.right);

    if(left.type == VAL_ERR || right.type == VAL_ERR)
        return (Value){ .type = VAL_ERR };

    switch(expr->bin.op) {
        case OP_ADD:
            if(left.type == VAL_INT && right.type == VAL_INT)
                return value_new_int(left.int_value + right.int_value);

            break;

        case OP_SUB:
            if(left.type == VAL_INT && right.type == VAL_INT)
                return value_new_int(left.int_value - right.int_value);

            break;

        case OP_MUL:
            if(left.type == VAL_INT && right.type == VAL_INT)
                return value_new_int(left.int_value * right.int_value);

            break;

        case OP_DIV:
            if(left.type == VAL_INT && right.type == VAL_INT)
                return value_new_int(left.int_value / right.int_value);

            break;
    }

    value_destroy(&left);
    value_destroy(&right);
    return (Value){ .type = VAL_ERR };
}

static Value lookup_variable(Variable_list *list, char *name) {
    int index = get_variable_index(list, name);
    if(index == -1) {
        printf("Κάπου στον κόδικα έχεις γράψη '%s' αλα αυτή η μεταβλητή ΔΕΝ υπάρχει κάπου\n", name);
        return (Value){ .type = VAL_ERR };
    }

    return eval_expr(list, list->vars[index].ex);
}

static Value eval_expr(Variable_list *list, Expr *expr) {
    Value v;
    switch(expr->type) {
        case EXPR_INT:
            return value_new_int(expr->int_value);
        case EXPR_STR:
            return value_new_string(expr->str_value);
        case EXPR_IDENT:
            return lookup_variable(list, expr->ident);
        case EXPR_MHKOS:
            v = eval_expr(list,expr->mhkos.expr);
            if(v.type == VAL_INT) {
                printf("Error: Έβαλες μεταβλητή που έχει μέσα αριθμό στην εντολή μήκος. Μόνο μηνήματα ή μεταβλητές με μηνήματα μπαίνουν εκεί\n");
                return (Value){.type=VAL_ERR};
            }

            Value val = value_new_int(strlen(v.string_value));
            value_destroy(&v);

            return val;
        case EXPR_BIN:
            return eval_binary(list, expr);
        default:
            return (Value){ .type = VAL_ERR };
    }
}

static int eval_condition(Condition *cond, Variable_list *list) {
    Value left = eval_expr(list,cond->left);
    Value right = eval_expr(list,cond->right);
    
    if(left.type == VAL_STR || right.type == VAL_STR) {
        printf("Error: Κάπου στον κόδικα έχεις βάλει μια μεταβλητή μέσα στο an και αυτή η μεταβλητή έχει μήνημα μέσα της. ΔΕΝ γίνεται να βάλεις μεταβλητές που έχουν μήνημα μεσα στο an\n");
        value_destroy(&left);
        value_destroy(&right);
        return 0;
    }

    int result = 0;
    switch(cond->type) {
        case COND_EQEQ:
            result = left.int_value == right.int_value;
            break;
        case COND_GE:
            result = left.int_value >= right.int_value;
            break;
        case COND_GT:
            result = left.int_value > right.int_value;
            break;
        case COND_LE:
            result = left.int_value <= right.int_value;
            break;
        case COND_LT:
            result = left.int_value < right.int_value;
            break;
        case COND_NOT_EQ:
            result = left.int_value != right.int_value;
            break;
    }

    value_destroy(&left);
    value_destroy(&right);

    return result;
}

int execute_program(Ast *ast, Variable_list *list);

void get_new_line() {
    char ch;
    while((ch = getchar())!='\n'&&ch!=EOF);
}

int execute_metablhth(Ast *ast, Variable_list *list) {
    char *input_buff = NULL;
    size_t input_buff_size = 0;
    int number;
    Expr *ex;

    FILE *fp;
    Value v;
    fflush(stdout);
    switch(ast->data.metablhth.expr->type) {
        case EXPR_READ:
            switch(ast->data.metablhth.expr->read.type) {
                case READ_BUFF:
                    printf("%s",ast->data.metablhth.expr->read.prompt);
                    getline(&input_buff,&input_buff_size,stdin);
                    input_buff[strcspn(input_buff,"\n")] = '\0';
                    ex = expr_new_str(input_buff);
                    push_variable_value(list,ast->data.metablhth.varname,ex);
                    expr_destroy(ex);
                    free(input_buff);
                    return 0;
                case READ_NUM:
                    printf("%s",ast->data.metablhth.expr->read.prompt);
                    if(scanf("%d",&number)!=1) {
                        printf("Error: Ο χρήστης δεν έδωσε αριθμό σωστό αριθμό\n");
                        get_new_line();
                        return -1;
                    }
                    get_new_line();

                    ex = expr_new_int(number);
                    push_variable_value(list,ast->data.metablhth.varname,ex);
                    expr_destroy(ex);
                    return 0;
            }
        case EXPR_READ_FILE:
            v = eval_expr(list,ast->data.metablhth.expr->read_file.expr);
            fp = fopen(v.string_value,"r");
            if(!fp) {
                printf("Error: Το πρόγραμα απέτηχε να ανοίξη το αρχείο %s σιγουρέψου ότι υπάρχει\n",v.string_value);
                value_destroy(&v);
                return -1;
            }

            fseek(fp,0,SEEK_END);
            int size = ftell(fp);
            rewind(fp);

            char *buffer = malloc(size+1);
            if(!buffer) {
                value_destroy(&v);
                fclose(fp);
                return -1;
            }

            int len = fread(buffer,1,size,fp);
            buffer[len] = '\0';
            fclose(fp);

            ex = expr_new_str(buffer);
            push_variable_value(list,ast->data.metablhth.varname,ex);
            expr_destroy(ex);
            value_destroy(&v);
            free(buffer);
            return 0;
        default:
            v = eval_expr(list,ast->data.metablhth.expr);
            switch(v.type) {
                case VAL_INT:
                    ex = expr_new_int(v.int_value);
                    push_variable_value(list,ast->data.metablhth.varname,ex);
                    expr_destroy(ex);
                    break;
                case VAL_STR:
                    ex = expr_new_str(v.string_value);
                    push_variable_value(list,ast->data.metablhth.varname,ex);
                    expr_destroy(ex);
                    break;
                case VAL_ERR:
                    printf("Εrror: Έβαλες μεταβλητή με μήνημα μεσα σε αριθμητική πράξη\n");
                    value_destroy(&v);
                    return -1;
            }

            value_destroy(&v);
            return 0;
    }
}

int execute_var_assign(Ast *ast, Variable_list *list) {
    char *input_buff = NULL;
    size_t input_buff_size = 0;
    int number;
    Expr *ex;

    FILE *fp;
    Value v;
    fflush(stdout);
    switch(ast->data.var_assign.expr->type) {
        case EXPR_READ:
            switch(ast->data.var_assign.expr->read.type) {
                case READ_BUFF:
                    printf("%s",ast->data.var_assign.expr->read.prompt);
                    getline(&input_buff,&input_buff_size,stdin);
                    input_buff[strcspn(input_buff,"\n")] = '\0';
                    ex = expr_new_str(input_buff);
                    set_variable_value(list,ast->data.var_assign.varname,ex);
                    expr_destroy(ex);
                    free(input_buff);
                    return 0;
                case READ_NUM:
                    printf("%s",ast->data.var_assign.expr->read.prompt);
                    if(scanf("%d",&number)!=1) {
                        printf("Error: Ο χρήστης δεν έδωσε αριθμό σωστό αριθμό\n");
                        get_new_line();
                        return -1;
                    }
                    get_new_line();

                    ex = expr_new_int(number);
                    set_variable_value(list,ast->data.var_assign.varname,ex);
                    expr_destroy(ex);
                    return 0;
            }
        case EXPR_READ_FILE:
            v = eval_expr(list,ast->data.var_assign.expr->read_file.expr);
            fp = fopen(v.string_value,"r");
            if(!fp) {
                printf("Error: Το πρόγραμα απέτηχε να ανοίξη το αρχείο %s σιγουρέψου ότι υπάρχει\n",v.string_value);
                value_destroy(&v);
                return -1;
            }

            fseek(fp,0,SEEK_END);
            int size = ftell(fp);
            rewind(fp);

            char *buffer = malloc(size+1);
            if(!buffer) {
                value_destroy(&v);
                fclose(fp);
                return -1;
            }

            int len = fread(buffer,1,size,fp);
            buffer[len] = '\0';
            fclose(fp);

            ex = expr_new_str(buffer);
            set_variable_value(list,ast->data.var_assign.varname,ex);
            expr_destroy(ex);
            value_destroy(&v);
            free(buffer);
            return 0;
        default:
            v = eval_expr(list,ast->data.var_assign.expr);
            switch(v.type) {
                case VAL_INT:
                    ex = expr_new_int(v.int_value);
                    set_variable_value(list,ast->data.var_assign.varname,ex);
                    expr_destroy(ex);
                    break;
                case VAL_STR:
                    ex = expr_new_str(v.string_value);
                    set_variable_value(list,ast->data.var_assign.varname,ex);
                    expr_destroy(ex);
                    break;
                case VAL_ERR:
                    printf("Εrror: Έβαλες μεταβλητή με μήνημα μεσα σε αριθμητική πράξη\n");
                    value_destroy(&v);
                    return -1;
            }

            value_destroy(&v);
            return 0;
    }

}


int execute_an(Ast *ast, Variable_list *list) {
    int result;
    fflush(stdout);
    if(eval_condition(ast->data.an.cond,list)) {
        result = execute_program(ast->data.an.body,list);
        if(result == 1) return 1;
        else if(result == -1) return -1;
    }

    return 0;
}

int execute_gia(Ast *ast, Variable_list *list) {
    Value from = eval_expr(list,ast->data.gia.from);
    Value to = eval_expr(list,ast->data.gia.to);

    if(from.type == VAL_STR || to.type == VAL_STR) {
        printf("Error: Κάπου στον κόδικα έχεις βάλει μια μεταβλητή μέσα στο gia και αυτή η μεταβλητή έχει μήνημα μέσα της. ΔΕΝ γίνεται να βάλεις μεταβλητές που έχουν μήνημα μέσα στο gia\n");
        value_destroy(&from);
        value_destroy(&to);
        return -1;
    }

    fflush(stdout);
    for(int i=from.int_value; i<=to.int_value; i++) {
        int result = execute_program(ast->data.gia.body,list);
        if(result == 1) {
            value_destroy(&from);
            value_destroy(&to);
            return 1;
        } else if(result == -1) {
            value_destroy(&from);
            value_destroy(&to);
            return -1;
        }
    }

    value_destroy(&from);
    value_destroy(&to);

    return 0;
}

int execute_print(Ast *ast, Variable_list *list) {
    Value v = eval_expr(list,ast->data.print.expr);
    fflush(stdout);
    switch(v.type) {
        case VAL_INT:
            printf("%d",v.int_value);
            break;
        case VAL_STR:
            printf("%s",v.string_value);
            break;
        default:
            break;
    }

    value_destroy(&v);
    return 0;
}

int execute_printent(Ast *ast, Variable_list *list) {
    Value v = eval_expr(list,ast->data.print.expr);
    fflush(stdout);
    switch(v.type) {
        case VAL_INT:
            printf("%d\n",v.int_value);
            break;
        case VAL_STR:
            printf("%s\n",v.string_value);
            break;
        default:
            break;
    }

    value_destroy(&v);
    return 0;
}


int execute_perimene(Ast *ast, Variable_list *list) {
    fflush(stdout);
    Value v = eval_expr(list,ast->data.print.expr);
    sleep(v.int_value);
    value_destroy(&v);
    return 0;
}

int execute_sbyse(Ast *ast, Variable_list *list) {
    Value v = eval_expr(list,ast->data.sbyse.expr);
    if(remove(v.string_value)!=0) {
        printf("Error: Το πρόγραμα απέτηχε να σβήσει το αρχείο/φάκελο με όνομα '%s' σιγουρέψου οτι υπάρχει\n",v.string_value);
        value_destroy(&v);
        return -1;
    }
    value_destroy(&v);
    return 0;
}

int execute_neosfakelos(Ast *ast, Variable_list *list) {
    Value v = eval_expr(list,ast->data.sbyse.expr);
    #ifdef _WIN32
        if(_mkdir(v.string_value)==-1) {
            printf("Error: Το πρόγραμα απέτηχε να δημιουργίσει τον φάκελο με όνομα '%s' σιγουρέψου οτι δεν υπάρχει ήδη αυτός ο φάκελος γτ το πρόγραμα δεν μπορεί να φτιάξει φάκελους με όνομα που ήδη υπάρχει στο σύστημα σου\n",v.string_value);
            value_destroy(&v);
            return -1;
        }
        value_destroy(&v);
    #else
        if(mkdir(v.string_value,0755)==-1) {
            printf("Error: Το πρόγραμα απέτηχε να δημιουργίσει τον φάκελο με όνομα '%s' σιγουρέψου οτι δεν υπάρχει ήδη αυτός ο φάκελος γτ το πρόγραμα δεν μπορεί να φτιάξει φάκελους με όνομα που ήδη υπάρχει στο σύστημα σου\n",v.string_value);
            value_destroy(&v);
            return -1;
        }
        value_destroy(&v);
    #endif
    return 0;
}

int execute_treje(Ast *ast, Variable_list *list) {
    Value v = eval_expr(list,ast->data.treje.expr);
    fflush(stdout);
    system(v.string_value);
    value_destroy(&v);
    return 0;
}

int execute_program(Ast *ast, Variable_list *list) {
    int result;
    while(ast != NULL) {
        switch(ast->type) {
            case AST_METABLHTH:
                if(execute_metablhth(ast,list)==-1) return -1;
                break;
            case AST_VAR_ASSIGN:
                if(execute_var_assign(ast,list)==-1) return -1;
                break;
            case AST_AN:
                result = execute_an(ast,list);
                if(result == 1) return 1;
                else if(result == -1) return -1;
                break;
            case AST_GIA:
                result = execute_gia(ast,list);
                if(result == 1) return 0;
                if(result == -1) return -1;
                break;
            case AST_PRINT:
                if(execute_print(ast,list)==-1) return -1;
                break;
            case AST_PRINTENT:
                if(execute_printent(ast,list)==-1) return -1;
                break;
            case AST_PERIMENE:
                if(execute_perimene(ast,list)==-1) return -1;
                break;
            case AST_SBYSE:
                if(execute_sbyse(ast,list)==-1) return -1;
                break;
            case AST_TREJE:
                if(execute_treje(ast,list)==-1) return -1;
                break;
            case AST_NEOSFAKELOS:
                if(execute_neosfakelos(ast,list)==-1) return -1;
                break;
            case AST_KA8ARISE:
                #ifdef _WIN32
                    system("cls");
                #else 
                    system("clear");
                #endif

                break;
            case AST_TELOSPROGRAMA:
                return 1;
        }
        ast = ast->next;
    }

    return 0;
}
