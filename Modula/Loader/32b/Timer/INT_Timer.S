// Bombela mardi 14 février 2006

// Essai IRQ INT

.global TimerIntAsm
TimerIntAsm:

	/*
	 * Backup the CPU context
	 */

	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	pushw %ss
	pushw %ds
	pushw %es
	pushw %fs
	pushw %gs


	/* Send EOI to PIC. */
	movb  $0x20, %al
	outb  %al, $0x20

	/*
	 * Call the handler
	 */
	call  TimerInt

	/* Restore the context */
	popw  %gs
	popw  %fs
	popw  %es
	popw  %ds
	popw  %ss
	popl  %eax
	popl  %ebx
	popl  %ecx
	popl  %edx
	popl  %esi
	popl  %edi

	iret
