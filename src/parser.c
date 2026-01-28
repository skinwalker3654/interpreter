#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "parser.h"
#include "lexer.h"

void add_variable(Variables *ptr, char *varname, void *value, Variable_type type) {
    strcpy(ptr->variablename[ptr->counter],varname);
    if(type == INT) {
        int number = *(int*)value;
        ptr->intvalue[ptr->counter] = number;
        ptr->type[ptr->counter] = INT;
    } else {
        strcpy(ptr->stringvalue[ptr->counter],(char*)value);
        ptr->type[ptr->counter] = STRING;
    }

    ptr->counter++;
}

int print_variable(Variables *ptr, char *varname,Print_type type) {
    int found = -1;
    for(int i=0; i<ptr->counter; i++) {
        if(strcmp(ptr->variablename[i],varname)==0) {
            found = i;
            if(ptr->type[i] == INT) {
                if(type == PRINT) {
                    printf("%d",ptr->intvalue[i]);
                    return 0;
                } else if(type == PRINTLN) {
                    printf("%d\n",ptr->intvalue[i]);
                    return 0;
                }

                return -1;
            } else if(ptr->type[i] == STRING){
                if(type == PRINT) {
                    printf("%s",ptr->stringvalue[i]);
                    return 0;
                } else if(type == PRINTLN) {
                    printf("%s\n",ptr->stringvalue[i]);
                    return 0;
                }

                return -1;
            }
        }
    }

    if(found == -1) {
        printf("Error: Variable %s not found\n",varname);
        return -1;
    }

    return -1;
}

void wait_seconds(int seconds) {
    sleep(seconds);
}

int parse_code(Parser *ptr,Variables *var) {
    ptr->current_token = get_next_token(&ptr->lexer);
    if(ptr->current_token.type == TOKEN_EOF) return 0;

    if(ptr->current_token.type == TOKEN_VARIABLE) {
        int found = -1;
        for(int i=0; i<var->counter; i++) {
            if(strcmp(var->variablename[i],ptr->current_token.value)==0) {
                found = i;
                break;
            }
        }

        if(found == -1) {
            printf("Error %d:%d -> This variable does not exists\n",ptr->current_token.line,ptr->current_token.column);
            return -1;
        }

        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_EQUAL && ptr->current_token.type != TOKEN_INCREASE && ptr->current_token.type != TOKEN_DECREASE) {
            printf("Error %d:%d -> Invalid variable assigment '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        if(ptr->current_token.type == TOKEN_INCREASE) {
            if(var->type[found] != INT) {
                printf("Error %d:%d Cannot increase a string variable\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }


            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> Forgot the semicolon\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            var->intvalue[found]++;
            return 0;
        }

        if(ptr->current_token.type == TOKEN_DECREASE) {
            if(var->type[found] != INT) {
                printf("Error %d:%d Cannot decrease a string variable\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> Forgot the semicolon\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            var->intvalue[found]--;
            return 0;
        }

        if(ptr->current_token.type == TOKEN_EQUAL) {
            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_STRING && ptr->current_token.type != TOKEN_NUMBER) {
                printf("Error %d:%d -> Invalid variable value '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }
            
            if(ptr->current_token.type == TOKEN_STRING) {
                if(var->type[found] != STRING) {
                    printf("Error %d:%d This variable is string and you havent assinged a string on it\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                strcpy(var->stringvalue[found],ptr->current_token.value);
                ptr->current_token = get_next_token(&ptr->lexer);
                if(ptr->current_token.type != TOKEN_SEMICOLON) {
                    printf("Error %d:%d -> Forgot the semicolon\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                return 0;
            } else if(ptr->current_token.type == TOKEN_NUMBER) {
                if(var->type[found] != INT) {
                    printf("Error %d:%d This variable is integer and you havent assinged an integer\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                var->intvalue[found] = atoi(ptr->current_token.value);
                ptr->current_token = get_next_token(&ptr->lexer);
                if(ptr->current_token.type != TOKEN_SEMICOLON) {
                    printf("Error %d:%d -> Forgot the semicolon\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                return 0;
            }

            return -1;
        }
    }

    if(ptr->current_token.type == TOKEN_PRINT) {
        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_STRING && ptr->current_token.type != TOKEN_VARIABLE) {
            printf("Error %d:%d -> Invalid value to print %s\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        if(ptr->current_token.type == TOKEN_STRING) {
            char value[200];
            strcpy(value,ptr->current_token.value);

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> Forgot the semicolon\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            printf("%s",value);
            return 0;
        }

        if(ptr->current_token.type == TOKEN_VARIABLE) {
            char value[200];
            strcpy(value,ptr->current_token.value);

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> Forgot the semicolon\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            if(print_variable(var,value,PRINT)==0) return 0;
            else return -1;
        }

        return -1;
    }

    if(ptr->current_token.type == TOKEN_PRINTLN) {
        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_STRING && ptr->current_token.type != TOKEN_VARIABLE) {
            printf("Error %d:%d -> Invalid value to print %s\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        if(ptr->current_token.type == TOKEN_STRING) {
            char string[200];
            strcpy(string,ptr->current_token.value);

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> Forgot the semicolon\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            printf("%s\n",string);
            return 0;
        }

        if(ptr->current_token.type == TOKEN_VARIABLE) {
            char value[200];
            strcpy(value,ptr->current_token.value);

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> Forgot the semicolon\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            if(print_variable(var,value,PRINTLN)==0) return 0;
            else return -1;
        }

        return -1;
    }

    if(ptr->current_token.type == TOKEN_SET) {
        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_VARIABLE) {
            printf("Error %d:%d -> Invalid variable name '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        char varname[200];
        strcpy(varname,ptr->current_token.value);

        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_EQUAL) {
            printf("Error %d:%d -> Forgot the assigment with '='\n",ptr->current_token.line,ptr->current_token.column);
            return -1;
        }

        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_STRING && ptr->current_token.type != TOKEN_NUMBER) {
            printf("Error %d:%d -> Invalid variable value '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        if(ptr->current_token.type == TOKEN_STRING) {
            add_variable(var,varname,ptr->current_token.value,STRING);
            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> Forgot the semicolon\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            return 0;
        }

        if(ptr->current_token.type == TOKEN_NUMBER) {
            int number = atoi(ptr->current_token.value);
            add_variable(var,varname,&number,INT);

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> Forgot the semicolon\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            return 0;
        }

        return -1;
    }
    
    if(ptr->current_token.type == TOKEN_WAIT) {
        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_NUMBER && ptr->current_token.type != TOKEN_VARIABLE) {
            printf("Error %d:%d -> Timer accepts only int variables or numebrs not strings\n",ptr->current_token.line,ptr->current_token.column);
            return -1;
        }

        if(ptr->current_token.type == TOKEN_VARIABLE) {
            int found = -1;
            for(int i=0; i<var->counter; i++) {
                if(strcmp(var->variablename[i],ptr->current_token.value)==0) {
                    found = i;
                    if(var->type[i] != INT) {
                        printf("Error %d:%d -> Timer accepts only integer variables\n",ptr->current_token.line,ptr->current_token.column);
                        return -1;
                    }
                }
            }

            if(found == -1) {
                printf("Error %d:%d -> Variable %s not found\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> Forgot the semicolon\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            sleep(var->intvalue[found]);
            return 0;
        } else {
            char value[200];
            strcpy(value,ptr->current_token.value);

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> Forgot the semicolon\n",ptr->current_token.line,ptr->current_token.column);
                return 0;
            }

            int number = atoi(value);
            wait_seconds(number);

            return 0;
        }

        return -1;
    }

    if(ptr->current_token.type == TOKEN_IF) {
        int line = ptr->current_token.line;
        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_NUMBER && ptr->current_token.type != TOKEN_VARIABLE) {
            printf("Error %d:%d -> Invalid value for comparation '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        if(ptr->current_token.type == TOKEN_NUMBER) {
            int number1 = atoi(ptr->current_token.value);
            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type !=
                    TOKEN_LESS &&
                    ptr->current_token.type != TOKEN_GREATER
                    && ptr->current_token.type != TOKEN_EQUAL_EQUAL
                    && ptr->current_token.type != TOKEN_GREATER_EQUAL 
                    && ptr->current_token.type != TOKEN_LESS_EQUAL) {
                printf("Error %d:%d -> Invalid compare symbol '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }

            char symbol[3];
            strcpy(symbol,ptr->current_token.value);

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_NUMBER && ptr->current_token.type != TOKEN_VARIABLE) {
                printf("Error %d:%d -> 2nd value is invalid for comparation '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }

            if(ptr->current_token.type == TOKEN_NUMBER) {
                int number2 = atoi(ptr->current_token.value);
                ptr->current_token = get_next_token(&ptr->lexer);
                if(ptr->current_token.type != TOKEN_COLON) {
                    printf("Error %d:%d -> Forgot the colon ':'\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                char block[1024];
                int counter = 0;

                int found = -1;
                while(ptr->lexer.source[ptr->lexer.pos] != '\0') {
                    if(strncmp(&ptr->lexer.source[ptr->lexer.pos],"ENDIF", 5)==0) {
                        found = 1;
                        break;
                    }

                    block[counter++] = ptr->lexer.source[ptr->lexer.pos++];
                }
            


                block[counter] = '\0';
                if(found == -1) {
                    printf("Error %d Forgot to close the if with ENDIF\n",line);
                    return -1;
                } 

                Parser parser = {0};
                parser.lexer = lexer_init(block);

                ptr->current_token = get_next_token(&ptr->lexer);

                if(strcmp(symbol,"<")==0) {
                    if(number1 < number2) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,">")==0) {
                    if(number1 > number2) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,"==")==0) {
                    if(number1 == number2) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,">=")==0) {
                    if(number1 >= number2) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,"<=")==0) {
                    if(number1 <= number2) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }

                    }

                    return 0;
                }
            } else if(ptr->current_token.type == TOKEN_VARIABLE) {
                int found = -1;
                for(int i=0; i<var->counter; i++) {
                    if(strcmp(var->variablename[i],ptr->current_token.value)==0) {
                        found = i;
                        break;
                    }
                }

                if(found == -1) {
                    printf("Error %d:%d -> This variable does not exists '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                    return -1;
                }

                if(var->type[found] != INT) {
                    printf("Error %d:%d -> This variable is not an integer '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                    return -1;
                }

                ptr->current_token = get_next_token(&ptr->lexer);
                if(ptr->current_token.type != TOKEN_COLON) {
                    printf("Error %d:%d -> Forgot to put the colon ':'\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                char block[1024];
                int counter = 0;

                int foundIdx = -1;
                while(ptr->lexer.source[ptr->lexer.pos] != '\0') {
                    if(strncmp(&ptr->lexer.source[ptr->lexer.pos],"ENDIF", 5)==0) {
                        foundIdx = 1;
                        break;
                    }

                    block[counter++] = ptr->lexer.source[ptr->lexer.pos++];
                }
            


                block[counter] = '\0';
                if(found == -1) {
                    printf("Error %d Forgot to close the if with ENDIF\n",line);
                    return -1;
                } 

                Parser parser = {0};
                parser.lexer = lexer_init(block);

                ptr->current_token = get_next_token(&ptr->lexer);

                if(strcmp(symbol,"<")==0) {
                    if(number1 < var->intvalue[foundIdx]) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,">")==0) {
                    if(number1 > var->intvalue[foundIdx]) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,"==")==0) {
                    if(number1 == var->intvalue[foundIdx]) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,">=")==0) {
                    if(number1 >= var->intvalue[foundIdx]) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,"<=")==0) {
                    if(number1 <= var->intvalue[foundIdx]) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }

                    }

                    return 0;
                }
            }
        } else if(ptr->current_token.type == TOKEN_VARIABLE) {
            int found = -1;
            for(int i=0; i<var->counter; i++) {
                if(strcmp(var->variablename[i],ptr->current_token.value)==0) {
                    found = i;
                    break;
                }
            }

            if(found == -1) {
                printf("Error %d:%d -> Variable '%s' does not exists\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }
            
            if(var->type[found] != INT) {
                printf("Error %d:%d -> Variable '%s' is not an integer\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }

            int number1 = var->intvalue[found];
            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type !=
                    TOKEN_EQUAL_EQUAL
                    && ptr->current_token.type != TOKEN_LESS
                    && ptr->current_token.type != TOKEN_LESS_EQUAL
                    && ptr->current_token.type != TOKEN_GREATER
                    && ptr->current_token.type != TOKEN_GREATER_EQUAL) {
                printf("Error %d:%d -> Invalid comparation symbol '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }

            char symbol[3];
            strcpy(symbol,ptr->current_token.value);

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_NUMBER && ptr->current_token.type != TOKEN_VARIABLE) {
                printf("Error %d:%d -> Invalid comparation value '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }

            if(ptr->current_token.type == TOKEN_NUMBER) {
                int number2 = atoi(ptr->current_token.value);
                ptr->current_token = get_next_token(&ptr->lexer);
                if(ptr->current_token.type != TOKEN_COLON) {
                    printf("Error %d:%d -> Forgot to put the colon ':'\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                char block[1024];
                int counter = 0;

                int foundIdx = -1;
                while(ptr->lexer.source[ptr->lexer.pos] != '\0') {
                    if(strncmp(&ptr->lexer.source[ptr->lexer.pos],"ENDIF", 5)==0) {
                        foundIdx = 1;
                        break;
                    }

                    block[counter++] = ptr->lexer.source[ptr->lexer.pos++];
                }
            

                block[counter] = '\0';
                if(found == -1) {
                    printf("Error %d Forgot to close the if with ENDIF\n",line);
                    return -1;
                } 

                Parser parser = {0};
                parser.lexer = lexer_init(block);

                ptr->current_token = get_next_token(&ptr->lexer);

                if(strcmp(symbol,"<")==0) {
                    if(number1 < number2) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,">")==0) {
                    if(number1 > var->intvalue[foundIdx]) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,"==")==0) {
                    if(number1 == number2) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,">=")==0) {
                    if(number1 >= number2) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,"<=")==0) {
                    if(number1 <= number2) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
    
                    }

                    return 0;
                }
            } else if(ptr->current_token.type == TOKEN_VARIABLE) {
                int found = -1;
                for(int i=0; i<var->counter; i++) {
                    if(strcmp(var->variablename[i],ptr->current_token.value)==0) {
                        found = i;
                        break;
                    }
                }

                if(found == -1) {
                    printf("Error %d:%d -> Variable '%s' does not exists\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                    return -1;
                }

                if(var->type[found] != INT) {
                    printf("Error %d:%d -> Variable '%s' is not an integer\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                    return -1;
                }

                int number2 = var->intvalue[found];
                ptr->current_token = get_next_token(&ptr->lexer);
                if(ptr->current_token.type != TOKEN_COLON) {
                    printf("Error %d:%d -> Forgot to put the colon ':'\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                char block[1024];
                int counter = 0;

                int foundIdx = -1;
                while(ptr->lexer.source[ptr->lexer.pos] != '\0') {
                    if(strncmp(&ptr->lexer.source[ptr->lexer.pos],"ENDIF", 5)==0) {
                        foundIdx = 1;
                        break;
                    }

                    block[counter++] = ptr->lexer.source[ptr->lexer.pos++];
                }
            

                block[counter] = '\0';
                if(found == -1) {
                    printf("Error %d Forgot to close the if with ENDIF\n",line);
                    return -1;
                } 

                Parser parser = {0};
                parser.lexer = lexer_init(block);

                ptr->current_token = get_next_token(&ptr->lexer);

                if(strcmp(symbol,"<")==0) {
                    if(number1 < number2) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,">")==0) {
                    if(number1 > var->intvalue[foundIdx]) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,"==")==0) {
                    if(number1 == number2) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,">=")==0) {
                    if(number1 >= number2) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
                    }

                    return 0;
                } else if(strcmp(symbol,"<=")==0) {
                    if(number1 <= number2) {
                        while(parser.current_token.type != TOKEN_EOF) {
                            if(parse_code(&parser,var)==-1) {
                                return -1;
                            }
                        }
    
                    }

                    return 0;
                }
            }
        } 
    }

    if(ptr->current_token.type == TOKEN_FOR) {
        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_VARIABLE && ptr->current_token.type != TOKEN_NUMBER) {
            printf("Error %d:%d -> Invalid counter '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        int line = ptr->current_token.line;
        int stating_point;
        if(ptr->current_token.type == TOKEN_VARIABLE) {
            int found = -1;
            for(int i=0; i<var->counter; i++) {
                if(strcmp(var->variablename[i],ptr->current_token.value)==0) {
                    found = i;
                    stating_point = var->intvalue[i];
                    break;
                }
            }

            if(found == -1) {
                printf("Error %d:%d -> Variable '%s' not found\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }
        } else if(ptr->current_token.type == TOKEN_NUMBER) {
            stating_point = atoi(ptr->current_token.value);
        }

        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_TO) {
            printf("Error %d:%d -> Forgot to put 'TO'\n",ptr->current_token.line,ptr->current_token.column);
            return -1;
        }

        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_VARIABLE && ptr->current_token.type != TOKEN_NUMBER) {
            printf("Error %d:%d -> Invalid counter '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        int ending_point;
        if(ptr->current_token.type == TOKEN_VARIABLE) {
            int found = -1;
            for(int i=0; i<var->counter; i++) {
                if(strcmp(var->variablename[i],ptr->current_token.value)==0) {
                    found = i;
                    ending_point = var->intvalue[i];
                    break;
                }
            }

            if(found == -1) {
                printf("Error %d:%d -> Variable '%s' not found\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }
        } else if(ptr->current_token.type == TOKEN_NUMBER) {
            ending_point = atoi(ptr->current_token.value);
        }

        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_COLON) {
            printf("Error %d:%d -> Forgot to put the colon ':'\n",ptr->current_token.line,ptr->current_token.column);
            return -1;
        }

        char block[1024];
        int counter = 0;

        int found = -1;
        while(ptr->lexer.source[ptr->lexer.pos] != '\0') {
            if(strncmp(&ptr->lexer.source[ptr->lexer.pos],"ENDFOR", 6)==0) {
                found = 1;
                break;
            }

            block[counter++] = ptr->lexer.source[ptr->lexer.pos++];
        }


        block[counter] = '\0';
        if(found == -1) {
            printf("Error %d Forgot to close the for loop with ENDFOR\n",line);
            return -1;
        }

        Parser parser = {0};
        parser.lexer = lexer_init(block);

        ptr->current_token = get_next_token(&ptr->lexer);

        if(stating_point < ending_point) {
            for(int i=stating_point; i<=ending_point; i++) {
                parser.lexer.pos = 0;
                parser.current_token.type = TOKEN_ERROR;

                while(parser.current_token.type != TOKEN_EOF) {
                    if(parse_code(&parser,var)==-1) {
                        return -1;
                    }
                }
            }
        } else if(stating_point > ending_point) {
            for(int i=stating_point; i>=ending_point; i--) {
                parser.lexer.pos = 0;
                parser.current_token.type = TOKEN_ERROR;

                while(parser.current_token.type != TOKEN_EOF) {
                    if(parse_code(&parser,var)==-1) {
                        return -1;
                    }
                }
            }
        }

        return 0;
    }

    printf("Error %d:%d -> Invalid start %s\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
    return -1;
 }
