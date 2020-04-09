#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "type.h"
#include "expr.h"
#include "decl.h"
#include "stmt.h"

int type_equals(Type* a, Type* b);

Type* type_copy(Type* t);

void type_delete(Type* t);

Type* expr_typecheck(Expr* e);

void decl_typecheck(Decl* d);

void stmt_typecheck(Stmt* s);

#endif
