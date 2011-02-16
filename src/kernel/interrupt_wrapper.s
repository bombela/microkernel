/*
 * context_switcher.S
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

.text

.globl interrupt_wrapper
interrupt_wrapper:
	/* EFLAGS */
	/* CS */
	/* EIP */
	/* arg2 == exception code */
	/* arg1 == interrupt val */

	/* backup context */
	pushl %ebp
	mov %esp, %ebp

	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	
	/*
	TODO backup segments
	subl  $2,%esp // aligement
	pushw %ss
	pushw %ds
	pushw %es
	pushw %fs
	pushw %gs
	*/

	/* change stack */

	/* call interrupt_handler */
	lea 12(%ebp), %eax
	pushl %eax    /* &eip */
	pushl 8(%ebp) /* exception code */
	pushl 4(%ebp) /* interrupt idx */
	call _interrupt_handler
	addl $12, %esp

	/* restore segments
	popw  %gs
	popw  %fs
	popw  %es
	popw  %ds
	popw  %ss
	addl  $2,%esp */
	
	/* restore context */
	popl  %eax
	popl  %ebx
	popl  %ecx
	popl  %edx
	popl  %esi
	popl  %edi
	popl  %ebp

	/* remove args */
	addl $8, %esp
	iret
