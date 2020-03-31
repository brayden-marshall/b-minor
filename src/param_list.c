#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "param_list.h"
#include "type.h"
#include "typecheck.h"

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

void param_list_delete(struct param_list* p) {
    if (!p) return;

    type_delete(p->type);
    free(p->name);
    param_list_delete(p->next);
    free(p);
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

int param_list_equals(struct param_list* a, struct param_list* b) {
    if (a == b) return 1;
    if (!a || !b) return 0;

    if (strcmp(a->name, b->name) != 0) {
        return 0;
    }

    if (!type_equals(a->type, b->type)) {
        return 0;
    }

    return param_list_equals(a->next, b->next);
}

struct param_list* param_list_copy(struct param_list* p) {
    if (!p) return 0;

    struct param_list* new_p = param_list_create(0, 0, 0);
    new_p->name = strdup(p->name);
    new_p->type = type_copy(p->type);
    new_p->next = param_list_copy(p->next);

    return new_p;
}
