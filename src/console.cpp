#include <kernel/console.h>
namespace kernel {
  namespace std {
 
    void console::write(const char *string) {
      while (*string != 0) {
	write(*string);
	string++;
      }
    }
    
    void console::setColor(const color c) {
      _color = c;
    }

    void console::write(const char c) {
      switch (c) {
      case '\n': {
	_idx = (((_idx/ROW)+1)*ROW);
	break;
      }
      case '\t': {
	_idx = _idx + 8 - (_idx % 8);
	break;
      }
      case '\r': {
	_idx = ((_idx/ROW)*ROW);
	break;
      }
      default: {
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

  } /* std */
}/* kernel */
