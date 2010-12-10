.file "irq_wrappers.s"

.text

.extern irq_handler

.globl irq_wrapper


/* Wrappers for IRQ (Master PIC) */
.irp id, 0,1,2,3,4,5,6,7

	.p2align 2, 0x90

	irq_wrapper_impl\id:
	.type irq_wrapper_impl\id,@function

		pushl %ebp
		movl %esp, %ebp

		/* Send EOI to PIC. */	
		movb  $0x20, %al
		outb  %al, $0x20

    		pushl $\id
		leal  irq_handler,%edi
		call  *\id*4(%edi)
		addl  $4, %esp

    		leave
		iret
	.endr


/* Wraperrs for IRQ (Slave PIC) */
.irp id, 8,9,10,11,12,13,14,15

	.p2align 2, 0x90

	irq_wrapper_impl\id:
	.type sos_irq_wrapper_impl\id,@function

    		pushl %ebp
		movl %esp, %ebp


		/* Send EOI to PIC. */	
		movb  $0x20, %al
		outb  %al, $0xa0
		outb  %al, $0x20

    		pushl $\id
		leal  irq_handler,%edi
		call  *\id*4(%edi)
		addl  $4, %esp

    		leave
    		iret
	.endr

.section ".rodata"
.p2align 5, 0x0
irq_wrapper:
	.irp id, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
		.long (irq_wrapper_impl\id)
	.endr
