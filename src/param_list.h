#ifndef PARAM_LIST_H
#define PARAM_LIST_H

#include "symbol.h"

typedef struct ParamList ParamList;

struct ParamList {
    char* name;
    Type* type;
    ParamList* next;

    Symbol* symbol;
};

ParamList* param_list_create(
    char* name, Type* type, ParamList* next
);

void param_list_delete(ParamList* p);

void param_list_print(ParamList* p);

int param_list_equals(ParamList* a, ParamList* b);

int param_list_length(ParamList* p);

ParamList* param_list_copy(ParamList* p);

#endif
