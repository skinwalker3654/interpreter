#include <stdio.h>
#include <stdlib.h>
#include "../header/parser.h"
#include "../header/execute.h"
#include "../header/var.h"

int main(int argc,char **argv) {
    if(argc != 2) {
        printf("Error: Invalid usage of the tool\n");
        printf("Correct usage: ./main <file_name>\n");
        return 1;
    }

    FILE *file = fopen(argv[1],"r");
    if(!file) {
        printf("Error: Failed to open the file '%s'\n",argv[1]);
        return 1;
    }

    fseek(file,0,SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *buffer = malloc(size+1);
    if(!buffer) {
        printf("Failed to create the source code\n");
        fclose(file);
        return 1;
    }

    size_t len = fread(buffer,1,size,file);
    buffer[len] = '\0';
    fclose(file);

    Variable_list *list = variable_list_init();
    Parser *ps = parser_init(buffer);

    Ast *ast = parser_parse_program(ps);
    if(ast == NULL) goto CLEAN_UP;

    if(execute_program(ast, list)==-1) goto CLEAN_UP;

CLEAN_UP:
    ast_destroy(ast);
    parser_destroy(ps);

    variable_list_free(list);
    free(buffer);

    return 0;
}
