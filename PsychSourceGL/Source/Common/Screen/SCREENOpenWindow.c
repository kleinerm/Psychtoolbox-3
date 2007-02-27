/*
	SCREENOpenWindow.c		

	AUTHORS:

	Allen.Ingling@nyu.edu		awi 
	mario.kleiner@tuebingen.mpg.de  mk
  

	PLATFORMS:	All

	HISTORY:

		12/18/01	awi		Created.  Copied the Synopsis string from old version of psychtoolbox. 
		10/18/02	awi		Added defaults to allow for optional arguments.
		12/05/02	awi		Started over again for OS X without SDL.
		10/12/04	awi		In useString: changed "SCREEN" to "Screen", and moved commas to inside [].
                2/15/05         awi             Commented out glEnable(GL_BLEND) and mode settings.  
                04/03/05        mk              Added support for selecting binocular stereo output via native OpenGL.
		11/14/06        mk              New onscreen windows blank to their background color after successfull init.
		                                Support for specification of pixelSize's for 10-10-10-2, 16-16-16-16 and
						32-32-32-32 framebuffers on supported hardware.
	TO DO:

*/

#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] =  "[windowPtr,rect]=Screen('OpenWindow',windowPtrOrScreenNumber [,color] [,rect][,pixelSize][,numberOfBuffers][,stereomode][,multisample][,imagingmode]);";
//                                                               1                         2        3      4           5                 6            7             8
static char synopsisString[] =
	"Open an onscreen window. Specify a screen by a windowPtr or a screenNumber (0 is "
	"the main screen, with menu bar). \"color\" is the clut index (scalar or [r g b] "
	"triplet) that you want to poke into each pixel; default is white. If supplied, "
	"\"rect\" must contain at least one pixel. If a windowPtr is supplied then \"rect\" "
	"is in the window's coordinates (origin at upper left), and defaults to the whole "
	"window. If a screenNumber is supplied then \"rect\" is in screen coordinates "
	"(origin at upper left), and defaults to the whole screen. (In all cases, "
	"subsequent references to this new window will use its coordinates: origin at its "
	"upper left.) The Windows and OS-X version accepts \"rect\" but disregards it, the window is "
	"always the size of the display on which it appears. \"pixelSize\" sets the depth "
	"(in bits) of each pixel; default is to leave depth unchanged. "
        "\"numberOfBuffers\" is the number of buffers to use. Setting anything else than 2 will be "
        "useful for development/debugging of PTB itself but will mess up any real experiment. "
        "\"stereomode\" Type of stereo display algorithm to use: 0 (default) means: Monoscopic viewing. "
        "1 means: Stereo output via OpenGL on any stereo hardware that is supported by MacOS-X, e.g., the "
        "shutter glasses from CrystalView. 2 means: Left view compressed into top half, right view into bottom half. "
        "3 means left view compressed into bottom half, right view compressed into top half. 4 and 5 allow split "
        "screen display where left view is shown in left half, right view is shown in right half or the display. "
        "A value of 5 does the opposite (cross-fusion). Values of 6,7,8 and 9 enable Anaglyph stereo rendering "
        "of types left=Red, right=Green, vice versa and left=Red, right=Blue and vice versa. "
        "\"multisample\" This parameter, if provided and set to a value greater than zero, enables automatic "
        "hardware anti-aliasing of the display: For each pixel, 'multisample' color samples are computed and "
        "combined into a single output pixel color. Higher numbers provide better quality but consume more "
        "video memory and lead to a reduction in framerate due to the higher computational demand. The maximum "
        "number of samples is hardware dependent. Psychtoolbox will silently clamp the number to the maximum "
        "supported by your hardware if you ask for too much. On very old hardware, the value will be ignored. "
        "Read 'help AntiAliasing' for more in-depth information about multi-sampling. "
		"\"imagingmode\" This optional parameter enables PTB's internal image processing pipeline. The pipeline is "
		"off by default. Read 'help PsychImagingMode' for information about this feature. "
        "Opening or closing a window takes about two to three seconds, depending on type of connected display. "
        "COMPATIBILITY TO OS-9 PTB: If you absolutely need to run old code for the old MacOS-9 or Windows "
        "Psychtoolbox, you can switch into a compatibility mode by adding the command "
        "Screen('Preference', 'EmulateOldPTB', 1) at the very top of your script. This will restore "
        "Offscreen windows and WaitBlanking functionality, but at the same time disable most of the new "
        "features of the OpenGL Psychtoolbox. Please do not write new experiment code in the old style! "
        "Emulation mode is pretty new and may contain significant bugs, so use with great caution!";  

static char seeAlsoString[] = "OpenOffscreenWindow, SelectStereoDrawBuffer";

PsychError SCREENOpenWindow(void) 

{
    int					screenNumber, numWindowBuffers, stereomode, multiSample, imagingmode;
    PsychRectType 			rect;
    PsychColorType			color;
    PsychColorModeType  		mode; 
    boolean				isArgThere, settingsMade, didWindowOpen;
    PsychScreenSettingsType		screenSettings;
    PsychWindowRecordType		*windowRecord;
    double dVals[4];
    PsychDepthType		specifiedDepth, possibleDepths, currentDepth, useDepth;
	int dummy1;
	double dummy2, dummy3, dummy4;
	Boolean EmulateOldPTB = PsychPrefStateGet_EmulateOldPTB();
    
	//just for debugging
    //if (PSYCH_DEBUG == PSYCH_ON) printf("Entering SCREENOpen\n");

    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //cap the number of inputs
    PsychErrorExit(PsychCapNumInputArgs(8));   //The maximum number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(2));  //The maximum number of outputs

    //get the screen number from the windowPtrOrScreenNumber.  This also checks to make sure that the specified screen exists.  
    PsychCopyInScreenNumberArg(kPsychUseDefaultArgPosition, TRUE, &screenNumber);
    if(screenNumber==-1)
        PsychErrorExitMsg(PsychError_user, "The specified offscreen window has no ancestral screen."); 

    /*
      The depth checking is ugly because of this stupid depth structure stuff.  
      Instead get a descriptor of the current video settings, change the depth field,
      and pass it to a validate function wich searches a list of valid video modes for the display.
      There seems to be no point in checking the depths alone because the legality of a particular
      depth depends on the other settings specified below.  Its probably best to wait until we have
      digested all settings and then test the full mode, declarin an invalid
      mode and not an invalid pixel size.  We could notice when the depth alone is specified 
      and in that case issue an invalid depth value.
     */  

    //find the PixelSize first because the color specifier depends on the screen depth.  
    PsychInitDepthStruct(&currentDepth);  //get the current depth
    PsychGetScreenDepth(screenNumber, &currentDepth);
    PsychInitDepthStruct(&possibleDepths); //get the possible depths
    PsychGetScreenDepths(screenNumber, &possibleDepths);

    #if PSYCH_SYSTEM == PSYCH_OSX || PSYCH_SYSTEM == PSYCH_WINDOWS
       // MK Experimental Hack: Add the special depth values 30, 64 and 128 to the depth struct. This allows for
       // 10 bpc color buffers and 16 bpc, 32 bpc floating point color buffers on the latest ATI
       // and NVidia hardware. Unfortunately at this point of the init sequence, we are not able
       // to check if these formats are supported by the hardware. Ugly ugly ugly...
       PsychAddValueToDepthStruct(30, &possibleDepths);
       PsychAddValueToDepthStruct(64, &possibleDepths);
       PsychAddValueToDepthStruct(128, &possibleDepths);
    #endif

    PsychInitDepthStruct(&specifiedDepth); //get the requested depth and validate it.  
    isArgThere = PsychCopyInSingleDepthArg(4, FALSE, &specifiedDepth);

    PsychInitDepthStruct(&useDepth);
    if(isArgThere){ //if the argument is there check that the screen supports it...
        if(!PsychIsMemberDepthStruct(&specifiedDepth, &possibleDepths))
            PsychErrorExit(PsychError_invalidDepthArg);
        else
            PsychCopyDepthStruct(&useDepth, &specifiedDepth);
    }else //otherwise use the default
        PsychCopyDepthStruct(&useDepth, &currentDepth);

    //find the rect.
    PsychGetScreenRect(screenNumber, rect); 	//get the rect describing the screen bounds.  This is the default Rect.  

    // Override it with a user supplied rect, if one was supplied:
    isArgThere=PsychCopyInRectArg(kPsychUseDefaultArgPosition, FALSE, rect );
    if (IsPsychRectEmpty(rect)) PsychErrorExitMsg(PsychError_user, "OpenWindow called with invalid (empty) rect argument.");

    //find the number of specified buffers. 

    //OS X:	The number of backbuffers is not a property of the display mode but an attribute of the pixel format.
    //		Therefore the value is held by a window record and not a screen record.    

    numWindowBuffers=2;	
    PsychCopyInIntegerArg(5,FALSE,&numWindowBuffers);
    if(numWindowBuffers < 1 || numWindowBuffers > kPsychMaxNumberWindowBuffers) PsychErrorExit(PsychError_invalidNumberBuffersArg);

    // MK: Check for optional spec of stereoscopic display: 0 (the default) = monoscopic viewing.
    // 1 == Stereo output via OpenGL built-in stereo facilities: This will drive any kind of
    // stereo display hardware that is directly supported by MacOS-X.
    // 2/3 == Stereo output via compressed frame output: Only one backbuffer is used for both
    // views: The left view image is put into the top-half of the screen, the right view image
    // is put into the bottom half of the screen. External hardware demangles this combi-image
    // again into two separate images. CrystalEyes seems to be able to do this. One looses half
    // of the vertical resolution, but potentially gains refresh rate...
    // Future PTB version may include different stereo algorithms with an id > 1, e.g., 

    // anaglyph stereo, interlaced stereo, ...

    stereomode=0;
    PsychCopyInIntegerArg(6,FALSE,&stereomode);
    if(stereomode < 0 || stereomode > 9) PsychErrorExitMsg(PsychError_user, "Invalid stereomode provided (Valid between 0 and 9).");
	if (stereomode!=0 && EmulateOldPTB) PsychErrorExitMsg(PsychError_user, "Sorry, stereo display functions are not supported in OS-9 PTB emulation mode.");

    multiSample=0;
    PsychCopyInIntegerArg(7,FALSE,&multiSample);
    if(multiSample < 0) PsychErrorExitMsg(PsychError_user, "Invalid multisample value provided (Valid are positive numbers >= 0).");
	if (multiSample!=0 && EmulateOldPTB) PsychErrorExitMsg(PsychError_user, "Sorry, anti-aliasing functions are not supported in OS-9 PTB emulation mode.");

	imagingmode=0;
    PsychCopyInIntegerArg(8,FALSE,&imagingmode);
    if(imagingmode < 0) PsychErrorExitMsg(PsychError_user, "Invalid imaging mode provided (See 'help PsychImagingMode' for usage info).");
	if (imagingmode!=0 && EmulateOldPTB) PsychErrorExitMsg(PsychError_user, "Sorry, imaging pipeline functions are not supported in OS-9 PTB emulation mode.");
	
    //set the video mode to change the pixel size.  TO DO: Set the rect and the default color  
    PsychGetScreenSettings(screenNumber, &screenSettings);    
    PsychInitDepthStruct(&(screenSettings.depth));
    PsychCopyDepthStruct(&(screenSettings.depth), &useDepth);

    // Here is where all the work goes on:

    // If the screen is not already captured then to that:
    if(~PsychIsScreenCaptured(screenNumber)) {
        PsychCaptureScreen(screenNumber);

        settingsMade=PsychSetScreenSettings(screenNumber, &screenSettings);
        //Capturing the screen and setting its settings always occur in conjunction
        //There should be a check above to see if the display is captured and openWindow is attempting to chang
        //the bit depth
    }

#if PSYCH_SYSTEM == PSYCH_WINDOWS
    // On M$-Windows we currently only support - and therefore require >= 30 bpp color depth.
    if (PsychGetScreenDepthValue(screenNumber) < 30) {
      // Display running at less than 30 bpp. OpenWindow will fail on M$-Windows anyway, so let's abort
      // now.

      // Release the captured screen:
        PsychReleaseScreen(screenNumber);

	// Output warning text:
        printf("PTB-ERROR: Your display screen %i is not running at the required color depth of at least 30 bit.\n", screenNumber);
        printf("PTB-ERROR: The current setting is %i bit color depth..\n", PsychGetScreenDepthValue(screenNumber));
        printf("PTB-ERROR: This will not work on Microsoft Windows operating systems.\n");
        printf("PTB-ERROR: Please use the 'Display settings' control panel of Windows to change the color depth to\n");
        printf("PTB-ERROR: 32 bits per pixel ('True color' or 'Highest' setting) and then retry. It may be neccessary\n");
        printf("PTB-ERROR: to restart Matlab after applying the change...\n");
        fflush(NULL);

	// Abort with Matlab error:
	PsychErrorExitMsg(PsychError_user, "Insufficient color depth setting for display device (smaller than 30 bpp).");
    }

#endif

    //if (PSYCH_DEBUG == PSYCH_ON) printf("Entering PsychOpenOnscreenWindow\n");
    PsychCopyDepthStruct(&(screenSettings.depth), &useDepth);
    didWindowOpen=PsychOpenOnscreenWindow(&screenSettings, &windowRecord, numWindowBuffers, stereomode, rect, multiSample);

    if (!didWindowOpen) {
        PsychReleaseScreen(screenNumber);

        // We use this dirty hack to exit with an error, but without printing
        // an error message. The specific error message has been printed in
        // PsychOpenOnscreenWindow() already..
        PsychErrMsgTxt("");
    }

    // Sufficient display depth for full alpha-blending and such?
    if (PsychGetScreenDepthValue(screenNumber) < 24) {
        // Nope. Output a little warning.
        printf("PTB-WARNING: Your display screen %i is not running at 24 bit color depth or higher.\n", screenNumber);
        printf("PTB-WARNING: The current setting is %i bit color depth..\n", PsychGetScreenDepthValue(screenNumber));
        printf("PTB-WARNING: This could cause failure to work correctly or visual artifacts in stimuli\n");
        printf("PTB-WARNING: that involve Alpha-Blending. It can also cause drastically reduced color resolution\n");
        printf("PTB-WARNING: for your stimuli! Please try to switch your display to 'True Color' (Windows)\n");
        printf("PTB-WARNING: our 'Millions of Colors' (MacOS-X) to get rid of this warning and the visual artifacts.\n");
        fflush(NULL);
    }
    
	// Define clear color: This depends on the color range of our onscreen window...
    isArgThere=PsychCopyInColorArg(kPsychUseDefaultArgPosition, FALSE, &color); //get from user
    if(!isArgThere) PsychLoadColorStruct(&color, kPsychIndexColor, PsychGetWhiteValueFromWindow(windowRecord)); //or use the default
    PsychCoerceColorMode(&color);

	// Initialize internal image processing pipeline if requested:
	PsychInitializeImagingPipeline(windowRecord, imagingmode);
	
	// On OS-X, if we are int quad-buffered frame sequential stereo mode, we automatically generate
	// blue-line-sync style sync lines for use with stereo shutter glasses. We don't do this
	// by default on Windows or Linux: These systems either don't have stereo capable hardware,
	// or they have some and its drivers already take care of sync signal generation.
	if ((PSYCH_SYSTEM == PSYCH_OSX) && (windowRecord->stereomode==kPsychOpenGLStereo)) {
		if (PsychPrefStateGet_Verbosity()>3) printf("PTB-INFO: Enabling internal blue line sync renderer for quad-buffered stereo...\n");
		PsychPipelineAddBuiltinFunctionToHook(windowRecord, "LeftFinalizerBlitChain", "Builtin:RenderStereoSyncLine", TRUE, "");
		PsychPipelineEnableHook(windowRecord, "LeftFinalizerBlitChain");		
		PsychPipelineAddBuiltinFunctionToHook(windowRecord, "RightFinalizerBlitChain", "Builtin:RenderStereoSyncLine", TRUE, "");
		PsychPipelineEnableHook(windowRecord, "RightFinalizerBlitChain");		
	}

	// Activate new onscreen window for userspace drawing: If imaging pipeline is active, this
	// will bind the correct rendertargets for the first time:
    PsychSetGLContext(windowRecord);
	PsychSetDrawingTarget(windowRecord);

    // Set the clear color and perform a backbuffer-clear:
    PsychConvertColorToDoubleVector(&color, windowRecord, dVals);
    glClearColor(dVals[0], dVals[1], dVals[2], dVals[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    // Mark end of drawing op. This is needed for single buffered drawing:
    PsychFlushGL(windowRecord);

    // Make sure no OpenGL errors happened up to this point:
    PsychTestForGLErrors();

    // If we are in logo-startup mode (former blue-screen mode) and double-buffering
    // is enabled, then do an initial bufferswap & clear, so the display starts in
    // the user selected background color instead of staying at the blue screen or
    // logo display until the Matlab script first calls 'Flip'.
    if ((PsychPrefStateGet_VisualDebugLevel()>=4) && numWindowBuffers>=2) {
      // Do immediate bufferswap by an internal call to Screen('Flip'). This will also
	  // take care of clearing the backbuffer in preparation of first userspace drawing
	  // commands and such...
	  PsychFlipWindowBuffers(windowRecord, 0, 0, 0, 0, &dummy1, &dummy2, &dummy3, &dummy4);
      // Display now shows background color, so user knows that PTB's 'OpenWindow'
      // procedure is successfully finished.
    }

    PsychTestForGLErrors();

    //Return the window index and the rect argument.
    PsychCopyOutDoubleArg(1, FALSE, windowRecord->windowIndex);

	 // rect argument needs special treatment in stereo mode:
	 if (windowRecord->stereomode==kPsychFreeFusionStereo || windowRecord->stereomode==kPsychFreeCrossFusionStereo) {
			// Special case for stereo: Only half the real window width:
			PsychMakeRect(&rect, windowRecord->rect[kPsychLeft],windowRecord->rect[kPsychTop],
							  windowRecord->rect[kPsychLeft] + PsychGetWidthFromRect(windowRecord->rect)/2,windowRecord->rect[kPsychBottom]);
	 }
	 else {
			// Normal case:
			PsychMakeRect(&rect, windowRecord->rect[kPsychLeft],windowRecord->rect[kPsychTop],windowRecord->rect[kPsychRight],windowRecord->rect[kPsychBottom]);
	 }
    PsychCopyOutRectArg(2, FALSE, rect);

    return(PsychError_none);   
}


