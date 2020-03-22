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

void indent_print(char* string, int level);
void _stmt_print(struct stmt* s, int indent);
void body_print(struct stmt *body, int indent_level, int indent_first);
void stmt_print(struct stmt* s);

void indent_print(char* string, int level) {
    for (int i = 0; i < level; i++) {
        printf("    ");
    }

    printf("%s", string);
}

// indent_first == boolean
void body_print(struct stmt *s, int indent_level, int indent_first) {
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
        _stmt_print(s->body, indent_level + 1);
    }
}

void _stmt_print(struct stmt* s, int indent) {
    if (!s) return;

    switch (s->kind) {
        case STMT_DECL:
            indent_print("", indent);
            decl_print(s->decl);
            break;
        case STMT_EXPR:
            indent_print("", indent);
            expr_print(s->expr);
            break;
        case STMT_IF_ELSE:
            indent_print("if (", indent);
            expr_print(s->expr);
            printf(") ");
            body_print(s->body, indent, 0);
            //_stmt_print(s->body, indent+1);

            if (s->else_body) {
                indent_print("else (", indent);
                expr_print(s->expr);
                printf(") ");
                body_print(s->body, indent, 0);
                //_stmt_print(s->body, indent);
            }
            break;
        case STMT_FOR:
            indent_print("for (", indent);
            expr_print(s->init_expr);
            printf("; ");
            expr_print(s->expr);
            printf("; ");
            expr_print(s->next_expr);
            printf(")\n");
            _stmt_print(s->body, indent+1);
            break;
        case STMT_PRINT:
            printf("FIXME: printing print statements.\n");
            break;
        case STMT_RETURN:
            indent_print("return ", indent);
            expr_print(s->expr);
            printf(";\n");
            break;
        case STMT_BLOCK:
            //indent_print("{\n", indent);
            //_stmt_print(s->body, indent+1);
            //indent_print("}\n", indent);
            body_print(s, indent, 1);
            break;
    }

    _stmt_print(s->next, indent);
}

void stmt_print(struct stmt* s) {
    _stmt_print(s, 0);
}
