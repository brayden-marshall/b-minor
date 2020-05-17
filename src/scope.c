#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "type.h"

#include "scope.h"
#include "symbol.h"
#include "hash_table.h"

static int scope_stack_top = 0;

extern struct hash_table* scope_stack[SCOPE_STACK_MAX];

// stores the current number of variables declared in each scope
// this is used to set the 'which' property on symbols for code generation
int scope_stack_var_counts[SCOPE_STACK_MAX];

void scope_enter() {
    //printf("entering scope...\n");
    scope_stack_top++;
    if (scope_stack_top >= SCOPE_STACK_MAX) {
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
    scope_stack_var_counts[scope_stack_top] = 0;
}

void scope_exit() {
    //printf("exiting scope...\n");
    if (scope_stack_top == 0) {
        printf("**(Compiler Bug)**: Attempt to delete global scope. Exiting.\n");
        exit(1);
    }

    hash_table_clear(scope_stack[scope_stack_top]);
    scope_stack_top--;
}

int scope_level() {
    return scope_stack_top+1;
}

void scope_bind(const char* name, Symbol* sym) {
    assert(scope_stack_top >= 0 && scope_stack_top < SCOPE_STACK_MAX);

    sym->which = scope_stack_var_counts[scope_stack_top];
    scope_stack_var_counts[scope_stack_top]++;

    hash_table_insert(scope_stack[scope_stack_top], name, sym);
}

Symbol* scope_lookup(const char* name) {
    int current_level = scope_stack_top;
    Symbol* s = NULL;
    while (s == NULL && current_level >= 0) {
        s = hash_table_lookup(scope_stack[current_level], name);
        current_level--;
    }

    return s;
}

Symbol* scope_lookup_current(const char* name) {
    assert(scope_stack_top < SCOPE_STACK_MAX);
    assert(scope_stack[scope_stack_top]);

    return hash_table_lookup(scope_stack[scope_stack_top], name);
}

void decl_resolve(Decl* d) {
    if (!d) return;

    Symbol_t kind = scope_level() > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;

    if (scope_lookup_current(d->name) != NULL) {
        printf("Error: Variable '%s' was redeclared.\n", d->name);
        scope_error = 1;
    }
    d->symbol = symbol_create(kind, d->type, d->name);

    expr_resolve(d->value);
    scope_bind(d->name, d->symbol);

    if (d->code) {
        scope_enter();
        param_list_resolve(d->type->params);
        stmt_resolve(d->code->body);
        scope_exit();
    }

    decl_resolve(d->next);
}

void expr_resolve(Expr* e) {
    if (!e) return;

    if (e->kind == EXPR_NAME) {
        e->symbol = scope_lookup(e->name);
        if (e->symbol == NULL) {
            printf("Error: Identifier '%s' used before it was declared.\n", e->name);
            scope_error = 1;
        }
    } else {
        expr_resolve(e->left);
        expr_resolve(e->right);
    }
}

void stmt_resolve(Stmt* s) {
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

void param_list_resolve(ParamList* p) {
    if (!p) return;

    p->symbol = symbol_create(SYMBOL_PARAM, p->type, p->name);

    scope_bind(p->name, p->symbol);

    param_list_resolve(p->next);
}
