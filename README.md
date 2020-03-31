## TODO

- for some expressions we are only allowing raw identifiers (such as increment and decrement). should we allow more complex expressions yet?

- custom varargs print function for error printing

- proper typechecking when assigning to an empty initializer list

## Operator Precedence Table

1 has highest precedence

- precedence 1:
    - `arr[]`   (subscript)
    - `f()`     (function call)
    - `++`      (postfix increment)
    - `--`      (postfix decrement)
- precedence 2:
    - `-`       (unary negation)
    - `!`       (logical negation)
- precedence 3:
    - `*`       (multiplication)
    - `/`       (division)
    - `%`       (modulo)
- precedence 4:
    - `+`       (addition)
    - `-`       (subtraction)
- precedence 5:
    - `>`       (greater than)
    - `>=`      (greater than or equal)
    - `<`       (less than)
    - `>=`      (less than or equal)
- precedence 6:
    - `==`      (equals)
    - `!=`      (not equals)
- precedence 7:
    - `&&`      (logical AND)
- precedence 8:
    - `||`      (logical OR)
- precedence 9:
    - `=`       (assignment)
