/* Thin C++ wrapper around the tinyexr.h header file only OpenEXR library.
 *
 * Used to isolate Psychtoolbox / Screen()'s regular C source code from the
 * C++ code needed to compile tinyexr.h.
 *
 * SCREENReadHDRImage.c uses functions imported from here with C linkage
 * to utilize tinyexr for reading of HDR files.
 *
 */

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

// Include tinyexr header only library. Currently tinyexr.h contains a
// snapshot of upstream https://github.com/syoyo/tinyexr at commit
// cf8550f1b8b9f5f79f02df810c885ed2a2b578f9 ("Merge branch 'AdrianAtGoogle-master").

#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"
