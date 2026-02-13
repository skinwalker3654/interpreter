#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
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
        printf("Error: Η μεταβλητη %s δεν υπαρχει καπου στον κοδικα.\n",varname);
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

    if(ptr->current_token.type == TOKEN_CLEAR) {
        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_SEMICOLON) {
            printf("Error %d:%d -> Ξεχασες να βαλεις το ';' στο τελος της εντολης ka8arise\n",ptr->current_token.line,ptr->current_token.column);
            return -1;
        }

        system("clear ");
        return 0;
    }

    if(ptr->current_token.type == TOKEN_REMOVE) {
        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_VARIABLE && ptr->current_token.type != TOKEN_STRING) {
            printf("Error %d:%d -> δεν μπορεις να σβησεις τον φακελο/αρχειο με ονομα '%s'. Πρεπει το ονομα να ειναι μεσα σε \" \" ή να βαλεις καποια μεταβλητη η οποια εχει ονομα μεσα ΟΧΙ αριθμο\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        if(ptr->current_token.type == TOKEN_VARIABLE) {
            int found = -1;
            for(int i=0; i<var->counter; i++) {
                if(strcmp(var->variablename[i],ptr->current_token.value)==0) {
                    found = i;
                    break;
                }
            }

            if(found == -1) {
                printf("Error %d:%d -> Η μεταβλητη που εβαλες στο sbyse -> '%s, δεν υπαρχει καπου στον κοδικα\n'",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }

            if(var->type[found] == INT) {
                printf("Error %d:%d -> Αυτη η μεταβλητη -> '%s' εχει τιμη αριθμου μεσα της και δεν μπορεις να σβυσεις φακελους/αρχεια που εχουν αριθμο για τιμι, πρεπει να φτιαξεις μια μεταβλητη οπου θα περιεχει το ονομα του φακελου/αρχειου μεσα σε \" \"\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> ξεχασες να βαλεις ενα ';' στο τελος της sbyse\n'",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            int check = remove(var->stringvalue[found]);
            if(check == -1) {
                printf("Error: δεν μπορεσε να σβυστει ο φακελος/αρχειο με ονομα '%s', σηγουρεψου οτι υπαρχει φακελος/αρχειο με το ιδιο ονομα στο συστημα σου.\n",var->stringvalue[found]);
                return -1;
            }

            return 0;
        }

        if(ptr->current_token.type == TOKEN_STRING) {
            char stringvalue[200];
            strcpy(stringvalue,ptr->current_token.value);

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> ξεαχασες να βαλεις ενα ';' στο τελος της εντολης sbyse\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            int check = remove(stringvalue);
            if(check == -1) {
                printf("Error: δεν μπορεσε να σβυστει ο φακελος/αρχειο με ονομα '%s', σηγουρεψου οτι υπαρχει φακελος/αρχειο με το ιδιο ονομα στο συστημα σου.\n",stringvalue);
                return -1;
            }

            return 0;
        }

        return -1;
    }


    if(ptr->current_token.type == TOKEN_MKDIR) {
        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_VARIABLE && ptr->current_token.type != TOKEN_STRING) {
            printf("Error %d:%d -> δεν μπορεις να φτιαξεις φακελο με ονομα '%s'. Πρεπει το ονομα να εινια μεσα σε \" \" ή να βαλεις καποια μεταβλητη η οποια εχει ονομα μεσα ΟΧΙ αριθμο\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        if(ptr->current_token.type == TOKEN_VARIABLE) {
            int found = -1;
            for(int i=0; i<var->counter; i++) {
                if(strcmp(var->variablename[i],ptr->current_token.value)==0) {
                    found = i;
                    break;
                }
            }

            if(found == -1) {
                printf("Error %d:%d -> Η μεταβλητη που εβαλες στο neosfakelos -> '%s, δεν υπαρχει καπου στον κοδικα\n'",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }

            if(var->type[found] == INT) {
                printf("Error %d:%d -> Αυτη η μεταβλητη -> '%s' εχει τιμη αριθμου μεσα της και δεν μπορεις να φτιαξεις φακελος που εχουν αριθμο για τιμι, πρεπει να φτιαξεις μια μεταβλητη οπου θα περιεχει το ονομα του φακελου μεσα σε \" \"\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> ξεχασες να βαλεις ενα ';' στο τελος της εντολης neosfakelos\n'",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            int check = mkdir(var->stringvalue[found],0777);
            if(check == -1) {
                printf("Error: δεν μπορεσε να φτιαχτει ο φακελος με ονομα '%s', σηγουρεψου οτι δεν υπαρχει φακελος με το ιδιο ονομα στο συστημα σου.\n",var->stringvalue[found]);
                return -1;
            }

            return 0;
        }

        if(ptr->current_token.type == TOKEN_STRING) {
            char stringvalue[200];
            strcpy(stringvalue,ptr->current_token.value);

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> ξεαχασες να βαλεις ενα ';' στο τελος της εντολης neosfakelos\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            int check = mkdir(stringvalue,0777);
            if(check == -1) {
                printf("Error: δεν μποροσε να φτιαχτει φακελος με ονομα '%s', σηγουρεψου οτι δεν υπαρχει φακελος με το ιδιο ονομα στο συστημα σου.\n",stringvalue);
                return -1;
            }

            return 0;
        }

        return -1;
    }

    if(ptr->current_token.type == TOKEN_VARIABLE) {
        int found = -1;
        for(int i=0; i<var->counter; i++) {
            if(strcmp(var->variablename[i],ptr->current_token.value)==0) {
                found = i;
                break;
            }
        }

        if(found == -1) {
            printf("Error %d:%d -> H μεταβλητη '%s' δεν υπαρχει καπου στον κοδικα\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_EQUAL && ptr->current_token.type != TOKEN_INCREASE && ptr->current_token.type != TOKEN_DECREASE && ptr->current_token.type != TOKEN_PLUS_EQUAL && ptr->current_token.type != TOKEN_MINUS_EQUAL) {
            printf("Error %d:%d -> Δεν υπαρχει τετοια λιτυοργια για αλαγη τιμον μιας μεταβλητης -> %s' μπορεις μονο να κανεις ++, -- , = , += ή -=\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        if(ptr->current_token.type == TOKEN_PLUS_EQUAL) {
            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_NUMBER && ptr->current_token.type != TOKEN_VARIABLE) {
                printf("Error %d:%d -> Δεν μπορεις να αυξασεις την μεταβλητη με '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }

            if(ptr->current_token.type == TOKEN_NUMBER) {
                char number[256];
                strcpy(number,ptr->current_token.value);

                ptr->current_token = get_next_token(&ptr->lexer);
                if(ptr->current_token.type != TOKEN_SEMICOLON) {
                    printf("Error %d:%d -> ξεχασες να βαλεις ενα ';' στο τελος της αυξησεις αυτης της μεταβλητης\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                var->intvalue[found] += atoi(number);
                return 0;
            } else if(ptr->current_token.type == TOKEN_VARIABLE) {
                int foundVar = -1;
                for(int i=0; i<var->counter; i++) {
                    if(strcmp(var->variablename[i],ptr->current_token.value)==0) {
                        foundVar = i;
                        break;
                    }
                }

                if(foundVar == -1) {
                    printf("Error %d:%d -> η μεταβλητη αυτη '%s', δεν υπαρχει καπου στον κοδικα ωστε να μπορουμε να την χρισιμοποιησουμε για να αυξησουμε την προηγουμενη\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                    return -1;
                }

                if(var->type[found] != INT) {
                    printf("Error %d:%d -> η μεταβλητη '%s', δεν εχει αριθμο για τιμι οποτε η αυξηση δεν ειναι εγκηρη\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                    return -1;
                }

                ptr->current_token = get_next_token(&ptr->lexer);
                if(ptr->current_token.type != TOKEN_SEMICOLON) {
                    printf("Error %d:%d -> ξεχασες να βαλεις ενα ';' στο τελος της αυξησεις αυτης της μεταβλητης\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                var->intvalue[found] += var->intvalue[foundVar];
                return 0;
            }

            return -1;
        }

        if(ptr->current_token.type == TOKEN_MINUS_EQUAL) {
            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_VARIABLE && ptr->current_token.type != TOKEN_NUMBER) {
                printf("Error %d:%d -> Δεν μπορεις να μιοσεις την μεταβλητη με '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }

            if(ptr->current_token.type == TOKEN_NUMBER) {
                char number[256];
                strcpy(number,ptr->current_token.value);

                ptr->current_token = get_next_token(&ptr->lexer);
                if(ptr->current_token.type != TOKEN_SEMICOLON) {
                    printf("Error %d:%d -> ξεχασες να βαλεις ενα ';' στο τελος της αυξησεις αυτης της μεταβλητης\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                var->intvalue[found] -= atoi(number);
                return 0;
            } else if(ptr->current_token.type == TOKEN_VARIABLE) {
                int foundVar = -1;
                for(int i=0; i<var->counter; i++) {
                    if(strcmp(var->variablename[i],ptr->current_token.value)==0) {
                        foundVar = i;
                        break;
                    }
                }

                if(foundVar == -1) {

                    printf("Error %d:%d -> η μεταβλητη αυτη '%s', δεν υπαρχει καπου στον κοδικα ωστε να μπορουμε να την χρισιμοποιησουμε για να μιοσουμε την προηγουμενη\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                    return -1;
                }

                if(var->type[found] != INT) {
                    printf("Error %d:%d -> η μεταβλητη '%s', δεν εχει αριθμο για τιμι οποτε η μιοσει δεν ειναι εγκηρη\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                    return -1;
                }

                ptr->current_token = get_next_token(&ptr->lexer);
                if(ptr->current_token.type != TOKEN_SEMICOLON) {
                    printf("Error %d:%d -> ξεχασες να βαλεις ενα ';' στο τελος της αυξησεις αυτης της μεταβλητης\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                var->intvalue[found] -= var->intvalue[foundVar];
                return 0;
            }
            
            return -1;
        }

        if(ptr->current_token.type == TOKEN_INCREASE) {
            if(var->type[found] != INT) {
                printf("Error %d:%d Η μεταβλητη αυτη εχει μηνημα για τιμι και δεν γινεται να αυξασεις την τιμι ενος μηνηματος. Πρεπει να εχει αριθμο η μεταβλητη αυτη για να δουλεψει η λιτουργια ++\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }


            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> ξεχασες να βαλεις το ';' στο τελος της αυξησεις μιας μεταβλητης\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            var->intvalue[found]++;
            return 0;
        }

        if(ptr->current_token.type == TOKEN_DECREASE) {
            if(var->type[found] != INT) {
                printf("Error %d:%d δεν μπορεις να μιωσεις την τιμι αυτης της μεταβλητης επιδη περιεχει μηνημα μεσα και δεν εινια δυνατον να μιοσεις ενα μηνημα. Πρεπει να βαλεις μεταβλητη που περιεχει αριθμο ος τιμι για να δουλεψει η λιτουργει ++\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> ξεχασες να βαλεις ενα ';' στο τελος της λιτυουργιας --\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            var->intvalue[found]--;
            return 0;
        }

        if(ptr->current_token.type == TOKEN_EQUAL) {
            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_STRING && ptr->current_token.type != TOKEN_NUMBER) {
                printf("Error %d:%d -> Δεν γινεται να βαλεις αυτο '%s' ος τιμη μια μεταβλητης. Μπορεις να βαλεις μονο αριθμο ή μηνημα μεσα σε \" \"\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }
            
            if(ptr->current_token.type == TOKEN_STRING) {
                if(var->type[found] != STRING) {
                    printf("Error %d:%d -> αυτη η μεταβλητη εχει μηνημα ος τιμι και εσυ δεν περασες μηνημα οσ νεα τιμι. Πρεπει η νεα τιμι να ειναι μεσα σε \"\"\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                strcpy(var->stringvalue[found],ptr->current_token.value);
                ptr->current_token = get_next_token(&ptr->lexer);
                if(ptr->current_token.type != TOKEN_SEMICOLON) {
                    printf("Error %d:%d -> ξεχασες να βαλεις το ';' στο τελος οπου εβαζες μια νεα τιμι στην μεταβλητη\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                return 0;
            } else if(ptr->current_token.type == TOKEN_NUMBER) {
                if(var->type[found] != INT) {
                    printf("Error %d:%d -> αυτη η μεταβλητη ειχε αριθμο ος τιμι οποτε η νεα τιμι πρεπει να εινια και αυτη αριθμος\n",ptr->current_token.line,ptr->current_token.column);
                    return -1;
                }

                var->intvalue[found] = atoi(ptr->current_token.value);
                ptr->current_token = get_next_token(&ptr->lexer);
                if(ptr->current_token.type != TOKEN_SEMICOLON) {
                    printf("Error %d:%d -> ξεχασες να βαλεις ';' στο τελος οπου εβαζες μια νεα τιμι στην μεταβλητη\n",ptr->current_token.line,ptr->current_token.column);
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
            printf("Error %d:%d -> δεν γινεται να κανεις print το %s. Πρεπει να βαλεις μια υπαρχοντει μεταλβητη ή ενα μηνημα μεσα σε \" \"\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        if(ptr->current_token.type == TOKEN_STRING) {
            char value[200];
            strcpy(value,ptr->current_token.value);

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> ξεχασες να βαλεις το ';' στο τελος της print\n",ptr->current_token.line,ptr->current_token.column);
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
                printf("Error %d:%d -> ξεχασες να βαλεις το ';' στο τελος της print\n",ptr->current_token.line,ptr->current_token.column);
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
            printf("Error %d:%d -> δεν γινεται να κανεις print το %s. Πρεπει να βαλεις μια υπαρχοντει μεταλβητη ή ενα μηνημα μεσα σε \" \"\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        if(ptr->current_token.type == TOKEN_STRING) {
            char string[200];
            strcpy(string,ptr->current_token.value);

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> ξεχασες να βαλεις το ';' στο τελος της printent\n",ptr->current_token.line,ptr->current_token.column);
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
                printf("Error %d:%d -> ξεχασες να βαλεις το ';' στο τελος της printent\n",ptr->current_token.line,ptr->current_token.column);
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
            printf("Error %d:%d -> δεν γινεται να ονομασεις μια μεταβλητη ετσι -> '%s' πρεπει παντα το προτο γραμα στο ονομα της μεταλητης να ειναι γραμμα ΟΧΙ αριθμος ΟΥΤΕ συμβολο\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        char varname[200];
        strcpy(varname,ptr->current_token.value);

        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_EQUAL) {
            printf("Error %d:%d -> ξεχασες να βαλεις '=' στην διμιουργια της νεας μεταβλητης\n",ptr->current_token.line,ptr->current_token.column);
            return -1;
        }

        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_STRING && ptr->current_token.type != TOKEN_NUMBER) {
            printf("Error %d:%d -> Δεν γινεται να βαλεις συμβολο ή αιρθμο αναμικτο με αλους χαρακτηες ΟΣ τιμι μιας μεταβλητης -> '%s', πρεπει να βαλεις εναν αιρθμο ή ενα μηνημα με \" \"\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
            return -1;
        }

        if(ptr->current_token.type == TOKEN_STRING) {
            add_variable(var,varname,ptr->current_token.value,STRING);
            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> ξεχασες να βαλεις το ';' στο τελος της νεας μεταβλητης\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            return 0;
        }

        if(ptr->current_token.type == TOKEN_NUMBER) {
            int number = atoi(ptr->current_token.value);
            add_variable(var,varname,&number,INT);

            ptr->current_token = get_next_token(&ptr->lexer);
            if(ptr->current_token.type != TOKEN_SEMICOLON) {
                printf("Error %d:%d -> ξεχασες να βαλεις το ';' στο τελος της νεας μεταβλητης\n",ptr->current_token.line,ptr->current_token.column);
                return -1;
            }

            return 0;
        }

        return -1;
    }
    
    if(ptr->current_token.type == TOKEN_WAIT) {
        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_NUMBER && ptr->current_token.type != TOKEN_VARIABLE) {
            printf("Error %d:%d -> Η εντολη perimene δεχεται μονο αριθμους ή μεταβλητες που εχουν τιμι εναν αριθμο\n",ptr->current_token.line,ptr->current_token.column);
            return -1;
        }

        if(ptr->current_token.type == TOKEN_VARIABLE) {
            int found = -1;
            for(int i=0; i<var->counter; i++) {
                if(strcmp(var->variablename[i],ptr->current_token.value)==0) {
                    found = i;
                    if(var->type[i] != INT) {
                        printf("Error %d:%d -> αυτη η μεταβλητη στο perimene δεν εχει αριθμο για τιμι!!\n",ptr->current_token.line,ptr->current_token.column);
                        return -1;
                    }
                }
            }

            if(found == -1) {
                printf("Error %d:%d -> Στην εντολη perimene η μεταβλητη %s δεν βρεθηκε στον κοδικα\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
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
                printf("Error %d:%d -> ξεχασες να βαλεις το ';' στο τελος της νεας μεταβλητης\n",ptr->current_token.line,ptr->current_token.column);
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
                    if(strncmp(&ptr->lexer.source[ptr->lexer.pos],"telosan", 7)==0) {
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
                    if(strncmp(&ptr->lexer.source[ptr->lexer.pos],"telosan", 7)==0) {
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
                    if(strncmp(&ptr->lexer.source[ptr->lexer.pos],"telosan", 7)==0) {
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
                    if(strncmp(&ptr->lexer.source[ptr->lexer.pos],"telosan", 7)==0) {
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
            printf("Error %d:%d -> Δεν γινεται να κανεις μετρηση στο loop με -> '%s', βαλε εναν αριθμο ή καποια μεταβλητη η οπια εχει αριθμο ος τιμι\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
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
                printf("Error %d:%d -> H προτη μεταβλητη στο loop -> '%s' δεν βρεθηκε στον κοδικα\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }
        } else if(ptr->current_token.type == TOKEN_NUMBER) {
            stating_point = atoi(ptr->current_token.value);
        }

        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_TO) {
            printf("Error %d:%d -> ξεχασες να βαλεις 'eos' στο loop\n",ptr->current_token.line,ptr->current_token.column);
            return -1;
        }

        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_VARIABLE && ptr->current_token.type != TOKEN_NUMBER) {
            printf("Error %d:%d -> Δεν γινεται να κανεις μετρηση στο loop με '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
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
                printf("Error %d:%d -> H δευτερη μεταβλητη στο loop -> %s' δεν βρεθηκε στον κοδικα\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
                return -1;
            }
        } else if(ptr->current_token.type == TOKEN_NUMBER) {
            ending_point = atoi(ptr->current_token.value);
        }

        ptr->current_token = get_next_token(&ptr->lexer);
        if(ptr->current_token.type != TOKEN_COLON) {
            printf("Error %d:%d -> ξεχασες να βαλεις το ':' για να αρχισι ο κοδικας του loop\n",ptr->current_token.line,ptr->current_token.column);
            return -1;
        }

        char block[1024];
        int counter = 0;

        int found = -1;
        while(ptr->lexer.source[ptr->lexer.pos] != '\0') {
            if(strncmp(&ptr->lexer.source[ptr->lexer.pos],"telosgia", 8)==0) {
                found = 1;
                break;
            }

            block[counter++] = ptr->lexer.source[ptr->lexer.pos++];
        }


        block[counter] = '\0';
        if(found == -1) {
            printf("Error %d -> ξεχασες να κλησης το loop με telosgia\n",line);
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

    printf("Error %d:%d -> Δεν υπαρχει εντολη '%s'\n",ptr->current_token.line,ptr->current_token.column,ptr->current_token.value);
    return -1;
 }
