#ifndef EXPR_H
#define EXPR_H

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

    EXPR_INCREMENT,
    EXPR_DECREMENT,

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

struct expr* expr_create_init_list(struct expr* args);

struct expr* expr_create_call(const char* name, struct expr* args);

struct expr* expr_create_subscript(const char* array_name, struct expr* at);

struct expr* expr_create_increment(const char* name);

struct expr* expr_create_decrement(const char* name);

void expr_print(struct expr* e);

#endif
