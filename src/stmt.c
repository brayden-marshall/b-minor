#include <stdlib.h>
#include <stdio.h>

#include "stmt.h"
#include "decl.h"
#include "expr.h"

Stmt* stmt_create(
    Stmt_t kind,
    Decl* decl,
    Expr* init_expr,
    Expr* expr,
    Expr* next_expr,
    Stmt* body,
    Stmt* else_body,
    Stmt* next
) {
    Stmt* s = malloc(sizeof(*s));
    s->kind = kind;
    s->decl = decl;
    s->init_expr = init_expr;
    s->expr = expr;
    s->next_expr = next_expr;
    s->body = body;
    s->else_body = else_body;
    s->next = next;
    s->symbol = NULL;
    return s;
}

void stmt_delete(Stmt* s) {
    if (!s) return;

    decl_delete(s->decl);
    expr_delete(s->init_expr);
    expr_delete(s->expr);
    expr_delete(s->next_expr);
    stmt_delete(s->body);
    stmt_delete(s->else_body);
    stmt_delete(s->next);

    free(s);
}

Stmt* stmt_create_if_else(
    Expr* expr, Stmt* body, Stmt* else_body
) {
    return stmt_create(
        STMT_IF_ELSE, 0, 0, expr, 0, body, else_body, 0
    );
}

Stmt* stmt_create_block(Stmt* body) {
    return stmt_create(
        STMT_BLOCK, 0, 0, 0, 0, body, 0, 0
    );
}

Stmt* stmt_create_return(Expr* expr) {
    return stmt_create(
        STMT_RETURN, 0, 0, expr, 0, 0, 0, 0
    );
}

Stmt* stmt_create_print(Expr* expr) {
    return stmt_create(
        STMT_PRINT, 0, 0, expr, 0, 0, 0, 0
    );
}

Stmt* stmt_create_for(
    Expr* init_expr, Expr* expr,
    Expr* next_expr, Stmt* body
) {
    return stmt_create(
        STMT_FOR, 0, init_expr, expr, next_expr, body, 0, 0
    );
}

int x = 1234;

Stmt* stmt_create_decl(Decl* decl) {
    return stmt_create(
        STMT_DECL, decl, 0, 0, 0, 0, 0, 0
    );
}

Stmt* stmt_create_expr(Expr* expr) {
    return stmt_create(
        STMT_EXPR, 0, 0, expr, 0, 0, 0, 0
    );
}

void indent_print(char* string, int level);
void _stmt_print(Stmt* s, int indent);
void body_print(Stmt *body, int indent_level, int indent_first);
void stmt_print(Stmt* s);

void indent_print(char* string, int level) {
    for (int i = 0; i < level; i++) {
        printf("    ");
    }

    printf("%s", string);
}

// indent_first == boolean
void body_print(Stmt *s, int indent_level, int indent_first) {
    if (!s) return;

    if (s->kind == STMT_BLOCK) {
        if (indent_first) {
            indent_print("{\n", indent_level);
        } else {
            printf("{\n");
        }
        _stmt_print(s->body, indent_level + 1);
        indent_print("}\n", indent_level);
    } else {
        printf("\n");
        _stmt_print(s, indent_level + 1);
    }
}

void _stmt_print(Stmt* s, int indent) {
    if (!s) return;

    switch (s->kind) {
        case STMT_DECL:
            indent_print("", indent);
            decl_print(s->decl);
            break;
        case STMT_EXPR:
            indent_print("", indent);
            expr_print(s->expr);
            printf(";\n");
            break;
        case STMT_IF_ELSE:
            indent_print("if (", indent);
            expr_print(s->expr);
            printf(") ");
            body_print(s->body, indent, 0);

            if (s->else_body) {
                indent_print("else (", indent);
                expr_print(s->expr);
                printf(") ");
                body_print(s->body, indent, 0);
            }
            break;
        case STMT_FOR:
            indent_print("for (", indent);
            expr_print(s->init_expr);
            printf("; ");
            expr_print(s->expr);
            printf("; ");
            expr_print(s->next_expr);
            printf(") ");
            body_print(s->body, indent, 0);
            break;
        case STMT_PRINT:
            indent_print("print ", indent);
            expr_print(s->expr);
            printf(";\n");
            break;
        case STMT_RETURN:
            indent_print("return ", indent);
            expr_print(s->expr);
            printf(";\n");
            break;
        case STMT_BLOCK:
            body_print(s, indent, 1);
            break;
    }

    _stmt_print(s->next, indent);
}

void stmt_print(Stmt* s) {
    _stmt_print(s, 0);
}
