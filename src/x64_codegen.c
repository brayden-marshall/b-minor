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
        name = s->name;
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
        case EXPR_NAME:
            e->reg = scratch_alloc();
            fprintf(output_file, "MOVQ %s, %s\n",
                symbol_codegen(e->symbol),
                scratch_name(e->reg));
            break;
        // literals
        case EXPR_STRING_LITERAL: {
            // .data
            // .<label>:
            //     .str <value>
            // .text
            fprintf(output_file, ".data\n");

            int str_label = label_create();
            fprintf(output_file, "%s:\n", label_name(str_label));
            fprintf(output_file, "\t.string \"%s\"\n", e->string_literal);
            fprintf(output_file, ".text\n");

            e->reg = scratch_alloc();
            fprintf(output_file, "LEAQ %s(%%rip), %s\n",
                    label_name(str_label),
                    scratch_name(e->reg));
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
            // ADDQ left, right
            expr_codegen(e->left);
            expr_codegen(e->right);

            fprintf(output_file, "ADDQ %s, %s\n",
                scratch_name(e->left->reg),
                scratch_name(e->right->reg));

            e->reg = e->right->reg;
            scratch_free(e->left->reg);
            break;
        case EXPR_SUB:
            // SUBQ left, right
            expr_codegen(e->left);
            expr_codegen(e->right);

            fprintf(output_file, "SUBQ %s, %s\n",
                scratch_name(e->right->reg),
                scratch_name(e->left->reg));

            e->reg = e->left->reg;
            scratch_free(e->right->reg);
            break;
        case EXPR_MUL: {
            // MOVQ left, %rax
            // IMULQ right
            // MOVQ %rax, right
            expr_codegen(e->left);
            expr_codegen(e->right);

            fprintf(output_file, "MOVQ %s, %%rax\n", scratch_name(e->left->reg));
            fprintf(output_file, "IMULQ %s\n",       scratch_name(e->right->reg));
            fprintf(output_file, "MOVQ %%rax, %s\n", scratch_name(e->right->reg));

            e->reg = e->right->reg;
            scratch_free(e->left->reg);
        } break;
        case EXPR_DIV:
            // MOVQ right, %rax
            // CQO
            // IDIVQ left
            // MOVQ %rax, right
            expr_codegen(e->left);
            expr_codegen(e->right);

            fprintf(output_file, "MOVQ %s, %%rax\n", scratch_name(e->right->reg));
            fprintf(output_file, "CQO\n");
            fprintf(output_file, "IDIVQ %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "MOVQ %%rax, %s\n", scratch_name(e->right->reg));

            e->reg = e->right->reg;
            scratch_free(e->left->reg);
            break;
        case EXPR_EXPONENT:
            printf("FIXME: codegen EXPR_EXPONENT unimplemented.\n");
            break;
        case EXPR_MODULO: {
            // MOVQ right, %rax
            // CQO
            // IDIVQ left
            // MOVQ %rdx, right
            expr_codegen(e->left);
            expr_codegen(e->right);

            fprintf(output_file, "MOVQ %s, %%rax\n", scratch_name(e->right->reg));
            fprintf(output_file, "CQO\n");
            fprintf(output_file, "IDIVQ %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "MOVQ %%rdx, %s\n", scratch_name(e->right->reg));

            e->reg = e->right->reg;
            scratch_free(e->left->reg);
        } break;
        case EXPR_NEGATE:
            expr_codegen(e->left);

            fprintf(output_file, "NEG %s\n", scratch_name(e->left->reg));

            e->reg = e->left->reg;
            break;

        // logical operations
        case EXPR_LOGICAL_OR:
            printf("FIXME: codegen EXPR_LOGICAL_OR unimplemented.\n");
            break;
        case EXPR_LOGICAL_AND:
            printf("FIXME: codegen EXPR_LOGICAL_AND unimplemented.\n");
            break;
        case EXPR_LOGICAL_NOT: {
            expr_codegen(e->left);

            int top_label = label_create();
            int end_label = label_create();

            fprintf(output_file, "CMP $0, %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "JE %s\n", label_name(top_label));

            fprintf(output_file, "XOR %s, %s\n",
                    scratch_name(e->left->reg),
                    scratch_name(e->left->reg));
            fprintf(output_file, "JMP %s\n", label_name(end_label));
            fprintf(output_file, "%s:\n", label_name(top_label));

            fprintf(output_file, "MOVQ $1, %s\n", scratch_name(e->left->reg));
            fprintf(output_file, "%s:\n", label_name(end_label));

            e->reg = e->left->reg;
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

            int top_label = label_create();
            int end_label = label_create();

            fprintf(output_file, "CMP %s, %s\n",
                    scratch_name(e->right->reg),
                    scratch_name(e->left->reg));

            switch (e->kind) {
                case EXPR_CMP_EQUAL:
                    fprintf(output_file, "JE %s\n", label_name(top_label));
                    break;
                case EXPR_CMP_NOT_EQUAL:
                    fprintf(output_file, "JNE %s\n", label_name(top_label));
                    break;
                case EXPR_CMP_GT:
                    fprintf(output_file, "JG %s\n", label_name(top_label));
                    break;
                case EXPR_CMP_GT_EQUAL:
                    fprintf(output_file, "JGE %s\n", label_name(top_label));
                    break;
                case EXPR_CMP_LT:
                    fprintf(output_file, "JL %s\n", label_name(top_label));
                    break;
                case EXPR_CMP_LT_EQUAL:
                    fprintf(output_file, "JLE %s\n", label_name(top_label));
                    break;
                default:
                    // unreachable because of outer switch
                    break;
            }
            fprintf(output_file, "MOVQ $0, %s\n", scratch_name(e->right->reg));
            fprintf(output_file, "JMP %s\n", label_name(end_label));
            fprintf(output_file, "%s:\n", label_name(top_label));

            fprintf(output_file, "MOVQ $1, %s\n", scratch_name(e->right->reg));
            fprintf(output_file, "%s:\n", label_name(end_label));

            scratch_free(e->left->reg);
            e->reg = e->right->reg;
        } break;

        // assignments
        case EXPR_ASSIGN:
            expr_codegen(e->right);
            fprintf(output_file, "MOVQ %s, %s\n",
                    scratch_name(e->right->reg),
                    symbol_codegen(e->left->symbol));
            e->reg = e->right->reg;
            break;
        case EXPR_INCREMENT:
            printf("FIXME: codegen EXPR_INCREMENT unimplemented.\n");
            break;
        case EXPR_DECREMENT:
            printf("FIXME: codegen EXPR_DECREMENT unimplemented.\n");
            break;

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
            printf("FIXME: codegen EXPR_SUBSCRIPT unimplemented.\n");
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
            fprintf(output_file, "CMP $0, %s\n", scratch_name(s->expr->reg));
            scratch_free(s->expr->reg);
            fprintf(output_file, "JE %s\n", label_name(else_label));

            // if branch
            stmt_codegen(s->body);
            fprintf(output_file, "JMP %s\n", label_name(done_label));

            // else branch
            fprintf(output_file, "%s:\n", label_name(else_label));
            stmt_codegen(s->else_body);
            fprintf(output_file, "%s:\n", label_name(done_label));
        } break;
        case STMT_FOR: {
            int top_label = label_create();
            int done_label = label_create();

            // init expr
            if (s->init_expr) {
                expr_codegen(s->init_expr);
                scratch_free(s->init_expr->reg);
            }

            fprintf(output_file, "%s:\n", label_name(top_label));

            // condition expr
            if (s->expr) {
                expr_codegen(s->expr);
                fprintf(output_file, "CMP $0, %s\n", scratch_name(s->expr->reg));
                scratch_free(s->expr->reg);
                fprintf(output_file, "JE %s\n", label_name(done_label));
            }

            // body
            stmt_codegen(s->body);

            // next expr
            if (s->next_expr) {
                expr_codegen(s->next_expr);
                scratch_free(s->next_expr->reg);
            }
            fprintf(output_file, "JMP %s\n", label_name(top_label));

            fprintf(output_file, "%s:\n", label_name(done_label));
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
                        int else_label = label_create();
                        int end_label = label_create();
                        fprintf(output_file, "CMP $0, %s\n",
                                scratch_name(current_arg->left->reg));
                        fprintf(output_file, "JE %s\n",
                                label_name(else_label));

                        fprintf(output_file, "LEAQ .__STR_TRUE(%%rip), %s\n",
                                scratch_name(current_arg->left->reg));
                        fprintf(output_file, "JMP %s\n", label_name(end_label));

                        fprintf(output_file, "%s:\n", label_name(else_label));
                        fprintf(output_file, "LEAQ .__STR_FALSE(%%rip), %s\n",
                                scratch_name(current_arg->left->reg));

                        fprintf(output_file, "%s:\n", label_name(end_label));
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

            fprintf(output_file, ".data\n");
            int format_string_label = label_create();
            fprintf(output_file, "%s:\n", label_name(format_string_label));
            fprintf(output_file, "\t.string \"%s\"\n", format_string);
            fprintf(output_file, ".text\n");

            fprintf(output_file, "LEAQ %s(%%rip), %s\n",
                    label_name(format_string_label), argument_registers[0]);

            fprintf(output_file, "XOR %%rax, %%rax\n");

            // save the caller-saved registers
            fprintf(output_file, "PUSHQ %%r10\n");
            fprintf(output_file, "PUSHQ %%r11\n");

            fprintf(output_file, "CALL printf@PLT\n");

            fprintf(output_file, "POPQ %%r11\n");
            fprintf(output_file, "POPQ %%r10\n");
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
            fprintf(output_file, "FIXME: codegen for TYPE_ARRAY unimplemented.\n");
            break;
        case TYPE_STRING:
            if (d->symbol->kind == SYMBOL_GLOBAL) {
                fprintf(output_file, ".data\n");
                fprintf(output_file, "%s:\n", symbol_codegen(d->symbol));
                const char* init_value = "";
                if (d->value) {
                    init_value = d->value->string_literal;
                }
                fprintf(output_file, "\t.string \"%s\"\n", init_value);
                fprintf(output_file, ".text\n\n");
            } else {
                int reg = scratch_alloc();
                int label = label_create();

                fprintf(output_file, ".data\n");
                fprintf(output_file, "%s:\n", label_name(label));
                const char* init_value = "";
                if (d->value) {
                    init_value = d->value->string_literal;
                }
                fprintf(output_file, "\t.string \"%s\"\n", init_value);

                fprintf(output_file, ".text\n\n");
                fprintf(output_file, "LEAQ %s(%%rip), %s\n",
                        label_name(label), scratch_name(reg));
                fprintf(output_file, "MOVQ %s, %s\n",
                        scratch_name(reg), symbol_codegen(d->symbol));
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
                fprintf(output_file, "%s:\n", symbol_codegen(d->symbol));
                int init_value = 0;
                if (d->value) {
                    init_value = d->value->integer_value;
                }
                fprintf(output_file, "\t.quad %d\n", init_value);
                fprintf(output_file, ".text\n\n");
            } else {
                expr_codegen(d->value);
                //printf("d->value->reg is %d, d->symbol->name is %s\n", d->value, d->symbol->name);
                fprintf(output_file,
                    "MOVQ %s, %s\n",
                    scratch_name(d->value->reg), symbol_codegen(d->symbol)
                );
                scratch_free(d->value->reg);
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
