#ifndef __H_CONSOLE__
#define __H_CONSOLE__

#include <kernel/std/array.hpp>
#define RAMSCREEN 0xB8000
#define LINE 25
#define ROW 80

namespace kernel {
  namespace std {
    enum class color : char{
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
      color attr;
    } PACKED;

    class console {
    public:
    console() : _idx(ROW), _color(color::white) { write("Constructeur Console\n"); }
      ~console() {}

      void write(const char *);
      void setColor(const color);

    private:
      void write(const char);

      int _idx;
      color _color;
      array<vga_char, ROW * LINE, buffer::absolute, RAMSCREEN> _video_mem;

    };
    

  } /* std */

} /* kernel */

#endif /* __H_CONSOLE__ */
