/*
 * pic.h
 * Copyright © 2010, Alexandre Gau <gau.alexandre@gmail.com>
 * Copyright © 2011, François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef PIC_H
#define PIC_H

#include <kernel/types.h>
#include <kernel/ioports.h>

#include KERNEL_PIC_DEBUG
#include KERNEL_PIC_CHECK

namespace kernel {
namespace pic {

class Manager
{
	public:
		Manager();
		~Manager();

		static inline uint8_t int2irq(uint8_t intIdx) {
			return intIdx - BASE_INT; }
		static inline uint8_t irq2int(uint8_t irqIdx) {
			return irqIdx + BASE_INT; }

		NOINLINE void enable(uint8_t irqIdx) {
			assert(irqIdx <= 15);
			assert(irqIdx != 2); // don't touch the master/slave link IRQ2
			if (irqIdx < 8)
			{
				/*  irq on master pic */
				_masterMask &= ~(1 << irqIdx);
				kernel::io::out::byte(_masterMask, PORT2_M);
			}
			else
			{
				/*  irq on slave pic */
				_slaveMask &= ~(1 << (irqIdx - 8));
				kernel::io::out::byte(_slaveMask, PORT2_S);
			}
			dbg("IRQ #%c enabled", irqIdx);
		}

		void disable(uint8_t irqIdx) {
			assert(irqIdx <= 15);
			assert(irqIdx != 2); // don't touch the master/slave link IRQ2
			if (irqIdx < 8)
			{
				/*  irq on master pic */
				_masterMask |= (1 << irqIdx);
				kernel::io::out::byte(_masterMask, PORT2_M);
			}
			else
			{
				/*  irq on slave pic */
				_slaveMask |= (1 << (irqIdx - 8));
				kernel::io::out::byte(_slaveMask, PORT2_S);
			}
			dbg("IRQ #%c disabled", irqIdx);
		}

		void eoi(uint8_t irqIdx) const {
			assert(irqIdx <= 15);
			assert(irqIdx != 2); // don't touch the master/slave link IRQ2
			dbg("IRQ EOI #%c", irqIdx);
			if (irqIdx < 8)
			{
				/*  irq on master pic */
				kernel::io::out::byte(EOI, PORT1_M);
			}
			else
			{
				/*  irq on slave pic */
				kernel::io::out::byte(EOI, PORT1_S);
				kernel::io::out::byte(EOI, PORT1_M);
			}
		}
	private:
		uint8_t _masterMask;
		uint8_t _slaveMask;

		static const uint8_t BASE_INT = 32;

		static const uint16_t PORT1_M = 0x20;
		static const uint16_t PORT2_M = 0x21;

		static const uint16_t PORT1_S = 0xA0;
		static const uint16_t PORT2_S = 0xA1;

		static const uint8_t ICW1_M = 0x11; // cascaded mode
		static const uint8_t ICW2_M = Manager::BASE_INT; // Base #INT
		static const uint8_t ICW3_M = 4; // master, IRQ 2 = PIC slave.
		static const uint8_t ICW4_M = 1; // intel mode, EOI manuel.
		static const uint8_t OCW1_M = 0xFB; // full mask but slave

		static const uint8_t ICW1_S = 0x11; // cascaded mode
		static const uint8_t ICW2_S = Manager::BASE_INT + 8; // Base #INT
		static const uint8_t ICW3_S = 2; // slave, connected on master IRQ 2
		static const uint8_t ICW4_S = 1; // intel mode, EOI manuel.
		static const uint8_t OCW1_S = 0xFF; // full mask
		
		static const uint8_t EOI = 0x20; // full mask
};

} // namespace pic

extern pic::Manager picManager;

} // namespace kernel

#include <check_off.h>
#include <debug_off.h>

#endif /* PIC_H */
