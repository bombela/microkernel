/*
 * multiboot.s
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

/* 16k stack */
KERNEL_BOOT_STACK_SIZE = 0x4000

.text

.globl _start
_start:
	/* Setup the stack */
	movl $(kernel_boot_stack + KERNEL_BOOT_STACK_SIZE), %ebp
	movl %ebp, %esp

	/* clean EFLAGS */
	pushl $0
	popf
	
	/* call main with multiboot magic & address */
	push %ebx /* multiboot structure address */
	push %eax /* multiboot magic */

	call kernel_main

	/* should never reach here */
	
	/* stop interruptions */
	cli
1:
	hlt
	jmp 1b

.section ".stack", "aw", @nobits
.size kernel_boot_stack, KERNEL_BOOT_STACK_SIZE
kernel_boot_stack:
	.space KERNEL_BOOT_STACK_SIZE

.section ".ctors", "aw"
	.space 1
