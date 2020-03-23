%{
#include "parser.h"
%}

%option nounput
%option noinput

DIGIT [0-9]
IDENT_START [a-zA-Z_]
IDENT       [a-zA-Z_0-9]

%%

"/*".*"*/"              { printf("found block comment, text is: '%s'\n", yytext); }
"//".*\n                { printf("found line comment, text is: %s", yytext); }

[ \n\r\t]*              ;

":"                     return TOKEN_COLON;
";"                     return TOKEN_SEMI;
","                     return TOKEN_COMMA;

"+"                     return TOKEN_PLUS;
"-"                     return TOKEN_MINUS;
"*"                     return TOKEN_MULTIPLY;
"/"                     return TOKEN_DIVIDE;
"="                     return TOKEN_ASSIGN;

"("                     return TOKEN_LPAREN;
")"                     return TOKEN_RPAREN;
"["                     return TOKEN_LBRACKET;
"]"                     return TOKEN_RBRACKET;
"{"                     return TOKEN_LBRACE;
"}"                     return TOKEN_RBRACE;

"array"                 return TOKEN_ARRAY;
"boolean"               return TOKEN_BOOLEAN;
"char"                  return TOKEN_CHAR;
"else"                  return TOKEN_ELSE;
"false"                 return TOKEN_FALSE;
"for"                   return TOKEN_FOR;
"function"              return TOKEN_FUNCTION;
"if"                    return TOKEN_IF;
"integer"               return TOKEN_INTEGER;
"print"                 return TOKEN_PRINT;
"return"                return TOKEN_RETURN;
"string"                return TOKEN_STRING;
"true"                  return TOKEN_TRUE;
"void"                  return TOKEN_VOID;
"while"                 return TOKEN_WHILE;

{DIGIT}+                return TOKEN_INTEGER_LITERAL;
\".*\"                  return TOKEN_STRING_LITERAL;
\'.\'                   return TOKEN_CHAR_LITERAL;
{IDENT_START}{IDENT}*   return TOKEN_IDENT;

.                       { printf("scan error. bad token: %c\n", yytext[0]); }

%%

int yywrap() { return 1; }
