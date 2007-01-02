/*
	PsychToolbox3/Source/OSX/Screen/PsychWindowGlue.c
	
	PLATFORMS:	
	
		This is the OS X Core Graphics version only.  
				
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu
                Mario Kleiner           mk              mario.kleiner at tuebingen.mpg.de

	HISTORY:
	
		12/20/02		awi		Wrote it mostly by modifying SDL-specific refugees (from an experimental SDL-based Psychtoolbox).
		11/16/04		awi		Added description.
                 4/22/05                mk              Added support for OpenGL stereo windows and enhanced Flip-behaviour:
                                                        Flip'ing at specified deadline, retaining instead of clear'ing backbuffer during flip,
                                                        return of stimulus onset related timestamps, properly syncing to VBL.
                 4/29/05                mk              Additional safety checks for VBL sync in PsychOpenOnscreenWindow().
                 5/14/05                mk              Additional safety checks for insufficient gfx-hardware and multi-display setups,
                                                        failing beam-position queries. New refresh interval estimation code, reworked Flip.
                 5/19/05                mk              Extra check for 'flipwhen' values over 1000 secs in future: Abort.
                 5/30/05                mk              Check for Screen('Preference', 'SkipSyncTests', 1) -> Shortened tests, if set.
                 6/09/05                mk              Experimental support for busy-waiting for VBL and for multi-flip of stereo displays.
                 9/30/05                mk              Added PsychRealtimePriority for improving timing tests in PsychOpenWindow()
                 9/30/05                mk              Added check for Screen('Preference', 'VisualDebugLevel', level) -> Amount of vis. feedback.
                 10/10/05               mk              Important Bugfix for PsychRealtimePriority() - didn't switch back to non-RT priority!!
		 10/19/05		awi		Cast NULL to CGLPixelFormatAttribute type to make the compiler happy.
                 01/02/05               mk              Modified to only contain the OS-X specific code. All OS independent code has been moved to
                                                        Common/Screen/PsychWindowSupport.c
 
	DESCRIPTION:
	
		Functions in this file comprise an abstraction layer for probing and controlling window state, except for window content.  
		
		Each C function which implements a particular Screen subcommand should be platform neutral.  For example, the source to SCREENPixelSizes() 
		should be platform-neutral, despite that the calls in OS X and Windows to detect available pixel sizes are different.  The platform 
		specificity is abstracted out in C files which end it "Glue", for example PsychScreenGlue, PsychWindowGlue, PsychWindowTextClue.

	NOTES:
	
	TO DO: 
	
		¥ The "glue" files should should be suffixed with a platform name.  The original (bad) plan was to distingish platform-specific files with the same 
		name by their placement in a directory tree.
 
*/

#include "Screen.h"

// Includes for low-level access to IOKit Framebuffer device:
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <IOKit/graphics/IOFramebufferShared.h>

static struct {
    io_connect_t        connect;
    StdFBShmem_t *      shmem;
    vm_size_t           shmemSize;    
} fbsharedmem[kPsychMaxPossibleDisplays];   


/** PsychRealtimePriority: Temporarily boost priority to THREAD_TIME_CONSTRAINT_POLICY.
    PsychRealtimePriority(true) enables realtime-scheduling (like Priority(9) would do in Matlab).
    PsychRealtimePriority(false) restores scheduling to the state before last invocation of PsychRealtimePriority(true),
    it undos whatever the previous switch did.

    We switch to RT scheduling during PsychGetMonitorRefreshInterval() and a few other timing tests in
    PsychOpenWindow() to reduce measurement jitter caused by possible interference of other tasks.
*/
boolean PsychRealtimePriority(boolean enable_realtime)
{
    bool				isError;
    thread_policy_flavor_t		flavorConstant;
    int					kernError;
    task_t				threadID;
    thread_policy_t			threadPolicy;
    static thread_policy_t		old_threadPolicy;
    mach_msg_type_number_t		policyCount, policyCountFilled;
    static mach_msg_type_number_t	old_policyCountFilled;
    boolean_t				isDefault;
    
    static boolean old_enable_realtime = FALSE;
    static boolean oldModeWasStandard = FALSE;
    
    if (old_enable_realtime == enable_realtime) {
        // No transition with respect to previous state -> Nothing to do.
        return(true);
    }
    
    // Transition requested:
    old_enable_realtime = enable_realtime;
    
    kernError=0;
    // Determine our threadID:
    threadID= mach_thread_self();
    
    if (enable_realtime) {
        // Transition to realtime requested:
        
        // Get current scheduling policy and its settings and back it up for later restore:
        old_threadPolicy=(thread_policy_t) malloc(sizeof(thread_time_constraint_policy_data_t));
        policyCount=THREAD_TIME_CONSTRAINT_POLICY_COUNT;
        old_policyCountFilled=policyCount;
        isDefault=FALSE;
        // We check if STANDARD_POLICY is active and query its settings, if so...
        kernError=thread_policy_get(threadID, THREAD_STANDARD_POLICY, old_threadPolicy, &old_policyCountFilled, &isDefault);
        if (kernError) {
            // Failed!
            old_enable_realtime=FALSE;
            free(old_threadPolicy);
            return(FALSE);
        }
        
        // oldModeWasStandard == TRUE --> We need to revert to STANDARD POLICY later...
        oldModeWasStandard=!isDefault;
        
        // Prepare realtime-policy: Query default settings for it...
        threadPolicy=(thread_policy_t) malloc(sizeof(thread_time_constraint_policy_data_t));
        policyCount=THREAD_TIME_CONSTRAINT_POLICY_COUNT;
        policyCountFilled=policyCount;
        isDefault=TRUE;
        kernError=thread_policy_get(threadID, THREAD_TIME_CONSTRAINT_POLICY, threadPolicy, &policyCountFilled, &isDefault);
        if (kernError) {
            // Failed!
            old_enable_realtime=FALSE;
            free(threadPolicy);
            return(FALSE);
        }
        
        // Check if realtime scheduling isn't already active (==oldModeWasSTanderd==FALSE)
        // If we are already in RT mode (e.g., Priority(9) call in Matlab), we skip the switch...
        if (oldModeWasStandard) {
            // RT scheduling not yet active -> Switch to it.
            kernError=thread_policy_set(threadID, THREAD_TIME_CONSTRAINT_POLICY, threadPolicy, policyCountFilled);
            if (kernError) {
                // Failed!
                old_enable_realtime=FALSE;
                free(threadPolicy);
                return(FALSE);
            }
        }
        
        // Successfully switched to RT-Scheduling:
        free((void*)threadPolicy);
    }
    else {
        // Transition from RT to Non-RT scheduling requested: We just reestablish the backed-up old
        // policy: If the old policy wasn't Non-RT, then we don't switch back...
        if (oldModeWasStandard) {
            kernError=thread_policy_set(threadID, THREAD_STANDARD_POLICY, old_threadPolicy, old_policyCountFilled);
            if (kernError) {
                // Failed!
                old_enable_realtime=TRUE;
                
                printf("PsychRealtimePriority: ERROR! COULDN'T SWITCH BACK TO NON-RT SCHEDULING!!!");
                fflush(NULL);
                return(FALSE);
            }
        }
        
        // Successfully switchted to RT-Scheduling:
        free((void*) old_threadPolicy);
    }
    
    // Success.
    return(TRUE);
}


/*
    PsychOSOpenOnscreenWindow()
    
    Creates the CGL pixel format and the CGL context objects and then instantiates the context onto the screen.
    
    -The pixel format and the context are stored in the target specific field of the window recored.  Close
    should clean up by destroying both the pixel format and the context.
    
    -We mantain the context because it must be be made the current context by drawing functions to draw into 
    the specified window.
    
    -We maintain the pixel format object because there seems to be now way to retrieve that from the context.
    
    -To tell the caller to clean up PsychOpenOnscreenWindow returns FALSE if we fail to open the window. It 
    would be better to just issue an PsychErrorExit() and have that clean up everything allocated outside of
    PsychOpenOnscreenWindow().
    
    MK: The new option 'stereomode' allows selection of stereo display instead of mono display:
    0 (default) == Old behaviour -> Monoscopic rendering context.
    >0          == Stereo display, where the number defines the type of stereo algorithm to use.
    =1          == Use OpenGL built-in stereo by creating a context/window with left- and right backbuffer.
    =2          == Use compressed frame stereo: Put both views into one framebuffer, one in top half, other in lower half.

*/
boolean PsychOSOpenOnscreenWindow(PsychScreenSettingsType *screenSettings, PsychWindowRecordType *windowRecord, int numBuffers, int stereomode, int conserveVRAM)
{
    CGLRendererInfoObj				rendererInfo;
    CGOpenGLDisplayMask 			displayMask;
    CGLError					error;
    CGDirectDisplayID				cgDisplayID;
    CGLPixelFormatAttribute			attribs[32];
    long					numVirtualScreens;
    GLboolean					isDoubleBuffer, isFloatBuffer;
    GLint bpc;
	GLenum glerr;
	
    int attribcount=0;
    int i;

    // Map screen number to physical display handle cgDisplayID:
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenSettings->screenNumber);
    displayMask=CGDisplayIDToOpenGLDisplayMask(cgDisplayID);

    attribs[attribcount++]=kCGLPFAFullScreen;
    attribs[attribcount++]=kCGLPFADisplayMask;
    attribs[attribcount++]=displayMask;

    // 10 bit per component framebuffer requested (10-10-10-2)?
    if (windowRecord->depth == 30) {
      // Request a 10 bit per color component framebuffer with 2 bit alpha channel:
      printf("PTB-INFO: Trying to enable 10 bpc framebuffer...\n");
	  attribs[attribcount++]=kCGLPFANoRecovery;
	  attribs[attribcount++]=kCGLPFAAccelerated;
      attribs[attribcount++]=kCGLPFAColorSize;
      attribs[attribcount++]=16*3;
      attribs[attribcount++]=kCGLPFAAlphaSize;
      attribs[attribcount++]=16;
    }

    // 16 bit per component, 64 bit framebuffer requested (16-16-16-16)?
    if (windowRecord->depth == 64) {
      // Request a floating point framebuffer in 16-bit half-float format, i.e., RGBA = 16 bits per component.
      printf("PTB-INFO: Trying to enable 16 bpc float framebuffer...\n");
      attribs[attribcount++]=kCGLPFAColorFloat;
      attribs[attribcount++]=kCGLPFAColorSize;
      attribs[attribcount++]=16*3;
      attribs[attribcount++]=kCGLPFAAlphaSize;
      attribs[attribcount++]=16;
    }

    // 32 bit per component, 128 bit framebuffer requested (32-32-32-32)?
    if (windowRecord->depth == 128) {
      // Request a floating point framebuffer in 32-bit float format, i.e., RGBA = 32 bits per component.
      printf("PTB-INFO: Trying to enable 32 bpc float framebuffer...\n");
      attribs[attribcount++]=kCGLPFAColorFloat;
      attribs[attribcount++]=kCGLPFAColorSize;
      attribs[attribcount++]=32*3;
      attribs[attribcount++]=kCGLPFAAlphaSize;
      attribs[attribcount++]=32;
    }

    // Support for 3D rendering requested?
    if (PsychPrefStateGet_3DGfx()) {
        // Yes. Allocate a 24-Bit depth and 8-Bit stencilbuffer for this purpose:
        attribs[attribcount++]=kCGLPFADepthSize;
        attribs[attribcount++]=24;
        attribs[attribcount++]=kCGLPFAStencilSize;
        attribs[attribcount++]=8;
    }
    if(numBuffers>=2){
        // Enable double-buffering:
        attribs[attribcount++]=kCGLPFADoubleBuffer;
        if ((conserveVRAM & kPsychDisableAUXBuffers) == 0) {
            // MK: Allocate one or two (mono vs. stereo) AUX buffers for new "don't clear" mode of Screen('Flip'):
            // Not clearing the framebuffer after "Flip" is implemented by storing a backup-copy of
            // the backbuffer to AUXs before flip and restoring the content from AUXs after flip.
            attribs[attribcount++]=kCGLPFAAuxBuffers;
            attribs[attribcount++]=(stereomode==kPsychOpenGLStereo || stereomode==kPsychCompressedTLBRStereo || stereomode==kPsychCompressedTRBLStereo) ? 2 : 1;
        }
    }

    // MK: Stereo display support: If stereo display output is requested with OpenGL native stereo,
    // we request a stereo-enabled rendering context.
    if(stereomode==kPsychOpenGLStereo) {
        attribs[attribcount++]=kCGLPFAStereo;
    }

    // Multisampled Anti-Aliasing requested?
    if (windowRecord->multiSample > 0) {
      // Request a multisample buffer:
      attribs[attribcount++]= kCGLPFASampleBuffers;
      attribs[attribcount++]= 1;
      // Request at least multiSample samples per pixel:
      attribs[attribcount++]= kCGLPFASamples;
      attribs[attribcount++]= windowRecord->multiSample;
    }

    // Finalize attribute array with NULL.
    attribs[attribcount]=(CGLPixelFormatAttribute)NULL;

    // Init to zero:
    windowRecord->targetSpecific.pixelFormatObject = NULL;
	windowRecord->targetSpecific.glusercontextObject = NULL;
	
    // First try in choosing a matching format for multisample mode:
    if (windowRecord->multiSample > 0) {
      error=CGLChoosePixelFormat(attribs, &(windowRecord->targetSpecific.pixelFormatObject), &numVirtualScreens);
      if (windowRecord->targetSpecific.pixelFormatObject==NULL && windowRecord->multiSample > 0) {
	// Failed. Probably due to too demanding multisample requirements: Lets lower them...
	for (i=0; i<attribcount && attribs[i]!=kCGLPFASamples; i++);
	while (windowRecord->targetSpecific.pixelFormatObject==NULL && windowRecord->multiSample > 0) {
	  attribs[i+1]--;
	  windowRecord->multiSample--;
          error=CGLChoosePixelFormat(attribs, &(windowRecord->targetSpecific.pixelFormatObject), &numVirtualScreens);
	}
	if (windowRecord->multiSample == 0 && windowRecord->targetSpecific.pixelFormatObject==NULL) {
	  for (i=0; i<attribcount && attribs[i]!=kCGLPFASampleBuffers; i++);
	  attribs[i+1]=0;
	}
      }
    }

    // Choose a matching display configuration and create the window and rendering context:
    // If one of these two fails, then the installed gfx hardware is not good enough to satisfy our
    // requirements, or we have massive ressource shortage in the system. -> Screwed up anyway, so we abort.
    if (windowRecord->targetSpecific.pixelFormatObject==NULL) error=CGLChoosePixelFormat(attribs, &(windowRecord->targetSpecific.pixelFormatObject), &numVirtualScreens);
    if (error) {
        printf("\nPTB-ERROR[ChoosePixelFormat failed: %s]:The specified display may not support double buffering and/or stereo output. There could be insufficient video memory\n\n", CGLErrorString(error));
        return(FALSE);
    }
    
    // Create an OpenGL rendering context with the selected pixelformat
    error=CGLCreateContext(windowRecord->targetSpecific.pixelFormatObject, NULL, &(windowRecord->targetSpecific.contextObject));
    if (error) {
        printf("\nPTB-ERROR[ContextCreation failed: %s]:The specified display may not support double buffering and/or stereo output. There could be insufficient video memory\n\n", CGLErrorString(error));
        return(FALSE);
    }
    
    // Enable the OpenGL rendering context associated with our window:
    error=CGLSetCurrentContext(windowRecord->targetSpecific.contextObject);
    if (error) {
        printf("\nPTB-ERROR[SetCurrentContext failed: %s]:The specified display may not support double buffering and/or stereo output. There could be insufficient video memory\n\n", CGLErrorString(error));
        return(FALSE);
    }
    
    // Switch to fullscreen display: We don't support windowed display on OS-X
    error=CGLSetFullScreen(windowRecord->targetSpecific.contextObject);
    if (error) {
        printf("\nPTB-ERROR[CGLSetFullScreen failed: %s]:The specified display may not support the current color depth -\nPlease switch to 'Millions of Colors' in Display Settings.\n\n", CGLErrorString(error));
        CGLSetCurrentContext(NULL);
        return(FALSE);
    }
    
	// Ok, the OpenGL rendering context is up and running. Auto-detect and bind all
	// available OpenGL extensions via GLEW:
	glerr = glewInit();
	if (GLEW_OK != glerr)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		printf("\nPTB-ERROR[GLEW init failed: %s]: Please report this to the forum. Will try to continue, but may crash soon!\n\n", glewGetErrorString(glerr));
		fflush(NULL);
	}
	else {
		if (PsychPrefStateGet_Verbosity()>3) printf("PTB-INFO: Using GLEW version %s for automatic detection of OpenGL extensions...\n", glewGetString(GLEW_VERSION));
	}
	
    // Enable multisampling if it was requested:
    if (windowRecord->multiSample > 0) glEnable(GL_MULTISAMPLE);
    
    // Double-check double buffer support:
    if(numBuffers>=2){
        //This doesn't work.  GL thinks that there are double buffers when we fail to get that because
        //their was insufficient video memory to open a back buffer.  
        isDoubleBuffer=false;
        glGetBooleanv(GL_DOUBLEBUFFER, &isDoubleBuffer);
        if(!isDoubleBuffer){
            CGLDestroyPixelFormat(windowRecord->targetSpecific.pixelFormatObject);
            CGLSetCurrentContext(NULL);
            CGLClearDrawable(windowRecord->targetSpecific.contextObject ) ;
            return(FALSE);
        }
    }
    
    // External 3D graphics support enabled?
	if (PsychPrefStateGet_3DGfx()) {
		// Yes. We need to create an extra OpenGL rendering context for the external
		// OpenGL code to provide optimal state-isolation. The context shares all
		// heavyweight ressources likes textures, FBOs, VBOs, PBOs, display lists and
		// starts off as an identical copy of PTB's context as of here.
		error=CGLCreateContext(windowRecord->targetSpecific.pixelFormatObject, windowRecord->targetSpecific.contextObject, &(windowRecord->targetSpecific.glusercontextObject));
		if (error) {
			printf("\nPTB-ERROR[UserContextCreation failed: %s]: Creating a private OpenGL context for Matlab OpenGL failed for unknown reasons.\n\n", CGLErrorString(error));
			return(FALSE);
		}
	    // Attach it to our onscreen drawable:
		error=CGLSetFullScreen(windowRecord->targetSpecific.glusercontextObject);
		if (error) {
			printf("\nPTB-ERROR[CGLSetFullScreen for user context failed: %s]: Attaching private OpenGL context for Matlab OpenGL failed for unknown reasons.\n\n", CGLErrorString(error));
			CGLSetCurrentContext(NULL);
			return(FALSE);
		}
		// Copy full state from our main context:
		error = CGLCopyContext(windowRecord->targetSpecific.contextObject, windowRecord->targetSpecific.glusercontextObject, GL_ALL_ATTRIB_BITS);
		if (error) {
			printf("\nPTB-ERROR[CGLCopyContext for user context failed: %s]: Copying state to private OpenGL context for Matlab OpenGL failed for unknown reasons.\n\n", CGLErrorString(error));
			CGLSetCurrentContext(NULL);
			return(FALSE);
		}
	}
	
    // Initialize a low-level mapping of Framebuffer device data structures into
    // our address space: Needed for additional timing checks:

    // Initialize to safe default:
    fbsharedmem[screenSettings->screenNumber].shmem = NULL;

    // A value of zero would forcefully disable this method:
    if (PsychPrefStateGet_VBLTimestampingMode()>0) {
        // Get access to Mach service port for the physical display device associated
        // with this onscreen window and open our own connection to the port:
        if ((kIOReturnSuccess == IOServiceOpen(CGDisplayIOServicePort(cgDisplayID), mach_task_self(), kIOFBSharedConnectType, &(fbsharedmem[screenSettings->screenNumber].connect))) ||
            (kIOReturnSuccess == IOServiceOpen(CGDisplayIOServicePort(CGMainDisplayID()), mach_task_self(), kIOFBSharedConnectType, &(fbsharedmem[screenSettings->screenNumber].connect)))) {
            // Connection established.

			// Map the slice of device memory into our VM space:
			if (kIOReturnSuccess != IOConnectMapMemory(fbsharedmem[screenSettings->screenNumber].connect, kIOFBCursorMemory, mach_task_self(),
													   (vm_address_t *) &(fbsharedmem[screenSettings->screenNumber].shmem),
													   &(fbsharedmem[screenSettings->screenNumber].shmemSize), kIOMapAnywhere)) {
				// Mapping failed!
				fbsharedmem[screenSettings->screenNumber].shmem = NULL;
				if (PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Failed to gain access to kernel-level vbl handler [IOConnectMapMemory()] - Fallback path for time stamping won't be available.\n");
			}
			else {
				if (PsychPrefStateGet_Verbosity()>3) printf("PTB-INFO: Connection to kernel-level vbl handler establised (shmem = %p).\n",  fbsharedmem[screenSettings->screenNumber].shmem);
			}
        }
        else {
            if (PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Failed to gain access to kernel-level vbl handler [IOServiceOpen()] - Fallback path for time stamping won't be available.\n");
        }
        
        // If the mapping worked, we have a pointer to the driver memory in .shmem, otherwise we have NULL:
    }
    
    // Done.
    return(TRUE);
}

/*
    PsychOSGetVBLTimeAndCount()

    Returns absolute system time of last VBL and current total count of VBL interrupts since
    startup of gfx-system for the given screen. Returns a time of -1 and a count of 0 if this
    feature is unavailable on the given OS/Hardware configuration.
*/
double PsychOSGetVBLTimeAndCount(unsigned int screenid, psych_uint64* vblCount)
{
    // Do we have a valid shared mapping?
    if (fbsharedmem[screenid].shmem) {
        // Retrieve absolute count of vbls since startup:
        *vblCount = (psych_uint64) fbsharedmem[screenid].shmem->vblCount;
        
        // Retrieve absolute system time of last retrace, convert into PTB standard time system and return it:
	return(((double) UnsignedWideToUInt64(AbsoluteToNanoseconds(fbsharedmem[screenid].shmem->vblTime))) / 1000000000.0);
    }
    else {
        // Unsupported :(
        *vblCount = 0;
        return(-1);
    }
}

/*
    PsychOSOpenOffscreenWindow()
    
    Accept specifications for the offscreen window in the platform-neutral structures, convert to native CoreGraphics structures,
    create the surface, allocate a window record and record the window specifications and memory location there.
	
	TO DO:  We need to walk down the screen number and fill in the correct value for the benefit of TexturizeOffscreenWindow
*/
boolean PsychOSOpenOffscreenWindow(double *rect, int depth, PsychWindowRecordType **windowRecord)
{

    //PsychTargetSpecificWindowRecordType 	cgStuff;
    CGLPixelFormatAttribute 			attribs[5];
    //CGLPixelFormatObj					pixelFormatObj;
    long								numVirtualScreens;
    CGLError							error;
    int									windowWidth, windowHeight;
    int									depthBytes;

    //First allocate the window recored to store stuff into.  If we exit with an error PsychErrorExit() should
    //call PsychPurgeInvalidWindows which will clean up the window record. 
    PsychCreateWindowRecord(windowRecord);  		//this also fills the window index field.
    
    attribs[0]=kCGLPFAOffScreen;
    attribs[1]=kCGLPFAColorSize;
    attribs[2]=(CGLPixelFormatAttribute)depth;
    attribs[3]=(CGLPixelFormatAttribute)NULL;
    
    error=CGLChoosePixelFormat(attribs, &((*windowRecord)->targetSpecific.pixelFormatObject), &numVirtualScreens);
    error=CGLCreateContext((*windowRecord)->targetSpecific.pixelFormatObject, NULL, &((*windowRecord)->targetSpecific.contextObject));
	CGLSetCurrentContext((*windowRecord)->targetSpecific.contextObject);
	
    windowWidth=(int)PsychGetWidthFromRect(rect);
    windowHeight=(int) PsychGetHeightFromRect(rect);
	//This section looks wrong because it does not allocate enough memory to insure alignment on word bounaries, which presumably is
	//dicated by the pixel format.  
    depthBytes=depth / 8;
    (*windowRecord)->surfaceSizeBytes= windowWidth * windowHeight * depthBytes;
    (*windowRecord)->surface=malloc((*windowRecord)->surfaceSizeBytes);
    CGLSetOffScreen((*windowRecord)->targetSpecific.contextObject, windowWidth, windowHeight, windowWidth * depthBytes, (*windowRecord)->surface); 
    gluOrtho2D(rect[kPsychLeft], rect[kPsychRight], rect[kPsychBottom], rect[kPsychTop]);
          
    //Fill in the window record.
    (*windowRecord)->windowType=kPsychSystemMemoryOffscreen;
    (*windowRecord)->screenNumber=kPsychUnaffiliatedWindow;
    PsychCopyRect((*windowRecord)->rect, rect);
    (*windowRecord)->depth=depth;
	

    //mark the contents of the window record as valid.  Between the time it is created (always with PsychCreateWindowRecord) and when it is marked valid 
    //(with PsychSetWindowRecordValid) it is a potential victim of PsychPurgeInvalidWindows.  
    PsychSetWindowRecordValid(*windowRecord);
    return(TRUE);
}


void PsychOSCloseWindow(PsychWindowRecordType *windowRecord)
{    
    CGDirectDisplayID				cgDisplayID;

    // Disable rendering context:
    CGLSetCurrentContext(NULL);
 
    // MK: Hack, needed to work around a "screen corruption on shutdown" bug.
    // When closing stereo display windows, it sometimes leads to a completely
    // messed up and unusable display.
    if (PsychIsOnscreenWindow(windowRecord)) {
        PsychReleaseScreen(windowRecord->screenNumber);
        // Destroy onscreen window, detach context:
        CGLClearDrawable(windowRecord->targetSpecific.contextObject);
		if (windowRecord->targetSpecific.glusercontextObject) CGLClearDrawable(windowRecord->targetSpecific.glusercontextObject);
        PsychCaptureScreen(windowRecord->screenNumber);
    }
    // Destroy pixelformat object:
    CGLDestroyPixelFormat(windowRecord->targetSpecific.pixelFormatObject);
    // Destroy rendering context:
    CGLDestroyContext(windowRecord->targetSpecific.contextObject);
	if (windowRecord->targetSpecific.glusercontextObject) CGLDestroyContext(windowRecord->targetSpecific.glusercontextObject);

    // Disable low-level mapping of framebuffer cursor memory:
    if (PsychPrefStateGet_VBLTimestampingMode()>0) {
        // Map screen number to physical display handle cgDisplayID:
        PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, windowRecord->screenNumber);
        
        // Unmap memory from our VM space, if any mapped:
        if (fbsharedmem[windowRecord->screenNumber].shmem) {
            IOConnectUnmapMemory(fbsharedmem[windowRecord->screenNumber].connect, kIOFBCursorMemory, mach_task_self(), (vm_address_t) fbsharedmem[windowRecord->screenNumber].shmem);
            fbsharedmem[windowRecord->screenNumber].shmem = NULL;
        }
        
        // Close the service port:
        IOServiceClose(fbsharedmem[windowRecord->screenNumber].connect);  

        // Cleanup done.
    }
    
    return;
}

/*
 * PsychOSFlipWindowBuffers() -- OS-X swapbuffers call.
 */
void PsychOSFlipWindowBuffers(PsychWindowRecordType *windowRecord)
{
    // Trigger the "Front <-> Back buffer swap (flip) (on next vertical retrace)":
    CGLFlushDrawable(windowRecord->targetSpecific.contextObject);
}

/*
    PsychOSSetGLContext()
    
    Set the window to which GL drawing commands are sent.  
*/
void PsychOSSetGLContext(PsychWindowRecordType *windowRecord)
{
    // Setup new context if it isn't already setup. -> Avoid redundant context switch.
    if (CGLGetCurrentContext() != windowRecord->targetSpecific.contextObject) {
		if (CGLGetCurrentContext() != NULL) {
			// We need to glFlush the old context before switching, otherwise race-conditions may occur:
			glFlush();
			
			// Need to unbind any FBO's in old context before switch, otherwise bad things can happen...
			if (glBindFramebufferEXT) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
		
		// Switch to new context:
		CGLSetCurrentContext(windowRecord->targetSpecific.contextObject);
		
		// If imaging pipe is active, we need to reset the current drawing target, so it and its
		// FBO bindings get properly reinitialized before next use. In non-imaging mode this is
		// not needed, because the new context already contains the proper setup for transformations,
		// drawbuffers and such, as well as the matching content in the backbuffer:
		if (windowRecord->imagingMode > 0) PsychSetDrawingTarget(NULL);
    }
}

/* Same as PsychOSSetGLContext() but for selecting userspace rendering context,
 * optionally copying state from PTBs context.
 */
void PsychOSSetUserGLContext(PsychWindowRecordType *windowRecord, Boolean copyfromPTBContext)
{
	// Child protection:
	if (windowRecord->targetSpecific.glusercontextObject == NULL) PsychErrorExitMsg(PsychError_user, "GL Userspace context unavailable! Call InitializeMatlabOpenGL *before* Screen('OpenWindow')!");
	
	if (copyfromPTBContext) {
		// Syncing of external contexts state with PTBs internal state requested. Do it:
		CGLSetCurrentContext(NULL);
		CGLCopyContext(windowRecord->targetSpecific.contextObject, windowRecord->targetSpecific.glusercontextObject, GL_ALL_ATTRIB_BITS);
	}
	
    // Setup new context if it isn't already setup. -> Avoid redundant context switch.
    if (CGLGetCurrentContext() != windowRecord->targetSpecific.glusercontextObject) {
        CGLSetCurrentContext(windowRecord->targetSpecific.glusercontextObject);
    }
}

/*
    PsychOSUnsetGLContext()
    
    Clear the drawing context.  
*/
void PsychOSUnsetGLContext(PsychWindowRecordType *windowRecord)
{
    CGLSetCurrentContext(NULL);
}

/* PsychOSSetVBLSyncLevel - Enable or disable synchronization of bufferswaps to
 * onset of vertical retrace. This is the OS-X version...
 */
void PsychOSSetVBLSyncLevel(PsychWindowRecordType *windowRecord, int swapInterval)
{
    CGLError	error;
    long myinterval = (long) swapInterval;
    error=CGLSetParameter(windowRecord->targetSpecific.contextObject, kCGLCPSwapInterval, &myinterval);
    if (error) {
        if (PsychPrefStateGet_Verbosity()>1) printf("\nPTB-WARNING: FAILED to enable synchronization to vertical retrace!\n\n");
    }
}

