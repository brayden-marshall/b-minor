#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "typecheck.h"
#include "param_list.h"
#include "type.h"
#include "decl.h"
#include "stmt.h"

int type_equals(struct type* a, struct type* b) {
    if (!a || !b || a->kind != b->kind) {
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
        default:
            printf("enum case not handled.");
            assert(0);
    }
}

struct type* type_copy(struct type* t) {
    if (!t) return 0;
    struct type* new_t = type_create(t->kind);

    new_t->subtype = type_copy(t->subtype);

    return new_t;
}

void type_delete(struct type* t) {
    if (!t) return;

    type_delete(t->subtype);
    param_list_delete(t->params);
    free(t);
}

void decl_typecheck(struct decl* d) {
    if (!d) return;

    if (d->value) {
        struct type* t;
        t = expr_typecheck(d->value);
        if (!type_equals(t, d->symbol->type)) {
            printf("Type error on symbol '%s': cannot assign to variable of a different type.\n", d->symbol->name);
        }
    }

    if (d->code) {
        stmt_typecheck(d->code);
    }

    decl_typecheck(d->next);
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
            t = expr_typecheck(s->expr);
            if (t->kind != TYPE_BOOLEAN) {
                printf("Type error: if statement expression must be a boolean.\n");
            }

            type_delete(t);
            stmt_typecheck(s->body);
            break;
        case STMT_PRINT:
            expr_typecheck(s->expr);
            break;
        case STMT_RETURN:
            expr_typecheck(s->expr);
            break;
        case STMT_BLOCK:
            stmt_typecheck(s->body);
            break;
    }
}

struct type* expr_typecheck(struct expr* e) {
    if (!e) return 0;

    struct type* lt = expr_typecheck(e->left);
    struct type* rt = expr_typecheck(e->right);

    struct type* result = NULL;

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
            if (e->symbol == NULL) {
                printf("e->symbol is NULL for expression with name: '%s'\n", e->name);
            }
            assert(e->symbol != NULL);
            result = type_copy(e->symbol->type);
            break;
        case EXPR_NEGATE:
            if (lt->kind != TYPE_INTEGER) {
                printf("Type error: cannot negate a non-integer.\n");
            }
            result = type_create(TYPE_INTEGER);
            break;
        case EXPR_LOGICAL_OR:
        case EXPR_LOGICAL_AND:
            if (lt->kind != TYPE_BOOLEAN || rt->kind != TYPE_BOOLEAN) {
                printf("Type error: cannot perform logical operation on non-boolean.\n");
            }
            result = type_create(TYPE_BOOLEAN);
            break;
        case EXPR_LOGICAL_NOT:
            if (lt->kind != TYPE_BOOLEAN) {
                printf("Type error: cannot logically negate a non-boolean.\n");
            }
            result = type_create(TYPE_BOOLEAN);
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
            if (lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
                printf("Type error: cannot relative compare non-integer types.\n");
            }
            result = type_create(TYPE_BOOLEAN);
            break;
        case EXPR_CHAR_LITERAL:
            result = type_create(TYPE_CHAR);
            break;
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
            // subtype is the return type of the function being called
            result = type_copy(lt->subtype);
            break;
        case EXPR_INIT_LIST:
            result = type_create_array(type_copy(rt), 0);
            break;
        case EXPR_ARG:
            result = type_copy(lt);
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
            if (lt->kind != rt->kind) {
                printf("Type error: cannot assign to a variable of a different type.\n");
            }
            result = type_copy(lt);
            break;
        case EXPR_INCREMENT:
            if (lt->kind != TYPE_INTEGER) {
                printf("Type error: cannot use increment operator on non-integer.\n");
            }
            result = type_create(TYPE_INTEGER);
            break;
        case EXPR_DECREMENT:
            if (lt->kind != TYPE_INTEGER) {
                printf("Type error: cannot use decrement operator on non-integer.\n");
            }
            result = type_create(TYPE_INTEGER);
            break;
    }

    type_delete(lt);
    type_delete(rt);

    assert(result != NULL);
    return result;
}
