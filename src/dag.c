#include <stdlib.h>

#include "dag.h"
#include "decl.h"
#include "expr.h"
#include "stmt.h"
#include "type.h"

DagNode* dag_node_create(
    DagNode_t kind,
    DagNode* left,
    DagNode* right
) {
    DagNode* d = malloc(sizeof(*d));
    d->kind = kind;
    d->left = left;
    d->right = right;
    return d;
}

void dag_node_delete(DagNode* d) {
    if (!d) return;

    dag_node_delete(d->left);
    dag_node_delete(d->right);
    free(d);
}

DagNode* dag_node_create_name(const char* name) {
    DagNode* d = dag_node_create(DAG_NAME, NULL, NULL);
    d->name = name;
    return d;
}

DagNode* dag_node_create_float(double float_value) {
    DagNode* d = dag_node_create(DAG_FLOAT_VALUE, NULL, NULL);
    d->float_value = float_value;
    return d;
}

DagNode* dag_node_create_integer(int integer_value) {
    DagNode* d = dag_node_create(DAG_INTEGER_VALUE, NULL, NULL);
    d->integer_value = integer_value;
    return d;
}

DagNode* decl_to_dag(Decl* d) {
    if (!d) return NULL;

    DagNode* name = dag_node_create_name(d->name);
    DagNode* expr = expr_to_dag(d->value);
    DagNode* dag = dag_node_create(DAG_ASSIGN, name, expr);

    return dag;
}

DagNode* stmt_to_dag(Stmt* s) {
    if (!s) return NULL;

    return NULL;
}

DagNode* expr_to_dag(Expr* e) {
    if (!e) return NULL;

    return NULL;
}
