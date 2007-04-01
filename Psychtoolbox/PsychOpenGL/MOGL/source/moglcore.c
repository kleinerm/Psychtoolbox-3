/*
 *
 * moglcore.cc -- MATLAB MEX and GNU/OCTAVE OCT file interface to OpenGL.
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
 */

#include "mogltypes.h"

/* Build and include support for glm if BUILD_GLM is defined.
   Otherwise, only build OpenGL wrappers.
*/

#ifdef BUILD_GLM
extern int glm_map_count;
extern cmdhandler glm_map[];
#endif

extern int gl_manual_map_count, gl_auto_map_count;
extern cmdhandler gl_manual_map[], gl_auto_map[];

// Pointer to the start of our list of allocated temporary memory buffers:
static unsigned int* PsychTempMemHead[2] = { NULL, NULL };

// Total count of allocated memory in Bytes:
static int totalTempMemAllocated[2] = { 0 , 0 };

// Flag that signals first real invocation of moglcore:
static int firsttime = 1;

// Debuglevel: Defines if moglcore should check for errors and how to respond to them.
static int debuglevel = 1;

// Dummymode: If set to a value greater than zero, moglcore does not really execute
// commands anymore. 
static int dummymode = 0;

// This flag is > 0 if we are in a section started by glBegin() in that case we are
// not allowed to call glGetError() as that itself is an error, until the corresponding
// glEnd gets executed.
static int glBeginLevel = 0;

// command string
#define CMDLEN 64
char cmd[CMDLEN];

// binary search routine
int binsearch(cmdhandler *map, int mapsize, char *str);

// error handler
void mogl_usageerr();

// Dynamic extension rebinding: This is in an extra file mogl_rebinder.c and defined
// with explicit C linkage. Reason: For the Octave version, we need to compile all
// files (except glew.c) as C++, not C, otherwise won't work. The C++ compiler is
// more picky about function pointer assignments and will abort for hacks like in
// mogl_rebind...  -> Move this function into separate file, compile that file as
// C file, make the compiler happy.
extern "C" void mogl_rebindARBExtensionsToCore(void);

// Automatic checking and handling for glError's and GLSL errors.
void mogl_checkerrors(const char* cmd, mxArray *prhs[]);

void mexExitFunction(void)
{
  // Release all memory in bufferlist 1 - The one that usually
  // persists over calls to moglcore.
  PsychFreeAllTempMemory(1);
  firsttime = 1;
}

/* This is the main entry point for Matlab or Octave. It gets called by
   Matlab or Octave, handles first-time initialization, error handling
   and subfunction dispatching.
*/
#if PSYCH_LANGUAGE == PSYCH_MATLAB
/* MEX interface function: Used as entry point for Matlab: */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
#endif

#if PSYCH_LANGUAGE == PSYCH_OCTAVE

// This jump-buffer stores CPU- and stackstate at the position
// where our octFunction() dispatcher actually starts executing
// the functions or subfunctions code. That is the point where
// a mexErrMsgTxt() will return control...
jmp_buf jmpbuffer;

#undef const

/* OCT interface function: Used as entry point for GNU/Octave: */
DEFUN_DLD(moglcore, inprhs, nlhs,
"moglcore - Octave core OCT file for 'OpenGL for Octave'.\n\n"
"You should *not* directly call moglcore, it is only called by\n"
"the OpenGL glXXX.m M-File wrappers in MOGL/wrap and by the InitializeMatlabOpenGL\n"
"M-File. The only interesting call you could make is moglcore('DEBUGLEVEL', x);\n"
"to specify a new level x of verbosity for debug output. If you want to call\n"
"any other moglcore subfunction, then make sure that you read and understand\n"
"moglcore's C source code!\n\n\n")
#endif
{
    // Start of dispatcher:
    int i;
    GLenum err;
    bool errorcondition = false;

       
#if PSYCH_LANGUAGE == PSYCH_OCTAVE
    #define const
    const char mexFunctionName[] = "moglcore";
    // outplhs is our octave_value_list of return values:
    octave_value tmpval;      // Temporary, needed in parser below...
    octave_value_list outplhs;   // Our list of left-hand-side return values...
    int nrhs = inprhs.length();

    // Abort here if dummymode >= 20. Skips basically everything so a call to
    // moglcore is really a complete "No operation".
    if (dummymode >= 20) return(octave_value());
    
    // Child protection: Is someone trying to call us after we've shut down already?
    if (jettisoned) {
      // Yep! Stupido...
      error("%s: Tried to call the module after it has been jettisoned!!! You need to do a 'clear %s;' now. Bug in Psychtoolbox?!?",
	    mexFunctionName, mexFunctionName);
      return(octave_value());
    }
    
    // Save CPU-state and stack at this position in 'jmpbuffer'. If any further code
    // calls an error-exit function like PsychErrorExit() or PsychErrorExitMsg() then
    // the corresponding longjmp() call in our mexErrMsgTxt() implementation (see top of file)
    // will unwind the stack and restore stack-state and CPU state to the saved values in
    // jmpbuffer --> We will end up at this setjmp() call again, with a cleaned up stack and
    // CPU state, but setjmp will return a non-zero error code, signaling the abnormal abortion.
    if (setjmp(jmpbuffer)!=0) {
      // PsychErrorExit() or friends called! The CPU and stack are restored to a sane state.
      // Call our cleanup-routine to release memory that is PsychMallocTemp()'ed and to other
      // error-handling...
      errorcondition = true;
      goto moglreturn;
    }

    // NULL-init our pointer array of call value pointers prhs:
    memset(&prhs[0], 0, sizeof(prhs));
    
    // Setup our prhs array of call argument pointers:
    // We make copies of prhs to simplify the rest of PsychScriptingGlue. This copy is not
    // as expensive as it might look, because Octave objects are all implemented via
    // "Copy-on-write" --> Only a pointer is copied as long as we don't modify the data.
    // MK: TODO FIXME -- Should we keep an extra array octave_value dummy[MAX_INPUT_ARGS];
    // around, assign to that dummy[i]=prhs(i); and set ptrs to it prhs[i]=&dummy[i];
    // This would require more memory, but prevent possible heap-fragmentation due to
    // lots of new()/delete() calls on each invocation of the OCT-Function --> possible
    // slow-down over time, could be confused with memory leaks???
    for(int i=0; i<nrhs && i<MAX_INPUT_ARGS; i++) {
      // Create and assign our mxArray-Struct:
      prhs[i] = (mxArray*) PsychMallocTemp(sizeof(mxArray), 0);
      
      // Extract data-pointer to each inprhs(i) octave_value and store a type-casted version
      // which is optimal for us.
      if (inprhs(i).is_string() || inprhs(i).is_char_matrix()) {
	// A string object:
	if (DEBUG_PTBOCTAVEGLUE) printf("INPUT %i: STRING\n", i); fflush(NULL);
	
	// Strings do not have a need for a data-ptr. Just copy the octave_value object...
	prhs[i]->d = NULL;
	prhs[i]->o = (void*) new octave_value(inprhs(i));  // Refcont now >= 2
	// Done.
      } 
      else if (inprhs(i).is_real_type() && !inprhs(i).is_scalar_type()) {
	// A N-Dimensional Array:
	if (DEBUG_PTBOCTAVEGLUE) printf("TYPE NAME %s\n", inprhs(i).type_name().c_str()); fflush(NULL);
	
	// Is it an uint8 or int8 NDArray?
	if (strstr(inprhs(i).type_name().c_str(), "int8")) {
	  // Seems to be an uint8 or int8 NDArray: Create an optimized uint8 object of it:
	  if (DEBUG_PTBOCTAVEGLUE) printf("INPUT %i: UINT8-MATRIX\n", i); fflush(NULL);
	  
	  // Create intermediate representation m: This is a shallow-copy...
	  const uint8NDArray m(inprhs(i).uint8_array_value()); // Refcount now >=2
	  
	  // Get internal dataptr from it:        // This triggers a deep-copy :(
	  prhs[i]->d = (void*) m.data();      // Refcount now == 1
	  
	  // Create a shallow backup copy of corresponding octave_value...
	  octave_value* ovptr = new octave_value();
	  *ovptr = m;
	  prhs[i]->o = (void*) ovptr;  // Refcont now == 2
	  
	  // As soon as m gets destructed by leaving this if-branch,
	  // the refcount will drop to == 1...
	  
	  // Done.
	}
	else 
	  // Is it an uint32 or int32 NDArray?
	  if (strstr(inprhs(i).type_name().c_str(), "uint32")) {
	    // Seems to be an uint32 NDArray: Create an optimized uint32 object of it:
	    if (DEBUG_PTBOCTAVEGLUE) printf("INPUT %i: UINT32-MATRIX\n", i); fflush(NULL);
	    
	    // Create intermediate representation m: This is a shallow-copy...
	    const uint32NDArray m(inprhs(i).uint32_array_value()); // Refcount now >=2
	    
	    // Get internal dataptr from it:        // This triggers a deep-copy :(
	    prhs[i]->d = (void*) m.data();      // Refcount now == 1
	    
	    // Create a shallow backup copy of corresponding octave_value...
	    octave_value* ovptr = new octave_value();
	    *ovptr = m;
	    prhs[i]->o = (void*) ovptr;  // Refcont now == 2
	    
	    // As soon as m gets destructed by leaving this if-branch,
	    // the refcount will drop to == 1...
	    
	    // Done.
	  } else
	    // Is it an int32 NDArray?
	    if (strstr(inprhs(i).type_name().c_str(), "int32")) {
	      // Seems to be an int32 NDArray: Create an optimized int32 object of it:
	      if (DEBUG_PTBOCTAVEGLUE) printf("INPUT %i: INT32-MATRIX\n", i); fflush(NULL);
	      
	      // Create intermediate representation m: This is a shallow-copy...
	      const int32NDArray m(inprhs(i).int32_array_value()); // Refcount now >=2
	      
	      // Get internal dataptr from it:        // This triggers a deep-copy :(
	      prhs[i]->d = (void*) m.data();      // Refcount now == 1
	      
	      // Create a shallow backup copy of corresponding octave_value...
	      octave_value* ovptr = new octave_value();
	      *ovptr = m;
	      prhs[i]->o = (void*) ovptr;  // Refcont now == 2
	      
	      // As soon as m gets destructed by leaving this if-branch,
	      // the refcount will drop to == 1...
	      
	      // Done.
	    } else {
	      // Seems to be a non-uint8 NDArray, i.e. bool type or double type.
	      if (DEBUG_PTBOCTAVEGLUE) printf("INPUT %i: DOUBLE-MATRIX\n", i); fflush(NULL);
	      
	      // We create a generic double NDArray from it...
	      
	      // Create intermediate representation m: This is a shallow-copy...
	      const NDArray m(inprhs(i).array_value()); // Refcount now >=2
	      
	      // Get internal dataptr from it:        // This triggers a deep-copy :(
	      prhs[i]->d = (void*) m.data();      // Refcount now == 1
	      
	      // Create a shallow backup copy of corresponding octave_value...
	      octave_value* ovptr = new octave_value();
	      *ovptr = m;
	      prhs[i]->o = (void*) ovptr;  // Refcont now == 2
	      
	      // As soon as m gets destructed by leaving this if-branch,
	      // the refcount will drop to == 1...
	      
	      // Done.
	    }
      } else if (inprhs(i).is_real_type() && inprhs(i).is_scalar_type()) {
	
	// A double or integer scalar value:
	if (DEBUG_PTBOCTAVEGLUE) printf("INPUT %i: SCALAR\n", i); fflush(NULL);
	prhs[i]->o = (void*) new octave_value(inprhs(i));
	// Special case: We allocate our own double value and store a
	// copy of the value in it.
	if (strstr(inprhs(i).type_name().c_str(), "uint32")) {
	  // uint32 scalar:
	  unsigned int* m = (unsigned int*) PsychMallocTemp(sizeof(unsigned int), 0);
	  *m = inprhs(i).uint_value();
	  prhs[i]->d = (void*) m;
	}
	else if (strstr(inprhs(i).type_name().c_str(), "int32")) {
	  // int32 scalar:
	  int* m = (int*) PsychMallocTemp(sizeof(int), 0);
	  *m = inprhs(i).int_value();
	  prhs[i]->d = (void*) m;
	}
	else {
	  // Double scalar:
	  double* m = (double*) PsychMallocTemp(sizeof(double), 0);
	  *m = inprhs(i).double_value();
	  prhs[i]->d = (void*) m;
	}
      }
      else {
	// Unkown argument type that we can't handle :(
	// We abort with a reasonable error message:
	prhs[i]=NULL;
	// We do, however, give an extra warning, as this could be Octave related...
	printf("PTB-WARNING: One of the values in the argument list was not recognized.\n");
	printf("PTB-WARNING: If your script runs well on Matlab then this may be a limitation or\n");
	printf("PTB-WARNING: bug in the GNU/Octave version of Psychtoolbox :( ...\n");
	mexErrMsgTxt("Unrecognized argument in list of command parameters.");
      }
    }
    
    // NULL-out our pointer array of return value pointers plhs:
    memset(&plhs[0], 0, sizeof(plhs));
#endif


    // see whether there's a string command
    if(nrhs<1 || !mxIsChar(prhs[0])) mogl_usageerr();
    
    // get string command
    mxGetString(prhs[0],cmd,CMDLEN);

    // Special case. If we're called with the special command "PREINIT", then
    // we return immediately. moglcore('PREINIT') is called by ptbmoglinit.m
    // on M$-Windows in order to preload the moglcore Mex-file into Matlab while
    // the current working directory is set to ..MOGL/core/ . This way, the dynamic
    // linker can find our own local version of glut32.dll and link it against moglcore.
    // Without this trick, we would need to install glut32.dll into the Windows system
    // folder which requires admin privileges and makes installation of Psychtoolbox
    // more complicated on M$-Windows...
    if (strcmp(cmd, "PREINIT")==0) {
        glBeginLevel=0;
        goto moglreturn;
    }
    
    if (strcmp(cmd, "DUMMYMODE")==0) {
        if (nrhs<2 || mxGetScalar(prhs[1])<0) {
            mexErrMsgTxt("MOGL-ERROR: No dummy mode level or invalid level (<0) given for subcommand DUMMYMODE!");
        }
        dummymode = (int) mxGetScalar(prhs[1]);
	if (dummymode>0) printf("MOGL-INFO: Switched to dummy mode level %i.\n", dummymode);
        goto moglreturn;
    }

    // Special command to set MOGL debug level:
    // debuglevel = 0 --> Shut up in any case, leave error-handling to higher-level code.
    // debuglevel > 0 --> Check for OpenGL error conditions.
    // debuglevel > 0 --> Output glError()'s in clear-text and abort. Output GLSL errors and abort.
    // debuglevel > 1 --> Output GLSL diagnostic messages as well.
    // debuglevel > 2 --> Be very verbose!
    if (strcmp(cmd, "DEBUGLEVEL")==0) {
        if (nrhs<2 || mxGetScalar(prhs[1])<0) {
            mexErrMsgTxt("MOGL-ERROR: No debuglevel or invalid debuglevel (<0) given for subcommand DEBUGLEVEL!");
        }

        debuglevel = (int) mxGetScalar(prhs[1]);
        goto moglreturn;
    }
    
    // Special cleanup subcommand needed for GNU/Octave: See explanation below in firstTime init.
    if (strcmp(cmd, "JettisonModuleHelper")==0) {
      #if PSYCH_LANGUAGE == PSYCH_OCTAVE
      // Call our cleanup routine:
      mexExitFunction();
      
      // Mark ourselves (via global variable "jettisoned") as shut-down. Any
      // further invocations of the module without previously clear'ing and
      // reloading it will be prevented.
      jettisoned = true;
      
      // Unlock ourselves from Octaves runtime environment so we can get clear'ed out:
      munlock(std::string(mexFunctionName));

      #endif

      goto moglreturn;
    }

    // Abort here if dummymode >= 10: Input arg. processing run, but no real
    // command parsing and processing;
    if (dummymode >= 10) {
      printf("MOGL-INFO: Dummy call to %s() - Ignored in dummy mode %i ...\n", cmd, dummymode);
      goto moglreturn;
    }

    #ifdef BUILD_GLM
    // GLM module is included and supported in moglcore: This is necessary if
    // one wants to use MOGL independent from Psychtoolbox. GLM is only supported
    // on MacOS-X, not on Linux or Windows...
    
    // We execute glm-commands without performing GLEW first-time initialization,
    // because to execute glewinit() we need a valid OpenGL context. This context is
    // either created by Psychtoolbox or by glm. Therefore glm-commands must be able
    // to execute before glewinit() happened.
    
    // look for command in glm command map
    if( (i=binsearch(glm_map,glm_map_count,cmd))>=0 ) {
        glm_map[i].cmdfn(nlhs,plhs,nrhs-1,prhs+1);
        goto moglreturn;
    }

    #endif

    // Is this the first invocation of moglcore?
    if (firsttime) {
        // Yes. Initialize GLEW, the GL Extension Wrangler Library. This will
        // auto-detect and dynamically link/bind all core OpenGL functionality
        // as well as all possible OpenGL extensions on OS-X, Linux and Windows.
        err = glewInit();
        if (GLEW_OK != err) {
            // Failed! Something is seriously wrong - We have to abort :(
            printf("MOGL: Failed to initialize! Probably you called an OpenGL command *before* opening an onscreen window?!?\n");
            printf("GLEW reported the following error: %s\n", glewGetErrorString(err)); fflush(NULL);
            goto moglreturn;
        }
        // Success. Ready to go...
		if (debuglevel > 1) {
			printf("MOGL - OpenGL for Matlab & GNU/Octave initialized - MOGL is (c) 2006 Richard F. Murray & Mario Kleiner, licensed to you under GPL.\n");
		}
        fflush(NULL);
        
        // Perform dynamic rebinding of ARB extensions to core functions, if necessary:
        mogl_rebindARBExtensionsToCore();
        
	// Register exit-handler: When flushing the mex-file, we free all allocated buffer memory:
	#if PSYCH_LANGUAGE == PSYCH_MATLAB
	mexAtExit(&mexExitFunction);
	#endif

	#if PSYCH_LANGUAGE == PSYCH_OCTAVE
	// Octave (as of Version 2.1.73) does not seem to support a way to register such a
	// cleanup handler, so we use the following trick: We tell octave to lock our OCT file
	// into memory, so it can not be clear'ed out of memory by Octave with the standard clear
	// command. Then we provide a new subfunction 'JettisonModuleHelper': If this
	// subcommand is called, it will call our cleanup routine, then unlock
	// ourselves from memory, now that it is safe to flush us. We provide special scripts
	// clearall.m, clearoct.m, clearmex.m and clearMODULENAME.m that do what clear all,
	// clear mex and clear MODULENAME would do on Matlab, by simply calling the
	// MODULENAME('JettisonModuleHelper'); function, followed by a clear MODULENAME; command.
	// --> User has same functionality with nearly same syntax and should be safe on Octave
	// as well.
	
	// Lock ourselves into Octaves runtime environment so we can't get clear'ed out easily:
	mlock(std::string(mexFunctionName));

	#endif
        // Done with first time initialization:
        firsttime = 0;
    }   

    // If glBeginLevel >  1 then most probably the script was aborted after execution of glBegin() but
    // before execution of glEnd(). In that case, we reset the level to zero.
    if (glBeginLevel > 1) glBeginLevel = 0;

    // Reset OpenGL error state so we can be sure that any of our error queries really
    // relate to errors caused by us:
    if (glBeginLevel == 0 && debuglevel > 0 && (strstr(cmd, "glGetError")==NULL)) glGetError();
        
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
    
    // moglreturn: Is the exit path of mogl. All execution ends at this point, regardless
    // if sucessfull or due to invocation of the mexErrMsgTxt() error - handler in the
    // Octave port. On Matlab this just performs return; on Octave it needs to assign
    // all output arguments to their proper octave_value_list slots, release temporary
    // memory and objects and then do the Octave style return:

 moglreturn:

#if PSYCH_LANGUAGE == PSYCH_MATLAB
    return;
#endif

#if PSYCH_LANGUAGE == PSYCH_OCTAVE
    // Release our own prhs array...

    // Release of memory for scalar types is done by PsychFreeAllTempMemory(0); 
    for(int i=0; i<nrhs && i<MAX_INPUT_ARGS; i++) if(prhs[i]) {
      delete(((octave_value*)(prhs[i]->o)));
      prhs[i]=NULL;	  
    }

    // "Copy" our octave-value's into the output array: If nlhs should be
    // zero (Octave-Script does not expect any return arguments), but our
    // subfunction has assigned a return argument in slot 0 anyway, then
    // we return that argument and release our own temp-memory. This
    // provides Matlab-semantic, where unsolicited return arguments are
    // printed anyway as content of the "ans" variable.
    for(i=0; (i==0 && plhs[0]!=NULL) || (i<nlhs && i<MAX_OUTPUT_ARGS); i++) {
      if (plhs[i]) {
	outplhs(i) = *((octave_value*)(plhs[i]->o));
	if (outplhs(i).is_scalar_type() && !(strstr(outplhs(i).type_name().c_str(), "uint32"))) {
	  // Special case: Scalar. Need to override with our double-ptrs value:
	  double* svalue = (double*) plhs[i]->d;
	  outplhs(i) = octave_value((double) *svalue);
	}

	if (outplhs(i).is_scalar_type() && (strstr(outplhs(i).type_name().c_str(), "uint32"))) {
	  // Special case: uint32 Scalar. Need to override with our uint32-ptrs value:
	  unsigned int* svalue = (unsigned int*) plhs[i]->d;
	  outplhs(i) = octave_value(uint32NDArray(*svalue));
	}

	// Delete our own octave_value object. All relevant data has been
	// copied via "copy-on-write" into outplhs(i) already:
	delete(((octave_value*)(plhs[i]->o)));
	
	// We don't need to free() the PsychMallocTemp()'ed object pointed to
	// by the d-Ptr, nor do we need to free the mxArray-Struct. This is done
	// below in PsychFreeAllTempMemory(0). Just NULL-out the array slot:
	plhs[i]=NULL;
      }
    }

    // Release all memory allocated via PsychMallocTemp(0):
    PsychFreeAllTempMemory(0);
    
    // Return our octave_value_list of returned values in any case and yield control
    // back to Octave:
    return(outplhs);
#endif
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
    glBeginLevel = 0;
    printf("MOGL-Command: %s\n", cmd);
    mexErrMsgTxt("invalid moglcore command");
}

// Error handler for unsupported core OpenGL functions or extensions.
// This handler gets called by the subroutines in gl_auto.c and gl_manual.c
// whenever a gl function is not bound == not supported by current OS/driver/gfx-hardware.
// As we use the GLEW library to dynamically detect and bind all OpenGL functions, we can
// easily check if a function is supported, e.g.,
// if (glCreateShader == NULL) mogl_glunsupported("glCreateShader");
//
void mogl_glunsupported(const char* fname)
{
    char errtxt[1000];
    sprintf(errtxt, "MOGL-Error: Your Matlab code tried to call the OpenGL function %s(), which is not supported\n"
                    "MOGL-Error: by your combination of graphics hardware + graphics device driver.\n"
                    "MOGL-Error: You'll have to download+install the latest gfx-drivers for your gfx-hardware\n"
                    "MOGL-Error: or upgrade your gfx-hardware with more recent one to use this function. Aborted.\n\n", fname);

    glBeginLevel = 0;
    
    // Exit to Matlab prompt with error message:
    mexErrMsgTxt(errtxt);
}

void mogl_checkerrors(const char* cmd, mxArray *prhs[]) 
{
    char errtxt[10000];
    GLint err, status, handle;
    
    // Reject no-op calls:
    if (debuglevel<=0) return;
    
    // Accounting for glBegin and glEnd calls...
    if (strcmp(cmd, "glBegin")==0) glBeginLevel++;
    if (strcmp(cmd, "glEnd")==0 && glBeginLevel > 0) glBeginLevel--;
    
    // Skip error-checking if we are in the middle of a glBegin(), glEnd() pair.
    // Calling glGetError() in between these two commands itself is an error!
    if (glBeginLevel > 0) return;
    
    // Check for glErrors():
    if ((err=glGetError())>0) {
        // Last command caused an OpenGL error condition: Report it and abort.
        sprintf(errtxt, "MOGL-Error: Your OpenGL command %s() caused the following OpenGL error: %s. Aborted.\n", cmd, gluErrorString(err));
        // Exit to Matlab prompt with error message:
        glBeginLevel = 0;
        mexErrMsgTxt(errtxt);
    }
    
    // No OpenGL core system errors. Check if a special GLSL command was executed:
    if (strcmp(cmd, "glCompileShader")==0) {
        // A GLSL shader got just compiled. Check its compile status...
        handle = (int) mxGetScalar(prhs[1]);
        glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
        if (status!=GL_TRUE) printf("MOGL-ERROR: Compilation of the GLSL shader object %i via glCompileShader(%i) failed!\n", handle, handle);
        if (debuglevel>1 || status!=GL_TRUE) {
            // Output shader info-log:
            glGetShaderInfoLog(handle, 9999, NULL, (GLchar*) &errtxt);
            printf("The shader info log for shader %i tells us the following:\n", handle);
            printf("%s \n\n", errtxt);
        }
        
        // Exit to Matlab prompt with error message if an error happened.
        if (status!=GL_TRUE) mexErrMsgTxt("Shader compilation failed!"); else return;
    }
        
    if (strcmp(cmd, "glLinkProgram")==0) {
        // A GLSL shader got just compiled. Check its compile status...
        handle = (int) mxGetScalar(prhs[1]);
        glGetProgramiv(handle, GL_LINK_STATUS, &status);
        if (status!=GL_TRUE) printf("MOGL-ERROR: Linking of the GLSL shader program %i via glLinkProgram(%i) failed!\n", handle, handle);
        if (debuglevel>1 || status!=GL_TRUE) {
            // Output shader info-log:
            glGetProgramInfoLog(handle, 9999, NULL, (GLchar*) &errtxt);
            printf("The program info log for program %i tells us the following:\n", handle);
            printf("%s \n\n", errtxt);
        }
        
        if (debuglevel>1) {
            // Output shader info-log after program validation:
            glValidateProgram(handle);
            glGetProgramInfoLog(handle, 9999, NULL, (GLchar*) &errtxt);
            printf("The program info log for program %i tells us the following after calling glValidateProgram():\n", handle);
            printf("%s \n\n", errtxt);
        }
        
        // Exit to Matlab prompt with error message if an error happened.
        if (status!=GL_TRUE) mexErrMsgTxt("GLSL link operation failed!"); else return;
    }

    return;
}

// Our memory buffer allocator, adapted from Psychtoolboxs PsychMemory.c
// allocator:

#define PTBTEMPMEMDEC(n,m) totalTempMemAllocated[(m)] -=(n)

// Enqueues a new record into our linked list of temp. memory buffers.
// Returns the memory pointer to be passed to rest of Psychtoolbox.
void* PsychEnqueueTempMemory(void* p, unsigned long n, int mlist)
{
  // Add current buffer-head ptr as next-pointer to our new buffer:
  *((unsigned int*) p) = (unsigned int) PsychTempMemHead[mlist];

  // Set our buffer as new head of list:
  PsychTempMemHead[mlist] = (unsigned int*) p;

  // Add allocated buffer size as 2nd element:
  p = (unsigned char*) p + sizeof(PsychTempMemHead[mlist]);
  *((unsigned long*) p) = n;

  // Accounting:
  totalTempMemAllocated[mlist] += n;

  // Increment p again to get real start of user-visible buffer:
  p = (unsigned char*) p + sizeof(n);

  if (debuglevel > 1) mexPrintf("MOGL: Memlist %i : Allocated new buffer %p of %i Bytes,  new total = %i.\n", mlist, p, n, totalTempMemAllocated[mlist]); fflush(NULL);

  // Return ptr:
  return(p);
}

void *PsychCallocTemp(unsigned long n, unsigned long size, int mlist)
{
  void *ret;
  // MK: This could create an overflow if product n * size is
  // bigger than length of a unsigned long int --> Only
  // happens if more than 4 GB of RAM are allocated at once.
  // --> Improbable for PTB, unless someones trying a buffer
  // overflow attack -- PTB would lose there badly anyway...
  unsigned long realsize = n * size + sizeof(void*) + sizeof(realsize);

  // realsize has extra bytes allocated for our little header...  
  if(NULL==(ret=calloc((size_t) 1, (size_t) realsize))) {
    mexErrMsgTxt("MOGL-FATAL ERROR: Out of memory in PsychCallocTemp!\n");
  }

  // Need to enqueue memory buffer...
  return(PsychEnqueueTempMemory(ret, realsize, mlist));
}

void *PsychMallocTemp(unsigned long n, int mlist)
{
  void *ret;

  // Allocate some extra bytes for our little header...
  n=n + sizeof(void*) + sizeof(n);
  if(NULL==(ret=malloc((size_t) n))){
    mexErrMsgTxt("MOGL-FATAL ERROR: Out of memory in PsychMallocTemp!\n");
  }

  // Need to enqueue memory buffer...
  return(PsychEnqueueTempMemory(ret, n, mlist));
}

// Free a single spec'd temp memory buffer.
// TODO Note: The current implementation of our allocator
// uses a single-linked list, which has O(1) cost for
// allocating memory (Optimal!) and O(n) cost for freeing
// all allocated memory (Optimal!), but it has up to
// O(n) cost for deleting a single memory buffer as well,
// be n the total number of allocated buffers. This is
// worst-case upper bound. If PsychFreeTemp() is used a
// lot on long buffer lists, this will incur significant
// overhead! A better implementation would use a double-
// linked list or even a binary tree or hash structure,
// but for now this has to be good enough(TM).
void PsychFreeTemp(void* ptr, int mlist)
{
  void* ptrbackup = ptr;
  unsigned long* psize = NULL;
  unsigned int* next = PsychTempMemHead[mlist];
  unsigned int* prevptr = NULL;

  if (ptr == NULL) return;
 
  // Convert ptb supplied pointer ptr into real start
  // of our buffer, including our header:
  ptr = (unsigned char*) ptr - sizeof((unsigned char*) ptr) - sizeof(unsigned long);
  if (ptr == NULL) return;

  if (PsychTempMemHead[mlist] == ptr) {
    // Special case: ptr is first buffer in queue. Dequeue:
    PsychTempMemHead[mlist] = (unsigned int*) *(PsychTempMemHead[mlist]);

    // Some accounting:
    PTBTEMPMEMDEC(((unsigned int*)ptr)[1], mlist);
    if (debuglevel > 1) mexPrintf("MOGL: Memlist %i : Freed buffer at %p, new total = %i.\n", mlist, ptrbackup, totalTempMemAllocated[mlist]); fflush(NULL);

    // Release it:
    free(ptr);

    return;
  }

  // ptr valid and not first buffer in queue.
  // Walk the whole buffer list until we encounter our buffer:
  while (next != NULL && next!=ptr) {
    prevptr = next;
    next = (unsigned int*) *next;
  }

  // Done with search loop. Did we find our buffer?
  if (next == ptr) {
    // Found it! Set next-ptr of previous buffer to next-ptr
    // of this buffer to dequeue from list:
    *prevptr = *next;

    // Some accounting:
    PTBTEMPMEMDEC(next[1], mlist);
    if (debuglevel > 1) mexPrintf("MOGL: Memlist %i: Freed buffer at %p, new total = %i.\n", mlist, ptrbackup, totalTempMemAllocated[mlist]); fflush(NULL);
    
    // Release:
    free(ptr);

    // Done.
    return;
  }

  // Oops.: Did not find matching buffer to pointer --> Trouble!
  printf("MOGL-BUG: In PsychFreeTemp: Tried to free non-existent temporary membuffer %p!!! Ignored.\n", ptrbackup);
  fflush(NULL);
  return;
}

// Master cleanup routine: Frees all allocated memory:
void PsychFreeAllTempMemory(int mlist)
{
  unsigned int* p = NULL;
  unsigned long* psize = NULL;
  unsigned int* next = PsychTempMemHead[mlist];

  // Walk our whole buffer list and release all buffers on it:
  while (next != NULL) {
    // next points to current buffer to release. Make a copy of
    // next:
    p = next;

    // Update next to point to the next buffer to release:
    next = (unsigned int*) *p;

    // Some accounting:
    PTBTEMPMEMDEC(p[1], mlist);

    // Release buffer p:
    free(p);
    
    // We're done with this buffer, next points to next one to release
    // or is NULL if all released...
  }

  // Done. NULL-out the list start ptr:
  PsychTempMemHead[mlist] = NULL;

  // Sanity check:
  if (totalTempMemAllocated[mlist] != 0) {
    printf("MOGL-CRITICAL BUG: Inconsistency detected in temporary memory allocator!\n");
    printf("MOGL-CRITICAL BUG: totalTempMemAllocated[%i] = %i after PsychFreeAllTempMemory(%i)!!!!\n",
	   mlist, totalTempMemAllocated[mlist], mlist);
    fflush(NULL);

    // Reset to defined state.
    totalTempMemAllocated[mlist] = 0;
  }

  if (debuglevel > 1) printf("MOGL: Memlist %i : Freed all internal memory buffers.\n", mlist); fflush(NULL);

  return;
}

// Convert a double value (which encodes a memory address) into a ptr:
void*  PsychDoubleToPtr(volatile double dptr)
{
  volatile psych_uint64* iptr = (psych_uint64*) &dptr;
  volatile psych_uint64 ival = *iptr;
  return((void*) ival);
}

// Convert a memory address pointer into a double value:
double PsychPtrToDouble(void* ptr)
{
  volatile psych_uint64 ival = (psych_uint64) ptr;
  volatile double* dptr = (double*) &ival;
  volatile double outval = *dptr;
  return(outval);
}

// Return the size of the buffer pointed to by ptr in bytes.
// CAUTION: Only works with buffers allocated via PsychMallocTemp()
// or PsychCallocTemp(). Will segfault, crash & burn with other pointers!
// The routine returns the net-size of the buffer (the size useable by
// code), not the allocated size (which would be a few additional bytes
// for the buffer-header).
unsigned int PsychGetBufferSizeForPtr(void* ptr)
{
  unsigned long mysize;

  // Decrement pointer to let it point to our size field in the buffer-header:
  ptr = (unsigned char*) ptr - sizeof(unsigned long);

  // Retrieve size value in header, subtract size of header itself:
  mysize = (*((unsigned long*) ptr)) - sizeof(void*) - sizeof(unsigned long);

  // Return it:
  return((unsigned int) mysize);
}

