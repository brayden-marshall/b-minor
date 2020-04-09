#ifndef DECL_H
#define DECL_H

#include "type.h"
#include "expr.h"
#include "stmt.h"

typedef struct Decl {
    char* name;
    Type* type;
    Expr* value;
    Stmt* code;
    Decl* next;
    Symbol* symbol;
} Decl;

Decl* decl_create(
    char* name,
    Type* type,
    Expr* value,
    Stmt* code,
    Decl* next
);

void decl_delete(Decl* d);

void decl_print(Decl* d);

#endif
