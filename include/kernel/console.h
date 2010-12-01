#ifndef __H_CONSOLE__
#define __H_CONSOLE__

#include <kernel/std/array.hpp>
#define RAMSCREEN 0xB8000
#define LINE 25
#define ROW 80

namespace kernel {
  namespace std {
    enum class console_color : char{
      black=0,
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

    struct vga_char {
      char c;
      console_color attr;
    } PACKED;

    class console {
    public:
    console() : _kX(0), _kY(1), _kattr(console_color::realwhite) { write("Constructeur Console\n"); }
      ~console() {}

      void write(const char *);
      void setColor(const console_color);

    private:
      void write(const char);
      void scrollup(const unsigned int);

      char _kX;
      char _kY;
      console_color _kattr;
      array<vga_char, (2*ROW) * LINE, buffer::absolute, RAMSCREEN> _video_mem;

    };
    

  } /* std */

} /* kernel */

#endif /* __H_CONSOLE__ */
