#include <kernel/exception.h>
#include <kernel/interrupt.h>

#include <kernel/console.h>
#include <kernel/vga_console.h>

#include <kernel/die.h>

namespace kernel {

	extern Interrupt interrupt;

#define Define_Except(NUM, TEXT) void Exception ## NUM ()	  \
	{ \
		kernel::main_console->setAttr(kernel::Console::Color::red); \
		kernel::main_console->write(TEXT); \
		kernel::main_console->write("\n"); \
		panic(); \
	}

	Define_Except(0, "Division By Zero Exception")
	Define_Except(1, "Debug Exception")
	Define_Except(2, "Non Maskable Interrupt Exception")
	Define_Except(3, "Breakpoint Exception")
	Define_Except(4, "Into Detected Overflow Exception")
	Define_Except(5, "Out of Bounds Exception")
	Define_Except(6, "Invalid Opcode Exception")
	Define_Except(7, "No Coprocessor Exception")
	Define_Except(8, "Double Fault Exception")
	Define_Except(9, "Coprocessor Segment Overrun Exception")
	Define_Except(10, "Bad TSS Exception")
	Define_Except(11, "Segment Not Present Exception")
	Define_Except(12, "Stack Fault Exception")
	Define_Except(13, "General Protection Fault Exception")
	Define_Except(14, "Page Fault Exception")
	Define_Except(15, "Unknown Interrupt Exception")
	Define_Except(16, "Coprocessor Fault Exception")


#define IDT_ElemDefine(INDEX,OFFSET)	  \
	{ \
		unsigned int Offset = (unsigned int) OFFSET; \
		Idt[INDEX].Selecteur = 8; \
		Idt[INDEX].Type = 0x8E00; \
		Idt[INDEX].Offset_Low  = Offset; \
		Idt[INDEX].Offset_High = Offset >> 16; \
	}

#define IDT_ExceptDefine(NUM) interrupt.setInterruptHandler(NUM,(void*)Exception ## NUM )// IDT_ElemDefine(NUM,Exception ## NUM)

	Exception::Exception()
	{
		//IDT_ExceptDefine(0);
		IDT_ExceptDefine(1);
		IDT_ExceptDefine(2);
		IDT_ExceptDefine(3);
		IDT_ExceptDefine(4);
		IDT_ExceptDefine(5);
		IDT_ExceptDefine(6);
		IDT_ExceptDefine(7);
		IDT_ExceptDefine(8);
		IDT_ExceptDefine(9);
		IDT_ExceptDefine(10);
		IDT_ExceptDefine(11);
		IDT_ExceptDefine(12);
		IDT_ExceptDefine(13);
		IDT_ExceptDefine(14);
		IDT_ExceptDefine(15);
		IDT_ExceptDefine(16);
	}

} // namespace kernel
