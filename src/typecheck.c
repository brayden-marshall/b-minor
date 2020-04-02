#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "typecheck.h"
#include "param_list.h"
#include "type.h"
#include "decl.h"
#include "stmt.h"

#include <stdarg.h>

void error_print(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt != '\0') {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'T':
                    type_print(va_arg(args, struct type*));
                    break;
                case 'E':
                    expr_print(va_arg(args, struct expr*));
                    break;
                case 's':
                    printf("%s", va_arg(args, char*));
                    break;
                case '%':
                    putchar('%');
                    break;
                default:
                    printf("Error in 'error_print': reached unexpected character %c after %%.\n", *fmt);
                    exit(1);
                    break;
            }
        } else {
            putchar(*fmt);
        }

        fmt++;
    }

    va_end(args);
}

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
            printf("Compiler bug: enum case not handled.\n");
            assert(0);
    }
}

struct type* type_copy(struct type* t) {
    if (!t) return NULL;

    struct type* new_t = type_create(t->kind);

    new_t->subtype = type_copy(t->subtype);

    return new_t;
}

void decl_typecheck(struct decl* d) {
    if (!d) return;

    if (d->value) {
        struct type* t;
        t = expr_typecheck(d->value);
        if (!type_equals(t, d->symbol->type)) {
            error_print("Type error: cannot assign to a variable of a different type.\n\tGot declaration (%s: %T = %E), which is of type (%T) = (%T).\n", d->name, d->symbol->type, d->value, d->symbol->type, t);
        }
    }

    if (d->code) {
        stmt_typecheck(d->code);
    }

    decl_typecheck(d->next);
}

void stmt_typecheck(struct stmt* s) {
    if (!s) return;

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
                error_print("Type error: if statement condition must be a boolean.\n\tGot expression (%E), which is of type (%T).\n", s->expr, t);
            }

            type_delete(t);
            stmt_typecheck(s->body);
            stmt_typecheck(s->else_body);
            break;
        case STMT_FOR:
            t = expr_typecheck(s->expr);

            if (t == NULL || t->kind != TYPE_BOOLEAN) {
                error_print("Type error: for loop condition must be a boolean.\n\tGot expression (%E), which is of type (%T)\n", s->expr, t);
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

    stmt_typecheck(s->next);
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
                error_print("Type error: arithmetic operations require integers. \n\tGot the expression (%E), which is of type (%T) + (%T)\n", e, lt, rt);
            }
            result = type_create(TYPE_INTEGER);
            break;
        case EXPR_NAME:
            // e->symbol is NULL if e is being used before it is declared
            if (e->symbol == NULL) {
                // return arbitrary type so we can continue with checking for other type errors
                result = type_create(TYPE_INTEGER);
            } else {
                result = type_copy(e->symbol->type);
            }
            break;
        case EXPR_NEGATE:
            if (lt->kind != TYPE_INTEGER) {
                error_print("Type error: negate operator requries an integer.\n\tGot expression (%E), which is of type (%T).\n", lt);
            }
            result = type_create(TYPE_INTEGER);
            break;
        case EXPR_LOGICAL_OR:
        case EXPR_LOGICAL_AND:
            if (lt->kind != TYPE_BOOLEAN || rt->kind != TYPE_BOOLEAN) {
                error_print("Type error: logical operators require boolean arguments.\n\tGot the expression (%E), which is of type (%T) %s (%T)\n", e, lt, e->kind == EXPR_LOGICAL_OR ? "||" : "&&", rt);
            }
            result = type_create(TYPE_BOOLEAN);
            break;
        case EXPR_LOGICAL_NOT:
            if (lt->kind != TYPE_BOOLEAN) {
                error_print("Type error: logical negation requires a boolean.\n\tGot the expression (%E) which is of type (%T)\n", e, lt);
            }
            result = type_create(TYPE_BOOLEAN);
            break;
        case EXPR_CMP_EQUAL:
        case EXPR_CMP_NOT_EQUAL:
            if (!type_equals(lt, rt)) {
                error_print("Type error: comparison operators may only be used on two values of the same type.\n\tGot the expression (%E), which is of the type (%T) %s (%T).\n", e, lt, e->kind == EXPR_CMP_EQUAL ? "==" : "!=", rt);
            }

            if (lt->kind == TYPE_VOID ||
                lt->kind == TYPE_ARRAY ||
                lt->kind == TYPE_FUNCTION) {
                error_print("Type error: cannot compare values of non-atomic types. \n\tGot the expression (%E), which is of type (%T)\n", e, lt);
            }
            result = type_create(TYPE_BOOLEAN);
            break;
        case EXPR_CMP_GT:
        case EXPR_CMP_GT_EQUAL:
        case EXPR_CMP_LT:
        case EXPR_CMP_LT_EQUAL:
            if (lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
                error_print("Type error: cannot use relative comparison operators on non-integer types.\n\tGot expression (%E), with operands of type (%T), (%T)\n", e, lt, rt);
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
                    error_print("Type error: array subscript must be an integer.\n\tGot expression (%E), which is of type (%T).\n", e->right, rt);
                }
                result = type_copy(lt->subtype);
            } else {
                error_print("Type error: subscript target is not an array.\n\tGot expression (%E), which is of type (%T)\n", e->left, lt);
                result = type_copy(lt);
            }
            break;
        case EXPR_ASSIGN:
            if (lt->kind != rt->kind) {
                error_print("Type error: cannot assign to a variable of a different type.\n\tGot expression (%E), which is of type (%T) = (%T).\n", e, lt, rt);
            }
            result = type_copy(lt);
            break;
        case EXPR_INCREMENT:
            if (lt->kind != TYPE_INTEGER) {
                error_print("Type error: cannot use increment operator on non-integer.\n\tGot expression (%E), which is of type (%T).\n", e, lt);
            }
            result = type_create(TYPE_INTEGER);
            break;
        case EXPR_DECREMENT:
            if (lt->kind != TYPE_INTEGER) {
                error_print("Type error: cannot use decrement operator on non-integer.\n\tGot expression (%E), which is of type (%T).\n", e, lt);
            }
            result = type_create(TYPE_INTEGER);
            break;
        default:
            printf("Compiler bug: enum case not handled.\n");
            assert(0);
    }

    type_delete(lt);
    type_delete(rt);

    assert(result != NULL);
    return result;
}
