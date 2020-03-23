#include <stdlib.h>
#include <stdio.h>

#include "expr.h"

struct expr* expr_create(expr_t kind, struct expr* left, struct expr* right) {
    struct expr* e = malloc(sizeof(*e));
    e->kind = kind;
    e->left = left;
    e->right = right;
    e->name = 0;
    e->integer_value = 0;
    e->string_literal = 0;
    return e;
}

struct expr* expr_create_name(const char* name) {
    struct expr* e = expr_create(EXPR_NAME, 0, 0);
    e->name = name;
    return e;
}

struct expr* expr_create_integer_literal(int i) {
    struct expr* e = expr_create(EXPR_INTEGER_LITERAL, 0, 0);
    e->integer_value = i;
    return e;
}

struct expr* expr_create_boolean_literal(int b) {
    struct expr* e = expr_create(EXPR_BOOLEAN_LITERAL, 0, 0);
    e->integer_value = b;
    return e;
}

struct expr* expr_create_char_literal(char c) {
    struct expr* e = expr_create(EXPR_CHAR_LITERAL, 0, 0);
    e->integer_value = c;
    return e;
}

struct expr* expr_create_string_literal(const char* str) {
    struct expr* e = expr_create(EXPR_STRING_LITERAL, 0, 0);
    e->string_literal = str;
    return e;
}

void expr_print(struct expr* e) {
    if (!e) return;

    expr_print(e->left);

    switch (e->kind) {
        case EXPR_ADD:
            printf(" + ");
            break;
        case EXPR_SUB:
            printf(" - ");
            break;
        case EXPR_MUL:
            printf(" * ");
            break;
        case EXPR_DIV:
            printf(" / ");
            break;
        case EXPR_NAME:
            printf("%s", e->name);
            break;
        case EXPR_CHAR_LITERAL:
            printf("'%c'", e->integer_value);
            break;
        case EXPR_STRING_LITERAL:
            printf("\"%s\"", e->string_literal);
            break;
        case EXPR_INTEGER_LITERAL:
            printf("%d", e->integer_value);
            break;
        case EXPR_BOOLEAN_LITERAL:
            printf("%s", e->integer_value ? "true" : "false");
            break;
        case EXPR_CALL:
            printf("FIXME: expr_print EXPR_CALL\n");
            break;
        case EXPR_ARG:
            printf("FIXME: expr_print EXPR_ARG\n");
            break;
        case EXPR_SUBSCRIPT:
            printf("FIXME: expr_print EXPR_SUBSCRIPT\n");
            break;
    }

    expr_print(e->right);
}
