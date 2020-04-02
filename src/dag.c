#include <stdlib.h>

#include "dag.h"

struct dag_node* dag_node_create(
    dag_kind_t kind,
    struct dag_node* left,
    struct dag_node* right
) {
    struct dag_node* d = malloc(sizeof(*d));
    d->kind = kind;
    d->left = left;
    d->right = right;
    return d;
}

void dag_node_delete(struct dag_node* d) {
    if (!d) return;

    dag_node_delete(d->left);
    dag_node_delete(d->right);
    free(d);
    d = NULL;
}

struct dag_node* dag_node_create_name(const char* name) {
    struct dag_node* d = dag_node_create(DAG_NAME, NULL, NULL);
    d->name = name;
    return d;
}

struct dag_node* dag_node_create_float(double float_value) {
    struct dag_node* d = dag_node_create(DAG_FLOAT_VALUE, NULL, NULL);
    d->float_value = float_value;
    return d;
}

struct dag_node* dag_node_create_integer(int integer_value) {
    struct dag_node* d = dag_node_create(DAG_INTEGER_VALUE, NULL, NULL);
    d->integer_value = integer_value;
    return d;
}
