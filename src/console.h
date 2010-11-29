#ifndef __H_CONSOLE__
#define __H_CONSOLE__

#define RAMSCREEN 0xB8000
#define SIZESCREEN 0xFA0
#define SCREENLIM 0xB8FA0

namespace kernel {
  
  namespace std {

    class console {
    public:
    console() : _kX(0), _kY(1), _kattr(0x07) { print("Constructeur Console\n"); }
      ~console() {}

      void print(char *);
      void print(const char *);

    private:
      void putcar(char);
      void scrollup(unsigned int);

      char _kX; /* = 0; */
      char _kY; /* = 8; */
      char _kattr; /* = 0x07; */

    };
    

  } /* std */

} /* kernel */

#endif /* __H_CONSOLE__ */
