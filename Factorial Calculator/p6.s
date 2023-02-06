/* Nish Patel
   Spring 2022

   PURPOSE This program takes a number inputted by the user and calculates the factorial of it. The Factorial
function is the main function which allows all of the code to work properly. This code utilizes the jump functions
as well as call functions. %eax contains the user inputted variable known as n.
*/

/* begin assembly stub */

/* FUNCTION	Factorial
   ARGUMENTS	n
   RETURNS	the number which is the factorial of n
   PURPOSE	this function takes an input n and returns the factorial of it
*/

/* put assembler directives here */
.globl Factorial #function creation
.type Factorial, @function

Factorial:
/* prolog */
pushl %ebp
pushl %ebx
movl %esp, %ebp #normal prolog procedures which push registers
subl $4, %esp #subtracts 4 from esp in order to make space for variables

/* put code here */
movl 12(%ebp), %eax #moves the inputted variable to %eax

cmp $0, %eax #compares 0 to %eax
je firstret #jumps to firstret of they are equal
cmp $1, %eax #compares 1 to %eax
je firstret #jumps to firstret of they are equal

subl $1, %eax #subtracts 1 from the %eax which has user input
pushl %eax #moves the user input %eax into the stack

call Factorial #calls the Factorial function above

movl 12(%ebp), %ebx #%ebx points to value in stack
mull %ebx #multiplies %ebx by %eax. basically multiplies the user input by the output of the Factorial function
jmp return #jumps to return

firstret:
movl $1, %eax #sets %eax register equal to one. needed for factorial calculation

/*epilog*/
return:
movl %ebp, %esp
popl %ebx
popl %ebp #normal epilog procedures which pop registers

ret #returns %eax which is the factorial.
