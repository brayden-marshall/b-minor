// keywords
%token TOKEN_ARRAY
%token TOKEN_BOOLEAN
%token TOKEN_CHAR
%token TOKEN_ELSE
%token TOKEN_FOR
%token TOKEN_FUNCTION
%token TOKEN_IF
%token TOKEN_INTEGER
%token TOKEN_PRINT
%token TOKEN_RETURN
%token TOKEN_STRING
%token TOKEN_VOID
%token TOKEN_WHILE

// punctuation
%token TOKEN_COLON
%token TOKEN_SEMI
%token TOKEN_COMMA

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
%token TOKEN_INTEGER_LITERAL
%token TOKEN_CHAR_LITERAL
%token TOKEN_STRING_LITERAL
%token TOKEN_TRUE
%token TOKEN_FALSE

// comments
//%token TOKEN_BLOCK_COMMENT
//%token TOKEN_LINE_COMMENT

// other
%token TOKEN_IDENT

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decl.h"
#include "expr.h"
#include "stmt.h"
#include "type.h"
#include "param_list.h"

extern char *yytext;
extern int yylex();
extern int yyerror(char *str);

FILE* yyin;

struct decl* parser_result;

struct decl* decl_list_last = NULL;

%}

%union {
    struct decl* decl;
    struct stmt* stmt;
    struct expr* expr;
    struct type* type;
    struct param_list* param_list;
    char* ident;
}

%type <decl> program decl_list decl
%type <stmt> stmt_list stmt for_expr
%type <expr> expr term factor maybe_expr print_args
%type <type> type atomic_type
%type <param_list> param_list param
%type <ident> ident

%%

program : decl_list
          { parser_result = $1; return 0; }
        ;

decl : ident TOKEN_COLON type TOKEN_SEMI
       { $$ = decl_create($1, $3, 0, 0, 0); }
     | ident TOKEN_COLON type TOKEN_ASSIGN expr TOKEN_SEMI
       { $$ = decl_create($1, $3, $5, 0, 0); }
     | ident TOKEN_COLON TOKEN_FUNCTION atomic_type
       TOKEN_LPAREN param_list TOKEN_RPAREN TOKEN_ASSIGN stmt
       {
           $$ = decl_create($1, type_create_function($4, $6), 0, $9, 0);
       }
     | ident TOKEN_COLON TOKEN_FUNCTION atomic_type
       TOKEN_LPAREN param_list TOKEN_RPAREN TOKEN_SEMI
       { $$ = decl_create($1, type_create_function($4, $6), 0, 0, 0); }
     ;

// FIXME: change this to use left-recursion (bison handles left-recursion better than right-recursion)
param_list : param
             { $$ = $1; }
           | param TOKEN_COMMA param_list
             { $$ = $1; $1->next = $3; }
           | /* epsilon */
             { $$ = NULL; }
           ;

param : ident TOKEN_COLON type
        { $$ = param_list_create($1, $3, 0); }
      ;

// FIXME: change this to use left-recursion (bison handles left-recursion better than right-recursion)
decl_list : decl decl_list
            { $$ = $1; $1->next = $2; }
          | /* epsilon */
            { $$ = NULL; }
          ;

ident : TOKEN_IDENT
       { $$ = strdup(yytext); }
     ;

stmt : TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt
       { $$ = stmt_create_if_else($3, $5, 0); }
     // FIXME: implement if-else statements with no conflicts
     //| TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt TOKEN_ELSE stmt
     //  { $$ = stmt_create_if_else($3, $5, 0); }
     | TOKEN_FOR TOKEN_LPAREN for_expr TOKEN_RPAREN stmt
       { $$ = $3; $$->body = $5; }
     | TOKEN_RETURN expr TOKEN_SEMI
       { $$ = stmt_create_return($2); }
     | TOKEN_PRINT print_args TOKEN_SEMI
       { $$ = stmt_create_print($2); }
     | TOKEN_LBRACE stmt_list TOKEN_RBRACE
       { $$ = stmt_create_block($2); }
     | TOKEN_SEMI
       { $$ = NULL; }
     ;

print_args : expr
             { $$ = expr_create_arg($1, 0); }
           | expr TOKEN_COMMA print_args
             { $$ = expr_create_arg($1, $3); }
           | /* epsilon */
             { $$ = NULL; }
           ;

for_expr : maybe_expr TOKEN_SEMI maybe_expr TOKEN_SEMI maybe_expr
           { $$ = stmt_create_for($1, $3, $5, 0); }
         ;

maybe_expr : expr
             { $$ = $1; }
           | /* epsilon */
             { $$ = NULL; }
           ;

// FIXME: change this to use left-recursion (bison handles left-recursion better than right-recursion)
stmt_list : stmt stmt_list
            { $$ = $1; $1->next = $2; }
          | /* epsilon */
            { $$ = NULL; }
          ;

expr : expr TOKEN_PLUS term
       { $$ = expr_create(EXPR_ADD, $1, $3); }
     | expr TOKEN_MINUS term
       { $$ = expr_create(EXPR_SUB, $1, $3); }
     | term
       { $$ = $1; }
     ;

term : term TOKEN_MULTIPLY factor
       { $$ = expr_create(EXPR_MUL, $1, $3); }
     | term TOKEN_DIVIDE factor
       { $$ = expr_create(EXPR_DIV, $1, $3); }
     | factor
       { $$ = $1; }
     ;

factor : TOKEN_LPAREN expr TOKEN_RPAREN
         { $$ = $2; }
       | TOKEN_MINUS factor
         { $$ = expr_create(EXPR_SUB, expr_create_integer_literal(0), $2); }
       | ident
         { $$ = expr_create_name($1); }
       | TOKEN_INTEGER_LITERAL
         { $$ = expr_create_integer_literal(atoi(yytext)); } 
       | TOKEN_CHAR_LITERAL
         {
             // text[1] because first and last character are (')
             $$ = expr_create_char_literal(yytext[1]);
         }
       | TOKEN_STRING_LITERAL
         {
             // remove leading and trailing quotes
             char* text = strdup(yytext+1);
             text[strlen(text)-1] = '\0';
             $$ = expr_create_string_literal(text);
         }
       | TOKEN_TRUE
         { $$ = expr_create_boolean_literal(1); }
       | TOKEN_FALSE
         { $$ = expr_create_boolean_literal(0); }
       ;

type : atomic_type
       { $$ = $1; }
     | TOKEN_ARRAY TOKEN_LBRACKET TOKEN_RBRACKET type
       { $$ = type_create_array($4, 0); }
     | TOKEN_ARRAY TOKEN_LBRACKET expr TOKEN_RBRACKET type
       { $$ = type_create_array($5, $3); }
     ;

atomic_type : TOKEN_INTEGER
              { $$ = type_create(TYPE_INTEGER); }
            | TOKEN_BOOLEAN
              { $$ = type_create(TYPE_BOOLEAN); }
            | TOKEN_CHAR
              { $$ = type_create(TYPE_CHAR); }
            | TOKEN_STRING
              { $$ = type_create(TYPE_STRING); }
            | TOKEN_VOID
              { $$ = type_create(TYPE_VOID); }
            ;

%%

int yyerror(char* str) {
    printf("parse error: %s\n", str);
    return 0;
}
