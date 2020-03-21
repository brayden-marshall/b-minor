#ifndef STMT_H
#define STMT_H

typedef enum {
    STMT_DECL = 0,
    STMT_EXPR,
    STMT_IF_ELSE,
    STMT_FOR,
    STMT_PRINT,
    STMT_RETURN,
    STMT_BLOCK
} stmt_t;

struct stmt {
    stmt_t kind;
    struct decl* decl;
    struct expr* init_expr;
    struct expr* expr;
    struct expr* next_expr;
    struct stmt* body;
    struct stmt* else_body;
    struct stmt* next;
};

struct stmt* stmt_create(
    stmt_t kind,
    struct decl* decl,
    struct expr* init_expr,
    struct expr* expr,
    struct expr* next_expr,
    struct stmt* body,
    struct stmt* else_body,
    struct stmt* next
);

struct stmt* stmt_create_if_else(
    struct expr* expr, struct stmt* body, struct stmt* else_body
);

struct stmt* stmt_create_block(struct stmt* body);

struct stmt* stmt_create_return(struct expr* expr);

void stmt_print(struct stmt* s);

#endif
