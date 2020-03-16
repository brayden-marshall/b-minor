#include <stdio.h>
#include <stdlib.h>

#include "expr.h"
#include "decl.h"
#include "stmt.h"
#include "type.h"
#include "param_list.h"

#include "parser.h"

extern int yyparse();
extern int yylex();
extern struct decl* parser_result;

int main() {
    printf("B-minor parser\n");
    printf(">> ");

    // print out token values
    //enum yytokentype t;
    //while ((t = yylex())) {
    //    printf("%d\n", t);
    //}
    
    if (yyparse() == 0) {
        printf("Parse successful!\n");
        printf("parser_result is:\n");
        if (parser_result) {
            printf("%s\n", parser_result->name);
            if (parser_result->type) {
                printf("%d\n", parser_result->type->kind);
            }
        } else {
            printf("null\n");
        }
    } else {
        printf("Parse failed!\n");
    }

    return EXIT_SUCCESS;
}
