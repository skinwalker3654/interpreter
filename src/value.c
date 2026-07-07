#include "../header/value.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Value value_new_int(int number) {
    Value v = {.int_value=number,VAL_INT};
    return v;
}

Value value_new_string(char *string) {
    Value v;
    v.string_value = strdup(string);
    if(!v.string_value) {
        printf("Value error: Failed to allocate memory\n");
        v.type = VAL_ERR;
        return v;
    }

    v.type = VAL_STR;
    return v;
}

void value_destroy(Value *v) {
    switch(v->type) {
        case VAL_STR:
            free(v->string_value);
            break;
        case VAL_INT:
        case VAL_ERR:
            break;
    }
}
