/*
	PsychToolbox3/Source/windows/Screen/PsychScreenGlue.c
	
	PLATFORMS:	
	
		This is the Windows version only.  
				
	AUTHORS:
	
	Mario Kleiner		mk		mario.kleiner at tuebingen.mpg.de

	HISTORY:
	
	12/27/05                mk              Wrote it. Derived from Allens OSX version.
        							
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
static boolean			displayLockSettingsFlags[kPsychMaxPossibleDisplays];
static CFDictionaryRef	displayOriginalCGSettings[kPsychMaxPossibleDisplays];        	//these track the original video state before the Psychtoolbox changed it.  
static boolean			displayOriginalCGSettingsValid[kPsychMaxPossibleDisplays];
static CFDictionaryRef	displayOverlayedCGSettings[kPsychMaxPossibleDisplays];        	//these track settings overlayed with 'Resolutions'.  
static boolean			displayOverlayedCGSettingsValid[kPsychMaxPossibleDisplays];
static CGDisplayCount 		numDisplays;
static CGDirectDisplayID 	displayCGIDs[kPsychMaxPossibleDisplays];    
                                                                                                
//file local functions
void InitCGDisplayIDList(void);
void PsychLockScreenSettings(int screenNumber);
void PsychUnlockScreenSettings(int screenNumber);
boolean PsychCheckScreenSettingsLock(int screenNumber);
//boolean PsychGetCGModeFromVideoSetting(CFDictionaryRef *cgMode, PsychScreenSettingsType *setting);

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
  // MK: FIXME Hack create one pseudo-display id to make the system happy.
  // We need to use DDrawEnumerateDevices() (see VideoToolboxPC.h) to really enumerate
  // the available displays. For now we hardcode to one single display, which is the
  // main display with the desktop window:
  numDisplays=1;
  displayCGIDs[0]=GetDC(GetDesktopWindow());
}

void PsychGetCGDisplayIDFromScreenNumber(CGDirectDisplayID *displayID, int screenNumber)
{
    if(screenNumber>=numDisplays) PsychErrorExit(PsychError_invalidScumber);
    *displayID=displayCGIDs[screenNumber];
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
    CGDisplayErr  error=0;
    
    if(screenNumber>=numDisplays) PsychErrorExit(PsychError_invalidScumber);
    // For now, this is a "No operation" on Windows. Don't know if facilities for
    // capturing the screen exist or are necessary at all. There's a SetCapture() call
    // for HWND window handles, but that applies to windows, not screens and i don't
    // know, what it actually does...
    if(error) PsychErrorExitMsg(PsychError_internal, "Unable to capture display");
    PsychLockScreenSettings(screenNumber);
}

/*
    PsychReleaseScreen()    
*/
void PsychReleaseScreen(int screenNumber)
{	
    CGDisplayErr  error=0;
    
    if(screenNumber>=numDisplays) PsychErrorExit(PsychError_invalidScumber);
    // On Windows we restore the original display settings of the to be released screen:
    PsychRestoreScreenSettings(screenNumber);
    if(error) PsychErrorExitMsg(PsychError_internal, "Unable to release display");
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
    /* 

    FIXME This just always returns a depth of 32 bits per pixel!
     
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
    }
    */

    // MK: FIXME: This just always returns a depth of 32 bits per pixel!
    PsychAddValueToDepthStruct((int) 32, depths);
}


/*
    static PsychGetCGModeFromVideoSettings()
   
*/
boolean PsychGetCGModeFromVideoSetting(CFDictionaryRef *cgMode, PsychScreenSettingsType *setting)
{
  /*
    FIXME - We just return a 1.

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

  */

  // Dummy assignment:
  *cgMode = 1;
  return(TRUE);
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
    PsychAddValueToDepthStruct((int) GetDeviceCaps(displayCGIDs[screenNumber], BITSPIXEL), depth);
}

int PsychGetScreenDepthValue(int screenNumber)
{
    PsychDepthType	depthStruct;
    
    PsychInitDepthStruct(&depthStruct);
    PsychGetScreenDepth(screenNumber, &depthStruct);
    return(PsychGetValueFromDepthStruct(0,&depthStruct));
}


int PsychGetNominalFramerate(int screenNumber)
{
  if(screenNumber>=numDisplays)
    PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); 
  return(GetDeviceCaps(displayCGIDs[screenNumber], VREFRESH));
}

void PsychGetScreenSize(int screenNumber, long *width, long *height)
{
  if(screenNumber>=numDisplays) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); 
  *width = GetDeviceCaps(displayCGIDs[screenNumber], HORZRES);
  *height = GetDeviceCaps(displayCGIDs[screenNumber], VERTRES);
}


void PsychGetGlobalScreenRect(int screenNumber, double *rect)
{
  // Create an empty rect:
  PsychMakeRect(rect, 0, 0, 1, 1);
  // Fill it with meaning by PsychGetScreenRect():
  PsychGetScreenRect(screenNumber, rect);
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
    return(PsychGetNumPlanesFromDepthValue(PsychGetScreenDepthValue(screenNumber)));
}



/*
	This is a place holder for a function which uncovers the number of dacbits.  To be filled in at a later date.
	If you know that your card supports >8 then you can fill that in the PsychtPreferences and the psychtoolbox
	will act accordingly.
	
	There seems to be no way to uncover the dacbits programatically.  According to apple CoreGraphics
	sends a 16-bit word and the driver throws out whatever it chooses not to use.
		
	For now we just use 8 to avoid false precision.  
	
	If we can uncover the video card model then  we can implement a table lookup of video card model to number of dacbits.  
*/
int PsychGetDacBitsFromDisplay(int screenNumber)
{
	return(8);
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
    //settings->dacbits=PsychGetDacBits(screenNumber);
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
      displayOriginalCGSettings[settings->screenNumber]= 1; // FIXME!!! CGDisplayCurrentMode(displayCGIDs[settings->screenNumber]);
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
    isCaptured=PsychIsScreenCaptured(settings->screenNumber);
    if(!isCaptured) PsychErrorExitMsg(PsychError_internal, "Attempt to change video settings without capturing the display");
        
    //Change the display mode.   

    
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
    CGDisplayErr 		error=0;


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
    isCaptured=PsychIsScreenCaptured(screenNumber);
    if(!isCaptured) PsychErrorExitMsg(PsychError_internal, "Attempt to change video settings without capturing the display");
    
    // Restore video settings from the defaults in the Windows registry:
    ChangeDisplaySettings(NULL, CDS_RESET);
            
    return(true);
}


void PsychHideCursor(int screenNumber)
{
  // Hide the mouse cursor: We ignore the screenNumber as Windows
  // doesn't allow to set the cursor per screen anyway.
  ShowCursor(FALSE);
  return;
}

void PsychShowCursor(int screenNumber)
{
  // Show the mouse cursor: We ignore the screenNumber as Windows
  // doesn't allow to set the cursor per screen anyway.
  ShowCursor(TRUE);
}

void PsychPositionCursor(int screenNumber, int x, int y)
{
  /*

  FIXME:

  Disabled for now. Not a problem as the SetMouse() Mexfile of the
  old WinPTB does a perfect job in doing this.

    CGDisplayErr 	error;
    CGDirectDisplayID	cgDisplayID;
    CGPoint 		point;
    
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);
    point.x=(float)x;
    point.y=(float)y;
    error=CGDisplayMoveCursorToPoint(cgDisplayID, point); 
    if(error)
        PsychErrorExit(PsychError_internal);
  */
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
    static  float localRed[256], localGreen[256], localBlue[256];

	 // Windows hardware LUT has 3 tables for R,G,B, 256 slots each, concatenated to one table.
    // Each entry is a 16-bit word with the n most significant bits used for an n-bit DAC.
	 psych_uint16	gammaTable[256 * 3]; 
    BOOL    ok;
	 int     i;        

    // Query OS for gamma table:
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);
    ok=GetDeviceGammaRamp(cgDisplayID, &gammaTable);
	 if (!ok) PsychErrorExitMsg(PsychError_internal, "Failed to query the hardware gamma table from graphics adapter!");

	 // Convert concatenated table into three separate tables, map 16-bit values into
    // 0-1 normalized floats
    *redTable=localRed; *greenTable=localGreen; *blueTable=localBlue;
    for (i=0; i<256; i++) localRed[i]   = ((float) gammaTable[i]) / 65535.0f;
    for (i=0; i<256; i++) localGreen[i] = ((float) gammaTable[i+256]) / 65535.0f;
    for (i=0; i<256; i++) localBlue[i]  = ((float) gammaTable[i+512]) / 65535.0f;

	 // The LUT's always have 256 slots for the 8-bit framebuffer:
    *numEntries= 256;
}

void PsychLoadNormalizedGammaTable(int screenNumber, int numEntries, float *redTable, float *greenTable, float *blueTable)
{
    BOOL 	ok; 
    CGDirectDisplayID	cgDisplayID;
	 int     i;        
	 // Windows hardware LUT has 3 tables for R,G,B, 256 slots each, concatenated to one table.
    // Each entry is a 16-bit word with the n most significant bits used for an n-bit DAC.
	 psych_uint16	gammaTable[256 * 3]; 

	 // Table must have 256 slots!
	 if (numEntries!=256) PsychErrorExitMsg(PsychError_user, "Loadable hardware gamma tables must have 256 slots!");    

	 // Convert input table to Windows specific gammaTable:
    for (i=0; i<256; i++) gammaTable[i]     = (int)(redTable[i]   * 65535.0f + 0.5f);
    for (i=0; i<256; i++) gammaTable[i+256] = (int)(greenTable[i] * 65535.0f + 0.5f);
    for (i=0; i<256; i++) gammaTable[i+512] = (int)(blueTable[i]  * 65535.0f + 0.5f);

	 // Set new gammaTable:
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);
    ok=SetDeviceGammaRamp(cgDisplayID, &gammaTable);
	 if (!ok) PsychErrorExitMsg(PsychError_user, "Failed to upload the hardware gamma table into graphics adapter! Read the help for explanation...");
}
