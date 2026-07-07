#ifndef VALUE_H
#define VALUE_H

typedef enum {
    VAL_INT,
    VAL_STR,
    VAL_ERR,
} ValueType;

typedef struct {
    union {
        char *string_value;
        int int_value;
    };
    ValueType type;
} Value;

Value value_new_int(int number);
Value value_new_string(char *string);
void value_destroy(Value *v);

#endif
