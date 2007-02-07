
/*
 * al_manual.c
 *
 * 5-Feb-2007 -- created (MK)
 *
 */

#include "moaltypes.h"

void al_getstring( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

    plhs[0]=mxCreateString((const char *)alGetString((ALenum)mxGetScalar(prhs[0])));

}

void alc_getstring( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

    plhs[0]=mxCreateString((const char *)alcGetString(alcGetContextsDevice(alcGetCurrentContext()), (ALenum)mxGetScalar(prhs[0])));
}

void moglmalloc(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
  // Allocate a memory buffer of prhs[0] bytes size. ptr points to start of buffer:
  void* ptr = PsychMallocTemp((unsigned long) mxGetScalar(prhs[0]), 1);

  // Convert ptr into a double value and assign it as first return argument:
  plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
  *((ALdouble*) mxGetPr(plhs[0])) = PsychPtrToDouble(ptr);
}

void moglcalloc(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
  // Allocate a memory buffer of prhs[0] bytes size. ptr points to start of buffer:
  void* ptr = PsychCallocTemp((unsigned long) mxGetScalar(prhs[0]), (unsigned long) mxGetScalar(prhs[1]), 1);

  // Convert ptr into a double value and assign it as first return argument:
  plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
  *((ALdouble*) mxGetPr(plhs[0])) = PsychPtrToDouble(ptr);
}

void moglfree(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
  // Retrieve ptr to membuffer, encoded as double and convert it into a void*
  void* ptr = PsychDoubleToPtr((ALdouble) mxGetScalar(prhs[0]));

  // Free memory buffer:
  PsychFreeTemp(ptr, 1);
}

void moglfreeall(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
  PsychFreeAllTempMemory(1);
}

void moglcopybuffertomatrix(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
/*
  int dims, type;
  ALenum mattype;
  ALfloat* dst;

  // Retrieve ptr to membuffer, encoded as double and convert it into a void*
  void* src = PsychDoubleToPtr((ALdouble) mxGetScalar(prhs[0]));

  // Retrieve size of buffer pointed to by src:
  unsigned int n = PsychGetBufferSizeForPtr(src);

  // Retrieve max number of bytes to copy:
  unsigned int nmax = (unsigned int) mxGetScalar(prhs[2]);

  if (nmax < n) n = nmax;

  // Retrieve type of matrix to create:
  mattype = (ALenum) mxGetScalar(prhs[1]);
  
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
      mexErrMsgTxt("MOAL-ERROR: Unknown matrix type requested in moglgetbuffer()! Ignored.");
    }

  // Allocate the beast:
  plhs[0] = mxCreateNumericArray(1, &dims, type, mxREAL);

  // Retrieve pointer to output matrix:
  dst = (ALfloat*) mxGetData(plhs[0]);

  // Do the copy:
  memcpy(dst, src, n);
*/
}

void moglcopymatrixtobuffer(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {
  // Retrieve ptr to membuffer, encoded as double and convert it into a void*
  void* dst = PsychDoubleToPtr((ALdouble) mxGetScalar(prhs[1]));

  // Retrieve pointer to input matrix:
  ALfloat* src = (ALfloat*) mxGetData(prhs[0]);

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
int gl_manual_map_count=8;
cmdhandler gl_manual_map[] = {
{ "alGetString",                    al_getstring                        },
{ "alcGetString",                   alc_getstring                       },
{ "moglcalloc",                     moglcalloc                          },
{ "moglcopybuffertomatrix",         moglcopybuffertomatrix              },
{ "moglcopymatrixtobuffer",         moglcopymatrixtobuffer              },
{ "moglfree",                       moglfree                            },
{ "moglfreeall",                    moglfreeall                         },
{ "moglmalloc",                     moglmalloc                          }};
