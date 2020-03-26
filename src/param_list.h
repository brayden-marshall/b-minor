#ifndef PARAM_LIST_H
#define PARAM_LIST_H

#include "symbol.h"

struct param_list {
    char* name;
    struct type* type;
    struct param_list* next;

    struct symbol* symbol;
};

struct param_list* param_list_create(
    char* name, struct type* type, struct param_list* next
);

void param_list_delete(struct param_list* p);

void param_list_print(struct param_list* p);

int param_list_equals(struct param_list* a, struct param_list* b);

struct param_list* param_list_copy(struct param_list* p);

#endif
