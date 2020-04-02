#include <stdlib.h>
#include <stdio.h>

#include "decl.h"
#include "expr.h"
#include "type.h"
#include "stmt.h"

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
    d->symbol = NULL;
    return d;
}

void decl_delete(struct decl* d) {
    if (!d) return;

    free((void*)d->name);
    type_delete(d->type);
    expr_delete(d->value);
    stmt_delete(d->code);
    decl_delete(d->next);

    free(d);
}

void decl_print(struct decl* d) {
    if (!d) return;

    printf("%s: ", d->name);
    type_print(d->type);
    if (d->value) {
        printf(" = ");
        expr_print(d->value);
    }

    if (d->code) {
        printf(" = ");
        stmt_print(d->code);
        if (d->code->kind != STMT_BLOCK) {
            printf(";");
        }
    } else {
        printf(";");
    }

    printf("\n");
    decl_print(d->next);
}
