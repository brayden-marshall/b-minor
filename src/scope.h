#ifndef SCOPE_H
#define SCOPE_H

#include "decl.h"
#include "expr.h"
#include "stmt.h"
#include "param_list.h"
#include "symbol.h"

#define SCOPE_STACK_MAX 256

extern struct hash_table* scope_stack[SCOPE_STACK_MAX];
extern int scope_error;

void scope_enter();

void scope_exit();

int scope_level();

void scope_bind(const char* name, Symbol* sym);

Symbol* scope_lookup(const char* name);

Symbol* scope_lookup_current(const char* name);

void decl_resolve(Decl* d);

void expr_resolve(Expr* e);

void stmt_resolve(Stmt* s);

void param_list_resolve(ParamList* p);

#endif
