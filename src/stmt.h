#ifndef STMT_H
#define STMT_H

typedef struct Decl Decl;
typedef struct Symbol Symbol;
typedef struct Expr Expr;

typedef enum {
    STMT_DECL = 0,
    STMT_EXPR,
    STMT_IF_ELSE,
    STMT_FOR,
    STMT_PRINT,
    STMT_RETURN,
    STMT_BLOCK,
} Stmt_t;

typedef struct Stmt Stmt;

struct Stmt {
    Stmt_t kind;
    Decl* decl;
    Expr* init_expr;
    Expr* expr;
    Expr* next_expr;
    Stmt* body;
    Stmt* else_body;
    Stmt* next;

    Symbol* symbol;

    // for return statements
    const char* function_name;
};

Stmt* stmt_create(
    Stmt_t kind,
    Decl* decl,
    Expr* init_expr,
    Expr* expr,
    Expr* next_expr,
    Stmt* body,
    Stmt* else_body,
    Stmt* next
);

void stmt_delete(Stmt* s);

Stmt* stmt_create_if_else(
    Expr* expr, Stmt* body, Stmt* else_body
);

Stmt* stmt_create_for(
    Expr* init_expr, Expr* expr,
    Expr* next_expr, Stmt* body
);

Stmt* stmt_create_block(Stmt* body);

Stmt* stmt_create_return(Expr* expr);

Stmt* stmt_create_print(Expr* expr);

Stmt* stmt_create_decl(Decl* decl);

Stmt* stmt_create_expr(Expr* expr);

void stmt_print(Stmt* s);

#endif
