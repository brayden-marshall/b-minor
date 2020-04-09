#ifndef TYPE_H
#define TYPE_H

#include "expr.h"
#include "param_list.h"

typedef enum {
    TYPE_VOID = 0,
    TYPE_BOOLEAN,
    TYPE_CHAR,
    TYPE_INTEGER,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_FUNCTION,
} Type_t;

typedef struct Type Type;

struct Type {
    Type_t kind;
    Type* subtype;
    ParamList* params;
    Expr* size_expr;
};

Type* type_create(Type_t kind);

void type_delete(Type* t);

Type* type_create_array(Type* subtype, Expr* size_expr);

Type* type_create_function(Type* return_type, ParamList* params);

void type_print(Type* t);
    
#endif
