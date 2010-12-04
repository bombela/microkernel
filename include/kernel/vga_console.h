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
		enum class fgcolor {
			black      =  0,
			dkgray     =  8,
			blue       =  1,
			ltblue     =  9,
			green      =  2,
			ltgreen    = 10,
			cyan       =  3,
			ltcyan     = 11,
			red        =  4,
			ltred      = 12,
			magenta    =  5,
			ltmagenta  = 13,
			brown      =  6,
			yellow     = 14,
			ltgray     =  7,
			white      = 15,
		};

		enum class bgcolor {
			black      =  (0 << 4),
			blue       =  (1 << 4),
			green      =  (2 << 4),
			cyan       =  (3 << 4),
			red        =  (4 << 4),
			magenta    =  (5 << 4),
			brown      =  (6 << 4),
			ltgray     =  (7 << 4),
		};
		
		enum class blink {
			off        = 0,
			on         = (1 << 7),
		};

		static void initInstance();
		static VGAConsole& getInstance();

		virtual void write(const char*);
		virtual void write(char);

		VGAConsole(const VGAConsole& from) = delete;
		VGAConsole& operator=(const VGAConsole& from) = delete;

	private:
		VGAConsole();
		~VGAConsole() = default;

		void putChar(char);
		void updateVGACursor();

		struct vga_char {
			char c;
			char attr;
		} PACKED;

		static const size_t lines_cnt = 25;
		static const size_t rows_cnt  = 80;

		std::array<vga_char, lines_cnt * rows_cnt,
			std::buffer::absolute, 0xB8000> _video_mem;
		int  _idx;
		char _charAttr;

#ifdef REENTRENT_ON
		int _reentrance;
#endif
};

} // namespace kernel

#undef REENTRENT_ON

#endif /* VGA_CONSOLE_H */
