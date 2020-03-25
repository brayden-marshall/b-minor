#include <stdlib.h>
#include <stdio.h>

#include "param_list.h"
#include "type.h"

struct param_list* param_list_create(
    char* name, struct type* type, struct param_list* next
) {
    struct param_list* p = malloc(sizeof(*p));
    p->name = name;
    p->type = type;
    p->next = next;
    p->symbol = NULL;
    return p;
}

void param_list_print(struct param_list* p) {
    if (!p) return;

    printf("%s: ", p->name);
    type_print(p->type);

    if (p->next) {
        printf(", ");
        param_list_print(p->next);
    }
}
