
/*	.long	0xe85250d6
	.long	0x03
	.long	-0xe85250d6 - 0x03*/

.file	"startup.S"
.text
.globl	start, _start
start:
_start:
	jmp codestart

	.p2align	2	/* force 4-byte alignment */
multiboot_header:
	.long	0x1BADB002
	.long	0x03
	.long	-0x1BADB002 - 0x03

codestart:
	jmp codestart

