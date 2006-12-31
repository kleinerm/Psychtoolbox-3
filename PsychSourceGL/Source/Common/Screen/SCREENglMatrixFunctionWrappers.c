/*
	Psychtoolbox3/Source/Common/SCREENglMatrixFunctionWrappers.c		
  
	AUTHORS:

                Mario Kleiner   mk      mario.kleiner at tuebingen.mpg.de
 
	PLATFORMS:	
	
		Plattform independent.
    

	HISTORY:
  
		11/1/05         mk              Created.

        DESCRIPTION:
 
                This file provides wrapper functions that provide direct low-level access
                to the OpenGL matrix functions: Manipulation of projection-, modelview- and
                texture matrices, matrix stacks and standard transforms like translation,
                rotation and scaling.
 
                The corresponding Screen - Subfunctions have a syntax that matches the syntax
                of the corresponding glXXX functions as closely as possible, so users can
                read about proper usage of this functions in any OpenGL textbook.
 
                The file also contains the interface code for 'BeginOpenGL' and 'EndOpenGL'.
                This probably needs to be moved somewhere else in the future.
 
	TO DO:
  

*/


#include "Screen.h"

PsychError SCREENBeginOpenGL(void)
{
    static char useString[] = "Screen('BeginOpenGL', windowPtr [, sharecontext]);";
    static char synopsisString[] = "Prepare window 'windowPtr' for OpenGL rendering by external OpenGL code. "
                                   "This allows to use OpenGL drawing routines other than the ones implemented "
        "in Screen() to draw to a Psychtoolbox onscreen- or offscreen window via execution of "
        "OpenGL commands. Typical clients of this function are mogl (Richard F. Murrays OpenGL for Matlab wrapper), "
        "the new Eyelink-Toolbox and third party Matlab Mex-Files which contain OpenGL rendering routines. "
        "You have to call this command once before using any of those external drawing commands. After drawing, you "
        "switch back to PTB's rendering via the Screen('EndOpenGL', windowPtr); command. "
		"Normally you won't provide the optional flag 'sharecontext', so PTB will automatically isolate the OpenGL "
		"state of your code from its internal state. However, if you provide sharecontext=1, then PTB will allow "
		"your code to use and affect PTBs internal context. Only do this if you really know what you're doing! "
		"If you provide sharecontext=2 then PTB will give you your own private context, but it will synchronize "
		"the state of that context with its internal state - Seldomly needed, but here for your convenience. "
		"The context state isolation is as strict as possible without seriously affecting performance and functionality: "
		"All OpenGL context state is separated, with two exceptions: The framebuffer binding (if any) is always synchronized "
		"with PTB (and reset to zero when calling 'EndOpenGL' or another Screen command) to allow external code to transparently "
		"render into PTBs internal framebuffers - Needed for the imaging pipeline to work. Ressources like textures, display lists, "
		"FBOs, VBOs, PBOs and GLSL shaders are shared between PTB and your code as well for efficiency reasons. Both types of "
		"ressource sharing shouldn't be a problem, because either you are a beginner or advanced OpenGL programmer and won't use "
		"those facilities anyway, or you are an expert user - in which case you'll know how to prevent any conflicts easily.";

    static char seeAlsoString[] = "EndOpenGL SetOpenGLTexture GetOpenGLTexture moglcore";	

    PsychWindowRecordType	*windowRecord;
    int sharecontext;
	GLint fboid;
	
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check for superfluous arguments
    PsychErrorExit(PsychCapNumInputArgs(2));        // The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1));    // Number of required inputs.
    PsychErrorExit(PsychCapNumOutputArgs(0));       // The maximum number of outputs
    
    //get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
    
    // Switch to windows OpenGL context:
    PsychSetGLContext(windowRecord); 
    
    // Set it as drawing target: This will set up the proper FBO bindings as well:
    PsychSetDrawingTarget(windowRecord);
	
	// Query current FBO binding. We need to manually transfer this to the userspace context, so
	// it can render into our window:
    if (glBindFramebufferEXT) glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &fboid);
	
	// (Optional) context sharing flag provided?
	sharecontext = 0;
	PsychCopyInIntegerArg(2, FALSE, &sharecontext);
	if (sharecontext<0 || sharecontext>2) PsychErrorExitMsg(PsychError_user, "Invalid value for 'sharecontext' provided. Not in range 0 to 2.");

	// Userspace wants its own private rendering context, optionally updated to match PTBs internal state?
	if (sharecontext == 0 || sharecontext == 2) {
		// Yes. This is the normal case for 3D rendering. MOGLs and PTBs contexts are separated to
		// increase robustness, only ressources like textures, display lists, PBO's, VBO's, FBO's
		// and GLSL shaders are shared, but not the current renderstate.
		
		// Make sure 3D rendering is globally enabled, otherwise this is considered a userspace bug:
		if (PsychPrefStateGet_3DGfx()==0) PsychErrorExitMsg(PsychError_user, "Tried to call 'BeginOpenGL' for external rendering, but rendering not enabled! Call 'InitializeMatlabOpenGL' at the beginning of your script!!");

		// Switch to userspace context for this window, optionally sync state with PTBs context:
		PsychOSSetUserGLContext(windowRecord, (sharecontext==2) ? TRUE : FALSE);
		
		// Manually establish proper FBO binding. This will get reset automaticaly on back-transition
		// inside PsychSetGLContext on its first invocation:
		if (glBindFramebufferEXT) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboid);
	}
    
	// Check for GL errors:
    PsychTestForGLErrors();
    
	// Ready for userspace rendering:
    return(PsychError_none);
}

PsychError SCREENEndOpenGL(void)
{
    static char useString[] = "Screen('EndOpenGL', windowPtr);";
    static char synopsisString[] = "Prepare window 'windowPtr' for OpenGL rendering by external OpenGL code. "
        "This allows to use OpenGL drawing routines other than the ones implemented "
        "in Screen() to draw to a Psychtoolbox onscreen- or offscreen window via execution of "
        "OpenGL commands. Typical clients of this function are mogl (Richard F. Murrays OpenGL for Matlab wrapper), "
        "the new Eyelink-Toolbox and third party Matlab Mex-Files which contain OpenGL rendering routines. "
        "You have to call this command once before using any of those external drawing commands. After drawing, you "
        "switch back to PTB's rendering via the Screen('EndOpenGL', windowPtr); command. ";
    static char seeAlsoString[] = "EndOpenGL SetOpenGLTexture GetOpenGLTexture moglcore";	
    
    PsychWindowRecordType	*windowRecord;
    GLenum error;
    
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check for superfluous arguments
    PsychErrorExit(PsychCapNumInputArgs(1));        // The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1));    // Number of required inputs.
    PsychErrorExit(PsychCapNumOutputArgs(0));       // The maximum number of outputs
    
    //get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
    
    // Check for OpenGL errors in external code:
    if ((error=glGetError())!=GL_NO_ERROR) {
        printf("PTB-ERROR: Some of your external OpenGL code executed between last invocation of Screen('BeginOpenGL') and\n");
        printf("PTB-ERROR: Screen('EndOpenGL') produced an OpenGL error condition. Please check your code. The reported GL\n");
        printf("PTB-ERROR: error was: %s\n\n", (const char*) gluErrorString(error)); 
        PsychErrorExitMsg(PsychError_user, "Failure in external OpenGL code.");
    }

	// Switch to windows OpenGL context:
    PsychSetGLContext(windowRecord); 
    
    // Set it as drawing target:
    PsychSetDrawingTarget(windowRecord);
    
	// Check again...
    PsychTestForGLErrors();
    
	// Ready for internal rendering:
    return(PsychError_none);
}

PsychError SCREENglPushMatrix(void)  
{
        // If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
        static char useString[] = "Screen('glPushMatrix', windowPtr);";
        static char synopsisString[] = "Store a backup copy of active current OpenGL matrix on the matrix stack for later reuse. "
            "The capacity of the matrix backup stack is limited, typically not more than 27 slots. For each call to glPushMatrix "
            "you need to call glPopMatrix at the appropriate place to avoid overflowing the stack. "
            "See <http://www.opengl.org/documentation/red_book_1.0/> Chapter 4 for detailed information.";
        static char seeAlsoString[] = "";	
    
	PsychWindowRecordType	*windowRecord;
        GLint stack_cur, stack_max;
    
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString,seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(1));        // The maximum number of inputs
        PsychErrorExit(PsychRequireNumInputArgs(1)); 	// Number of required inputs.
	PsychErrorExit(PsychCapNumOutputArgs(0));       // The maximum number of outputs

	//get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
        
        // Switch to windows OpenGL context:
	PsychSetGLContext(windowRecord); 
        glMatrixMode(GL_MODELVIEW);
        
        // Compare current fill level of matrix stack with maximum level: We reserve five
        // stack-slots for PTB internal use, so at least that needs to be free before push.

        glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &stack_max);
        glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &stack_cur);
        if (stack_max - stack_cur < 6) {
            printf("\nCouldn't push OpenGL-Modelview matrix because matrix stack is full! The most common reason is\n");
            printf("forgetting to call glPopMatrix a matching number of times... \n");
            printf("The maximum number of pushable matrices is %i -- Please check your code.\n", stack_max - 5);
            PsychErrorExitMsg(PsychError_user, "Too many calls to glPushMatrix. Imbalance?");
        }
        
        // Execute push op:
        glPushMatrix();        
        
        PsychTestForGLErrors();
        
 	//All psychfunctions require this.
	return(PsychError_none);
}

PsychError SCREENglPopMatrix(void)  
{
    // If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
    static char useString[] = "Screen('glPopMatrix', windowPtr);";
    static char synopsisString[] = "Restore an OpenGL matrix by fetching it from the matrix stack. "
        "See <http://www.opengl.org/documentation/red_book_1.0/> Chapter 4 for detailed information.";
    static char seeAlsoString[] = "";	
    
    PsychWindowRecordType	*windowRecord;
    GLint stack_cur;
    
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check for superfluous arguments
    PsychErrorExit(PsychCapNumInputArgs(1));        // The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1)); 	// Number of required inputs.
    PsychErrorExit(PsychCapNumOutputArgs(0));       // The maximum number of outputs
    
    //get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
    
    // Switch to windows OpenGL context:
    PsychSetGLContext(windowRecord); 
    glMatrixMode(GL_MODELVIEW);
    
    // Compare current fill level of matrix stack with maximum level: We reserve five
    // stack-slots for PTB internal use, so at least that needs to be free before push.

    glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &stack_cur);
    if (stack_cur < 2) {
        printf("\nCouldn't pop matrix from top of OpenGL-Modelview matrix stack, because matrix stack is empty! The most common reason is\n");
        printf("that you tried to call glPopMatrix more often than you called glPushMatrix -- Please check your code.\n");
        PsychErrorExitMsg(PsychError_user, "Too many calls to glPopMatrix. Imbalance?!?");
    }

    // Execute pop operation:
    glPopMatrix();
    
    PsychTestForGLErrors();
    
    //All psychfunctions require this.
    return(PsychError_none);
}

PsychError SCREENglLoadIdentity(void)  
{
    // If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
    static char useString[] = "Screen('glLoadIdentity', windowPtr);";
    static char synopsisString[] = "Reset an OpenGL matrix to its default identity setting. "
        "See <http://www.opengl.org/documentation/red_book_1.0/> Chapter 4 for detailed information.";
    static char seeAlsoString[] = "";	
    
    PsychWindowRecordType	*windowRecord;
    
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check for superfluous arguments
    PsychErrorExit(PsychCapNumInputArgs(1));        // The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1)); 	// Number of required inputs.
    PsychErrorExit(PsychCapNumOutputArgs(0));       // The maximum number of outputs
    
    //get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
    
    // Switch to windows OpenGL context:
    PsychSetGLContext(windowRecord); 
    // Execute it:
    glLoadIdentity();
    
    PsychTestForGLErrors();
    
    //All psychfunctions require this.
    return(PsychError_none);
}

PsychError SCREENglTranslate(void)  
{
    // If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
    static char useString[] = "Screen('glTranslate', windowPtr, tx, ty [, tz]);";
    //                                               1          2   3     4
    static char synopsisString[] = "Define a translation by (tx, ty, tz) in space, relative to the enclosing reference frame."
    "See <http://www.opengl.org/documentation/red_book_1.0/> Chapter 4 for detailed information.";
    static char seeAlsoString[] = "";	
    
    PsychWindowRecordType	*windowRecord;
    double                      tx, ty, tz;

    // Default to zero-translation:
    tx=ty=tz=0.0f;
    
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check for superfluous arguments
    PsychErrorExit(PsychCapNumInputArgs(4));        // The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(3));    // Number of required inputs.
    PsychErrorExit(PsychCapNumOutputArgs(0));       // The maximum number of outputs
    
    //get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
    
    // Fetch translation vector:
    PsychCopyInDoubleArg(2, FALSE, &tx);
    PsychCopyInDoubleArg(3, FALSE, &ty);
    PsychCopyInDoubleArg(4, FALSE, &tz);
    
    // Switch to windows OpenGL context:
    PsychSetGLContext(windowRecord); 
    
    // Execute it:
    glTranslated(tx, ty, tz);
    PsychTestForGLErrors();
    
    //All psychfunctions require this.
    return(PsychError_none);
}

PsychError SCREENglScale(void)  
{
    // If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
    static char useString[] = "Screen('glScale', windowPtr, sx, sy [, sz]);";
    //                                           1          2   3     4
    static char synopsisString[] = "Define a scale transform by (sx, sy, sz) in space, relative to the enclosing reference frame."
        "See <http://www.opengl.org/documentation/red_book_1.0/> Chapter 4 for detailed information.";
    static char seeAlsoString[] = "";	
    
    PsychWindowRecordType	*windowRecord;
    double                      sx, sy, sz;
    
    // Default to non-scale, aka scale by 1.0:
    sx=sy=sz=1.0f;
    
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check for superfluous arguments
    PsychErrorExit(PsychCapNumInputArgs(4));        // The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(3));    // Number of required inputs.
    PsychErrorExit(PsychCapNumOutputArgs(0));       // The maximum number of outputs
    
    //get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
    
    // Fetch translation vector:
    PsychCopyInDoubleArg(2, FALSE, &sx);
    PsychCopyInDoubleArg(3, FALSE, &sy);
    PsychCopyInDoubleArg(4, FALSE, &sz);
    
    // Switch to windows OpenGL context:
    PsychSetGLContext(windowRecord); 
    
    // Execute it:
    glScaled(sx, sy, sz);
    PsychTestForGLErrors();
    
    //All psychfunctions require this.
    return(PsychError_none);
}

PsychError SCREENglRotate(void)  
{
    // If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
    static char useString[] = "Screen('glRotate', windowPtr, angle, [rx = 0], [ry = 0] ,[rz = 1]);";
    //                                            1          2       3         4         5
    static char synopsisString[] = "Define a rotation transform by an angle of 'angle' degrees around the "
        "axis defined by the vector (rx,ry,rz) in space, relative to the enclosing reference frame."
        "See <http://www.opengl.org/documentation/red_book_1.0/> Chapter 4 for detailed information.";
    static char seeAlsoString[] = "";	
    
    PsychWindowRecordType	*windowRecord;
    double                      rx, ry, rz;
    double                      angle;
    
    // Default to rotation around z-axis, aka in-plane rotation:
    rx=ry=0;
    rz=1;
    // Default to a non-rotation (zero degrees):
    angle=0;
    
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check for superfluous arguments
    PsychErrorExit(PsychCapNumInputArgs(5));        // The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(2));    // Number of required inputs.
    PsychErrorExit(PsychCapNumOutputArgs(0));       // The maximum number of outputs
    
    //get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);

    // Fetch rotation angle:
    PsychCopyInDoubleArg(2, TRUE, &angle);
    
    // Fetch rotation vector:
    PsychCopyInDoubleArg(3, FALSE, &rx);
    PsychCopyInDoubleArg(4, FALSE, &ry);
    PsychCopyInDoubleArg(5, FALSE, &rz);
    
    // Switch to windows OpenGL context:
    PsychSetGLContext(windowRecord); 
    
    // Execute it:
    glRotated(angle, rx, ry, rz);
    PsychTestForGLErrors();
    
    //All psychfunctions require this.
    return(PsychError_none);
}

/* To be continued... Stay tuned! ;-) */

