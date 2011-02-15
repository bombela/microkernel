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

	pushl (%esp)
	pushl 4(%esp)
	call _interrupt_handler
	addl $8, %esp

	/* restore context */

	/* remove args */
	addl $8, %esp
	iret
