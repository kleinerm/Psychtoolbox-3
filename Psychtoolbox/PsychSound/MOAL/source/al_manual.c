
/*
 * al_manual.c
 *
 * 5-Feb-2007  -- created (MK)
 * 27-Mar-2011 -- Remove totally unused functions (MK)
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

// command map:  moalcore string commands and functions that handle them
// *** it's important that this list be kept in alphabetical order, 
//     and that gl_manual_map_count be updated
//     for each new entry ***
int gl_manual_map_count=4;
cmdhandler gl_manual_map[] = {
{ "alGetString",                    al_getstring                        },
{ "alcASASetListener",              alc_ASASetListener                  },
{ "alcASASetSource",                alc_ASASetSource                    },
{ "alcGetString",                   alc_getstring                       }};
