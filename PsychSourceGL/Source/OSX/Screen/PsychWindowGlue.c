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
    PsychOpenOnscreenWindow()
    
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
 
    MK: Calibration/Measurement code was added that estimates the monitor refresh interval and the number of
        the last scanline (End of vertical blank interval). This increases time for opening an onscreen window
        by up to multiple seconds on slow (60 Hz) displays, but allows reliable syncing to VBL and kind of WaitBlanking
        functionality in Screen('Flip')... Also lots of tests for proper working of VBL-Sync and other stuff have been added.
        Contains experimental support for flipping multiple displays synchronously, e.g., for dual display stereo setups.
 
*/
boolean PsychOpenOnscreenWindow(PsychScreenSettingsType *screenSettings, PsychWindowRecordType **windowRecord, int numBuffers, int stereomode)
{

    //PsychTargetSpecificWindowRecordType 	cgStuff;
    CGOpenGLDisplayMask 			displayMask;
    CGDirectDisplayID				cgDisplayID;
    CGLPixelFormatAttribute			attribs[20];
    //CGLPixelFormatObj 			pixelFormatObj;
    long							swapInterval, numVirtualScreens;
    CGLError						error;
    //CGLRendererInfoObj				rendererInfo;
    GLboolean						isDoubleBuffer;
    int attribcount=0;
    int ringTheBell=-1;
    bool sync_trouble = false;
    bool sync_disaster = false;
    bool skip_synctests = PsychPrefStateGet_SkipSyncTests();
    int visual_debuglevel = PsychPrefStateGet_VisualDebugLevel();
    int conserveVRAM = PsychPrefStateGet_ConserveVRAM();
    
    // Child protection: We need 2 AUX buffers for compressed stereo.
    if ((conserveVRAM & kPsychDisableAUXBuffers) && (stereomode==kPsychCompressedTLBRStereo || stereomode==kPsychCompressedTRBLStereo)) {
        printf("ERROR! You tried to disable AUX buffers via Screen('Preference', 'ConserveVRAM')\n while trying to use compressed stereo, which needs AUX-Buffers!\n");
        return(FALSE);
    }
    
    //First allocate the window recored to store stuff into.  If we exit with an error PsychErrorExit() should
    //call PsychPurgeInvalidWindows which will clean up the window record. 
    PsychCreateWindowRecord(windowRecord);  		//this also fills the window index field.
    
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenSettings->screenNumber);
    displayMask=CGDisplayIDToOpenGLDisplayMask(cgDisplayID);
    attribs[attribcount++]=kCGLPFAFullScreen;
    //attribs[attribcount++]=kCGLPFAMinimumPolicy;
    //attribs[attribcount++]=kCGLPFAColorSize;
    //attribs[attribcount++]=32;
    attribs[attribcount++]=kCGLPFADisplayMask;
    attribs[attribcount++]=displayMask;
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
    // Finalize attribute array with NULL.
    attribs[attribcount]=(CGLPixelFormatAttribute)NULL;
    
    // Choose a matching display configuration and create the window and rendering context:
    // If one of these two fails, then the installed gfx hardware is not good enough to satisfy our
    // requirements, or we have massive ressource shortage in the system. -> Screwed up anyway, so we abort.
    error=CGLChoosePixelFormat(attribs, &((*windowRecord)->targetSpecific.pixelFormatObject), &numVirtualScreens);
    if (error) {
        printf("\nPTB-ERROR[ChoosePixelFormat failed]:The specified display may not support double buffering and/or stereo output. There could be insufficient video memory\n\n");
        FreeWindowRecordFromPntr(*windowRecord);
        return(FALSE);
    }
    
    error=CGLCreateContext((*windowRecord)->targetSpecific.pixelFormatObject, NULL, &((*windowRecord)->targetSpecific.contextObject));
    if (error) {
        printf("\nPTB-ERROR[ContextCreation failed]:The specified display may not support double buffering and/or stereo output. There could be insufficient video memory\n\n");
        FreeWindowRecordFromPntr(*windowRecord);
        return(FALSE);
    }
    
    // Activate syncing to onset of vertical retrace (VBL) for double-buffered windows:
    if(numBuffers>=2){
        swapInterval=1; 
        error=CGLSetParameter( (*windowRecord)->targetSpecific.contextObject, kCGLCPSwapInterval, &swapInterval);
        if (error) {
            sync_disaster = true;
            printf("\nPTB-WARNING: FAILED to enable synchronization to vertical retrace!\n\n");
        }
    }
    
    // Enable the OpenGL rendering context associated with our window:
    error=CGLSetCurrentContext((*windowRecord)->targetSpecific.contextObject);
    if (error) {
        printf("\nPTB-ERROR[SetCurrentContext failed]:The specified display may not support double buffering and/or stereo output. There could be insufficient video memory\n\n");
        FreeWindowRecordFromPntr(*windowRecord);
        return(FALSE);
    }
    
    // Switch to fullscreen display:
    error=CGLSetFullScreen((*windowRecord)->targetSpecific.contextObject);
    if (error) {
        printf("\nPTB-ERROR[CGLSetFullScreen failed]:The specified display may not support the current color depth -\nPlease switch to 'Millions of Colors' in Display Settings.\n\n");
        CGLSetCurrentContext(NULL);
        FreeWindowRecordFromPntr(*windowRecord); 
        return(FALSE);
    }
    
    if(numBuffers>=2){
        //This doesn't work.  GL thinks that there are double buffers when we fail to get that because
        //their was insufficient video memory to open a back buffer.  
        isDoubleBuffer=false;
        glGetBooleanv(GL_DOUBLEBUFFER, &isDoubleBuffer);
        if(!isDoubleBuffer){
            CGLDestroyPixelFormat((*windowRecord)->targetSpecific.pixelFormatObject);
            CGLSetCurrentContext(NULL);
            CGLClearDrawable( (*windowRecord)->targetSpecific.contextObject ) ;
            FreeWindowRecordFromPntr(*windowRecord); 
            return(FALSE);
        }
    }
        
    //get information from the renderer about support for double buffering
    /*
    error= CGLQueryRendererInfo(displayMask, &rendererInfo, &numRenderers);
    if(numRenderers>10) numRenderers=10;
    for(i=0;i<numRenderers;i++)
    {
        CGLDescribeRenderer(rendererInfo, i, kCGLRPBufferModes, &rendererPropertyValue);
        singleBufferSupport[i]=rendererPropertyValue & kCGLSingleBufferBit;
        doubleBufferSupport[i]=rendererPropertyValue & kCGLDoubleBufferBit;
        CGLDescribeRenderer(rendererInfo, i, kCGLRPBackingStore, backingStoreSupport+i);
        CGLDestroyRendererInfo(rendererInfo);
    }
    */
    
    //Configure OpenGL here
    gluOrtho2D(screenSettings->rect[kPsychLeft], screenSettings->rect[kPsychRight], screenSettings->rect[kPsychBottom], screenSettings->rect[kPsychTop]);
    
    //TO DO: set the clear color to be the color passed as the window background color.
      
    //Fill in the window record.
    (*windowRecord)->screenNumber=screenSettings->screenNumber;
    if(numBuffers==1)
        (*windowRecord)->windowType=kPsychSingleBufferOnscreen;
    else
        (*windowRecord)->windowType=kPsychDoubleBufferOnscreen;
    PsychCopyRect((*windowRecord)->rect, screenSettings->rect);
    (*windowRecord)->depth=PsychGetScreenDepthValue(screenSettings->screenNumber);
 
    if (true) {
      printf("\n\nPTB-INFO: This is the Screen-Prototype for PTB 1.0.7, intended as update for Psychtoolbox 1.0.6\n");
      printf("PTB-INFO: Implementation details may change in the final release for Psychtoolbox 1.0.7 - Use at your own Risk!\n\n");
    }

    // Check if the graphics hardware supports all required OpenGL-Extensions:
    const GLubyte* gl_extensions = glGetString(GL_EXTENSIONS);    
    if (strstr(gl_extensions, "GL_APPLE_client_storage")==NULL || strstr(gl_extensions, "GL_EXT_texture_rectangle")==NULL ||
        strstr(gl_extensions, "GL_APPLE_texture_range")==NULL) {
        printf("\n\nPTB-WARNING: Your graphics hardware doesn't support some of the required OpenGL-Extensions.\n");
        printf("PTB-WARNING: Required extensions are GL_APPLE_client_storage, GL_APPLE_texture_range and GL_EXT_texture_rectangle\n");
        printf("PTB-WARNING: This means that some Psychtoolbox Screen() functions will not work at all or at least not properly:\n");
        printf("PTB-WARNING: At least MakeTexture, DrawTexture and DrawText will fail. You could probably work around this for some experiments\n");
        printf("PTB-WARNING: by using PutImage and some cleverness, but we *strongly* recommend upgrading to a more modern gfx-card, e.g,\n");
        printf("PTB-WARNING: some ATI-Radeon or NVIDIA-GeForce model...\n");
        printf("PTB-WARNING: PTB will continue operation, but don't expect too much, except some frustration...\n\n");
        // Flash our visual warning bell:
        if (ringTheBell<2) ringTheBell=2;
    }
    
    // printf("\n\nExtensions are: %s\n\n", gl_extensions);
    
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
    
    // Are we running a multi-display setup? Then some tests and words of wisdom for the user are important
    // to reduce the traffic on the Psychtoolbox-Forum ;-)
    
    // Query number of physically connected and switched on displays...
    CGDisplayCount totaldisplaycount=0;
    CGGetOnlineDisplayList(0, NULL, &totaldisplaycount);
    
    bool multidisplay = (totaldisplaycount>1) ? true : false;    
    if (multidisplay) {
        printf("\n\nPTB-INFO: You are using a multi-display setup (%i displays): Some graphics hardware\n", totaldisplaycount);
        printf("PTB-INFO: for OS-X is known to have bugs which can cause PTB's VBL syncing and timestamp\n");
        printf("PTB-INFO: reporting code to fail or report wrong stimulus presentation timestamps!\n");
        printf("PTB-INFO: We have some built-in checks to detect this problems, but they rely on having the\n");
        printf("PTB-INFO: main stimulus presentation display set to a monitor-refresh rate *different* from\n");
        printf("PTB-INFO: the refresh rate of *all other* connected displays.\n");
        printf("PTB-INFO: Please set refresh intervals accordingly. If this is not possible for your study,\n");
        printf("PTB-INFO: e.g., 60 Hz LCD-Projector connected to Laptop with 60 Hz internal Flatpanel, then\n");
        printf("PTB-INFO: please test your system and display configuration *once* with an external CRT monitor\n");
        printf("PTB-INFO: connected. If it works once on your machine, it should always work...\n");
        printf("PTB-INFO: Ignore this message if you did the tests already - Sorry for spamming ;-)\n\n");
        // Unconditionally flash our visual warning bell in "Testsheet mode": This will also show some
        // test-pattern for visually testing if syncing and beamqueries work properly.
        // We skip the test-sheet if user asked us to skip sync-tests...
        if (!skip_synctests) PsychVisualBell((*windowRecord), 10, 3);
    }
    
    if (multidisplay && (!CGDisplayIsInMirrorSet(cgDisplayID) || PsychGetNumDisplays()>1)) {
        // This is a multi-display setup with separate (non-mirrored) displays: Bad for presentation timing :-(
        // Output some warning message to user, but continue. After all its the users
        // decision... ...and for some experiments were you need to show two different stims on two connected
        // monitors (haploscope, some stereo or binocular rivalry stuff) it is necessary. Let's hope they bought
        // a really fast gfx-card with plenty of VRAM :-)
        printf("\n\nPTB-WARNING: Some of your connected displays are *NOT* switched into mirror mode!\n");
        printf("PTB-WARNING: This can cause reduced drawing performance and therefore inaccurate/wrong stimulus\n");
        printf("PTB-WARNING: presentation timing due to skipped frames when showing moving-/movie stimuli.\n");
        printf("PTB-WARNING: Feel free to do this for development/debugging of your experiment, but for final\n");
        printf("PTB-WARNING: timing tests of your script and for running your study, please put *all* displays\n");
        printf("PTB-WARNING: into mirror mode or use a single-display setup. You can put displays into\n");
        printf("PTB-WARNING: mirror mode by opening the 'SystemPreferences' panel, setting all displays to the\n");
        printf("PTB-WARNING: same resolution and color depth and then checking the 'Mirror' checkbox.\n");
        printf("PTB-WARNING: Please try to set *different* refresh rates for the different displays if possible\n");
        printf("PTB-WARNING: If you use a non-mirrored dual-display setup for showing two different stimuli\n");
        printf("PTB-WARNING: on two different monitors (e.g., for binocular rivalry studies or stereo displays)\n");
        printf("PTB-WARNING: by the use of two separate Onscreen-Windows, then ignore this warning - PTB will take\n");
        printf("PTB-WARNING: care of that. But make sure your graphics card is fast enough and has plenty of VRAM\n");
        printf("PTB-WARNING: installed (minimum 128 MB recommended) and carefully check your presentation timing.\n\n");
        // Flash our visual warning bell:
        if (ringTheBell<0) ringTheBell=0;
    }
    
    if (CGDisplayIsInMirrorSet(cgDisplayID) && !CGDisplayIsInHWMirrorSet(cgDisplayID)) {
        // This is a multi-display setup with software-mirrored displays instead of hardware-mirrored ones: Not so good :-(
        // Output some warning message to user, but continue. After all its the users
        // decision...
        printf("\n\nPTB-WARNING: Not all connected displays are switched into HARDWARE-mirror mode!\n");
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
    
    if (numBuffers<2) {
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
        PsychSetWindowRecordValid(*windowRecord);
        return(TRUE);
    }
    
    // Everything below this line is only for double-buffered contexts!
    
    // Setup of initial interframe-interval by multiple methods, for comparison:
    
    // First we query what the OS thinks is our monitor refresh interval:
    double ifi_nominal=0;    
    if (PsychGetNominalFramerate(screenSettings->screenNumber) > 0) {
        // Valid nominal framerate returned by OS: Calculate nominal IFI from it.
        ifi_nominal = 1.0 / ((double) PsychGetNominalFramerate(screenSettings->screenNumber));        
    }

    // This is pure eye-candy: We clear both framebuffers to a blue background color,
    // just to get rid of the junk that's in the framebuffers...
    // If visual debuglevel < 4 then we clear to black background...
    glClearColor(0,0,((visual_debuglevel >= 4) ? 1:0),0);
        
    glDrawBuffer(GL_BACK_LEFT);
    glClear(GL_COLOR_BUFFER_BIT);
    CGLFlushDrawable((*windowRecord)->targetSpecific.contextObject);
    glClear(GL_COLOR_BUFFER_BIT);
    CGLFlushDrawable((*windowRecord)->targetSpecific.contextObject);
    // We do it twice to clear possible stereo-contexts as well...
    if ((*windowRecord)->stereomode==1) {
        glDrawBuffer(GL_BACK_RIGHT);
        glClear(GL_COLOR_BUFFER_BIT);
        CGLFlushDrawable((*windowRecord)->targetSpecific.contextObject);
        glClear(GL_COLOR_BUFFER_BIT);
        CGLFlushDrawable((*windowRecord)->targetSpecific.contextObject);
    }    
    glDrawBuffer(GL_BACK);

    // Make sure that the gfx-pipeline has settled to a stable state...
    glFinish();
    
    // Then we perform an initial calibration using VBL-Syncing of OpenGL:
    // We use 50 samples (50 monitor refresh intervals) and provide the ifi_nominal
    // as a hint to the measurement routine to stabilize it:
    double ifi_estimate = 0;
    int retry_count=0;    
    int numSamples;
    double stddev;
    double maxsecs;
    
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
    
    // Switch to RT scheduling for timing tests:
    PsychRealtimePriority(true);
    
    // Code for estimating the final scanline of the vertical blank interval of display (needed by Screen('Flip')):
    int VBL_Endline = -1;
    int i, maxline, bp;
    double tsum=0;
    double tcount=0;
    double ifi_beamestimate = 0;
    
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
    }        
    
    // Switch to previous scheduling mode after timing tests:
    PsychRealtimePriority(false);

    // Is the VBL endline >= VBL startline, aka screen height?
    if (VBL_Endline < PsychGetHeightFromRect((*windowRecord)->rect)) {
        // Completely bogus VBL_Endline detected! Warn the user and mark VBL_Endline
        // as invalid so it doesn't get used anywhere:
        sync_trouble = true;
        ifi_beamestimate = 0;
        printf("\nWARNING: Couldn't determine end-line of vertical blanking interval for your display! Trouble with beamposition queries?!?\n");
    }
    else {
        // Compute ifi from beampos:
        ifi_beamestimate = tsum / tcount;
    }
    
    // Compare ifi_estimate from VBL-Sync against beam estimate. If we are in OpenGL native
    // flip-frame stereo mode, a ifi_estimate approx. 2 times the beamestimate would be valid
    // and we would correct it down to half ifi_estimate.
    (*windowRecord)->VideoRefreshInterval = ifi_estimate;
    if ((*windowRecord)->stereomode == kPsychOpenGLStereo) {
        // Flip frame stereo enabled. Check for ifi_estimate = 2 * ifi_beamestimate:
        if (ifi_estimate >= 0.9 * 2 * ifi_beamestimate && ifi_estimate <= 1.1 * 2 * ifi_beamestimate) {
            // This seems to be a valid result: Flip-interval is roughly twice the monitor refresh interval.
            // We "force" ifi_estimate = 0.5 * ifi_estimate, so ifi_estimate roughly equals to ifi_nominal and
            // ifi_beamestimate, in order to simplify all timing checks below. We also store this value as
            // video refresh interval...
            ifi_estimate = ifi_estimate * 0.5f;
            (*windowRecord)->VideoRefreshInterval = ifi_estimate;
            printf("\nPTB-INFO: The timing granularity of stimulus onset/offset via Screen('Flip') is twice as long\n");
            printf("PTB-INFO: as the refresh interval of your monitor when using OpenGL flip-frame stereo on your setup.\n");
            printf("PTB-INFO: Please keep this in mind, otherwise you'll be confused about your timing.\n");
        }
    }
    
    printf("\n\nPTB-INFO: OpenGL-Renderer is %s :: %s :: %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));
    printf("PTB-Info: VBL startline = %i , VBL Endline = %i\n", (int) PsychGetHeightFromRect((*windowRecord)->rect), VBL_Endline);
    printf("PTB-Info: Measured monitor refresh interval from beamposition = %f ms [%f Hz].\n", ifi_beamestimate * 1000, 1/ifi_beamestimate);
    printf("PTB-Info: Measured monitor refresh interval from VBLsync = %f ms [%f Hz]. (%i valid samples taken, stddev=%f ms.)\n",
              ifi_estimate * 1000, 1/ifi_estimate, numSamples, stddev*1000);
    if (ifi_nominal > 0) printf("PTB-Info: Reported monitor refresh interval from operating system = %f ms [%f Hz].\n", ifi_nominal * 1000, 1/ifi_nominal);
    printf("PTB-Info: Small deviations between reported values are normal and no reason to worry.\n");
    if ((*windowRecord)->stereomode==kPsychOpenGLStereo) printf("PTB-INFO: Stereo display via OpenGL built-in sequential frame stereo enabled.\n");
    if ((*windowRecord)->stereomode==kPsychCompressedTLBRStereo) printf("PTB-INFO: Stereo display via vertical image compression enabled (Top=LeftEye, Bot.=RightEye).\n");
    if ((*windowRecord)->stereomode==kPsychCompressedTRBLStereo) printf("PTB-INFO: Stereo display via vertical image compression enabled (Top=RightEye, Bot.=LeftEye).\n");
    if ((*windowRecord)->stereomode==kPsychFreeFusionStereo) printf("PTB-INFO: Stereo display via free fusion enabled (2-in-1 stereo).\n");
    if ((*windowRecord)->stereomode==kPsychFreeCrossFusionStereo) printf("PTB-INFO: Stereo display via free cross-fusion enabled (2-in-1 stereo).\n");
    if ((*windowRecord)->stereomode==kPsychAnaglyphRGStereo) printf("PTB-INFO: Stereo display via Anaglyph Red-Green stereo enabled.\n");
    if ((*windowRecord)->stereomode==kPsychAnaglyphGRStereo) printf("PTB-INFO: Stereo display via Anaglyph Green-Red stereo enabled.\n");
    if ((*windowRecord)->stereomode==kPsychAnaglyphRBStereo) printf("PTB-INFO: Stereo display via Anaglyph Red-Blue stereo enabled.\n");
    if ((*windowRecord)->stereomode==kPsychAnaglyphBRStereo) printf("PTB-INFO: Stereo display via Anaglyph Blue-Red stereo enabled.\n");

    // Reliable estimate? These are our minimum requirements...
    if (numSamples<50 || stddev>0.001) {
        sync_disaster = true;
        printf("\nWARNING: Couldn't compute a reliable estimate of monitor refresh interval! Trouble with VBL syncing?!?\n");
    }
    
    // Check for mismatch between measured ifi from glFinish() VBLSync method and the value reported by the OS, if any:
    // This would indicate that we have massive trouble syncing to the VBL!
    if ((ifi_nominal > 0) && (ifi_estimate < 0.9 * ifi_nominal || ifi_estimate > 1.1 * ifi_nominal)) {
        printf("\nWARNING: Mismatch between measured monitor refresh interval and interval reported by operating system.\nThis indicates massive problems with VBL sync.\n");    
        sync_disaster = true;
    }
    
    // Another check for proper VBL syncing: We only accept monitor refresh intervals between 25 Hz and 250 Hz.
    // Lower- / higher values probably indicate sync-trouble...
    if (ifi_estimate < 0.004 || ifi_estimate > 0.040) {
        printf("\nWARNING: Measured monitor refresh interval indicates a display refresh of less than 25 Hz or more than 250 Hz?!?\nThis indicates massive problems with VBL sync.\n");    
        sync_disaster = true;        
    }
    
    // This is a "last resort" fallback: If user requests to *skip* all sync-tests and calibration routines
    // and we are unable to compute any ifi_estimate, we will fake one in order to be able to continue.
    // Either we use the nominal framerate provided by the operating system, or - if that's unavailable as well -
    // we assume a monitor refresh of 60 Hz, the typical value for flat-panels.
    if (ifi_estimate==0 && skip_synctests) {
        ifi_estimate = (ifi_nominal>0) ? ifi_nominal : (1.0/60.0);
        (*windowRecord)->nrIFISamples=1;
        (*windowRecord)->IFIRunningSum=ifi_estimate;
        (*windowRecord)->VideoRefreshInterval = ifi_estimate;
        printf("\nPTB-WARNING: Unable to measure monitor refresh interval! Using a fake value of %f milliseconds.\n", ifi_estimate*1000);
    }
    
    if (sync_disaster) {
        // We fail! Continuing would be too dangerous without a working VBL sync. We don't
        // want to spoil somebodys study just because s(he) is relying on a non-working sync.
        printf("\n\n");
        printf("----- ! PTB - ERROR: SYNCHRONIZATION FAILURE ! ----\n\n");
        printf("One or more internal checks (see Warnings above) indicate that synchronization\n");
        printf("of Psychtoolbox to the vertical retrace (VBL) is screwed up for your setup.\n\n");
        printf("This would have the following bad effects:\n");
        printf("- Screen('Flip') not synchronizing to the VBL -> Flicker & tearing artifacts, messed up stimulus onset and offset timing.\n");
        printf("- Wrong stimulus presentation timing and/or wrong synchronization of Matlab to the VBL.\n");
        printf("- Inaccurate or completely wrong timestamps (VBLTimestamp, StimulusOnsetTime, FlipTimestamp) for stimulus timing reported by Flip\n");
        printf("- Inaccurate or completely wrong rasterbeam positions and monitor refresh intervals reported by Screen('Flip') and Screen('Framerate')\n");
        printf("\n\n");
        printf("Please try the following measures in the given order to solve or work-around the problem:\n");
        printf("1. Make sure that your machine is not running other demanding software in parallel and then retry.\n");
        printf("2. If you use a dual/multi-display setup, please put all displays in mirror-mode with same resolution and color depth,\n");
        printf("   but different monitor refresh interval (set stimulus display to higher refresh rate).\n");
        printf("3. If it doesn't help or is not possible, try to change assignment of the primary- and secondary display in the Display\n");
        printf("   Preference settings of MacOS-X.\n");
        printf("4. On desktops or tower machines with two external display connectors, *PHYSICALLY* swap the connections, aka\n");
        printf("   which display is plugged into which video-out socket at the back-side of your computer.\n");
        printf("5. If that fails or is not possible on dual- or multi-display setups, disconnect or disable all displays except\n");
        printf("   the one for stimulus presentation to the subject.\n");
        printf("\n\nIf everything else fails, please report your problem including all these messages and information on your specific\n");
        printf("setup (Type of graphics card and connected displays, output of 'DescribeComputer') to the Psychtoolbox forum.\n");
        printf("-> You may have found a bug in Psychtoolbox, MacOS-X or your graphics-hardware.\n\n\n\n");
        // Flash our visual warning bell at alert-level: We only flash 1 sec if sync-tests should be skipped.
        PsychVisualBell((*windowRecord), (skip_synctests) ? 1 : 4, 2);
        if (!skip_synctests) return(FALSE);
    }
    
    // Ok, basic syncing to VBL via CGLFlushDrawable + glFinish seems to work and we have a valid
    // estimate of monitor refresh interval...
    
    // Check for mismatch between measured ifi from beamposition and from glFinish() VBLSync method.
    // This would indicate that the beam position is reported from a different display device
    // than the one we are VBL syncing to. -> Trouble!
    if (ifi_beamestimate < 0.8 * ifi_estimate || ifi_beamestimate > 1.2 * ifi_estimate) {
        printf("\nWARNING: Mismatch between measured monitor refresh intervals! This indicates problems with rasterbeam position queries.\n");    
        sync_trouble = true;
    }

    if (sync_trouble) {
        // Fail-Safe: Mark VBL-Endline as invalid, so a couple of mechanisms get disabled in Screen('Flip') aka PsychFlipWindowBuffers().
        VBL_Endline = -1;
        printf("\n\n");
        printf("----- ! PTB - WARNING: SYNCHRONIZATION TROUBLE ! ----\n\n");
        printf("One or more internal checks (see Warnings above) indicate that\n");
        printf("queries of rasterbeam position are not properly working for your setup.\n\n");
        printf("This will cause Screen('Flip') to report less accurate/robust timestamps\n");
        printf("(VBLTimestamp, StimulusOnsetTime) for stimulus timing. As accurate timestamps are crucial for\n");
        printf("properly timed stimulus presentation and automatic checking of presentation timing, you may experience\n");
        printf("an excessive number of skipped frames / wrong presentation deadlines *plus* the built-in check for such\n");
        printf("problems doesn't work reliably anymore!\n");
        printf("\n\n");
        printf("Please try the following measures to solve or work-around the problem:\n");
        printf("1. Make sure that your machine is not running other demanding software in parallel and then retry.\n");
        printf("2. If you use a dual-display setup, please put all displays in mirror-mode with same resolution and color depth.\n");
        printf("3. If it doesn't help or is not possible, try to change assignment of the primary- and secondary display.\n");
        printf("4. On desktops or tower machines with two external display connectors, *PHYSICALLY* swap the connections, aka\n");
        printf("   which display is plugged into which video-out socket at the back-side of your computer.\n");
        printf("5. If that doesn't help or is not possible on dual- or multi-display setup, disconnect or disable all displays except\n");
        printf("   the one for stimulus presentation to the subject.\n");
        printf("\n\nIf everything fails, please report your problem including all these messages and information on your specific\n");
        printf("setup (Type of graphics card and connected displays, output of 'DescribeComputer') to the Psychtoolbox forum.\n");
        printf("-> You may have found a bug in MacOS-X or your graphics-hardware.\n\n\n\n");
        // Flash our visual warning bell:
        if (ringTheBell<2) ringTheBell=2;        
    }
    
    // The start of a gfx-card "Blacklist"...
    if ((strstr(glGetString(GL_VENDOR), "ATI")!=NULL) && multidisplay) {
        // ATI card detected -> Give hint to be extra cautious about beampos...
        printf("\n\nPTB-HINT: Your graphics card is KNOWN TO HAVE TROUBLE with beamposition queries on some dual display setups\n");
        printf("PTB-HINT: due to an ATI driver bug in all versions of MacOS-X 10.3.x and in early versions of MacOS-X 10.4!\n");
        printf("PTB-HINT: Please *double-check* this by setting different monitor refresh rates for the different displays.\n");
        printf("PTB-HINT: If you then get a warning about SYNCHRONIZATION TROUBLE, it might help to *physically*\n");
        printf("PTB-HINT: reconnect your displays: Swap, which display is plugged into which socket at the back-side\n");
        printf("PTB-HINT: of your computer! If that doesn't help, you'll have to switch to a single display configuration\n");
        printf("PTB-HINT: for getting highest possible timing accuracy.\n");
        printf("PTB-HINT: ATI is working on a fix for their drivers. Please check the Psychtoolbox forum regularly for updates.\n");
    }
    
    // Assign our best estimate of the scanline which marks end of vertical blanking interval:
    (*windowRecord)->VBL_Endline = VBL_Endline;
    
    //mark the contents of the window record as valid.  Between the time it is created (always with PsychCreateWindowRecord) and when it is marked valid 
    //(with PsychSetWindowRecordValid) it is a potential victim of PsychPurgeInvalidWindows.  
    PsychSetWindowRecordValid(*windowRecord);

    // Ring the visual bell if anything demands this:
    if (ringTheBell>=0 && !skip_synctests) PsychVisualBell((*windowRecord), 4, ringTheBell);

    // Done.
    return(TRUE);
}


/*
    PsychOpenOffscreenWindow()
    
    Accept specifications for the offscreen window in the platform-neutral structures, convert to native CoreGraphics structures,
    create the surface, allocate a window record and record the window specifications and memory location there.
	
	TO DO:  We need to walk down the screen number and fill in the correct value for the benefit of TexturizeOffscreenWindow
*/
boolean PsychOpenOffscreenWindow(double *rect, int depth, PsychWindowRecordType **windowRecord)
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


void PsychCloseWindow(PsychWindowRecordType *windowRecord)
{
    PsychWindowRecordType	**windowRecordArray;
    int                         i, numWindows; 
    
    if(PsychIsOnscreenWindow(windowRecord) || PsychIsOffscreenWindow(windowRecord)){
		CGLSetCurrentContext(NULL);
                // MK: Hack, needed to work around a "screen corruption on shutdown" bug.
                // When closing stereo display windows, it sometimes leads to a completely
                // messed up and unusable display.
                if (PsychIsOnscreenWindow(windowRecord)) {
                    PsychReleaseScreen(windowRecord->screenNumber);
                    CGLClearDrawable(windowRecord->targetSpecific.contextObject);
                    PsychCaptureScreen(windowRecord->screenNumber);
                }
                CGLDestroyPixelFormat(windowRecord->targetSpecific.pixelFormatObject);
		CGLDestroyContext(windowRecord->targetSpecific.contextObject);
                
                // We need to NULL-out all references to the - now destroyed - OpenGL context:
                PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);
                for(i=0;i<numWindows;i++) {
                    if (windowRecordArray[i]->targetSpecific.contextObject == windowRecord->targetSpecific.contextObject &&
                        windowRecordArray[i]->windowType==kPsychTexture) {
                        windowRecordArray[i]->targetSpecific.contextObject = NULL;
                    }
                }
                PsychDestroyVolatileWindowRecordPointerList(windowRecordArray);
                windowRecord->targetSpecific.contextObject=NULL;
                
                if(PsychIsOffscreenWindow(windowRecord)) free((void*)windowRecord->surface);
	}else if(windowRecord->windowType==kPsychTexture){
		PsychFreeTextureForWindowRecord(windowRecord);
    }else
		PsychErrorExitMsg(PsychError_internal, "Unrecognized window type");

    if (PsychIsOnscreenWindow(windowRecord) && (windowRecord->nr_missed_deadlines>0)) {
        printf("\n\nWARNING: PTB's Screen('Flip') command missed the requested stimulus presentation deadline %i times!\n\n", windowRecord->nr_missed_deadlines);
    }
    
    if (PsychIsOnscreenWindow(windowRecord) && PsychPrefStateGet_SkipSyncTests()) {
        printf("\n\nWARNING: This session of your experiment was run by you with the setting Screen('Preference', 'SkipSyncTests', 1).\n");
        printf("WARNING: This means that some internal self-tests and calibrations were skipped. Your stimulus presentation timing\n");
        printf("WARNING: may have been wrong. This is fine for development and debugging of your experiment, but for running the real\n");
        printf("WARNING: study, please make sure to set Screen('Preference', 'SkipSyncTests', 0) for maximum accuracy and reliability.\n");
    }
    
    PsychErrorExit(FreeWindowRecordFromPntr(windowRecord));
}


/*
    PsychFlushGL()
    
    Render the queue of GL drawing commands onto the window.  

    -If single-buffered window then set the context to the specified window and call glFlush().
    -If double-buffered window then don't do anything because CGLFlushDrawable which is called by PsychFlipWindowBuffers()
    implicitley calls glFlush() before flipping the buffers. Apple warns of lowered perfomance if glFlush() is called 
    immediately before CGLFlushDrawable().
*/
void PsychFlushGL(PsychWindowRecordType *windowRecord)
{
    if(windowRecord->windowType!=kPsychDoubleBufferOnscreen)
        glFlush();
            
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
        slackfactor = 1.5;
    }
    
    // Retrieve display id and screen size spec that is needed later...
    PsychGetCGDisplayIDFromScreenNumber(&displayID, windowRecord->screenNumber);
    screenwidth=(int) PsychGetWidthFromRect(windowRecord->rect);
    screenheight=(int) PsychGetHeightFromRect(windowRecord->rect);

    // Should we sync to the onset of vertical retrace?
    // Note: Flipping the front- and backbuffers is nearly always done in sync with VBL on
    // a double-buffered setup. sync_to_vbl specs, if the application should wait for
    // the VBL to start before continuing execution.
    sync_to_vbl = (vbl_synclevel == 0 || vbl_synclevel == 3) ? true : false;
    
    if (vbl_synclevel==2) {
        // We are requested to flip immediately, instead of syncing to VBL. Disable VBL-Sync.
        long swapInterval=0; 
        CGLError error=CGLSetParameter(windowRecord->targetSpecific.contextObject, kCGLCPSwapInterval, &swapInterval);
        if (error) {
            PsychErrorExitMsg(PsychError_internal, "\nFlip FAILED to disable synchronization to vertical retrace!\n\n");
        }        
    }
    
    if (multiflip > 0) {
        // Experimental Multiflip requested. Build list of all onscreen windows...
        PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);
    }
    
    if (multiflip == 2) {
        // Disable VBL-Sync for all onscreen windows except our primary one:
        for(i=0;i<numWindows;i++) {
            long swapInterval=0; 
            if (PsychIsOnscreenWindow(windowRecordArray[i]) && (windowRecordArray[i]!=windowRecord)) {
                CGLError error=CGLSetParameter(windowRecordArray[i]->targetSpecific.contextObject, kCGLCPSwapInterval, &swapInterval);
                if (error) {
                    PsychErrorExitMsg(PsychError_internal, "\nMultiFlip FAILED to disable synchronization to vertical retrace!\n\n");
                }                        
            }
        }
    }
    
    // Enable GL-Context of current onscreen window:
    PsychSetGLContext(windowRecord);
    
    // Backup current assignment of read- writebuffers:
    glGetIntegerv(GL_READ_BUFFER, &read_buffer);
    glGetIntegerv(GL_DRAW_BUFFER, &draw_buffer);
    
    // Perform preflip-operations: Backbuffer backups for the different dontclear-modes
    // and special compositing operations for specific stereo algorithms...
    PsychPreFlipOperations(windowRecord, dont_clear);
    
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
        tshouldflip = flipwhen + slackfactor * currentflipestimate;
        
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
        
        // Do we know the exact system time when a VBL happened in the past?
        if ((windowRecord->time_at_last_vbl > 0) && (currentflipestimate > 0)) {
            // Yes! We use this as a base-line time to compute from the current time a virtual deadline,
            // which is exactly in the middle of the current monitor refresh interval, so this deadline
            // is "as good" as a real deadline spec'd by the user via "flipwhen"!
            //
            // As long as we do synchronous Flips (sync_to_vbl == true - PTB blocks until VBL onset),
            // we should have a valid time_at_last_vbl, so this mechanism works.
            // Only on the *very first* invocation of Flip either after PTB-Startup or after a non-blocking
            // Flip, we can't do this because the time_at_last_vbl timestamp isn't available...
            tshouldflip = windowRecord->time_at_last_vbl + ((0.5 + floor((tshouldflip - windowRecord->time_at_last_vbl) / currentflipestimate)) * currentflipestimate);
        }

        // Calculate final deadline for the lock on next retrace - case:
        tshouldflip = tshouldflip + slackfactor * currentflipestimate;        
    }
    
    // Trigger the "Front <-> Back buffer swap (flip) on next vertical retrace":
    CGLFlushDrawable(windowRecord->targetSpecific.contextObject);
    
    // Multiflip with vbl-sync requested?
    if (multiflip==1) {
        //  Trigger the "Front <-> Back buffer swap (flip) on next vertical retrace"
        //  for all onscreen windows except our primary one:
        for(i=0;i<numWindows;i++) {
            if (PsychIsOnscreenWindow(windowRecordArray[i]) && (windowRecordArray[i]!=windowRecord)) {
                CGLFlushDrawable(windowRecordArray[i]->targetSpecific.contextObject);
            }
        }
    }
    
    // Pause execution of application until start of VBL, if requested:
    if (sync_to_vbl) {
        if (vbl_synclevel==3) {
            // Wait for VBL onset via experimental busy-waiting spinloop instead of
            // blocking: We spin-wait until the rasterbeam of our master-display enters the
            // VBL-Area of the display:
            while (screenheight > (int) CGDisplayBeamPosition(displayID));
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
                    CGLFlushDrawable(windowRecordArray[i]->targetSpecific.contextObject);
                }
            }
        }
        
        // Query and return rasterbeam position immediately after Flip and before timestamp:
        *beamPosAtFlip=(int) CGDisplayBeamPosition(displayID);
         // We take a timestamp here and return it to "userspace"
        PsychGetAdjustedPrecisionTimerSeconds(&time_at_vbl);

        // Calculate estimate of real time of VBL, based on our post glFinish() timestamp, post glFinish() beam-
        // position and the roughly known height of image and duration of IFI. The corrected time_at_vbl
        // contains time at start of VBL. This value is crucial for control stimulus presentation timing.
        // We also estimate the end of VBL, aka the stimulus onset time in time_at_onset.
        
        // First we calculate the number of scanlines that have passed since start of VBL area:
        double vbl_startline = screenheight;
        double vbl_endline = windowRecord->VBL_Endline;
        double vbl_lines_elapsed, onset_lines_togo;
        
        // VBL_Endline is determined in a calibration loop in PsychOpenOnscreenWindow above.
        // If this fails for some reason, we mark it as invalid by setting it to -1.
        if (windowRecord->VBL_Endline != -1) {
            if (*beamPosAtFlip >= vbl_startline) {
                vbl_lines_elapsed = *beamPosAtFlip - vbl_startline;
                onset_lines_togo = vbl_endline - (*beamPosAtFlip) + 1;
            }
            else {
                vbl_lines_elapsed = vbl_endline - vbl_startline + 1 + *beamPosAtFlip;
                onset_lines_togo = -1.0 * (*beamPosAtFlip);
            }
            
            // From the elapsed number we calculate the elapsed time since VBL start:
            double vbl_time_elapsed = vbl_lines_elapsed / vbl_endline * currentrefreshestimate; 
            double onset_time_togo = onset_lines_togo / vbl_endline * currentrefreshestimate;
            // Compute of stimulus-onset, aka time when retrace is finished:
            *time_at_onset = time_at_vbl + onset_time_togo;
            // Now we correct our time_at_vbl by this correction value:
            time_at_vbl = time_at_vbl - vbl_time_elapsed;
        }
        else {
            // If we can't depend on timestamp correction, we just set time_at_onset == time_at_vbl.
            // This is not strictly correct, but at least the user doesn't have to change the whole
            // implementation of his code and we've warned him anyway at Window open time...
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

    // Restore assignments of read- and drawbuffers to pre-Flip state:
    glReadBuffer(read_buffer);
    glDrawBuffer(draw_buffer);
    
    // Reset flags used for avoiding redundant Pipeline flushes and backbuffer-backups:
    // This flags are altered and checked by SCREENDrawingFinished() and PsychPreFlipOperations() as well:
    windowRecord->PipelineFlushDone = false;
    windowRecord->backBufferBackupDone = false;
    
    // If we disabled (upon request) VBL syncing, we have to reenable it here:
    if (vbl_synclevel==2) {
        long swapInterval=1; 
        CGLError error=CGLSetParameter(windowRecord->targetSpecific.contextObject, kCGLCPSwapInterval, &swapInterval);
        if (error) {
            PsychErrorExitMsg(PsychError_internal, "\nFlip FAILED to re-enable synchronization to vertical retrace!\n\n");
        }        
    }
    
    // Was this an experimental Multiflip with "hard" busy flipping?
    if (multiflip==2) {
        // Reenable VBL-Sync for all onscreen windows except our primary one:
        for(i=0;i<numWindows;i++) {
            long swapInterval=1; 
            if (PsychIsOnscreenWindow(windowRecordArray[i]) && (windowRecordArray[i]!=windowRecord)) {
                CGLError error=CGLSetParameter(windowRecordArray[i]->targetSpecific.contextObject, kCGLCPSwapInterval, &swapInterval);
                if (error) {
                    PsychErrorExitMsg(PsychError_internal, "\nMultiFlip FAILED to re-enable synchronization to vertical retrace!\n\n");
                }                        
            }
        }
    }
    
    if (multiflip>0) {
        // Cleanup our multiflip windowlist:
        PsychDestroyVolatileWindowRecordPointerList(windowRecordArray);
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
    // MK: Setup new context if it isn't already setup. -> Avoid redundant context switch.
    if (CGLGetCurrentContext() != windowRecord->targetSpecific.contextObject) {
        CGLSetCurrentContext(windowRecord->targetSpecific.contextObject);
    }
}

/*
    PsychClearGLContext()
    
    Clear the drawing context.  
*/
void PsychUnsetGLContext(void)
{
    CGLSetCurrentContext(NULL);
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
        glDrawBuffer(GL_BACK_LEFT);
        
        PsychGetAdjustedPrecisionTimerSeconds(&tnew);
        tstart = tnew;
        
        // Take samples during consecutive refresh intervals:
        // We measure until either:
        // - A maximum measurment time of maxsecs seconds has elapsed... (This is the emergency switch to prevent infinite loops).
        // - Or at least numSamples valid samples have been taken AND measured standard deviation is below the requested deviation stddev.
        for (i=0; (fallthroughcount<10) && ((tnew - tstart) < *maxsecs) && (n < *numSamples || ((n >= *numSamples) && (tstddev > reqstddev))); i++) {
            // Schedule a buffer-swap on next VBL:
            CGLFlushDrawable(windowRecord->targetSpecific.contextObject);
            
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
                      ((windowRecord->stereomode==kPsychOpenGLStereo) && (tdur > 0.8 * 2 * intervalHint) && (tdur < 1.2 * 2 * intervalHint))
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
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, windowRecord->screenNumber);    
    
    // Query current visual feedback level and abort, if it doesn't
    // allow requested type of visual feedback:
    int visual_debuglevel = PsychPrefStateGet_VisualDebugLevel();    
    if (belltype == 0 && visual_debuglevel < 3) return;
    if (belltype == 1 && visual_debuglevel < 2) return;
    if (belltype == 2 && visual_debuglevel < 1) return;
    if (belltype == 3 && visual_debuglevel < 5) return;
    
    glGetDoublev(GL_COLOR_CLEAR_VALUE, (GLdouble*) &color);

    PsychGetAdjustedPrecisionTimerSeconds(&tdeadline);
    tdeadline+=duration;
    
    // Setup context:
    PsychSetGLContext(windowRecord);
    float w=PsychGetWidthFromRect(windowRecord->rect);
    float h=PsychGetHeightFromRect(windowRecord->rect);
    
    // Clear out both buffers so it doesn't lool like junk:
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    CGLFlushDrawable(windowRecord->targetSpecific.contextObject);
    glClear(GL_COLOR_BUFFER_BIT);
    CGLFlushDrawable(windowRecord->targetSpecific.contextObject);
    
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
        CGLFlushDrawable(windowRecord->targetSpecific.contextObject);
        
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
    GLint auxbuffers;

    // Early reject: If this flag is set, then there's no need for any processing:
    if (windowRecord->backBufferBackupDone) return;

    // Switch to associated GL-Context of windowRecord:
    PsychSetGLContext(windowRecord);

    // Reset viewport to full-screen default:
    glViewport(0, 0, screenwidth, screenheight);
    
    // Reset color buffer writemask to "All enabled":
    glColorMask(TRUE, TRUE, TRUE, TRUE);

    // Query number of available AUX-buffers:
    glGetIntegerv(GL_AUX_BUFFERS, &auxbuffers);

    // Set transform matrix to well-defined state:
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    //glLoadIdentity();
    
    // Check for compressed stereo handling...
    if (stereo_mode==kPsychCompressedTLBRStereo || stereo_mode==kPsychCompressedTRBLStereo) {
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
        GLint read_buffer, draw_buffer;
        glGetIntegerv(GL_READ_BUFFER, &read_buffer);
        glGetIntegerv(GL_DRAW_BUFFER, &draw_buffer);
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

        glEnable(GL_BLEND);
        // Restore assignment of read- writebuffers:
        glReadBuffer(read_buffer);
        glDrawBuffer(draw_buffer);        
    }

    // Restore modelview matrix:
    glPopMatrix();
    
    // Tell Flip that backbuffer backup has been done already to avoid redundant backups:
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

    // Set transform matrix to well-defined state:
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    //glLoadIdentity();

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
    PsychTestForGLErrors();
    
    // Done.
    return;
}
