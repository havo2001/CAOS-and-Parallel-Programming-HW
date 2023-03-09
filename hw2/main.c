#include <stdlib.h>
#include <stdio.h>


/// Operation is a binary function, which receives two numbers and return one
typedef int(*operation_ptr)(int, int);

int apply_operation (operation_ptr operation, int first_value, int second_value);

int op_add (const int first_value, const int second_value) { return first_value + second_value; }
int op_sub (const int first_value, const int second_value) { return first_value - second_value; }
int op_mul (const int first_value, const int second_value) { return first_value * second_value; }
int op_div (const int first_value, const int second_value) { return first_value / second_value; }


int main () {
    const operation_ptr ops[4] = { &op_add, &op_sub, &op_mul, &op_div };

    int first_value, second_value;
    scanf("%d%d", &first_value, &second_value);

    for (size_t i = 0; i < 4; i ++)
        printf("%d\n", apply_operation(ops[i], first_value, second_value));

    return 0;
}
