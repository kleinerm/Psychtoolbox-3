/*
	PsychToolbox3/Source/Common/Screen/PsychWindowSupport.c
	
	PLATFORMS:	
	
		All.  
				
	AUTHORS:
	
		Allen Ingling		awi	Allen.Ingling@nyu.edu
		Mario Kleiner		mk	mario.kleiner at tuebingen.mpg.de

	HISTORY:
	
		12/20/02		awi	Wrote it mostly by modifying SDL-specific refugees (from an experimental SDL-based Psychtoolbox).
		11/16/04		awi	Added description.
		4/22/05			mk      Added support for OpenGL stereo windows and enhanced Flip-behaviour:
						Flip'ing at specified deadline, retaining instead of clear'ing backbuffer during flip,
						return of stimulus onset related timestamps, properly syncing to VBL.
		4/29/05			mk      Additional safety checks for VBL sync in PsychOpenOnscreenWindow().
		5/14/05			mk      Additional safety checks for insufficient gfx-hardware and multi-display setups,
						failing beam-position queries. New refresh interval estimation code, reworked Flip.
		5/19/05			mk      Extra check for 'flipwhen' values over 1000 secs in future: Abort.
		5/30/05			mk      Check for Screen('Preference', 'SkipSyncTests', 1) -> Shortened tests, if set.
		6/09/05			mk      Experimental support for busy-waiting for VBL and for multi-flip of stereo displays.
		9/30/05			mk      Added PsychRealtimePriority for improving timing tests in PsychOpenWindow()
		9/30/05			mk      Added check for Screen('Preference', 'VisualDebugLevel', level) -> Amount of vis. feedback.
		10/10/05		mk      Important Bugfix for PsychRealtimePriority() - didn't switch back to non-RT priority!!
		10/19/05		awi	Cast NULL to CGLPixelFormatAttribute type to make the compiler happy.
		12/27/05		mk	PsychWindowSupport.h/c contains the shared parts of the windows implementation for all OS'es.
		3/07/06			awi	Print warnings conditionally according to PsychPrefStateGet_SuppressAllWarnings().
		11/14/06                mk      Replace blue screen by welcome text splash screen. Tighten threshold for frame skip detector for
		                                systems without beampos queries from 1.5 to 1.2, remove 0.5 offset and use unified deadline calculation
						code for the flipwhen>0 case and the flipwhen==0 case. All this should not matter on systems with beampos
						queries, but it makes the test more sensitive on systems without beampos queries, biasing it to more false
						positives on noisy systems, reducing the chance for false negatives.
                11/15/06                mk      Experimental support for low-level queries of vbl count and time from the driver: Used for verifying
                                                beampos query timestamping and as a fallback on systems that lack beampos query support.
 
	DESCRIPTION:
	
	NOTES:
	
        Documentation on the kernel-level shared memory access to the gfx-driver can be found here:
 
        http://developer.apple.com/documentation/Darwin/Reference/IOKit/IOFramebufferShared/index.html
	
        TO DO: 
	
*/

#include "Screen.h"

#if PSYCH_SYSTEM != PSYCH_WINDOWS
#include "ptbstartlogo.h"
#else
/* This is a placeholder for ptbstartlogo.h until the fu%&$ing M$-Compiler can handle it.
 * GIMP RGBA C-Source image dump (welcomeWait.c)
 */
static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[4 + 1];
} gimp_image = {
  1, 1, 4, "    ",};
#endif

/* Flag which defines if userspace rendering is active: */
static boolean inGLUserspace = FALSE;

// We keep track of the current active rendertarget in order to
// avoid needless state changes:
static PsychWindowRecordType* currentRendertarget = NULL;

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
void PsychRebindARBExtensionsToCore(void)
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
    // if (NULL == glGetUniformLocation) glGetUniformLocation = (GLint (*)(GLint, const GLchar*)) glGetUniformLocationARB;
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
    
    // Misc other stuff to remap...
    if (NULL == glDrawRangeElements) glDrawRangeElements = glDrawRangeElementsEXT;
    return;
}


/*
    PsychOpenOnscreenWindow()
    
    This routine first calls the operating system dependent setup routine in PsychWindowGlue to open
    an onscreen - window and create, setup and attach an OpenGL rendering context to it.

    Then it does all the OS independent setup stuff, like sanity and timing checks, determining the
    real monitor refresh and flip interval and start/endline of VBL via measurements and so on...

    -The pixel format and the context are stored in the target specific field of the window recored.  Close
    should clean up by destroying both the pixel format and the context.
    
    -We mantain the context because it must be be made the current context by drawing functions to draw into 
    the specified window.
    
    -We maintain the pixel format object because there seems to be no way to retrieve that from the context.
    
    -To tell the caller to clean up PsychOpenOnscreenWindow returns FALSE if we fail to open the window. It 
    would be better to just issue an PsychErrorExit() and have that clean up everything allocated outside of
    PsychOpenOnscreenWindow().
    
    MK: The new option 'stereomode' allows selection of stereo display instead of mono display:
    0 (default) == Old behaviour -> Monoscopic rendering context.
    >0          == Stereo display, where the number defines the type of stereo algorithm to use.
    =1          == Use OpenGL built-in stereo by creating a context/window with left- and right backbuffer.
    =2          == Use compressed frame stereo: Put both views into one framebuffer, one in top half, other in lower half.
 
    MK: Calibration/Measurement code was added that estimates the monitor refresh interval and the number of
        the last scanline (End of vertical blank interval). This increases time for opening an onscreen window
        by up to multiple seconds on slow (60 Hz) displays, but allows reliable syncing to VBL and kind of WaitBlanking
        functionality in Screen('Flip')... Also lots of tests for proper working of VBL-Sync and other stuff have been added.
        Contains experimental support for flipping multiple displays synchronously, e.g., for dual display stereo setups.
 
*/
boolean PsychOpenOnscreenWindow(PsychScreenSettingsType *screenSettings, PsychWindowRecordType **windowRecord, int numBuffers, int stereomode, double* rect, int multiSample)
{
    PsychRectType dummyrect;
    double ifi_nominal=0;    
    double ifi_estimate = 0;
    int retry_count=0;    
    int numSamples=0;
    double stddev=0;
    double maxsecs;    
    int VBL_Endline = -1;
    long vbl_startline, dummy_width;
    int i, maxline, bp;
    double tsum=0;
    double tcount=0;
    double ifi_beamestimate = 0;

    CGDirectDisplayID				cgDisplayID;
    int attribcount=0;
    int ringTheBell=-1;
    long VRAMTotal=0;
    long TexmemTotal=0;
    bool multidisplay = FALSE;
    bool sync_trouble = false;
    bool sync_disaster = false;
    int  skip_synctests = PsychPrefStateGet_SkipSyncTests();
    int visual_debuglevel = PsychPrefStateGet_VisualDebugLevel();
    int conserveVRAM = PsychPrefStateGet_ConserveVRAM();
    int logo_x, logo_y;
    GLboolean	isFloatBuffer;
    GLint bpc;

    // OS-9 emulation? If so, then we only work in double-buffer mode:
    if (PsychPrefStateGet_EmulateOldPTB()) numBuffers = 2;

    // Child protection: We need 2 AUX buffers for compressed stereo.
    if ((conserveVRAM & kPsychDisableAUXBuffers) && (stereomode==kPsychCompressedTLBRStereo || stereomode==kPsychCompressedTRBLStereo)) {
        printf("ERROR! You tried to disable AUX buffers via Screen('Preference', 'ConserveVRAM')\n while trying to use compressed stereo, which needs AUX-Buffers!\n");
        return(FALSE);
    }
    
    //First allocate the window recored to store stuff into.  If we exit with an error PsychErrorExit() should
    //call PsychPurgeInvalidWindows which will clean up the window record. 
    PsychCreateWindowRecord(windowRecord);  		//this also fills the window index field.

    // Assign the passed windowrect 'rect' to the new window:
    PsychCopyRect((*windowRecord)->rect, rect);
    
    // Assign requested level of multisampling for hardware Anti-Aliasing: 0 means - No hw-AA,
    // n>0 means: use hw-AA and try to get multisample buffers for at least n samples per pixel.
    // Todays hardware (as of mid 2006) typically supports 2x and 4x AA, Radeons support 6x AA.
    // If a request for n samples/pixel can't be satisfied by the hardware/OS, then we fall back
    // to the highest possible value. Worst case: We fall back to non-multisampled mode.
    // We pass in the requested value, after opening the window, the windowRecord contains
    // the real value used.
    (*windowRecord)->multiSample = multiSample;

    // Assign requested color buffer depth:
    (*windowRecord)->depth = screenSettings->depth.depths[0];
    
    //if (PSYCH_DEBUG == PSYCH_ON) printf("Entering PsychOSOpenOnscreenWindow\n");
    
    // Call the OS specific low-level Window & Context setup routine:
    if (!PsychOSOpenOnscreenWindow(screenSettings, (*windowRecord), numBuffers, stereomode, conserveVRAM)) {
        printf("\nPTB-ERROR[Low-Level setup of window failed]:The specified display may not support double buffering and/or stereo output. There could be insufficient video memory\n\n");
        FreeWindowRecordFromPntr(*windowRecord);
        return(FALSE);
    }

    // Now we have a valid, visible onscreen (fullscreen) window with valid
    // OpenGL context attached. We mark it immediately as Onscreen window,
    // so in case of an error, the Screen('CloseAll') routine can properly
    // close it and release the Window system and OpenGL ressources.
    if(numBuffers==1) {
      (*windowRecord)->windowType=kPsychSingleBufferOnscreen;
    } 
    else {
      (*windowRecord)->windowType=kPsychDoubleBufferOnscreen;
    }

    // Dynamically rebind core extensions: Ugly ugly...
    PsychRebindARBExtensionsToCore();
    
    if ((*windowRecord)->depth == 30 || (*windowRecord)->depth == 64 || (*windowRecord)->depth == 128) {

        // Floating point framebuffer active? GL_RGBA_FLOAT_MODE_ARB would be a viable alternative?
        glGetBooleanv(GL_COLOR_FLOAT_APPLE, &isFloatBuffer);
        if (isFloatBuffer) {
            printf("PTB-INFO: Floating point precision framebuffer enabled.\n");
        }
        else {
            printf("PTB-INFO: Fixed point precision integer framebuffer enabled.\n");
        }
        
        // Query and show bpc for all channels:
        glGetIntegerv(GL_RED_BITS, &bpc);
        printf("PTB-INFO: Frame buffer provides %i bits for red channel.\n", bpc);
        glGetIntegerv(GL_GREEN_BITS, &bpc);
        printf("PTB-INFO: Frame buffer provides %i bits for green channel.\n", bpc);
        glGetIntegerv(GL_BLUE_BITS, &bpc);
        printf("PTB-INFO: Frame buffer provides %i bits for blue channel.\n", bpc);
        glGetIntegerv(GL_ALPHA_BITS, &bpc);
        printf("PTB-INFO: Frame buffer provides %i bits for alpha channel.\n", bpc);
    }

	// Query if this onscreen window has a backbuffer with alpha channel, i.e.
	// it has more than zero alpha bits: 
	glGetIntegerv(GL_ALPHA_BITS, &bpc);
	
	// Windows are either RGB or RGBA, so either 3 or 4 channels. Here we
	// assign the default depths for this window record. This value needs to get
	// overriden when imaging pipeline is active, because there we use framebuffer
	// objects as backing store which always have RGBA 4 channel format.
	(*windowRecord)->nrchannels = (bpc > 0) ? 4 : 3;

	// We need the real color depth (bits per color component) of the framebuffer attached
	// to this onscreen window. We need it to setup color range correctly. Let's assume the
	// red bits value is representative for the green and blue channel as well:
	glGetIntegerv(GL_RED_BITS, &bpc);
	(*windowRecord)->colorRange = (double) ((1 << bpc) - 1);

    // Now we start to fill in the remaining windowRecord with settings:
    // -----------------------------------------------------------------

    if (PSYCH_SYSTEM == PSYCH_OSX) {
        // Override for window rectangle: On OS-X we only support fullscreen mode:
        PsychCopyRect((*windowRecord)->rect, screenSettings->rect);
    }

    // Normalize final windowRect: It is shifted so that its top-left corner is
    // always the origin (0,0). This way we lose the information about the absolute
    // position of the window on the screen, but this can be still queried from the
    // Screen('Rect') command for a screen index. Not normalizing creates breakage
    // in a lot of our own internal code, many demos and probably a lot of user code.
    PsychCopyRect(dummyrect, (*windowRecord)->rect);
    PsychNormalizeRect(dummyrect, (*windowRecord)->rect);

    // Compute logo_x and logo_y x,y offset for drawing the startup logo:
    logo_x = ((int) PsychGetWidthFromRect((*windowRecord)->rect) - (int) gimp_image.width) / 2;
    logo_x = (logo_x > 0) ? logo_x : 0;
    logo_y = ((int) PsychGetHeightFromRect((*windowRecord)->rect) - (int) gimp_image.height) / 2;
    logo_y = (logo_y > 0) ? logo_y : 0;

    //if (PSYCH_DEBUG == PSYCH_ON) printf("OSOpenOnscreenWindow done.\n");

    // Retrieve real number of samples/pixel for multisampling:
    (*windowRecord)->multiSample = 0;
    while(glGetError()!=GL_NO_ERROR);
    glGetIntegerv(GL_SAMPLES_ARB, (GLint*) &((*windowRecord)->multiSample));
    while(glGetError()!=GL_NO_ERROR);

    // Retrieve display handle for beamposition queries:
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenSettings->screenNumber);

    // Retrieve final vbl_startline, aka physical height of the display in pixels:
    PsychGetScreenSize(screenSettings->screenNumber, &dummy_width, &vbl_startline);
      
    // Associated screens id and depth:
    (*windowRecord)->screenNumber=screenSettings->screenNumber;
    (*windowRecord)->depth=PsychGetScreenDepthValue(screenSettings->screenNumber);
    
    // MK: Assign stereomode: 0 == monoscopic (default) window. >0 Stereo output window, where
    // the number specifies the type of stereo-algorithm used. Currently value 1 is
    // supported, which means: Output via OpenGL built-in stereo facilities. This can drive
    // all Stereo display devices that are supported by MacOS-X, e.g., the "Crystal Space"
    // Liquid crystal eye shutter glasses.
    // We also support value 2 and 3, which means "compressed" stereo: Only one framebuffer is used,
    // the left-eyes image is placed in the top half of the framebuffer, the right-eyes image is
    // placed int the bottom half of the buffer. One looses half of the vertical image resolution,
    // but both views are encoded in one video frame and can be decoded by external stereo-hardware,
    // e.g., the one available from CrystalEyes, this allows for potentially faster refresh.
    // Mode 4/5 is implemented by simple manipulations to the glViewPort...
    (*windowRecord)->stereomode = (stereomode>=0 && stereomode<=9) ? stereomode : 0;
    
    // Setup timestamps and pipeline state for 'Flip' and 'DrawingFinished' commands of Screen:
    (*windowRecord)->time_at_last_vbl = 0;
    (*windowRecord)->PipelineFlushDone = false;
    (*windowRecord)->backBufferBackupDone = false;
    (*windowRecord)->nr_missed_deadlines = 0;
    (*windowRecord)->IFIRunningSum = 0;
    (*windowRecord)->nrIFISamples = 0;
    (*windowRecord)->VBL_Endline = -1;

    // Set the textureOrientation of onscreen windows to 2 aka "Normal, upright, non-transposed".
    // Textures of onscreen windows are created on demand as backup of the content of the onscreen
    // windows framebuffer. This happens in PsychSetDrawingTarget() if a switch from onscreen to
    // offscreen drawing target happens and the slow-path is used due to lack of Framebuffer-objects.
    // See code in PsychDrawingTarget()...
    (*windowRecord)->textureOrientation=2;

    // Enable GL-Context of current onscreen window:
    PsychSetGLContext(*windowRecord);

    // Perform a full reset of the framebuffer-object switching code:
    PsychSetDrawingTarget(NULL);
    
    // Enable this windowRecords framebuffer as current drawingtarget. This will also setup
    // the projection and modelview matrices, viewports and such to proper values:
    PsychSetDrawingTarget(*windowRecord);
    
	if(PsychPrefStateGet_Verbosity()>2) {
		  printf("\n\nPTB-INFO: This is the OpenGL-Psychtoolbox version %i.%i.%i. ", PsychGetMajorVersionNumber(), PsychGetMinorVersionNumber(), PsychGetPointVersionNumber());
		  printf("Type 'PsychtoolboxVersion' for more detailed version information.\n"); 
		  printf("PTB-INFO: Psychtoolbox is licensed to you under terms of the GNU General Public License (GPL). See file 'License.txt' in the\n");
		  printf("PTB-INFO: Psychtoolbox root folder for a copy of the GPL license.\n\n");
		
		  printf("\n\nOpenGL-Extensions are: %s\n\n", glGetString(GL_EXTENSIONS));
	}
	
	if (PsychPrefStateGet_EmulateOldPTB() && PsychPrefStateGet_Verbosity()>1) {
			printf("PTB-INFO: Psychtoolbox is running in compatibility mode to old MacOS-9 PTB. This is an experimental feature with\n");
			printf("PTB-INFO: limited support and possibly significant bugs hidden in it! Use with great caution and avoid if you can!\n");
			printf("PTB-INFO: Currently implemented: Screen('OpenOffscreenWindow'), Screen('CopyWindow') and Screen('WaitBlanking')\n");
	}

    
#if PSYCH_SYSTEM == PSYCH_OSX
    CGLRendererInfoObj				rendererInfo;
    CGOpenGLDisplayMask 			displayMask;
    CGLError					error;

    displayMask=CGDisplayIDToOpenGLDisplayMask(cgDisplayID);

    if (true) {
        long numRenderers, i;
        error= CGLQueryRendererInfo(displayMask, &rendererInfo, &numRenderers);
        if(numRenderers>1) numRenderers=1;
        for(i=0;i<numRenderers;i++) {
            CGLDescribeRenderer(rendererInfo, i, kCGLRPVideoMemory, &VRAMTotal);
            CGLDescribeRenderer(rendererInfo, i, kCGLRPTextureMemory, &TexmemTotal);
        }
        CGLDestroyRendererInfo(rendererInfo);
    }

    // Are we running a multi-display setup? Then some tests and words of wisdom for the user are important
    // to reduce the traffic on the Psychtoolbox-Forum ;-)

    // Query number of physically connected and switched on displays...
    CGDisplayCount totaldisplaycount=0;
    CGGetOnlineDisplayList(0, NULL, &totaldisplaycount);
    
    if(PsychPrefStateGet_Verbosity()>1){
		multidisplay = (totaldisplaycount>1) ? true : false;    
		if (multidisplay) {
			printf("\n\nPTB-INFO: You are using a multi-display setup (%i active displays):\n", totaldisplaycount);
			printf("PTB-INFO: Please read 'help MultiDisplaySetups' for specific information on the Do's, Dont's,\n");
			printf("PTB-INFO: and possible causes of trouble and how to diagnose and resolve them."); 
		}
		
		if (multidisplay && (!CGDisplayIsInMirrorSet(cgDisplayID) || PsychGetNumDisplays()>1)) {
			// This is a multi-display setup with separate (non-mirrored) displays: Bad for presentation timing :-(
			// Output some warning message to user, but continue. After all its the users
			// decision... ...and for some experiments were you need to show two different stims on two connected
			// monitors (haploscope, some stereo or binocular rivalry stuff) it is necessary. Let's hope they bought
			// a really fast gfx-card with plenty of VRAM :-)
			printf("\n\nPTB-INFO: According to the operating system, some of your connected displays do not seem to \n");
			printf("PTB-INFO: be switched into mirror mode. For a discussion of mirror mode vs. non-mirror mode,\n");
			printf("PTB-INFO: please read 'help MirrorMode'.\n");
		}
		
		if (CGDisplayIsInMirrorSet(cgDisplayID) && !CGDisplayIsInHWMirrorSet(cgDisplayID)) {
			// This is a multi-display setup with software-mirrored displays instead of hardware-mirrored ones: Not so good :-(
			// Output some warning message to user, but continue. After all its the users
			// decision...
			printf("\n\nPTB-WARNING: Seems that not all connected displays are switched into HARDWARE-mirror mode!\n");
			printf("PTB-WARNING: This could cause reduced drawing performance and inaccurate/wrong stimulus\n");
			printf("PTB-WARNING: presentation timing or skipped frames when showing moving/movie stimuli.\n");
			printf("PTB-WARNING: Seems that only SOFTWARE-mirroring is available for your current setup. You could\n");
			printf("PTB-WARNING: try to promote hardware-mirroring by trying different display settings...\n");
			printf("PTB-WARNING: If you still get this warning after putting your displays into mirror-mode, then\n");
			printf("PTB-WARNING: your system is unable to use hardware-mirroring and we recommend switching to a\n");
			printf("PTB-WARNING: single display setup if you encounter timing problems...\n\n");
			// Flash our visual warning bell:
			if (ringTheBell<1) ringTheBell=1;
		}
   } 
#endif

    // If we are in stereo mode 4 or 5 (free-fusion, cross-fusion, desktop-spanning stereo),
    // we need to enable Scissor tests to restrict drawing and buffer clear operations to
    // the currently set glScissor() rectangle (which is identical to the glViewport).
    if (stereomode == 4 || stereomode == 5) glEnable(GL_SCISSOR_TEST);


    if (numBuffers<2) {
		if(PsychPrefStateGet_Verbosity()>1){
			// Setup for single-buffer mode is finished!
			printf("\n\nPTB-WARNING: You are using a *single-buffered* window. This is *strongly discouraged* unless you\n");
			printf("PTB-WARNING: *really* know what you're doing! Stimulus presentation timing and all reported timestamps\n");
			printf("PTB-WARNING: will be inaccurate or wrong and synchronization to the vertical retrace will not work.\n");
			printf("PTB-WARNING: Please use *double-buffered* windows when doing anything else than debugging the PTB.\n\n");
			// Flash our visual warning bell:
			if (ringTheBell<2) ringTheBell=2;
			if (ringTheBell>=0) PsychVisualBell((*windowRecord), 4, ringTheBell);
			//mark the contents of the window record as valid.  Between the time it is created (always with PsychCreateWindowRecord) and when it is marked valid 
			//(with PsychSetWindowRecordValid) it is a potential victim of PsychPurgeInvalidWindows. 
		}
        PsychSetWindowRecordValid(*windowRecord);
        return(TRUE);
    }
    
    // Everything below this line is only for double-buffered contexts!

    // Activate syncing to onset of vertical retrace (VBL) for double-buffered windows:
    PsychOSSetVBLSyncLevel(*windowRecord, 1);
    
    // Setup of initial interframe-interval by multiple methods, for comparison:
    
    // First we query what the OS thinks is our monitor refresh interval:
    if (PsychGetNominalFramerate(screenSettings->screenNumber) > 0) {
        // Valid nominal framerate returned by OS: Calculate nominal IFI from it.
        ifi_nominal = 1.0 / ((double) PsychGetNominalFramerate(screenSettings->screenNumber));        
    }

    // This is pure eye-candy: We clear both framebuffers to a background color,
    // just to get rid of the junk that's in the framebuffers...
    // If visual debuglevel < 4 then we clear to black background...
    if (visual_debuglevel >= 4) {
      // Clear to white to prepare drawing of our logo:
      glClearColor(1,1,1,0);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    else {
      // Clear to black:
      glClearColor(0,0,0,0);
    }

    glDrawBuffer(GL_BACK_LEFT);
    glClear(GL_COLOR_BUFFER_BIT);

    glPixelZoom(1, -1);
    if (visual_debuglevel>=4) { glRasterPos2i(logo_x, logo_y); glDrawPixels(gimp_image.width, gimp_image.height, GL_RGBA, GL_UNSIGNED_BYTE, (void*) &gimp_image.pixel_data[0]); }
    PsychOSFlipWindowBuffers(*windowRecord);
    glClear(GL_COLOR_BUFFER_BIT);
    if (visual_debuglevel>=4) { glRasterPos2i(logo_x, logo_y); glDrawPixels(gimp_image.width, gimp_image.height, GL_RGBA, GL_UNSIGNED_BYTE, (void*) &gimp_image.pixel_data[0]); }
    PsychOSFlipWindowBuffers(*windowRecord);
    // We do it twice to clear possible stereo-contexts as well...
    if ((*windowRecord)->stereomode==kPsychOpenGLStereo) {
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT);
	if (visual_debuglevel>=4) { glRasterPos2i(logo_x, logo_y); glDrawPixels(gimp_image.width, gimp_image.height, GL_RGBA, GL_UNSIGNED_BYTE, (void*) &gimp_image.pixel_data[0]); }
	PsychOSFlipWindowBuffers(*windowRecord);
        glClear(GL_COLOR_BUFFER_BIT);
	if (visual_debuglevel>=4) { glRasterPos2i(logo_x, logo_y); glDrawPixels(gimp_image.width, gimp_image.height, GL_RGBA, GL_UNSIGNED_BYTE, (void*) &gimp_image.pixel_data[0]); }
	PsychOSFlipWindowBuffers(*windowRecord);
    }    
    glPixelZoom(1, 1);

    glDrawBuffer(GL_BACK);

    // Make sure that the gfx-pipeline has settled to a stable state...
    glFinish();
    
    // Complete skip of sync tests and all calibrations requested?
    // This should be only done if Psychtoolbox is not used as psychophysics
    // toolbox, but simply as a windowing/drawing toolkit for OpenGL in Matlab/Octave.
    if (skip_synctests<2) {
      // Normal calibration and at least some sync testing requested:

      // We perform an initial calibration using VBL-Syncing of OpenGL:
      // We use 50 samples (50 monitor refresh intervals) and provide the ifi_nominal
      // as a hint to the measurement routine to stabilize it:
      
      // We try 3 times a 5 seconds max., in case something goes wrong...
      while(ifi_estimate==0 && retry_count<3) {
        numSamples=50;      // Require at least 50 *valid* samples...
        stddev=0.00010;     // Require a std-deviation less than 100 microseconds..
        maxsecs=(skip_synctests) ? 1 : 5;  // If skipping of sync-test is requested, we limit the calibration to 1 sec.
        retry_count++;
        ifi_estimate = PsychGetMonitorRefreshInterval(*windowRecord, &numSamples, &maxsecs, &stddev, ifi_nominal);
      }
      
      // Now we try if CGDisplayBeamPosition works and try to estimate monitor refresh from it
      // as well...
      
      // Check if a beamposition of 0 is returned at two points in time on OS-X:
      i = 0;
      if (((int) CGDisplayBeamPosition(cgDisplayID) == 0) && (PSYCH_SYSTEM == PSYCH_OSX)) {
        // Recheck after 2 ms on OS-X:
        PsychWaitIntervalSeconds(0.002);
        if ((int) CGDisplayBeamPosition(cgDisplayID) == 0) {
	  // A constant value of zero is reported on OS-X -> Beam position queries unsupported
	  // on this combo of gfx-driver and hardware :(
	  i=12345;
        }
      }
      
      // Check if a beamposition of -1 is returned: This would indicate that beamposition queries
      // are not available on this system: This always happens on Linux as that feature is unavailable.
      if ((-1 != ((int) CGDisplayBeamPosition(cgDisplayID))) && (i!=12345)) {
	// Switch to RT scheduling for timing tests:
	PsychRealtimePriority(true);
	
	// Code for estimating the final scanline of the vertical blank interval of display (needed by Screen('Flip')):
	
	// Check if CGDisplayBeamPosition is working properly:
	// The first test checks, if it returns changing values at all or if it returns a constant
	// value at two measurements 2 ms apart...
	i=(int) CGDisplayBeamPosition(cgDisplayID);
	PsychWaitIntervalSeconds(0.002);
	if (((int) CGDisplayBeamPosition(cgDisplayID)) == i) {
	  // CGDisplayBeamPosition returns the same value at two different points in time?!?
	  // That's impossible on anything else than a high-precision 500 Hz display!
	  // --> CGDisplayBeamPosition is not working correctly for some reason.
	  sync_trouble = true;
	  if(PsychPrefStateGet_Verbosity()>1)
	    printf("\nWARNING: Querying rasterbeam-position doesn't work on your setup! (Returns a constant value)\n");
	}
	else {
	  // CGDisplayBeamPosition works: Use it to find VBL-Endline...
	  // Sample over 50 monitor refresh frames:
	  double told, tnew;
	  for (i=0; i<50; i++) {
	    // Take beam position samples from current monitor refresh interval:
	    maxline = -1;
	    // We spin-wait until retrace and record our highest measurement:
	    while ((bp=(int) CGDisplayBeamPosition(cgDisplayID)) >= maxline) maxline=bp;
	    // We also take timestamps for "yet another way" to measure monitor refresh interval...
	    PsychGetAdjustedPrecisionTimerSeconds(&tnew);
	    if (i>0) {
	      tsum+=(tnew - told);
	      tcount+=1;
	    }
	    told=tnew;
	    
	    // Update global maximum with current sample:
	    if (maxline > VBL_Endline) VBL_Endline = maxline;
	  }
	  
	  // Setup reasonable timestamp for time of last vbl in emulation mode:
	  if (PsychPrefStateGet_EmulateOldPTB()) (*windowRecord)->time_at_last_vbl = tnew;
	}        
	
	// Switch to previous scheduling mode after timing tests:
	PsychRealtimePriority(false);
	
	// Is the VBL endline >= VBL startline - 1, aka screen height?
	if (VBL_Endline < (int) vbl_startline - 1) {
	  // Completely bogus VBL_Endline detected! Warn the user and mark VBL_Endline
	  // as invalid so it doesn't get used anywhere:
	  sync_trouble = true;
	  ifi_beamestimate = 0;
	  if(PsychPrefStateGet_Verbosity()>1)
	    printf("\nWARNING: Couldn't determine end-line of vertical blanking interval for your display! Trouble with beamposition queries?!?\n");
	}
	else {
	  // Compute ifi from beampos:
	  ifi_beamestimate = tsum / tcount;
	}
      }
      else {
	// We don't have beamposition queries on this system:
	ifi_beamestimate = 0;
	// Setup fake-timestamp for time of last vbl in emulation mode:
	if (PsychPrefStateGet_EmulateOldPTB()) PsychGetAdjustedPrecisionTimerSeconds(&((*windowRecord)->time_at_last_vbl));
      }

      // Compare ifi_estimate from VBL-Sync against beam estimate. If we are in OpenGL native
      // flip-frame stereo mode, a ifi_estimate approx. 2 times the beamestimate would be valid
      // and we would correct it down to half ifi_estimate. If multiSampling is enabled, it is also
      // possible that the gfx-hw is not capable of downsampling fast enough to do it every refresh
      // interval, so we could get an ifi_estimate which is twice the real refresh, which would be valid.
      (*windowRecord)->VideoRefreshInterval = ifi_estimate;
      if ((*windowRecord)->stereomode == kPsychOpenGLStereo || (*windowRecord)->multiSample > 0) {
        // Flip frame stereo or multiSampling enabled. Check for ifi_estimate = 2 * ifi_beamestimate:
        if ((ifi_beamestimate>0 && ifi_estimate >= 0.9 * 2 * ifi_beamestimate && ifi_estimate <= 1.1 * 2 * ifi_beamestimate) ||
	    (ifi_beamestimate==0 && ifi_nominal>0 && ifi_estimate >= 0.9 * 2 * ifi_nominal && ifi_estimate <= 1.1 * 2 * ifi_nominal)
	    ){
	  // This seems to be a valid result: Flip-interval is roughly twice the monitor refresh interval.
	  // We "force" ifi_estimate = 0.5 * ifi_estimate, so ifi_estimate roughly equals to ifi_nominal and
	  // ifi_beamestimate, in order to simplify all timing checks below. We also store this value as
	  // video refresh interval...
	  ifi_estimate = ifi_estimate * 0.5f;
	  (*windowRecord)->VideoRefreshInterval = ifi_estimate;
	  if(PsychPrefStateGet_Verbosity()>2){
	    if ((*windowRecord)->stereomode == kPsychOpenGLStereo) {
	      printf("\nPTB-INFO: The timing granularity of stimulus onset/offset via Screen('Flip') is twice as long\n");
	      printf("PTB-INFO: as the refresh interval of your monitor when using OpenGL flip-frame stereo on your setup.\n");
	      printf("PTB-INFO: Please keep this in mind, otherwise you'll be confused about your timing.\n");
	    }
	    if ((*windowRecord)->multiSample > 0) {
	      printf("\nPTB-INFO: The timing granularity of stimulus onset/offset via Screen('Flip') is twice as long\n");
	      printf("PTB-INFO: as the refresh interval of your monitor when using Anti-Aliasing at multiSample=%i on your setup.\n",
		     (*windowRecord)->multiSample);
	      printf("PTB-INFO: Please keep this in mind, otherwise you'll be confused about your timing.\n");
	    }
	  }
        }
      }
    } // End of display calibration part I of synctests.
    else {
      // Complete skip of calibration and synctests: Mark all calibrations as invalid:
      ifi_beamestimate = 0;
    }

	// HACK: FIXME: On M$-Windows, we forcefully disable beamposition queries for timestamping if we are
	// running on an Intel onboard gfx-chip. Some of them have problems (hardware or driver bugs), so
	// until i've implemented a proper detection & workaround code for detecting and fixing this, its
	// safer to disable this method:
	#if PSYCH_SYSTEM == PSYCH_WINDOWS
	if (strstr(glGetString(GL_VENDOR), "Intel") || strstr(glGetString(GL_VENDOR), "INTEL")) {
		// Shutdown advanced timestamping facilities for Intel onboard chips under Windows:
		PsychPrefStateSet_VBLTimestampingMode(-1);
		
		// And tell the user about the c%!@%p stuff he is trying to use for his work... 
		if(PsychPrefStateGet_Verbosity()>1) {
			printf("PTB-WARNING: Intel onboard graphics chip detected. Some of these are known to have severe bugs in their drivers or hardware\n");
			printf("PTB-WARNING: which could seriously screw up PTBs timestamping code. We disable high-precision timestamping until this issue\n");
			printf("PTB-WARNING: is properly resolved by a proper work-around (if possible) in a future PTB release. Screen('Flip') timestamps\n");
			printf("PTB-WARNING: will be less robust and accurate and more noisy due to this, but still at least as good as the ones provided by\n");
			printf("PTB-WARNING: the old Psychtoolboxes or other toolkits ;-)  -- Stay tuned...\n");
		}
	}
	#endif
	
	if(PsychPrefStateGet_Verbosity()>2) printf("\n\nPTB-INFO: OpenGL-Renderer is %s :: %s :: %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

    if(PsychPrefStateGet_Verbosity()>1) {
		if (strstr(glGetString(GL_RENDERER), "GDI")) {
			printf("PTB-WARNING: Seems that Microsofts OpenGL software renderer is active! This will likely cause miserable\n");
			printf("PTB-WARNING: performance and severe timing and synchronization problems. A reason could be that you run at\n");
			printf("PTB-WARNING: a too high display resolution, or the system is running out of ressources for some other reason.\n");
			printf("PTB-WARNING: Another reason could be that you disabled hardware acceleration in the display settings panel: Make sure that\n");
			printf("PTB-WARNING: in Display settings panel -> Settings -> Advanced -> Troubleshoot -> The hardware acceleration slider is\n");
			printf("PTB-WARNING: set to 'Full' (rightmost position).\n");
		}
	}
	
    if(PsychPrefStateGet_Verbosity()>2) {
      if (VRAMTotal>0) printf("PTB-INFO: Renderer has %li MB of VRAM and a maximum %li MB of texture memory.\n", VRAMTotal / 1024 / 1024, TexmemTotal / 1024 / 1024);
      printf("PTB-Info: VBL startline = %i , VBL Endline = %i\n", (int) vbl_startline, VBL_Endline);
      if (ifi_beamestimate>0) {
          printf("PTB-Info: Measured monitor refresh interval from beamposition = %f ms [%f Hz].\n", ifi_beamestimate * 1000, 1/ifi_beamestimate);
          if (PsychPrefStateGet_VBLTimestampingMode()==3 && PSYCH_SYSTEM == PSYCH_OSX) {
              printf("PTB-Info: Will try to use kernel-level interrupts for accurate Flip time stamping.\n");
          }
          else {
              if (PsychPrefStateGet_VBLTimestampingMode()>=0) printf("PTB-Info: Will use beamposition query for accurate Flip time stamping.\n");
          }
      }
      else {
          if ((PsychPrefStateGet_VBLTimestampingMode()==1 || PsychPrefStateGet_VBLTimestampingMode()==3) && PSYCH_SYSTEM == PSYCH_OSX) {
              printf("PTB-Info: Beamposition queries unsupported on this system. Will try to use kernel-level vbl interrupts as fallback.\n");
          }
          else {
              printf("PTB-Info: Beamposition queries unsupported on this system. Timestamps returned by Screen('Flip') will be less robust and accurate.\n");
          }
      }
      printf("PTB-Info: Measured monitor refresh interval from VBLsync = %f ms [%f Hz]. (%i valid samples taken, stddev=%f ms.)\n",
	     ifi_estimate * 1000, 1/ifi_estimate, numSamples, stddev*1000);
      if (ifi_nominal > 0) printf("PTB-Info: Reported monitor refresh interval from operating system = %f ms [%f Hz].\n", ifi_nominal * 1000, 1/ifi_nominal);
      printf("PTB-Info: Small deviations between reported values are normal and no reason to worry.\n");
      if ((*windowRecord)->stereomode==kPsychOpenGLStereo) printf("PTB-INFO: Stereo display via OpenGL built-in sequential frame stereo enabled.\n");
      if ((*windowRecord)->stereomode==kPsychCompressedTLBRStereo) printf("PTB-INFO: Stereo display via vertical image compression enabled (Top=LeftEye, Bot.=RightEye).\n");
      if ((*windowRecord)->stereomode==kPsychCompressedTRBLStereo) printf("PTB-INFO: Stereo display via vertical image compression enabled (Top=RightEye, Bot.=LeftEye).\n");
      if ((*windowRecord)->stereomode==kPsychFreeFusionStereo) printf("PTB-INFO: Stereo for free fusion or dual-display desktop spanning enabled (2-in-1 stereo).\n");
      if ((*windowRecord)->stereomode==kPsychFreeCrossFusionStereo) printf("PTB-INFO: Stereo via free cross-fusion enabled (2-in-1 stereo).\n");
      if ((*windowRecord)->stereomode==kPsychAnaglyphRGStereo) printf("PTB-INFO: Stereo display via Anaglyph Red-Green stereo enabled.\n");
      if ((*windowRecord)->stereomode==kPsychAnaglyphGRStereo) printf("PTB-INFO: Stereo display via Anaglyph Green-Red stereo enabled.\n");
      if ((*windowRecord)->stereomode==kPsychAnaglyphRBStereo) printf("PTB-INFO: Stereo display via Anaglyph Red-Blue stereo enabled.\n");
      if ((*windowRecord)->stereomode==kPsychAnaglyphBRStereo) printf("PTB-INFO: Stereo display via Anaglyph Blue-Red stereo enabled.\n");
      if ((PsychPrefStateGet_ConserveVRAM() & kPsychDontCacheTextures) && (strstr(glGetString(GL_EXTENSIONS), "GL_APPLE_client_storage")==NULL)) {
	// User wants us to use client storage, but client storage is unavailable :(
	printf("PTB-WARNING: You asked me for reducing VRAM consumption but for this, your graphics hardware would need\n");
	printf("PTB-WARNING: to support the GL_APPLE_client_storage extension, which it doesn't! Sorry... :(\n");
      }
      if (PsychPrefStateGet_3DGfx()) printf("PTB-INFO: Support for OpenGL 3D graphics rendering enabled: 24 bit depth-buffer and 8 bit stencil buffer attached.\n");
      if (multiSample>0) {
	if ((*windowRecord)->multiSample >= multiSample) {
	  printf("PTB-INFO: Anti-Aliasing with %i samples per pixel enabled.\n", (*windowRecord)->multiSample);
	}
	if ((*windowRecord)->multiSample < multiSample && (*windowRecord)->multiSample>0) {
	  printf("PTB-WARNING: Anti-Aliasing with %i samples per pixel enabled. Requested value of %i not supported by hardware.\n",
		 (*windowRecord)->multiSample, multiSample);
	}
	if ((*windowRecord)->multiSample <= 0) {
	  printf("PTB-WARNING: Could not enable Anti-Aliasing as requested. Your hardware does not support this feature!\n");
	}
      }
      else {
	// Multisampling enabled by external code, e.g., operating system override on M$-Windows?
	if ((*windowRecord)->multiSample > 0) {
	  // Report this, so user is aware of possible issues reg. performance and stimulus properties:
	  printf("PTB-WARNING: Anti-Aliasing with %i samples per pixel enabled, contrary to Psychtoolboxs request\n", (*windowRecord)->multiSample);                        
	  printf("PTB-WARNING: for non Anti-Aliased drawing! This will reduce drawing performance and will affect\n");                        
	  printf("PTB-WARNING: low-level properties of your visual stimuli! Check your display settings for a way\n");                        
	  printf("PTB-WARNING: to disable this behaviour if you don't like it. I will try to forcefully disable it now,\n");                        
	  printf("PTB-WARNING: but have no way to check if disabling it worked.\n");                        
	}
      }
    }
    
    // Final master-setup for multisampling:
    if (multiSample>0) {
      // Try to enable multisampling in software:
      while(glGetError()!=GL_NO_ERROR);
      glEnable(0x809D); // 0x809D == GL_MULTISAMPLE_ARB
      while(glGetError()!=GL_NO_ERROR);
      // Set sampling algorithm to the most high-quality one, even if it is
      // computationally more expensive: This will only work if the NVidia
      // GL_NV_multisample_filter_hint extension is supported...
      glHint(0x8534, GL_NICEST); // Set MULTISAMPLE_FILTER_HINT_NV (0x8534) to NICEST.
      while(glGetError()!=GL_NO_ERROR);
    }
    else {
      // Try to disable multisampling in software. That is the best we can do here:
      while(glGetError()!=GL_NO_ERROR);
      glDisable(0x809D);
      while(glGetError()!=GL_NO_ERROR);
    }
    
    // Autodetect and setup type of texture extension to use for high-perf texture mapping:
    PsychDetectTextureTarget(*windowRecord);

    if (skip_synctests < 2) {
      // Reliable estimate? These are our minimum requirements...
      if (numSamples<50 || stddev>0.001) {
        sync_disaster = true;
	if(PsychPrefStateGet_Verbosity()>1)
	  printf("\nWARNING: Couldn't compute a reliable estimate of monitor refresh interval! Trouble with VBL syncing?!?\n");
      }
      
      // Check for mismatch between measured ifi from glFinish() VBLSync method and the value reported by the OS, if any:
      // This would indicate that we have massive trouble syncing to the VBL!
      if ((ifi_nominal > 0) && (ifi_estimate < 0.9 * ifi_nominal || ifi_estimate > 1.1 * ifi_nominal)) {
        if(PsychPrefStateGet_Verbosity()>1)
	  printf("\nWARNING: Mismatch between measured monitor refresh interval and interval reported by operating system.\nThis indicates massive problems with VBL sync.\n");    
        sync_disaster = true;
      }
    
      // Another check for proper VBL syncing: We only accept monitor refresh intervals between 25 Hz and 250 Hz.
      // Lower- / higher values probably indicate sync-trouble...
      if (ifi_estimate < 0.004 || ifi_estimate > 0.040) {
        if(PsychPrefStateGet_Verbosity()>1)
	  printf("\nWARNING: Measured monitor refresh interval indicates a display refresh of less than 25 Hz or more than 250 Hz?!?\nThis indicates massive problems with VBL sync.\n");    
        sync_disaster = true;        
      }
    } // End of synctests part II.
    
    // This is a "last resort" fallback: If user requests to *skip* all sync-tests and calibration routines
    // and we are unable to compute any ifi_estimate, we will fake one in order to be able to continue.
    // Either we use the nominal framerate provided by the operating system, or - if that's unavailable as well -
    // we assume a monitor refresh of 60 Hz, the typical value for flat-panels.
    if (ifi_estimate==0 && skip_synctests) {
      ifi_estimate = (ifi_nominal>0) ? ifi_nominal : (1.0/60.0);
      (*windowRecord)->nrIFISamples=1;
      (*windowRecord)->IFIRunningSum=ifi_estimate;
      (*windowRecord)->VideoRefreshInterval = ifi_estimate;
      if(PsychPrefStateGet_Verbosity()>1) {
	if (skip_synctests < 2) {
	  printf("\nPTB-WARNING: Unable to measure monitor refresh interval! Using a fake value of %f milliseconds.\n", ifi_estimate*1000);
	}
	else {
	  printf("PTB-INFO: All display tests and calibrations disabled. Assuming a refresh interval of %f Hz. Timing will be inaccurate!\n", 1.0/ifi_estimate);
	}
      }
    }
    
    if (sync_disaster) {
      // We fail! Continuing would be too dangerous without a working VBL sync. We don't
      // want to spoil somebodys study just because s(he) is relying on a non-working sync.
      if(PsychPrefStateGet_Verbosity()>0){		
	printf("\n\n");
	printf("----- ! PTB - ERROR: SYNCHRONIZATION FAILURE ! ----\n\n");
	printf("One or more internal checks (see Warnings above) indicate that synchronization\n");
	printf("of Psychtoolbox to the vertical retrace (VBL) is not working on your setup.\n\n");
	printf("This will seriously impair proper stimulus presentation and stimulus presentation timing!\n");
	printf("Please read 'help SyncTrouble' for information about how to solve or work-around the problem.\n");
	printf("You can force Psychtoolbox to continue, despite the severe problems, by adding the command\n");
	printf("Screen('Preference', 'SkipSyncTests',1); at the top of your script, if you really know what you are doing.\n\n\n");
      }
      
      // Abort right here if sync tests are enabled:
      if (!skip_synctests) return(FALSE);

      // Flash our visual warning bell at alert-level for 1 second if skipping sync tests is requested:
      PsychVisualBell((*windowRecord), 1, 2);
    }
    
    // Ok, basic syncing to VBL via CGLFlushDrawable + glFinish seems to work and we have a valid
    // estimate of monitor refresh interval...
    
    // Check for mismatch between measured ifi from beamposition and from glFinish() VBLSync method.
    // This would indicate that the beam position is reported from a different display device
    // than the one we are VBL syncing to. -> Trouble!
    if ((ifi_beamestimate < 0.8 * ifi_estimate || ifi_beamestimate > 1.2 * ifi_estimate) && (ifi_beamestimate > 0)) {
        if(PsychPrefStateGet_Verbosity()>1)
	  printf("\nWARNING: Mismatch between measured monitor refresh intervals! This indicates problems with rasterbeam position queries.\n");    
        sync_trouble = true;
    }

    if (sync_trouble) {
        // Fail-Safe: Mark VBL-Endline as invalid, so a couple of mechanisms get disabled in Screen('Flip') aka PsychFlipWindowBuffers().
        VBL_Endline = -1;
		if(PsychPrefStateGet_Verbosity()>1){		
			printf("\n\n");
			printf("----- ! PTB - WARNING: SYNCHRONIZATION TROUBLE ! ----\n\n");
			printf("One or more internal checks (see Warnings above) indicate that\n");
			printf("queries of rasterbeam position are not properly working for your setup.\n\n");
			printf("Psychtoolbox will work around this by using a different timing algorithm, \n");
			printf("but it will cause Screen('Flip') to report less accurate/robust timestamps\n");
			printf("for stimulus timing.\n");
			printf("Read 'help BeampositionQueries' for more info and troubleshooting tips.\n");
			printf("\n\n");
			// Flash our visual warning bell:
			if (ringTheBell<2) ringTheBell=2;
		}
    }

    // The start of a gfx-card "Blacklist"...
    if ((strstr(glGetString(GL_VENDOR), "ATI")!=NULL) && multidisplay && PSYCH_SYSTEM == PSYCH_OSX) {
        // ATI card detected -> Give hint to be extra cautious about beampos...
		if(PsychPrefStateGet_Verbosity()>2){	
			printf("\n\nPTB-HINT: Your graphics card is KNOWN TO HAVE TROUBLE with beamposition queries on some dual display setups\n");
			printf("PTB-HINT: due to an ATI driver bug in all versions of MacOS-X 10.3.x and in versions of MacOS-X before 10.4.3!\n");
			printf("PTB-HINT: Please *double-check* this by setting different monitor refresh rates for the different displays.\n");
			printf("PTB-HINT: If you then get a warning about SYNCHRONIZATION TROUBLE, it might help to *physically*\n");
			printf("PTB-HINT: reconnect your displays: Swap, which display is plugged into which socket at the back-side\n");
			printf("PTB-HINT: of your computer! If that doesn't help, you'll have to switch to a single display configuration\n");
			printf("PTB-HINT: for getting highest possible timing accuracy.\n");
		}
    }

    // Assign our best estimate of the scanline which marks end of vertical blanking interval:
    (*windowRecord)->VBL_Endline = VBL_Endline;
    
    //mark the contents of the window record as valid.  Between the time it is created (always with PsychCreateWindowRecord) and when it is marked valid 
    //(with PsychSetWindowRecordValid) it is a potential victim of PsychPurgeInvalidWindows.  
    PsychSetWindowRecordValid(*windowRecord);

    // Ring the visual bell for one second if anything demands this:
    if (ringTheBell>=0 && !skip_synctests) PsychVisualBell((*windowRecord), 1, ringTheBell);

    if (PsychPrefStateGet_EmulateOldPTB()) {
        // Perform all drawing and reading in the front-buffer for OS-9 emulation mode:
        glReadBuffer(GL_FRONT);
        glDrawBuffer(GL_FRONT);
    }

    // Done.
    return(TRUE);
}


/*
    PsychOpenOffscreenWindow()
    
    Accept specifications for the offscreen window in the platform-neutral structures, convert to native OpenGL structures,
    create the texture, allocate a window record and record the window specifications and memory location there.
    TO DO:  We need to walk down the screen number and fill in the correct value for the benefit of TexturizeOffscreenWindow
*/
boolean PsychOpenOffscreenWindow(double *rect, int depth, PsychWindowRecordType **windowRecord)
{
    // This is a complete no-op as everything is implemented in SCREENOpenOffscreenWindow at the moment.
    return(TRUE);
    
    //    return(PsychOSOpenOffscreenWindow(rect, depth, windowRecord));
}


void PsychCloseWindow(PsychWindowRecordType *windowRecord)
{
    PsychWindowRecordType	**windowRecordArray;
    int                         i, numWindows; 
    
	// Extra child-protection to protect against half-initialized windowRecords...
	if (!windowRecord->isValid) {
		if (PsychPrefStateGet_Verbosity()>5) {
			printf("PTB-ERROR: Tried to destroy invalid windowRecord. Screw up in init sequence?!? Skipped.\n");
			fflush(NULL);
		}
		
		return;
	}
	
	// If our to-be-destroyed windowRecord is currently bound as drawing target,
	// e.g. as onscreen window or offscreen window, then we need to soft-reset
	// our drawing engine - Unbind its FBO (if any) and reset current target to
	// 'none'.
	if (PsychGetDrawingTarget() == windowRecord) PsychSetDrawingTarget(NULL);
	
    if(PsychIsOnscreenWindow(windowRecord)){
                // Free possible shadow textures:
                PsychFreeTextureForWindowRecord(windowRecord);        
                
                // Make sure that OpenGL pipeline is done & idle for this window:
                PsychSetGLContext(windowRecord);
				
				// Execute hook chain for OpenGL related shutdown:
				PsychPipelineExecuteHook(windowRecord, kPsychCloseWindowPreGLShutdown, NULL, NULL, FALSE, FALSE, NULL, NULL, NULL, NULL);

				// Sync and idle the pipeline:
                glFinish();
                
				// Shutdown only OpenGL related parts of imaging pipeline for this windowRecord, i.e.
				// do the shutdown work which still requires a fully functional OpenGL context and
				// hook-chains:
				PsychShutdownImagingPipeline(windowRecord, TRUE);
				
				// Sync and idle the pipeline again:
                glFinish();

                // We need to NULL-out all references to the - now destroyed - OpenGL context:
                PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);
                for(i=0;i<numWindows;i++) {
                    if (windowRecordArray[i]->targetSpecific.contextObject == windowRecord->targetSpecific.contextObject &&
                        (windowRecordArray[i]->windowType==kPsychTexture || windowRecordArray[i]->windowType==kPsychProxyWindow)) {
                        windowRecordArray[i]->targetSpecific.contextObject = NULL;
						windowRecordArray[i]->targetSpecific.glusercontextObject = NULL;
                    }
                }
                PsychDestroyVolatileWindowRecordPointerList(windowRecordArray);

                // Disable rendering context:
                PsychOSUnsetGLContext(windowRecord);

				// Call OS specific low-level window close routine:
				PsychOSCloseWindow(windowRecord);

                windowRecord->targetSpecific.contextObject=NULL;
				
				// Execute hook chain for final non-OpenGL related shutdown:
				PsychPipelineExecuteHook(windowRecord, kPsychCloseWindowPostGLShutdown, NULL, NULL, FALSE, FALSE, NULL, NULL, NULL, NULL);				
    }
    else if(windowRecord->windowType==kPsychTexture) {
                // Texture or Offscreen window - which is also just a form of texture.
				PsychFreeTextureForWindowRecord(windowRecord);

				// Shutdown only OpenGL related parts of imaging pipeline for this windowRecord, i.e.
				// do the shutdown work which still requires a fully functional OpenGL context and
				// hook-chains:
				PsychShutdownImagingPipeline(windowRecord, TRUE);
    }
    else if(windowRecord->windowType==kPsychProxyWindow) {
				// Proxy window object without associated OpenGL state or content.
				// Run shutdown sequence for imaging pipeline in case the proxy has bounce-buffer or
				// lookup table textures or FBO's attached:
				PsychShutdownImagingPipeline(windowRecord, TRUE);
    }
    else if(windowRecord->windowType==kPsychNoWindow) {
				// Partially initialized windowRecord, not yet associated to a real Window system
				// window or OpenGL rendering context. We skip this one - there's nothing to do.
				// Well almost... ...we output some warning, as something must have screwed up seriously if
				// we reached this point in control-flow...
				printf("PTB-ERROR: Something is screwed up seriously! Please read all warnings and error messages\n");
				printf("PTB-ERROR: above these lines very carefully to assess and fix the problem...\n");
				fflush(NULL);
				return;
    }
    else {
                // If we reach this point then we've really screwed up, e.g., internal memory corruption.
				PsychErrorExitMsg(PsychError_internal, "FATAL ERROR: Unrecognized window type. Memory corruption?!?");
    }
    
    if (PsychIsOnscreenWindow(windowRecord) && (windowRecord->nr_missed_deadlines>0)) {
		if(PsychPrefStateGet_Verbosity()>1) {
			printf("\n\nWARNING: PTB's Screen('Flip') command missed the requested stimulus presentation deadline\n");
			printf("WARNING: a total of %i times during this session!\n\n", windowRecord->nr_missed_deadlines);
		}
    }
    
    if (PsychIsOnscreenWindow(windowRecord) && PsychPrefStateGet_SkipSyncTests()) {
        if(PsychPrefStateGet_Verbosity()>1){
			printf("\n\nWARNING: This session of your experiment was run by you with the setting Screen('Preference', 'SkipSyncTests', %i).\n",
			       (int) PsychPrefStateGet_SkipSyncTests());
			printf("WARNING: This means that some internal self-tests and calibrations were skipped. Your stimulus presentation timing\n");
			printf("WARNING: may have been wrong. This is fine for development and debugging of your experiment, but for running the real\n");
			printf("WARNING: study, please make sure to set Screen('Preference', 'SkipSyncTests', 0) for maximum accuracy and reliability.\n");
		}
    }

	// Shutdown non-OpenGL related parts of imaging pipeline for this windowRecord:
	PsychShutdownImagingPipeline(windowRecord, FALSE);

    PsychErrorExit(FreeWindowRecordFromPntr(windowRecord));
}


/*
    PsychFlushGL()
    
    Enforce rendering of all pending OpenGL drawing commands and wait for render completion.
    This routine is called at the end of each Screen drawing subfunction. A call to it signals
    the end of a single Matlab drawing command.
 
    -If this is an onscreen window in OS-9 emulation mode we call glFinish();

    -In all other cases we don't do anything because CGLFlushDrawable which is called by PsychFlipWindowBuffers()
    implicitley calls glFlush() before flipping the buffers. Apple warns of lowered perfomance if glFlush() is called 
    immediately before CGLFlushDrawable().
 
    Note that a glFinish() after each drawing command can significantly impair overall drawing performance and
    execution speed of Matlab Psychtoolbox scripts, because the parallelism between CPU and GPU breaks down completely
    and we can run out of DMA command buffers, effectively stalling the CPU!
 
    We need to do this to provide backward compatibility for old PTB code from OS-9 or Win PTB, where
    synchronization to VBL is done via WaitBlanking: After a WaitBlanking, all drawing commands must execute as soon
    as possible and a GetSecs - call after *any* drawing command must return a useful timestamp for stimulus onset
    time. We can only achieve a compatible semantic by using a glFinish() after each drawing op.
*/
void PsychFlushGL(PsychWindowRecordType *windowRecord)
{
    if(PsychIsOnscreenWindow(windowRecord) && PsychPrefStateGet_EmulateOldPTB()) glFinish();            
}

/*
    PsychFlipWindowBuffers()
    
    Flip front and back buffers in sync with vertical retrace (VBL) and sync Matlab to VBL.
    Returns various timestamps related to sync to VBL, so experimenters can check proper
    syncing and presentation timing by themselves. Also contains an automatic "skipped frames"
    /"missed presentation deadline" detector, which is pretty reliable.
    Allows to not clear the framebuffer after flip, but keep it as before flip - allows
    incremental drawing of stims. Allows to flip not at next retrace, but at the retrace
    immediately after some deadline (spec'd in system time) has been reached.
    Optimizes rendering in collaboration with new SCREENDrawingFinished.c.
 
    Accepts:
    dont_clear flag: 0 = glClear after flip, 1 = restore backbuffer after flip, 2 = don't do anything.
    flipwhen: 0 = traditional flip (flip on next VBL), >0 flip at VBL immediately after time "flipwhen" has been reached.
             -1 = don't sync PTB's execution to VBL, aka sync stimulus onset to VBL but don't pause execution up to then
                  This also disables all timestamping and deadline checking code and makes synchronization of Matlabs
                  execution locked to the VBL impossible. -> Only useful for very special cases...
    
    Returns:
    double value VBL start time: Calculated time of VBL start (aka buffer swap) from timestamp, beamposition and IFI.
    beamPosAtFlip = Position of monitor scanning beam at time that timestamp was taken.
    miss_estimate = Estimate of how far we've missed our presentation deadline. >0 == No deadline-miss, < 0 == deadline-miss
    time_at_onset = Estimated time when stimulus onset, aka end of VBL, aka beamposition==0 occurs.
    time_at_flipend = Timestamp taken shortly before return of FlipWindowBuffers for benchmarking.
 
*/
double PsychFlipWindowBuffers(PsychWindowRecordType *windowRecord, int multiflip, int vbl_synclevel, int dont_clear, double flipwhen, int* beamPosAtFlip, double* miss_estimate, double* time_at_flipend, double* time_at_onset)
{
    int screenheight, screenwidth;
    GLint read_buffer, draw_buffer;
    unsigned char bufferstamp;
    const boolean vblsyncworkaround=false;  // Setting this to 'true' would enable some checking code. Leave it false by default.
    static unsigned char id=1;
    boolean sync_to_vbl;                    // Should we synchronize the CPU to vertical retrace? 
    double tremaining;                      // Remaining time to flipwhen - deadline
    CGDirectDisplayID displayID;            // Handle for our display - needed for beampos-query.
    double time_at_vbl=0;                   // Time (in seconds) when last Flip in sync with start of VBL happened.
    double currentflipestimate;             // Estimated video flip interval in seconds at current monitor frame rate.
    double currentrefreshestimate;          // Estimated video refresh interval in seconds at current monitor frame rate.
    double tshouldflip;                     // Deadline for a successfull flip. If time_at_vbl > tshouldflip --> Deadline miss!
    double slackfactor;                     // Slack factor for deadline miss detection.
    double vbl_startline;
    double vbl_endline;
    double vbl_lines_elapsed, onset_lines_togo;
    double vbl_time_elapsed; 
    double onset_time_togo;
    long scw, sch;
    psych_uint64 preflip_vblcount;          // VBL counters and timestamps acquired from low-level OS specific routines.
    psych_uint64 postflip_vblcount;         // Currently only supported on OS-X, but Linux/X11 implementation will follow.
    double preflip_vbltimestamp = -1;
    double postflip_vbltimestamp = -1;
	unsigned int vbltimestampquery_retrycount = 0;
	
    int vbltimestampmode = PsychPrefStateGet_VBLTimestampingMode();
    
    PsychWindowRecordType **windowRecordArray=NULL;
    int	i;
    int numWindows=0; 
    
    // Child protection:
    if(windowRecord->windowType!=kPsychDoubleBufferOnscreen)
        PsychErrorExitMsg(PsychError_internal,"Attempt to swap a single window buffer");
    
    // Retrieve estimate of interframe flip-interval:
    if (windowRecord->nrIFISamples > 0) {
        currentflipestimate=windowRecord->IFIRunningSum / ((double) windowRecord->nrIFISamples);
    }
    else {
        // We don't have a valid estimate! This will screw up all timestamping, checking and waiting code!
        // It also indicates that syncing to VBL doesn't work!
        currentflipestimate=0;
        // We abort - This is too unsafe...
        PsychErrorExitMsg(PsychError_internal,"Flip called, while estimate of monitor flip interval is INVALID -> Syncing trouble -> Aborting!");
    }
    
    // Retrieve estimate of monitor refresh interval:
    if (windowRecord->VideoRefreshInterval > 0) {
        currentrefreshestimate = windowRecord->VideoRefreshInterval;
    }
    else {
        currentrefreshestimate=0;
        // We abort - This is too unsafe...
        PsychErrorExitMsg(PsychError_internal,"Flip called, while estimate of monitor refresh interval is INVALID -> Syncing trouble -> Aborting!");
    }
    
    // Setup reasonable slack-factor for deadline miss detector:
    if (windowRecord->VBL_Endline!=-1) {
        // If beam position queries work, we use a tight value:
        slackfactor = 1.05;
    }
    else {
        // If beam position queries don't work, we use a "slacky" value:
        slackfactor = 1.2;
    }
    
    // Retrieve display id and screen size spec that is needed later...
    PsychGetCGDisplayIDFromScreenNumber(&displayID, windowRecord->screenNumber);
    screenwidth=(int) PsychGetWidthFromRect(windowRecord->rect);
    screenheight=(int) PsychGetHeightFromRect(windowRecord->rect);
    // Query real size of the underlying display in order to define the vbl_startline:
    PsychGetScreenSize(windowRecord->screenNumber, &scw, &sch);
    vbl_startline = (int) sch;

    // Enable GL-Context of current onscreen window:
    PsychSetGLContext(windowRecord);

    // Enable this windowRecords framebuffer as current drawingtarget:
    PsychSetDrawingTarget(windowRecord);
    
    // Should we sync to the onset of vertical retrace?
    // Note: Flipping the front- and backbuffers is nearly always done in sync with VBL on
    // a double-buffered setup. sync_to_vbl specs, if the application should wait for
    // the VBL to start before continuing execution.
    sync_to_vbl = (vbl_synclevel == 0 || vbl_synclevel == 3) ? true : false;
    
    if (vbl_synclevel==2) {
        // We are requested to flip immediately, instead of syncing to VBL. Disable VBL-Sync.
	PsychOSSetVBLSyncLevel(windowRecord, 0);
    }
    
    if (multiflip > 0) {
        // Experimental Multiflip requested. Build list of all onscreen windows...
        PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);
    }
    
    if (multiflip == 2) {
        // Disable VBL-Sync for all onscreen windows except our primary one:
        for(i=0;i<numWindows;i++) {
            if (PsychIsOnscreenWindow(windowRecordArray[i]) && (windowRecordArray[i]!=windowRecord)) {
	      PsychOSSetVBLSyncLevel(windowRecordArray[i], 0);
            }
        }
    }
    
    // Backup current assignment of read- writebuffers:
    glGetIntegerv(GL_READ_BUFFER, &read_buffer);
    glGetIntegerv(GL_DRAW_BUFFER, &draw_buffer);
    
    // Perform preflip-operations: Backbuffer backups for the different dontclear-modes
    // and special compositing operations for specific stereo algorithms...
    PsychPreFlipOperations(windowRecord, dont_clear);
    
	// Special imaging mode active? in that case a FBO may be bound instead of the system framebuffer.
	if (windowRecord->imagingMode > 0) {
		// Reset our drawing engine: This will unbind any FBO's (i.e. reset to system framebuffer)
		// and reset the current target window to 'none'. We need this to be sure that our flip
		// sync pixel token is written to the real system backbuffer...
		PsychSetDrawingTarget(NULL);
	}
	
    // Part 1 of workaround- /checkcode for syncing to vertical retrace:
    if (vblsyncworkaround) {
        glDrawBuffer(GL_BACK);
        glRasterPos2f(0, screenheight);
        glDrawPixels(1,1,GL_RED,GL_UNSIGNED_BYTE, &id);
    }
 
    // Pausing until a specific deadline requested?
    if (flipwhen>0) {
        // We shall not swap at next VSYNC, but at the VSYNC immediately following the
        // system time "flipwhen". This is the premium version of the old WaitBlanking... ;-)
        
        // Calculate deadline for a successfull flip: If time_at_vbl is later than that,
        // it means that we missed the proper video refresh cycle:
        tshouldflip = flipwhen;
        
        // Some time left until deadline 'flipwhen'?
        PsychGetAdjustedPrecisionTimerSeconds(&tremaining);
        if ((flipwhen - tremaining) > 0) {
            // Child protection against people specifying a flipwhen that's infinite, e.g.,
            // via wrong ifi calculation in Matlab: if a flipwhen more than 1000 secs.
            // in the future is specified, we just assume this is an error...
            if (flipwhen - tremaining > 1000) {
                PsychErrorExitMsg(PsychError_user, "\nYou specified a 'when' value to Flip that's over 1000 seconds in the future?!? Aborting, assuming that's an error.\n\n");
            }
            
            // We force the rendering pipeline to finish all pending OpenGL operations,
            // so we can be sure that swapping at VBL will be as fast as possible.
            // Btw: glFlush() is not recommended by Apple, but in this specific case
            // it makes sense (MK). We avoid this redundant flush, if a pipeline flush has
            // been already done by other routines, e.g, SCREENDrawingFinished.
            if (!windowRecord->PipelineFlushDone) glFlush();
            
            // We'll sleep - and hope that the OS will wake us up in time, if the remaining waiting
            // time is more than 0 milliseconds. This way, we don't burn up valuable CPU cycles by
            // busy waiting and don't get punished by the overload detection of the OS:
            PsychWaitUntilSeconds(flipwhen);
        }
        // At this point, we are less than one video refresh interval away from the deadline - the next
        // VBL will be the one we want to flip at. Leave the rest of the job to CGLFlushDrawable...
    }
    else {
        // We should lock to next possible VSYNC:
        // Calculate deadline for a successfull flip on next VSYNC: If time_at_vbl is later than that,
        // it means that we missed the proper video refresh cycle:
        PsychGetAdjustedPrecisionTimerSeconds(&tshouldflip);        
    }

    // Do we know the exact system time when a VBL happened in the past?
    if ((windowRecord->time_at_last_vbl > 0) && (currentflipestimate > 0)) {
      // Yes! We use this as a base-line time to compute from the current time a virtual deadline,
      // which is at the beginning of the current monitor refresh interval.
      //
      // As long as we do synchronous Flips (sync_to_vbl == true - PTB blocks until VBL onset),
      // we should have a valid time_at_last_vbl, so this mechanism works.
      // Only on the *very first* invocation of Flip either after PTB-Startup or after a non-blocking
      // Flip, we can't do this because the time_at_last_vbl timestamp isn't available...
      tshouldflip = windowRecord->time_at_last_vbl + ((floor((tshouldflip - windowRecord->time_at_last_vbl) / currentflipestimate)) * currentflipestimate);
    }
    
    // Calculate final deadline for deadline-miss detection:
    tshouldflip = tshouldflip + slackfactor * currentflipestimate;        
    
    // Update of hardware gamma table in sync with flip requested?
    if ((windowRecord->inRedTable) && (windowRecord->loadGammaTableOnNextFlip > 0)) {
      // Yes! Call the update routine now. It should schedule the actual update for
      // the same VSYNC to which our bufferswap will lock. "Should" means, we have no
      // way of checking programmatically if it really worked, only via our normal deadline
      // miss detector. If we are running under M$-Windows then loading the hw-gamma table
      // will block our execution until next retrace. Then it will upload the new gamma table.
      // Therefore we need to disable sync of bufferswaps to VBL, otherwise we would swap only
      // one VBL after the table update -> out of sync by one monitor refresh!
      if (PSYCH_SYSTEM==PSYCH_WINDOWS) PsychOSSetVBLSyncLevel(windowRecord, 0);
      
      // We need to wait for render-completion here, otherwise hw-gamma table update and
      // bufferswap could get out of sync due to unfinished rendering commands which would
      // defer the bufferswap, but not the clut update.
      glFinish();
      
      // Perform hw-table upload on M$-Windows in sync with retrace, wait until completion. On
      // OS-X just schedule update in sync with next retrace, but continue immediately:
      PsychLoadNormalizedGammaTable(windowRecord->screenNumber, windowRecord->inTableSize, windowRecord->inRedTable, windowRecord->inGreenTable, windowRecord->inBlueTable);
    }

    #if PSYCH_SYSTEM == PSYCH_OSX
        // OS-X only: Low level queries to the driver:
        preflip_vbltimestamp = PsychOSGetVBLTimeAndCount(windowRecord->screenNumber, &preflip_vblcount);
    #endif
    
    // Trigger the "Front <-> Back buffer swap (flip) on next vertical retrace":
    PsychOSFlipWindowBuffers(windowRecord);
    
    // Multiflip with vbl-sync requested?
    if (multiflip==1) {
        //  Trigger the "Front <-> Back buffer swap (flip) on next vertical retrace"
        //  for all onscreen windows except our primary one:
        for(i=0;i<numWindows;i++) {
            if (PsychIsOnscreenWindow(windowRecordArray[i]) && (windowRecordArray[i]!=windowRecord)) {
                PsychOSFlipWindowBuffers(windowRecordArray[i]);
            }
        }
    }
    
    // Pause execution of application until start of VBL, if requested:
    if (sync_to_vbl) {
        if ((vbl_synclevel==3) && (windowRecord->VBL_Endline != -1)) {
            // Wait for VBL onset via experimental busy-waiting spinloop instead of
            // blocking: We spin-wait until the rasterbeam of our master-display enters the
            // VBL-Area of the display:
            while (vbl_startline > (int) CGDisplayBeamPosition(displayID));
        }
        else {
            // Standard blocking wait for flip/VBL onset requested:
            
            // Draw a single pixel in left-top area of back-buffer. This will wait/stall the rendering pipeline
            // until the buffer flip has happened, aka immediately after the VBL has started.
            // We need the pixel as "synchronization token", so the following glFinish() really
            // waits for VBL instead of just "falling through" due to the asynchronous nature of
            // OpenGL:
            glDrawBuffer(GL_BACK_LEFT);
            // We draw our single pixel with an alpha-value of zero - so effectively it doesn't
            // change the color buffer - just the z-buffer if z-writes are enabled...
            glColor4f(0,0,0,0);
            glBegin(GL_POINTS);
            glVertex2i(10,10);
            glEnd();
            // This glFinish() will wait until point drawing is finished, ergo backbuffer was ready
            // for drawing, ergo buffer swap in sync with start of VBL has happened.
            glFinish();
        }
        
        // At this point, start of VBL on masterdisplay has happened and we can continue execution...
        
        // Multiflip without vbl-sync requested?
        if (multiflip==2) {
            // Immediately flip all onscreen windows except our primary one:
            for(i=0;i<numWindows;i++) {
                if (PsychIsOnscreenWindow(windowRecordArray[i]) && (windowRecordArray[i]!=windowRecord)) {
                    PsychOSFlipWindowBuffers(windowRecordArray[i]);
                }
            }
        }
        
        // Query and return rasterbeam position immediately after Flip and before timestamp:
        *beamPosAtFlip=(int) CGDisplayBeamPosition(displayID);
         // We take a timestamp here and return it to "userspace"
        PsychGetAdjustedPrecisionTimerSeconds(&time_at_vbl);

        #if PSYCH_SYSTEM == PSYCH_OSX
        // Run kernel-level timestamping always in mode > 1 or on demand in mode 1 if beampos.
        // queries don't work properly:
        if (vbltimestampmode > 1 || (vbltimestampmode == 1 && windowRecord->VBL_Endline == -1)) {
            // OS-X only: Low level query to the driver: We need to yield the cpu for a couple of
            // microseconds, let's say 250 microsecs. for now, so the low-level vbl interrupt task
            // in IOKits workloop can do its job.
			vbltimestampquery_retrycount = 0;
            do {
                PsychWaitIntervalSeconds(0.00025);
                postflip_vbltimestamp = PsychOSGetVBLTimeAndCount(windowRecord->screenNumber, &postflip_vblcount);
				vbltimestampquery_retrycount++;
            } while ((preflip_vbltimestamp > 0) && (preflip_vbltimestamp == postflip_vbltimestamp) && (vbltimestampquery_retrycount < 10000));
			
			if (vbltimestampquery_retrycount>=10000) {
				// VBL irq queries broken! Disable them.
				printf("PTB-ERROR: VBL kernel-level timestamp queries broken on your setup! Please disable them via Screen('Preference', 'VBLTimestampingMode', 0);\n");
				printf("PTB-ERROR: until the problem is resolved. You may want to restart Matlab and retry.\n");
			}
        }
        #endif
        
        // Calculate estimate of real time of VBL, based on our post glFinish() timestamp, post glFinish() beam-
        // position and the roughly known height of image and duration of IFI. The corrected time_at_vbl
        // contains time at start of VBL. This value is crucial for control stimulus presentation timing.
        // We also estimate the end of VBL, aka the stimulus onset time in time_at_onset.
        
        // First we calculate the number of scanlines that have passed since start of VBL area:
        vbl_endline = windowRecord->VBL_Endline;
        vbl_lines_elapsed, onset_lines_togo;
        
        // VBL_Endline is determined in a calibration loop in PsychOpenOnscreenWindow above.
        // If this fails for some reason, we mark it as invalid by setting it to -1.
        if ((windowRecord->VBL_Endline != -1) && (vbltimestampmode>=0)) {
            if (*beamPosAtFlip >= vbl_startline) {
                vbl_lines_elapsed = *beamPosAtFlip - vbl_startline;
                onset_lines_togo = vbl_endline - (*beamPosAtFlip) + 1;
            }
            else {
                vbl_lines_elapsed = vbl_endline - vbl_startline + 1 + *beamPosAtFlip;
                onset_lines_togo = -1.0 * (*beamPosAtFlip);
            }
            
            // From the elapsed number we calculate the elapsed time since VBL start:
            vbl_time_elapsed = vbl_lines_elapsed / vbl_endline * currentrefreshestimate; 
            onset_time_togo = onset_lines_togo / vbl_endline * currentrefreshestimate;
            // Compute of stimulus-onset, aka time when retrace is finished:
            *time_at_onset = time_at_vbl + onset_time_togo;
            // Now we correct our time_at_vbl by this correction value:
            time_at_vbl = time_at_vbl - vbl_time_elapsed;
        }
        else {
            // Beamposition queries unavailable!
            
            // Shall we fall-back to kernel-level query?
            if (vbltimestampmode==1 && preflip_vbltimestamp > 0) {
                // Yes: Use fallback result:
                time_at_vbl = postflip_vbltimestamp;
            }
            
            // If we can't depend on timestamp correction, we just set time_at_onset == time_at_vbl.
            // This is not strictly correct, but at least the user doesn't have to change the whole
            // implementation of his code and we've warned him anyway at Window open time...
            *time_at_onset=time_at_vbl;
        }
                
        // OS level queries of timestamps supported and consistency check wanted?
        if (preflip_vbltimestamp > 0 && vbltimestampmode==2) {
            // Yes. Check both methods for consistency: We accept max. 1 ms deviation.
            if (fabs(postflip_vbltimestamp - time_at_vbl)>0.001) {
                printf("VBL timestamp deviation: precount=%i , postcount=%i, delta = %i, postflip_vbltimestamp = %lf  -  beampos_vbltimestamp = %lf  == Delta is = %lf \n",
                   (int) preflip_vblcount, (int) postflip_vblcount, (int) (postflip_vblcount - preflip_vblcount), postflip_vbltimestamp, time_at_vbl, postflip_vbltimestamp - time_at_vbl);

            }
        }
        
        // Shall kernel-level method override everything else?
        if (preflip_vbltimestamp > 0 && vbltimestampmode==3) {
            time_at_vbl = postflip_vbltimestamp;
            *time_at_onset=time_at_vbl;
        }
        
        // Check for missed / skipped frames: We exclude the very first "Flip" after
        // creation of the onscreen window from the check, as deadline-miss is expected
        // in that case:
        if ((time_at_vbl > tshouldflip) && (windowRecord->time_at_last_vbl!=0)) {
            // Deadline missed!
            windowRecord->nr_missed_deadlines = windowRecord->nr_missed_deadlines + 1;
        }
        
        // Return some estimate of how much we've missed our deadline (positive value) or
        // how much headroom was left (negative value):
        *miss_estimate = time_at_vbl - tshouldflip;
        
        // Update timestamp of last vbl:
        windowRecord->time_at_last_vbl = time_at_vbl;
    }
    else {
        // syncing to vbl is disabled, time_at_vbl becomes meaningless, so we set it to a
        // safe default of zero to indicate this.
        time_at_vbl = 0;
        *time_at_onset = 0;
        *beamPosAtFlip = -1;  // Ditto for beam position...
        
        // Invalidate timestamp of last vbl:
        windowRecord->time_at_last_vbl = 0;
    }
    
    // The remaining code will run asynchronously on the GPU again and prepares the back-buffer
    // for drawing of next stim.
    PsychPostFlipOperations(windowRecord, dont_clear);
        
    // Part 2 of workaround- /checkcode for syncing to vertical retrace:
    if (vblsyncworkaround) {
        glReadBuffer(GL_FRONT);
        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glReadPixels(0,0,1,1,GL_RED,GL_UNSIGNED_BYTE, &bufferstamp);
        if (bufferstamp!=id) {
            printf("%i -> %i  ", id, bufferstamp);
            glColor3b((GLint) bufferstamp, (GLint) 0,(GLint) 0);
            glBegin(GL_TRIANGLES);
            glVertex2d(20,20);
            glVertex2d(200,200);
            glVertex2d(20,200);
            glEnd();
        }

        id++;
    }

	// Special imaging mode active? in that case we need to restore drawing engine state to preflip state.
	if (windowRecord->imagingMode > 0) {
		PsychSetDrawingTarget(windowRecord);
	}

    // Restore assignments of read- and drawbuffers to pre-Flip state:
    glReadBuffer(read_buffer);
    glDrawBuffer(draw_buffer);

    // Reset flags used for avoiding redundant Pipeline flushes and backbuffer-backups:
    // This flags are altered and checked by SCREENDrawingFinished() and PsychPreFlipOperations() as well:
    windowRecord->PipelineFlushDone = false;
    windowRecord->backBufferBackupDone = false;
    
    // If we disabled (upon request) VBL syncing, we have to reenable it here:
    if (vbl_synclevel==2 || (windowRecord->inRedTable && (PSYCH_SYSTEM == PSYCH_WINDOWS))) {
      PsychOSSetVBLSyncLevel(windowRecord, 1);
    }
    
    // Was this an experimental Multiflip with "hard" busy flipping?
    if (multiflip==2) {
        // Reenable VBL-Sync for all onscreen windows except our primary one:
        for(i=0;i<numWindows;i++) {
            if (PsychIsOnscreenWindow(windowRecordArray[i]) && (windowRecordArray[i]!=windowRecord)) {
	      PsychOSSetVBLSyncLevel(windowRecordArray[i], 1);
            }
        }
    }
    
    if (multiflip>0) {
        // Cleanup our multiflip windowlist:
        PsychDestroyVolatileWindowRecordPointerList(windowRecordArray);
    }
    
	 // Cleanup temporary gamma tables if needed:
	 if ((windowRecord->inRedTable) && (windowRecord->loadGammaTableOnNextFlip > 0)) {
		free(windowRecord->inRedTable); windowRecord->inRedTable = NULL;
		free(windowRecord->inGreenTable); windowRecord->inGreenTable = NULL;
		free(windowRecord->inBlueTable); windowRecord->inBlueTable = NULL;
		windowRecord->inTableSize = 0;
		windowRecord->loadGammaTableOnNextFlip = 0;
	 }

    // We take a second timestamp here to mark the end of the Flip-routine and return it to "userspace"
    PsychGetAdjustedPrecisionTimerSeconds(time_at_flipend);
    
    // Done. Return high resolution system time in seconds when VBL happened.
    return(time_at_vbl);
}

/*
    PsychSetGLContext()
    
    Set the window to which GL drawing commands are sent.  
*/
void PsychSetGLContext(PsychWindowRecordType *windowRecord)
{
	// Child protection: Calling this function is only allowed in non-userspace rendering mode:
    if (PsychIsUserspaceRendering()) PsychErrorExitMsg(PsychError_user, "You tried to call a Screen graphics command after Screen('BeginOpenGL'), but without calling Screen('EndOpenGL') beforehand! Read the help for 'Screen EndOpenGL?'.");

	// Call OS - specific context switching code:
    PsychOSSetGLContext(windowRecord);
}

/*
    PsychClearGLContext()
    
    Clear the drawing context.  
*/
void PsychUnsetGLContext(void)
{
  PsychErrorExitMsg(PsychError_internal, "Ouuuucchhhh!!! PsychUnsetGLContext(void) called!!!!\n");    
}

/*
    PsychGetMonitorRefreshInterval() -- Monitor refresh calibration.
    
    When called with numSamples>0, will enter a calibration loop that measures until
    either numSamples valid samples have been taken *and* measured standard deviation
    is below some spec'd threshold, or some maximum time has elapsed (timeout).
    The refresh interval - intervalHint (if >0) is taken as a hint for the real refresh
    interval to stabilize measurements by detection/elimination of outliers.

    The estimate is stored in the windowRecord of the associated onscreen-window for
    use by different PTB-Routines, e.g., PsychFlipWindowBuffers(), it is also returned
    in the double-return argument.
 
    When called with numSamples=0, will return the refresh interval estimated by a
    previous calibration run of the routine.
 
    This routine will be called in PsychOpenOnscreenWindow for initial calibration,
    taking at least 50 samples and can be triggered by Matlab by calling the
    SCREENGetFlipInterval routine, if an experimenter needs a more accurate estimate...
*/
double PsychGetMonitorRefreshInterval(PsychWindowRecordType *windowRecord, int* numSamples, double* maxsecs, double* stddev, double intervalHint)
{
    int i;
    double told, tnew, tdur, tstart;
    double tstddev=10000.0f;
    double tavg=0;
    double tavgsq=0;
    double n=0;
    double reqstddev=*stddev;   // stddev contains the requested standard deviation.
    int fallthroughcount=0;
    
    // Child protection: We only work on double-buffered onscreen-windows...
    if (windowRecord->windowType != kPsychDoubleBufferOnscreen) {
        PsychErrorExitMsg(PsychError_InvalidWindowRecord, "Tried to query/measure monitor refresh interval on a window that's not double-buffered and on-screen.");
    }
    
    // Calibration run requested?
    if (*numSamples>0) {
        // Calibration run of 'numSamples' requested. Let's do it.
        
        // Switch to RT scheduling for timing tests:
        PsychRealtimePriority(true);

        // Wipe out old measurements:
        windowRecord->IFIRunningSum = 0;
        windowRecord->nrIFISamples = 0;

        // Setup context and back-drawbuffer:
        PsychSetGLContext(windowRecord);

        // Enable this windowRecords framebuffer as current drawingtarget:
        PsychSetDrawingTarget(windowRecord);

        glDrawBuffer(GL_BACK_LEFT);
        
        PsychGetAdjustedPrecisionTimerSeconds(&tnew);
        tstart = tnew;
        
        // Take samples during consecutive refresh intervals:
        // We measure until either:
        // - A maximum measurment time of maxsecs seconds has elapsed... (This is the emergency switch to prevent infinite loops).
        // - Or at least numSamples valid samples have been taken AND measured standard deviation is below the requested deviation stddev.
        for (i=0; (fallthroughcount<10) && ((tnew - tstart) < *maxsecs) && (n < *numSamples || ((n >= *numSamples) && (tstddev > reqstddev))); i++) {
            // Schedule a buffer-swap on next VBL:
	    PsychOSFlipWindowBuffers(windowRecord);
            
            // Wait for it, aka VBL start: See PsychFlipWindowBuffers for explanation...
            glBegin(GL_POINTS);
            glColor4f(0,0,0,0);
            glVertex2i(10,10);
            glEnd();
            
            // This glFinish() will wait until point drawing is finished, ergo backbuffer was ready
            // for drawing, ergo buffer swap in sync with start of VBL has happened.
            glFinish();
            
            // At this point, start of VBL has happened and we can continue execution...
            // We take our timestamp here:
            PsychGetAdjustedPrecisionTimerSeconds(&tnew);
            
            // We skip the first measurement, because we first need to establish an initial base-time 'told'
            if (i>0) {
                // Compute duration of this refresh interval in tnew:
                tdur = tnew - told;
                
                // This is a catch for complete sync-failure:
                // tdur < 0.004 can happen occasionally due to operating system scheduling jitter,
                // in this case tdur will be >> 1 monitor refresh for next iteration. Both samples
                // will be rejected, so they don't skew the estimate.
                // But if tdur < 0.004 for multiple consecutive frames, this indicates that
                // synchronization fails completely and we are just "falling through" glFinish().
                // A fallthroughcount>=10 will therefore abort the measurment-loop and invalidate
                // the whole result - indicating VBL sync trouble...
                // We need this additional check, because without it, we could get 1 valid sample with
                // approx. 10 ms ifi accidentally because the whole loop can stall for 10 ms on overload.
                // The 10 ms value is a valid value corresponding to 100 Hz refresh and by coincidence its
                // the "standard-timeslicing quantum" of the MacOS-X scheduler... ...Wonderful world of
                // operating system design and unintended side-effects for poor psychologists... ;-)
                fallthroughcount = (tdur < 0.004) ? fallthroughcount+1 : 0;

                // We accept the measurement as valid if either no intervalHint is available as reference or
                // we are in an interval between +/-20% of the hint.
                // We also check if interval corresponds to a measured refresh between 25 Hz and 250 Hz. Other
                // values are considered impossible and are therefore rejected...
                // If we are in OpenGL native stereo display mode, aka temporally interleaved flip-frame stereo,
                // then we also accept samples that are in a +/-20% rnage around twice the intervalHint. This is,
                // because in OpenGL stereo mode, ATI hardware doubles the flip-interval: It only flips every 2nd
                // video refresh, so a doubled flip interval is a legal valid result.
                if ((tdur >= 0.004 && tdur <= 0.040) && ((intervalHint<=0) || (intervalHint>0 &&
                    ( ((tdur > 0.8 * intervalHint) && (tdur < 1.2 * intervalHint)) ||
                      (((windowRecord->stereomode==kPsychOpenGLStereo) || (windowRecord->multiSample > 0)) && (tdur > 0.8 * 2 * intervalHint) && (tdur < 1.2 * 2 * intervalHint))
                    )))) {
                    // Valid measurement - Update our estimate:
                    windowRecord->IFIRunningSum = windowRecord->IFIRunningSum + tdur;
                    windowRecord->nrIFISamples = windowRecord->nrIFISamples + 1;

                    // Update our sliding mean and standard-deviation:
                    tavg = tavg + tdur;
                    tavgsq = tavgsq + (tdur * tdur);
                    n=windowRecord->nrIFISamples;
                    tstddev = (n>1) ? sqrt( ( tavgsq - ( tavg * tavg / n ) ) / (n-1) ) : 10000.0f;
                }
            }
            // Update reference timestamp:
            told = tnew;
        } // Next measurement loop iteration...
        
        // Switch back to old scheduling after timing tests:
        PsychRealtimePriority(false);
        
        // Ok, now we should have a pretty good estimate of IFI.
        if ( windowRecord->nrIFISamples <= 0 ) {
            printf("PTB-WARNING: Couldn't even collect one single valid flip interval sample! Sanity range checks failed!\n");
        }

        // Some additional check:
        if (fallthroughcount>=10) {
            // Complete sync failure! Invalidate all measurements:
            windowRecord->nrIFISamples = 0;
            n=0;
            tstddev=1000000.0;
            windowRecord->VideoRefreshInterval = 0;
            printf("PTB-WARNING: Couldn't collect valid flip interval samples! Fatal VBL sync failure!\n");
        }
        
        *numSamples = n;
        *stddev = tstddev;
    } // End of IFI measurement code.
    else {
        // No measurements taken...
        *numSamples = 0;
        *stddev = 0;
    }
    
    // Return the current estimate of flip interval & monitor refresh interval, if any...
    if (windowRecord->nrIFISamples > 0) {
        return(windowRecord->IFIRunningSum / windowRecord->nrIFISamples);
    }
    else {
        // Invalidate refresh on error.
        windowRecord->VideoRefreshInterval = 0;
        return(0);
    }
}

/*
    PsychVisualBell()
 
    Visual bell: Flashes the screen multiple times by changing background-color.
    This meant to be used when PTB detects some condition important for the user.
    The idea is to output some debug/warning messages to the Matlab command window,
    but as the user can't see them while the fullscreen stimulus window is open, we
    have to tell him/her somehow that his attention is recquired.
    This is mostly used in Screen('OpenWindow') of some tests fail or similar things...
 
    "duration" Defines duration in seconds.
    "belltype" Defines kind of info (Info = 0, Warning = 1, Error/Urgent = 2, 3 = Visual flicker test-sheet)
 
*/
void PsychVisualBell(PsychWindowRecordType *windowRecord, double duration, int belltype)
{
    double tdeadline, tcurrent, v=0;
    GLdouble color[4];
    int f=0;
    int scanline;
    CGDirectDisplayID cgDisplayID;
    float w,h;
    int visual_debuglevel;
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, windowRecord->screenNumber);    
    
    // Query current visual feedback level and abort, if it doesn't
    // allow requested type of visual feedback:
    visual_debuglevel = PsychPrefStateGet_VisualDebugLevel();    
    if (belltype == 0 && visual_debuglevel < 3) return;
    if (belltype == 1 && visual_debuglevel < 2) return;
    if (belltype == 2 && visual_debuglevel < 1) return;
    if (belltype == 3 && visual_debuglevel < 5) return;
    
    glGetDoublev(GL_COLOR_CLEAR_VALUE, (GLdouble*) &color);

    PsychGetAdjustedPrecisionTimerSeconds(&tdeadline);
    tdeadline+=duration;
    
    // Setup context:
    PsychSetGLContext(windowRecord);
    // Enable this windowRecords framebuffer as current drawingtarget:
    PsychSetDrawingTarget(windowRecord);

    w=PsychGetWidthFromRect(windowRecord->rect);
    h=PsychGetHeightFromRect(windowRecord->rect);
    
    // Clear out both buffers so it doesn't lool like junk:
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    PsychOSFlipWindowBuffers(windowRecord);
    glClear(GL_COLOR_BUFFER_BIT);
    PsychOSFlipWindowBuffers(windowRecord);
    
    if (belltype==3) {
        // Test-Sheet mode: Need smaller warning triangle...
        w=w/3;
        h=h/3;
    }
    
    do {
        // Take timestamp for abort-check and driving animation:
        PsychGetAdjustedPrecisionTimerSeconds(&tcurrent);

        // Calc our visual ;-)
        v=0.5 + 0.5 * sin(tcurrent*6.283);
        
        switch (belltype) {
            case 0: // Info - Make it blue
                glClearColor(0,0,v,0);
                break;
            case 1: // Warning - Make it yellow
                glClearColor(v,v,0,0);
                break;
            case 2: // Error - Make it red
                glClearColor(v,0,0,0);
            break;
            case 3: // Test-Sheet - Don't clear...
                // Draw some flickering area (alternating black-white flicker)
                f=1-f;
                glBegin(GL_QUADS);
                glColor3f(f,f,f);
                glVertex2f(0.00*w, 0.00*h);
                glVertex2f(2.00*w, 0.00*h);
                glVertex2f(2.00*w, 3.00*h);
                glVertex2f(0.00*w, 3.00*h);
                glColor3f(0,0,v);
                glVertex2f(0.00*w, 0.00*h);
                glVertex2f(1.00*w, 0.00*h);
                glVertex2f(1.00*w, 1.00*h);
                glVertex2f(0.00*w, 1.00*h);
                glEnd();
            break;
        }
        
        if (belltype!=3) glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw a yellow triangle with black border:
        glColor3f(0,0,0);
        glBegin(GL_TRIANGLES);
        glVertex2f(0.1*w, 0.1*h);
        glVertex2f(0.9*w, 0.1*h);
        glVertex2f(0.5*w, 0.9*h);
        glColor3f(1,1,0);
        glVertex2f(0.2*w, 0.2*h);
        glVertex2f(0.8*w, 0.2*h);
        glVertex2f(0.5*w, 0.8*h);
        glEnd();
        // Draw a black exclamation mark into triangle:
        glBegin(GL_QUADS);
        glColor3f(0,0,0);
        glVertex2f(0.47*w, 0.23*h);
        glVertex2f(0.53*w, 0.23*h);
        glVertex2f(0.53*w, 0.55*h);
        glVertex2f(0.47*w, 0.55*h);
        glVertex2f(0.47*w, 0.60*h);
        glVertex2f(0.53*w, 0.60*h);
        glVertex2f(0.53*w, 0.70*h);
        glVertex2f(0.47*w, 0.70*h);
        glEnd();
        
        // Initiate back-front buffer flip:
	PsychOSFlipWindowBuffers(windowRecord);
        
        // Our old VBL-Sync trick again... We need sync to VBL to visually check if
        // beamposition is locked to VBL:
        // We draw our single pixel with an alpha-value of zero - so effectively it doesn't
        // change the color buffer - just the z-buffer if z-writes are enabled...
        glColor4f(0,0,0,0);
        glBegin(GL_POINTS);
        glVertex2i(10,10);
        glEnd();        
        // This glFinish() will wait until point drawing is finished, ergo backbuffer was ready
        // for drawing, ergo buffer swap in sync with start of VBL has happened.
        glFinish();

        // Query and visualize scanline immediately after VBL onset, aka return of glFinish();
        scanline=(int) CGDisplayBeamPosition(cgDisplayID);    
        if (belltype==3) {
            glColor3f(1,1,0);
            glBegin(GL_LINES);
            glVertex2f(2*w, scanline);
            glVertex2f(3*w, scanline);
            glEnd();
        }
    } while (tcurrent < tdeadline);

    // Restore clear color:
    glClearColor(color[0], color[1], color[2], color[3]);

    return;
}

/*
 * PsychPreFlipOperations()  -- Prepare windows backbuffer for flip.
 *
 * This routine performs all preparatory work to bring the windows backbuffer in its
 * final state for bufferswap as soon as possible.
 *
 * If a special stereo display mode is active, it performs all necessary drawing/setup/
 * compositing operations to assemble the final stereo display from the content of diverse
 * stereo backbuffers/AUX buffers/stereo metadata and such.
 *
 * If clearmode = Don't clear after flip is selected, the necessary backup copy of the
 * backbuffers into AUX buffers is made, so backbuffer can be restored to previous state
 * after Flip.
 *
 * This routine is called automatically by PsychFlipWindowBuffers on Screen('Flip') time as
 * well as by Screen('DrawingFinished') for manually triggered preflip work.
 *
 * -> Unifies the code in Flip and DrawingFinished.
 *
 */
void PsychPreFlipOperations(PsychWindowRecordType *windowRecord, int clearmode)
{
    int screenwidth=(int) PsychGetWidthFromRect(windowRecord->rect);
    int screenheight=(int) PsychGetHeightFromRect(windowRecord->rect);
    int stereo_mode=windowRecord->stereomode;
	int imagingMode = windowRecord->imagingMode;
	int viewid, hookchainid;
	GLint read_buffer, draw_buffer, blending_on;
    GLint auxbuffers;

    // Early reject: If this flag is set, then there's no need for any processing:
    // We only continue processing textures, aka offscreen windows...
    if (windowRecord->windowType!=kPsychTexture && windowRecord->backBufferBackupDone) return;

    // Switch to associated GL-Context of windowRecord:
    PsychSetGLContext(windowRecord);

    // Enable this windowRecords framebuffer as current drawingtarget:
    PsychSetDrawingTarget(windowRecord);
    
    // We stop processing here if window is a texture, aka offscreen window...
    if (windowRecord->windowType==kPsychTexture) return;
    
    // Reset viewport to full-screen default:
    glViewport(0, 0, screenwidth, screenheight);
    glScissor(0, 0, screenwidth, screenheight);
    
    // Reset color buffer writemask to "All enabled":
    glColorMask(TRUE, TRUE, TRUE, TRUE);

    // Query number of available AUX-buffers:
    glGetIntegerv(GL_AUX_BUFFERS, &auxbuffers);

    // Set transform matrix to well-defined state:
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
	// The following code is for traditional non-imaging rendering:
	if (imagingMode == 0) {
		// Check for compressed stereo handling...
		if (stereo_mode==kPsychCompressedTLBRStereo || stereo_mode==kPsychCompressedTRBLStereo) {
			if (auxbuffers<2) {
				PsychErrorExitMsg(PsychError_user, "OpenGL AUX buffers unavailable! The requested stereo mode doesn't work without them.\n"
								  "Either unsupported by your graphics card, or you disabled them via call to Screen('Preference', 'ConserveVRAM')?");
			}
			
			// Compressed stereo mode active. Compositing already done?
			// Backup backbuffer -> AUX buffer: We trigger this via transition to "no buffer":
			PsychSwitchCompressedStereoDrawBuffer(windowRecord, 2);
			
			// Ok, now both AUX buffers contain the final stereo content: Compose them into
			// back-buffer:
			PsychComposeCompressedStereoBuffer(windowRecord);
		}
		// Non-compressed stereo case: Mono or other stereo alg. Normal treatment applies...
		// Check if we should do the backbuffer -> AUX buffer backup, because we use
		// clearmode 1 aka "Don't clear after flip, but retain backbuffer content"
		else if (clearmode==1 && windowRecord->windowType==kPsychDoubleBufferOnscreen) {
			// Backup current assignment of read- writebuffers:
			glGetIntegerv(GL_READ_BUFFER, &read_buffer);
			glGetIntegerv(GL_DRAW_BUFFER, &draw_buffer);
			blending_on = (int) glIsEnabled(GL_BLEND);
			glDisable(GL_BLEND);
			
			// Is this window equipped with a native OpenGL stereo rendering context?
			// If so, then we need to backup both backbuffers (left-eye and right-eye),
			// instead of only the monoscopic one.
			if (stereo_mode==kPsychOpenGLStereo) {
				if (auxbuffers<2) {
					PsychErrorExitMsg(PsychError_user, "OpenGL AUX buffers unavailable! dontclear=1 in Screen-Flip doesn't work without them.\n"
									  "Either unsupported by your graphics card, or you disabled them via call to Screen('Preference', 'ConserveVRAM')?");
				}
				
				glDrawBuffer(GL_AUX0);
				glReadBuffer(GL_BACK_LEFT);
				glRasterPos2i(0, screenheight);
				glCopyPixels(0, 0, screenwidth, screenheight, GL_COLOR);            
				glDrawBuffer(GL_AUX1);
				glReadBuffer(GL_BACK_RIGHT);
				glRasterPos2i(0, screenheight);
				glCopyPixels(0, 0, screenwidth, screenheight, GL_COLOR);            
			}
			else {
				if (auxbuffers<1) {
					PsychErrorExitMsg(PsychError_user, "OpenGL AUX buffers unavailable! dontclear=1 in Screen-Flip doesn't work without them.\n"
									  "Either unsupported by your graphics card, or you disabled them via call to Screen('Preference', 'ConserveVRAM')?");
				}
				glDrawBuffer(GL_AUX0);
				glReadBuffer(GL_BACK);
				glRasterPos2i(0, screenheight);
				glCopyPixels(0, 0, screenwidth, screenheight, GL_COLOR);            
			}
			
			if (blending_on) glEnable(GL_BLEND);
			
			// Restore assignment of read- writebuffers:
			glReadBuffer(read_buffer);
			glDrawBuffer(draw_buffer);        
		}

		// Check if the finalizer blit chain is operational. This is the only blit chain available for preflip operations in non-imaging mode,
		// useful for encoding special information into final framebuffer images, e.g., sync lines, time stamps, cluts...
		// All other blit chains are only available in imaging mode - they need support for shaders and framebuffer objects...
		if (PsychIsHookChainOperational(windowRecord, kPsychLeftFinalizerBlit) || PsychIsHookChainOperational(windowRecord, kPsychRightFinalizerBlit)) {
			glGetIntegerv(GL_READ_BUFFER, &read_buffer);
			glGetIntegerv(GL_DRAW_BUFFER, &draw_buffer);
			blending_on = (int) glIsEnabled(GL_BLEND);
			glDisable(GL_BLEND);
			
			// Process each of the (up to two) streams:
			for (viewid = 0; viewid < ((stereo_mode == kPsychOpenGLStereo) ? 2 : 1); viewid++) {
				
				// Select drawbuffer:
				if (stereo_mode == kPsychOpenGLStereo) {
					// Quad buffered stereo: Select proper backbuffer:
					glDrawBuffer((viewid==0) ? GL_BACK_LEFT : GL_BACK_RIGHT);
				} else {
					// Mono mode: Select backbuffer:
					glDrawBuffer(GL_BACK);
				}
				
				// This special purpose blit chains can be used to encode low-level information about frames into
				// the frames or do other limited per-frame processing. Their main use (as of now) is to draw
				// the blue-line sync signal into quad-buffered windows in quad-buffered stereo mode. One could
				// use them e.g., to encode a frame index, a timestamp or a trigger signal into frames as well.
				// Encoding CLUTs for devices like the Bits++ is conceivable as well - these would be automatically
				// synchronous to frame updates and could be injected from our own gamma-table functions.
				PsychPipelineExecuteHook(windowRecord, (viewid==0) ? kPsychLeftFinalizerBlit : kPsychRightFinalizerBlit, NULL, NULL, TRUE, FALSE, NULL, NULL, NULL, NULL);				
			}
			
			// Restore blending mode:
			if (blending_on) glEnable(GL_BLEND);
			
			// Restore assignment of read- writebuffers:
			glReadBuffer(read_buffer);
			glDrawBuffer(draw_buffer);
		}		        
	}	// End of traditional preflip path.
	
	if (imagingMode) {
		// Preflip operations for imaging mode:

		// Detach any active drawing targets:
		PsychSetDrawingTarget(NULL);

		// Reset modelview matrix to identity:
		glLoadIdentity();

		// Save all state:
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		
		// Disable alpha-blending:
		glDisable(GL_BLEND);
		
		// Execute post processing sequence for this onscreen window:
		
		// Generic image processing on viewchannels enabled?
		if (imagingMode & kPsychNeedImageProcessing) {
			// Yes. Process each of the (up to two) streams:
			for (viewid = 0; viewid < ((stereo_mode > 0) ? 2 : 1); viewid++) {
				// Processing chain enabled and non-empty?
				hookchainid = (viewid==0) ? kPsychStereoLeftCompositingBlit : kPsychStereoRightCompositingBlit;
				if (PsychIsHookChainOperational(windowRecord, hookchainid)) {
					// Hook chain ready to do its job: Execute it.      userd,blitf
					// Don't supply user-specific data, blitfunction is default blitter, unless defined otherwise in blitchain,
					// srcfbos are read-only, swizzling forbidden, 2nd srcfbo doesn't exist (only needed for stereo merge op),
					// We provide a bounce-buffer... We could bind the 2nd channel in steromode if we wanted. Should we?
					// TODO: Define special userdata struct, e.g., for C-Callbacks or scripting callbacks?
					PsychPipelineExecuteHook(windowRecord, hookchainid, NULL, NULL, TRUE, FALSE, &(windowRecord->fboTable[windowRecord->drawBufferFBO[viewid]]), NULL, &(windowRecord->fboTable[windowRecord->processedDrawBufferFBO[viewid]]),  (windowRecord->processedDrawBufferFBO[2]>=0) ? &(windowRecord->fboTable[windowRecord->processedDrawBufferFBO[2]]) : NULL);
				}
				else {
					// Hook chain disabled by userspace or doesn't contain any instructions.
					// Execute our special identity blit chain to transfer the data from source buffer
					// to destination buffer:
					PsychPipelineExecuteHook(windowRecord, kPsychIdentityBlit, NULL, NULL, TRUE, FALSE, &(windowRecord->fboTable[windowRecord->drawBufferFBO[viewid]]), NULL, &(windowRecord->fboTable[windowRecord->processedDrawBufferFBO[viewid]]), NULL);
				}
			}
		}
		
		// At this point, processedDrawBufferFBO[0 and 1] contain the per-viewchannel result of
		// user defined (or stereo) image processing.
		
		// Stereo processing: This depends on selected stereomode...
		if (stereo_mode <= kPsychOpenGLStereo) {
			// No stereo or quad-buffered stereo - Nothing to do in merge stage.
		}
		else if (stereo_mode <= kPsychAnaglyphBRStereo) {
			// Merged stereo - All work is done by the anaglyph shader that was created for this purpose
			// in pipeline setup, no geometric transform or such are needed, so we can use the default blitter:
			if (PsychIsHookChainOperational(windowRecord, kPsychStereoCompositingBlit)) {
				// Don't supply user-specific data, blitfunction is default blitter, unless defined otherwise in blitchain,
				// srcfbos are read-only, swizzling forbidden, 2nd srcfbo is right-eye channel, whereas 1st srcfbo is left-eye channel.
				// We provide a bounce-buffer as well.
				// TODO: Define special userdata struct, e.g., for C-Callbacks or scripting callbacks?
				PsychPipelineExecuteHook(windowRecord, kPsychStereoCompositingBlit, NULL, NULL, TRUE, FALSE, &(windowRecord->fboTable[windowRecord->processedDrawBufferFBO[0]]), &(windowRecord->fboTable[windowRecord->processedDrawBufferFBO[1]]), &(windowRecord->fboTable[windowRecord->preConversionFBO[0]]), (windowRecord->preConversionFBO[2]>=0) ? &(windowRecord->fboTable[windowRecord->preConversionFBO[2]]) : NULL);
			}
			else {
				// Hook chain disabled by userspace or doesn't contain any instructions.
				// We vitally need the compositing chain, there's no simple fallback here!
				PsychErrorExitMsg(PsychError_internal, "Processing chain for stereo processing merge operations is needed, but empty or disabled - No visual output produced! Bug?!?\n");
			}			
		}
		else {
			// Invalid stereo mode?
			PsychErrorExitMsg(PsychError_internal, "Invalid stereo mode encountered!?!");
		}
		
		// At this point we have image data ready for final post-processing and special device output formatting...
		// In mono mode: Image in preConversionFBO[0].
		// In quad-buffered stereo mode: Left eye image in preConversionFBO[0], Right eye image in preConversionFBO[1].
		// In other stereo modes: Merged image in both preConversionFBO[0] and preConversionFBO[1], both reference the same image buffer.
		
		// Ready to create the final content, either for drawing into a snapshot buffer or into the real system framebuffer.
		// finalizedFBO[0] is set up to take the final image for anything but quad-buffered stereo.
		// In quad-buffered mode, finalizedFBO[0] shall receive the left-eye image, finalizedFBO[1] shall receive the right-eye image.
		// Each FBO is either a real FBO for framebuffer "screenshots" or the system framebuffer for final output into the backbuffer.

		// Process each of the (up to two) streams:
		for (viewid = 0; viewid < ((stereo_mode == kPsychOpenGLStereo) ? 2 : 1); viewid++) {

			// Select final drawbuffer if our target is the system framebuffer:
			if (windowRecord->fboTable[windowRecord->finalizedFBO[viewid]]->fboid == 0) {
				// Final target is system backbuffer:
				if (stereo_mode == kPsychOpenGLStereo) {
					// Quad buffered stereo: Select proper backbuffer:
					glDrawBuffer((viewid==0) ? GL_BACK_LEFT : GL_BACK_RIGHT);
				} else {
					// Mono mode: Select backbuffer:
					glDrawBuffer(GL_BACK);
				}
			}

			// Output conversion needed, processing chain enabled and non-empty?
			if ((imagingMode & kPsychNeedOutputConversion) && PsychIsHookChainOperational(windowRecord, kPsychFinalOutputFormattingBlit)) {
				// Yes - Execute it:
				PsychPipelineExecuteHook(windowRecord, kPsychFinalOutputFormattingBlit, NULL, NULL, TRUE, FALSE, &(windowRecord->fboTable[windowRecord->preConversionFBO[viewid]]), NULL, &(windowRecord->fboTable[windowRecord->finalizedFBO[viewid]]), (windowRecord->preConversionFBO[2]>=0) ? &(windowRecord->fboTable[windowRecord->preConversionFBO[2]]) : NULL);
			}
			else {
				// No conversion needed or chain disabled: Do our identity blit, but only if really needed!
				// This gets skipped in mono-mode if no conversion needed and only single-pass image processing
				// applied. In that case, the image processing state did the final blit already.
				if (windowRecord->preConversionFBO[viewid] != windowRecord->finalizedFBO[viewid]) {
					if ((imagingMode & kPsychNeedOutputConversion) && (PsychPrefStateGet_Verbosity()>3)) printf("PTB-INFO: Processing chain for output conversion disabled -- Using identity copy as workaround.\n");
					PsychPipelineExecuteHook(windowRecord, kPsychIdentityBlit, NULL, NULL, TRUE, FALSE, &(windowRecord->fboTable[windowRecord->preConversionFBO[viewid]]), NULL, &(windowRecord->fboTable[windowRecord->finalizedFBO[viewid]]), NULL);				
				}
			}
			
			// This special purpose blit chains can be used to encode low-level information about frames into
			// the frames or do other limited per-frame processing. Their main use (as of now) is to draw
			// the blue-line sync signal into quad-buffered windows in quad-buffered stereo mode. One could
			// use them e.g., to encode a frame index, a timestamp or a trigger signal into frames as well.
			// Encoding CLUTs for devices like the Bits++ is conceivable as well - these would be automatically
			// synchronous to frame updates and could be injected from our own gamma-table functions.
			PsychPipelineExecuteHook(windowRecord, (viewid==0) ? kPsychLeftFinalizerBlit : kPsychRightFinalizerBlit, NULL, NULL, TRUE, FALSE, NULL, NULL, &(windowRecord->fboTable[windowRecord->finalizedFBO[viewid]]), NULL);				
		}
		
		// At this point we should have either a valid snapshot of the framebuffer in the finalizedFBOs, or
		// (the common case) the final image in the system backbuffers, ready for display after swap.
		
		// Disabled debug code:
		if (FALSE) {
			windowRecord->textureNumber = windowRecord->fboTable[windowRecord->drawBufferFBO[0]]->coltexid;
			
			// Now we need to blit the new rendertargets texture into the framebuffer. We need to make
			// sure that alpha-blending is disabled during this blit operation:
			// Alpha blending not enabled. Just blit it:
			PsychBlitTextureToDisplay(windowRecord, windowRecord, windowRecord->rect, windowRecord->rect, 0, 0, 1);
			
			windowRecord->textureNumber = 0;
		}

		// Restore all state, including blending and texturing state:
		glPopAttrib();
		
	}	// End of preflip operations for imaging mode:

	// EXPERIMENTAL: Execute hook chain for final backbuffer data formatting after stereo composition and post processing:
	// PsychPipelineExecuteHook(windowRecord, kPsychFinalOutputFormattingBlit, NULL, NULL, FALSE, FALSE, NULL, NULL, NULL, NULL);

    // Restore modelview matrix:
    glPopMatrix();
    
    // Tell Flip that backbuffer backup has been done already to avoid redundant backups. This is a bit of a
	// unlucky name. It actually signals that all the preflip processing has been done, the old name is historical.
    windowRecord->backBufferBackupDone = true;

    return;
}

/*
 * PsychPostFlipOperations()  -- Prepare windows backbuffer after flip.
 *
 * This routine performs all preparatory work to bring the windows backbuffer in its
 * proper state for drawing the next stimulus after bufferswap has completed.
 *
 * If a special stereo display mode is active, it performs all necessary setup/
 * operations to restore the content of diverse stereo backbuffers/AUX buffers/stereo
 * metadata and such.
 *
 * If clearmode = Don't clear after flip is selected, the backbuffer is restored to previous state
 * after Flip from the AUX buffer copies.
 *
 * This routine is called automatically by PsychFlipWindowBuffers on Screen('Flip') time after
 * the flip has happened.
 *
 * -> Unifies the code in Flip and DrawingFinished.
 *
 */
void PsychPostFlipOperations(PsychWindowRecordType *windowRecord, int clearmode)
{
    int screenwidth=(int) PsychGetWidthFromRect(windowRecord->rect);
    int screenheight=(int) PsychGetHeightFromRect(windowRecord->rect);
    int stereo_mode=windowRecord->stereomode;

    // Switch to associated GL-Context of windowRecord:
    PsychSetGLContext(windowRecord);

	// Imaging pipeline off?
	if (windowRecord->imagingMode==0) {
		// Imaging pipeline disabled: This is the old-style way of doing things:
		
		// Set transform matrix to well-defined state:
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		
		// Vertical compression stereo active? This needs special treatment...
		if (stereo_mode==kPsychCompressedTLBRStereo || stereo_mode==kPsychCompressedTRBLStereo) {
			// Yes. We reset the active stereo buffer to 2 == none selected.
			windowRecord->stereodrawbuffer=2;
			// In clearmode==1, aka retain we don't do anything. This way the AUX buffers
			// restore the preflip state automatically. clearmode=2 is undefined by definition ;-)
			if (clearmode==0) {
				// clearmode 0 active. Sterobuffers shall be cleared on flip. We just
				// reset the dirty-flags of the AUX buffers, so backbuffer gets cleared
				// on first use after selection of a new stereo draw buffer:
				windowRecord->auxbuffer_dirty[0]=FALSE;
				windowRecord->auxbuffer_dirty[1]=FALSE;
			}
		}
		// In other stereo modes and mono mode, we don't need to play backbuffer-AUX buffer games,
		// just treat'em as in mono case...
		else if (clearmode!=2) {
			// Reinitialization of back buffer for drawing of next stim requested:
			if (clearmode==1) {
				// We shall not clear the back buffer(s), but restore them to state before "Flip",
				// so previous stim can be incrementally updated where this makes sense.
				// Copy back our backup-copy from AUX buffers:
				glDisable(GL_BLEND);
				
				// Need to do it on both backbuffers when OpenGL native stereo is enabled:
				if (stereo_mode==kPsychOpenGLStereo) {
					glDrawBuffer(GL_BACK_LEFT);
					glReadBuffer(GL_AUX0);
					glRasterPos2i(0, screenheight);
					glCopyPixels(0, 0, screenwidth, screenheight, GL_COLOR);
					glDrawBuffer(GL_BACK_RIGHT);
					glReadBuffer(GL_AUX1);
					glRasterPos2i(0, screenheight);
					glCopyPixels(0, 0, screenwidth, screenheight, GL_COLOR);
				}
				else {
					glDrawBuffer(GL_BACK);
					glReadBuffer(GL_AUX0);
					glRasterPos2i(0, screenheight);
					glCopyPixels(0, 0, screenwidth, screenheight, GL_COLOR);
				}
				
				glEnable(GL_BLEND);
			}
			else {
				// Clearing (both)  back buffer requested:
				if (stereo_mode==kPsychOpenGLStereo) {
					glDrawBuffer(GL_BACK_LEFT);
					glClear(GL_COLOR_BUFFER_BIT);
					glDrawBuffer(GL_BACK_RIGHT);
					glClear(GL_COLOR_BUFFER_BIT);
				}
				else {
					glDrawBuffer(GL_BACK);
					glClear(GL_COLOR_BUFFER_BIT);
				}
			}
		}
		
		// Restore modelview matrix:
		glPopMatrix();
	} // End of traditional postflip implementation for non-imaging mode:
	
	// Imaging pipeline enabled?
    if (windowRecord->imagingMode > 0) {
		// Yes. This is rather simple. In dontclear=2 mode we do nothing, except reenable
		// the windowRecord as drawing target again. In dontclear=1 mode ditto, because
		// our backing store FBO's already retained a backup of the preflip-framebuffer.
		// Only in dontclear = 0 mode, we need to clear the backing FBO's:
		
		if (clearmode==0) {
			// Select proper viewport and cliprectangles for clearing:
			PsychSetupView(windowRecord);
			
			// Bind left view (or mono view) buffer:
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, windowRecord->fboTable[windowRecord->drawBufferFBO[0]]->fboid);
			// and clear it:
			glClear(GL_COLOR_BUFFER_BIT);
			
			if (windowRecord->stereomode > 0) {
				// Bind right view buffer for stereo mode:
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, windowRecord->fboTable[windowRecord->drawBufferFBO[1]]->fboid);
				// and clear it:
				glClear(GL_COLOR_BUFFER_BIT);
			}
		}
		
		// Select proper rendertarget for further drawing ops - restore preflip state:
		PsychSetDrawingTarget(windowRecord);
	}

	if (glGetError() == GL_OUT_OF_MEMORY) {
		// Special case: Out of memory after Flip + Postflip operations.
		printf("PTB-Error: The OpenGL graphics hardware encountered an out of memory condition!\n");
		printf("PTB-Error: One cause of this could be that you are running your display at a too\n");
		printf("PTB-Error: high resolution and/or use Anti-Aliasing with a multiSample value that\n");
		printf("PTB-Error: your gfx-card can't handle at the current display resolution. If this is\n");
		printf("PTB-Error: the case, you may have to reduce multiSample level or display resolution.\n");
		printf("PTB-Error: It may help to quit and restart Matlab or Octave before continuing.\n");
	 }

    PsychTestForGLErrors();
    
	// EXPERIMENTAL: Execute hook chain for preparation of user space drawing ops:
	PsychPipelineExecuteHook(windowRecord, kPsychUserspaceBufferDrawingPrepare, NULL, NULL, FALSE, FALSE, NULL, NULL, NULL, NULL);

    // Done.
    return;
}

PsychWindowRecordType* PsychGetDrawingTarget(void)
{
	return(currentRendertarget);
}

/* PsychSetDrawingTarget - Set the target window for following drawing ops.
 *
 * Set up 'windowRecord' as the target window for all drawing operations.
 * If windowRecord corresponds to an onscreen window, the standard framebuffer is
 * selected as drawing target. If 'windowRecord' corresponds to a Psychtoolbox
 * texture (aka Offscreen Window), we bind the texture as OpenGL framebuffer object,
 * so we have render-to-texture functionality.
 *
 * This routine only performs state-transitions if necessary to save expensive
 * state switches. Calling the routine with a NULL-Ptr doesn't change the rendertarget
 * but signals render-completion.
 *
 * This routine requires support for OpenGL Framebuffer objects, aka OpenGL 1.5 with
 * extension or OpenGL 2 or later. As a consequence it requires OS-X 10.4.3 or later or
 * a Windows system with proper support. It is a no-op on all other systems.
 *
 */
void PsychSetDrawingTarget(PsychWindowRecordType *windowRecord)
{
    static GLuint framebufferobject = 0;
    static Boolean fbo_workaround_needed = FALSE;
    static GLuint framebufferobject2 = 0;
    static GLuint renderbuffer = 0;
    static unsigned int recursionLevel = 0;
    static int use_framebuffer_objects = -1;
    int texid, twidth, theight;
    Boolean EmulateOldPTB = PsychPrefStateGet_EmulateOldPTB();
    
    // Increase recursion level count:
    recursionLevel++;
    
    // Is this a recursive call?
    if (recursionLevel>1) {
        // Yep. Then we just do nothing:
        recursionLevel--;
        return;
    }
    
	// windowRecord or NULL provided? NULL would mean a warm-start:
    if (windowRecord) {
        // State transition requested?
        if (currentRendertarget != windowRecord) {
            // Need to do a switch between drawing target windows:

			// Check if the imaging pipeline is enabled for this window. If so, we will use
			// the fast FBO based rendertarget implementation:
            if (windowRecord->imagingMode & kPsychNeedFastBackingStore) {
                // Imaging pipeline active for this window. Use OpenGL framebuffer objects: This is the fast-path!

                // Transition to offscreen rendertarget?
                if (windowRecord->windowType == kPsychTexture) {
                    // Yes. Need to bind the texture as framebuffer object. This only works for rectangle textures.
					if (PsychGetTextureTarget(windowRecord)!=GL_TEXTURE_RECTANGLE_EXT) {
						PsychErrorExitMsg(PsychError_user, "You tried to draw into a special power-of-two offscreen window or texture. Sorry, this is not supported.");
					}
					
					// It also only works on RGB or RGBA textures, not Luminance or LA textures, and the texture needs to be upright.
					// PsychNormalizeTextureOrientation takes care of swapping it upright and converting it into a RGB or RGBA format,
					// if needed. Only if it were an upright non-RGB(A) texture, it would slip through this and trigger an error abort
					// in the following PsychCreateShadowFBO... call. This however can't happen with textures created by 'OpenOffscreenWindow',
					// textures from the Quicktime movie engine, the videocapture engine or other internal sources. Textures created via
					// MakeTexture will be auto-converted as well, unless some special flags to MakeTexture are given.
					// --> The user code needs to do something very unusual and special to trigger an error abort here, and if it triggers
					// one, it will abort with a helpful error message, telling how to fix the problem very simply.
					PsychNormalizeTextureOrientation(windowRecord);
					
					// Do we already have a framebuffer object for this texture? All textures start off without one,
					// because most textures are just used for drawing them, not drawing *into* them. Therefore we
					// only create a full blown FBO on demand here.
					PsychCreateShadowFBOForTexture(windowRecord, TRUE, -1);

					// Switch to FBO for given texture or offscreen window:
					glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, windowRecord->fboTable[0]->fboid);

				} // Special setup for offscreen windows or textures finished.
				else {
					// Bind onscreen window as drawing target:
					// We either bind the drawBufferFBO for the left channel or right channel, depending
					// on stereo mode and selected stereo buffer:
					if ((windowRecord->stereomode > 0) && (windowRecord->stereodrawbuffer == 1)) {
						// We are in stereo mode and want to draw into the right-eye channel. Bind FBO-1
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, windowRecord->fboTable[windowRecord->drawBufferFBO[1]]->fboid);
					}
					else {
						// We are either in stereo mode and want to draw into left-eye channel or we are
						// in mono mode. Bind FBO-0:
						glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, windowRecord->fboTable[windowRecord->drawBufferFBO[0]]->fboid);
					}
				}

				// Fast path for rendertarget switch finished.
            }	// End of fast-path: FBO based processing...
            else {
                // Use standard OpenGL without framebuffer objects for drawing target switch:
                // This code path is executed when the imaging pipeline is disabled. It only uses
				// OpenGL 1.1 functionality so it should work on any piece of gfx-hardware:
				
                // Whatever is bound at the moment needs to be backed-up into a texture...
                // If currentRendertarget is NULL then we've got nothing to back up.
                if (currentRendertarget) {
                    // There is a bound render target.
                    if (currentRendertarget->windowType == kPsychTexture || windowRecord->windowType == kPsychTexture) {
                        // Ok we transition from- or to a texture. We need to backup the old content:
                        if (EmulateOldPTB) {
                            // OS-9 emulation: frontbuffer = framebuffer, backbuffer = offscreen scratchpad
                            if (PsychIsOnscreenWindow(currentRendertarget)) {
                                // Need to read the content of the frontbuffer to create the backup copy:
                                glReadBuffer(GL_FRONT);
                                glDrawBuffer(GL_FRONT);
                            }
                            else {
                                // Need to read the content of the backbuffer (scratch buffer for offscreen windows) to create the backup copy:
                                glReadBuffer(GL_BACK);
                                glDrawBuffer(GL_BACK);
                            }
                        }
                        
                        // In emulation mode for old PTB, we only need to back up offscreen windows, as they
                        // share the backbuffer as scratchpad. Each onscreen window has its own frontbuffer, so
                        // it will be unaffected by the switch --> No need to backup & restore.
                        if (!EmulateOldPTB || (EmulateOldPTB && !PsychIsOnscreenWindow(currentRendertarget))) {
                            if (currentRendertarget->textureNumber == 0) {
                                // This one is an onscreen window that doesn't have a shadow-texture yet. Create a suitable one.
                                glGenTextures(1, &(currentRendertarget->textureNumber));
                                glBindTexture(PsychGetTextureTarget(currentRendertarget), currentRendertarget->textureNumber);
								// If this system only supports power-of-2 textures, then we'll need a little trick:
								if (PsychGetTextureTarget(currentRendertarget)==GL_TEXTURE_2D) {
									// Ok, we need to create an empty texture of suitable power-of-two size:
									// Now we can do subimage texturing...
									twidth=1; while(twidth < (int) PsychGetWidthFromRect(currentRendertarget->rect)) { twidth = twidth * 2; };
									theight=1; while(theight < (int) PsychGetHeightFromRect(currentRendertarget->rect)) { theight = theight * 2; };
									glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, twidth, theight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
									glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, (int) PsychGetWidthFromRect(currentRendertarget->rect), (int) PsychGetHeightFromRect(currentRendertarget->rect));
								}
								else {
									// Supports rectangle textures. Just create texture as copy of framebuffer:
									glCopyTexImage2D(PsychGetTextureTarget(currentRendertarget), 0, GL_RGBA8, 0, 0, (int) PsychGetWidthFromRect(currentRendertarget->rect), (int) PsychGetHeightFromRect(currentRendertarget->rect), 0); 
								}
                            }
                            else {
								// Texture for this one already exist: Bind and update it:
								twidth  = (int) PsychGetWidthFromRect(currentRendertarget->rect);
								theight = (int) PsychGetHeightFromRect(currentRendertarget->rect);
								
								// If this is a texture in non-normal orientation, we need to swap width and height, and reset orientation
								// to upright:
								if (!PsychIsOnscreenWindow(currentRendertarget)) {
									// Texture. Handle size correctly:
									if ((currentRendertarget->textureOrientation <= 1) && (PsychGetTextureTarget(currentRendertarget)==GL_TEXTURE_2D)) {
										// Transposed power of two texture. Need to realloc texture...
										twidth=1; while(twidth < (int) PsychGetWidthFromRect(currentRendertarget->rect)) { twidth = twidth * 2; };
										theight=1; while(theight < (int) PsychGetHeightFromRect(currentRendertarget->rect)) { theight = theight * 2; };
										glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, twidth, theight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);

										// Reassign real size:
										twidth  = (int) PsychGetWidthFromRect(currentRendertarget->rect);
										theight = (int) PsychGetHeightFromRect(currentRendertarget->rect);
									}
									
									// After this backup-op, the texture orientation will be a nice upright one:
									currentRendertarget->textureOrientation = 2;
								}
								
								glBindTexture(PsychGetTextureTarget(currentRendertarget), currentRendertarget->textureNumber);
								if (PsychGetTextureTarget(currentRendertarget)==GL_TEXTURE_2D) {
									// Special case for power-of-two textures:
									glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, twidth, theight);
								}
								else {
									// This would be appropriate but crashes for no good reason on OS-X 10.4.4: glCopyTexSubImage2D(PsychGetTextureTarget(currentRendertarget), 0, 0, 0, 0, 0, (int) PsychGetWidthFromRect(currentRendertarget->rect), (int) PsychGetHeightFromRect(currentRendertarget->rect));                         
									glCopyTexImage2D(PsychGetTextureTarget(currentRendertarget), 0, GL_RGBA8, 0, 0, twidth, theight, 0); 
								}
                            }
                        }
                    }
                }
				
                // We only blit when a texture was involved, either as previous rendertarget or as new rendertarget:
                if (windowRecord->windowType == kPsychTexture || (currentRendertarget && currentRendertarget->windowType == kPsychTexture)) {
                    // OS-9 emulation: frontbuffer = framebuffer, backbuffer = offscreen scratchpad
                    if (EmulateOldPTB) {
                        // OS-9 emulation: frontbuffer = framebuffer, backbuffer = offscreen scratchpad
                        if (PsychIsOnscreenWindow(windowRecord)) {
                            // Need to write the content to the frontbuffer to restore from the backup copy:
                            glReadBuffer(GL_FRONT);
                            glDrawBuffer(GL_FRONT);
                        }
                        else {
							// Need to write the content to the backbuffer (scratch buffer for offscreen windows) to restore from the backup copy:
                            glReadBuffer(GL_BACK);
                            glDrawBuffer(GL_BACK);
                        }
                    }
					
                    // In emulation mode for old PTB, we only need to restore offscreen windows, as they
                    // share the backbuffer as scratchpad. Each onscreen window has its own frontbuffer, so
                    // it will be unaffected by the switch --> No need to backup & restore.
                    if (!EmulateOldPTB || (EmulateOldPTB && !PsychIsOnscreenWindow(windowRecord))) {
                        // Setup viewport and projections to fit new dimensions of new rendertarget:
                        PsychSetupView(windowRecord);
						glPushMatrix();
						glLoadIdentity();
						
                        // Now we need to blit the new rendertargets texture into the framebuffer. We need to make
						// sure that alpha-blending is disabled during this blit operation:
						if (glIsEnabled(GL_BLEND)) {
							// Alpha blending enabled. Disable it, blit texture, reenable it:
							glDisable(GL_BLEND);
							PsychBlitTextureToDisplay(windowRecord, windowRecord, windowRecord->rect, windowRecord->rect, 0, 0, 1);
							glEnable(GL_BLEND);
						}
						else {
							// Alpha blending not enabled. Just blit it:
							PsychBlitTextureToDisplay(windowRecord, windowRecord, windowRecord->rect, windowRecord->rect, 0, 0, 1);
						}
                        
						glPopMatrix();
						
						// Ok, the framebuffer has been initialized with the content of our texture.
                    }
                }
                
                // At this point we should have the image of our drawing target in the framebuffer.
                // If this transition didn't involve a switch from- or to a texture aka offscreen window,
                // then the whole switching up to now was a no-op... This way, we optimize for the common
                // case: No drawing to Offscreen windows at all, but proper use of other drawing functions
                // or of MakeTexture.
            }
            
			// Common code after fast- or slow-path switching:
			
            // Setup viewport, clip rectangle and projections to fit new dimensions of new drawingtarget:
            PsychSetupView(windowRecord);
			
            // Update our bookkeeping, set windowRecord as current rendertarget:
            currentRendertarget = windowRecord;
			
			// Transition finished.
        } // End of transition code.
    } // End of if(windowRecord) - then branch...
    else {
        // windowRecord==NULL. Reset of currentRendertarget and framebufferobject requested:
		
		// Reset current rendertarget to 'none':
        currentRendertarget = NULL;
		
		// Bind system framebuffer if FBO's supported on this system:
        if (glBindFramebufferEXT) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
	
    // Decrease recursion level tracker:
    recursionLevel--;
    
    return;
}

/* PsychSetupView()  -- Setup proper viewport, clip rectangle and projection
 * matrix for specified window.
 */
void PsychSetupView(PsychWindowRecordType *windowRecord)
{
    // Set viewport to windowsize:
    glViewport(0, 0, (int) PsychGetWidthFromRect(windowRecord->rect), (int) PsychGetHeightFromRect(windowRecord->rect));
    glScissor(0, 0, (int) PsychGetWidthFromRect(windowRecord->rect), (int) PsychGetHeightFromRect(windowRecord->rect));
    
    // Setup projection matrix for a proper orthonormal projection for this framebuffer or window:
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(windowRecord->rect[kPsychLeft], windowRecord->rect[kPsychRight], windowRecord->rect[kPsychBottom], windowRecord->rect[kPsychTop]);

    // Switch back to modelview matrix, but leave it unaltered:
    glMatrixMode(GL_MODELVIEW);
    return;
}

/* Set Screen - global flag which tells PTB if userspace rendering is active or not. */
void PsychSetUserspaceGLFlag(boolean inuserspace)
{
	inGLUserspace = inuserspace;
}

/* Get Screen - global flag which tells if we are in userspace rendering mode: */
boolean PsychIsUserspaceRendering(void)
{
	return(inGLUserspace);
}
