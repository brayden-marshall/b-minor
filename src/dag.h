#ifndef DAG_H
#define DAG_H

#include "decl.h"
#include "expr.h"
#include "stmt.h"

typedef enum {
    DAG_ASSIGN,
    DAG_DEREF,
    DAG_IADD,
    DAG_IMUL,
    DAG_NAME,
    DAG_FLOAT_VALUE,
    DAG_INTEGER_VALUE,
} DagNode_t;

typedef struct DagNode DagNode;

struct DagNode {
    DagNode_t kind;
    DagNode* left;
    DagNode* right;
    union {
        const char* name;
        double float_value;
        int integer_value;
    };
};

DagNode* dag_node_create(
    DagNode_t kind,
    DagNode* left,
    DagNode* right
);

void dag_node_delete(DagNode* d);

DagNode* dag_node_create_name(const char* name);

DagNode* dag_node_create_float(double float_value);

DagNode* dag_node_create_integer(int integer_value);

DagNode* decl_to_dag(Decl* d);

DagNode* stmt_to_dag(Stmt* s);

DagNode* expr_to_dag(Expr* e);

#endif
