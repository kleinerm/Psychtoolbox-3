
/*
 * gl_manual.c
 *
 * 19-Dec-2005 -- created (RFM)
 *
 */

#include "mogltypes.h"

void gl_getbufferpointerv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
    
	glGetBufferPointerv((GLenum)mxGetScalar(prhs[0]),
        (GLenum)mxGetScalar(prhs[1]),
        (GLvoid **)mxGetData(prhs[2]));

}

void gl_getpointerv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
    
	glGetPointerv((GLenum)mxGetScalar(prhs[0]),
		(GLvoid **)mxGetData(prhs[1]));

}

void gl_getstring( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

    plhs[0]=mxCreateString((const char *)glGetString((GLenum)mxGetScalar(prhs[0])));

}

void gl_getvertexattribpointerv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetVertexAttribPointerv((GLuint)mxGetScalar(prhs[0]),
        (GLenum)mxGetScalar(prhs[1]),
        (GLvoid **)mxGetData(prhs[2]));

}

void glu_errorstring( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

    plhs[0]=mxCreateString((const char *)gluErrorString((GLenum)mxGetScalar(prhs[0])));

}

void glu_getstring( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

    plhs[0]=mxCreateString((const char *)gluGetString((GLenum)mxGetScalar(prhs[0])));

}

void gl_samplepass( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	// MK: Need to define interface manually, as GLEW doesn't know this function, so
    // trouble with autocode.m and gl_auto.c
	glSamplePass((GLenum)mxGetScalar(prhs[0]));

}

// command map:  moglcore string commands and functions that handle them
// *** it's important that this list be kept in alphabetical order, 
//     and that gl_manual_map_count be updated
//     for each new entry ***
int gl_manual_map_count=7;
cmdhandler gl_manual_map[] = {
{ "glGetBufferPointerv",            gl_getbufferpointerv                },
{ "glGetPointerv",                  gl_getpointerv                      },
{ "glGetString",                    gl_getstring                        },
{ "glGetVertexAttribPointerv",      gl_getvertexattribpointerv          },
{ "gluErrorString",                 glu_errorstring                     },
{ "gluGetString",                   glu_getstring                       },
{ "glSamplePass",                   gl_samplepass                       }};

