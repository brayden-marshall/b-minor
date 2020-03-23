#ifndef EXPR_H
#define EXPR_H

typedef enum {
    EXPR_ADD = 0,
    EXPR_SUB,
    EXPR_MUL,
    EXPR_DIV,
    EXPR_NAME,

    EXPR_CHAR_LITERAL,
    EXPR_STRING_LITERAL,
    EXPR_INTEGER_LITERAL,
    EXPR_BOOLEAN_LITERAL,

    EXPR_CALL,
    EXPR_ARG,
    EXPR_SUBSCRIPT,
} expr_t;

struct expr {
    expr_t kind;
    struct expr* left;
    struct expr* right;

    const char* name;
    int integer_value;
    const char* string_literal;
};

struct expr* expr_create(expr_t kind, struct expr* left, struct expr* right);

struct expr* expr_create_name(const char* name);

struct expr* expr_create_integer_literal(int i);

struct expr* expr_create_boolean_literal(int b);

struct expr* expr_create_char_literal(char c);

struct expr* expr_create_string_literal(const char* str);

struct expr* expr_create_arg(struct expr* expr, struct expr* next);

void expr_print(struct expr* e);

#endif
