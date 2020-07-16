#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "x64_codegen.h"
#include "symbol.h"
#include "expr.h"
#include "stmt.h"
#include "decl.h"

#define X64_NUM_SCRATCH_REGISTERS 7
#define X64_NUM_ARGUMENT_REGISTERS 6

FILE* output_file = NULL;

const char* argument_registers[X64_NUM_ARGUMENT_REGISTERS] = {
    "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9",
};

struct ScratchTable {
    const char* name[X64_NUM_SCRATCH_REGISTERS];
    int in_use[X64_NUM_SCRATCH_REGISTERS];
};

struct ScratchTable scratch_table = {
    { "%rbx", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15" },
    { 0, 0, 0, 0, 0, 0, 0 },
};

int scratch_alloc() {
    for (int r = 0; r < X64_NUM_SCRATCH_REGISTERS; r++) {
        if (!scratch_table.in_use[r]) {
            scratch_table.in_use[r] = 1;
            return r;
        }
    }
    printf("Error: All registers are in use.\n");
    assert(0);
}

void scratch_free(int r) {
    if (r < 0 || r >= X64_NUM_SCRATCH_REGISTERS) {
        printf("Error: Register value passed to scratch_free (%d) is not a valid register.\n", r);
        assert(0);
    }
    scratch_table.in_use[r] = 0;
}

const char* scratch_name(int r) {
    if (r < 0 || r >= X64_NUM_SCRATCH_REGISTERS) {
        printf("Error: Register value passed to scratch_name (%d) is not a valid register.\n", r);
        assert(0);
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
    if (name == NULL) {
        printf("Error: ran out of memory.");
        assert(0);
    }

    if (s->kind == SYMBOL_GLOBAL) {
        sprintf(name, "%s(%%rip)", s->name);
    } else if (s->kind == SYMBOL_LOCAL) {
        // (s->which+1) here to convert from zero-based
        int offset = (s->which+1)*8;
        sprintf(name, "-%d(%%rbp)", offset);
    } else if (s->kind == SYMBOL_PARAM) {
        if (s->which < X64_NUM_ARGUMENT_REGISTERS) {
            sprintf(name, "-%d(%%rbp)", (s->which+1)*8);
        } else {
            int offset = 32 + ((s->which - X64_NUM_ARGUMENT_REGISTERS) * 8);
            sprintf(name, "%d(%%rbp)", offset);
        }
    }
    //printf("s->name gets symbol %s\n", name);

    return name;
}

void expr_codegen(Expr* e) {
    if (!e) return;

    switch (e->kind) {
        case EXPR_NAME: {
            const char* symbol = symbol_codegen(e->symbol);
            e->reg = scratch_alloc();
            fprintf(output_file, "MOVQ %s, %s\n",
                symbol,
                scratch_name(e->reg));
            free((void*) symbol);
        } break;
        // literals
        case EXPR_STRING_LITERAL: {
            // .data
            // .<label>:
            //     .str <value>
            // .text
            fprintf(output_file, ".data\n");

            const char* str_label = label_name(label_create());

            fprintf(output_file, "%s:\n", str_label);

            fprintf(output_file, "\t.string \"%s\"\n", e->string_literal);
            fprintf(output_file, ".text\n");

            e->reg = scratch_alloc();
            fprintf(output_file, "LEAQ %s(%%rip), %s\n",
                    str_label,
                    scratch_name(e->reg));

            free((void*)str_label);
        } break;
        case EXPR_CHAR_LITERAL:
        case EXPR_INTEGER_LITERAL:
        case EXPR_BOOLEAN_LITERAL:
            e->reg = scratch_alloc();
            fprintf(output_file, "MOVQ $%d, %s\n",
                e->integer_value,
                scratch_name(e->reg));
            break;
        // arithmetic expressions
        case EXPR_ADD:
            expr_codegen(e->left);
            expr_codegen(e->right);

            fprintf(output_file, "ADDQ %s, %s\n",
                scratch_name(e->left->reg),
                scratch_name(e->right->reg));

            e->reg = e->right->reg;
            scratch_free(e->left->reg);
            break;
        case EXPR_SUB:
            expr_codegen(e->left);
            expr_codegen(e->right);

            fprintf(output_file, "SUBQ %s, %s\n",
                scratch_name(e->right->reg),
                scratch_name(e->left->reg));

            e->reg = e->left->reg;
            scratch_free(e->right->reg);
            break;
        case EXPR_MUL: {
            expr_codegen(e->left);
            expr_codegen(e->right);

            fprintf(output_file, "MOVQ %s, %%rax\n", scratch_name(e->left->reg));
            fprintf(output_file, "IMULQ %s\n",       scratch_name(e->right->reg));
            fprintf(output_file, "MOVQ %%rax, %s\n", scratch_name(e->right->reg));

            e->reg = e->right->reg;
            scratch_free(e->left->reg);
        } break;
        case EXPR_DIV:
            expr_codegen(e->left);
            expr_codegen(e->right);

            fprintf(output_file, "MOVQ %s, %%rax\n", scratch_name(e->left->reg));
            fprintf(output_file, "CQO\n");
            fprintf(output_file, "IDIVQ %s\n", scratch_name(e->right->reg));
            fprintf(output_file, "MOVQ %%rax, %s\n", scratch_name(e->left->reg));

            e->reg = e->left->reg;
            scratch_free(e->right->reg);
            break;
        case EXPR_EXPONENT:
            printf("FIXME: codegen EXPR_EXPONENT unimplemented.\n");
            break;
        case EXPR_MODULO: {
            expr_codegen(e->left);
            expr_codegen(e->right);

            fprintf(output_file, "MOVQ %s, %%rax\n", scratch_name(e->left->reg));
            fprintf(output_file, "CQO\n");
            fprintf(output_file, "IDIVQ %s\n", scratch_name(e->right->reg));
            fprintf(output_file, "MOVQ %%rdx, %s\n", scratch_name(e->left->reg));

            e->reg = e->left->reg;
            scratch_free(e->right->reg);
        } break;
        case EXPR_NEGATE:
            expr_codegen(e->left);

            fprintf(output_file, "NEG %s\n", scratch_name(e->left->reg));

            e->reg = e->left->reg;
            break;

        // logical operations
        case EXPR_LOGICAL_OR: {
            // FIXME: probably a very inefficient implementation
            expr_codegen(e->left);
            expr_codegen(e->right);

            const char* label_1 = label_name(label_create());
            const char* label_2 = label_name(label_create());
            const char* end_label = label_name(label_create());

            fprintf(output_file, "CMP $0, %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "JE %s\n", label_1);
            fprintf(output_file, "MOV $1, %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "JMP %s\n", end_label);
            fprintf(output_file, "%s:\n", label_1);

            fprintf(output_file, "CMP $0, %s\n", scratch_name(e->right->reg));
            fprintf(output_file, "JE %s\n", label_2);
            fprintf(output_file, "MOV $1, %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "JMP %s\n", end_label);
            fprintf(output_file, "%s:\n", label_2);

            fprintf(output_file, "MOV $0, %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "%s:\n", end_label);

            e->reg = e->left->reg;
            scratch_free(e->right->reg);

            free((void*) label_1);
            free((void*) label_2);
            free((void*) end_label);
        } break;
        case EXPR_LOGICAL_AND: {
            // FIXME: probably a very inefficient implementation
            expr_codegen(e->left);
            expr_codegen(e->right);

            const char* label = label_name(label_create());
            const char* end_label = label_name(label_create());

            fprintf(output_file, "CMP $0, %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "JE %s\n", label);
            fprintf(output_file, "CMP $0, %s\n", scratch_name(e->right->reg));
            fprintf(output_file, "JE %s\n", label);
            fprintf(output_file, "MOVQ $1, %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "JMP %s\n", end_label);

            fprintf(output_file, "%s:\n", label);
            fprintf(output_file, "MOVQ $0, %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "%s:\n", end_label);

            e->reg = e->left->reg;
            scratch_free(e->right->reg);

            free((void*) label);
            free((void*) end_label);
        } break;
        case EXPR_LOGICAL_NOT: {
            expr_codegen(e->left);

            const char* top_label = label_name(label_create());
            const char* end_label = label_name(label_create());

            fprintf(output_file, "CMP $0, %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "JE %s\n", top_label);

            fprintf(output_file, "XOR %s, %s\n",
                    scratch_name(e->left->reg),
                    scratch_name(e->left->reg));
            fprintf(output_file, "JMP %s\n", end_label);
            fprintf(output_file, "%s:\n", top_label);

            fprintf(output_file, "MOVQ $1, %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "%s:\n", end_label);

            e->reg = e->left->reg;

            free((void*) top_label);
            free((void*) end_label);
        } break;

        // conditionals
        case EXPR_CMP_EQUAL:
        case EXPR_CMP_NOT_EQUAL:
        case EXPR_CMP_GT:
        case EXPR_CMP_GT_EQUAL:
        case EXPR_CMP_LT:
        case EXPR_CMP_LT_EQUAL: {
            expr_codegen(e->left);
            expr_codegen(e->right);

            const char* top_label = label_name(label_create());
            const char* end_label = label_name(label_create());

            fprintf(output_file, "CMP %s, %s\n",
                    scratch_name(e->right->reg),
                    scratch_name(e->left->reg));

            switch (e->kind) {
                case EXPR_CMP_EQUAL:
                    fprintf(output_file, "JE %s\n", top_label);
                    break;
                case EXPR_CMP_NOT_EQUAL:
                    fprintf(output_file, "JNE %s\n", top_label);
                    break;
                case EXPR_CMP_GT:
                    fprintf(output_file, "JG %s\n", top_label);
                    break;
                case EXPR_CMP_GT_EQUAL:
                    fprintf(output_file, "JGE %s\n", top_label);
                    break;
                case EXPR_CMP_LT:
                    fprintf(output_file, "JL %s\n", top_label);
                    break;
                case EXPR_CMP_LT_EQUAL:
                    fprintf(output_file, "JLE %s\n", top_label);
                    break;
                default:
                    // unreachable because of outer switch
                    break;
            }

            fprintf(output_file, "MOVQ $0, %s\n", scratch_name(e->right->reg));
            fprintf(output_file, "JMP %s\n", end_label);
            fprintf(output_file, "%s:\n", top_label);

            fprintf(output_file, "MOVQ $1, %s\n", scratch_name(e->right->reg));
            fprintf(output_file, "%s:\n", end_label);

            scratch_free(e->left->reg);
            e->reg = e->right->reg;

            free((void*) top_label);
            free((void*) end_label);
        } break;

        // assignments
        case EXPR_ASSIGN: {
            const char* symbol = symbol_codegen(e->left->symbol);

            expr_codegen(e->right);
            fprintf(output_file, "MOVQ %s, %s\n",
                    scratch_name(e->right->reg),
                    symbol);
            e->reg = e->right->reg;

            free((void*) symbol);
        } break;
        case EXPR_INCREMENT: {
            const char* symbol = symbol_codegen(e->left->symbol);

            expr_codegen(e->left);
            fprintf(output_file, "INC %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "MOVQ %s, %s\n",
                    scratch_name(e->left->reg),
                    symbol);
            e->reg = e->left->reg;

            free((void*) symbol);
        } break;
        case EXPR_DECREMENT: {
            const char* symbol = symbol_codegen(e->left->symbol);

            expr_codegen(e->left);
            fprintf(output_file, "DEC %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "MOVQ %s, %s\n",
                    scratch_name(e->left->reg),
                    symbol);
            e->reg = e->left->reg;

            free((void*) symbol);
        } break;

        // misc.
        case EXPR_CALL: {
            // load the arguments into registers/onto stack
            {
                Expr* current_arg = e->right;
                int arg_count = 0;
                // this puts a practical limit of 2048 for number of function
                // parameters. big whoop
                Expr* arg_stack[2048];
                for (; current_arg != NULL;
                    arg_count++, current_arg = current_arg->right
                ) {
                    arg_stack[arg_count] = current_arg;
                }

                for (int j = arg_count-1; j >= 0; j--) {
                    current_arg = arg_stack[j];
                    expr_codegen(current_arg);
                    //printf("current_arg->reg is %d\n", current_arg->reg);
                    fprintf(output_file,
                            "PUSHQ %s\n",
                            scratch_name(current_arg->reg));
                    scratch_free(current_arg->reg);
                }

                for (int i = 0;
                     i < (X64_NUM_ARGUMENT_REGISTERS < arg_count ? X64_NUM_ARGUMENT_REGISTERS : arg_count);
                     i++
                ) {
                    fprintf(output_file,
                            "POPQ %s\n",
                            argument_registers[i]);
                }
            }
            // zero floating point args
            fprintf(output_file, "XOR %%rax, %%rax\n\n");

            // save the caller-saved registers
            fprintf(output_file, "PUSHQ %%r10\n");
            fprintf(output_file, "PUSHQ %%r11\n");

            // call the function
            // e->left should always be set to an EXPR_NAME with the name of
            // the function being called
            assert(e->left && e->left->kind == EXPR_NAME);
            fprintf(output_file, "CALL %s\n", e->left->name);

            // restore the caller-saved registers
            fprintf(output_file, "POPQ %%r11\n");
            fprintf(output_file, "POPQ %%r10\n");

            // save the argument into a scratch register
            e->reg = scratch_alloc();
            fprintf(output_file,
                    "MOVQ %%rax, %s\n",
                    scratch_name(e->reg));
        } break;
        case EXPR_INIT_LIST:
            printf("FIXME: codegen EXPR_INIT_LIST unimplemented.\n");
            break;
        case EXPR_ARG:
            expr_codegen(e->left);
            e->reg = e->left->reg;
            break;
        case EXPR_SUBSCRIPT:
            // generate code for the index expression
            expr_codegen(e->right);

            int base_reg = scratch_alloc();
            const char* base_reg_name = scratch_name(base_reg);
            const char* index_reg_name = scratch_name(e->right->reg);

            // load address of the array
            fprintf(output_file, "LEAQ %s, %s\n",
                    symbol_codegen(e->left->symbol),
                    base_reg_name);
            
            // move the indexed value into a register
            fprintf(output_file, "MOVQ 0(%s, %s, 8), %s\n",
                    base_reg_name,
                    index_reg_name,
                    index_reg_name);

            e->reg = e->right->reg;
            // free scratch register and allocated string
            scratch_free(base_reg);
            //free((void*) base_reg_name);
            //free((void*) index_reg_name);
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
            const char* else_label = label_name(label_create());
            const char* done_label = label_name(label_create());

            // condition expr
            expr_codegen(s->expr);
            fprintf(output_file, "CMP $0, %s\n", scratch_name(s->expr->reg));
            scratch_free(s->expr->reg);
            fprintf(output_file, "JE %s\n", else_label);

            // if branch
            stmt_codegen(s->body);
            fprintf(output_file, "JMP %s\n", done_label);

            // else branch
            fprintf(output_file, "%s:\n", else_label);
            stmt_codegen(s->else_body);
            fprintf(output_file, "%s:\n", done_label);

            free((void*) else_label);
            free((void*) done_label);
        } break;
        case STMT_FOR: {
            const char* top_label  = label_name(label_create());
            const char* done_label = label_name(label_create());

            // init expr
            if (s->init_expr) {
                expr_codegen(s->init_expr);
                scratch_free(s->init_expr->reg);
            }

            fprintf(output_file, "%s:\n", top_label);

            // condition expr
            if (s->expr) {
                expr_codegen(s->expr);
                fprintf(output_file, "CMP $0, %s\n", scratch_name(s->expr->reg));
                scratch_free(s->expr->reg);
                fprintf(output_file, "JE %s\n", done_label);
            }

            // body
            stmt_codegen(s->body);

            // next expr
            if (s->next_expr) {
                expr_codegen(s->next_expr);
                scratch_free(s->next_expr->reg);
            }
            fprintf(output_file, "JMP %s\n", top_label);

            fprintf(output_file, "%s:\n", done_label);

            free((void*) top_label);
            free((void*) done_label);
        } break;
        case STMT_PRINT: {
            
            Expr* current_arg = s->expr;
            char format_string[4096];
            format_string[0] = '\0';
            int arg_count = 0;
            // FIXME: again. practical limit of 2048 on number of args
            Expr* arg_stack[2048];
            while (current_arg != NULL) {
                arg_stack[arg_count] = current_arg;
                const char* format_string_append = NULL;
                switch (current_arg->left->type->kind) {
                    case TYPE_CHAR:
                        format_string_append = "%c";
                        break;
                    case TYPE_INTEGER:
                        format_string_append = "%d";
                        break;
                    case TYPE_BOOLEAN:
                    case TYPE_STRING:
                    case TYPE_ARRAY:
                    case TYPE_FUNCTION:
                    default:
                        format_string_append = "%s";
                        break;
                }

                strcat(format_string, format_string_append);

                current_arg = current_arg->right;
                arg_count++;
            }

            for (int i = arg_count-1; i >= 0; i--) {
                current_arg = arg_stack[i];
                expr_codegen(current_arg->left);
                switch (current_arg->left->type->kind) {
                    case TYPE_BOOLEAN: {
                        const char* else_label = label_name(label_create());
                        const char* end_label  = label_name(label_create());

                        fprintf(output_file, "CMP $0, %s\n",
                                scratch_name(current_arg->left->reg));
                        fprintf(output_file, "JE %s\n",
                                else_label);

                        fprintf(output_file, "LEAQ .__STR_TRUE(%%rip), %s\n",
                                scratch_name(current_arg->left->reg));
                        fprintf(output_file, "JMP %s\n", end_label);

                        fprintf(output_file, "%s:\n", else_label);
                        fprintf(output_file, "LEAQ .__STR_FALSE(%%rip), %s\n",
                                scratch_name(current_arg->left->reg));

                        fprintf(output_file, "%s:\n", end_label);

                        free((void*) else_label);
                        free((void*) end_label);
                    } break;
                    case TYPE_CHAR:
                        break;
                    case TYPE_INTEGER:
                        break;
                    case TYPE_STRING:
                        break;
                    case TYPE_ARRAY:
                        fprintf(output_file, "LEAQ .__STR_ARRAY(%%rip), %s\n",
                                scratch_name(current_arg->left->reg));
                        break;
                    case TYPE_FUNCTION:
                        fprintf(output_file, "LEAQ .__STR_FUNCTION(%%rip), %s\n",
                                scratch_name(current_arg->left->reg));
                        break;
                    default:
                        break;
                }

                fprintf(output_file,
                        "PUSHQ %s\n",
                        scratch_name(current_arg->left->reg));

                scratch_free(current_arg->left->reg);
                current_arg = current_arg->right;
            }

            for (int i = 0;
                 i < (X64_NUM_ARGUMENT_REGISTERS-1 < arg_count
                      ? X64_NUM_ARGUMENT_REGISTERS-1 : arg_count);
                 i++
            ) {
                fprintf(output_file,
                        "POPQ %s\n",
                        argument_registers[i+1]);
            }

            const char* format_string_label = label_name(label_create());

            fprintf(output_file, ".data\n");
            fprintf(output_file, "%s:\n", format_string_label);
            fprintf(output_file, "\t.string \"%s\"\n", format_string);
            fprintf(output_file, ".text\n");

            fprintf(output_file, "LEAQ %s(%%rip), %s\n",
                    format_string_label, argument_registers[0]);

            fprintf(output_file, "XOR %%rax, %%rax\n");

            // save the caller-saved registers
            fprintf(output_file, "PUSHQ %%r10\n");
            fprintf(output_file, "PUSHQ %%r11\n");

            fprintf(output_file, "CALL printf@PLT\n");

            fprintf(output_file, "POPQ %%r11\n");
            fprintf(output_file, "POPQ %%r10\n");

            free((void*) format_string_label);
        } break;
        case STMT_RETURN:
            expr_codegen(s->expr);
            fprintf(output_file, "MOVQ %s, %%rax\n", scratch_name(s->expr->reg));
            fprintf(output_file, "JMP .%s_epilogue\n", s->function_name);
            scratch_free(s->expr->reg);
            break;
        case STMT_BLOCK:
            stmt_codegen(s->body);
            break;
    }

    fprintf(output_file, "\n");
    stmt_codegen(s->next);
}

void decl_codegen(Decl* d) {
    if (!d) return;

    switch (d->type->kind) {
        case TYPE_FUNCTION:
            // directives and label
            fprintf(output_file, ".text\n");
            fprintf(output_file, ".global %s\n", d->name);
            fprintf(output_file, "%s:\n", d->name);

            // ***********
            // ** Prologue
            // ***********
            // save the old base pointer and set the new one
            fprintf(output_file, "PUSHQ %%rbp\n");
            fprintf(output_file, "MOVQ %%rsp, %%rbp\n");

            // save arguments
            {
                ParamList* current = d->type->params;
                for (int i = 0; i < X64_NUM_ARGUMENT_REGISTERS && current != NULL;
                    i++, current = current->next
                ) {
                    fprintf(output_file,
                            "PUSHQ %s\n",
                            argument_registers[i]);
                }
            }

            // allocate space for local variables FIXME: incomplete
            if (d->local_var_count > 0) {
                fprintf(output_file,
                        "\nSUBQ $%d, %%rsp\n\n",
                        8 * d->local_var_count);
            }

            // save callee-saved registers
            fprintf(output_file, "PUSHQ %%rbx\n");
            fprintf(output_file, "PUSHQ %%r12\n");
            fprintf(output_file, "PUSHQ %%r13\n");
            fprintf(output_file, "PUSHQ %%r14\n");
            fprintf(output_file, "PUSHQ %%r15\n\n");
            
            // ***********
            // ** Body
            // ***********
            stmt_codegen(d->code);

            // ***********
            // ** Epilogue
            // ***********

            fprintf(output_file, ".%s_epilogue:\n", d->name);

            // restore callee-saved registers
            fprintf(output_file, "POPQ %%r15\n");
            fprintf(output_file, "POPQ %%r14\n");
            fprintf(output_file, "POPQ %%r13\n");
            fprintf(output_file, "POPQ %%r12\n");
            fprintf(output_file, "POPQ %%rbx\n");

            // reset stack pointer and recove base pointer
            fprintf(output_file, "MOVQ %%rbp, %%rsp\n");
            fprintf(output_file, "POPQ %%rbp\n");

            // return
            fprintf(output_file, "RET\n");
            break;
        case TYPE_ARRAY:
            // FIXME: incomplete
            printf("FIXME: codegen for TYPE_ARRAY unimplemented.\n");
            if (d->symbol->kind == SYMBOL_GLOBAL) {
                fprintf(output_file, ".global %s\n", d->symbol->name);
                fprintf(output_file, ".data\n");
                fprintf(output_file, "%s:\n", d->symbol->name);

                int size = -1;
                // should always be integer literal if it passes typechecking
                if (d->type->size_expr) {
                    size = d->type->size_expr->integer_value;
                }

                if (d->value) {
                    int init_list_length = 0;
                    Expr* element = d->value->right;

                    while (element != NULL) {
                        if (init_list_length >= size) {
                            break;
                        }

                        switch (element->left->type->kind) {
                            case TYPE_BOOLEAN:
                            case TYPE_CHAR:
                            case TYPE_INTEGER:
                                fprintf(output_file,
                                    "\t.quad %d\n",
                                    element->left->integer_value
                                );
                                break;
                            case TYPE_STRING:
                                printf("FIXME: Array of string is not implemented.\n");
                                break;
                            case TYPE_ARRAY:
                                printf("FIXME: Multi-dimensional arrays are not implemented.\n");
                                break;
                            default:
                                break;
                        }

                        element = element->right;
                        init_list_length++;
                    }

                    if (init_list_length < size) {
                        fprintf(
                            output_file,
                            "\t.zero %d\n",
                            (size - init_list_length) * 8
                        );
                    }
                } else {
                    fprintf(output_file, ".zero %d\n", size * 8);
                }
            } else {
                const char* label = label_name(label_create());

                free((void*) label);
            }
            break;
        case TYPE_STRING:
            if (d->symbol->kind == SYMBOL_GLOBAL) {
                const char* label = label_name(label_create());

                fprintf(output_file, ".global %s\n", d->symbol->name);
                fprintf(output_file, ".data\n");
                fprintf(output_file, "%s:\n", label);
                const char* init_value = "";
                if (d->value) {
                    init_value = d->value->string_literal;
                }
                fprintf(output_file, "\t.string \"%s\"\n", init_value);

                fprintf(output_file, "%s:\n", d->symbol->name);
                fprintf(output_file, "\t.quad %s\n", label);

                fprintf(output_file, ".text\n\n");

                free((void*) label);
            } else {
                int reg = scratch_alloc();
                const char* label = label_name(label_create());

                fprintf(output_file, ".data\n");
                fprintf(output_file, "%s:\n", label);
                const char* init_value = "";
                if (d->value) {
                    init_value = d->value->string_literal;
                }
                fprintf(output_file, "\t.string \"%s\"\n", init_value);

                fprintf(output_file, ".text\n\n");
                fprintf(output_file, "LEAQ %s(%%rip), %s\n",
                        label, scratch_name(reg));

                const char* symbol = symbol_codegen(d->symbol);
                fprintf(output_file, "MOVQ %s, %s\n",
                        scratch_name(reg), symbol);

                free((void*) label);
                free((void*) symbol);
            }
            break;
        case TYPE_BOOLEAN:
        case TYPE_CHAR:
        case TYPE_INTEGER:
            // if no initializer value was given, create one with a value of zero
            if (!d->value) {
                switch (d->type->kind) {
                    case TYPE_BOOLEAN:
                        d->value = expr_create_boolean_literal(0);
                        break;
                    case TYPE_CHAR:
                        d->value = expr_create_char_literal(0);
                        break;
                    case TYPE_INTEGER:
                        d->value = expr_create_integer_literal(0);
                        break;
                    default:
                        // unreachable due to the outer switch statement
                        break;
                }
                d->value = expr_create_integer_literal(0);
            }

            if (d->symbol->kind == SYMBOL_GLOBAL) {
                fprintf(output_file, ".data\n");
                fprintf(output_file, "%s:\n", d->symbol->name);
                int init_value = 0;
                if (d->value) {
                    init_value = d->value->integer_value;
                }
                fprintf(output_file, "\t.quad %d\n", init_value);
                fprintf(output_file, ".text\n\n");
            } else {
                const char* symbol = symbol_codegen(d->symbol);
                expr_codegen(d->value);
                //printf("d->value->reg is %d, d->symbol->name is %s\n", d->value, d->symbol->name);
                fprintf(output_file,
                    "MOVQ %s, %s\n",
                    scratch_name(d->value->reg), symbol
                );

                scratch_free(d->value->reg);
                free((void*) symbol);
            }
            break;
        case TYPE_VOID:
            printf("Error: cannot create variable of type void.\n");
            assert(0);
            break;
    }

    decl_codegen(d->next);
}

FILE* codegen(Decl* decl, const char* output_filename) {
    output_file = fopen(output_filename, "w+");

    fprintf(output_file, ".data\n");
    fprintf(output_file, ".__STR_TRUE:\n");
    fprintf(output_file, "\t.string \"true\"\n");
    fprintf(output_file, ".__STR_FALSE:\n");
    fprintf(output_file, "\t.string \"false\"\n");
    fprintf(output_file, ".__STR_ARRAY:\n");
    fprintf(output_file, "\t.string \"(T_ARRAY)\"\n");
    fprintf(output_file, ".__STR_FUNCTION:\n");
    fprintf(output_file, "\t.string \"(T_FUNCTION)\"\n");
    fprintf(output_file, ".text\n");

    decl_codegen(decl);

    return output_file;
}
