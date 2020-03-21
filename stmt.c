#include <stdlib.h>
#include <stdio.h>

#include "stmt.h"
#include "decl.h"
#include "expr.h"

struct stmt* stmt_create(
    stmt_t kind,
    struct decl* decl,
    struct expr* init_expr,
    struct expr* expr,
    struct expr* next_expr,
    struct stmt* body,
    struct stmt* else_body,
    struct stmt* next
) {
    struct stmt* s = malloc(sizeof(*s));
    s->kind = kind;
    s->decl = decl;
    s->init_expr = init_expr;
    s->expr = expr;
    s->next_expr = next_expr;
    s->body = body;
    s->else_body = else_body;
    s->next = next;

    return s;
}

struct stmt* stmt_create_if_else(
    struct expr* expr, struct stmt* body, struct stmt* else_body
) {
    struct stmt* s = malloc(sizeof(*s));
    s->kind = STMT_IF_ELSE;
    s->expr = expr;
    s->body = body;
    s->else_body = else_body;
    return s;
}

struct stmt* stmt_create_block(struct stmt* body) {
    struct stmt* s = malloc(sizeof(*s));
    s->kind = STMT_BLOCK;
    s->body = body;
    return s;
}

struct stmt* stmt_create_return(struct expr* expr) {
    struct stmt* s = malloc(sizeof(*s));
    s->kind = STMT_RETURN;
    s->expr = expr;
    return s;
}

void stmt_print(struct stmt* s) {
    if (!s) return;

    switch (s->kind) {
        case STMT_DECL:
            decl_print(s->decl);
            break;
        case STMT_EXPR:
            expr_print(s->expr);
            break;
        case STMT_IF_ELSE:
            printf("if (");
            expr_print(s->expr);
            printf(") ");
            stmt_print(s->body);

            if (s->else_body) {
                printf(" else (");
                expr_print(s->expr);
                printf(") ");
                stmt_print(s->body);
            }
            break;
        case STMT_FOR:
            printf("for (");
            expr_print(s->init_expr);
            printf("; ");
            expr_print(s->expr);
            printf("; ");
            expr_print(s->next_expr);
            printf(") ");
            stmt_print(s->body);
            break;
        case STMT_PRINT:
            printf("FIXME: printing print statements.\n");
            break;
        case STMT_RETURN:
            printf("return ");
            expr_print(s->expr);
            printf(";\n");
            break;
        case STMT_BLOCK:
            printf("{\n");
            stmt_print(s->body);
            printf("}\n");
            break;
    }

    stmt_print(s->next);
}
