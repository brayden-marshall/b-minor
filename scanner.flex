%{
#include "parser.h"
%}

%option nounput
%option noinput

DIGIT [0-9]
IDENT_START [a-zA-Z_]
IDENT       [a-zA-Z_0-9]

%%

[ \n\r\t]*  ;
array                   return TOKEN_KEYWORD_ARRAY;
boolean                 return TOKEN_KEYWORD_BOOLEAN;
char                    return TOKEN_KEYWORD_CHAR;
else                    return TOKEN_KEYWORD_ELSE;
false                   return TOKEN_KEYWORD_FALSE;
for                     return TOKEN_KEYWORD_FOR;
function                return TOKEN_KEYWORD_FUNCTION;
if                      return TOKEN_KEYWORD_IF;
integer                 return TOKEN_KEYWORD_INTEGER;
print                   return TOKEN_KEYWORD_PRINT;
return                  return TOKEN_KEYWORD_RETURN;
string                  return TOKEN_KEYWORD_STRING;
true                    return TOKEN_KEYWORD_TRUE;
void                    return TOKEN_KEYWORD_VOID;
while                   return TOKEN_KEYWORD_WHILE;
{DIGIT}+                return TOKEN_INTEGER_LITERAL;

%%

int yywrap() { return 0; }
