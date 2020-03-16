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
array                   return TOKEN_ARRAY;
boolean                 return TOKEN_BOOLEAN;
char                    return TOKEN_CHAR;
else                    return TOKEN_ELSE;
false                   return TOKEN_FALSE;
for                     return TOKEN_FOR;
function                return TOKEN_FUNCTION;
if                      return TOKEN_IF;
integer                 return TOKEN_INTEGER;
print                   return TOKEN_PRINT;
return                  return TOKEN_RETURN;
string                  return TOKEN_STRING;
true                    return TOKEN_TRUE;
void                    return TOKEN_VOID;
while                   return TOKEN_WHILE;
{DIGIT}+                return TOKEN_INTEGER_LITERAL;

%%

int yywrap() { return 0; }
