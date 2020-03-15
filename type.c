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

struct param_list* param_list_create(
    char* name, struct type* type, struct param_list* next
) {
    struct param_list* p = malloc(sizeof(*p));
    p->name = name;
    p->type = type;
    p->next = next;
    return p;
}
