
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
    // MK: For some reason, glSamplePass() seems to be only available on MacOS-X.
    // GLEW doesn't know this function and i couldn't find any definition of it
    // anywhere on the internet. We handle this manually by only exposing it on
    // MacOS-X:
    #ifdef MACOSX
	    glSamplePass((GLenum)mxGetScalar(prhs[0]));
    #else
        mogl_glunsupported("glSamplePass");
    #endif
}

void gl_shadersource( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
    char* sourcestring;
    char** srcstrings;
    GLuint handle;
    int i, count, savedlength;
    count = 0;
    
    // Ok, glShaderSource needs a list of one-line strings for the single lines
    // of code in a shader program. We take a single big string of newline-
    // separated lines of code and break it up into a list of strings.
    
    // Retrieve handle to shader:
    handle = (GLuint) mxGetScalar(prhs[0]);
    // Retrieve shader source code string:
    sourcestring = mxArrayToString(prhs[1]);
    // Count number of lines in string:
    savedlength = strlen(sourcestring);
    for (i=0; i<savedlength; i++) if(sourcestring[i]=='\n') count++;
    count++;
    // printf("COUNT %i\n", count); fflush(NULL);
    // Allocate char* array of proper capacity:
    srcstrings=(char**) malloc(count * sizeof(char*));
    // Initialize array of char-ptrs and setup string:
    count=0;
    srcstrings[0]=(char*) sourcestring;
    count++;
    for (i=0; i<savedlength; i++) if(sourcestring[i]=='\n') {
        // NULL-out the end-of-line terminator to create a null-terminated piece of
        // substring:
        sourcestring[i]=0;
        // Setup a new char* that points behind the sourcestring[i]:
        i++;
        srcstrings[count]=(char*) &(sourcestring[i]);
        count++;
    }

    if (NULL == glShaderSource || mxGetScalar(prhs[2])>0) {
        printf("\n\n");
        for(i=0; i<count; i++) printf("Shader Line %i: %s\n", i, srcstrings[i]);
        printf("\n\n");
        fflush(NULL);
        // Free the sourcestring:
        mxFree(sourcestring);
        // Free our array:
        free(srcstrings);
        // Abort with error:
        mogl_glunsupported("glShaderSource");
    }
    
    // Ok, now srcstrings should be an array of count char*'s to single line, null-terminated
    // strings, suitable for glShaderSource. Call it.
    glGetError();
    glShaderSource(handle, count, (const char**) srcstrings, NULL);
    
    // Free the sourcestring:
    mxFree(sourcestring);
    // Free our array:
    free(srcstrings);
    
    // Done.
    return;
}

void gl_feedbackbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

  // Retrieve memory ptr from double argument:
  GLfloat* ptr = (GLfloat*) PsychDoubleToPtr((GLdouble) mxGetScalar(prhs[2]));

  if (NULL == glFeedbackBuffer) mogl_glunsupported("glFeedbackBuffer");

  glFeedbackBuffer((GLsizei)mxGetScalar(prhs[0]),
		   (GLenum)mxGetScalar(prhs[1]),
		   ptr);
}

void gl_selectbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

  // Retrieve memory ptr from double argument:
  GLuint* ptr = (GLuint*) PsychDoubleToPtr((GLdouble) mxGetScalar(prhs[1]));

  if (NULL == glSelectBuffer) mogl_glunsupported("glSelectBuffer");
  glSelectBuffer((GLsizei)mxGetScalar(prhs[0]), ptr);
}

void moglmalloc(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
  // Allocate a memory buffer of prhs[0] bytes size. ptr points to start of buffer:
  void* ptr = PsychMallocTemp((unsigned long) mxGetScalar(prhs[0]), 1);

  // Convert ptr into a double value and assign it as first return argument:
  plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
  *((GLdouble*) mxGetPr(plhs[0])) = PsychPtrToDouble(ptr);
}

void moglcalloc(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
  // Allocate a memory buffer of prhs[0] bytes size. ptr points to start of buffer:
  void* ptr = PsychCallocTemp((unsigned long) mxGetScalar(prhs[0]), (unsigned long) mxGetScalar(prhs[1]), 1);

  // Convert ptr into a double value and assign it as first return argument:
  plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
  *((GLdouble*) mxGetPr(plhs[0])) = PsychPtrToDouble(ptr);
}

void moglfree(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
  // Retrieve ptr to membuffer, encoded as double and convert it into a void*
  void* ptr = PsychDoubleToPtr((GLdouble) mxGetScalar(prhs[0]));

  // Free memory buffer:
  PsychFreeTemp(ptr, 1);
}

void moglfreeall(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
  PsychFreeAllTempMemory(1);
}

void moglcopybuffertomatrix(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
  int dims, type;
  GLenum mattype;
  GLfloat* dst;

  // Retrieve ptr to membuffer, encoded as double and convert it into a void*
  void* src = PsychDoubleToPtr((GLdouble) mxGetScalar(prhs[0]));

  // Retrieve size of buffer pointed to by src:
  unsigned int n = PsychGetBufferSizeForPtr(src);

  // Retrieve max number of bytes to copy:
  unsigned int nmax = (unsigned int) mxGetScalar(prhs[2]);

  if (nmax < n) n = nmax;

  // Retrieve type of matrix to create:
  mattype = (GLenum) mxGetScalar(prhs[1]);

  switch(mattype)
    {
    case GL_DOUBLE:
      type = mxDOUBLE_CLASS;
      dims = n / sizeof(double);
      if (dims * sizeof(double) < n) dims++;
      break;
    case GL_FLOAT:
      type = mxSINGLE_CLASS;
      dims = n / sizeof(float);
      if (dims * sizeof(float) < n) dims++;
      break;
    case GL_UNSIGNED_INT:
      type = mxUINT32_CLASS;
      dims = n / sizeof(unsigned int);
      if (dims * sizeof(unsigned int) < n) dims++;
      break;
    case GL_UNSIGNED_BYTE:
      type = mxUINT8_CLASS;
      dims = n / sizeof(unsigned char);
      if (dims * sizeof(unsigned char) < n) dims++;
      break;
    default:
      mexErrMsgTxt("MOGL-ERROR: Unknown matrix type requested in moglgetbuffer()! Ignored.");
    }

  // Allocate the beast:
  plhs[0] = mxCreateNumericArray(1, &dims, type, mxREAL);

  // Retrieve pointer to output matrix:
  dst = (GLfloat*) mxGetData(plhs[0]);

  // Do the copy:
  memcpy(dst, src, n);
}

void moglcopymatrixtobuffer(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
  // Retrieve ptr to membuffer, encoded as double and convert it into a void*
  void* dst = PsychDoubleToPtr((GLdouble) mxGetScalar(prhs[1]));

  // Retrieve pointer to input matrix:
  GLfloat* src = (GLfloat*) mxGetData(prhs[0]);

  // Retrieve size of buffer pointed to by dst:
  unsigned int nmax = PsychGetBufferSizeForPtr(dst);

  // Set final size of data to copy:
  unsigned int nin = (unsigned int) mxGetScalar(prhs[2]);
  if (nin > nmax) nin = nmax;

  // Do the copy:
  memcpy(dst, src, nin);
}

// command map:  moglcore string commands and functions that handle them
// *** it's important that this list be kept in alphabetical order, 
//     and that gl_manual_map_count be updated
//     for each new entry ***
int gl_manual_map_count=16;
cmdhandler gl_manual_map[] = {
{ "glFeedbackBuffer",               gl_feedbackbuffer                   },
{ "glGetBufferPointerv",            gl_getbufferpointerv                },
{ "glGetPointerv",                  gl_getpointerv                      },
{ "glGetString",                    gl_getstring                        },
{ "glGetVertexAttribPointerv",      gl_getvertexattribpointerv          },
{ "glSamplePass",                   gl_samplepass                       },
{ "glSelectBuffer",                 gl_selectbuffer                     },
{ "glShaderSource",                 gl_shadersource                     },
{ "gluErrorString",                 glu_errorstring                     },
{ "gluGetString",                   glu_getstring                       },
{ "moglcalloc",                     moglcalloc                          },
{ "moglcopybuffertomatrix",         moglcopybuffertomatrix              },
{ "moglcopymatrixtobuffer",         moglcopymatrixtobuffer              },
{ "moglfree",                       moglfree                            },
{ "moglfreeall",                    moglfreeall                         },
{ "moglmalloc",                     moglmalloc                          }};
