# B-Minor Compiler

My implementation of the compiler from the book _Introduction to Compilers and Language Design_ by Douglas Thain ([here](https://www3.nd.edu/~dthain/compilerbook))

## TODO/Issues

- for some expressions we are only allowing raw identifiers (such as increment and decrement). should we allow more complex expressions yet?
- switch to CMake instead of the crusty Makefile we currently have
- Create a routine to output the AST into Graphiz DOT notation (or some other visual representation)
- proper typechecking when assigning to an empty initializer list
- modify code generation so it can be passed directly to 'ld' rather than using gcc

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
