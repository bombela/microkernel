/*
 * interrupt.h
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <kernel/types.h>
#include <kernel/segmentation.h>
#include <kernel/memory.h>
#include <attributes.h>
#include <array>

namespace kernel {
namespace interrupt {

enum Type { BITS = 3, interrupt = 0, trap = 1 };

class Manager
{
	public:
		Manager();
		~Manager();

		Manager(const Manager& from) = default;
		Manager(Manager&& from) = default;
		Manager& operator=(const Manager& from) = default;
		Manager& operator=(Manager&& from) = default;

		inline void	enable() { asm volatile ("sti"); }
		inline void	disable() { asm volatile ("cli"); }

	private:
		struct IDTEntry
		{
			uint16_t               offset_low;
			segmentation::Selector selector;
	
			uint8_t                reserved:5;
			uint8_t                flags:3;

			uint8_t                type:3;
			uint8_t                op_size:1;
			uint8_t                zero:1;
			uint8_t                dpl:2;
			uint8_t                present:1;
			
			uint16_t               offset_high;

			inline void setOffset(uint32_t off) {
				offset_low  = static_cast<uint16_t>(off);
				offset_high = static_cast<uint16_t>(off >> 16);
			}
		} PACKED ALIGNED(8);

		std::array<IDTEntry, 256> _idt;
};

} // namespace interrupt

extern interrupt::Manager interruptManager;

} // namespace kernel

#endif /* INTERRUPT_H */
