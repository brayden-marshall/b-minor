#ifndef SCOPE_H
#define SCOPE_H

#include "decl.h"
#include "expr.h"
#include "stmt.h"
#include "param_list.h"
#include "symbol.h"

struct hash_table* scope_stack[256];

void scope_enter();

void scope_exit();

int scope_level();

void scope_bind(const char* name, struct symbol* sym);

struct symbol* scope_lookup(const char* name);

struct symbol* scope_lookup_current(const char* name);

void decl_resolve(struct decl* d);

void expr_resolve(struct expr* e);

void stmt_resolve(struct stmt* s);

void param_list_resolve(struct param_list* p);

#endif
