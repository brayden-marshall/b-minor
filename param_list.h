#ifndef PARAM_LIST_H
#define PARAM_LIST_H

struct param_list {
    char* name;
    struct type* type;
    struct param_list* next;
};

struct param_list* param_list_create(
    char* name, struct type* type, struct param_list* next
);

void param_list_print(struct param_list* p);

#endif
