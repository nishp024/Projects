/* begin C driver */
/* This program will ask for a number, then
* print the Fibonacci sequence up to that number plus one.
*/
#include <stdio.h>
void Fib(void);
int global_var; /* this is a GLOBAL variable */
int main(void)
{
int i;
int number;
int fib_subscript;
printf("Please enter a number: ");
scanf("%d",&number);
printf("Fibonacci sequence from subscript 0 to %d :\n", number);
for(i=0; i<=number; i++) {
global_var = i;
Fib();
fib_subscript = global_var;
printf("%3d ", fib_subscript);
}
printf("\n");
}
/* end C driver */
