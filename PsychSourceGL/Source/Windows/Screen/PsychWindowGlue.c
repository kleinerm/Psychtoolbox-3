/*
	PsychToolbox3/Source/windows/Screen/PsychWindowGlue.c
	
	PLATFORMS:	
	
		This is the Windows  version only.  
				
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

// Application instance handle:
static HINSTANCE hInstance = 0;
// Number of currently open onscreen windows:
static int win32_windowcount = 0;

// Definitions for dynamic binding of VSYNC extension:
typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;

/** PsychRealtimePriority: Temporarily boost priority to highest available priority in M$-Windows.
    PsychRealtimePriority(true) enables realtime-scheduling (like Priority(2) would do in Matlab).
    PsychRealtimePriority(false) restores scheduling to the state before last invocation of PsychRealtimePriority(true),
    it undos whatever the previous switch did.

    We switch to RT scheduling during PsychGetMonitorRefreshInterval() and a few other timing tests in
    PsychOpenWindow() to reduce measurement jitter caused by possible interference of other tasks.
*/
boolean PsychRealtimePriority(boolean enable_realtime)
{
    HANDLE	   currentProcess;
    static boolean old_enable_realtime = FALSE;
    static DWORD   oldPriority = NORMAL_PRIORITY_CLASS;
    const  DWORD   realtime_class = REALTIME_PRIORITY_CLASS;

    if (old_enable_realtime == enable_realtime) {
        // No transition with respect to previous state -> Nothing to do.
        return(true);
    }
    
    // Transition requested:
    old_enable_realtime = enable_realtime;
    
    // Determine our processID:
    currentProcess = GetCurrentProcess();
    
    if (enable_realtime) {
      // Transition to realtime requested:
      
      // Get current scheduling policy and back it up for later restore:
      oldPriority = GetPriorityClass(currentProcess);
        
      // Check if realtime scheduling isn't already active.
      // If we are already in RT mode (e.g., Priority(2) call in Matlab), we skip the switch...
      if (oldPriority != realtime_class) {
	// RT scheduling not yet active -> Switch to it.
	SetPriorityClass(currentProcess, realtime_class);
      }
    }
    else {
      // Transition from RT to whatever-it-was-before scheduling requested: We just reestablish the backed-up old
      // policy: If the old policy wasn't Non-RT, then we don't switch back...
      SetPriorityClass(currentProcess, oldPriority);      
    }
    
    // Success.
    return(TRUE);
}

// Callback handler for Window manager: Handles some events
LONG FAR PASCAL WndProc(HWND hWnd, unsigned uMsg, unsigned wParam, LONG lParam)
{
  static PAINTSTRUCT ps;
  PsychWindowRecordType	**windowRecordArray;
  int i, numWindows; 

  // What event happened?
  switch(uMsg)
    {
    case WM_SYSCOMMAND:
      // System command received: We intercept system commands that would start
      // the screensaver or put the display into powersaving sleep-mode:
      switch(wParam)
	{
	case SC_SCREENSAVE:
	case SC_MONITORPOWER:
	  return(0);
	}
      break;

    case WM_PAINT:
      // Repaint event: This happens if a previously covered non-fullscreen window
      // got uncovered, so part of it needs to be redrawn. PTB's rendering model
      // doesn't have a concept of redrawing a stimulus. As this is mostly useful
      // for debugging, we just do a double doublebuffer swap in the hope that this
      // will restore the frontbuffer...
      BeginPaint(hWnd, &ps);
      EndPaint(hWnd, &ps);
      // Scan the list of windows to find onscreen window with handle hWnd:
      PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);
      for(i = 0; i < numWindows; i++) {
	if (PsychIsOnscreenWindow(windowRecordArray[i]) &&
	    windowRecordArray[i]->targetSpecific.windowHandle == hWnd) {
	  // This is it! Initiate bufferswap twice:
	  PsychOSFlipWindowBuffers(windowRecordArray[i]);
	  PsychOSFlipWindowBuffers(windowRecordArray[i]);
	}
      }
      PsychDestroyVolatileWindowRecordPointerList(windowRecordArray);
      // Done.
      return 0;

    case WM_SIZE:
      // Window resize event: Only happens in debug-mode (non-fullscreen).
      // We resize the viewport accordingly and then trigger a repaint-op.
      glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
      PostMessage(hWnd, WM_PAINT, 0, 0);
      printf("\nPTB-INFO: Onscreen window resized to: %i x %i.\n", (int) LOWORD(lParam), (int) HIWORD(lParam));
      return 0;

    case WM_CLOSE:
      // WM_CLOSE falls through to WM_CHAR and emulates an Abort-key press.
      // -> Manually closing an onscreen window does the same as pressing the Abort-key.
      wParam='@';
      //      return(0);
    case WM_CHAR:
      // Character received. We only care about one key, the '@' key.
      // Pressing '@' will immediately close all onscreen windows, show
      // the cursor and such. It is the emergency stop key.
      if (wParam=='@') {
	// Emergency shutdown:
	printf("\nPTB-INFO: Master-Abort key '@' pressed by user.\n");
	printf("PTB-INFO: Enforcing script abortion and restoring desktop by executing Screen('CloseAll') now!\n");
	printf("PTB-INFO: Please ignore the false error message (INTERNAL PSYCHTOOLBOX ERROR) caused by this...\n");
	ScreenCloseAllWindows();
	return(0);
      }
      break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

boolean ChangeScreenResolution (int width, int height, int bitsPerPixel, int fps)	// Change The Screen Resolution
{
  DEVMODE dmScreenSettings; // Device mode.

  // Clear struct:
  memset (&dmScreenSettings, 0, sizeof (DEVMODE));
  // Init struct with our settings:
  dmScreenSettings.dmSize		= sizeof (DEVMODE);
  dmScreenSettings.dmPelsWidth		= width;		// Select Screen Width
  dmScreenSettings.dmPelsHeight		= height;		// Select Screen Height
  dmScreenSettings.dmBitsPerPel		= bitsPerPixel;		// Select Bits Per Pixel
  if (fps>0) {
    dmScreenSettings.dmDisplayFrequency   = fps;                  // Select display refresh rate in Hz
    dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
  }
  else {
    dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
  }

  if (ChangeDisplaySettings (&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
    return FALSE;	// Display Change Failed, Return False
  }
  return TRUE;		// Display Change Was Successful, Return True
}

/*
    PsychOSOpenOnscreenWindow()
    
    Creates the CGL pixel format and the CGL context objects and then instantiates the context onto the screen.
    
    -The pixel format and the context are stored in the target specific field of the window recored.  Close
    should clean up by destroying both the pixel format and the context.
    
    -We mantain the context because it must be be made the current context by drawing functions to draw into 
    the specified window.
    
    -We maintain the pixel format object because there seems to be now way to retrieve that from the context.
    
    -To tell the caller to clean up PsychOSOpenOnscreenWindow returns FALSE if we fail to open the window. It 
    would be better to just issue an PsychErrorExit() and have that clean up everything allocated outside of
    PsychOpenOnscreenWindow().
*/
boolean PsychOSOpenOnscreenWindow(PsychScreenSettingsType *screenSettings, PsychWindowRecordType *windowRecord, int numBuffers, int stereomode, int conserveVRAM)
{
  RECT winRec;
  PsychRectType             screenrect;
  CGDirectDisplayID         cgDisplayID;
  int         pf;
  HDC         hDC;
  HWND        hWnd;
  WNDCLASS    wc;
  PIXELFORMATDESCRIPTOR pfd;
  int x, y, width, height;
  DWORD flags;
  boolean fullscreen = FALSE;
  DWORD windowStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  DWORD windowExtendedStyle = WS_EX_APPWINDOW;

    // Map the logical screen number to a device handle for the corresponding
    // physical display device: CGDirectDisplayID is currently typedef'd to a
    // HDC windows hardware device context handle.
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenSettings->screenNumber);

    // Check if this should be a fullscreen window, and if not, what its dimensions
    // should be:
    PsychGetScreenRect(screenSettings->screenNumber, screenrect);
    if (PsychMatchRect(screenrect, windowRecord->rect)) {
      // This is supposed to be a fullscreen window with the dimensions of
      // the current display/desktop:
      x=0;
      y=0;
      width=PsychGetWidthFromRect(screenrect);
      height=PsychGetHeightFromRect(screenrect);      

      // Switch system to fullscreen-mode:
      fullscreen = ChangeScreenResolution(width, height, 32, 0);
    }
    else {
      // Window size different from current screen size:
      // Extract settings:
      x=windowRecord->rect[kPsychLeft];
      y=windowRecord->rect[kPsychTop];
      width=PsychGetWidthFromRect(windowRecord->rect);
      height=PsychGetHeightFromRect(windowRecord->rect);

      if (FALSE && x==0 && y==0) {
	// Try to see if we can make it a fullscreen window of
	// requested size by switching to different display mode:
	fullscreen = ChangeScreenResolution(width, height, 32, 0);
      }
      else {
	// A regular desktop window with borders and control icons is requested, e.g., for debugging:
	fullscreen = FALSE;
      }      
    }

    if (fullscreen) {
      windowStyle = WS_POPUP;		      // Set The WindowStyle To WS_POPUP (Popup Window)
      windowExtendedStyle |= WS_EX_TOPMOST;   // Set The Extended Window Style To WS_EX_TOPMOST
      // PsychWaitIntervalSeconds(5);
    }

    // Register our own window class for Psychtoolbox onscreen windows:
    // Only register the window class once - use hInstance as a flag.
    if (!hInstance) {
      hInstance = GetModuleHandle(NULL);
      //hInstance = (HINSTANCE) GetWindowLong(GetFocus(), GWL_HINSTANCE);
      wc.style         = CS_OWNDC;
      wc.lpfnWndProc   = WndProc;
      wc.cbClsExtra    = 0;
      wc.cbWndExtra    = 0;
      wc.hInstance     = hInstance;
      wc.hIcon         = LoadIcon(hInstance, IDI_WINLOGO);
      wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
      wc.hbrBackground = NULL;
      wc.lpszMenuName  = NULL;
      wc.lpszClassName = "PTB-OpenGL";

      if (!RegisterClass(&wc)) {
	hInstance = 0;
        printf("\nPTB-ERROR[Register Windowclass failed]: Unknown error, Win32 specific.\n\n");
        return(FALSE);
      }
    }
    //if (PSYCH_DEBUG == PSYCH_ON) printf("Creating Window...\n");

    // Adjust window bounds to account for the window borders if we are in non-fullscreen mode:
    if (!fullscreen) {
      winRec.left=x; winRec.top=y; winRec.right=x+width; winRec.bottom=y+height;
      AdjustWindowRectEx(&winRec, windowStyle, 0, windowExtendedStyle);
      x=winRec.left; y=winRec.top; width=winRec.right - winRec.left; height=winRec.bottom - winRec.top;
    }

    // Window class registered: Create a window of this class with some specific properties:
    hWnd = CreateWindowEx(windowExtendedStyle,
			  "PTB-OpenGL",
			  "PTB Onscreen window",
			  windowStyle,
			  x, y, width, height, NULL, NULL, hInstance, NULL);
    if (hWnd == NULL) {
        printf("\nPTB-ERROR[CreateWindow() failed]: Unknown error, Win32 specific.\n\n");
        return(FALSE);
    }

    //if (PSYCH_DEBUG == PSYCH_ON) printf("Getting Device context...\n");

    // Retrieve device context for the window:
    hDC = GetDC(hWnd);

    // Setup optional flags for pixelformat:
    flags = 0;
    // Init pfd to zero:
    memset(&pfd, 0, sizeof(pfd));

    // Stereo display support: If stereo display output is requested with OpenGL native stereo,
    // we request a stereo-enabled rendering context.
    if(stereomode==kPsychOpenGLStereo) {
      flags = flags | PFD_STEREO;
    }

    // Double buffering requested?
    if(numBuffers>=2) {
      // Enable double-buffering:
      flags = flags | PFD_DOUBLEBUFFER;
      // AUX buffers for Flip-Operations needed?
      if ((conserveVRAM & kPsychDisableAUXBuffers) == 0) {
	// Allocate one or two (mono vs. stereo) AUX buffers for new "don't clear" mode of Screen('Flip'):
	// Not clearing the framebuffer after "Flip" is implemented by storing a backup-copy of
	// the backbuffer to AUXs before flip and restoring the content from AUXs after flip.
	pfd.cAuxBuffers=(stereomode==kPsychOpenGLStereo || stereomode==kPsychCompressedTLBRStereo || stereomode==kPsychCompressedTRBLStereo) ? 2 : 1;
      }
    }

    //if (PSYCH_DEBUG == PSYCH_ON) printf("Device context is %p\n", hDC);

    // Build pixelformat descriptor:
    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SWAP_EXCHANGE |flags;  // Want OpenGL capable window with bufferswap via page-flipping...
    pfd.iPixelType   = PFD_TYPE_RGBA; // Want a RGBA pixel format.
    pfd.cColorBits   = 32;            // 32 bpp at least...
    pfd.cAlphaBits   = 8;             // Want a 8 bit alpha-buffer.

    //if (PSYCH_DEBUG == PSYCH_ON) printf("Choosing pixelformat\n");

    // Create pixelformat:
    pf = ChoosePixelFormat(hDC, &pfd);
    if (pf == 0) {
      ReleaseDC(hDC, hWnd);
      DestroyWindow(hWnd);      
      printf("\nPTB-ERROR[ChoosePixelFormat() failed]: Unknown error, Win32 specific.\n\n");
      return(FALSE);
    }

    //if (PSYCH_DEBUG == PSYCH_ON) printf("Setting pixelformat\n");

    // Set it.
    if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
      ReleaseDC(hDC, hWnd);
      DestroyWindow(hWnd);      

      printf("\nPTB-ERROR[SetPixelFormat() failed]: Unknown error, Win32 specific.\n\n");
      return(FALSE);
    }

    DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    if ((stereomode==kPsychOpenGLStereo) && ((pfd.dwFlags & PFD_STEREO)==0)) {
      // Ooops. Couldn't get the requested stereo-context from hardware :(
      ReleaseDC(hDC, hWnd);
      DestroyWindow(hWnd);

      printf("PTB-ERROR: OpenGL native stereo mode unavailable. Your hardware may not support it,\n"
	     "PTB-ERROR: or at least not on a flat-panel? Expect abortion of your script soon...");

      return(FALSE);
    }

    // Ok, create and attach the rendering context.
    windowRecord->targetSpecific.contextObject = wglCreateContext(hDC);
    if (windowRecord->targetSpecific.contextObject == NULL) {
      ReleaseDC(hDC, hWnd);
      DestroyWindow(hWnd);

      printf("\nPTB-ERROR:[Context creation failed] Unknown, Win32 specific.\n\n");
      return(FALSE);
    }
    
    // Store the handles...
    windowRecord->targetSpecific.windowHandle = hWnd;
    windowRecord->targetSpecific.deviceContext = hDC;

    // Activate the rendering context:
    PsychOSSetGLContext(windowRecord);

    // Finally, show our new window:
    ShowWindow(hWnd, SW_SHOW);

    // Give it higher priority as other applications windows:
    SetForegroundWindow(hWnd);

    // Set the focus on it:
    SetFocus(hWnd);

    // Capture the window if it is a fullscreen one, whatever that means...
    if (fullscreen) SetCapture(hWnd);

    // Increase our own open window counter:
    win32_windowcount++;

    // Some info for the user regarding non-fullscreen and ATI hw:
    if (!fullscreen && (strstr(glGetString(GL_VENDOR), "ATI"))) {
      printf("PTB-INFO: Some ATI graphics cards may not support proper syncing to vertical retrace when\n");
      printf("PTB-INFO: running in windowed mode (non-fullscreen). If PTB aborts with 'Synchronization failure'\n");
      printf("PTB-INFO: you can disable the sync test via call to Screen('Preference', 'SkipSyncTests', 1); .\n");
      printf("PTB-INFO: You won't get proper stimulus onset timestamps though, so windowed mode may be of limited use.\n");
    }

    // Dynamically bind the VSYNC extension:
    if (strstr(glGetString(GL_EXTENSIONS), "WGL_EXT_swap_control")) {
      // Bind it:
      wglSwapIntervalEXT=(PFNWGLEXTSWAPCONTROLPROC) wglGetProcAddress("wglSwapIntervalEXT");
    }
    else {
      wglSwapIntervalEXT = NULL;
      printf("PTB-WARNING: Your graphics driver doesn't allow me to control syncing wrt. vertical retrace!\n");
      printf("PTB-WARNING: Please update your display graphics driver as soon as possible to fix this.\n");
      printf("PTB-WARNING: Until then, you can manually enable syncing to VBL somewhere in the display settings\n");
      printf("PTB-WARNING: tab of your machine.\n");
    }

    // Ok, we should be ready for OS independent setup...

    //printf("\nPTB-INFO: Low-level (Windoze) setup of onscreen window finished!\n");
    //fflush(NULL);

    // Well Done!
    return(TRUE);
}


/*
    PsychOSOpenOffscreenWindow()
    
    Accept specifications for the offscreen window in the platform-neutral structures, convert to native CoreGraphics structures,
    create the surface, allocate a window record and record the window specifications and memory location there.
	
	TO DO:  We need to walk down the screen number and fill in the correct value for the benefit of TexturizeOffscreenWindow
*/
boolean PsychOSOpenOffscreenWindow(double *rect, int depth, PsychWindowRecordType **windowRecord)
{
  /*
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
  */

  // FIXME: Not yet implemented.
  return(FALSE);

}


void PsychOSCloseWindow(PsychWindowRecordType *windowRecord)
{
  // Detach rendering context again - just to be safe!
  wglMakeCurrent(NULL, NULL);

  // Delete rendering context:
  wglDeleteContext(windowRecord->targetSpecific.contextObject);
  windowRecord->targetSpecific.contextObject=NULL;

  // Release device context:
  ReleaseDC(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle);
  windowRecord->targetSpecific.deviceContext=NULL;

  // Delete pixelformat:
  // FIXME Don't know yet how to do this. We leak memory here!

  // Release the capture, whatever that means...
  ReleaseCapture();

  // Restore video settings from the defaults in the Windows registry:
  ChangeDisplaySettings(NULL, CDS_RESET);

  // Close & Destroy the window:
  DestroyWindow(windowRecord->targetSpecific.windowHandle);
  windowRecord->targetSpecific.windowHandle=NULL;

  // Was this the last window?
  win32_windowcount--;

  if (win32_windowcount<=0) {
    win32_windowcount=0;

    // Unregister our window class if it is still registered:
    if (hInstance) {
      UnregisterClass("PTB-OpenGL", hInstance);
      hInstance=NULL;
    }
  }

  // Done.
  return;
}

/*
    PsychOSFlipWindowBuffers()
    
    Performs OS specific double buffer swap call.
*/
void PsychOSFlipWindowBuffers(PsychWindowRecordType *windowRecord)
{
  // Trigger the "Front <-> Back buffer swap (flip) (on next vertical retrace)":
  SwapBuffers(windowRecord->targetSpecific.deviceContext);
}

/* Enable/disable syncing of buffer-swaps to vertical retrace. */
void PsychOSSetVBLSyncLevel(PsychWindowRecordType *windowRecord, int swapInterval)
{
  // Enable rendering context of window:
  PsychSetGLContext(windowRecord);
  // Try to set requested swapInterval if swap-control extension is supported on
  // this windows machine. Otherwise this will be a no-op...
  if (wglSwapIntervalEXT) wglSwapIntervalEXT(swapInterval);
  return;
}

/*
    PsychOSSetGLContext()
    
    Set the window to which GL drawing commands are sent.  
*/
void PsychOSSetGLContext(PsychWindowRecordType *windowRecord)
{
  if (wglGetCurrentContext() != windowRecord->targetSpecific.contextObject) {
    wglMakeCurrent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.contextObject);
  }
}

/*
    PsychOSUnsetGLContext()
    
    Clear the drawing context.  
*/
void PsychOSUnsetGLContext(PsychWindowRecordType* windowRecord)
{
  wglMakeCurrent(windowRecord->targetSpecific.deviceContext, NULL);
}

int CGDisplayBeamPosition(CGDirectDisplayID cgDisplayId)
{
  // FIXME: Don't know how to do this on windows.
  // We return -1 as an indicator to high-level routines that we don't
  // know the rasterbeam position.
  return(-1);
}
