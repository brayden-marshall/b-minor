#include <stdio.h>
#include <stdlib.h>

#include "type.h"

#include "scope.h"
#include "symbol.h"
#include "hash_table.h"

static int scope_stack_top = 0;

void scope_enter() {
    scope_stack_top++;
    if ((unsigned int)scope_stack_top >= sizeof(scope_stack)/sizeof(scope_stack[0])) {
        printf("Maximum scope level reached.\n");
        printf("I'm so sorry for your loss.\n");
        exit(1);
    }

    // only create a new hash table if the slot in the underlying array is null
    // if it is non-null, it means there is an empty hash_table there and there is no
    // need to allocate another
    if (!scope_stack[scope_stack_top]) {
        scope_stack[scope_stack_top] = hash_table_create(0, 0);
    }
}

void scope_exit() {
    if (scope_stack_top == 0) {
        printf("Attempt to delete global scope. Exiting.\n");
        exit(1);
    }

    hash_table_clear(scope_stack[scope_stack_top]);
    scope_stack_top--;
}

int scope_level() {
    return scope_stack_top+1;
}

void scope_bind(const char* name, struct symbol* sym) {
    hash_table_insert(scope_stack[scope_stack_top], name, sym);
}

struct symbol* scope_lookup(const char* name) {
    int current_level = scope_stack_top;
    struct symbol* s = NULL;
    while (s == NULL && current_level >= 0) {
        s = hash_table_lookup(scope_stack[current_level], name);
        current_level--;
    }

    return s;
}

struct symbol* scope_lookup_current(const char* name) {
    return hash_table_lookup(scope_stack[scope_stack_top], name);
}

void decl_resolve(struct decl* d) {
    if (!d) return;

    symbol_t kind = scope_level() > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;

    if (scope_lookup_current(d->name) != NULL) {
        printf("Error: Variable '%s' was redeclared.\n", d->name);
    }
    d->symbol = symbol_create(kind, d->type, d->name);

    expr_resolve(d->value);
    scope_bind(d->name, d->symbol);

    if (d->code) {
        scope_enter();
        param_list_resolve(d->type->params);
        stmt_resolve(d->code);
        scope_exit();
    }

    decl_resolve(d->next);
}

void expr_resolve(struct expr* e) {
    if (!e) return;

    if (e->kind == EXPR_NAME) {
        e->symbol = scope_lookup(e->name);
        if (e->symbol == NULL) {
            printf("Identifier '%s' used before it was declared.\n", e->name);
        }
    } else {
        expr_resolve(e->left);
        expr_resolve(e->right);
    }
}

void stmt_resolve(struct stmt* s) {
    if (!s) return;

    if (s->kind == STMT_BLOCK) {
        scope_enter();
        stmt_resolve(s->body);
        scope_exit();
    } else {
        decl_resolve(s->decl);
        expr_resolve(s->init_expr);
        expr_resolve(s->expr);
        expr_resolve(s->next_expr);
        stmt_resolve(s->body);
        stmt_resolve(s->else_body);
    }
    stmt_resolve(s->next);
}

void param_list_resolve(struct param_list* p) {
    if (!p) return;

    symbol_t kind = SYMBOL_PARAM;

    p->symbol = symbol_create(kind, p->type, p->name);

    scope_bind(p->name, p->symbol);

    param_list_resolve(p->next);
}
