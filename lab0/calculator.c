/* CSE 351 Section 01
 *
 * calculator.c
 *
 * Author: Matt Dorsett
 * Modified by: Max Willsey
 *
 * Provide two operands and an operator, and this program will print the
 * output of that operation.
 *
 * Usage: ./calculator operand1 operand2 operator, where operator can be
 *        one of [+,-,x] and the operands are integers
 *
 * Example ./calculator 4 5 + => will output 4 + 5 = 9
 *         ./calculator 6 3 x => will output 6 x 3 = 18
 *
 * Notes:
 *
 * => argc = # of provided arguments + 1
 * => argv = array of strings of arguments (just like Java String[] args)
 * => function prototype above main() tells compiler the return type and params of print_operation()
 *    => could have also just declared the entire function above main()
 *    => neglecting to do either will result in a compilation error
 */

#include <stdio.h>
#include <stdlib.h>

void print_operation(int a, int b, char operator);

int main(int argc, char **argv)
{
    /* We want three parameters, but the executable name is always argv[0]
     * so we actually want argc to be 4 */
    if (argc != 4)
    {
        printf("Usage: ./calculator operand1 operand2 operator\n");
        return 1;
    }
    
    /* atoi() converts strings to integers */
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);

    /* Grab the first character of the third parameter */
    char operator = argv[3][0];

    print_operation(a, b, operator);

    return 0;
}

/* Prints out the result when the operator is applied to the 
 * operands a and b. Supports addition (+), subtraction (-),
 * multiplication (x). Can you add division (/)? 
 * What happens when you divide by 0? Can you avoid that? */
void print_operation(int a, int b, char operator)
{
    int result = 0;
    switch(operator)
    {
        case '+':
            result = a + b;
            break;
        case '-':
            result = a - b;
            break;
        case 'x':
            result = a * b;
            break;
        default:
            printf("Invalid operator");
            return;
    }
    printf("%d %c %d = %d\n", a, operator, b, result);
}
