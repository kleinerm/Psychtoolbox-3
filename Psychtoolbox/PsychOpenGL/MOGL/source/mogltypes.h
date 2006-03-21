#ifndef _MOGLTYPE_H
#define _MOGLTYPE_H

/*
 * mogltype.h -- common definitions for gl/glu and glm modules
 *
 * 09-Dec-2005 -- created (RFM)
 *
*/

#include "mex.h"

/* glew.h is part of GLEW library for automatic detection and binding of
   OpenGL core functionality and extensions.
 */
#include "glew.h"

/* Includes specific to MacOS-X version of mogl: */
#ifdef MACOSX

#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <AGL/agl.h>

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
#define glPointParameteri glPointParameteriNV
#define glPointParameteriv glPointParameterivNV
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

#endif
