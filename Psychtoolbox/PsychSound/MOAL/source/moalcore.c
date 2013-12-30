/*
 *
 * moalcore.cc -- MATLAB MEX and GNU/OCTAVE OCT file interface to OpenAL.
 *
 * 08-May-2005 -- created (RFM)
 * 08-Dec-2005 -- reworked into direct interface to gl, glu, and glm functions (RFM)
 * 05-Mar-2006 -- reworked to make inclusion of glm optional (for Psychtoolbox) (MK)
 * 20-Mar-2006 -- Included support for GLEW lib for auto-detection of OpenGL extensions. (MK)
 * 15-Apr-2006 -- Dynamic rebinding of OpenGL-2 core functions to ARB extensions. (MK)
 * 16-Apr-2006 -- Built-in error detection and handling via the debuglevel parameter. (MK)
 * 16-May-2006 -- Implementation of a buffer memory manager for dynamic buffer allocation and such.
 *                This is needed for commands like glFeedbackBuffer() to work properly.
 * 19-Jun-2006 -- Implement support for GNU/Octave (MK).
 * 07-Feb-2007 -- Derived from moglcore (MK).
 * 27-Mar-2011 -- Make 64-bit clean, remove totally bitrotten Octave-2 support (MK).
 *
 */

#include "moaltypes.h"

// alm command support:
extern int glm_map_count;
extern cmdhandler glm_map[];

// main command dispatch support for manual and auto-generated code:
extern int gl_manual_map_count, gl_auto_map_count;
extern cmdhandler gl_manual_map[], gl_auto_map[];

// Flag that signals first real invocation of moglcore:
static int firsttime = 1;

// Debuglevel: Defines if moglcore should check for errors and how to respond to them.
static int debuglevel = 1;

// Dummymode: If set to a value greater than zero, moglcore does not really execute
// commands anymore. 
static int dummymode = 0;

// command string
#define CMDLEN 64
char cmd[CMDLEN];

// Audio shutdown routine:
extern void glm_doclose(void);

// binary search routine
int binsearch(cmdhandler *map, int mapsize, char *str);

// error handler
void mogl_usageerr();

// Dynamic extension rebinding:
void mogl_rebindARBExtensionsToCore(void);

// Automatic checking and handling for glError's and ALSL errors.
void mogl_checkerrors(const char* cmd, const mxArray *prhs[]);

void mexExitFunction(void)
{
  // Shutdown OpenAL:
  glm_doclose();    
  firsttime = 1;
}

/* This is the main entry point for Matlab or Octave. It gets called by
   Matlab or Octave, handles first-time initialization, error handling
   and subfunction dispatching.
*/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // Start of dispatcher:
    int i;

    // see whether there's a string command
    if(nrhs<1 || !mxIsChar(prhs[0])) mogl_usageerr();
    
    // get string command
    mxGetString(prhs[0],cmd,CMDLEN);

    // Special case. If we're called with the special command "PREINIT", then
    // we return immediately. moalcore('PREINIT') is called by InitializeMatlabOpenAL.m
    // on M$-Windows in order to preload the moalcore Mex-file into Matlab while
    // the current working directory is set to ..MOAL/core/ . This way, the dynamic
    // linker can find our own local version of openalXX.dll and link it against moalcore,
	// retry in system folders and fail gracefully if OpenAL is not properly installed.
    if (strcmp(cmd, "PREINIT")==0) {
        goto moglreturn;
    }
    
    if (strcmp(cmd, "DUMMYMODE")==0) {
        if (nrhs<2 || mxGetScalar(prhs[1])<0) {
            mexErrMsgTxt("MOAL-ERROR: No dummy mode level or invalid level (<0) given for subcommand DUMMYMODE!");
        }
        dummymode = (int) mxGetScalar(prhs[1]);
		if (dummymode>0) printf("MOAL-INFO: Switched to dummy mode level %i.\n", dummymode);
        goto moglreturn;
    }
	
    // Special command to set MOAL debug level:
    // debuglevel = 0 --> Shut up in any case, leave error-handling to higher-level code.
    // debuglevel > 0 --> Check for OpenAL error conditions.
    // debuglevel > 0 --> Output alError()'s in clear-text and abort.
    // debuglevel > 1 --> Output ALSL diagnostic messages as well.
    // debuglevel > 2 --> Be very verbose!
    if (strcmp(cmd, "DEBUGLEVEL")==0) {
        if (nrhs<2 || mxGetScalar(prhs[1])<0) {
            mexErrMsgTxt("MOAL-ERROR: No debuglevel or invalid debuglevel (<0) given for subcommand DEBUGLEVEL!");
        }

        debuglevel = (int) mxGetScalar(prhs[1]);
        goto moglreturn;
    }

    // Abort here if dummymode >= 10: Input arg. processing run, but no real
    // command parsing and processing;
    if (dummymode >= 10) {
      printf("MOAL-INFO: Dummy call to %s() - Ignored in dummy mode %i ...\n", cmd, dummymode);
      goto moglreturn;
    }

    // look for command in glm command map
    if( (i=binsearch(glm_map,glm_map_count,cmd))>=0 ) {
        glm_map[i].cmdfn(nlhs,plhs,nrhs-1,prhs+1);
        goto moglreturn;
    }

    // Is this the first invocation of moalcore?
    if (firsttime) {
        // Yes. Ready to go...
		if (debuglevel > 1) {
			printf("MOAL - OpenAL for Matlab & GNU/Octave initialized. MOAL is (c) 2007-2013 Richard F. Murray & Mario Kleiner, licensed to you under MIT license.\n");
			printf("See file 'License.txt' in the Psychtoolbox root folder for the exact licensing conditions.\n");
		}
        fflush(NULL);
        
        // Perform dynamic rebinding of ARB extensions to core functions, if necessary:
        // mogl_rebindARBExtensionsToCore();
        
		// Register exit-handler: When flushing the mex-file, we free all allocated buffer memory:
		mexAtExit(&mexExitFunction);
        // Done with first time initialization:
        firsttime = 0;
    }   
	
    // Reset OpenAL error state so we can be sure that any of our error queries really
    // relate to errors caused by us:
    if (debuglevel > 0 && strcmp(cmd, "alGetError")!=0) alGetError();
        
    // look for command in manual command map
    if( (i=binsearch(gl_manual_map,gl_manual_map_count,cmd))>=0 ) {
        gl_manual_map[i].cmdfn(nlhs,plhs,nrhs-1,(const mxArray**) prhs+1);
        if (debuglevel > 0) mogl_checkerrors(cmd, prhs);
        goto moglreturn;
    }
    
    // look for command in auto command map
    if( (i=binsearch(gl_auto_map,gl_auto_map_count,cmd))>=0 ) {
        gl_auto_map[i].cmdfn(nlhs,plhs,nrhs-1,(const mxArray**) prhs+1);
        if (debuglevel > 0) mogl_checkerrors(cmd, prhs);
        goto moglreturn;
    }
    
    // no match
    mogl_usageerr();
    
    // moglreturn: Is the exit path of moal. All execution ends at this point.
moglreturn:
		
	return;
}

// do binary search in a command map for a command string
int binsearch(cmdhandler *map, int mapsize, char *str) {
    int m=0,n=mapsize-1,i,k,count=0;
    while( m<=n && count < 100) {
      count++;
        i=(int)((m+n)/2);
        k=strcmp(str,map[i].cmdstr);
        if( k==0 )
            return( i );
        else if( k<0 )
            n=i-1;
        else
            m=i+1;
    }
    return( -1 );
}

// error handler
void mogl_usageerr() {
    printf("MOAL-Command: %s\n", cmd);
    mexErrMsgTxt("invalid moalcore command");
}

// Error handler for unsupported core OpenAL functions or extensions.
// This handler gets called by the subroutines in gl_auto.c and gl_manual.c
// whenever a al function is not bound == not supported by current OS/driver/gfx-hardware.
void mogl_glunsupported(const char* fname)
{
    char errtxt[1000];
    sprintf(errtxt, "MOAL-Error: Your Matlab code tried to call the OpenAL function %s(), which is not supported\n"
                    "MOAL-Error: by your combination of sound hardware + sound device driver.\n"
                    "MOAL-Error: You'll have to download+install the latest sound-drivers for your sound-hardware\n"
                    "MOAL-Error: or upgrade your sound-hardware with more recent one to use this function. Aborted.\n\n", fname);

    // Exit to Matlab prompt with error message:
    mexErrMsgTxt(errtxt);
}

// Dynamic rebinding of extensions to core routines:
// In this function, we try to detect OS dependent quirks and try to work around them...
void mogl_rebindARBExtensionsToCore(void)
{   
    // Currently pretty much a no operation...
    return;
}

void mogl_checkerrors(const char* cmd, const mxArray *prhs[]) 
{
    char errtxt[10000];
    ALenum err;

    // Reject no-op calls:
    if (debuglevel<=0) return;
    
    // Check for alErrors():
    if ((err=alGetError())>0) {
        // Last command caused an OpenAL error condition: Report it and abort.
        sprintf(errtxt, "MOAL-Error: Your OpenAL command %s() caused the following OpenAL error: %s. Aborted.\n", cmd, alGetString(err));
        // Exit to Matlab prompt with error message:
        mexErrMsgTxt(errtxt);
    }
    
    return;
}
