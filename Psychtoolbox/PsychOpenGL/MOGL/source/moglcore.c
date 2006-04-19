/*
 *
 * moglcore.c -- MATLAB MEX file interface to OpenGL under OS X
 *
 * 08-May-2005 -- created (RFM)
 * 08-Dec-2005 -- reworked into direct interface to gl, glu, and glm functions (RFM)
 * 05-Mar-2006 -- reworked to make inclusion of glm optional (for Psychtoolbox) (MK)
 * 20-Mar-2006 -- Included support for GLEW lib for auto-detection of OpenGL extensions. (MK)
 * 15-Apr-2006 -- Dynamic rebinding of OpenGL-2 core functions to ARB extensions. (MK)
 * 16-Apr-2006 -- Built-in error detection and handling via the debuglevel parameter.
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

// Flag that signals first real invocation of moglcore:
static int firsttime = 1;

// Debuglevel: Defines if moglcore should check for errors and how to respond to them.
static int debuglevel = 1;

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

// Dynamic extension rebinding:
void mogl_rebindARBExtensionsToCore(void);

// Automatic checking and handling for glError's and GLSL errors.
void mogl_checkerrors(const char* cmd, const mxArray *prhs[]);

// MEX interface function
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    int i;
    GLenum err;
       
    // see whether there's a string command
    if( nrhs<1 || !mxIsChar(prhs[0]) )
        mogl_usageerr();
    
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
        return;
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
        return;
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
        return;
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
            return;
        }
        // Success. Ready to go...
        printf("MOGL - OpenGL for Matlab initialized - MOGL is (c) 2006 Richard F. Murray, licensed to you under GPL.\n");
        fflush(NULL);
        
        // Perform dynamic rebinding of ARB extensions to core functions, if necessary:
        mogl_rebindARBExtensionsToCore();
        
        // Done with first time initialization:
        firsttime = 0;
    }   

    // If glBeginLevel >  1 then most probably the script was aborted after execution of glBegin() but
    // before execution of glEnd(). In that case, we reset the level to zero.
    if (glBeginLevel > 1) glBeginLevel = 0;

    // Reset OpenGL error state so we can be sure that any of our error queries really
    // relate to errors caused by us:
    if (glBeginLevel == 0 && debuglevel > 0) glGetError();
        
    // look for command in manual command map
    if( (i=binsearch(gl_manual_map,gl_manual_map_count,cmd))>=0 ) {
        gl_manual_map[i].cmdfn(nlhs,plhs,nrhs-1,prhs+1);
        if (debuglevel > 0) mogl_checkerrors(cmd, prhs);
        return;
    }
    
    // look for command in auto command map
    if( (i=binsearch(gl_auto_map,gl_auto_map_count,cmd))>=0 ) {
        gl_auto_map[i].cmdfn(nlhs,plhs,nrhs-1,prhs+1);
        if (debuglevel > 0) mogl_checkerrors(cmd, prhs);
        return;
    }
    
    // no match
    mogl_usageerr();
    
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

// Dynamic rebinding of ARB extensions to core routines:
// This is a trick to get GLSL working on current OS-X (10.4.4). MacOS-X supports the OpenGL
// shading language on all graphics cards as an ARB extension. But as OS-X only supports
// OpenGL versions < 2.0 as of now, the functionality is not available as core functions, but
// only as their ARB counterparts. e.g., glCreateProgram() is always a NULL-Ptr on OS-X, but
// glCreateProgramObjectARB() is supported with exactly the same syntax and behaviour. By
// binding glCreateProgram as glCreateProgramObjectARB, we allow users to write Matlab code
// that uses glCreateProgram -- which is cleaner code than using glCreateProgramObjectARB,
// and it saves us from parsing tons of additional redundant function definitions anc code
// generation...
// In this function, we try to detect such OS dependent quirks and try to work around them...
void mogl_rebindARBExtensionsToCore(void)
{   
    // Remap unsupported OpenGL 2.0 core functions for GLSL to supported ARB extension counterparts:
    if (NULL == glCreateProgram) glCreateProgram = glCreateProgramObjectARB;
    if (NULL == glCreateShader) glCreateShader = glCreateShaderObjectARB;
    if (NULL == glShaderSource) glShaderSource = glShaderSourceARB;
    if (NULL == glCompileShader) glCompileShader = glCompileShaderARB;
    if (NULL == glAttachShader) glAttachShader = glAttachObjectARB;
    if (NULL == glLinkProgram) glLinkProgram = glLinkProgramARB;
    if (NULL == glUseProgram) glUseProgram = glUseProgramObjectARB;
    if (NULL == glGetAttribLocation) glGetAttribLocation = glGetAttribLocationARB;
    if (NULL == glGetUniformLocation) glGetUniformLocation = glGetUniformLocationARB;
    if (NULL == glUniform1f) glUniform1f = glUniform1fARB;
    if (NULL == glUniform2f) glUniform2f = glUniform2fARB;
    if (NULL == glUniform3f) glUniform3f = glUniform3fARB;
    if (NULL == glUniform4f) glUniform4f = glUniform4fARB;
    if (NULL == glUniform1fv) glUniform1fv = glUniform1fvARB;
    if (NULL == glUniform2fv) glUniform2fv = glUniform2fvARB;
    if (NULL == glUniform3fv) glUniform3fv = glUniform3fvARB;
    if (NULL == glUniform4fv) glUniform4fv = glUniform4fvARB;
    if (NULL == glUniform1i) glUniform1i = glUniform1iARB;
    if (NULL == glUniform2i) glUniform2i = glUniform2iARB;
    if (NULL == glUniform3i) glUniform3i = glUniform3iARB;
    if (NULL == glUniform4i) glUniform4i = glUniform4iARB;
    if (NULL == glUniform1iv) glUniform1iv = glUniform1ivARB;
    if (NULL == glUniform2iv) glUniform2iv = glUniform2ivARB;
    if (NULL == glUniform3iv) glUniform3iv = glUniform3ivARB;
    if (NULL == glUniform4iv) glUniform4iv = glUniform4ivARB;
    if (NULL == glUniformMatrix2fv) glUniformMatrix2fv = glUniformMatrix2fvARB;
    if (NULL == glUniformMatrix3fv) glUniformMatrix3fv = glUniformMatrix3fvARB;
    if (NULL == glUniformMatrix4fv) glUniformMatrix4fv = glUniformMatrix4fvARB;
    if (NULL == glGetShaderiv) glGetShaderiv = glGetObjectParameterivARB;
    if (NULL == glGetProgramiv) glGetProgramiv = glGetObjectParameterivARB;
    if (NULL == glGetShaderInfoLog) glGetShaderInfoLog = glGetInfoLogARB;
    if (NULL == glGetProgramInfoLog) glGetProgramInfoLog = glGetInfoLogARB;
    if (NULL == glValidateProgram) glValidateProgram = glValidateProgramARB;
    return;
}

void mogl_checkerrors(const char* cmd, const mxArray *prhs[]) 
{
    char errtxt[10000];
    int err, status, handle;
    
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
            glGetShaderInfoLog(handle, 9999, NULL, &errtxt);
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
            glGetProgramInfoLog(handle, 9999, NULL, &errtxt);
            printf("The program info log for program %i tells us the following:\n", handle);
            printf("%s \n\n", errtxt);
        }
        
        if (debuglevel>1) {
            // Output shader info-log after program validation:
            glValidateProgram(handle);
            glGetProgramInfoLog(handle, 9999, NULL, &errtxt);
            printf("The program info log for program %i tells us the following after calling glValidateProgram():\n", handle);
            printf("%s \n\n", errtxt);
        }
        
        // Exit to Matlab prompt with error message if an error happened.
        if (status!=GL_TRUE) mexErrMsgTxt("GLSL link operation failed!"); else return;
    }

    return;
}
