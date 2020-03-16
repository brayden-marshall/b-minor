#include <stdio.h>

#include "expr.h"
#include "decl.h"
#include "stmt.h"
#include "type.h"
#include "param_list.h"

extern int yyparse();
extern struct decl* parser_result;

int main() {
    printf("B-minor parser\n");
    printf(">> ");
    
    if (yyparse() == 0) {
        printf("Parse successful!\n");
    } else {
        printf("Parse failed!\n");
    }
}
