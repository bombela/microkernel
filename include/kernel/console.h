#ifndef __H_CONSOLE__
#define __H_CONSOLE__

#include <kernel/std/array.hpp>

#define RAMSCREEN 0xB8000
#define LINE 25
#define ROW 160
/* #define SIZESCREEN 0xFA0 */
/* #define SCREENLIM 0xB8FA0 */

namespace kernel {
  
  namespace std {

    enum console_color {
      black=0x00,
      blue,
      green,
      cyan,
      red,
      purple,
      orange,
      white,
      grey,
      lightblue,
      lightgreen,
      lightcyan,
      lightred,
      lightpurple,
      yellow,
      realwhite
    };

    class console {
    public:
    console() : _kX(0), _kY(1), _kattr(realwhite) { write("Constructeur Console\n"); }
      ~console() {}

      void write(char *);
      void write(const char *);
      void setColor(console_color);

    private:
      void write(char);
      void scrollup(unsigned int);

      char _kX;
      char _kY;
      char _kattr;
      array<char, (ROW * LINE), buffer::absolute, RAMSCREEN> _video_mem;

    };
    

  } /* std */

} /* kernel */

#endif /* __H_CONSOLE__ */
