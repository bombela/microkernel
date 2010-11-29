#include "console.h"

namespace kernel {

  namespace std {

 
    void console::print(char *string) {
      while (*string != 0) {
	putcar(*string);
	string++;
      }
    }

    void console::print(const char *string) {
      int i = 0;
      while (string[i]){
	putcar(string[i]);
	i++;
      }
    }
    
    void console::scrollup(unsigned int n) {
      unsigned char *video, *tmp;

      for (video = (unsigned char *) RAMSCREEN;
	   video < (unsigned char *) SCREENLIM; video += 2) {
	tmp = (unsigned char *) (video + n * 160);

	if (tmp < (unsigned char *) SCREENLIM) {
	  *video = *tmp;
	  *(video + 1) = *(tmp + 1);
	} else {
	  *video = 0;
	  *(video + 1) = 0x07;
	}
      }

      _kY -= n;
      if (_kY < 0)
	_kY = 0;

    }

    void console::putcar(char c) {
      unsigned char *video;
      
      if (c == 10) {
	_kX = 0;
	_kY++;
      } else if (c == 9) {
	_kX = _kX + 8 - (_kX % 8);
      } else if (c == 13) {
	_kX = 0;
      } else {
	video = (unsigned char *) (RAMSCREEN + 2 * _kX + 160 * _kY);
	*video = c;
	*(video + 1) = _kattr;
	
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
