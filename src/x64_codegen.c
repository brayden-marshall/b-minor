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
    char* temp = 0;
    sprintf(temp, ".L%d", label);
    return temp;
}

const char* symbol_codegen(Symbol* s) {
    char* name = NULL;

    if (s->kind == SYMBOL_GLOBAL) {
        name = s->name;
    } else {
        // (s->which+1) here to convert from zero-based
        int offset_abs = (s->which+1)*8;
        sprintf(name, "-%d(%%rbp)", offset_abs);
    }
    printf("s->name gets symbol %s\n", name);

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
        
        // Interior node: generate children, then add them.
        case EXPR_ADD:
            expr_codegen(e->left);
            expr_codegen(e->right);
            printf("ADDQ %s, %s\n",
                scratch_name(e->left->reg),
                scratch_name(e->right->reg));
            e->reg = e->right->reg;
            scratch_free(e->left->reg);
            break;
        case EXPR_SUB:
            break;
        case EXPR_MUL:
            break;
        case EXPR_DIV:
            break;
        case EXPR_EXPONENT:
            break;
        case EXPR_MODULO:
            break;
        case EXPR_NEGATE:
            break;

        case EXPR_ASSIGN:
            expr_codegen(e->left);
            printf("MOVQ %s, %s\n",
                    scratch_name(e->left->reg),
                    symbol_codegen(e->right->symbol));
            e->reg = e->left->reg;
            break;

        case EXPR_LOGICAL_OR:
            break;
        case EXPR_LOGICAL_AND:
            break;
        case EXPR_LOGICAL_NOT:
            break;

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

        case EXPR_CHAR_LITERAL:
            break;
        case EXPR_STRING_LITERAL:
            break;
        case EXPR_INTEGER_LITERAL:
            break;
        case EXPR_BOOLEAN_LITERAL:
            break;

        case EXPR_CALL:
            break;
        case EXPR_INIT_LIST:
            break;
        case EXPR_ARG:
            break;
        case EXPR_SUBSCRIPT:
            break;

        case EXPR_INCREMENT:
            break;
        case EXPR_DECREMENT:
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
        case STMT_IF_ELSE:
            break;
        case STMT_FOR:
            break;
        case STMT_PRINT:
            break;
        case STMT_RETURN:
            expr_codegen(s->expr);
            printf("MOV %s, %%rax\n", scratch_name(s->expr->reg));
            printf("JMP .%s_epilogue\n", /*FIXME: function_name*/"");
            scratch_free(s->expr->reg);
            break;
        case STMT_BLOCK:
            break;
    }

    stmt_codegen(s->next);
}

void decl_codegen(Decl* d) {
}
