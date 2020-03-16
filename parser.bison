// keywords
%token TOKEN_KEYWORD_ARRAY
%token TOKEN_KEYWORD_BOOLEAN
%token TOKEN_KEYWORD_CHAR
%token TOKEN_KEYWORD_ELSE
%token TOKEN_KEYWORD_FALSE
%token TOKEN_KEYWORD_FOR
%token TOKEN_KEYWORD_FUNCTION
%token TOKEN_KEYWORD_IF
%token TOKEN_KEYWORD_INTEGER
%token TOKEN_KEYWORD_PRINT
%token TOKEN_KEYWORD_RETURN
%token TOKEN_KEYWORD_STRING
%token TOKEN_KEYWORD_TRUE
%token TOKEN_KEYWORD_VOID
%token TOKEN_KEYWORD_WHILE

// punctuation
%token TOKEN_COLON
%token TOKEN_SEMI
%token TOKEN_ASSIGN

// brackets
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_LBRACKET
%token TOKEN_RBRACKET
%token TOKEN_LBRACE
%token TOKEN_RBRACE

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
            { $$ = $1; %1->next = $2; }
          | /* epsilon */
            { $$ = 0; }
          ;

stmt : TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt
       { $$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, $5, 0, 0); }
     | TOKEN_LBRACE stmt_list TOKEN_RBRACE
       { $$ = stmt_create(STMT_BLOCK, 0, 0, 0, 0, $2, 0, 0); }
     | /* more cases here */

     ;

type : TOKEN_KEYWORD_INTEGER
       { $$ = type_create(TYPE_INTEGER); }
     | TOKEN_KEYWORD_BOOLEAN
       { $$ = type_create(TYPE_BOOLEAN); }
     | TOKEN_KEYWORD_CHAR
       { $$ = type_create(TYPE_CHAR); }
     | TOKEN_KEYWORD_STRING
       { $$ = type_create(TYPE_STRING); }
     | TOKEN_KEYWORD_VOID
       { $$ = type_create(TYPE_VOID); }
     | TOKEN_KEYWORD_ARRAY TOKEN_LBRACKET TOKEN_RBRACKET type
       { $$ = type_create_array($4); }
     ;

%%

