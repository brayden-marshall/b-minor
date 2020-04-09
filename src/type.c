#include <stdlib.h>
#include <stdio.h>

#include "type.h"
#include "expr.h"
#include "param_list.h"

Type* type_create(Type_t kind) {
    Type* t = malloc(sizeof(*t));
    t->kind = kind;
    t->subtype = 0;
    t->params = 0;
    t->size_expr = 0;
    return t;
}

void type_delete(Type* t) {
    if (!t) return;

    type_delete(t->subtype);
    param_list_delete(t->params);
    free(t);
}

Type* type_create_array(Type* subtype, Expr* size_expr) {
    Type* t = type_create(TYPE_ARRAY);
    t->subtype = subtype;
    t->size_expr = size_expr;
    return t;
}

Type* type_create_function(Type* return_type, ParamList* params) {
    Type* t = type_create(TYPE_FUNCTION);
    t->subtype = return_type;
    t->params = params;
    return t;
}

void type_print(Type* t) {
    if (!t) return;

    switch (t->kind) {
        case TYPE_VOID:
            printf("void");
            break;
        case TYPE_BOOLEAN:
            printf("boolean");
            break;
        case TYPE_CHAR:
            printf("char");
            break;
        case TYPE_INTEGER:
            printf("integer");
            break;
        case TYPE_STRING:
            printf("string");
            break;
        case TYPE_ARRAY:
            printf("array [");
            expr_print(t->size_expr);
            printf("] ");
            type_print(t->subtype);
            break;
        case TYPE_FUNCTION:
            printf("function ");
            type_print(t->subtype);
            printf(" (");
            param_list_print(t->params);
            printf(")");
            break;
        default:
            break;
    }
}
