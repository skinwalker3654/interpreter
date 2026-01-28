#include "parser.h"
#include <stdio.h>

int main(int argc,char *argv[]) {
    if(argc != 2) {
        printf("Error: Invalid arguments passed\n");
        printf("Correct usage: ./main <filename>\n");
        return 1;
    }

    Parser parser = {0};
    Variables variables = {0};
    FILE *file = fopen(argv[1],"r");
    if(!file) {
        printf("Error: Failed to open the file '%s'\n",argv[1]);
        return 1;
    }

    fseek(file,0,SEEK_END);
    int size = ftell(file);
    rewind(file);

    char buffer[size+1];
    fread(buffer,1,size,file);
    buffer[size] = '\0';

    parser.lexer = lexer_init(buffer);
    while(parser.current_token.type != TOKEN_EOF) {
        if(parse_code(&parser,&variables)==-1) {
            return 1;
        }
    }

    fclose(file);
    return 0;
}
