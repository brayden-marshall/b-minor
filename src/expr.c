#include <stdlib.h>
#include <stdio.h>

#include "expr.h"

Expr* expr_create(Expr_t kind, Expr* left, Expr* right) {
    Expr* e = malloc(sizeof(Expr));
    e->kind = kind;
    e->left = left;
    e->right = right;
    e->name = NULL;
    e->integer_value = 0;
    e->string_literal = NULL;
    e->symbol = NULL;
    e->reg = -1;
    return e;
}

void expr_delete(Expr* e) {
    if (!e) return;

    expr_delete(e->left);
    expr_delete(e->right);
    free((void*)e->string_literal);

    free(e);
}

Expr* expr_create_name(const char* name) {
    Expr* e = expr_create(EXPR_NAME, 0, 0);
    e->name = name;
    return e;
}

Expr* expr_create_integer_literal(int i) {
    Expr* e = expr_create(EXPR_INTEGER_LITERAL, 0, 0);
    e->integer_value = i;
    return e;
}

Expr* expr_create_boolean_literal(int b) {
    Expr* e = expr_create(EXPR_BOOLEAN_LITERAL, 0, 0);
    e->integer_value = b;
    return e;
}

Expr* expr_create_char_literal(char c) {
    Expr* e = expr_create(EXPR_CHAR_LITERAL, 0, 0);
    e->integer_value = c;
    return e;
}

Expr* expr_create_string_literal(const char* str) {
    Expr* e = expr_create(EXPR_STRING_LITERAL, 0, 0);
    e->string_literal = str;
    return e;
}

Expr* expr_create_arg(Expr* expr, Expr* next) {
    return expr_create(EXPR_ARG, expr, next);
}

Expr* expr_create_init_list(Expr* args) {
    return expr_create(EXPR_INIT_LIST, 0, args);
}

Expr* expr_create_call(const char* name, Expr* args) {
    return expr_create(EXPR_CALL, expr_create_name(name), args);
}

Expr* expr_create_subscript(const char* array_name, Expr* at) {
    return expr_create(EXPR_SUBSCRIPT, expr_create_name(array_name), at);
}

Expr* expr_create_increment(const char* name) {
    return expr_create(EXPR_INCREMENT, expr_create_name(name), 0);
}

Expr* expr_create_decrement(const char* name) {
    return expr_create(EXPR_DECREMENT, expr_create_name(name), 0);
}

void expr_print(Expr* e) {
    if (!e) return;

    if (e->kind != EXPR_LOGICAL_NOT && e->kind != EXPR_NEGATE) {
        expr_print(e->left);
    }

    int print_right = 1;
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
        case EXPR_EXPONENT:
            printf(" ^ ");
            break;
        case EXPR_MODULO:
            printf(" %% ");
            break;
        case EXPR_ASSIGN:
            printf(" = ");
            break;
        case EXPR_NEGATE:
            printf("-");
            expr_print(e->left);
            break;
        case EXPR_LOGICAL_OR:
            printf(" || ");
            break;
        case EXPR_LOGICAL_AND:
            printf(" && ");
            break;
        case EXPR_LOGICAL_NOT:
            printf("!");
            expr_print(e->left);
            break;
        case EXPR_CMP_EQUAL:
            printf(" == ");
            break;
        case EXPR_CMP_NOT_EQUAL:
            printf(" != ");
            break;
        case EXPR_CMP_GT:
            printf(" > ");
            break;
        case EXPR_CMP_GT_EQUAL:
            printf(" >= ");
            break;
        case EXPR_CMP_LT:
            printf(" < ");
            break;
        case EXPR_CMP_LT_EQUAL:
            printf(" <= ");
            break;
        case EXPR_INCREMENT:
            printf("++");
            break;
        case EXPR_DECREMENT:
            printf("--");
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
            print_right = 0;
            printf("(");
            expr_print(e->right);
            printf(")");
            break;
        case EXPR_INIT_LIST:
            print_right = 0;
            printf("{");
            expr_print(e->right);
            printf("}");
            break;
        case EXPR_ARG:
            if (e->right) {
                printf(", ");
            }
            break;
        case EXPR_SUBSCRIPT:
            print_right = 0;
            printf("[");
            expr_print(e->right);
            printf("]");
            break;
    }

    if (print_right) {
        expr_print(e->right);
    }
}
