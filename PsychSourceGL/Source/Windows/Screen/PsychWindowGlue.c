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

#ifndef WS_EX_LAYERED
/* Define prototype for this function: */
BOOL SetLayeredWindowAttributes(      
    HWND hwnd,
    COLORREF crKey,
    BYTE bAlpha,
    DWORD dwFlags);
#endif

#ifdef MATLAB_R11
/* This is the rusty old build system that can't handle Windows 200 only
   headers and their functions. It doesn't support SetLayeredWindowAttributes()
   We define our own dummy "do nothing" implementation here: */
BOOL SetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
{
	printf("PTB-INFO: Sorry, transparent windows unsupported. You need Matlab 7.4 (R2007a) or later for this to work at all.\n");
	return(TRUE);
}

#endif

// Add missing defines from Vista SDK for the DWM, if not already defined:

// Need to enable 1-Byte alignment of structures:
#pragma pack(push,1)

typedef struct UNSIGNED_RATIO {
    UINT32 uiNumerator;
    UINT32 uiDenominator;
} UNSIGNED_RATIO;

typedef ULONGLONG DWM_FRAME_COUNT;
typedef ULONGLONG QPC_TIME;

typedef  struct DWM_TIMING_INFO {
    UINT32          cbSize;

    // Data on DWM composition overall
    
    // Monitor refresh rate
    UNSIGNED_RATIO  rateRefresh;

    // Actual period
    QPC_TIME        qpcRefreshPeriod;

    // composition rate     
    UNSIGNED_RATIO  rateCompose;

    // QPC time at a VSync interupt
    QPC_TIME        qpcVBlank;

    // DWM refresh count of the last vsync
    // DWM refresh count is a 64bit number where zero is
    // the first refresh the DWM woke up to process
    DWM_FRAME_COUNT cRefresh;

    // DX refresh count at the last Vsync Interupt
    // DX refresh count is a 32bit number with zero 
    // being the first refresh after the card was initialized
    // DX increments a counter when ever a VSync ISR is processed
    // It is possible for DX to miss VSyncs
    //
    // There is not a fixed mapping between DX and DWM refresh counts
    // because the DX will rollover and may miss VSync interupts
    UINT cDXRefresh;

    // QPC time at a compose time.  
    QPC_TIME        qpcCompose;

    // Frame number that was composed at qpcCompose
    DWM_FRAME_COUNT cFrame;

    // The present number DX uses to identify renderer frames
    UINT            cDXPresent;

    // Refresh count of the frame that was composed at qpcCompose
    DWM_FRAME_COUNT cRefreshFrame;


    // DWM frame number that was last submitted
    DWM_FRAME_COUNT cFrameSubmitted;

    // DX Present number that was last submitted
    UINT cDXPresentSubmitted;

    // DWM frame number that was last confirmed presented
    DWM_FRAME_COUNT cFrameConfirmed;

    // DX Present number that was last confirmed presented
    UINT cDXPresentConfirmed;

    // The target refresh count of the last
    // frame confirmed completed by the GPU
    DWM_FRAME_COUNT cRefreshConfirmed;

    // DX refresh count when the frame was confirmed presented
    UINT cDXRefreshConfirmed;

    // Number of frames the DWM presented late
    // AKA Glitches
    DWM_FRAME_COUNT          cFramesLate;
    
    // the number of composition frames that 
    // have been issued but not confirmed completed
    UINT          cFramesOutstanding;


    // Following fields are only relavent when an HWND is specified
    // Display frame


    // Last frame displayed
    DWM_FRAME_COUNT cFrameDisplayed;

    // QPC time of the composition pass when the frame was displayed
    QPC_TIME        qpcFrameDisplayed; 

    // Count of the VSync when the frame should have become visible
    DWM_FRAME_COUNT cRefreshFrameDisplayed;

    // Complete frames: DX has notified the DWM that the frame is done rendering

    // ID of the the last frame marked complete (starts at 0)
    DWM_FRAME_COUNT cFrameComplete;

    // QPC time when the last frame was marked complete
    QPC_TIME        qpcFrameComplete;

    // Pending frames:
    // The application has been submitted to DX but not completed by the GPU
 
    // ID of the the last frame marked pending (starts at 0)
    DWM_FRAME_COUNT cFramePending;

    // QPC time when the last frame was marked pending
    QPC_TIME        qpcFramePending;

    // number of unique frames displayed
    DWM_FRAME_COUNT cFramesDisplayed;

    // number of new completed frames that have been received
    DWM_FRAME_COUNT cFramesComplete;

     // number of new frames submitted to DX but not yet complete
    DWM_FRAME_COUNT cFramesPending;

    // number of frames available but not displayed, used or dropped
    DWM_FRAME_COUNT cFramesAvailable;

    // number of rendered frames that were never
    // displayed because composition occured too late
    DWM_FRAME_COUNT cFramesDropped;
    
    // number of times an old frame was composed 
    // when a new frame should have been used
    // but was not available
    DWM_FRAME_COUNT cFramesMissed;
    
    // the refresh at which the next frame is
    // scheduled to be displayed
    DWM_FRAME_COUNT cRefreshNextDisplayed;

    // the refresh at which the next DX present is 
    // scheduled to be displayed
    DWM_FRAME_COUNT cRefreshNextPresented;

    // The total number of refreshes worth of content
    // for this HWND that have been displayed by the DWM
    // since DwmSetPresentParameters was called
    DWM_FRAME_COUNT cRefreshesDisplayed;
	
    // The total number of refreshes worth of content
    // that have been presented by the application
    // since DwmSetPresentParameters was called
    DWM_FRAME_COUNT cRefreshesPresented;


    // The actual refresh # when content for this
    // window started to be displayed
    // it may be different than that requested
    // DwmSetPresentParameters
    DWM_FRAME_COUNT cRefreshStarted;

    // Total number of pixels DX redirected
    // to the DWM.
    // If Queueing is used the full buffer
    // is transfered on each present.
    // If not queuing it is possible only 
    // a dirty region is updated
    ULONGLONG  cPixelsReceived;

    // Total number of pixels drawn.
    // Does not take into account if
    // if the window is only partial drawn
    // do to clipping or dirty rect management 
    ULONGLONG  cPixelsDrawn;

    // The number of buffers in the flipchain
    // that are empty.   An application can 
    // present that number of times and guarantee 
    // it won't be blocked waiting for a buffer to 
    // become empty to present to
    DWM_FRAME_COUNT      cBuffersEmpty;

} DWM_TIMING_INFO;

typedef enum _DWM_SOURCE_FRAME_SAMPLING {
    DWM_SOURCE_FRAME_SAMPLING_POINT = 1,
    DWM_SOURCE_FRAME_SAMPLING_COVERAGE,
    DWM_SOURCE_FRAME_SAMPLING_LAST
} DWM_SOURCE_FRAME_SAMPLING;

typedef struct _DWM_PRESENT_PARAMETERS {
    UINT32 cbSize;
    BOOL fQueue;
    DWM_FRAME_COUNT cRefreshStart;
    UINT cBuffer;
    BOOL fUseSourceRate;
    UNSIGNED_RATIO rateSource;
    UINT cRefreshesPerFrame;
    DWM_SOURCE_FRAME_SAMPLING eSampling;
} DWM_PRESENT_PARAMETERS;

#pragma pack(pop)

// Application instance handle:
static HINSTANCE hInstance = 0;

// Number of currently open onscreen windows:
static int win32_windowcount = 0;

// Mouse button states:
static psych_bool mousebutton_l=FALSE;
static psych_bool mousebutton_m=FALSE;
static psych_bool mousebutton_r=FALSE;

// Module handle for the DWM library 'dwmapi.dll': Or 0 if unsupported.
HMODULE dwmlibrary = 0;

// dwmSupported is true if DWM is supported and library is linked:
static psych_bool dwmSupported = FALSE;

// Store enabled state of DWM: TRUE == Is enabled, FALSE == Is disabled or non-existent:
psych_bool IsDWMEnabled = FALSE;

// DWM function definitions and procpointers:
typedef HRESULT (APIENTRY *DwmIsCompositionEnabledPROC)(BOOL *pfEnabled);
typedef HRESULT (APIENTRY *DwmEnableCompositionPROC)(UINT enable);
typedef HRESULT (APIENTRY *DwmEnableMMCSSPROC)(BOOL fEnableMMCSS);
typedef HRESULT (APIENTRY *DwmGetCompositionTimingInfoPROC)(HWND hwnd, DWM_TIMING_INFO* pTimingInfo);
typedef HRESULT (APIENTRY *DwmSetPresentParametersPROC)(HWND hwnd, DWM_PRESENT_PARAMETERS *pPresentParams);

DwmIsCompositionEnabledPROC			PsychDwmIsCompositionEnabled = NULL;
DwmEnableCompositionPROC			PsychDwmEnableComposition = NULL;
DwmEnableMMCSSPROC					PsychDwmEnableMMCSS = NULL;
DwmGetCompositionTimingInfoPROC		PsychDwmGetCompositionTimingInfo = NULL;
DwmSetPresentParametersPROC			PsychDwmSetPresentParameters = NULL;

// Definitions for dynamic binding of VSYNC extension:
//typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
//PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;

// Definitions for dynamic binding of wglChoosePixelformat extension:
// typedef BOOL (APIENTRY *PFNWGLCHOOSEPIXELFORMATPROC) (HDC,const int*, const FLOAT *, UINT, int*, UINT*);
// PFNWGLCHOOSEPIXELFORMATPROC wglChoosePixelFormatARB = NULL;

/** PsychRealtimePriority: Temporarily boost priority to highest available priority in M$-Windows.
    PsychRealtimePriority(true) enables realtime-scheduling (like Priority(2) would do in Matlab).
    PsychRealtimePriority(false) restores scheduling to the state before last invocation of PsychRealtimePriority(true),
    it undos whatever the previous switch did.

    We switch to RT scheduling during PsychGetMonitorRefreshInterval() and a few other timing tests in
    PsychOpenWindow() to reduce measurement jitter caused by possible interference of other tasks.
*/
psych_bool PsychRealtimePriority(psych_bool enable_realtime)
{
    HANDLE	   currentProcess;
    static psych_bool old_enable_realtime = FALSE;
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
			
			// Check if transition to realtime class worked. If not then retry
			// with high priority class -- better than nothing...
			if (GetPriorityClass(currentProcess)!=realtime_class) {
				if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: Could not enable REALTIME_PRIORITY_CLASS scheduling! Retrying with HIGH_PRIORITY_CLASS...\n");
				SetPriorityClass(currentProcess, HIGH_PRIORITY_CLASS);
				if ((GetPriorityClass(currentProcess)!=HIGH_PRIORITY_CLASS) && (PsychPrefStateGet_Verbosity() > 1)) printf("PTB-WARNING: Could not enable HIGH_PRIORITY_CLASS scheduling. OS malfunction? Timing may be noisy...\n");
			}
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
  int verbosity = PsychPrefStateGet_Verbosity();

  if (verbosity > 6) printf("PTB-DEBUG: WndProc(): Called!\n");
  
  // What event happened?
  switch(uMsg) {
	case WM_MOUSEACTIVATE:
		// Mouseclick into our inactive window (non-fullscreen) received. Eat it:
		if (verbosity > 6) printf("PTB-DEBUG: WndProc(): MOUSE ACTIVATION!\n");
		return(MA_NOACTIVATEANDEAT);
	break;
	
    case WM_SYSCOMMAND:
      // System command received: We intercept system commands that would start
      // the screensaver or put the display into powersaving sleep-mode:
	  if (verbosity > 6) printf("PTB-DEBUG: WndProc(): WM_SYSCOMMAND!\n");
      switch(wParam) {
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
	  		return(0);
		}
    break;
	
	 case WM_LBUTTONDOWN:
		// Left mouse button depressed:
		mousebutton_l = TRUE;
	 break;

	 case WM_LBUTTONUP:
		// Left mouse button released:
		mousebutton_l = FALSE;
	 break;

	 case WM_MBUTTONDOWN:
		// Middle mouse button depressed:
		mousebutton_m = TRUE;
	 break;

	 case WM_MBUTTONUP:
		// Middle mouse button released:
		mousebutton_m = FALSE;
	 break;

	 case WM_RBUTTONDOWN:
		// Right mouse button depressed:
		mousebutton_r = TRUE;
	 break;

	 case WM_RBUTTONUP:
		// Right mouse button released:
		mousebutton_r = FALSE;
	 break;
	 
    case WM_PAINT:
      // Repaint event: This happens if a previously covered non-fullscreen window
      // got uncovered, so part of it needs to be redrawn. PTB's rendering model
      // doesn't have a concept of redrawing a stimulus. As this is mostly useful
      // for debugging, we just do a double doublebuffer swap in the hope that this
      // will restore the frontbuffer...
	  if (verbosity > 6) printf("PTB-DEBUG: WndProc(): WM_PAINT!\n");
      BeginPaint(hWnd, &ps);
      EndPaint(hWnd, &ps);
      // Scan the list of windows to find onscreen window with handle hWnd:
      PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);
      for(i = 0; i < numWindows; i++) {
			if (PsychIsOnscreenWindow(windowRecordArray[i]) &&
	    		 windowRecordArray[i]->targetSpecific.windowHandle == hWnd &&
				 windowRecordArray[i]->stereomode == 0) {
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
	  if (verbosity > 6) printf("PTB-DEBUG: WndProc(): WM_SIZE!\n");

      glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
      PostMessage(hWnd, WM_PAINT, 0, 0);
      // printf("\nPTB-INFO: Onscreen window resized to: %i x %i.\n", (int) LOWORD(lParam), (int) HIWORD(lParam));
      return 0;

    case WM_CLOSE:
      // WM_CLOSE falls through to WM_CHAR and emulates an Abort-key press.
      // -> Manually closing an onscreen window does the same as pressing the Abort-key.
	  if (verbosity > 6) printf("PTB-DEBUG: WndProc(): WM_PAINT!\n");
      wParam='@';
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

/* PsychGetMouseButtonState: Returns current mouse button up-/down state. Called by SCREENGetMouseHelper. */
void PsychGetMouseButtonState(double* buttonArray)
{
    MSG msg;

	// Run our message dispatch loop until we've processed all events in the event queue for our onscreen windows:
	// We need to do this periodically so WindowsXP and later are convinced that our window/application is still
	// alive and responsive. Otherwise it might conclude trouble and replace our onscreen window by a ghost window
	// which is effectively dead and useless for OpenGL rendering and display. See "About Messages and Message Queues"
	// at MSDN: http://msdn.microsoft.com/en-us/library/ms644927(VS.85).aspx for explanation of this braindead
	// mechanism...
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		// Translate event in case it is a keyboard event that needs translation:
		TranslateMessage(&msg);
		
		// Dispatch to the WndProc() function above for final event handling:
		DispatchMessage(&msg);
	}

	// The routine expects a preallocated 3-element double matrix for the three mouse-buttons. If
	// NULL is passed, it doesn't execute the actual mouse query, but only performs the event processing
	// above:
	if (NULL != buttonArray) {
		// GetAsyncKeyState() directly returns the current state of the physical mouse
		// buttons, independent of window system event processing, keyboard or mouse
		// focus and such. Much more robust than old approach:
		buttonArray[0]=(double) ((GetAsyncKeyState(VK_LBUTTON) & -32768) ? 1 : 0);
		buttonArray[1]=(double) ((GetAsyncKeyState(VK_MBUTTON) & -32768) ? 1 : 0);
		buttonArray[2]=(double) ((GetAsyncKeyState(VK_RBUTTON) & -32768) ? 1 : 0);
	}
	
	return;
}

psych_bool ChangeScreenResolution (int screenNumber, int width, int height, int bitsPerPixel, int fps)	// Change The Screen Resolution
{
  DEVMODE dmScreenSettings; // Device mode structure

  // Clear structure:
  memset (&dmScreenSettings, 0, sizeof (DEVMODE));
  dmScreenSettings.dmSize		= sizeof (DEVMODE);
  dmScreenSettings.dmDriverExtra	= 0;

  // Query current display settings and init struct with them:
  EnumDisplaySettings(PsychGetDisplayDeviceName(screenNumber), ENUM_CURRENT_SETTINGS, &dmScreenSettings);

  // Override current settings with the requested settings, if any:
  if (width>0)  dmScreenSettings.dmPelsWidth  = width;  // Select Screen Width
  if (height>0) dmScreenSettings.dmPelsHeight = height; // Select Screen Height
  if (bitsPerPixel>0) dmScreenSettings.dmBitsPerPel = bitsPerPixel; // Select Bits Per Pixel
  if (fps>0) dmScreenSettings.dmDisplayFrequency = fps; // Select display refresh rate in Hz
  
  // All provided values should be honored: We need to spec the refresh explicitely,
  // because otherwise the system will select the lowest fps for a given display mode.
  dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

  // Perform the change:
  if (ChangeDisplaySettingsEx(PsychGetDisplayDeviceName(screenNumber), &dmScreenSettings, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL) {
    return(FALSE);	// Display Change Failed, Return False
  }

  // Display Change Was Successful, Return True
  return(TRUE);
}

/*
    PsychOSOpenOnscreenWindow()
    
    Creates the pixel format and the context objects and then instantiates the context onto the screen.
    
    -The pixel format and the context are stored in the target specific field of the window recored.  Close
    should clean up by destroying both the pixel format and the context.
    
    -We mantain the context because it must be be made the current context by drawing functions to draw into 
    the specified window.
    
    -We maintain the pixel format object because there seems to be now way to retrieve that from the context.
    
    -To tell the caller to clean up PsychOSOpenOnscreenWindow returns FALSE if we fail to open the window. It 
    would be better to just issue an PsychErrorExit() and have that clean up everything allocated outside of
    PsychOpenOnscreenWindow().
*/
psych_bool PsychOSOpenOnscreenWindow(PsychScreenSettingsType *screenSettings, PsychWindowRecordType *windowRecord, int numBuffers, int stereomode, int conserveVRAM)
{
  char winname[100];
  RECT winRec;
  PsychRectType             screenrect;
  CGDirectDisplayID         cgDisplayID;
  int         pf;
  unsigned int nNumFormats;
  HDC         hDC;
  HWND        hWnd;
  WNDCLASS    wc;
  PIXELFORMATDESCRIPTOR pfd;
  int         attribs[58];
  int         attribcount;
  float       fattribs[2]={0,0};
  int x, y, width, height, i, bpc;
  int		  windowLevel;
  GLenum      glerr;
  DWORD		  flags;
  BOOL        compositorEnabled, compositorPostEnabled;
  
  psych_bool fullscreen = FALSE;
  DWORD windowStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  // The WS_EX_NOACTIVATE flag prevents the window from grabbing keyboard focus. That way,
  // the new Java-GetChar can do its job.
  DWORD windowExtendedStyle = WS_EX_APPWINDOW | 0x08000000; // const int WS_EX_NOACTIVATE = 0x08000000;

	 if (PsychPrefStateGet_Verbosity()>6) {
		 printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: Entering Win32 specific window setup...\n");
		 fflush(NULL);
	 }

	// Retrieve windowLevel, an indicator of where non-fullscreen windows should
	// be located wrt. to other windows. 0 = Behind everything else, occluded by
	// everything else. 1 - 999 = At layer windowLevel -> Occludes stuff on layers "below" it.
	// 1000 - 1999 = At highest level, but partially translucent / alpha channel allows to make
	// regions transparent. 2000 or higher: Above everything, fully opaque, occludes everything.
	// 2000 is the default.
	windowLevel = PsychPrefStateGet_WindowShieldingLevel();

	#ifndef MATLAB_R11
		// No op on old R11 builds due to lack of API support.
		// Shielding levels below 1500 will let mouse event through to underlying windows, i.e.,
		// the window is non-existent for the mouse:
		if (windowLevel < 1500) windowExtendedStyle = windowExtendedStyle | WS_EX_TRANSPARENT;
	#endif

	 // Init to safe default:
    windowRecord->targetSpecific.glusercontextObject = NULL;
    
    // Map the logical screen number to a device handle for the corresponding
    // physical display device: CGDirectDisplayID is currently typedef'd to a
    // HDC windows hardware device context handle.
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenSettings->screenNumber);

    // The compositing desktop window manager (DWM) of MS-Vista and later needs special
    // treatment. Under normal operating conditions we want to disable it, because it
    // can severely interfere with display timing. In certain conditions we want to
    // keep it enabled. In any case we need access to the functions for query and setup
    // of DWM state. As we can't know beforehand if we're running on Vista, we need to
    // dynamically load and link the relevant functions:
    
    // First time invocation? hInstance is zero at this point, if so. We only execute
    // the whole dynamic library detection code once for performance reasons:
    if (!hInstance) {
        // First time. Check if we can load the dwmapi.dll:
		if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: Checking for Aero desktop compositor support... "); 
		
        dwmSupported = FALSE;
        dwmlibrary = LoadLibrary("dwmapi.dll");
        if (dwmlibrary) {
            // Load success. Dynamically bind the relevant functions:
			if (PsychPrefStateGet_Verbosity() > 5) printf(" ...Aero desktop compositing window manager available on a Vista or later system. Binding controls ..."); 
            PsychDwmIsCompositionEnabled = (DwmIsCompositionEnabledPROC) GetProcAddress(dwmlibrary, "DwmIsCompositionEnabled");
            PsychDwmEnableComposition    = (DwmEnableCompositionPROC) GetProcAddress(dwmlibrary, "DwmEnableComposition");
            PsychDwmEnableMMCSS          = (DwmEnableMMCSSPROC) GetProcAddress(dwmlibrary, "DwmEnableMMCSS");
            PsychDwmGetCompositionTimingInfo = (DwmGetCompositionTimingInfoPROC) GetProcAddress(dwmlibrary, "DwmGetCompositionTimingInfo");
            PsychDwmSetPresentParameters = (DwmSetPresentParametersPROC) GetProcAddress(dwmlibrary, "DwmSetPresentParameters");
			
            if (PsychDwmIsCompositionEnabled && PsychDwmEnableComposition && PsychDwmEnableMMCSS && PsychDwmGetCompositionTimingInfo && PsychDwmSetPresentParameters) {
                // Mark dwm as supported:
                dwmSupported = TRUE;
				if (PsychPrefStateGet_Verbosity() > 5) printf(" ...done\n"); 
            }
			else {
				FreeLibrary(dwmlibrary);
				dwmlibrary = 0;
				if (PsychPrefStateGet_Verbosity() > 5) printf(" ...FAILED!\n"); 
				if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Could not attach to DWM window manager control library dwmapi.dll - Trouble ahead!!\n"); 
			}
        }
		else {
			if (PsychPrefStateGet_Verbosity() > 5) printf(" ...Aero desktop compositing window manager unsupported. Running on Pre-Vista system.\n"); 
		}
    }
    
    // Check if this should be a fullscreen window:
    PsychGetScreenRect(screenSettings->screenNumber, screenrect);
    if (PsychMatchRect(screenrect, windowRecord->rect) && (windowLevel >= 2000)) {
      // This is supposed to be a fullscreen window with the dimensions of
      // the current display/desktop:
      fullscreen = TRUE;
    }
    else {
      // Window size different from current screen size:
      // A regular desktop window with borders and control icons is requested, e.g., for debugging:
      fullscreen = FALSE;
    }

    // DWM supported?
    if (dwmSupported) {
        // This is Vista, Windows-7, or a later system with DWM compositing window manager.
		
		// Check current enable state:
        if (PsychDwmIsCompositionEnabled(&compositorEnabled)) {
            // Failed to query state: Assume the worst, i.e., compositor on:
            compositorEnabled = TRUE;
            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: PsychOSOpenOnscreenWindow: Failed to query state of Windows desktop compositor! Assuming it is ON!\n");
            }
        }
        else {
            if (PsychPrefStateGet_Verbosity() > 3) {
                printf("PTB-INFO: Aero desktop compositor is currently %s.\n", (compositorEnabled) ? "enabled" : "disabled");
            }
        }
        
        // Should the compositor be enabled?
        // It should always be enabled, unless it is forced off via kPsychDisableAeroWDM conserveVRAM setting,
        if (conserveVRAM & kPsychDisableAeroWDM) {
            // Compositor shall be off:
            compositorEnabled = FALSE;            
            if (PsychPrefStateGet_Verbosity() > 2) {
				printf("PTB-INFO: Will disable Aero desktop compositor on user request, because the kPsychDisableAeroWDM flag was set via a call to Screen('Preference', 'ConserveVRAM').\n");
			}
        }
        else {
            // Compositor shall be on:
            compositorEnabled = TRUE;
        }

        if (PsychPrefStateGet_Verbosity() > 2) {
            printf("PTB-INFO: Will try to %s Windows Aero desktop compositor.\n", (compositorEnabled) ? "enable" : "disable");
        }
        
        // Set new compositor state:
        if (PsychDwmEnableComposition((compositorEnabled) ? 1 : 0)) {
            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: PsychOSOpenOnscreenWindow: Failed to change state of Windows desktop compositor! Expect timing and performance problems!!\n");
            }
        }

		// Retest state:
        if (PsychDwmIsCompositionEnabled(&compositorPostEnabled)) {
            // Failed to query state: Assume the best, i.e., compositor on:
            compositorPostEnabled = TRUE;
            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: PsychOSOpenOnscreenWindow: Failed to query state of Windows desktop compositor! Assuming it is ON!\n");
            }
        }
        else {
            if (PsychPrefStateGet_Verbosity() > 2) {
                printf("PTB-INFO: Aero desktop compositor is now %s.\n", (compositorPostEnabled) ? "enabled" : "disabled");
            }
        }
		
		if (compositorPostEnabled != compositorEnabled) {
            if (PsychPrefStateGet_Verbosity() > 0) {
                printf("PTB-ERROR: PsychOSOpenOnscreenWindow: Windows desktop compositor is not %s as requested!\n", (compositorEnabled) ? "enabled" : "disabled");
                printf("PTB-ERROR: PsychOSOpenOnscreenWindow: EXPECT SERIOUS PROBLEMS WITH VISUAL STIMULUS ONSET TIMING AND TIMESTAMPING!!\n");
            }
		}
        
        // Switch compositor to MMCSS scheduling for good timing, if compositor shall be enabled:
        if (compositorPostEnabled && (PsychDwmEnableMMCSS(compositorPostEnabled))) {
            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: PsychOSOpenOnscreenWindow: Failed to switch Windows desktop compositor to realtime scheduling! Expect timing and performance problems!!\n");
            }
        }
		
		// Store state globally:
		IsDWMEnabled = compositorPostEnabled;
    }
    else {
		// Store globally that DWM is unsupported and therefore off on this system:
		IsDWMEnabled = FALSE;
	}

    // Wanna fullscreen?
    if (fullscreen) {
      // Switch system to fullscreen-mode without changing any settings:
      fullscreen = ChangeScreenResolution(screenSettings->screenNumber, 0, 0, 0, 0);
    }
    
	 // Special case for explicit multi-display setup under Windows when opening a window on
	 // screen zero. We enforce the fullscreen - flag, aka a borderless top level window. This way,
    // if anything of our automatic full-desktop window emulation code goes wrong on exotic setups,
    // the user can still enforce a suitably positioned and sized borderless toplevel window.
    if (PsychGetNumDisplays()>2 && screenSettings->screenNumber == 0) fullscreen = TRUE;

    if (fullscreen) {
      windowStyle = WS_POPUP;		      // Set The WindowStyle To WS_POPUP (Popup Window without borders)
      windowExtendedStyle |= WS_EX_TOPMOST;   // Set The Extended Window Style To WS_EX_TOPMOST
	  
	  // Copy absolute screen location and area of window to 'globalrect',
	  // so functions like Screen('GlobalRect') can still query the real
	  // bounding gox of a window onscreen:
	  PsychGetGlobalScreenRect(screenSettings->screenNumber, windowRecord->globalrect);	  
    }
    else {
      windowStyle |= WS_OVERLAPPEDWINDOW;
	  // Set The Extended Window Style To WS_EX_TOPMOST, ie., this window is in front of all other
	  // windows all the time, unless windowLevel is smaller than 1000:
	  if (windowLevel >= 1000) windowExtendedStyle |= WS_EX_TOPMOST;
	  
	  // If windowLevel is that of a transparent window, then try to enable support for transparent
	  // windows:
	  // Could also define _WIN32_WINNT >= 0x0500
	  #ifndef WS_EX_LAYERED
	  #define WS_EX_LAYERED           0x00080000
	  #endif
	  #ifndef LWA_ALPHA
	  #define LWA_ALPHA               2
	  #endif
	  if ((windowLevel >= 1000) && (windowLevel <  2000)) windowExtendedStyle |= WS_EX_LAYERED;

	  // Copy absolute screen location and area of window to 'globalrect',
	  // so functions like Screen('GlobalRect') can still query the real
	  // bounding gox of a window onscreen:
	  PsychCopyRect(windowRecord->globalrect, windowRecord->rect);	  
    }

    // Define final position and size of window:
    x=windowRecord->rect[kPsychLeft];
    y=windowRecord->rect[kPsychTop];
    width=PsychGetWidthFromRect(windowRecord->rect);
    height=PsychGetHeightFromRect(windowRecord->rect);

    // Assemble windows caption name from window index:
    sprintf(winname, "PTB Onscreen window [%i]:", (int) windowRecord->windowIndex);

	 if (PsychPrefStateGet_Verbosity()>6) {
		 printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: Window parameters computed, display switch to fullscreen done (if needed). Registering window class...\n");
		 fflush(NULL);
	 }

    // Register our own window class for Psychtoolbox onscreen windows:
    // Only register the window class once - use hInstance as a flag.
    if (!hInstance) {
      hInstance = GetModuleHandle(NULL);
      wc.style         = ((windowLevel >= 1000) && (windowLevel <  2000)) ? 0 : CS_OWNDC;
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

	 if (PsychPrefStateGet_Verbosity()>6) {
		 printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: Window class registered - Creating GDI window...\n");
		 fflush(NULL);
	 }

    // Window class registered: Create a window of this class with some specific properties:
    hWnd = CreateWindowEx(windowExtendedStyle,
			  "PTB-OpenGL",
			  winname,
			  windowStyle,
			  x, y, width, height, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
        printf("\nPTB-ERROR[CreateWindow() failed]: Unknown error, Win32 specific.\n\n");
        return(FALSE);
    }

	// Setup transparency level for eligible non-fullscreen windows:
	if (!fullscreen && (windowLevel >= 1000) && (windowLevel < 2000)) {
		// For windowLevels between 1000 and 1999, make the window background transparent, so standard GUI
		// would be visible, wherever nothing is drawn, i.e., where alpha channel is zero.
		// Levels 1000 - 1499 and 1500 to 1999 map to a master opacity level of 0.0 - 1.0:
		SetLayeredWindowAttributes(hWnd, 0, (BYTE) ((((float) (windowLevel % 500)) / 499.0) * 255 + 0.5), LWA_ALPHA);
	}

    // Retrieve device context for the window:
    hDC = GetDC(hWnd);

	 if (PsychPrefStateGet_Verbosity()>6) {
		 printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: Window created - Pixelformat selection...\n");
		 fflush(NULL);
	 }

    // Setup optional flags for pixelformat:
    flags = 0;
    // Init pfd to zero:
    memset(&pfd, 0, sizeof(pfd));
    attribcount = 0;

    attribs[attribcount++]=0x2001; // WGL_DRAW_TO_WINDOW_ARB
    attribs[attribcount++]=GL_TRUE;
    attribs[attribcount++]=0x2010; // WGL_SUPPORT_OPENGL_ARB
    attribs[attribcount++]=GL_TRUE;
    attribs[attribcount++]=0x2007; // WGL_SWAP_METHOD_ARB
    attribs[attribcount++]=0x2028; // WGL_SWAP_EXCHANGE_ARB
    attribs[attribcount++]=0x2013; // WGL_PIXEL_TYPE_ARB
    
    // Select either floating point or fixed point framebuffer:
//    if (windowRecord->depth == 64 || windowRecord->depth == 128) {
//      // Request a floating point drawable instead of a fixed-point one:
//      attribs[attribcount++]=WGL_TYPE_RGBA_FLOAT_ARB;
//    }
//    else {
      // Request standard fixed point drawable:
      attribs[attribcount++]=0x202B; // WGL_TYPE_RGBA_ARB
//    }
    
    // Select requested depth per color component 'bpc' for each channel:
    bpc = 8; // We default to 8 bpc == RGBA8
    if (windowRecord->depth == 30)  { bpc = 10; printf("PTB-INFO: Trying to enable at least 10 bpc fixed point framebuffer.\n"); }
    if (windowRecord->depth == 64)  { bpc = 16; printf("PTB-INFO: Trying to enable 16 bpc fixed point framebuffer.\n"); }
    if (windowRecord->depth == 128) { bpc = 32; printf("PTB-INFO: Trying to enable 32 bpc fixed point framebuffer.\n"); }
    
    // Set up color depth for each channel:
    attribs[attribcount++]=WGL_RED_BITS_ARB;
    attribs[attribcount++]=bpc;
    attribs[attribcount++]=WGL_GREEN_BITS_ARB;
    attribs[attribcount++]=bpc;
    attribs[attribcount++]=WGL_BLUE_BITS_ARB;
    attribs[attribcount++]=bpc;
    attribs[attribcount++]=WGL_ALPHA_BITS_ARB;
    // Alpha channel has only 2 bpc in the fixed point bpc=10 case, i.e. RGBA=8882.
    attribs[attribcount++]=(bpc == 10) ? 2 : bpc;
    
    
    // Stereo display support: If stereo display output is requested with OpenGL native stereo,
    // we request a stereo-enabled rendering context.
    if(stereomode==kPsychOpenGLStereo) {
      flags = flags | PFD_STEREO;
      attribs[attribcount++]=0x2012; // WGL_STEREO_ARB
      attribs[attribcount++]=GL_TRUE;
    }
    
    // Double buffering requested?
    if(numBuffers>=2) {
      // Enable double-buffering:
      flags = flags | PFD_DOUBLEBUFFER;
      attribs[attribcount++]=0x2011; // WGL_DOUBLE_BUFFER_ARB
      attribs[attribcount++]=GL_TRUE;
      
      // AUX buffers for Flip-Operations needed?
      if ((conserveVRAM & kPsychDisableAUXBuffers) == 0) {
	// Allocate one or two (mono vs. stereo) AUX buffers for new "don't clear" mode of Screen('Flip'):
	// Not clearing the framebuffer after "Flip" is implemented by storing a backup-copy of
	// the backbuffer to AUXs before flip and restoring the content from AUXs after flip.
	pfd.cAuxBuffers=(stereomode==kPsychOpenGLStereo || stereomode==kPsychCompressedTLBRStereo || stereomode==kPsychCompressedTRBLStereo) ? 2 : 1;
	attribs[attribcount++]=0x2024; // WGL_AUX_BUFFERS_ARB
	attribs[attribcount++]=pfd.cAuxBuffers;
      }
    }
    
    //if (PSYCH_DEBUG == PSYCH_ON) printf("Device context is %p\n", hDC);
    
    // Build pixelformat descriptor:
    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SWAP_EXCHANGE |flags;  // Want OpenGL capable window with bufferswap via page-flipping...
    pfd.iPixelType   = PFD_TYPE_RGBA; // Want a RGBA pixel format.
    pfd.cColorBits   = 32;            // 32 bpp at least...
    pfd.cAlphaBits   = (bpc == 10) ? 2 : 8;	// Want a 8 bit alpha-buffer, unless R10G10B10A2 pixelformat requested for native 10 bpc support.

    // Support for OpenGL 3D rendering requested?
    if (PsychPrefStateGet_3DGfx()) {
      // Yes. Allocate and attach a 24bit depth buffer and 8 bit stencil buffer:
      pfd.cDepthBits = 24;
      pfd.cStencilBits = 8;
      attribs[attribcount++]=0x2022; // WGL_DEPTH_BITS_ARB
      attribs[attribcount++]=24;
      attribs[attribcount++]=0x2023; // WGL_STENCIL_BITS_ARB
      attribs[attribcount++]=8;
	  // Alloc an accumulation buffer as well?
	  if (PsychPrefStateGet_3DGfx() & 2) {
		  // Yes: Alloc accum buffer, request 64 bpp, aka 16 bits integer per color component if possible:
		  attribs[attribcount++] = WGL_ACCUM_BITS_EXT;
		  attribs[attribcount++] = 64;
		  attribs[attribcount++] = WGL_ACCUM_RED_BITS_EXT;
		  attribs[attribcount++] = 16;
		  attribs[attribcount++] = WGL_ACCUM_GREEN_BITS_EXT;
		  attribs[attribcount++] = 16;
		  attribs[attribcount++] = WGL_ACCUM_BLUE_BITS_EXT;
		  attribs[attribcount++] = 16;
		  attribs[attribcount++] = WGL_ACCUM_ALPHA_BITS_EXT;
		  attribs[attribcount++] = 16;
		  pfd.cAccumBits      = 64;
		  pfd.cAccumRedBits   = 16;
		  pfd.cAccumGreenBits = 16;
		  pfd.cAccumBlueBits  = 16;
		  pfd.cAccumAlphaBits = 16;
	  }	  
    }
    
    // Multisampled Anti-Aliasing requested?
    if (windowRecord->multiSample > 0) {
      // Request a multisample buffer:
      attribs[attribcount++]= 0x2041; // WGL_SAMPLE_BUFFERS_ARB
      attribs[attribcount++]= 1;
      // Request at least multiSample samples per pixel:
      attribs[attribcount++]= 0x2042; // WGL_SAMPLES_ARB
      attribs[attribcount++]= windowRecord->multiSample;
    }
    
    // Finalize attribs-array:
    attribs[attribcount++]= 0;
    
    //if (PSYCH_DEBUG == PSYCH_ON) printf("Choosing pixelformat\n");
    
    // Create pixelformat:
    // This is typical Microsoft brain-damage: We first need to create a window the
    // conventional old way just to be able to get a handle to the new wglChoosePixelFormat
    // method, which will us - after destroying and recreating the new window - allow to
    // select the pixelformat we actually want!
    
    // Step 1: Choose pixelformat old-style:
    pf = ChoosePixelFormat(hDC, &pfd);
    
    // Do we have a valid pixelformat?
    if (pf == 0) {
      // Nope. We give up!
      ReleaseDC(hDC, hWnd);
      DestroyWindow(hWnd);      
      printf("\nPTB-ERROR[ChoosePixelFormat() failed]: Unknown error, Win32 specific.\n\n");
      return(FALSE);
    }
    
    // Yes. Set it:
    if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
      ReleaseDC(hDC, hWnd);
      DestroyWindow(hWnd);      
      
      printf("\nPTB-ERROR[SetPixelFormat() failed]: Unknown error, Win32 specific.\n\n");
      return(FALSE);
    }
    
	 if (PsychPrefStateGet_Verbosity()>6) {
		 printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: ChoosePixelFormat(), SetPixelFormat() done... Creating OpenGL context...\n");
		 fflush(NULL);
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
    
	 if (PsychPrefStateGet_Verbosity()>6) {
		 printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: Context created - Activating and binding context...\n");
		 fflush(NULL);
	 }

    // Activate the rendering context:
    PsychOSSetGLContext(windowRecord);
    
	 if (PsychPrefStateGet_Verbosity()>6) {
		 printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: Online - glewInit()...\n");
		 fflush(NULL);
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
      if (PsychPrefStateGet_Verbosity()>4) printf("PTB-INFO: Using GLEW version %s for automatic detection of OpenGL extensions...\n", glewGetString(GLEW_VERSION));
    }
    
    DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    
    if ((stereomode==kPsychOpenGLStereo) && ((pfd.dwFlags & PFD_STEREO)==0)) {
      // Ooops. Couldn't get the requested stereo-context from hardware :(

	  // We handle this in higher-level code now, so error-handling is a bit nicer...

      // ReleaseDC(hDC, hWnd);
      // DestroyWindow(hWnd);
      
      printf("PTB-ERROR: OpenGL native stereo mode unavailable. Your hardware may not support it,\n"
	     "PTB-ERROR: or at least not on a flat-panel? Expect abortion of your script soon...");
      
      // return(FALSE);
    }
    
	// Special debug override for faulty drivers with non-working extension:
	if (conserveVRAM & kPsychOverrideWglChoosePixelformat) wglChoosePixelFormatARB = NULL;

	// Step 2: Ok, we have an OpenGL rendering context with all known extensions bound:
	// Do we have (or want) support for wglChoosePixelFormatARB?
   // We skip use of it if we can do without it, i.e., when we don't need unusual framebuffer
   // configs (like non 8bpc fixed) and we don't need multisampling. This is a work-around
   // for hardware that has trouble (=driver bugs) with wglChoosePixelformat() in some modes, e.g., the NVidia
	// Quadro gfx card, which fails to enable quad-buffered stereo when using wglChoosePixelformat(),
	// but does so perfectly well when using the old-style ChoosePixelFormat(). 
	if ((wglChoosePixelFormatARB == NULL) || ((bpc==8) && (windowRecord->multiSample <= 0))) {
	  // Failed (==NULL) or disabled via override.
	  if ((wglChoosePixelFormatARB == NULL) && (PsychPrefStateGet_Verbosity() > 1)) printf("PTB-WARNING: Could not bind wglChoosePixelFormat - Extension. Some features will be unavailable, e.g., Anti-Aliasing and high precision framebuffers.\n");
	}
	else {
	  // Supported. We destroy the rendering context and window, then recreate it with
	  // the wglChoosePixelFormat - method...

	 if (PsychPrefStateGet_Verbosity()>6) {
		 printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: Need to use wglChoosePixelFormat() path due to multisampling or bpc > 8 enabled. Destroy and reinit sequence...\n");
		 fflush(NULL);
	 }

	  wglMakeCurrent(NULL, NULL);

	  // Delete rendering context:
	  wglDeleteContext(windowRecord->targetSpecific.contextObject);
	  windowRecord->targetSpecific.contextObject=NULL;

      // Release device context:
      ReleaseDC(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle);
      windowRecord->targetSpecific.deviceContext=NULL;

   	// Close & Destroy the window:
      DestroyWindow(windowRecord->targetSpecific.windowHandle);
      windowRecord->targetSpecific.windowHandle=NULL;

		// Ok, old window and stuff is dead. Create new window:
    	hWnd = CreateWindowEx(windowExtendedStyle, "PTB-OpenGL", winname, windowStyle,
			  						 x, y, width, height, NULL, NULL, hInstance, NULL);
    	if (hWnd == NULL) {
        printf("\nPTB-ERROR[CreateWindow() - II failed]: Unknown error, Win32 specific.\n\n");
        return(FALSE);
    	}

		// Setup transparency level for eligible non-fullscreen windows:
		if (!fullscreen && (windowLevel >= 1000) && (windowLevel < 2000)) {
			// For windowLevels between 1000 and 1999, make the window background transparent, so standard GUI
			// would be visible, wherever nothing is drawn, i.e., where alpha channel is zero.
			// Levels 1000 - 1499 and 1500 to 1999 map to a master opacity level of 0.0 - 1.0:
			SetLayeredWindowAttributes(hWnd, 0, (BYTE) ((((float) (windowLevel % 500)) / 499.0) * 255 + 0.5), LWA_ALPHA);
		}
		
	   // Retrieve device context for the window:
    	hDC = GetDC(hWnd);

		if (PsychPrefStateGet_Verbosity()>6) {
			printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: Context, pixelformat, window destroyed, window recreated - Now wglChoosePixelformat()...\n");
			fflush(NULL);
		}
		
		pf = 0;
		nNumFormats=0;
		wglChoosePixelFormatARB(hDC, &attribs[0], NULL, 1, &pf, &nNumFormats);
      if (nNumFormats==0 && windowRecord->multiSample > 0) {
		 	// Failed. Probably due to too demanding multisample requirements: Lets lower them...
			for (i=0; i<attribcount && attribs[i]!=0x2042; i++);
			// Reduce requested number of samples/pixel and retry until we get one:
			while (nNumFormats==0 && windowRecord->multiSample > 0) {
				// printf("Failed for multisampling level %i nNum=%i\n", windowRecord->multiSample, nNumFormats);
				attribs[i+1]--;
	  			windowRecord->multiSample--;
				wglChoosePixelFormatARB(hDC, &attribs[0], NULL, 1, &pf, &nNumFormats);
			}
			// If we still do not get one with 0 samples per pixel, we can try to disable
			// multisampling completely:
			if (windowRecord->multiSample == 0 && nNumFormats==0) {
				// printf("Failed for multisampling level %i nNum=%i --> Disabling multisampling...\n", windowRecord->multiSample, nNumFormats);
				// We 0-Out all entries related to multi-sampling, including the
				// GL_SAMPLES_ARB and GL_SAMPLE_BUFFERS_ARB enums themselves:
				for (i=0; i<attribcount && attribs[i]!=0x2042; i++);
	  			attribs[i]=0;
	  			for (i=0; i<attribcount && attribs[i]!=0x2041; i++);
	  			attribs[i]=0;
	  			attribs[i+1]=0;
				wglChoosePixelFormatARB(hDC, &attribs[0], NULL, 1, &pf, &nNumFormats);
			}
      }

		if (nNumFormats==0 && numBuffers>=2) {
			// We still don't have a valid pixelformat, but double-buffering is enabled.
			// Let's try if we get one if we do not request any AUX-Buffers:
			for (i=0; i<attribcount && attribs[i]!=0x2024; i++);
			attribs[i+1] = 0; // Zero AUX-Buffers requested.
			wglChoosePixelFormatARB(hDC, &attribs[0], NULL, 1, &pf, &nNumFormats);
		}

		if (nNumFormats==0 && numBuffers>=2) {
			// We still don't have a valid pixelformat, but double-buffering is enabled.
			// Let's try if we get one if we do not request SWAP_EXCHANGED double buffering anymore:
			for (i=0; i<attribcount && attribs[i]!=0x2028; i++);
			attribs[i] = 0x202A; // WGL_SWAP_UNDEFINED_ARB
			wglChoosePixelFormatARB(hDC, &attribs[0], NULL, 1, &pf, &nNumFormats);
		}

		// Either we have a multisampled or non-multisampled format, or none. If we failed,
		// then we can not do anything anymore about it.

	   // Do we have a valid pixelformat?
      if (nNumFormats == 0) {
		   // Nope. We give up!
         ReleaseDC(hDC, hWnd);
         DestroyWindow(hWnd);      
         printf("\nPTB-ERROR[wglChoosePixelFormat() failed]: Unknown error, Win32 specific. Code: %i.\n\n", GetLastError());
         return(FALSE);
      }

      // Yes. Set it:
      if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
         ReleaseDC(hDC, hWnd);
         DestroyWindow(hWnd);      

         printf("\nPTB-ERROR[SetPixelFormat() - II failed]: Unknown error, Win32 specific.\n\n");
         return(FALSE);
      }

		if (PsychPrefStateGet_Verbosity()>6) {
			printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: New pixelformat chosen and set - About to recreate master opengl context...\n");
			fflush(NULL);
		}

      // Ok, create and attach the rendering context.
      windowRecord->targetSpecific.contextObject = wglCreateContext(hDC);
      if (windowRecord->targetSpecific.contextObject == NULL) {
         ReleaseDC(hDC, hWnd);
         DestroyWindow(hWnd);

         printf("\nPTB-ERROR:[Context creation II failed] Unknown, Win32 specific.\n\n");
         return(FALSE);
      }
    
      // Store the handles...
      windowRecord->targetSpecific.windowHandle = hWnd;
      windowRecord->targetSpecific.deviceContext = hDC;

		if (PsychPrefStateGet_Verbosity()>6) {
			printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: Context recreated - Activating...\n");
			fflush(NULL);
		}

      // Activate the rendering context:
      PsychOSSetGLContext(windowRecord);

      DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

      if ((stereomode==kPsychOpenGLStereo) && ((pfd.dwFlags & PFD_STEREO)==0)) {
         // Ooops. Couldn't get the requested stereo-context from hardware :(

		 // We handle this in higher-level code now, so error-handling is a bit nicer...

         //ReleaseDC(hDC, hWnd);
         //DestroyWindow(hWnd);

         printf("PTB-ERROR: OpenGL native stereo mode unavailable. Your hardware may not support it,\n"
	             "PTB-ERROR: or at least not on a flat-panel? Expect abortion of your script soon...");

         //return(FALSE);
      }		
		// Done with final window and OpenGL context setup. We've got our final context enabled.
	 }

	if (PsychPrefStateGet_Verbosity()>6) {
		printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: Mastercontext created, activated and bound - Enabling multisampling if needed...\n");
		fflush(NULL);
	}
	
	 // Enable multisampling if this was requested:
    if (windowRecord->multiSample > 0) glEnable(0x809D); // 0x809D == GL_MULTISAMPLE_ARB
	 // Throw away any error-state this could have created on old hardware...

	 while((glerr = glGetError())!=GL_NO_ERROR) {
		 if (PsychPrefStateGet_Verbosity()>6) {
			 printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: After multisample enable - if any - glGetError reports error: %s..\n", gluErrorString(glerr));
			 fflush(NULL);
		 }		 
	 }

	 if (PsychPrefStateGet_Verbosity()>4) {
		 printf("PTB-DEBUG: Window and master OpenGL context creation finished.\n");
		 fflush(NULL);
	 }
	 
    // External 3D graphics support enabled? Or OpenGL quad-buffered stereo enabled?
	 // For the former, we need this code for OpenGL state isolation. For the latter we
    // need this code as workaround for Windows brain-damage. For some reason it helps
    // to properly shutdown stereo contexts on Windows...
	// One can disable this mechanism via the flag kPsychDisableContextIsolation to account
	// for even more MS brain-damage.
	 if ((!(conserveVRAM & kPsychDisableContextIsolation)) && (PsychPrefStateGet_3DGfx() || stereomode == kPsychOpenGLStereo)) {
		// Yes. We need to create an extra OpenGL rendering context for the external
		// OpenGL code to provide optimal state-isolation. The context shares all
		// heavyweight ressources likes textures, FBOs, VBOs, PBOs, display lists and
		// starts off as an identical copy of PTB's context as of here.
		
		if (PsychPrefStateGet_Verbosity()>4) {
			printf("PTB-DEBUG: Setup of userspace rendering context...\n");
			fflush(NULL);
		}

		windowRecord->targetSpecific.glusercontextObject = wglCreateContext(hDC);
		if (windowRecord->targetSpecific.glusercontextObject == NULL) {
         ReleaseDC(hDC, hWnd);
         DestroyWindow(hWnd);
			printf("\nPTB-ERROR[UserContextCreation failed]: Creating a private OpenGL context for Matlab OpenGL failed for unknown reasons.\n\n");
			return(FALSE);
		}

		if (PsychPrefStateGet_Verbosity()>4) {
			printf("PTB-DEBUG: wglCreateContext() done. Now copying context state...\n");
			fflush(NULL);
		}

		if (PsychPrefStateGet_Verbosity()>6) {
			printf("PTB-DEBUG: Userspace context wglCreateContext() done: glGetString reports %p pointer...\n", glGetString(GL_EXTENSIONS));
			fflush(NULL);
		}
		
		glFinish();
		while((glerr = glGetError())!=GL_NO_ERROR) {
			if (PsychPrefStateGet_Verbosity()>6) {
				printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: Pre-Switch to usercontext glGetError reports error: %s..\n", gluErrorString(glerr));
				fflush(NULL);
			}		 
		}

		// Special debug override for faulty drivers with context sharing setup bugs (NVidia GF8xxx with drivers from Dec. 2007):
		// Ok, we unconditionally disable this code-path, regardless what kPsychUseWindowsContextSharingWorkaround1 is.
		// The wglCopyContext() call isn't needed for proper operation, but causes crashes on recent drivers. By avoiding
		// it completely we don't lose functionality, but avoid noise on the forum about non-working stuff...
/*
		if (!(conserveVRAM & kPsychUseWindowsContextSharingWorkaround1)) {
		
			wglMakeCurrent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.glusercontextObject);
			
			while((glerr = glGetError())!=GL_NO_ERROR) {
				if (PsychPrefStateGet_Verbosity()>6) {
					printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: Post-Switch to usercontext glGetError reports error: %s..\n", gluErrorString(glerr));
					fflush(NULL);
				}		 
			}
			
			if (PsychPrefStateGet_Verbosity()>6) {
				printf("PTB-DEBUG: In Userspace context: glGetString reports %p pointer...\n", glGetString(GL_EXTENSIONS));
				fflush(NULL);
			}
			
			glFinish();
			wglMakeCurrent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.contextObject);
			
			glFinish();
			while((glerr = glGetError())!=GL_NO_ERROR) {
				if (PsychPrefStateGet_Verbosity()>6) {
					printf("PTB-DEBUG: PsychOSOpenOnscreenWindow: Post-Switch to mastercontext glGetError reports error: %s..\n", gluErrorString(glerr));
					fflush(NULL);
				}		 
			}
			
			if (PsychPrefStateGet_Verbosity()>6) {
				printf("PTB-DEBUG: Back in mastercontext: glGetString reports %p pointer...\n", glGetString(GL_EXTENSIONS));
				fflush(NULL);
			}
			
			// Copy full state from our main context:
			if(!wglCopyContext(windowRecord->targetSpecific.contextObject, windowRecord->targetSpecific.glusercontextObject, GL_ALL_ATTRIB_BITS)) {
				// This is ugly, but not fatal...
				if (PsychPrefStateGet_Verbosity()>1) {
					printf("\nPTB-WARNING[wglCopyContext for user context failed]: Copying state to private OpenGL context for Matlab OpenGL failed for unknown reasons.\n\n");
				}
			}
			
			if (PsychPrefStateGet_Verbosity()>6) {
				printf("PTB-DEBUG: After wglCopyContext(): glGetString reports %p pointer...\n", glGetString(GL_EXTENSIONS));
				fflush(NULL);
			}
			
			if (PsychPrefStateGet_Verbosity()>4) {
				printf("PTB-DEBUG: wglCopyContext() done. Now enabling ressource sharing..\n");
				fflush(NULL);
			}
		}
		else {
			if (PsychPrefStateGet_Verbosity()>4) {
				printf("PTB-DEBUG: wglMakeCurrent(usercontext); wglMakeCurrent(mastercontext); wglCopyContext() skipped due to kPsychUseWindowsContextSharingWorkaround1 flag. Now enabling ressource sharing..\n");
				fflush(NULL);
			}
		}
*/
		
	   // Enable ressource sharing with master context for this window:
		if (!wglShareLists(windowRecord->targetSpecific.contextObject, windowRecord->targetSpecific.glusercontextObject)) {
			// This is ugly, but not fatal...
			if (PsychPrefStateGet_Verbosity()>1) {
				printf("\nPTB-WARNING[wglShareLists for user context failed]: Ressource sharing with private OpenGL context for Matlab OpenGL failed for unknown reasons.\n\n");
			}		
		}

		if (PsychPrefStateGet_Verbosity()>6) {
			printf("PTB-DEBUG: After wglShareLists(): glGetString reports %p pointer...\n", glGetString(GL_EXTENSIONS));
			fflush(NULL);
		}

		if (PsychPrefStateGet_Verbosity()>4) {
			printf("PTB-DEBUG: Userspace context setup done..\n");
			fflush(NULL);
		}
	 }

	 glFinish();
	 while((glerr = glGetError())!=GL_NO_ERROR) {
		 if (PsychPrefStateGet_Verbosity()>6) {
			 printf("PTB-DEBUG: Before slaveWindow context sharing: glGetError reports error: %s..\n", gluErrorString(glerr));
			 fflush(NULL);
		 }		 
	 }
	 
	 if (PsychPrefStateGet_Verbosity()>6) {
		 printf("PTB-DEBUG: Before slaveWindow context sharing: glGetString reports %p pointer...\n", glGetString(GL_EXTENSIONS));
		 fflush(NULL);
	 }
	 
	 // Do we have a slaveWindow with which to share all object ressources like display lists, textures, FBO's and shaders?
	 if (windowRecord->slaveWindow) {
		 // Enable ressource sharing with slaveWindow context for this window:
		 if (!wglShareLists(windowRecord->slaveWindow->targetSpecific.contextObject, windowRecord->targetSpecific.contextObject)) {
			 // This is ugly, but not fatal...
			 if (PsychPrefStateGet_Verbosity()>0) {
				 printf("\nPTB-WARNING[wglShareLists for slaveWindow context failed]: Ressource sharing with OpenGL context for slave window failed for unknown reasons. Dual-Window stereo may not work.\n\n");
			 }		
		 }
	 }
	 
	 glFinish();
	 while((glerr = glGetError())!=GL_NO_ERROR) {
		 if (PsychPrefStateGet_Verbosity()>6) {
			 printf("PTB-DEBUG: After slaveWindow context sharing: glGetError reports error: %s..\n", gluErrorString(glerr));
			 fflush(NULL);
		 }		 
	 }
	 
	 if (PsychPrefStateGet_Verbosity()>6) {
		 printf("PTB-DEBUG: After slaveWindow context sharing: glGetString reports %p pointer...\n", glGetString(GL_EXTENSIONS));
		 fflush(NULL);
	 }

	 if (PsychPrefStateGet_Verbosity()>4) {
		printf("PTB-DEBUG: Final low-level window setup: ShowWindow(), SetCapture(), diagnostics...\n");
		fflush(NULL);
	 }

	 if (PsychPrefStateGet_Verbosity()>4) {
		printf("PTB-DEBUG: OpenGL initialization of all master-/slave-/shared-/userspace contexts finished...\n");
		printf("PTB-DEBUG: Final low-level window setup: ShowWindow(), SetCapture(), diagnostics...\n");
		fflush(NULL);
	 }

    // Finally, show our new window:
    ShowWindow(hWnd, SW_SHOW);

	// Following settings enforce the onscreen window being and staying the
	// foreground window with keyboard focus. However they interfere with our
	// Java based GetChar implementation, so use of GetChar and of this feature
	// is mutually exclusive. A special 'Preference'-'ConserveVRAM' flag allows
	// to enable this feature:
	if (conserveVRAM & kPsychEnforceForegroundWindow) {
		// Give it higher priority than other applications windows:
		SetForegroundWindow(hWnd);

		// Set the focus on it:
		SetFocus(hWnd);
	}
	
    // Capture the window if it is a fullscreen one: This window will receive all
    // mouse move and mouse button press events. Important for GetMouse() to work
    // properly...
    if (fullscreen) SetCapture(hWnd);

    // Increase our own open window counter:
    win32_windowcount++;

    // Some info for the user regarding non-fullscreen and ATI hw:
    if ((glGetString(GL_VENDOR)) && !fullscreen && (strstr(glGetString(GL_VENDOR), "ATI"))) {
      printf("PTB-INFO: Some ATI graphics cards may not support proper syncing to vertical retrace when\n");
      printf("PTB-INFO: running in windowed mode (non-fullscreen). If PTB aborts with 'Synchronization failure'\n");
      printf("PTB-INFO: you can disable the sync test via call to Screen('Preference', 'SkipSyncTests', 1); .\n");
      printf("PTB-INFO: You won't get proper stimulus onset timestamps though, so windowed mode may be of limited use.\n");
    }

    // Dynamically bind the VSYNC extension:
    //if (strstr(glGetString(GL_EXTENSIONS), "WGL_EXT_swap_control")) {
      // Bind it:
      // wglSwapIntervalEXT=(PFNWGLEXTSWAPCONTROLPROC) wglGetProcAddress("wglSwapIntervalEXT");
    //}
    //else {
	 if (wglSwapIntervalEXT == NULL) {
      wglSwapIntervalEXT = NULL;
      printf("PTB-WARNING: Your graphics driver doesn't allow me to control syncing wrt. vertical retrace!\n");
      printf("PTB-WARNING: Please update your display graphics driver as soon as possible to fix this.\n");
      printf("PTB-WARNING: Until then, you can manually enable syncing to VBL somewhere in the display settings\n");
      printf("PTB-WARNING: tab of your machine.\n");
    }

	// Enforce a one-shot GUI event queue dispatch via this dummy call to PsychGetMouseButtonState() to
	// make windows GUI event processing happy:
	PsychGetMouseButtonState(NULL);

    // Ok, we should be ready for OS independent setup...
	 if (PsychPrefStateGet_Verbosity()>4) {
		 printf("PTB-DEBUG: Final low-level window setup finished. Continuing with OS-independent setup.\n");
		 fflush(NULL);
	 }

    // Well Done!
    return(TRUE);
}


/*
    PsychOSOpenOffscreenWindow()
    
    Accept specifications for the offscreen window in the platform-neutral structures, convert to native CoreGraphics structures,
    create the surface, allocate a window record and record the window specifications and memory location there.
	
	TO DO:  We need to walk down the screen number and fill in the correct value for the benefit of TexturizeOffscreenWindow
*/
psych_bool PsychOSOpenOffscreenWindow(double *rect, int depth, PsychWindowRecordType **windowRecord)
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

  // Delete userspace context:
  if (windowRecord->targetSpecific.glusercontextObject) {
		wglDeleteContext(windowRecord->targetSpecific.glusercontextObject);
  		windowRecord->targetSpecific.glusercontextObject = NULL;
  }

  // Release device context:
  ReleaseDC(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle);
  windowRecord->targetSpecific.deviceContext=NULL;

  // Release the capture, whatever that means...
  ReleaseCapture();

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
      
      // Free dwmapi.dll if loaded:
      if (dwmSupported && dwmlibrary) {
          FreeLibrary(dwmlibrary);
          dwmlibrary = 0;
          dwmSupported = FALSE;
      }
    }
  }

  // Restore video settings from the defaults in the Windows registry:
  ChangeDisplaySettings(NULL, 0);

  // Done.
  return;
}

/*
    PsychOSGetVBLTimeAndCount()

    Returns absolute system time of last VBL and current total count of VBL interrupts since
    startup of gfx-system for the given screen. Returns a time of -1 and a count of 0 if this
    feature is unavailable on the given OS/Hardware configuration.
*/
double  PsychOSGetVBLTimeAndCount(PsychWindowRecordType *windowRecord, psych_uint64* vblCount)
{
	DWM_TIMING_INFO	dwmtiming;	
	psych_uint64 ust, msc, sbc;
	CGDirectDisplayID displayID;
	HRESULT rc1 = 0;
	HRESULT rc2 = 0;
	unsigned int screenid = windowRecord->screenNumber;

	// Retrieve displayID, aka HDC for this screenid:
	PsychGetCGDisplayIDFromScreenNumber(&displayID, screenid);

	// Windows Vista DWM available, supported and enabled?
	dwmtiming.cbSize = sizeof(dwmtiming);
	
	if ( IsDWMEnabled && (NULL != PsychDwmGetCompositionTimingInfo) && (
		((rc1 = PsychDwmGetCompositionTimingInfo(windowRecord->targetSpecific.windowHandle, &dwmtiming)) == 0) ||
		((rc2 = PsychDwmGetCompositionTimingInfo(NULL, &dwmtiming)) == 0)
		)) {
		// Yes. Supported, enabled, and we got timing info from it. Extract:
		
		// VBLCount of last VBL:
		// *vblCount = (psych_uint64) dwmtiming.cDXRefresh;
		*vblCount = (psych_uint64) dwmtiming.cRefresh;
		
		// VBLTime of last VBL in QPC, ie., as query performance counter 64-bit psych_uint64 value:
		ust = (psych_uint64) dwmtiming.qpcVBlank;

		if (PsychPrefStateGet_Verbosity() > 15) {
			 printf("PTB-DEBUG: VBLCount = %i  :  VBLTime = %d ticks. ClockHz = %f\n", (int) *vblCount, ust, PsychGetKernelTimebaseFrequencyHz());
		}

		// Convert ust into regular GetSecs timestamp:
		return(PsychMapPrecisionTimerTicksToSeconds(ust));
	}
	else {
		 if (IsDWMEnabled && PsychPrefStateGet_Verbosity()>6) {
			 printf("PTB-DEBUG: Call to PsychDwmGetCompositionTimingInfo(%i) failed with rc1 = %x, rc2 = %x, GetLastError() = %i\n", dwmtiming.cbSize, rc1, rc2, GetLastError());
		}
	}

	// DWM unsupported, unavailable, disabled or failed to query if we reach this point.
	// Let's try if we have more luck with OpenML support...
	
	// Ok, this will return VBL count and last VBL time via the OML GetSyncValuesOML call
	// if that extension is supported on this setup. As of mid 2009 i'm not aware of any
	// affordable graphics card that would support this extension, but who knows??
	if ((NULL != wglGetSyncValuesOML) && (wglGetSyncValuesOML((HDC) displayID, (INT64*) &ust, (INT64*) &msc, (INT64*) &sbc))) {
		*vblCount = msc;
		
		// Convert ust into regular GetSecs timestamp:
		return(PsychMapPrecisionTimerTicksToSeconds(ust));
	}
	else {
		// Unsupported on Windows so far :(
		*vblCount = 0;
		return(-1);
	}
}

/*
	PsychOSGetPresentationTimingInfo()

	Retrieve low-level counts and timestamps related to stimulus onset
	for the last presented frame via PsychOSFlipWindowBuffers().
	
	Returns true on success, false on error.

*/
psych_bool PsychOSGetPresentationTimingInfo(PsychWindowRecordType *windowRecord, psych_bool postSwap, unsigned int flags, psych_uint64* onsetVBLCount, double* onsetVBLTime, psych_uint64* frameId, double* compositionRate)
{
	DWM_TIMING_INFO	dwmtiming;	
	psych_uint64 qpcFrameDisplayed, qpcFrameComplete;
	static double qpcfreq = -1;
	HRESULT rc1 = 0;

	// First time call?
	if (qpcfreq == -1) {
		// Query and assign QPC frequency: This just to make sure that our QPC timebase is working correctly:
		qpcfreq = PsychGetKernelTimebaseFrequencyHz();

		if (PsychPrefStateGet_Verbosity() > 15) {
			 printf("PTB-DEBUG: Initial ClockHz = %f : Health = %i\n", qpcfreq, PsychGetTimeBaseHealthiness());
		}
		
		// Reliable?
		if ((PsychGetTimeBaseHealthiness() & 1) || (qpcfreq < 10000)) {
			// QPC doesn't work correctly. We disable ourselves:
			qpcfreq = 0;
		}

		if (PsychPrefStateGet_Verbosity() > 15) {
			 printf("PTB-DEBUG: Final ClockHz = %f\n", qpcfreq);
		}
	}
	
	// Fail if qpcfreq == 0 ie. timing trouble:
	if (qpcfreq == 0) return(FALSE);

	// Windows Vista DWM available, supported and enabled?
	dwmtiming.cbSize = sizeof(dwmtiming);
	if ( IsDWMEnabled && ((rc1 = PsychDwmGetCompositionTimingInfo(windowRecord->targetSpecific.windowHandle, &dwmtiming)) == 0) ) {
		// Yes. Supported, enabled, and we got valid timing info from it. Extract:

		// qpcFrameComplete seems to correspond almost perfectly to our concept of stimulus onset time, so use this:
		qpcFrameComplete = (psych_uint64) dwmtiming.qpcFrameComplete;
		
		// Convert to GetSecs() time:
		*onsetVBLTime = PsychMapPrecisionTimerTicksToSeconds(qpcFrameComplete);
		
		// VBL count of stimulus onset:
		*onsetVBLCount = (psych_uint64) dwmtiming.cRefreshFrameDisplayed;
		
		// Assumed frame id: Basically a flip count - Very first bufferswap is id 0, and so on -- A unique
		// serial number for each Screen('Flip') request: This is the id of the most recently completed flip:
		*frameId = (psych_uint64) dwmtiming.cFrameComplete;

		// Current composition rate of the DWM: Ideally at least our video refresh rate.
		*compositionRate = (double) dwmtiming.rateCompose.uiNumerator / (double) dwmtiming.rateCompose.uiDenominator;

		if (PsychPrefStateGet_Verbosity() > 6) {
			printf("PTB-DEBUG: === PsychDwmGetCompositionTimingInfo returned data follows: ===\n\n");
			printf("qpcFrameDisplayed       : %15.6f \n", PsychMapPrecisionTimerTicksToSeconds(dwmtiming.qpcFrameDisplayed));
			printf("qpcRefreshPeriod        : %15.6f \n", PsychMapPrecisionTimerTicksToSeconds(dwmtiming.qpcRefreshPeriod));
			printf("qpcFrameComplete        : %15.6f \n", PsychMapPrecisionTimerTicksToSeconds(dwmtiming.qpcFrameComplete));
			printf("cFrameComplete          : %i \n", (psych_uint64) dwmtiming.cFrameComplete);
			printf("cFramePending           : %i \n", (psych_uint64) dwmtiming.cFramePending);
			printf("qpcFramePending         : %15.6f \n", PsychMapPrecisionTimerTicksToSeconds(dwmtiming.qpcFramePending));
			printf("cRefreshFrameDisplayed  : %i \n", (psych_uint64) dwmtiming.cRefreshFrameDisplayed);
			printf("cRefreshStarted         : %i \n", (psych_uint64) dwmtiming.cRefreshStarted);
			printf("cFramesLate             : %i \n", (psych_uint64) dwmtiming.cFramesLate);
			printf("cFramesDropped          : %i \n", (psych_uint64) dwmtiming.cFramesDropped);
			printf("cFramesMissed           : %i \n", (psych_uint64) dwmtiming.cFramesMissed);
			printf("cBuffersEmpty           : %i \n", (psych_uint64) dwmtiming.cBuffersEmpty);
			
			printf("PTB-DEBUG: === End of PsychDwmGetCompositionTimingInfo returned data.  ===\n\n");
		}
		
		// Return success:
		return(TRUE);
	}
	else {
		if (IsDWMEnabled && PsychPrefStateGet_Verbosity() > 6) {
			printf("PTB-DEBUG: Call to PsychDwmGetCompositionTimingInfo() failed with rc1 = %x, GetLastError() = %i\n", rc1, GetLastError());			
		}
	}

	// DWM Unsupported, DWM disabled or query failed:
	return(FALSE);
}

/* PsychOSIsDWMEnabled()
 *
 * Return current Desktop Window Manager (DWM) status. Zero for disabled, Non-Zero for enabled.
 */
int	PsychOSIsDWMEnabled(void)
{
	return(IsDWMEnabled);
}

psych_bool PsychOSSetPresentParameters(PsychWindowRecordType *windowRecord, psych_uint64 targetVBL, unsigned int queueLength, double rateDuration)
{
	int rc;
	DWM_PRESENT_PARAMETERS dwmPresentParams;
	UNSIGNED_RATIO rateSource;
	
	// Map positive rateDuration to some refresh rate:
	if (rateDuration >= 0) {
		rateSource.uiNumerator = (int) rateDuration;
	}
	else {
		rateSource.uiNumerator = 0;
	}
	rateSource.uiDenominator = 1;
	
	dwmPresentParams.cbSize = sizeof(dwmPresentParams);
	dwmPresentParams.fQueue = (targetVBL > 0) ? TRUE : FALSE;
	dwmPresentParams.cRefreshStart = targetVBL;
	dwmPresentParams.cBuffer = queueLength;
	dwmPresentParams.fUseSourceRate = (rateDuration >= 0) ? TRUE : FALSE;
	dwmPresentParams.rateSource = rateSource;
	dwmPresentParams.cRefreshesPerFrame = (int) (rateDuration < 0) ? -rateDuration : 0;
	dwmPresentParams.eSampling = DWM_SOURCE_FRAME_SAMPLING_POINT;
	
	// Call function if DWM is supported and enabled:
	if (PsychOSIsDWMEnabled() && ((rc = PsychDwmSetPresentParameters(windowRecord->targetSpecific.windowHandle, &dwmPresentParams)) == 0)) return(TRUE);
	
	// DWM unsupported, disabled, or call failed:
	return(FALSE);
}

/*
    PsychOSFlipWindowBuffers()
    
    Performs OS specific double buffer swap call.
*/
void PsychOSFlipWindowBuffers(PsychWindowRecordType *windowRecord)
{
	// Execute OS neutral bufferswap code first:
	PsychExecuteBufferSwapPrefix(windowRecord);

	// Trigger the "Front <-> Back buffer swap (flip) (on next vertical retrace)":
	SwapBuffers(windowRecord->targetSpecific.deviceContext);
}

/* Enable/disable syncing of buffer-swaps to vertical retrace. */
void PsychOSSetVBLSyncLevel(PsychWindowRecordType *windowRecord, int swapInterval)
{
  static unsigned int failcount = 0;
  
  // Enable rendering context of window:
  PsychSetGLContext(windowRecord);
  
  // Store new setting also in internal helper variable, e.g., to allow workarounds to work:
  windowRecord->vSynced = (swapInterval > 0) ? TRUE : FALSE;

  // Try to set requested swapInterval if swap-control extension is supported on
  // this windows machine. Otherwise this will be a no-op...
  if (wglSwapIntervalEXT) {
	if(!wglSwapIntervalEXT(swapInterval)) {
		failcount++;
		if (failcount <= 10) printf("PTB-ERROR: Setting wglSwapInterval(%i) failed! Expect severe display timing and display tearing problems!!! See 'help SyncTrouble' for more info.\n", swapInterval);
		if (failcount == 10) printf("PTB-ERROR: This error message won't repeat on subsequent failure...\n");
	}
	
	// Double check new setting:
	if ((NULL == wglGetSwapIntervalEXT) || (wglGetSwapIntervalEXT() != swapInterval)) {
		failcount++;
		if (failcount <= 10) {
			if (NULL == wglGetSwapIntervalEXT) {
				printf("PTB-ERROR: wglGetSwapIntervalEXT() unsupported. This is a bug in your graphics driver or system setup!! See 'help SyncTrouble' for more info.\n");
			}
			else {
				printf("PTB-ERROR: Mismatch between requested display swap interval %i and actual swap interval %i! Synchronization of Screen('Flip') to display refresh will likely malfunction!\n", swapInterval, wglGetSwapIntervalEXT());
				printf("PTB-ERROR: This is either a bug in your graphics driver or system setup, or some misconfiguration in the display setting control panel of your system. See 'help SyncTrouble' for more info.\n");
			}
		}
		if (failcount == 10) printf("PTB-ERROR: These error messages won't repeat on subsequent failure...\n");
	}
  }
  
  return;
}

/*
    PsychOSSetGLContext()
    
    Set the window to which GL drawing commands are sent.  
*/
void PsychOSSetGLContext(PsychWindowRecordType *windowRecord)
{
	if (wglGetCurrentContext() != windowRecord->targetSpecific.contextObject) {
		if (wglGetCurrentContext() != NULL) {
			// We need to glFlush the context before switching, otherwise race-conditions may occur:
			glFlush();
			
			// Need to unbind any FBO's in old context before switch, otherwise bad things can happen...
			if (glBindFramebufferEXT) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}

		// Switch to new context:
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

/* Same as PsychOSSetGLContext() but for selecting userspace rendering context,
 * optionally copying state from PTBs context.
 */
void PsychOSSetUserGLContext(PsychWindowRecordType *windowRecord, psych_bool copyfromPTBContext)
{
	// Child protection:
	if (windowRecord->targetSpecific.glusercontextObject == NULL) PsychErrorExitMsg(PsychError_user, "GL Userspace context unavailable! Call InitializeMatlabOpenGL *before* Screen('OpenWindow')!");
	
	if (copyfromPTBContext) {
  		wglMakeCurrent(windowRecord->targetSpecific.deviceContext, NULL);		
		wglCopyContext(windowRecord->targetSpecific.contextObject, windowRecord->targetSpecific.glusercontextObject, GL_ALL_ATTRIB_BITS);
	}
	
    // Setup new context if it isn't already setup. -> Avoid redundant context switch.
  	 if (wglGetCurrentContext() != windowRecord->targetSpecific.glusercontextObject) {
		 wglMakeCurrent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.glusercontextObject);
    }
}
