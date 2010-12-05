/*
 * pic_i8259.cpp
 * Copyright © 2010 Alexandre Gau <gau.alexandre@gmail.com>
 *
*/

#include <kernel/ioports.h>
#include <kernel/pic_i8259.h>

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
	this->irq_init();
}

void PIC_i8259::irq_init() const
{
	const uint16_t ICW1_M = 0x11; // Envoi de ICW4_M et mode 2 PIC.
	const uint16_t ICW2_M = 32; // Uint16_T de base.
	const uint16_t ICW3_M = 4; // IRQ 2 = PIC slave.
	const uint16_t ICW4_M = 1; // Mode Intel, EOI manuel.
	const uint16_t OCW1_M = 0xFB; // Toutes les INT masquée sauv celle du slave !

	const uint16_t ICW1_S = 0x11; // Envoi de ICW4_M et mode 2 PIC.
	const uint16_t ICW2_S = 40; // Int de base.
	const uint16_t ICW3_S = 2; // Relier au maître par l'int 2.
	const uint16_t ICW4_S = 1; // Mode Intel, EOI manuel.
	const uint16_t OCW1_S = 0xFF; // Toutes les INT masquée !

	kernel::io::out::word(ICW1_M, ICW001_M);
    kernel::io::out::word(ICW2_M, ICW234_M);
    kernel::io::out::word(ICW3_M, ICW234_M);
    kernel::io::out::word(ICW4_M, ICW234_M);
    kernel::io::out::word(OCW1_M, OCW001_M);

    kernel::io::out::word(ICW1_S, ICW001_S);
	kernel::io::out::word(ICW2_S, ICW234_S);
	kernel::io::out::word(ICW3_S, ICW234_S);
	kernel::io::out::word(ICW4_S, ICW234_S);
    kernel::io::out::word(OCW1_S, OCW001_S);
}

void	PIC_i8259::release() const
{
	kernel::io::out::word(0xff, OCW001_M);
    kernel::io::out::word(0xff, OCW001_S);
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

		Mask = kernel::io::in::word(OCW001_S);
		kernel::io::out::word(Mask | NumIrq, OCW001_S);
		return 0;
	}
	if (NumIrq < 15)
	{
		// Master PIC
		if (NumIrq > 9)
			NumIrq = NbToBit(NumIrq-7);
		else
			NumIrq = NbToBit(NumIrq);

		Mask = kernel::io::in::word(OCW001_M);
		kernel::io::out::word(OCW001_M, Mask | NumIrq);
		return 0;
	}
	return 1;
}

bool	PIC_i8259::enable(uint8_t NumIrq)
{
	uint8_t Mask;

	if ((NumIrq > 1) && (NumIrq < 10))
	{
		// Slave PIC
		NumIrq = NbToBit(NumIrq-2);

		Mask = kernel::io::in::word(OCW001_S);
		kernel::io::out::word(Mask & ~NumIrq, OCW001_S);
		return 0;
	}
	if (NumIrq < 15)
	{
		// Master PIC
		if (NumIrq > 9)
			NumIrq = NbToBit(NumIrq-7);
		else
			NumIrq = NbToBit(NumIrq);

		Mask = kernel::io::in::word(OCW001_M);
		kernel::io::out::word(OCW001_M, Mask & ~NumIrq);
		return 0;
	}
	return 1;
}


} // namespace driver
