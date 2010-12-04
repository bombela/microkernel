
#include <kernel/console.h>
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

namespace kernel {
	namespace std {

		namespace {
			char instance[sizeof (console)];
			int init = 1;
		}

		void console::initInstance()
		{
			assert(init == 1);
			new (&instance) console();
			init = 0;
		}
		
		console& console::getInstance()
		{
			assert(init == 0);
			return reinterpret_cast<console&>(instance);
		}

		console::console():
			_idx(0),
			_color(color::white)
#ifdef REENTRENT_ON
			,_reentrance(0)
#endif
		{
			unsigned char *curRow = (unsigned char *)BIOSCURSORROW;
			if (*curRow <= 25)
				_idx = (*curRow)*ROW;
			write("Constructeur Console\n");
		}
  
		void console::write(const char *string)
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
			updateVgaCursor();
#ifdef REENTRENT_ON
			_reentrance = 0;
#endif
		}

		void console::write(const char c)
		{
			assert(init == 0);
#ifdef REENTRENT_ON
			if (_reentrance)
				return;
			_reentrance = 1;
#endif
			putChar(c);
			updateVgaCursor();
#ifdef REENTRENT_ON
			_reentrance = 0;
#endif
		}
    
		void console::setColor(const color c)
		{
			assert(init == 0);
			_color = c;
		}

		void console::putChar(const char c)
		{
			switch (c)
				{
				case '\n':
					{
						_idx = (((_idx/ROW)+1)*ROW);
						break;
					}
				case '\t':
					{
						_idx = _idx + 8 - (_idx % 8);
						break;
					}
				case '\r':
					{
						_idx = ((_idx/ROW)*ROW);
						break;
					}
				default: 
					{
						auto it_vmem = _video_mem.begin() + _idx;
						*it_vmem = {c, _color};
						_idx++;
					}
				}
			
			if (_idx > ((LINE-1)*ROW))
				{
					kernel::std::copy(_video_mem.begin()+ROW, _video_mem.end(), _video_mem.begin());
					kernel::std::fill(_video_mem.end()-ROW, _video_mem.end(), vga_char{0, color::white});
					_idx -= ROW;
				}
		}		

		void console::updateVgaCursor()
		{
			// kernel::io::out::byte(VGA_SET_CURSOR_HIGH, VGA_CMD_PORT);
			// kernel::io::out::byte((_idx >> 8), VGA_DTA_PORT);
			// kernel::io::out::byte(VGA_SET_CURSOR_LOW, VGA_CMD_PORT);
			// kernel::io::out::byte((_idx bitand 0xFF), VGA_DTA_PORT);

			kernel::io::out::word((_idx bitand 0xFF00) bitor VGA_SET_CURSOR_HIGH, VGA_CMD_PORT);
			kernel::io::out::word((_idx << 8) bitor VGA_SET_CURSOR_LOW, VGA_CMD_PORT);
		}

	} /* std */
}/* kernel */
