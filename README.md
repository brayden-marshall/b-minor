## TODO

- print statement. example:
    `print "The temperature is: ", temp, " degrees\n";

- implement parsing for the else portion of if-else statements

- allow expressions in array type definitions. example:
    `array [5] integer`

- initializer lists for array initialization

- operators to implement, with precedence level:
    - precedence 9:
        - []
        - f() (function call)
    - precedence 8:
        - ++ (postfix only)
        - -- (postfix only)
    - precedence 7:
        - - (unary negation)
        - !
    - precedence 6:
        - ^ (exponentiation)
    - precedence 5:
        - % (modulo)
    - precedence 3:
        - <
        - <=
        - >
        - >=
        - ==
        - !=
    - precedence 2:
        - &&
        - ||
