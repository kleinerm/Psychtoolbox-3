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
	
	* DirectDraw objects are not yet destroyed at Screen flush time. This will leak a bit of memory...
*/


#include "Screen.h"
#include <Windows.h>
// We need to define this and include Multimon.h to allow for enumeration of multiple display screens:
#define COMPILE_MULTIMON_STUBS
#include <Multimon.h>

// Include DirectDraw header for access to the GetScanLine() function:
#include <ddraw.h>

// file local variables

// Maybe use NULLs in the settings arrays to mark entries invalid instead of using boolean flags in a different array.   
static boolean			displayLockSettingsFlags[kPsychMaxPossibleDisplays];
static CFDictionaryRef	displayOriginalCGSettings[kPsychMaxPossibleDisplays];        	//these track the original video state before the Psychtoolbox changed it.  
static boolean			displayOriginalCGSettingsValid[kPsychMaxPossibleDisplays];
static CFDictionaryRef	displayOverlayedCGSettings[kPsychMaxPossibleDisplays];        	//these track settings overlayed with 'Resolutions'.  
static boolean			displayOverlayedCGSettingsValid[kPsychMaxPossibleDisplays];
static CGDisplayCount 		numDisplays;
static CGDirectDisplayID 	displayCGIDs[kPsychMaxPossibleDisplays];    
static char*                    displayDeviceName[kPsychMaxPossibleDisplays];   // Windows internal monitor device name. Default display has NULL
static int 	                displayDeviceStartX[kPsychMaxPossibleDisplays]; // Top-Left corner of display on virtual screen. Default display has (0,0).
static int 	                displayDeviceStartY[kPsychMaxPossibleDisplays];
static LPDIRECTDRAW             displayDeviceDDrawObject[kPsychMaxPossibleDisplays]; // Pointer to associated DirectDraw object, if any. NULL otherwise.

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
	displayDeviceDDrawObject[i]=NULL;
    }
    
    //init the list of Core Graphics display IDs.
    InitCGDisplayIDList();
}

// This callback function is called by Windows EnumDisplayMonitors() function for each
// detected display device: We can happily ignore all provided parameters, except for the
// hMonitor struct which contains the Windows internal name for the detected display. We
// need to pass this name string to a variety of Windows-Functions to refer to the monitor
// of interest.
Boolean CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

Boolean CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	MONITORINFOEX moninfo;

	// hMonitor is the handle to the monitor info. Resolve it to a moninfo information struct:
	moninfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &moninfo);

	// Query and copy the display device name into our own screenNumber->Name mapping array:
	displayDeviceName[numDisplays] = (char*) malloc(256);
	strncpy(displayDeviceName[numDisplays], moninfo.szDevice, 256);

	// Query and copy the top-left corner of the monitor:
	displayDeviceStartX[numDisplays] = moninfo.rcMonitor.left;
	displayDeviceStartY[numDisplays] = moninfo.rcMonitor.top;

	// Create a device context for this display and store it in our displayCGIDs array:
	displayCGIDs[numDisplays] = CreateDC(displayDeviceName[numDisplays], displayDeviceName[numDisplays], NULL, NULL);

	// EXPERIMENTAL: Replicate DirectDraw object pointer of primary display device to all display devices.
	// This is not the correct solution for multi-display setups, but at least it allows for basic testing
	// on some multi display setups to find the proper solution. On multi-display setups we disable the new
	// beamposition query mechanism by default, so it is up to the user to take the risk:
	displayDeviceDDrawObject[numDisplays] = displayDeviceDDrawObject[0];

	// Increase global counter of available separate displays:
	numDisplays++;

	// Return and ask system to continue display enumeration:
	return(TRUE);
}

void InitCGDisplayIDList(void)
{
  int i, w1, w2, h1, h2;
  psych_uint32 beampos = 100000;
  HRESULT rc;

  // We always provide the full (virtual) desktop as screen number zero. This way,
  // queries to screen 0 will always provide the global settings and dimensions of
  // the full desktop (either single display, or extended desktop on multi-display systems).
  // Opening an onscreen window on screen 0 will always yield a window covering the full
  // desktop, possibly spanning multiple physical display devices. Very useful for standard
  // dual display stereo applications in stereoMode 4.
  numDisplays=1;
  displayCGIDs[0]=GetDC(GetDesktopWindow());
  displayDeviceName[0] = NULL;
  displayDeviceStartX[0] = 0;
  displayDeviceStartY[0] = 0;

  // EXPERIMENTAL: Create a DirectDraw object for the primary display device, i.e.
  // the single display on a single display setup or the display device corresponding to
  // the desktop on a multi-display setup:
  if (DirectDrawCreate(DDCREATE_HARDWAREONLY, &(displayDeviceDDrawObject[0]), NULL)!=DD_OK) {
    // Failed to create Direct Draw object:
    displayDeviceDDrawObject[0]=NULL;
    printf("PTB-WARNING: Failed to create DirectDraw interface for primary display. Won't be able to generate high-precision 'Flip' timestamps.\n");
  }
  else {
    rc=IDirectDraw_GetScanLine(displayDeviceDDrawObject[0], (LPDWORD) &beampos);
	 if (rc!=DD_OK && rc!=DDERR_VERTICALBLANKINPROGRESS) {
		// Beamposition query failed :(
		switch(rc) {
			case DDERR_UNSUPPORTED:
				printf("PTB-INFO: Beamposition query unsupported on this system configuration.\n");
			break;

			default:
				printf("PTB-INFO: Beamposition query test failed: Direct Draw Error.\n");
		}
    }
  }
  // Now call M$-Windows monitor enumeration routine. It will call our callback-function
  // MonitorEnumProc() for each detected display device...
  EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

  // Only one additional display found?
  if (numDisplays <=2) {
    // Single display mode: Enumeration only found the single monitor that constitutes
    // the desktop on a single display setup, so screen 1 is identical to screen 0.
    // In that case, we release screen 1 again as it would be redundant...
    if (numDisplays == 2) DeleteDC(displayCGIDs[1]);
    numDisplays = 1;
  }
  else {
    // At least two different displays enumerated: This is a multi-display setup.
    // Screen 0 is the full desktop. Screens i=1,2,...,n are display monitors 1 to n.
    // Output some info to the command window:
    printf("PTB-INFO: Multi-display setup in explicit multi-display mode detected. Using the following mapping:\n");
    printf("PTB-INFO: Screen 0 corresponds to the full Windows desktop area. Useful for stereo presentations in stereomode=4 ...\n");
    for (i=1; i<numDisplays; i++) {
      printf("PTB-INFO: Screen %i corresponds to the display area of the monitor with the Windows-internal name %s ...\n", i, displayDeviceName[i]);
    }
    
    // Check for sane display dimensions: Our emulation trick for creating a display spanning screen 0 will only work,
    // if the first two physical displays 1 and 2 are of the same resolution/size and if they are arranged so that both
    // together are suitable for a horizontal desktop spanning window, aka they touch each other at their horizontal borders.
    // If the user has a more exotic display setup, e.g., triple-display or monitors with different resolution, (s)he can still
    // use the [rect] parameter when opening a window on screen 0 to enforce arbitrary onscreen window position and size. We'll
    // always create a borderless window on screen 0 when in multidisplay mode...
    PsychGetScreenSize(1, &w1, &h1);
    PsychGetScreenSize(2, &w2, &h2);
    if (w1!=w2 || h1!=h2) {
		printf("PTB-WARNING: Screens 1 and 2 do have non-equal width and height. This will probably cause wrong behaviour\n");
		printf("PTB-WARNING: when trying to open a fullscreen window on Screen 0 that is supposed to fully cover displays 1 and 2.\n");
		printf("PTB-WARNING: In that case, either change your screen settings to matching size and refresh rate in the display control\n");
		printf("PTB-WARNING: panel and retry after a 'clear all', or manually specify a [rect] parameter for the window in the\n");
		printf("PTB-WARNING: Screen('OpenWindow', 0, [color], [rect]); command to manually enforce creation of a proper onscreen window.\n");
	 }
    
    printf("\n"); fflush(NULL);

    // On a multi-display setup in explicit multi-display mode, we disable beamposition queries by default for now.
    // Users are free to override the default. This behaviour will be changed in the future when multi-display support
    // for DirectDraw has been properly implemented and tested:
    PsychPrefStateSet_VBLTimestampingMode(-1);
  }

  // Ready.
  return;
}

void PsychGetCGDisplayIDFromScreenNumber(CGDirectDisplayID *displayID, int screenNumber)
{
    if(screenNumber>=numDisplays) PsychErrorExit(PsychError_invalidScumber);
    *displayID=displayCGIDs[screenNumber];
}

char* PsychGetDisplayDeviceName(int screenNumber)
{
    if(screenNumber>=numDisplays) PsychErrorExit(PsychError_invalidScumber);
	 return(displayDeviceName[screenNumber]);
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
    int i, rc;
    DEVMODE result;
    
    if(screenNumber>=numDisplays) PsychErrorExit(PsychError_invalidScumber);

    // Query all available modes for this display device and retrieve their
    // depth values:
    i=0;
    do {
        // Query next setting (i) for current (NULL) display:
        result.dmSize = sizeof(DEVMODE);
        result.dmDriverExtra = 0;
        rc = EnumDisplaySettings(PsychGetDisplayDeviceName(screenNumber), i, &result);
        i++;

        // Valid setting returned?
        if (rc) {
            // Yes. Add its depth-value to our depth struct:
            PsychAddValueToDepthStruct((int) result.dmBitsPerPel, depths);
        }
    } while (rc!=0);
    return;    
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

/* Returns the physical display size as reported by Windows: */
void PsychGetDisplaySize(int screenNumber, int *width, int *height)
{
    if(screenNumber>=numDisplays)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetDisplaySize() is out of range");
    *width = (int) GetDeviceCaps(displayCGIDs[screenNumber], HORZSIZE);
    *height = (int) GetDeviceCaps(displayCGIDs[screenNumber], VERTSIZE); 
}

void PsychGetScreenSize(int screenNumber, long *width, long *height)
{
  if(screenNumber>=numDisplays) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); 
  *width = GetDeviceCaps(displayCGIDs[screenNumber], HORZRES);
  *height = GetDeviceCaps(displayCGIDs[screenNumber], VERTRES);
  // If we are in multi-display mode and size of screen 0 (our virtual full desktop) is
  // requested, then we return a size which is twice the desktop width, thereby providing
  // the proper dimensions for a full desktop spanning onscreen window on screen 0.
  if (numDisplays>2 && screenNumber == 0) *width = *width * 2;
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
    rect[kPsychLeft]= (int) displayDeviceStartX[screenNumber];
    rect[kPsychTop]= (int) displayDeviceStartY[screenNumber];
    rect[kPsychRight]=rect[kPsychLeft] + (int) width;
    rect[kPsychBottom]=rect[kPsychTop] + (int) height; 
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
    return((PsychGetScreenDepthValue(screenNumber)>24) ? 4 : 3);
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
	 // FIXME: Not yet implemented. We do this in PsychOSOpenWindow() if necessary for fullscreen-mode...
	 // Would be better to do it here though...
    
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
    ChangeDisplaySettingsEx(PsychGetDisplayDeviceName(screenNumber), NULL, NULL, 0, NULL);
            
    return(true);
}


void PsychHideCursor(int screenNumber)
{
  // Hide the mouse cursor: We ignore the screenNumber as Windows
  // doesn't allow to set the cursor per screen anyway.
  while(ShowCursor(FALSE)>=0);
  return;
}

void PsychShowCursor(int screenNumber)
{
  // Show the mouse cursor: We ignore the screenNumber as Windows
  // doesn't allow to set the cursor per screen anyway.
  while(ShowCursor(TRUE)<0);
}

void PsychPositionCursor(int screenNumber, int x, int y)
{
  // Reposition the mouse cursor: We ignore the screenNumber as Windows
  // doesn't allow to set the cursor per screen anyway.
  if (SetCursorPos(x,y) == 0) PsychErrorExitMsg(PsychError_internal, "Couldn't position the mouse cursor! (SetCursorPos() failed).");
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

	 // Set new gammaTable: On M$-Windows, we retry up to 10 times before giving up, because some
	 // buggy Windows graphics drivers seem to fail on first invocation of SetDeviceGammaRamp(), just
	 // to succeed on a 2nd invocation!
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);
	 ok=FALSE;
    for (i=0; i<10 && !ok; i++) ok=SetDeviceGammaRamp(cgDisplayID, &gammaTable);
	 if (!ok) PsychErrorExitMsg(PsychError_user, "Failed to upload the hardware gamma table into graphics adapter! Read the help for explanation...");
}

// Beamposition queries on Windows are implemented via the DirectDraw-7 interface. It provides
// the GetScanLine() method for beamposition queries, and also functions like WaitForVerticalBlank()
// and VerticalBlankStatus(). This is supposed to work on Windows 2000 and later.
// See http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/ddraw7/directdraw7/ddref_2n5j.asp
//
int CGDisplayBeamPosition(CGDirectDisplayID cgDisplayId)
{
  HRESULT rc;
  psych_uint32 beampos = 0;

  // EXPERIMENTAL: For now this only queries the primary display device and
  // probably only works properly on single-display setups and some multi-setups.
  if(displayDeviceDDrawObject[0]) {
    // We have a Direct draw object: Try to use GetScanLine():
	 rc=IDirectDraw_GetScanLine(displayDeviceDDrawObject[0], (LPDWORD) &beampos);
	 if (rc==DD_OK || rc==DDERR_VERTICALBLANKINPROGRESS) return((int) beampos);
  }

  // Direct Draw unavailable or function unsupported, or hardware
  // doesn't support query under given configuration:
  // We return -1 as an indicator to high-level routines that we don't
  // know the rasterbeam position.
  return(-1);
}

