#include <stdlib.h>

#include "symbol.h"
#include "type.h"

struct symbol* symbol_create(symbol_t kind, struct type* type, char* name) {
    struct symbol* s = malloc(sizeof(*s));
    s->kind = kind;
    s->type = type;
    s->name = name;
    return s;
}

void symbol_delete(struct symbol* s) {
    if (!s) return;

    type_delete(s->type);
    free(s->name);

    free(s);
}
