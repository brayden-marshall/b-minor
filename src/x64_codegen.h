#ifndef X64_CODEGEN_H
#define X64_CODEGEN_H

#include "symbol.h"
#include "expr.h"
#include "stmt.h"
#include "decl.h"

int scratch_alloc();

void scratch_free(int r);

const char* scratch_name(int r);

int label_create();

const char* label_name(int label);

const char* symbol_codegen(Symbol* s);

void expr_codegen(Expr* e);

void stmt_codegen(Stmt* s);

void decl_codegen(Decl* d);

#endif
