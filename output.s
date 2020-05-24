.data
global_integer:
	.quad 12345
.text

.data
global_boolean:
	.quad 1
.text

.data
global_character:
	.quad 120
.text

.data
global_string:
	.string "Hello, World!"
.text

.text
.global func_many_args
func_many_args:
PUSHQ %rbp
MOVQ %rsp, %rbp
PUSHQ %rdi
PUSHQ %rsi
PUSHQ %rdx
PUSHQ %rcx
PUSHQ %r8
PUSHQ %r9
PUSHQ 16(%rbp)
PUSHQ 24(%rbp)
PUSHQ 32(%rbp)
PUSHQ 40(%rbp)

SUBQ $8, %rsp

PUSHQ %rbx
PUSHQ %r12
PUSHQ %r13
PUSHQ %r14
PUSHQ %r15

MOVQ 40(%rbp), %rbx
MOVQ $1, %r10
ADDQ %rbx, %r10
MOVQ %r10, -8(%rbp)

MOVQ %rdi, %rbx
MOVQ %rsi, %r10
ADDQ %rbx, %r10
MOVQ %rdx, %rbx
ADDQ %r10, %rbx
MOVQ %rcx, %r10
ADDQ %rbx, %r10
MOVQ %r8, %rbx
ADDQ %r10, %rbx
MOVQ %r9, %r10
ADDQ %rbx, %r10
MOVQ 16(%rbp), %rbx
ADDQ %r10, %rbx
MOVQ 24(%rbp), %r10
ADDQ %rbx, %r10
MOVQ 32(%rbp), %rbx
ADDQ %r10, %rbx
MOVQ 40(%rbp), %r10
ADDQ %rbx, %r10
MOVQ -8(%rbp), %rbx
ADDQ %r10, %rbx
MOVQ %rbx, %rax
JMP .func_many_args_epilogue


.func_many_args_epilogue:
POPQ %r15
POPQ %r14
POPQ %r13
POPQ %r12
POPQ %rbx
MOVQ %rbp, %rsp
POPQ %rbp
RET
.text
.global main
main:
PUSHQ %rbp
MOVQ %rsp, %rbp

SUBQ $72, %rsp

PUSHQ %rbx
PUSHQ %r12
PUSHQ %r13
PUSHQ %r14
PUSHQ %r15

MOVQ $54321, %rbx
MOVQ %rbx, -8(%rbp)

MOVQ $0, %rbx
MOVQ %rbx, -16(%rbp)

MOVQ $89, %rbx
MOVQ %rbx, -24(%rbp)

MOVQ $1, %rbx
MOVQ $1, %r10
ADDQ %rbx, %r10
MOVQ %r10, -32(%rbp)

MOVQ $10, %rbx
MOVQ -32(%rbp), %r10
SUBQ %rbx, %r10
MOVQ %r10, -40(%rbp)

MOVQ -40(%rbp), %rbx
MOVQ -32(%rbp), %r10
MOVQ %rbx, %rax
IMULQ %r10
MOVQ %rax, %r10
MOVQ %r10, -48(%rbp)

MOVQ -48(%rbp), %rbx
MOVQ -48(%rbp), %r10
MOVQ %r10, %rax
CQO
IDIVQ %rbx
MOVQ %rax, %r10
MOVQ %r10, -56(%rbp)

MOVQ $10, %rbx
MOVQ -48(%rbp), %r10
MOVQ %r10, %rax
CQO
IDIVQ %rbx
MOVQ %rdx, %r10
MOVQ %r10, -64(%rbp)

MOVQ $1, %rbx
MOVQ %rbx, -8(%rbp)

MOVQ $1, %rbx
MOVQ %rbx, -16(%rbp)

MOVQ $33, %rbx
MOVQ %rbx, -24(%rbp)

MOVQ $10, %rbx
PUSHQ %rbx
MOVQ $9, %rbx
PUSHQ %rbx
MOVQ $8, %rbx
PUSHQ %rbx
MOVQ $7, %rbx
PUSHQ %rbx
MOVQ $6, %rbx
MOVQ %rbx, %r9
MOVQ $5, %rbx
MOVQ %rbx, %r8
MOVQ $4, %rbx
MOVQ %rbx, %rcx
MOVQ $3, %rbx
MOVQ %rbx, %rdx
MOVQ $2, %rbx
MOVQ %rbx, %rsi
MOVQ $1, %rbx
MOVQ %rbx, %rdi
XOR %rax, %rax

PUSHQ %r10
PUSHQ %r11
CALL func_many_args
POPQ %r11
POPQ %r10
MOVQ %rax, %rbx
MOVQ %rbx, -72(%rbp)


.main_epilogue:
POPQ %r15
POPQ %r14
POPQ %r13
POPQ %r12
POPQ %rbx
MOVQ %rbp, %rsp
POPQ %rbp
RET
