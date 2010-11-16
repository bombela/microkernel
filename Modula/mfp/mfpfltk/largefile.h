// Bombela - 03/11/2005

// Pour avoir un acc�s 64 bits sur les fichiers m�me en 32 bits.
// Et de mani�re ind�pendante de la plateforme.

#ifndef LARGEFILEHEADER
#define LARGEFILEHEADER

#define _FILE_OFFSET_BITS 64

#ifdef __WINNT__
#define _OFF_T_
#include <stdint.h>
typedef uint64_t off_t;
typedef uint64_t _off_t;
#define fseeko fseeko64
#define ftello ftello64
#endif

#endif
