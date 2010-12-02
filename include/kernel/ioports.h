/*
 * ioports.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef IOPORTS_H
#define IOPORTS_H

#include <types.h>

namespace io {

namespace out {

inline void byte(uint16_t port, uint8_t data)
{
	/*
	 * "a" stand for accumalator (%eax)
	 * "N" stand for 8 bits unsigned value (i386 sepcific for ioports)
	 * "d" stand for the d register (with N: %dl)
	 */
	asm volatile ("outb %b0, %w1" :: "a"(data), "Nd"(port));
}

inline void word(uint16_t port, uint16_t data)
{
	asm volatile ("outw %b0, %w1" :: "a"(data), "Nd"(port));
}

inline void qword(uint16_t port, uint32_t data)
{
	asm volatile ("outl %b0, %w1" :: "a"(data), "Nd"(port));
}

} // namespace out

namespace in {

inline uint8_t byte(uint16_t port)
{
	uint8_t ret;
	asm volatile ("inb %w1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

inline uint16_t word(uint16_t port)
{
	uint16_t ret;
	asm volatile ("inw %w1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

inline uint32_t dword(uint16_t port)
{
	uint32_t ret;
	asm volatile ("inl %w1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

} // namespace in

} // namespace io

#endif /* IOPORTS_H */
