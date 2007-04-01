#ifndef _MOGLTYPE_H
#define _MOGLTYPE_H

/*
 * mogltype.h -- common definitions for gl/glu and glm modules
 *
 * 09-Dec-2005 -- created (RFM)
 *
*/

#define PSYCH_MATLAB 0
#define PSYCH_OCTAVE 1

#ifndef PTBOCTAVE
// Include mex.h with MEX - API definition for Matlab:
#define PSYCH_LANGUAGE PSYCH_MATLAB
#include "mex.h"


#else


// Include oct.h and friends with OCT - API definitions for Octave:
#define PSYCH_LANGUAGE PSYCH_OCTAVE
#include <octave/oct.h>
#include <setjmp.h>

// MK: These three are probably not needed for moglcore:
#include <octave/parse.h>
#include <octave/ov-struct.h>
#include <octave/ov-cell.h>

#define const

// octavemex.h defines pseudo MEX functions emulated by our code:
#include "octavemex.h"

char* mxArrayToString(mxArray* arrayPtr);

#define printf mexPrintf

// Define this to 1 if you want lots of debug-output for the Octave-Scripting glue.
#define DEBUG_PTBOCTAVEGLUE 0

#define MAX_OUTPUT_ARGS 100
#define MAX_INPUT_ARGS 100
static mxArray* plhs[MAX_OUTPUT_ARGS]; // An array of pointers to the octave return arguments.
static mxArray* prhs[MAX_INPUT_ARGS];  // An array of pointers to the octave call arguments.
static bool jettisoned = false;

#endif

/* glew.h is part of GLEW library for automatic detection and binding of
   OpenGL core functionality and extensions.
 */
#include "glew.h"
/* Hack to take care of missing function prototypes in current glew.h */
#define glPointParameteri glPointParameteriNV
#define glPointParameteriv glPointParameterivNV

/* Includes specific to MacOS-X version of mogl: */
#ifdef MACOSX

//#include <ApplicationServices/ApplicationServices.h>
//#include <Carbon/Carbon.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
//#include <AGL/agl.h>
#endif

/* Includes specific to GNU/Linux version of mogl: */
#ifdef LINUX

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "glxew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#endif

/* Includes specific to M$-Windows version of mogl: */
#ifdef WINDOWS
#include <stdlib.h>
#include <string.h>
#include "wglew.h"
#include <GL/glut.h>

/* Hacks to get Windows version running - to be replaced soon. */
double gluCheckExtension(const GLubyte* a, const GLubyte* b);
double gluUnProject4(double a1, double a2, double a3, double a4, double* a5, double* a6, int* a7, double a8, double a9, double* a10, double* a11, double* a12, double* a13);
double gluBuild1DMipmapLevels(double a1, double a2, double a3, double a4, double a5, double a6, double a7, double a8, void* a9);
double gluBuild2DMipmapLevels(double a1, double a2, double a3, double a4, double a5, double a6, double a7, double a8, double a9, void* a10);
double gluBuild3DMipmapLevels(double a1, double a2, double a3, double a4, double a5, double a6, double a7, double a8, double a9, double a10, void* a11);
double gluBuild3DMipmaps(double a1, double a2, double a3, double a4, double a5, double a6, double a7, void* a8);
mxArray* mxCreateNumericMatrix(int m, int n, int class, int complex);

#endif

// Function prototype for error handler for unsupported gl-Functions.
void mogl_glunsupported(const char* fname);

// typedef for command map entries
typedef struct cmdhandler {
    char *cmdstr;
    void (*cmdfn)(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
} cmdhandler;

// Our own little buffer memory manager: This code is adapted from
// Psychtoolbox's PsychMemory.h/.c routines:

// Definition of unsigned int 64 bit datatype for Windows vs. Unix
#ifndef WINDOWS
typedef unsigned long long int psych_uint64;
#else
typedef LONG psych_uint64;
#endif

// Convert a double value (which encodes a memory address) into a ptr:
void*  PsychDoubleToPtr(double dptr);

// Convert a memory address pointer into a double value:
double PsychPtrToDouble(void* ptr);

// Return the size of the buffer pointed to by ptr in bytes.
// CAUTION: Only works with buffers allocated via PsychMallocTemp()
// or PsychCallocTemp(). Will segfault, crash & burn with other pointers!
unsigned int PsychGetBufferSizeForPtr(void* ptr);

// Allocate and zero-out n elements of memory, each size bytes big.
void *PsychCallocTemp(unsigned long n, unsigned long size, int mlist);

// Allocate n bytes of memory:
void *PsychMallocTemp(unsigned long n, int mlist);

// PsychFreeTemp frees memory allocated with PsychM(C)allocTemp().
// This is not strictly needed as our memory manager will free
// the memory anyway when this file is flushed:
void PsychFreeTemp(void* ptr, int mlist);

// Master cleanup routine: Frees all allocated memory.
void PsychFreeAllTempMemory(int mlist);

#endif
