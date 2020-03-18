#include <stdlib.h>
#include <stdio.h>

#include "type.h"

struct type* type_create(type_t kind) {
    struct type* t = malloc(sizeof(*t));
    t->kind = kind;
    t->subtype = 0;
    t->params = 0;
    return t;
}

struct type* type_create_array(struct type* subtype) {
    struct type* t = type_create(TYPE_ARRAY);
    t->subtype = subtype;
    return t;
}

void type_print(struct type* t) {
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
            printf("array [] ");
            type_print(t->subtype);
            break;
        default:
            break;
    }
}
