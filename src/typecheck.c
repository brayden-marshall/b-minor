#include <stdlib.h>
#include <stdio.h>

#include "typecheck.h"
#include "param_list.h"
#include "type.h"
#include "decl.h"
#include "stmt.h"

int type_equals(struct type* a, struct type* b) {
    if (a->kind != b->kind) {
        return 0;
    }

    switch (a->kind) {
        case TYPE_VOID:
        case TYPE_BOOLEAN:
        case TYPE_CHAR:
        case TYPE_INTEGER:
        case TYPE_STRING:
            return 1;
        case TYPE_ARRAY:
            return type_equals(a->subtype, b->subtype);
        case TYPE_FUNCTION:
            return type_equals(a->subtype, b->subtype)
                    && param_list_equals(a->params, b->params);
    }

    return 0;
}

struct type* type_copy(struct type* t) {
    if (!t) return 0;
    struct type* new_t = type_create(t->kind);

    new_t->subtype = type_copy(t);

    return t;
}

void type_delete(struct type* t) {
    if (!t) return;

    type_delete(t->subtype);
    param_list_delete(t->params);
}

void decl_typecheck(struct decl* d) {
    if (!d) return;

    if (d->value) {
        struct type* t;
        t = expr_typecheck(d->value);
        if (!type_equals(t, d->symbol->type)) {
            printf("Type error: cannot assign to variable of a different type.\n");
        }
    }

    if (d->code) {
        stmt_typecheck(d->code);
    }
}

void stmt_typecheck(struct stmt* s) {
    struct type* t;
    switch (s->kind) {
        case STMT_DECL:
            decl_typecheck(s->decl);
            break;
        case STMT_EXPR:
            t = expr_typecheck(s->expr);
            type_delete(t);
            break;
        case STMT_IF_ELSE:
            t = expr_typecheck(s->expr);
            if (t->kind != TYPE_BOOLEAN) {
                printf("Type error: if statement expression must be a boolean.\n");
            }

            type_delete(t);
            stmt_typecheck(s->body);
            stmt_typecheck(s->else_body);
            break;
        case STMT_FOR:
            printf("FIXME: stmt_typecheck for STMT_FOR.\n");
            break;
        case STMT_PRINT:
            printf("FIXME: stmt_typecheck for PRINT.\n");
            break;
        case STMT_RETURN:
            printf("FIXME: stmt_typecheck for RETURN.\n");
            break;
        case STMT_BLOCK:
            printf("FIXME: stmt_typecheck for STMT_BLOCK.\n");
            break;
    }
}

struct type* expr_typecheck(struct expr* e) {
    if (!e) return 0;

    struct type* lt = expr_typecheck(e->left);
    struct type* rt = expr_typecheck(e->right);

    struct type* result;

    switch (e->kind) {
        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_MUL:
        case EXPR_DIV:
        case EXPR_EXPONENT:
        case EXPR_MODULO:
            if (lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
                printf("Type error: arithmetic operations require integers.\n");
            }
            result = type_create(TYPE_INTEGER);
            break;
        case EXPR_NAME:
        case EXPR_NEGATE:
        case EXPR_LOGICAL_OR:
        case EXPR_LOGICAL_AND:
        case EXPR_LOGICAL_NOT:
            break;
        case EXPR_CMP_EQUAL:
        case EXPR_CMP_NOT_EQUAL:
            if (!type_equals(lt, rt)) {
                printf("Type error: comparison operators may only be used on two values of the same type.\n");
            }
            if (lt->kind == TYPE_VOID ||
                lt->kind == TYPE_ARRAY ||
                lt->kind == TYPE_FUNCTION) {
                printf("Type error: cannot compare values of non-atomic types.\n");
            }
            result = type_create(TYPE_BOOLEAN);
            break;
        case EXPR_CMP_GT:
        case EXPR_CMP_GT_EQUAL:
        case EXPR_CMP_LT:
        case EXPR_CMP_LT_EQUAL:
            break;

        case EXPR_CHAR_LITERAL:
            result = type_create(TYPE_CHAR);
        case EXPR_STRING_LITERAL:
            result = type_create(TYPE_STRING);
            break;
        case EXPR_INTEGER_LITERAL:
            result = type_create(TYPE_INTEGER);
            break;
        case EXPR_BOOLEAN_LITERAL:
            result = type_create(TYPE_BOOLEAN);
            break;

        case EXPR_CALL:
        case EXPR_INIT_LIST:
        case EXPR_ARG:
            break;
        case EXPR_SUBSCRIPT:
            if (lt->kind == TYPE_ARRAY) {
                if (rt->kind != TYPE_INTEGER) {
                    printf("Type error: array subscript must be an integer.\n");
                }
                result = type_copy(lt->subtype);
            } else {
                printf("Type error: subscript target is not an array.\n");
                result = type_copy(lt);
            }
            break;
        case EXPR_ASSIGN:

        case EXPR_INCREMENT:
        case EXPR_DECREMENT:
            break;
    }

    type_delete(lt);
    type_delete(rt);

    return result;
}
