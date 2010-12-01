#include <kernel/console.h>
namespace kernel {
  namespace std {
 
    void console::write(const char *string) {
      while (*string != 0) {
	write(*string);
	string++;
      }
    }
    
    void console::setColor(const console_color c) {
      _kattr = c;
    }

    void console::scrollup(const size_t n) {
      bool t = false;
      for(auto i = _video_mem.begin(); i!=_video_mem.end(); i++)
	{
	  if (i!=_video_mem.end()-ROW && !t)
	    *i = *(i+ROW);
	  else
	    *i = {0, console_color::white};
	  if (i==_video_mem.end()-ROW)
	    t= true;
	}

      _kY -= n;
      if (_kY < 0)
	_kY = 0;
      
    }

    void console::write(const char c) {
      switch (c) {
      case '\n': {
	_kX = 0;
	_kY++;
	break;
      }
      case '\t': {
	_kX = _kX + 8 - (_kX % 8);
	break;
      }
      case '\r': {
	_kX = 0;
	break;
      }
      default: {
	auto it_vmem = _video_mem.begin() + (_kY * ROW) + (_kX);
	*it_vmem = {c, _kattr};
	_kX++;
	if (_kX > 79) {
	  _kX = 0;
	  _kY++;
	}
      }

      }
      if (_kY > 24)
	scrollup(_kY - 24);
    }

  } /* std */
}/* kernel */
