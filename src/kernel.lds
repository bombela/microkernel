/*
 * kernel.lds
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

/* We generate binary in the ELF format */
OUTPUT_FORMAT("elf32-i386","elf32-i386","elf32-i386");

/* The entry point is _start */
ENTRY(_start)

/* The architecture is i386 */
OUTPUT_ARCH("i386")

SECTIONS
{
	/* our kernel is loaded at 0x200000 */
	. = 0x200000;
    __b_load = .;

	/* the multiboot header MUST come early enough in the output
	   object file */
	.multiboot :
	{
		/* The multiboot section (containing the multiboot header)
		   goes here */
		*(.multiboot);
	
		/*
		 * With the following line, we force this section to be
		 * allocated in the output file as soon as possible, no matter
		 * when the file containing the multiboot header (multiboot.S)
		 * is compiled. This is to conform to the multiboot spec, which
		 * says "The Multiboot header must be contained completely
		 * within the first 8192 bytes of the OS image, and must be
		 * longword (32-bit) aligned."
		 */
		LONG(0);
	}

	/* Defines a symbol '__b_kernel to mark the start of the kernel
	   code/data */
	. = ALIGN(4096);
	__b_kernel = .;
  
	/* Beginning of the text section */
	.text ALIGN(4096):
	{	
		/* This section includes the code */
		*(.text*)
		/* Defines the end symbol */
		__e_text = .;
	}

	/* Beginning of the data section */
	.data . :
	{
		/* This section includes the data */
		*(.data*) 
		/* Defines the end symbol */
		__e_data = .;
	}

	/* Beginning of the read-only data section */
	.rodata . :
	{
		/* This section includes the rodata */
		*(.rodata*)

		/* Defines the end symbol */
		__e_rodata = .;
	}

	/* We take note of the end of the data to load */
	__e_load = .;

	/* Beginning of the BSS section (global uninitialized data) */
	.bss . :
	{
		*(.bss)

		/* All the *common* symbols.
		 * see gnu binutils ld documentation,
		 * section "Input Section for Common Symbols"
		 */
		*(COMMON)
		
		/* We put the bootstrap stack */
		. = ALIGN(4096);
		*(.stack)

		/* Defines the end symbol */
		__e_bss = .;
	}

	/* We take note of the end of the kernel */
	__e_kernel = .;
}
