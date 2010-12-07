#include <kernel/interrupt.h>

#include <kernel/console.h>
#include <kernel/vga_console.h>
#include <attributes.h>

namespace kernel {


	Interrupt::Interrupt()
	{
		kernel::main_console->write("Initialisation interrupt\n");		

		IDT_ptrIDTR ptrIDT;
		ptrIDT.Base = &Idt;
		ptrIDT.Limite = IDT_SIZE * 8;
		asm("lidtl %0"::"m"(ptrIDT));
	}

	Interrupt::~Interrupt()
	{
		kernel::main_console->write("bye interrupt\n");
	}

	int	Interrupt::setInterruptHandler(uint16_t NbInt, void* Offset)
	{
		if (NbInt > IDT_SIZE)
			return -1;
		if (Offset != nullptr)
		{
			if (this->Idt[NbInt].Type == 0x8E00)
			{
				kernel::main_console->write("Int déjà allouée.");
				return -1;
			}
			this->Idt[NbInt].Selecteur = 8;
			this->Idt[NbInt].Type = 0x8E00;
			this->Idt[NbInt].Offset_Low  = ((uint32_t)Offset) & 0xffff;
			this->Idt[NbInt].Offset_High = (((uint32_t) Offset) >> 16) & 0xffff;
		}
		else
		{
			// Disable this IDT entry
			this->Idt[NbInt].Selecteur = 0;
			this->Idt[NbInt].Type = 0;
			this->Idt[NbInt].Offset_Low  = 0;
			this->Idt[NbInt].Offset_High = 0;
		}
		return 0;
	}

	void*	Interrupt::getInterruptHandler(uint16_t NbInt)
	{
		if (NbInt > IDT_SIZE)
			return nullptr;
		return (void*)(uint32_t)(this->Idt[NbInt].Offset_Low | (this->Idt[NbInt].Offset_High << 16));
	}

INIT_PRIORITY(65533) Interrupt interrupt;

}


