#include <stdlib.h>

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
