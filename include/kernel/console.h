#ifndef __H_CONSOLE__
#define __H_CONSOLE__

// TODO if array debug or assert check reentrency
#include <kernel/std/array.hpp>

#define RAMSCREEN 0xB8000

#define BIOSCURSORROW 0x0451
#define VGA_CMD_PORT 0x3D4
#define VGA_DTA_PORT 0x3D5
#define VGA_SET_CURSOR_HIGH 0xE
#define VGA_SET_CURSOR_LOW 0xF

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
      console();
      ~console() {}

      void write(const char *);
      void write(const char);
      void setColor(const color);

    private:
      void putChar(const char);
      void printnb(const int num);
      void updateVgaCursor();

      int _idx;
      color _color;
      array<vga_char, ROW * LINE, buffer::absolute, RAMSCREEN> _video_mem;

    };
    

  } /* std */

} /* kernel */

#endif /* __H_CONSOLE__ */
