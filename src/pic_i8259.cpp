/*
 * pic_i8259.cpp
 * Copyright © 2010 Alexandre Gau <gau.alexandre@gmail.com>
 *
*/

#include <kernel/ioports.h>
#include <kernel/pic_i8259.h>
#include <kernel/console.h>

namespace {
	const uint16_t ICW001_M = 0x20;
	const uint16_t ICW234_M	= 0x21;
	const uint16_t OCW001_M	= 0x21;
	const uint16_t OCW023_M	= 0x20;
	const uint16_t ICW001_S	= 0xA0;
	const uint16_t ICW234_S	= 0xA1;
	const uint16_t OCW001_S	= 0xA1;
	const uint16_t OCW023_S	= 0xA0;
} // namespace

namespace driver
{

PIC_i8259::PIC_i8259()
{
	kernel::main_console->write("Initialisation PIC i8259.\n");
	this->irq_init();
}

PIC_i8259::~PIC_i8259()
{
	kernel::main_console->write("Realese PIC i8259.\n");
	this->release();
}

void PIC_i8259::irq_init() const
{
	const uint8_t ICW1_M = 0x11; // Envoi de ICW4_M et mode 2 PIC.
	const uint8_t ICW2_M = 32; // Int de base.
	const uint8_t ICW3_M = 4; // IRQ 2 = PIC slave.
	const uint8_t ICW4_M = 1; // Mode Intel, EOI manuel.
	const uint8_t OCW1_M = 0xFB; // Toutes les INT masquée sauv celle du slave !

	const uint8_t ICW1_S = 0x11; // Envoi de ICW4_M et mode 2 PIC.
	const uint8_t ICW2_S = 40; // Int de base.
	const uint8_t ICW3_S = 2; // Relier au maître par l'int 2.
	const uint8_t ICW4_S = 1; // Mode Intel, EOI manuel.
	const uint8_t OCW1_S = 0xFF; // Toutes les INT masquée !

	kernel::io::out::byte(ICW1_M, ICW001_M);
    kernel::io::out::byte(ICW2_M, ICW234_M);
    kernel::io::out::byte(ICW3_M, ICW234_M);
    kernel::io::out::byte(ICW4_M, ICW234_M);
    kernel::io::out::byte(OCW1_M, OCW001_M);

    kernel::io::out::byte(ICW1_S, ICW001_S);
	kernel::io::out::byte(ICW2_S, ICW234_S);
	kernel::io::out::byte(ICW3_S, ICW234_S);
	kernel::io::out::byte(ICW4_S, ICW234_S);
    kernel::io::out::byte(OCW1_S, OCW001_S);
}

void	PIC_i8259::release() const
{
	kernel::io::out::byte(0xff, OCW001_M);
    kernel::io::out::byte(0xff, OCW001_S);
}

static uint8_t NbToBit(uint8_t Nb)
{
	uint8_t i;
	uint8_t r = 1;
	for (i = 0; i < Nb; i++) r <<= 1;
	return r;
}


bool	PIC_i8259::disable(uint8_t NumIrq)
{
	uint8_t Mask;

	if ((NumIrq > 1) && (NumIrq < 10))
	{
		// Slave PIC
		NumIrq = NbToBit(NumIrq-2);

		Mask = kernel::io::in::byte(OCW001_S);
		kernel::io::out::byte(Mask | NumIrq, OCW001_S);
		return 0;
	}
	if (NumIrq < 15)
	{
		// Master PIC
		if (NumIrq > 9)
			NumIrq = NbToBit(NumIrq-7);
		else
			NumIrq = NbToBit(NumIrq);

		Mask = kernel::io::in::byte(OCW001_M);
		kernel::io::out::byte(OCW001_M, Mask | NumIrq);
		return 0;
	}
	return 1;
}

bool	PIC_i8259::enable(uint8_t NumIrq)
{
	uint8_t Mask;

	kernel::main_console->write("Enable Irq\n");
	if ((NumIrq > 1) && (NumIrq < 10))
	{
		// Slave PIC
		NumIrq = NbToBit(NumIrq-2);

		Mask = kernel::io::in::byte(OCW001_S);
		kernel::io::out::byte(Mask & ~NumIrq, OCW001_S);
		return 0;
	}
	if (NumIrq < 15)
	{
		// Master PIC
		kernel::main_console->write("   Enable Irq Master\n");
		if (NumIrq > 9)
			NumIrq = NbToBit(NumIrq-7);
		else
			NumIrq = NbToBit(NumIrq);

		Mask = kernel::io::in::byte(OCW001_M);
		kernel::io::out::byte(OCW001_M, Mask & ~NumIrq);
		Mask = kernel::io::in::byte(OCW001_M);
		return 0;
	}
	return 1;
}

uint8_t		PIC_i8259::intToIrq(uint8_t Irq)
{
	// if ((Irq > 1) && (Irq < 10))
	// {
	// 	//Slave
	// 	return ICW2_S+Irq-2;
	// }
	// if (Irq < 15)
	// {
	// 	//Master
	// 	if (Irq > 9) return ICW2_M+Irq-7; else return ICW2_M+Irq;
	// }
	return 0;
}

} // namespace driver
