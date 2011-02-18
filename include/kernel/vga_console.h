/*
 * VGAConsole.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef VGA_CONSOLE_H
#define VGA_CONSOLE_H

#include <array>
#include <kernel/console.h>
#include <kernel/memory.h>

/*
 * Enable REENTRENT check if array compiling with debug/check on.
 * Attention, reentrant check is totally not thread-safe, but it's only a way
 * for help checking the VGAConsole code itself, or external VGAConsole state
 * modification, so it's only an help for testing developpement code.
 */
#include KERNEL_STD_ARRAY_DEBUG
#include KERNEL_STD_ARRAY_CHECK

#if defined(DEBUG_ON) || defined(CHECK_ON)
#	define REENTRENT_ON
#endif

#include <check_off.h>
#include <debug_off.h>

namespace kernel {

class VGAConsole: public Console {
	public:
		static VGAConsole& getInstance();

		void write(const char*);
		void write(const char*, size_t);
		void write(char);
		void setAttr(const Attr&);
		void resetAttr();

		static inline memory::Range<const uint8_t*> mem() {
			return memory::range(vmem_base_addr,
					vmem_base_addr + line_len * line_cnt)
				.cast<const uint8_t*>();
		}

		VGAConsole(const VGAConsole&) = delete;
		VGAConsole& operator=(const VGAConsole&) = delete;

	private:
		VGAConsole();
		~VGAConsole() = default;

		void putChar(char);
		void updateVGACursor();

		struct vga_char {
			uint8_t c;
			uint8_t attr;
		} PACKED;

		static const uint32_t vmem_base_addr = 0xB8000;
		static const size_t line_len = 80;
		static const size_t line_cnt = 25;

		std::array<vga_char, line_len * line_cnt,
			std::buffer::absolute, vmem_base_addr> _video_mem;
		size_t _idx;
		char   _attr;

#ifdef REENTRENT_ON
		int _reentrance;
#endif
};

} // namespace kernel

#undef REENTRENT_ON

#endif /* VGA_CONSOLE_H */
