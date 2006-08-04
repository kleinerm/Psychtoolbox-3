/* Memorybuffer2Texture.cc -- Minimal example of how to
 * pass a pointer to a memory buffer to Psychtoolbox,
 * so that one can create a texture out of its content.
 *
 * This simple example creates a 640 x 480 pixels color
 * image in a memory buffer and returns a pointer to it
 * to Octave, encoded as a double value: The way we encode
 * the pointer as double may look clumsy, but it guarantees
 * that it works even on 64-Bit systems.
 *
 * Compile with: mkoctfile Memorybuffer2Texture.cc
 * Then run Memorybuffer2TextureDemo.m
 *
 * Written by Mario Kleiner.
 */

#include <stdlib.h>
#include "octave/oct.h"

/* Byte array with 640 x 480 greyscale pixels */
static float rawimage[640*480*3];

/* Definition of a 64-bit number: Operating system dependend: */
#ifndef __WIN32__
/* Unix style: */
typedef unsigned long long  psych_uint64;
#else
/* Microsoft Windows style: */
typedef LONG  psych_uint64;
#endif

/* Convert a memory address pointer into a double value: */
double PtrToDouble(void* ptr)
{
  volatile psych_uint64 ival = (psych_uint64) ptr;
  volatile double* dptr = (double*) &ival;
  volatile double outval = *dptr;
  return(outval);
}

DEFUN_DLD(Memorybuffer2Texture, inargs, nargout,
"MemoryBuffer2Texture -- Octave version\n"
"See MemoryBuffer2TextureDemo.m for help and info.")
{
  octave_value_list outarg;
  int x,y, pos;
  double dblimgptr;
  
  /* Fill our rawimage image array with content: */
  pos = 0;
  for(y=0; y<480; y++)
    for (x=0; x<640; x++) {
      // Red channel:
      rawimage[pos++] = (float)x/640;
      // Green channel:
      rawimage[pos++] = (float)y/480;
      // Blue channel:
      rawimage[pos++] = 0;
    }

  /* Convert our memory pointer to something Psychtoolbox
     understands, i.e., convert it into a double value:
  */
  dblimgptr = PtrToDouble((void*) &rawimage);
  // printf("InTexPtr %p --> %.20e", (void*) &rawimage, dblimgptr);

  /* Return double-encoded pointer to Octave: */
  outarg(0) = octave_value(dblimgptr);
  return(outarg);
}
