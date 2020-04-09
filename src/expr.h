#ifndef EXPR_H
#define EXPR_H

#include "symbol.h"

typedef enum {
    // arithmetic operations
    EXPR_ADD = 0,
    EXPR_SUB,
    EXPR_MUL,
    EXPR_DIV,
    EXPR_EXPONENT,
    EXPR_MODULO,
    EXPR_NAME,
    EXPR_NEGATE,

    // logical operations
    EXPR_LOGICAL_OR,
    EXPR_LOGICAL_AND,
    EXPR_LOGICAL_NOT,

    // comparison operations
    EXPR_CMP_EQUAL,
    EXPR_CMP_NOT_EQUAL,
    EXPR_CMP_GT,
    EXPR_CMP_GT_EQUAL,
    EXPR_CMP_LT,
    EXPR_CMP_LT_EQUAL,

    // literals
    EXPR_CHAR_LITERAL,
    EXPR_STRING_LITERAL,
    EXPR_INTEGER_LITERAL,
    EXPR_BOOLEAN_LITERAL,

    // other
    EXPR_CALL,
    EXPR_INIT_LIST,
    EXPR_ARG,
    EXPR_SUBSCRIPT,
    EXPR_ASSIGN,

    EXPR_INCREMENT,
    EXPR_DECREMENT,
} Expr_t;

typedef struct Expr Expr;

struct Expr {
    Expr_t kind;
    Expr* left;
    Expr* right;

    const char* name;
    int integer_value;
    const char* string_literal;

    Symbol* symbol;
};

Expr* expr_create(Expr_t kind, Expr* left, Expr* right);

void expr_delete(Expr* e);

Expr* expr_create_name(const char* name);

Expr* expr_create_integer_literal(int i);

Expr* expr_create_boolean_literal(int b);

Expr* expr_create_char_literal(char c);

Expr* expr_create_string_literal(const char* str);

Expr* expr_create_arg(Expr* expr, Expr* next);

Expr* expr_create_init_list(Expr* args);

Expr* expr_create_call(const char* name, Expr* args);

Expr* expr_create_subscript(const char* array_name, Expr* at);

Expr* expr_create_increment(const char* name);

Expr* expr_create_decrement(const char* name);

void expr_print(Expr* e);

#endif
