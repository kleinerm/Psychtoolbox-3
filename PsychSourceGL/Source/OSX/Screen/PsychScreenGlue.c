/*
	PsychToolbox3/Source/OSX/Screen/PsychScreenGlue.c
	
	PLATFORMS:	
	
		This is the OS X Core Graphics version only.  
				
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	
		10/20/02		awi			Wrote it mostly by adding in SDL-specific refugeess (from an experimental SDL Psychtoolbox) from header and source files.
		11/16/04		awi			added  PsychGetGlobalScreenRect.  Enhanced DESCRIPTION text.  
		04/29/05        mk          Return id of primary display for displays in hardware mirroring sets.
		12/29/06		mk			Implement query code for DAC output resolution on OS-X, finally...
        							
	DESCRIPTION:
	
		Functions in this file comprise an abstraction layer for probing and controlling screen state.  
		
		Each C function which implements a particular Screen subcommand should be platform neutral.  For example, the source to SCREENPixelSizes() 
		should be platform-neutral, despite that the calls in OS X and Windows to detect available pixel sizes are
		different.  The platform specificity is abstracted out in C files which end it "Glue", for example PsychScreenGlue, PsychWindowGlue, 
		PsychWindowTextClue.
	
		In addition to glue functions for windows and screen there are functions which implement shared functionality between between Screen commands,
		such as ScreenTypes.c and WindowBank.c. 
			
	NOTES:
	
	TO DO: 
	
		¥ The "glue" files should should be suffixed with a platform name.  The original (bad) plan was to distingish platform-specific files with the same 
		name by their placement in a directory tree.
		
		¥ All of the functions which accept a screen number should be suffixed with "...FromScreenNumber". 
*/


#include "Screen.h"

// file local variables

// Maybe use NULLs in the settings arrays to mark entries invalid instead of using boolean flags in a different array.   
static boolean				displayLockSettingsFlags[kPsychMaxPossibleDisplays];
static CFDictionaryRef		displayOriginalCGSettings[kPsychMaxPossibleDisplays];        	//these track the original video state before the Psychtoolbox changed it.  
static boolean				displayOriginalCGSettingsValid[kPsychMaxPossibleDisplays];
static CFDictionaryRef		displayOverlayedCGSettings[kPsychMaxPossibleDisplays];        	//these track settings overlayed with 'Resolutions'.  
static boolean				displayOverlayedCGSettingsValid[kPsychMaxPossibleDisplays];
static CGDisplayCount 		numDisplays;
static CGDirectDisplayID 	displayCGIDs[kPsychMaxPossibleDisplays];    
                                                                                                
//file local functions
void InitCGDisplayIDList(void);
void PsychLockScreenSettings(int screenNumber);
void PsychUnlockScreenSettings(int screenNumber);
boolean PsychCheckScreenSettingsLock(int screenNumber);
boolean PsychGetCGModeFromVideoSetting(CFDictionaryRef *cgMode, PsychScreenSettingsType *setting);

//Initialization functions
void InitializePsychDisplayGlue(void)
{
    int i;
    
    //init the display settings flags.
    for(i=0;i<kPsychMaxPossibleDisplays;i++){
        displayLockSettingsFlags[i]=FALSE;
        displayOriginalCGSettingsValid[i]=FALSE;
        displayOverlayedCGSettingsValid[i]=FALSE;
    }
    
    //init the list of Core Graphics display IDs.
    InitCGDisplayIDList();
}

// MK-TODO: There's still a bug here: We need to call InitCGDisplayIDList() not only at
// Screen-Init time but also as part of *EACH* query to the list...
// Otherwise, if the user changes display settings (layout of displays, primary<->secondary display,
// mirror<->non-mirror mode) or connects/disconnects/replugs/powers on or powers off displays,
// while a Matlab session is running and without "clear Screen"
// after the change, PTB will not notice the change in display configuration and access
// invalid or wrong display handles --> all kind of syncing problems and weird bugs...
void InitCGDisplayIDList(void)
{
    CGDisplayErr error;
    // MK: This kills syncing in mirrored-setups: error = CGGetActiveDisplayList(kPsychMaxPossibleDisplays, displayCGIDs, &numDisplays);
    // Therefore we query the list of displays that are "online" - That means: Connected and powered on.
    // Displays that are in power-saving mode (asleep) or not in the ActiveDisplayList,
    // because they are part of a mirror set and therefore not drawable, are still in the online list.
    // We need handles for displays that are in a mirror set in order to send our beampos-queries
    // and CGLFlushDrawable - requests to them -- Otherwise time-stamping and sync of bufferswap
    // to VBL can fail due to syncing to / querying the wrong display in a mirror set...
    // Currently we accept system failure in case of user switching on/off displays during a session...
    // error = CGGetOnlineDisplayList(kPsychMaxPossibleDisplays, displayCGIDs, &numDisplays);
    error = CGGetActiveDisplayList(kPsychMaxPossibleDisplays, displayCGIDs, &numDisplays);
    if(error)
        PsychErrorExitMsg(PsychError_internal, "CGGetActiveDisplayList failed to enumerate displays");
    
    // TESTCODE:
    if (false) {
        unsigned int testvals[kPsychMaxPossibleDisplays*100];    
        int i;
        for (i=0; i<numDisplays*100; i++) {
            testvals[i]=0;
        }
        for (i=0; i<numDisplays*100; i++) {
            testvals[i]=CGDisplayBeamPosition(displayCGIDs[i % numDisplays]);
        }
        
        for (i=0; i<numDisplays*100; i++) {
            mexPrintf("PTB TESTCODE : Display %i : beampos %i\n", i % numDisplays, testvals[i]);
        }        
    }
}


void PsychGetCGDisplayIDFromScreenNumber(CGDirectDisplayID *displayID, int screenNumber)
{
    if(screenNumber>=numDisplays)
        PsychErrorExit(PsychError_invalidScumber);
    // MK: We return the id of the primary display of the hardware-mirror set to which
    // the display for 'screenNumber' belongs to. This will be the same display on
    // single display setups. On dual-display setups, it will return the ID of the
    // display we are really syncing in Screen('Flip'). This is important for querying
    // the rasterbeam position of the correct display in such setups.
    //
    // I'm not sure if this is the best place for performing this lookup, but
    // at least it should be safe to do here...
    // Old: Fails on some dual-display setups - *displayID=displayCGIDs[screenNumber];
    *displayID=CGDisplayPrimaryDisplay(displayCGIDs[screenNumber]);
    // Protection against Apples stupidity... - our our's if we made false assumptions ;-)
    if (CGDisplayUnitNumber(*displayID)!=CGDisplayUnitNumber(displayCGIDs[screenNumber])) {
        mexPrintf("PTB-DEBUG : ACTIVE DISPLAY <-> PRIMARY DISPLAY MISMATCH FOR SCREEN %i!!!!\n", screenNumber);
    }
}


/*  About locking display settings:

    SCREENOpenWindow and SCREENOpenOffscreenWindow  set the lock when opening  windows and 
    SCREENCloseWindow unsets upon the close of the last of a screen's windows. PsychSetVideoSettings checks for a lock
    before changing the settings.  Anything (SCREENOpenWindow or SCREENResolutions) which attemps to change
    the display settings should report that attempts to change a dipslay's settings are not allowed when its windows are open.
    
    PsychSetVideoSettings() may be called by either SCREENOpenWindow or by Resolutions().  If called by Resolutions it both sets the video settings
    and caches the video settings so that subsequent calls to OpenWindow can use the cached mode regardless of whether interceding calls to OpenWindow
    have changed the display settings or reverted to the virgin display settings by closing.  SCREENOpenWindow should thus invoke the cached mode
    settings if they have been specified and not current actual display settings.  
    
*/    
    


void PsychLockScreenSettings(int screenNumber)
{
    displayLockSettingsFlags[screenNumber]=TRUE;
}

void PsychUnlockScreenSettings(int screenNumber)
{
    displayLockSettingsFlags[screenNumber]=FALSE;
}

boolean PsychCheckScreenSettingsLock(int screenNumber)
{
    return(displayLockSettingsFlags[screenNumber]);
}


/* Because capture and lock will always be used in conjuction, capture calls lock, and SCREENOpenWindow must only call Capture and Release */
void PsychCaptureScreen(int screenNumber)
{
    CGDisplayErr  error;
    
    if(screenNumber>=numDisplays)
        PsychErrorExit(PsychError_invalidScumber);
    error=CGDisplayCapture(displayCGIDs[screenNumber]);
    if(error)
        PsychErrorExitMsg(PsychError_internal, "Unable to capture display");
    PsychLockScreenSettings(screenNumber);
}

/*
    PsychReleaseScreen()
    
*/
void PsychReleaseScreen(int screenNumber)
{	
    CGDisplayErr  error;
    
    if(screenNumber>=numDisplays)
        PsychErrorExit(PsychError_invalidScumber);
    error=CGDisplayRelease(displayCGIDs[screenNumber]);
    if(error)
        PsychErrorExitMsg(PsychError_internal, "Unable to release display");
    PsychUnlockScreenSettings(screenNumber);
}

boolean PsychIsScreenCaptured(screenNumber)
{
    return(PsychCheckScreenSettingsLock(screenNumber));
}    


//Read display parameters.
/*
    PsychGetNumDisplays()
    Get the number of video displays connected to the system.
*/

int PsychGetNumDisplays(void)
{
    return((int)numDisplays);
}

void PsychGetScreenDepths(int screenNumber, PsychDepthType *depths)
{
    CFDictionaryRef currentMode, tempMode;
    CFArrayRef modeList;
    CFNumberRef n;
    int i, numPossibleModes;
    long currentWidth, currentHeight, tempWidth, tempHeight, currentFrequency, tempFrequency, tempDepth;
    
    
    if(screenNumber>=numDisplays)
        PsychErrorExit(PsychError_invalidScumber); //also checked within SCREENPixelSizes
     
    //Get the current display mode.  We will want to match against width and hz when looking for available depths. 
    currentMode = CGDisplayCurrentMode(displayCGIDs[screenNumber]);
    n=CFDictionaryGetValue(currentMode, kCGDisplayWidth);
    CFNumberGetValue(n,kCFNumberLongType, &currentWidth);
    n=CFDictionaryGetValue(currentMode, kCGDisplayHeight);
    CFNumberGetValue(n,kCFNumberLongType, &currentHeight);
    n=CFDictionaryGetValue(currentMode, kCGDisplayRefreshRate );
    CFNumberGetValue(n, kCFNumberLongType, &currentFrequency ) ;

    //get a list of avialable modes for the specified display
    modeList = CGDisplayAvailableModes(displayCGIDs[screenNumber]);
    numPossibleModes= CFArrayGetCount(modeList);
    for(i=0;i<numPossibleModes;i++){
        tempMode = CFArrayGetValueAtIndex(modeList,i);
        n=CFDictionaryGetValue(tempMode, kCGDisplayWidth);
        CFNumberGetValue(n,kCFNumberLongType, &tempWidth);
        n=CFDictionaryGetValue(tempMode, kCGDisplayHeight);
        CFNumberGetValue(n,kCFNumberLongType, &tempHeight);
        n=CFDictionaryGetValue(tempMode, kCGDisplayRefreshRate);
        CFNumberGetValue(n, kCFNumberLongType, &tempFrequency) ;
        if(currentWidth==tempWidth && currentHeight==tempHeight && currentFrequency==tempFrequency){
            n=CFDictionaryGetValue(tempMode, kCGDisplayBitsPerPixel);
            CFNumberGetValue(n, kCFNumberLongType, &tempDepth) ;
            PsychAddValueToDepthStruct((int)tempDepth, depths);
        }
		// printf("mode %i : w x h = %i x %i, fps = %i, depths = %i\n", i, tempWidth, tempHeight, tempFrequency, tempDepth);
    }

}


/*
    static PsychGetCGModeFromVideoSettings()
   
*/
boolean PsychGetCGModeFromVideoSetting(CFDictionaryRef *cgMode, PsychScreenSettingsType *setting)
{
    CFArrayRef modeList;
    CFNumberRef n;
    int i, numPossibleModes;
    long width, height, depth, frameRate, tempWidth, tempHeight, tempDepth,  tempFrameRate;
    
    if(setting->screenNumber>=numDisplays)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); //also checked within SCREENPixelSizes
        
    //adjust parameter formats
    width=(long)PsychGetWidthFromRect(setting->rect);
    height=(long)PsychGetHeightFromRect(setting->rect);
    depth=(long)PsychGetValueFromDepthStruct(0,&(setting->depth));
    frameRate=(long)setting->nominalFrameRate;

    //get a list of avialable modes for the specified display and iterate over the list looking for our mode.
    modeList = CGDisplayAvailableModes(displayCGIDs[setting->screenNumber]);
    numPossibleModes= CFArrayGetCount(modeList);
    for(i=0;i<numPossibleModes;i++){
        *cgMode = CFArrayGetValueAtIndex(modeList,i);			
        n=CFDictionaryGetValue(*cgMode, kCGDisplayWidth);		//width
        CFNumberGetValue(n,kCFNumberLongType, &tempWidth);
        n=CFDictionaryGetValue(*cgMode, kCGDisplayHeight);		//height
        CFNumberGetValue(n,kCFNumberLongType, &tempHeight);
        n=CFDictionaryGetValue(*cgMode, kCGDisplayRefreshRate);	//frequency
        CFNumberGetValue(n, kCFNumberLongType, &tempFrameRate) ;
        n=CFDictionaryGetValue(*cgMode, kCGDisplayBitsPerPixel);	//depth
        CFNumberGetValue(n, kCFNumberLongType, &tempDepth) ;
        if(width==tempWidth && height==tempHeight && frameRate==tempFrameRate && depth==tempDepth)
            return(TRUE);
    }
    return(FALSE);    
}


/*
    PsychCheckVideoSettings()
    
    Check all available video display modes for the specified screen number and return true if the 
    settings are valid and false otherwise.
*/
boolean PsychCheckVideoSettings(PsychScreenSettingsType *setting)
{
        CFDictionaryRef cgMode;
        
        return(PsychGetCGModeFromVideoSetting(&cgMode, setting));
}



/*
    PsychGetScreenDepth()
    
    The caller must allocate and initialize the depth struct. 
*/
void PsychGetScreenDepth(int screenNumber, PsychDepthType *depth)
{
    
    if(screenNumber>=numDisplays)
        PsychErrorExitMsg(PsychError_internal, "screenNumber is out of range"); //also checked within SCREENPixelSizes
    PsychAddValueToDepthStruct((int)CGDisplayBitsPerPixel(displayCGIDs[screenNumber]),depth);

}

int PsychGetScreenDepthValue(int screenNumber)
{
    PsychDepthType	depthStruct;
    
    PsychInitDepthStruct(&depthStruct);
    PsychGetScreenDepth(screenNumber, &depthStruct);
    return(PsychGetValueFromDepthStruct(0,&depthStruct));
}


float PsychGetNominalFramerate(int screenNumber)
{
    CFDictionaryRef currentMode;
    CFNumberRef n;
    double currentFrequency;
    
    //Get the CG display ID index for the specified display
    if(screenNumber>=numDisplays)
        PsychErrorExitMsg(PsychError_internal, "screenNumber is out of range"); 
    currentMode = CGDisplayCurrentMode(displayCGIDs[screenNumber]);
    n=CFDictionaryGetValue(currentMode, kCGDisplayRefreshRate);
    CFNumberGetValue(n, kCFNumberDoubleType, &currentFrequency);
    return(currentFrequency);
}

void PsychGetScreenSize(int screenNumber, long *width, long *height)
{
    CFDictionaryRef currentMode;
    CFNumberRef n;
    
    if(screenNumber>=numDisplays)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); 
    currentMode = CGDisplayCurrentMode(displayCGIDs[screenNumber]);
    n=CFDictionaryGetValue(currentMode, kCGDisplayWidth);
    CFNumberGetValue(n,kCFNumberLongType, width); 
    n=CFDictionaryGetValue(currentMode, kCGDisplayHeight);
    CFNumberGetValue(n,kCFNumberLongType, height);

}

/* Returns the physical display size as reported by OS-X: */
void PsychGetDisplaySize(int screenNumber, int *width, int *height)
{
    CGSize physSize;
    if(screenNumber>=numDisplays)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetDisplaySize() is out of range");
    physSize = CGDisplayScreenSize(displayCGIDs[screenNumber]);
    *width = (int) physSize.width;
    *height = (int) physSize.height;
}

void PsychGetGlobalScreenRect(int screenNumber, double *rect)
{
	CGDirectDisplayID	displayID;
	CGRect				cgRect;
	double				rLeft, rRight, rTop, rBottom;

    if(screenNumber>=numDisplays)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); 
	displayID=displayCGIDs[screenNumber];
	cgRect=CGDisplayBounds(displayID);
	rLeft=cgRect.origin.x;
	rTop=cgRect.origin.y;
	rRight=cgRect.origin.x + cgRect.size.width;
	rBottom=cgRect.origin.y + cgRect.size.height;
	PsychMakeRect(rect, rLeft, rTop, rRight, rBottom);
	
}


void PsychGetScreenRect(int screenNumber, double *rect)
{
    long width, height; 

    PsychGetScreenSize(screenNumber, &width, &height);
    rect[kPsychLeft]=0;
    rect[kPsychTop]=0;
    rect[kPsychRight]=(int)width;
    rect[kPsychBottom]=(int)height; 
} 


PsychColorModeType PsychGetScreenMode(int screenNumber)
{
    PsychDepthType depth;
        
    PsychInitDepthStruct(&depth);
    PsychGetScreenDepth(screenNumber, &depth);
    return(PsychGetColorModeFromDepthStruct(&depth));
}


/*
    Its probably better to read this directly from the CG renderer info than to infer it from the pixel size
*/	
int PsychGetNumScreenPlanes(int screenNumber)
{
    return((PsychGetScreenDepthValue(screenNumber)>24) ? 4 : 3 );
}



/*
	PsychGetDacBitsFromDisplay()
	
	Return output resolution of video DAC in bits per color component.
	We return a safe default of 8 bpc if we can't query the real value.
*/
int PsychGetDacBitsFromDisplay(int screenNumber)
{
    CGDirectDisplayID	displayID;
	CFMutableDictionaryRef properties;
	CFNumberRef cfGammaWidth;
	SInt32 dacbits;
	io_service_t displayService;
	kern_return_t kr;
	
	// Retrieve display handle for screen:
	if(screenNumber>=numDisplays) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetDacBitsFromDisplay() is out of range"); 
	displayID=displayCGIDs[screenNumber];

	// Retrieve low-level IOKit service port for this display:
	displayService = CGDisplayIOServicePort(displayID);
	// printf("Display 0x%08X with IOServicePort 0x%08X\n", displayID, displayService);
	
	// Obtain the properties from that service
	kr = IORegistryEntryCreateCFProperties(displayService, &properties, NULL, 0);
	if((kr == kIOReturnSuccess) && ((cfGammaWidth = (CFNumberRef) CFDictionaryGetValue(properties, CFSTR(kIOFBGammaWidthKey)))!=NULL))
	{
		CFNumberGetValue(cfGammaWidth, kCFNumberSInt32Type, &dacbits);
		CFRelease(properties);
		return((int) dacbits);
	}
	else {
		// Failed! Return safe 8 bits...
		CFRelease(properties);
		if (PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Failed to query resolution of video DAC for screen %i! Will return safe default of 8 bits.\n", screenNumber);
		return(8);
	}
}



/*
    PsychGetVideoSettings()
    
    Fills a structure describing the screen settings such as x, y, depth, frequency, etc.
    
    Consider inverting the calling sequence so that this function is at the bottom of call hierarchy.  
*/ 
void PsychGetScreenSettings(int screenNumber, PsychScreenSettingsType *settings)
{
    settings->screenNumber=screenNumber;
    PsychGetScreenRect(screenNumber, settings->rect);
    PsychInitDepthStruct(&(settings->depth));
    PsychGetScreenDepth(screenNumber, &(settings->depth));
    settings->mode=PsychGetColorModeFromDepthStruct(&(settings->depth));
    settings->nominalFrameRate=PsychGetNominalFramerate(screenNumber);
	// settings->dacbits=PsychGetDacBitsFromDisplay(screenNumber);
}

//Set display parameters

/*
    PsychSetScreenSettings()
	
	Accept a PsychScreenSettingsType structure holding a video mode and set the display mode accordingly.
    
    If we can not change the display settings because of a lock (set by open window or close window) then return false.
    
    SCREENOpenWindow should capture the display before it sets the video mode.  If it doesn't, then PsychSetVideoSettings will
    detect that and exit with an error.  SCREENClose should uncapture the display. 
    
    The duties of SCREENOpenWindow are:
    -Lock the screen which serves the purpose of preventing changes in video setting with open Windows.
    -Capture the display which gives the application synchronous control of display parameters.
    
    TO DO: for 8-bit palletized mode there is probably more work to do.  
      
*/

boolean PsychSetScreenSettings(boolean cacheSettings, PsychScreenSettingsType *settings)
{
    CFDictionaryRef 		cgMode;
    boolean 			isValid, isCaptured;
    CGDisplayErr 		error;

    //get the display IDs.  Maybe we should consolidate this out of these functions and cache the IDs in a file static
    //variable, since basicially every core graphics function goes through this deal.    
    if(settings->screenNumber>=numDisplays)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); //also checked within SCREENPixelSizes

    //Check for a lock which means onscreen or offscreen windows tied to this screen are currently open.
    if(PsychCheckScreenSettingsLock(settings->screenNumber))
        return(false);  //calling function should issue an error for attempt to change display settings while windows were open.
    
    
    //store the original display mode if this is the first time we have called this function.  The psychtoolbox will disregard changes in 
    //the screen state made through the control panel after the Psychtoolbox was launched. That is, OpenWindow will by default continue to 
    //open windows with finder settings which were in place at the first call of OpenWindow.  That's not intuitive, but not much of a problem
    //either. 
    if(!displayOriginalCGSettingsValid[settings->screenNumber]){
        displayOriginalCGSettings[settings->screenNumber]=CGDisplayCurrentMode(displayCGIDs[settings->screenNumber]);
        displayOriginalCGSettingsValid[settings->screenNumber]=TRUE;
    }
    
    //Find core graphics video settings which correspond to settings as specified withing by an abstracted psychsettings structure.  
    isValid=PsychGetCGModeFromVideoSetting(&cgMode, settings);
    if(!isValid){
        PsychErrorExitMsg(PsychError_internal, "Attempt to set invalid video settings"); 
        //this is an internal error because the caller is expected to check first. 
    }
    
    //If the caller passed cache settings (then it is SCREENResolutions) and we should cache the current video mode settings for this display.  These
    //are cached in the form of CoreGraphics settings and not Psychtoolbox video settings.  The only caller which should pass a set cache flag is 
    //SCREENResolutions
    if(cacheSettings){
        displayOverlayedCGSettings[settings->screenNumber]=cgMode;
        displayOverlayedCGSettingsValid[settings->screenNumber]=TRUE;
    }
    
    //Check to make sure that this display is captured, which OpenWindow should have done.  If it has not been done, then exit with an error.  
    isCaptured=CGDisplayIsCaptured(displayCGIDs[settings->screenNumber]);
    if(!isCaptured)
        PsychErrorExitMsg(PsychError_internal, "Attempt to change video settings without capturing the display");
        
    //Change the display mode.   
    error=CGDisplaySwitchToMode(displayCGIDs[settings->screenNumber], cgMode);
    
    return(true);
}

/*
    PsychRestoreVideoSettings()
    
    Restores video settings to the state set by the finder.  Returns TRUE if the settings can be restored or false if they 
    can not be restored because a lock is in effect, which would mean that there are still open windows.    
    
*/
boolean PsychRestoreScreenSettings(int screenNumber)
{
    boolean 			isCaptured;
    CGDisplayErr 		error;


    if(screenNumber>=numDisplays)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); //also checked within SCREENPixelSizes

    //Check for a lock which means onscreen or offscreen windows tied to this screen are currently open.
    if(PsychCheckScreenSettingsLock(screenNumber))
        return(false);  //calling function will issue error for attempt to change display settings while windows were open.
    
    //Check to make sure that the original graphics settings were cached.  If not, it means that the settings were never changed, so we can just
    //return true. 
    if(!displayOriginalCGSettingsValid[screenNumber])
        return(true);
    
    //Check to make sure that this display is captured, which OpenWindow should have done.  If it has not been done, then exit with an error.  
    isCaptured=CGDisplayIsCaptured(displayCGIDs[screenNumber]);
    if(!isCaptured)
        PsychErrorExitMsg(PsychError_internal, "Attempt to change video settings without capturing the display");
    
    //Change the display mode.   
    error=CGDisplaySwitchToMode(displayCGIDs[screenNumber], displayOriginalCGSettings[kPsychMaxPossibleDisplays]);
    if(error)
        PsychErrorExitMsg(PsychError_internal, "Unable to set switch video modes");
            
    return(true);
}


void PsychHideCursor(int screenNumber)
{

    CGDisplayErr 	error;
    CGDirectDisplayID	cgDisplayID;
    
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);
    error=CGDisplayHideCursor(cgDisplayID);
    if(error)
        PsychErrorExit(PsychError_internal);

}

void PsychShowCursor(int screenNumber)
{

    CGDisplayErr 	error;
    CGDirectDisplayID	cgDisplayID;
    
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);
    error=CGDisplayShowCursor(cgDisplayID);
    if(error)
        PsychErrorExit(PsychError_internal);

}

void PsychPositionCursor(int screenNumber, int x, int y)
{
    CGDisplayErr 	error;
    CGDirectDisplayID	cgDisplayID;
    CGPoint 		point;
    
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);
    point.x=(float)x;
    point.y=(float)y;
    error=CGDisplayMoveCursorToPoint(cgDisplayID, point); 
    if(error)
        PsychErrorExit(PsychError_internal);

}

/*
    PsychReadNormalizedGammaTable()
    
    TO DO: This should probably be changed so that the caller allocates the memory.
    TO DO: Adopt a naming convention which distinguishes between functions which allocate memory for return variables from those which do not.
            For example, PsychReadNormalizedGammaTable() vs. PsychGetNormalizedGammaTable().
    
*/
void PsychReadNormalizedGammaTable(int screenNumber, int *numEntries, float **redTable, float **greenTable, float **blueTable)
{
    CGDirectDisplayID	cgDisplayID;
    static float localRed[1024], localGreen[1024], localBlue[1024];
    CGDisplayErr error; 
    CGTableCount sampleCount; 
        
    *redTable=localRed; *greenTable=localGreen; *blueTable=localBlue; 
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);
    error=CGGetDisplayTransferByTable(cgDisplayID, (CGTableCount)1024, *redTable, *greenTable, *blueTable, &sampleCount);
    *numEntries=(int)sampleCount;
}

void PsychLoadNormalizedGammaTable(int screenNumber, int numEntries, float *redTable, float *greenTable, float *blueTable)
{
    CGDisplayErr 	error; 
    CGDirectDisplayID	cgDisplayID;
    
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);
    error=CGSetDisplayTransferByTable(cgDisplayID, (CGTableCount)numEntries, redTable, greenTable, blueTable);
    
}

    





            


    
    

