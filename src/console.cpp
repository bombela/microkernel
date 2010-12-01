#include <kernel/console.h>
namespace kernel {
  namespace std {
 
    void console::write(char *string) {
      while (*string != 0) {
    	write(*string);
    	string++;
      }
    }

    void console::write(const char *string) {
      while (*string != 0) {
	write(*string);
	string++;
      }
    }
    
    void console::setColor(console_color c) {
      _kattr = c;
    }

    void console::scrollup(size_t n) {
      size_t index;

      for (index=0; index<_video_mem.size()-ROW; ++index)
	_video_mem[index] = _video_mem[index+ROW];
      for(;index<_video_mem.size();++index)
	_video_mem[index]=0;

      _kY -= n;
      if (_kY < 0)
	_kY = 0;

    }

    void console::write(char c) {
      size_t index;

      if (c == 10) {
	_kX = 0;
	_kY++;
      } else if (c == 9) {
	_kX = _kX + 8 - (_kX % 8);
      } else if (c == 13) {
	_kX = 0;
      } else {
	index = 2 * _kX + 160 * _kY;
	_video_mem[index] = c;
	_video_mem[index+1] = _kattr;
	_kX++;
	if (_kX > 79) {
	  _kX = 0;
	  _kY++;
	}
      }
      
      if (_kY > 24)
	scrollup(_kY - 24);
    }

  } /* std */
}/* kernel */
