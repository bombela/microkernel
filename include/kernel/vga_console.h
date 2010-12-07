/*
 * VGAConsole.h
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef VGA_CONSOLE_H
#define VGA_CONSOLE_H

#include <array.hpp>
#include <kernel/console.h>

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

		virtual void write(const char*);
		virtual void write(const char*, size_t);
		virtual void write(char);
		virtual void setAttr(const Attr&);
		virtual void resetAttr();

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

		static const size_t line_len = 80;
		static const size_t line_cnt = 25;

		std::array<vga_char, line_len * line_cnt,
			std::buffer::absolute, 0xB8000> _video_mem;
		size_t _idx;
		char   _attr;

#ifdef REENTRENT_ON
		int _reentrance;
#endif
};

} // namespace kernel

#undef REENTRENT_ON

#endif /* VGA_CONSOLE_H */
