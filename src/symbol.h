#ifndef SYMBOL_H
#define SYMBOL_H

typedef enum {
    SYMBOL_LOCAL,
    SYMBOL_PARAM,
    SYMBOL_GLOBAL,
} symbol_t;

struct symbol {
    symbol_t kind;
    struct type* type;
    char* name;
    int which;
};

struct symbol* symbol_create(symbol_t kind, struct type* type, char* name);

void symbol_delete(struct symbol* s);

#endif
