#include <stdlib.h>
#include <stdio.h>

#include "x64_codegen.h"
#include "symbol.h"
#include "expr.h"
#include "stmt.h"
#include "decl.h"

#define X64_NUM_REGISTERS 7

//
// scratch register stuff
//

struct ScratchTable {
    const char* name[X64_NUM_REGISTERS];
    int in_use[X64_NUM_REGISTERS];
};

struct ScratchTable scratch_table = {
    { "%rbx", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15" },
    { 0, 0, 0, 0, 0, 0, 0 },
};

int scratch_alloc() {
    for (int r = 0; r < X64_NUM_REGISTERS; r++) {
        if (!scratch_table.in_use[r]) {
            scratch_table.in_use[r] = 1;
            return r;
        }
    }
    printf("Error: All registers are in use.\n");
    exit(1);
}

void scratch_free(int r) {
    if (r < 0 || r >= X64_NUM_REGISTERS) {
        printf("Error: Register value passed to scratch_free (%d) is not a valid register.\n", r);
        exit(1);
    }
    scratch_table.in_use[r] = 0;
}

const char* scratch_name(int r) {
    if (r < 0 || r >= X64_NUM_REGISTERS) {
        printf("Error: Register value passed to scratch_name (%d) is not a valid register.\n", r);
        exit(1);
    }
    return scratch_table.name[r];
}

//
// label stuff
//

int current_label_num = 0;

int label_create() {
    return current_label_num++;
}

const char* label_name(int label) {
    char* temp = malloc(sizeof(char) * 256);
    sprintf(temp, ".L%d", label);
    return temp;
}

const char* symbol_codegen(Symbol* s) {
    char* name = malloc(sizeof(char) * 256);
    if (name == 0) {
        printf("Error: ran out of memory.");
        exit(1);
    }

    if (s->kind == SYMBOL_GLOBAL) {
        name = s->name;
    } else {
        // (s->which+1) here to convert from zero-based
        int offset_abs = (s->which+1)*8;
        sprintf(name, "-%d(%%rbp)", offset_abs);
    }
    //printf("s->name gets symbol %s\n", name);

    return name;
}

void expr_codegen(Expr* e) {
    if (!e) return;

    switch (e->kind) {
        // LEAF node: allocate register and load value
        case EXPR_NAME:
            e->reg = scratch_alloc();
            printf("MOVQ %s, %s\n",
                symbol_codegen(e->symbol),
                scratch_name(e->reg));
            break;
        // literals
        case EXPR_STRING_LITERAL: {
            // .data
            // .<label>:
            //     .str <value>
            // .text
            printf(".data\n");

            int str_label = label_create();
            printf("%s:\n", label_name(str_label));
            printf("\t.str \"%s\"\n", e->string_literal);
            printf(".text\n");

            e->reg = scratch_alloc();
            printf("LEAQ %s, %s\n",
                    label_name(str_label),
                    scratch_name(e->reg));
        } break;
        case EXPR_CHAR_LITERAL:
        case EXPR_INTEGER_LITERAL:
        case EXPR_BOOLEAN_LITERAL:
            e->reg = scratch_alloc();
            printf("MOVQ %d, %s\n",
                e->integer_value,
                scratch_name(e->reg));
            break;
        
        // Interior node: generate children, then add them.
        
        // arithmetic expressions
        case EXPR_ADD:
            // ADDQ left, right
            expr_codegen(e->left);
            expr_codegen(e->right);

            printf("ADDQ %s, %s\n",
                scratch_name(e->left->reg),
                scratch_name(e->right->reg));

            e->reg = e->right->reg;
            scratch_free(e->left->reg);
            break;
        case EXPR_SUB:
            // SUBQ left, right
            expr_codegen(e->left);
            expr_codegen(e->right);

            printf("SUBQ %s, %s\n",
                scratch_name(e->left->reg),
                scratch_name(e->right->reg));

            e->reg = e->right->reg;
            scratch_free(e->left->reg);
            break;
        case EXPR_MUL:
            // MOVQ left, %rax
            // IMULQ right
            // MOVQ %rax, right
            expr_codegen(e->left);
            expr_codegen(e->right);

            printf("MOVQ %s, %%rax\n", scratch_name(e->left->reg));
            printf("IMULQ %s\n",       scratch_name(e->right->reg));
            printf("MOVQ %%rax, %s\n", scratch_name(e->right->reg));

            e->reg = e->right->reg;
            scratch_free(e->left->reg);
            break;
        case EXPR_DIV:
            // MOVQ right, %rax
            // CQO
            // IDIVQ left
            // MOVQ %rax, right
            expr_codegen(e->left);
            expr_codegen(e->right);

            printf("MOVQ %s, %%rax\n", scratch_name(e->right->reg));
            printf("CQO\n");
            printf("IDIVQ %s\n", scratch_name(e->left->reg));
            printf("MOVQ %%rax, %s\n", scratch_name(e->right->reg));

            e->reg = e->right->reg;
            scratch_free(e->left->reg);
            break;
        case EXPR_EXPONENT:
            break;
        case EXPR_MODULO:
            // MOVQ right, %rax
            // CQO
            // IDIVQ left
            // MOVQ %rdx, right
            expr_codegen(e->left);
            expr_codegen(e->right);

            printf("MOVQ %s, %%rax\n", scratch_name(e->right->reg));
            printf("CQO\n");
            printf("IDIVQ %s\n", scratch_name(e->left->reg));
            printf("MOVQ %%rdx, %s\n", scratch_name(e->right->reg));

            e->reg = e->right->reg;
            scratch_free(e->left->reg);
            break;
        case EXPR_NEGATE:
            break;

        // logical operations
        case EXPR_LOGICAL_OR:
            break;
        case EXPR_LOGICAL_AND:
            break;
        case EXPR_LOGICAL_NOT:
            break;

        // conditionals
        case EXPR_CMP_EQUAL:
            break;
        case EXPR_CMP_NOT_EQUAL:
            break;
        case EXPR_CMP_GT:
            break;
        case EXPR_CMP_GT_EQUAL:
            break;
        case EXPR_CMP_LT:
            break;
        case EXPR_CMP_LT_EQUAL:
            break;

        // assignments
        case EXPR_ASSIGN:
            expr_codegen(e->right);
            printf("MOVQ %s, %s\n",
                    scratch_name(e->right->reg),
                    symbol_codegen(e->left->symbol));
            e->reg = e->right->reg;
            break;
        case EXPR_INCREMENT:
            break;
        case EXPR_DECREMENT:
            break;

        // misc.
        case EXPR_CALL:
            break;
        case EXPR_INIT_LIST:
            break;
        case EXPR_ARG:
            break;
        case EXPR_SUBSCRIPT:
            break;
    }
}

void stmt_codegen(Stmt* s) {
    if (!s) return;

    switch (s->kind) {
        case STMT_DECL:
            decl_codegen(s->decl);
            break;
        case STMT_EXPR:
            expr_codegen(s->expr);
            scratch_free(s->expr->reg);
            break;
        case STMT_IF_ELSE: {
            int else_label = label_create();
            int done_label = label_create();

            // condition expr
            expr_codegen(s->expr);
            printf("CMP %s, $0\n", scratch_name(s->expr->reg));
            scratch_free(s->expr->reg);
            printf("JE %s\n", label_name(else_label));

            // if branch
            stmt_codegen(s->body);
            printf("JMP %s\n", label_name(done_label));

            // else branch
            printf("%s:\n", label_name(else_label));
            stmt_codegen(s->else_body);
            printf("%s:\n", label_name(done_label));
        } break;
        case STMT_FOR: {
            int top_label = label_create();
            int done_label = label_create();

            // init expr
            if (s->init_expr) {
                expr_codegen(s->init_expr);
                scratch_free(s->init_expr->reg);
            }

            printf("%s:\n", label_name(top_label));

            // condition expr
            if (s->expr) {
                expr_codegen(s->expr);
                printf("CMP %s, $0\n", scratch_name(s->expr->reg));
                scratch_free(s->expr->reg);
                printf("JE %s\n", label_name(done_label));
            }

            // body
            stmt_codegen(s->body);

            // next expr
            if (s->next_expr) {
                expr_codegen(s->next_expr);
                scratch_free(s->next_expr->reg);
            }
            printf("JMP %s\n", label_name(top_label));

            printf("%s:\n", label_name(done_label));
        } break;
        case STMT_PRINT:
            break;
        case STMT_RETURN:
            expr_codegen(s->expr);
            printf("MOVQ %s, %%rax\n", scratch_name(s->expr->reg));
            printf("JMP .%s_epilogue\n", "***FIXME FUNCTION NAME GOES HERE***");
            scratch_free(s->expr->reg);
            break;
        case STMT_BLOCK:
            stmt_codegen(s->body);
            break;
    }

    printf("\n");
    stmt_codegen(s->next);
}

void decl_codegen(Decl* d) {
    if (!d) return;

    // FIXME: incomplete

    stmt_codegen(d->code);
    switch (d->type->kind) {
        case TYPE_FUNCTION:
            // FIXME: incomplete
            break;
        case TYPE_ARRAY:
            // FIXME: incomplete
            break;
        case TYPE_STRING:
            if (d->symbol->kind == SYMBOL_GLOBAL) {
                printf(".data\n");
                printf("%s:\n", symbol_codegen(d->symbol));
                const char* init_value = "";
                if (d->value) {
                    init_value = d->value->string_literal;
                }
                printf("\t.string \"%s\"\n", init_value);
                printf(".text\n\n");
            } else {
                // FIXME: local string declaration
                printf("FIXME: local string declaration.\n");
            }
            break;
        case TYPE_BOOLEAN:
        case TYPE_CHAR:
        case TYPE_INTEGER:
            if (d->symbol->kind == SYMBOL_GLOBAL) {
                printf(".data\n");
                printf("%s:\n", symbol_codegen(d->symbol));
                int init_value = 0;
                if (d->value) {
                    init_value = d->value->integer_value;
                }
                printf("\t.quad %d\n", init_value);
                printf(".text\n\n");
            } else {
                expr_codegen(d->value);
                //printf("d->value->reg is %d, d->symbol->name is %s\n", d->value, d->symbol->name);
                printf(
                    "MOVQ %s, %s\n",
                    scratch_name(d->value->reg), symbol_codegen(d->symbol)
                );
                scratch_free(d->value->reg);
            }
            break;
        case TYPE_VOID:
            printf("Error: cannot create variable of type void.\n");
            exit(1);
            break;
    }

    decl_codegen(d->next);
}
