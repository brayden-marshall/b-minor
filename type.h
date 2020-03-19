#ifndef TYPE_H
#define TYPE_H

typedef enum {
    TYPE_VOID = 0,
    TYPE_BOOLEAN,
    TYPE_CHAR,
    TYPE_INTEGER,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_FUNCTION,
} type_t;

struct type {
    type_t kind;
    struct type* subtype;
    struct param_list* params;
};

struct type* type_create(type_t kind);

struct type* type_create_array(struct type* subtype);

struct type* type_create_function(struct type* return_type, struct param_list* params);

void type_print(struct type* t);
    
#endif
