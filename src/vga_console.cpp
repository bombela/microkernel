/*
 * vga_VGAConsole.cpp
 * Copyright © 2010 François-Régis 'fregux' Robert <robert.fregis@gmail.com>
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <kernel/vga_console.h>
#include <kernel/ioports.h>
#include <kernel/std/algo.hpp>
#include <new>

// Enable REENTRENT check if array compiling with debug/check on.
#include KERNEL_STD_ARRAY_DEBUG
#include KERNEL_STD_ARRAY_CHECK

#if defined(DEBUG_ON) || defined(CHECK_ON)
#	define REENTRENT_ON
#endif

#include <check_off.h>
#include <debug_off.h>

#include KERNEL_CONSOLE_DEBUG
#include KERNEL_CONSOLE_CHECK

#define VGA_CMD_PORT 0x3D4
#define VGA_DTA_PORT 0x3D5
#define VGA_SET_CURSOR_HIGH 0xE
#define VGA_SET_CURSOR_LOW 0xF

namespace kernel {

namespace fgcolor {
	enum type: uint8_t {
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
} // namespace fg

namespace bgcolor {
	enum type: uint8_t {
		black      =  (0 << 4),
		blue       =  (1 << 4),
		green      =  (2 << 4),
		cyan       =  (3 << 4),
		red        =  (4 << 4),
		magenta    =  (5 << 4),
		brown      =  (6 << 4),
		ltgray     =  (7 << 4),
	};
} // namespace bg

namespace blink {
	enum type: uint8_t {
		off        = 0,
		on         = (1 << 7),
	};
} // namespace blink

namespace {
	char instance[sizeof (VGAConsole)];
	int init = 1;
} // namespace 

VGAConsole& VGAConsole::getInstance()
{
	if (init == 1)
	{
		new (&instance) VGAConsole();
		init = 0;
	}
	return reinterpret_cast<VGAConsole&>(instance);
}

VGAConsole::VGAConsole():
	_idx(0),
	_attr(fgcolor::ltgray bitor bgcolor::black bitor blink::off)
#ifdef REENTRENT_ON
	,_reentrance(0)
#endif
	{
		const unsigned char* biosCursorRow = reinterpret_cast<const unsigned char*>(0x0451);
		if (*biosCursorRow <= 25)
			_idx = *biosCursorRow * line_len;
		dbg("Console() x % y %\n", _idx % line_len, _idx / line_len);
	}

void VGAConsole::write(const char *string)
{
	assert(init == 0);
#ifdef REENTRENT_ON
	if (_reentrance)
		return;
	_reentrance = 1;
#endif
	while (*string != 0)
	{
		putChar(*string);
		string++;
	}
	updateVGACursor();
#ifdef REENTRENT_ON
	_reentrance = 0;
#endif
}

void VGAConsole::write(const char c)
{
	assert(init == 0);
#ifdef REENTRENT_ON
	if (_reentrance)
		return;
	_reentrance = 1;
#endif
	putChar(c);
	updateVGACursor();
#ifdef REENTRENT_ON
	_reentrance = 0;
#endif
}

void VGAConsole::setAttr(const Attr& attr)
{
	// CONVERTIR CONNARD
	// TODO
}

void VGAConsole::putChar(const char c)
{
	assert(_idx < _video_mem.size());
	switch (c)
	{
		case '\n':
			{
				_idx = ((_idx / line_len) + 1) * line_len;
				break;
			}
		case '\t':
			{
				static const int tab_len = 8;
				_idx = ((_idx / tab_len) + 1) * tab_len;
				break;
			}
		case '\r':
			{
				_idx = (_idx / line_len) * line_len;
				break;
			}
		default: 
			{
				_video_mem[_idx++] = { c, _attr };
			}
	}
	
	if (_idx == _video_mem.size())
	{
		kernel::std::copy(_video_mem.begin() + line_len, _video_mem.end(),
				_video_mem.begin());
		kernel::std::fill(_video_mem.end() - line_len, _video_mem.end(),
				vga_char{ 0, fgcolor::ltgray bitor bgcolor::black });
		_idx -= line_len;
		assert(_idx < _video_mem.size());
	}
}		

void VGAConsole::updateVGACursor()
{
	// kernel::io::out::byte(VGA_SET_CURSOR_HIGH, VGA_CMD_PORT);
	// kernel::io::out::byte((_idx >> 8), VGA_DTA_PORT);
	// kernel::io::out::byte(VGA_SET_CURSOR_LOW, VGA_CMD_PORT);
	// kernel::io::out::byte((_idx bitand 0xFF), VGA_DTA_PORT);

	kernel::io::out::word((_idx bitand 0xFF00) bitor VGA_SET_CURSOR_HIGH, VGA_CMD_PORT);
	kernel::io::out::word((_idx << 8) bitor VGA_SET_CURSOR_LOW, VGA_CMD_PORT);
}

} // namespace kernel
