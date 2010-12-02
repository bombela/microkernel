/*
 * ioports.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef IOPORTS_H
#define IOPORTS_H

#include <kernel/types.h>

/*
 * Extended asm brief summary:
 *
 * In asm parameters:
 * "a" stand for accumalator (%eax)
 * "N" stand for a direct 8 bits unsigned value (i386 sepcific for ioports)
 * "d" stand for the d register
 *
 * In asm code:
 * "b" stand for byte size
 * "w" stand for word size
 * "d" stand for double word size
 */

namespace kernel {
namespace io {

namespace out {

inline void byte(uint8_t data, uint16_t port)
{
	asm volatile ("outb %b0, %w1" :: "a"(data), "Nd"(port));
}

inline void word(uint16_t data, uint16_t port)
{
	asm volatile ("outw %w0, %w1" :: "a"(data), "Nd"(port));
}

inline void dword(uint32_t data, uint16_t port)
{
	asm volatile ("outl %d0, %w1" :: "a"(data), "Nd"(port));
}

} // namespace out

namespace in {

inline uint8_t byte(uint16_t port)
{
	uint8_t ret;
	asm volatile ("inb %w1, %b0" : "=a"(ret) : "Nd"(port));
	return ret;
}

inline uint16_t word(uint16_t port)
{
	uint16_t ret;
	asm volatile ("inw %w1, %w0" : "=a"(ret) : "Nd"(port));
	return ret;
}

inline uint32_t dword(uint16_t port)
{
	uint32_t ret;
	asm volatile ("inl %w1, %d0" : "=a"(ret) : "Nd"(port));
	return ret;
}

} // namespace in

} // namespace io
} // namespace kernel

#endif /* IOPORTS_H */
