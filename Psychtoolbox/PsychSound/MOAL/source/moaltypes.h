#ifndef _MOALTYPE_H
#define _MOALTYPE_H

/*
 * moaltype.h -- common definitions for al/alc and alm modules
 *
 * 05-Feb-2007 -- created (MK)
 * 24-Mar-2011 -- Make 64-bit clean, remove totally bitrotten Octave-2 support (MK).
 * 01-Jul-2012 -- Kill Matlab R11 compatibility cruft (MK).
 *
 */

#define PSYCH_MATLAB 0
#define PSYCH_LANGUAGE PSYCH_MATLAB

// Need to include uchar.h before mex.h for definition of
// char16_t, which seems to be missing on recent compilers
// by default. Otherwise compile failure at least under
// Matlab R2012a with current Linux or OSX. On Linux this
// solves it. On OSX 10.9 we instead -Dchar16_t=UINT16_T in mexopts.sh
// and are fine again.
#ifdef LINUX
#include <uchar.h>
#endif

// Include mex.h with MEX - API definition for Matlab:
#include "mex.h"

/* Includes specific to MacOS-X version of moal: */
#ifdef MACOSX

#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <OpenAL/MacOSX_OALExtensions.h>
// Prototypes for ASA extensions for Reverb etc. (used in al_manual.c):
ALenum  alcASASetSource(const ALuint property, ALuint source, ALvoid *data, ALuint dataSize);
ALenum  alcASASetListener(const ALuint property, ALvoid *data, ALuint dataSize);
// And for OSX rendering quality selection, now needed since 10.9 - Thanks Apple!
ALvoid  alcMacOSXRenderingQuality(ALint value);
#endif

/* Includes specific to GNU/Linux version of moal: */
#ifdef LINUX

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <AL/al.h>
#include <AL/alc.h>

#endif

/* Includes specific to M$-Windows version of moal: */
#ifdef WINDOWS

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <al.h>
#include <alc.h>

#endif

#define printf mexPrintf

// Function prototype for error handler for unsupported al-Functions.
void mogl_glunsupported(const char* fname);

// typedef for command map entries
typedef struct cmdhandler {
    char *cmdstr;
    void (*cmdfn)(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
} cmdhandler;

// Definition of unsigned int 64 bit datatype for Windows vs. Unix
#ifndef WINDOWS
typedef unsigned long long int psych_uint64;
#else
typedef ULONGLONG psych_uint64;
#endif

#endif
