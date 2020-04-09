#include <stdlib.h>

#include "symbol.h"
#include "type.h"

Symbol* symbol_create(Symbol_t kind, Type* type, char* name) {
    Symbol* s = malloc(sizeof(*s));
    s->kind = kind;
    s->type = type;
    s->name = name;
    return s;
}

void symbol_delete(Symbol* s) {
    if (!s) return;

    type_delete(s->type);
    free(s->name);

    free(s);
}
