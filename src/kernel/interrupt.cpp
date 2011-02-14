/*
 * interrupt.cpp
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/interrupt.h>

#include <kernel/console.h>
#include <kernel/vga_console.h>
#include <attributes.h>

#include KERNEL_INTERRUPT_DEBUG
#include KERNEL_INTERRUPT_CHECK

namespace kernel {
namespace interrupt {

Manager::Manager()
{
	dbg("starting");

	struct {
		uint16_t idt_size;
		void*    idt_addr;
	} PACKED ptrIDTR{
		static_cast<uint16_t>(_idt.size() / sizeof _idt[0]), &_idt[0]
	};

	asm volatile ("lidtl %0" :: "m" (ptrIDTR));
	setInts();
	dbg("started");
}

Manager::~Manager()
{
	dbg("stopping");
	clsInts();
	dbg("stopped");
}

//int	Manager::setManagerHandler(uint16_t NbInt, void* Offset)
//{
//        if (NbInt > IDT_SIZE)
//            return -1;
//        if (Offset != nullptr)
//        {
//            if (this->_idt[NbInt].Type == 0x8E00)
//            {
//                kernel::main_console->write("Int déjà allouée.");
//                return -1;
//            }
//            this->_idt[NbInt].Selecteur = 8;
//            this->_idt[NbInt].Type = 0x8E00;
//            this->_idt[NbInt].Offset_Low  = ((uint32_t)Offset) & 0xffff;
//            this->_idt[NbInt].Offset_High = (((uint32_t) Offset) >> 16) & 0xffff;
//        }
//        else
//        {
//         Disable this IDT entry
//            this->_idt[NbInt].Selecteur = 0;
//            this->_idt[NbInt].Type = 0;
//            this->_idt[NbInt].Offset_Low  = 0;
//            this->_idt[NbInt].Offset_High = 0;
//        }
//    return 0;
//}

//void*	Manager::getManagerHandler(uint16_t NbInt)
//{
//        if (NbInt > IDT_SIZE)
//            return nullptr;
//        return (void*)(uint32_t)(this->_idt[NbInt].Offset_Low | (this->_idt[NbInt].Offset_High << 16));
//}

//INIT_PRIORITY(65533) Manager interrupt;

} // namespace interrupt
} // namespace kernel
