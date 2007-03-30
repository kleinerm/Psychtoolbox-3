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

// Mouse button states:
static boolean mousebutton_l=FALSE;
static boolean mousebutton_m=FALSE;
static boolean mousebutton_r=FALSE;

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
  switch(uMsg) {
    case WM_SYSCOMMAND:
      // System command received: We intercept system commands that would start
      // the screensaver or put the display into powersaving sleep-mode:
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
      glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
      PostMessage(hWnd, WM_PAINT, 0, 0);
      // printf("\nPTB-INFO: Onscreen window resized to: %i x %i.\n", (int) LOWORD(lParam), (int) HIWORD(lParam));
      return 0;

    case WM_CLOSE:
      // WM_CLOSE falls through to WM_CHAR and emulates an Abort-key press.
      // -> Manually closing an onscreen window does the same as pressing the Abort-key.
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
  // MSG msg;
	// Old-Style mouse button queries by parsing events in the event-queue and
	// keeping track of state changes... Obsoleted by GetAsyncKeyState() below.

	// Run our message dispatch loop until we've processed all mouse-related events
	// in the queue:
	//	while(PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE)) {
	//	// Mouse event in queue. Dispatch it to our WndProc...
	//	TranslateMessage(&msg);
	//	// The WndProc will keep track of the current mouse button state by
	//	// updating it according to the UP or DOWN message.
	//	DispatchMessage(&msg);
	// }

	// The routine expects a preallocated 3-element double matrix for the three mouse-buttons:
	// It simply copies our internally held current state to the output matrix:
	//buttonArray[0]=(double) mousebutton_l;
	//buttonArray[1]=(double) mousebutton_m;
	//buttonArray[2]=(double) mousebutton_r;

	// GetAsyncKeyState() directly returns the current state of the physical mouse
	// buttons, independent of window system event processing, keyboard or mouse
	// focus and such. Much more robust.
	buttonArray[0]=(double) ((GetAsyncKeyState(VK_LBUTTON) & -32768) ? 1 : 0);
	buttonArray[1]=(double) ((GetAsyncKeyState(VK_MBUTTON) & -32768) ? 1 : 0);
	buttonArray[2]=(double) ((GetAsyncKeyState(VK_RBUTTON) & -32768) ? 1 : 0);

	return;
}

boolean ChangeScreenResolution (int screenNumber, int width, int height, int bitsPerPixel, int fps)	// Change The Screen Resolution
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
boolean PsychOSOpenOnscreenWindow(PsychScreenSettingsType *screenSettings, PsychWindowRecordType *windowRecord, int numBuffers, int stereomode, int conserveVRAM)
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
  int         attribs[48];
  int         attribcount;
  float       fattribs[2]={0,0};
  int x, y, width, height, i, bpc;
  GLenum      glerr;
  DWORD flags;
  boolean fullscreen = FALSE;
  DWORD windowStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  // The WS_EX_NOACTIVATE flag prevents the window from grabbing keyboard focus. That way,
  // the new Java-GetChar can do its job.
  DWORD windowExtendedStyle = WS_EX_APPWINDOW | 0x08000000; // const int WS_EX_NOACTIVATE = 0x08000000;

	 // Init to safe default:
    windowRecord->targetSpecific.glusercontextObject = NULL;
    
    // Map the logical screen number to a device handle for the corresponding
    // physical display device: CGDirectDisplayID is currently typedef'd to a
    // HDC windows hardware device context handle.
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenSettings->screenNumber);

    // Check if this should be a fullscreen window:
    PsychGetScreenRect(screenSettings->screenNumber, screenrect);
    if (PsychMatchRect(screenrect, windowRecord->rect)) {
      // This is supposed to be a fullscreen window with the dimensions of
      // the current display/desktop:
      // Switch system to fullscreen-mode without changing any settings:
      fullscreen = ChangeScreenResolution(screenSettings->screenNumber, 0, 0, 0, 0);
    }
    else {
      // Window size different from current screen size:
      // A regular desktop window with borders and control icons is requested, e.g., for debugging:
      fullscreen = FALSE;
    }

	 // Special case for explicit multi-display setup under Windows when opening a window on
	 // screen zero. We enforce the fullscreen - flag, aka a borderless top level window. This way,
    // if anything of our automatic full-desktop window emulation code goes wrong on exotic setups,
    // the user can still enforce a suitably positioned and sized borderless toplevel window.
    if (PsychGetNumDisplays()>2 && screenSettings->screenNumber == 0) fullscreen = TRUE;

    if (fullscreen) {
      windowStyle = WS_POPUP;		      // Set The WindowStyle To WS_POPUP (Popup Window without borders)
      windowExtendedStyle |= WS_EX_TOPMOST;   // Set The Extended Window Style To WS_EX_TOPMOST
    }
    else {
      windowStyle |= WS_OVERLAPPEDWINDOW;
      windowExtendedStyle |= WS_EX_TOPMOST;   // Set The Extended Window Style To WS_EX_TOPMOST
    }

    // Define final position and size of window:
    x=windowRecord->rect[kPsychLeft];
    y=windowRecord->rect[kPsychTop];
    width=PsychGetWidthFromRect(windowRecord->rect);
    height=PsychGetHeightFromRect(windowRecord->rect);

    // Assemble windows caption name from window index:
    sprintf(winname, "PTB Onscreen window [%i]:", (int) windowRecord->windowIndex);

    // Register our own window class for Psychtoolbox onscreen windows:
    // Only register the window class once - use hInstance as a flag.
    if (!hInstance) {
      hInstance = GetModuleHandle(NULL);
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
			  winname,
			  windowStyle,
			  x, y, width, height, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
        printf("\nPTB-ERROR[CreateWindow() failed]: Unknown error, Win32 specific.\n\n");
        return(FALSE);
    }

    // Retrieve device context for the window:
    hDC = GetDC(hWnd);

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
    if (windowRecord->depth == 64 || windowRecord->depth == 128) {
      // Request a floating point drawable instead of a fixed-point one:
      attribs[attribcount++]=WGL_TYPE_RGBA_FLOAT_ARB;
    }
    else {
      // Request standard fixed point drawable:
      attribs[attribcount++]=0x202B; // WGL_TYPE_RGBA_ARB
    }
    
    // Select requested depth per color component 'bpc' for each channel:
    bpc = 8; // We default to 8 bpc == RGBA8
    if (windowRecord->depth == 30)  { bpc = 10; printf("PTB-INFO: Trying to enable at least 10 bpc fixed point framebuffer.\n"); }
    if (windowRecord->depth == 64)  { bpc = 16; printf("PTB-INFO: Trying to enable 16 bpc floating point framebuffer.\n"); }
    if (windowRecord->depth == 128) { bpc = 32; printf("PTB-INFO: Trying to enable 32 bpc floating point framebuffer.\n"); }
    
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
    pfd.cAlphaBits   = 8;             // Want a 8 bit alpha-buffer.
    
    // Support for OpenGL 3D rendering requested?
    if (PsychPrefStateGet_3DGfx()) {
      // Yes. Allocate and attach a 24bit depth buffer and 8 bit stencil buffer:
      pfd.cDepthBits = 24;
      pfd.cStencilBits = 8;
      attribs[attribcount++]=0x2022; // WGL_DEPTH_BITS_ARB
      attribs[attribcount++]=24;
      attribs[attribcount++]=0x2023; // WGL_STENCIL_BITS_ARB
      attribs[attribcount++]=8;
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
      ReleaseDC(hDC, hWnd);
      DestroyWindow(hWnd);
      
      printf("PTB-ERROR: OpenGL native stereo mode unavailable. Your hardware may not support it,\n"
	     "PTB-ERROR: or at least not on a flat-panel? Expect abortion of your script soon...");
      
      return(FALSE);
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

	   // Retrieve device context for the window:
    	hDC = GetDC(hWnd);

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

      // Activate the rendering context:
      PsychOSSetGLContext(windowRecord);

      DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

      if ((stereomode==kPsychOpenGLStereo) && ((pfd.dwFlags & PFD_STEREO)==0)) {
         // Ooops. Couldn't get the requested stereo-context from hardware :(
         ReleaseDC(hDC, hWnd);
         DestroyWindow(hWnd);

         printf("PTB-ERROR: OpenGL native stereo mode unavailable. Your hardware may not support it,\n"
	             "PTB-ERROR: or at least not on a flat-panel? Expect abortion of your script soon...");

         return(FALSE);
      }		
		// Done with final window and OpenGL context setup. We've got our final context enabled.
	 }

	 // Enable multisampling if this was requested:
    if (windowRecord->multiSample > 0) glEnable(0x809D); // 0x809D == GL_MULTISAMPLE_ARB
	 // Throw away any error-state this could have created on old hardware...
	 glGetError();

    // External 3D graphics support enabled? Or OpenGL quad-buffered stereo enabled?
	 // For the former, we need this code for OpenGL state isolation. For the latter we
    // need this code as workaround for Windows brain-damage. For some reason it helps
    // to properly shutdown stereo contexts on Windows...
	 if (PsychPrefStateGet_3DGfx() || stereomode == kPsychOpenGLStereo) {
		// Yes. We need to create an extra OpenGL rendering context for the external
		// OpenGL code to provide optimal state-isolation. The context shares all
		// heavyweight ressources likes textures, FBOs, VBOs, PBOs, display lists and
		// starts off as an identical copy of PTB's context as of here.
      windowRecord->targetSpecific.glusercontextObject = wglCreateContext(hDC);
		if (windowRecord->targetSpecific.glusercontextObject == NULL) {
         ReleaseDC(hDC, hWnd);
         DestroyWindow(hWnd);
			printf("\nPTB-ERROR[UserContextCreation failed]: Creating a private OpenGL context for Matlab OpenGL failed for unknown reasons.\n\n");
			return(FALSE);
		}

		wglMakeCurrent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.glusercontextObject);
		wglMakeCurrent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.contextObject);

		// Copy full state from our main context:
		if(!wglCopyContext(windowRecord->targetSpecific.contextObject, windowRecord->targetSpecific.glusercontextObject, GL_ALL_ATTRIB_BITS)) {
			// This is ugly, but not fatal...
			if (PsychPrefStateGet_Verbosity()>1) {
				printf("\nPTB-WARNING[wglCopyContext for user context failed]: Copying state to private OpenGL context for Matlab OpenGL failed for unknown reasons.\n\n");
			}
		}

	   // Enable ressource sharing with master context for this window:
		if (!wglShareLists(windowRecord->targetSpecific.contextObject, windowRecord->targetSpecific.glusercontextObject)) {
			// This is ugly, but not fatal...
			if (PsychPrefStateGet_Verbosity()>1) {
				printf("\nPTB-WARNING[wglShareLists for user context failed]: Ressource sharing with private OpenGL context for Matlab OpenGL failed for unknown reasons.\n\n");
			}		
		}

	 }

    // Finally, show our new window:
    ShowWindow(hWnd, SW_SHOW);

    // Give it higher priority than other applications windows:
    // Disabled: Interferes with JAVA-GetChar: SetForegroundWindow(hWnd);

    // Set the focus on it:
    // Disabled: Interferes with JAVA-GetChar: SetFocus(hWnd);

    // Capture the window if it is a fullscreen one: This window will receive all
    // mouse move and mouse button press events. Important for GetMouse() to work
    // properly...
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
    }
  }

  // Restore video settings from the defaults in the Windows registry:
  ChangeDisplaySettings(NULL, 0);

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
		if (wglGetCurrentContext() != NULL) {
			// We need to glFlush the context before switching, otherwise race-conditions may occur:
			glFlush();
			
			// Need to unbind any FBO's in old context before switch, otherwise bad things can happen...
			if (glBindFramebufferEXT) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}

		// Switch to new context:
		wglMakeCurrent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.contextObject);
		
		// If imaging pipe is active, we need to reset the current drawing target, so it and its
		// FBO bindings get properly reinitialized before next use. In non-imaging mode this is
		// not needed, because the new context already contains the proper setup for transformations,
		// drawbuffers and such, as well as the matching content in the backbuffer:
		if (windowRecord->imagingMode > 0) PsychSetDrawingTarget(NULL);
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
void PsychOSSetUserGLContext(PsychWindowRecordType *windowRecord, Boolean copyfromPTBContext)
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
