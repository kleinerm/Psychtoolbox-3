/*
	PsychToolbox3/Source/Linux/Screen/PsychScreenGlue.c
	
	PLATFORMS:	
	
		This is the Linux/X11 version only.  
				
	AUTHORS:
	
	Mario Kleiner		mk		mario.kleiner at tuebingen.mpg.de

	HISTORY:
	
	2/20/06                 mk              Wrote it. Derived from Windows version.
        							
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

// Functions for setup and query of hw gamma CLUTS and for monitor refresh rate query:
#include <X11/extensions/xf86vmode.h>

// file local variables

// Maybe use NULLs in the settings arrays to mark entries invalid instead of using boolean flags in a different array.   
static boolean			displayLockSettingsFlags[kPsychMaxPossibleDisplays];
static CFDictionaryRef	        displayOriginalCGSettings[kPsychMaxPossibleDisplays];        	//these track the original video state before the Psychtoolbox changed it.  
static boolean			displayOriginalCGSettingsValid[kPsychMaxPossibleDisplays];
static CFDictionaryRef	        displayOverlayedCGSettings[kPsychMaxPossibleDisplays];        	//these track settings overlayed with 'Resolutions'.  
static boolean			displayOverlayedCGSettingsValid[kPsychMaxPossibleDisplays];
static CGDisplayCount 		numDisplays;

// MK: This doesn't currently exist on X11: static CGDirectDisplayID 	displayCGIDs[kPsychMaxPossibleDisplays];
// X11 has a different - and much more powerful and flexible - concept of displays than OS-X or Windows:
// One can have multiple X11 connections to different logical displays. A logical display corresponds
// to a specific X-Server. This X-Server could run on the same machine as Matlab+PTB or on a different
// machine connected via network somewhere in the building or the world. A single machine can even run
// multiple X-Servers. Anyway, each display itself can consist of multiple screens. Each screen represents
// a single display surface. E.g., a dual-head gfx-adaptor could be driven by a single X-Server and have
// two screens for each physical output. A single X-Server could also drive multiple different gfx-cards
// and therefore have many screens. A Linux render-cluster could consist of multiple independent machines,
// each with multiple screens aka gfx heads connected to each machine (aka X11 display).
//
// Anyway this doesn't currently fit into PTB's representation, so we just have a single CGDirectDisplayId
// statically allocated that connects to the default X11 display, which can be set by the $DISPLAY environment
// variable or as "-display" option for Matlab. The default setting is the local machine on which the user
// is logged in. The screenNumbers correspond to the screen-number of this display.
//
// In the future we could add a new Screen - subfunction that allows to provide a list of X11 displays to
// PTB. PTB would then connect to all these displays, probe for all their screens and build a composite
// list of all screens on all displays...

static CGDirectDisplayID x11_dpy = NULL;

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

void InitCGDisplayIDList(void)
{
  // Build a connection to the default X11 display. The default display is the one
  // provided in the users $DISPLAY environment variable or as "-display" override
  // provided to Matlab. We could specify a specific X11 display instead of the NULL
  // parameter, but PTB currently lacks a proper concept to handle such flexibility.
  x11_dpy = XOpenDisplay(NULL);
  if (x11_dpy == NULL) {
    // We're screwed :(
    PsychErrorExitMsg(PsychError_internal, "FATAL ERROR: Couldn't open default X11 display connection to X-Server!!!");
  }

  // Query number of available screens on this X11 display:
  numDisplays=ScreenCount(x11_dpy);

  return;
}

void PsychGetCGDisplayIDFromScreenNumber(CGDirectDisplayID *displayID, int screenNumber)
{
    if(screenNumber>=numDisplays) PsychErrorExit(PsychError_invalidScumber);
    //    *displayID=displayCGIDs[screenNumber];
    *displayID=x11_dpy;
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

    // MK: We could do this to get exclusive access to the X-Server, but i'm too
    // scared of doing it at the moment:
    // XGrabServer(x11_dpy);

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
    // MK: We could do this to release exclusive access to the X-Server, but i'm too
    // scared of doing it at the moment:
    // XUngrabServer(x11_dpy);

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
    return((int) numDisplays);
}

void PsychGetScreenDepths(int screenNumber, PsychDepthType *depths)
{
    if(screenNumber>=numDisplays)
        PsychErrorExit(PsychError_invalidScumber); //also checked within SCREENPixelSizes

    // MK: FIXME: This just always returns a depth of 32 bits per pixel!
    PsychAddValueToDepthStruct((int) 32, depths);
}


/*
    static PsychGetCGModeFromVideoSettings()
   
*/
boolean PsychGetCGModeFromVideoSetting(CFDictionaryRef *cgMode, PsychScreenSettingsType *setting)
{
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
    //    PsychAddValueToDepthStruct((int) GetDeviceCaps(displayCGIDs[screenNumber], BITSPIXEL), depth);
    // MK: TODO - Implement this...
    PsychAddValueToDepthStruct(32, depth); 
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
  // Information returned by the XF86VidModeExtension:
  XF86VidModeModeLine mode_line;  // The mode line of the current video mode.
  int dot_clock;                  // The RAMDAC / TDMS pixel clock frequency.

  // We start with a default vrefresh of zero, which means "couldn't query refresh from OS":
  static unsigned int vrefresh = 0;

  if(screenNumber>=numDisplays)
    PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); 

  // Query vertical refresh rate. If it fails we default to the last known good value...
  // A problem of our current (int) return type is the limitation to integral numbers (Hz), although real displays can
  // run at non-integral refresh rates on Linux.

  if (XF86VidModeGetModeLine(x11_dpy, screenNumber, &dot_clock, &mode_line)) {
    // Vertical refresh rate is: RAMDAC pixel clock / width of a scanline in clockcylces /
    // number of scanlines per videoframe.
    vrefresh = (((dot_clock * 1000) / mode_line.htotal) * 1000) / mode_line.vtotal;

    // Divide vrefresh by 1000 to get real Hz - value. This will yield a floating-point
    // number with the real refresh rate. As the current interface only allows for integral
    // numbers, we need to round this value to the closest integral number, e.g.,
    // 59.8 Hz --> 60 Hz. or 85.3 Hz --> 85 Hz.
    vrefresh = (int)((((float) vrefresh) / 1000.0f) + 0.5f);
  }

  // Done.
  return(vrefresh);
}

void PsychGetScreenSize(int screenNumber, long *width, long *height)
{
  if(screenNumber>=numDisplays) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); 
  *width=XDisplayWidth(x11_dpy, screenNumber);
  *height=XDisplayHeight(x11_dpy, screenNumber);
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
    // FIXME: Not yet implemented.
    
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
    
    // FIXME: Not yet implemented...
    return(true);
}


void PsychHideCursor(int screenNumber)
{
  // Static "Cursor" object which defines a completely transparent - and therefore invisible
  // X11 cursor for the mouse-pointer.
  static Cursor nullCursor = -1;

  // Check for valid screenNumber:
  if(screenNumber>=numDisplays) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychHideCursor() is out of range"); //also checked within SCREENPixelSizes

  // nullCursor already ready?
  if( nullCursor == (Cursor) -1 ) {
    // Create one:
    Pixmap cursormask;
    XGCValues xgc;
    GC gc;
    XColor dummycolour;

    cursormask = XCreatePixmap(x11_dpy, RootWindow(x11_dpy, screenNumber), 1, 1, 1/*depth*/);
    xgc.function = GXclear;
    gc = XCreateGC(x11_dpy, cursormask, GCFunction, &xgc );
    XFillRectangle(x11_dpy, cursormask, gc, 0, 0, 1, 1 );
    dummycolour.pixel = 0;
    dummycolour.red   = 0;
    dummycolour.flags = 04;
    nullCursor = XCreatePixmapCursor(x11_dpy, cursormask, cursormask, &dummycolour, &dummycolour, 0, 0 );
    XFreePixmap(x11_dpy, cursormask );
    XFreeGC(x11_dpy, gc );
  }

  // Attach nullCursor to our onscreen window:
  XDefineCursor(x11_dpy, RootWindow(x11_dpy, screenNumber), nullCursor );
  XFlush(x11_dpy);

  return;
}

void PsychShowCursor(int screenNumber)
{
  // Check for valid screenNumber:
  if(screenNumber>=numDisplays) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychHideCursor() is out of range"); //also checked within SCREENPixelSizes
  // Reset to default system cursor, which is a visible one.
  XUndefineCursor(x11_dpy, RootWindow(x11_dpy, screenNumber));
  XFlush(x11_dpy);
}

void PsychPositionCursor(int screenNumber, int x, int y)
{
  // Reposition the mouse cursor:
  if (XWarpPointer(x11_dpy, None, RootWindow(x11_dpy, screenNumber), 0, 0, 0, 0, x, y)==BadWindow) {
    PsychErrorExitMsg(PsychError_internal, "Couldn't position the mouse cursor! (XWarpPointer() failed).");
  }
  XFlush(x11_dpy);
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
  
  // The X-Windows hardware LUT has 3 tables for R,G,B, 256 slots each.
  // Each entry is a 16-bit word with the n most significant bits used for an n-bit DAC.
  psych_uint16	RTable[256];
  psych_uint16	GTable[256];
  psych_uint16	BTable[256];
  int     i;        

  // Query OS for gamma table:
  PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);
  XF86VidModeGetGammaRamp(cgDisplayID, screenNumber, 256, (unsigned short*) RTable, (unsigned short*) GTable, (unsigned short*) BTable);

  // Convert tables:Map 16-bit values into 0-1 normalized floats:
  *redTable=localRed; *greenTable=localGreen; *blueTable=localBlue;
  for (i=0; i<256; i++) localRed[i]   = ((float) RTable[i]) / 65535.0f;
  for (i=0; i<256; i++) localGreen[i] = ((float) GTable[i]) / 65535.0f;
  for (i=0; i<256; i++) localBlue[i]  = ((float) BTable[i]) / 65535.0f;

  // The LUT's always have 256 slots for the 8-bit framebuffer:
  *numEntries= 256;
  return;
}

void PsychLoadNormalizedGammaTable(int screenNumber, int numEntries, float *redTable, float *greenTable, float *blueTable)
{
  CGDirectDisplayID	cgDisplayID;
  int     i;        
  psych_uint16	RTable[256];
  psych_uint16	GTable[256];
  psych_uint16	BTable[256];

  // Table must have 256 slots!
  if (numEntries!=256) PsychErrorExitMsg(PsychError_user, "Loadable hardware gamma tables must have 256 slots!");    
  
  // The X-Windows hardware LUT has 3 tables for R,G,B, 256 slots each.
  // Each entry is a 16-bit word with the n most significant bits used for an n-bit DAC.

  // Convert input table to X11 specific gammaTable:
  for (i=0; i<256; i++) RTable[i] = (int)(redTable[i]   * 65535.0f + 0.5f);
  for (i=0; i<256; i++) GTable[i] = (int)(greenTable[i] * 65535.0f + 0.5f);
  for (i=0; i<256; i++) BTable[i] = (int)(blueTable[i]  * 65535.0f + 0.5f);
  
  // Set new gammaTable:
  PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);
  XF86VidModeSetGammaRamp(cgDisplayID, screenNumber, 256, (unsigned short*) RTable, (unsigned short*) GTable, (unsigned short*) BTable);
  return;
}
