#ifndef SYMBOL_H
#define SYMBOL_H

// forward declare to break include cycle
typedef struct Type Type;

typedef enum {
    SYMBOL_LOCAL,
    SYMBOL_PARAM,
    SYMBOL_GLOBAL,
} Symbol_t;

typedef struct Symbol Symbol;

struct Symbol {
    Symbol_t kind;
    Type* type;
    char* name;
    int which;
};

Symbol* symbol_create(Symbol_t kind, Type* type, char* name);

void symbol_delete(Symbol* s);

#endif
