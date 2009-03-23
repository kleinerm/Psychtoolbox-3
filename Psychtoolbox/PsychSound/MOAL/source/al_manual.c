
/*
 * al_manual.c
 *
 * 5-Feb-2007 -- created (MK)
 *
 */

#include "moaltypes.h"

void alc_ASASetListener( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

    #ifdef MACOSX
        // Retrieve fourcc with property name:
        char rfourcc[5];
        char fourcc[4];
        ALuint* ptype = (ALuint*) &fourcc[0];
        mxGetString(prhs[0], rfourcc, sizeof(rfourcc));

        // Swap left-right:
        fourcc[0]=rfourcc[3];
        fourcc[1]=rfourcc[2];
        fourcc[2]=rfourcc[1];
        fourcc[3]=rfourcc[0];

        if (NULL == alcASASetListener) mogl_glunsupported("alcASASetListener");
        plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
        *mxGetPr(plhs[0])= (double) alcASASetListener((const ALuint) *ptype, (ALvoid *) mxGetData(prhs[1]), (ALuint) mxGetScalar(prhs[2]));
    #else
        mogl_glunsupported("alcASASetListener");
    #endif
}

void alc_ASASetSource( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

    #ifdef MACOSX
        // Retrieve fourcc with property name:
        char rfourcc[5];
        char fourcc[4];
        ALuint* ptype = (ALuint*) &fourcc[0];
        mxGetString(prhs[0], rfourcc, sizeof(rfourcc));

        // Swap left-right:
        fourcc[0]=rfourcc[3];
        fourcc[1]=rfourcc[2];
        fourcc[2]=rfourcc[1];
        fourcc[3]=rfourcc[0];

        if (NULL == alcASASetSource) mogl_glunsupported("alcASASetSource");

        plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
        *mxGetPr(plhs[0])= (double) alcASASetSource((const ALuint) *ptype, (ALuint) mxGetScalar(prhs[1]), (ALvoid *) mxGetData(prhs[2]), (ALuint) mxGetScalar(prhs[3]));
    #else
        mogl_glunsupported("alc_ASASetSource");
    #endif
}

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
int gl_manual_map_count=10;
cmdhandler gl_manual_map[] = {
{ "alGetString",                    al_getstring                        },
{ "alcASASetListener",              alc_ASASetListener                  },
{ "alcASASetSource",                alc_ASASetSource                    },
{ "alcGetString",                   alc_getstring                       },
{ "moglcalloc",                     moglcalloc                          },
{ "moglcopybuffertomatrix",         moglcopybuffertomatrix              },
{ "moglcopymatrixtobuffer",         moglcopymatrixtobuffer              },
{ "moglfree",                       moglfree                            },
{ "moglfreeall",                    moglfreeall                         },
{ "moglmalloc",                     moglmalloc                          }};
