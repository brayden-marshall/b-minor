%{
#include "decl.h"
#include "expr.h"
#include "stmt.h"
#include "type.h"
#include "param_list.h"

int yyerror(char* str);

struct decl* parser_result;

%}

// keywords
%token TOKEN_ARRAY
%token TOKEN_BOOLEAN
%token TOKEN_CHAR
%token TOKEN_ELSE
%token TOKEN_FALSE
%token TOKEN_FOR
%token TOKEN_FUNCTION
%token TOKEN_IF
%token TOKEN_INTEGER
%token TOKEN_PRINT
%token TOKEN_RETURN
%token TOKEN_STRING
%token TOKEN_TRUE
%token TOKEN_VOID
%token TOKEN_WHILE

// punctuation
%token TOKEN_COLON
%token TOKEN_SEMI

// brackets
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_LBRACKET
%token TOKEN_RBRACKET
%token TOKEN_LBRACE
%token TOKEN_RBRACE

// operators
%token TOKEN_ASSIGN
%token TOKEN_PLUS
%token TOKEN_MINUS
%token TOKEN_MULTIPLY
%token TOKEN_DIVIDE

// literals
%token TOKEN_BOOLEAN_LITERAL
%token TOKEN_INTEGER_LITERAL
%token TOKEN_CHAR_LITERAL
%token TOKEN_STRING_LITERAL

%union {
    struct decl* decl;
    struct stmt* stmt;
    struct expr* expr;
    struct type* type;

    /* possibly more? */

    char* name;
}

%type <decl> program decl_list decl /* more */
%type <stmt> stmt_list stmt /* more */
%type <expr> expr /* more */
%type <type> type /* more */

/* more types */

%type <name> name

%%

program : decl_list
          { parser_result = $1; }
        ;

decl : name TOKEN_COLON type TOKEN_SEMI
       { $$ = decl_create($1, $3, 0, 0, 0); }
     | name TOKEN_COLON type TOKEN_ASSIGN expr TOKEN_SEMI
       { $$ = decl_create($1, $3, $5, 0, 0); }
     | /* more cases here */

     ;


decl_list : decl decl_list
            { $$ = $1; $1->next = $2; }
          | /* epsilon */
            { $$ = 0; }
          ;

name : /* epsilon */
     ;

stmt : TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt
       { $$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, $5, 0, 0); }
     | TOKEN_LBRACE stmt_list TOKEN_RBRACE
       { $$ = stmt_create(STMT_BLOCK, 0, 0, 0, 0, $2, 0, 0); }
     | /* more cases here */

     ;

stmt_list : stmt stmt_list
            { $$ = $1; $1->next = $2; }
          ;

expr : /* epsilon */
     ;

type : TOKEN_INTEGER
       { $$ = type_create(TYPE_INTEGER); }
     | TOKEN_BOOLEAN
       { $$ = type_create(TYPE_BOOLEAN); }
     | TOKEN_CHAR
       { $$ = type_create(TYPE_CHAR); }
     | TOKEN_STRING
       { $$ = type_create(TYPE_STRING); }
     | TOKEN_VOID
       { $$ = type_create(TYPE_VOID); }
     | TOKEN_ARRAY TOKEN_LBRACKET TOKEN_RBRACKET type
       { $$ = type_create_array($4); }
     ;

%%

int yyerror(char* str) {
    printf("parse error: %s\n", str);
    return 0;
}
