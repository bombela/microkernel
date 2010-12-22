/*
 * exception_wrappers.S
 * Copyright © 2010 Alexandre Gau <gau.alexandre@gmail.com>
 *
*/

.file "exception_wrappers.S"

.text

.extern exception_handler

.globl exception_wrapper

.irp id, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31

        .p2align 2, 0x90
        exception_wrapper_impl\id:
        .type exception_wrapper_impl\id,@function

                pushl %ebp
                movl %esp, %ebp

                /* Call the handler */
                pushl $\id
                leal  exception_handler,%edi
                call  *\id*4(%edi)
                addl  $4, %esp

                //popl  %ebp
		leave
    		iret
.endr

.section ".rodata"
.p2align 5, 0x0
exception_wrapper:
	.irp id, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,29,30,31
	  .long (exception_wrapper_impl\id)
	.endr
