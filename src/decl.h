#ifndef DECL_H
#define DECL_H

struct decl {
    char* name;
    struct type* type;
    struct expr* value;
    struct stmt* code;
    struct decl* next;
    struct symbol* symbol;
};

struct decl* decl_create(
    char* name,
    struct type* type,
    struct expr* value,
    struct stmt* code,
    struct decl* next
);

void decl_print(struct decl* d);

#endif
