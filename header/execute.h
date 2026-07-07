#ifndef EXECUTE_H
#define EXECUTE_H

#include "ast.h"
#include "var.h"

int execute_metablhth(Ast *ast,Variable_list *list);
int execute_var_assign(Ast *ast,Variable_list *list);
int execute_an(Ast *ast,Variable_list *list);
int execute_gia(Ast *ast,Variable_list *list);
int execute_perimene(Ast *ast,Variable_list *list);
int execute_print(Ast *ast,Variable_list *list);
int execute_printent(Ast *ast,Variable_list *list);
int execute_sbyse(Ast *ast,Variable_list *list);
int execute_treje(Ast *ast,Variable_list *list);
int execute_neosfakelos(Ast *ast,Variable_list *list);
int execute_telosprograma(Ast *ast,Variable_list *list);
int execute_ka8arise(Ast *ast,Variable_list *list);
int execute_program(Ast *ast,Variable_list *list);

#endif
