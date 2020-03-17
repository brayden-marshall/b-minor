#include <stdio.h>
#include <stdlib.h>

#include "expr.h"
#include "decl.h"
#include "stmt.h"
#include "type.h"
#include "param_list.h"

#include "parser.h"

extern int yyparse();
extern struct decl* parser_result;

extern int yylex();
extern FILE* yyin;

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Require one argument: filename.\n");
        return EXIT_FAILURE;
    }

    // print out token values
    //enum yytokentype t;
    //while ((t = yylex())) {
    //    printf("%d\n", t);
    //}
    
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        printf("Could not open file '%s'.", argv[2]);
        return EXIT_FAILURE;
    }
    
    if (yyparse() == 0) {
        printf("Parse successful!\n");
        printf("Result is:\n");
        if (parser_result) {
            struct decl* current = parser_result;
            while (current != NULL) {
                printf("\n");
                printf("\tname: %s\t", current->name);
                if (current->type) {
                    printf("kind: %d\n", current->type->kind);
                }

                printf("\t");
                expr_print(current->value);
                if (current->value) {
                }
                printf("\n");

                current = current->next;
            }
        } else {
            printf("null\n");
        }
    } else {
        printf("Parse failed!\n");
    }

    fclose(yyin);

    return EXIT_SUCCESS;
}
