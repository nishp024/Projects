/*
Nish Patel
Spring 2022

PURPOSE This program teached me how to use recursion and stack in assembly. The program technically runs several times
as a result of the recursion set up. The amount of times varies based on the user input. By using stack, I
was able to store all values that needed to be outputted very easily.
*/

/* FUNCTION 	Fib
   ARGUMENTS  none
   RETURNS	  n/a
   PURPOSE	  This function takes in the user input and uses it to output the Fibonacci sequence corresponding to it
*/
.globl Fib
.type Fib,@function
Fib:
	/*prolog*/
	pushl %ebx #preparing registers
	pushl %ebp
	movl %esp, %ebp
	subl $8, %esp

	movl global_var, %eax
	movl  %eax, -4(%ebp)

	cmpl $0, -4(%ebp)
	je return #jumps to return if %ebp equals 0

	cmpl $1, -4(%ebp)
	je return #jumps to return if %ebp equals 1

	movl -4(%ebp), %eax
	subl $1, %eax #subtracts 1 from %eax
	movl %eax, global_var #moves %eax into global_var

	call Fib #needed for recursion
	movl global_var, %eax #sets %eax to global_var
	movl %eax, -8(%ebp)
	movl -4(%ebp), %ebx #stores %ebp into %ebx
	subl $2, %ebx
	movl %ebx, global_var #gives global_var the value of %ebx

	call Fib #needed for recursion
	movl global_var, %eax #gives %eax value of global_var for stack
	movl -8(%ebp), %ebx
	addl %ebx, %eax #adds %ebx and %eax
	movl %eax, global_var

	/*epilog*/
	return:
		movl %ebp, %esp
    popl %ebp
    popl %ebx

	ret
    /* end assembly stub */
