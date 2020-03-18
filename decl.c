#include <stdlib.h>
#include <stdio.h>

#include "decl.h"
#include "expr.h"
#include "type.h"

struct decl* decl_create(
    char* name,
    struct type* type,
    struct expr* value,
    struct stmt* code,
    struct decl* next
) {
    struct decl* d = malloc(sizeof(*d));
    d->name = name;
    d->type = type;
    d->value = value;
    d->code = code;
    d->next = next;
    return d;
}

void decl_print(struct decl* d) {
    if (!d) return;

    printf("%s: ", d->name);
    type_print(d->type);
    if (d->value) {
        printf(" = ");
        expr_print(d->value);
    }

    // TODO: print d->code

    printf(";\n");
    decl_print(d->next);
}
