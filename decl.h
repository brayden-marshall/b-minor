#ifndef DECL_H
#define DECL_H

struct decl {
    char* name;
    struct type* type;
    struct expr* value;
    struct stmt* code;
    struct decl* next;
};

struct decl* decl_create(
    char* name,
    struct type* type,
    struct expr* value,
    struct stmt* code,
    struct decl* next
);

#endif
