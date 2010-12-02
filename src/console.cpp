#include <kernel/console.h>
#include <kernel/ioports.h>

namespace kernel {
	namespace std {
		
		console::console() : _idx(0), _color(color::white)
		{
			unsigned char *curRow = (unsigned char *)BIOSCURSORROW;
			if (*curRow <= 25)
				_idx = (*curRow)*ROW;
			write("Constructeur Console\n");
		}
  
		void console::write(const char *string)
		{
			while (*string != 0)
				{
					putChar(*string);
					string++;
				}
			updateVgaCursor();
		}

		void console::write(const char c)
		{
			putChar(c);
			updateVgaCursor();
		}
    
		void console::setColor(const color c)
		{
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
					array<vga_char, ROW * (LINE-1), buffer::absolute, RAMSCREEN> new_video_mem(_video_mem.begin()+ROW, _video_mem.end());
					auto i=_video_mem.end()-ROW;
					for(; i != _video_mem.end(); ++i)
						*i={0, color::white};
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
