/* begin C driver */
/* This is the C driver. It will input a number, then
print the Factorial of that number.
*/
#include <stdio.h>
int main(void);
int Factorial(int);
int main(void)
{
int i;
int number;
int answer;
int fib_subscript;
printf("Please enter a number: ");
scanf("%d",&number); answer = Factorial(number);
printf("The factorial of %d is %d.\n", number, answer);
}
/* end C driver */
