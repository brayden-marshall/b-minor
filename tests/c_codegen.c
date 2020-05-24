int global_integer = 12345;
int global_boolean = 1;
char global_character = 'x';
char* global_string = "Hello, World!";

int func() {
    int local_integer = 54321;
    int local_boolean = 0;
    char local_character = 'Y';

    int add = 1 + 1;
    int subtract = 10 - add;
    int multiply = subtract * add;
    int divide = multiply * multiply;
    int modulo = 10 % multiply;

    local_integer = 1;
    local_boolean = 1;
    local_character = 'c';

    //if (false) {
    //    return 10;
    //} else {
    //    return local_integer;
    //}

    return 0;
}
