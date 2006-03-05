#ifndef _MOGLTYPE_H
#define _MOGLTYPE_H

/*
 * mogltype.h -- common definitions for gl/glu and glm modules
 *
 * 09-Dec-2005 -- created (RFM)
 *
*/

#include "mex.h"

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

/* To do: M$-Windows and GNU/Linux specific includes... */

// typedef for command map entries
typedef struct cmdhandler {
    char *cmdstr;
    void (*cmdfn)(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
} cmdhandler;

#endif
