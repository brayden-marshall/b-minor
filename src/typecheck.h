#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "type.h"
#include "expr.h"
#include "decl.h"
#include "stmt.h"

int type_equals(struct type* a, struct type* b);

struct type* type_copy(struct type* t);

void type_delete(struct type* t);

struct type* expr_typecheck(struct expr* e);

void decl_typecheck(struct decl* d);

void stmt_typecheck(struct stmt* s);

#endif
