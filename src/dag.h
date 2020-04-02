#ifndef DAG_H
#define DAG_H

typedef enum {
    DAG_ASSIGN,
    DAG_DEREF,
    DAG_IADD,
    DAG_IMUL,
    DAG_NAME,
    DAG_FLOAT_VALUE,
    DAG_INTEGER_VALUE,
} dag_kind_t;

struct dag_node {
    dag_kind_t kind;
    struct dag_node* left;
    struct dag_node* right;
    union {
        const char* name;
        double float_value;
        int integer_value;
    };
};

struct dag_node* dag_node_create(
    dag_kind_t kind,
    struct dag_node* left,
    struct dag_node* right
);

void dag_node_delete(struct dag_node* d);

struct dag_node* dag_node_create_name(const char* name);

struct dag_node* dag_node_create_float(double float_value);

struct dag_node* dag_node_create_integer(int integer_value);

#endif
