/*
	PsychToolbox3/Source/Linux/Screen/PsychWindowGlue.c
	
	PLATFORMS:	
	
		This is the Linux/X11 version only.  
				
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu
                Mario Kleiner           mk              mario.kleiner at tuebingen.mpg.de

	HISTORY:
	
	        2/20/06                 mk              Created - Derived from Windows version.
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

#include <sched.h>
#include <errno.h>

// Number of currently open onscreen windows:
static int x11_windowcount = 0;


typedef int (*GLXEXTVSYNCCONTROLPROC) (int);
GLXEXTVSYNCCONTROLPROC PTBglXSwapIntervalSGI = NULL;


/** PsychRealtimePriority: Temporarily boost priority to highest available priority on Linux.
    PsychRealtimePriority(true) enables realtime-scheduling (like Priority(>0) would do in Matlab).
    PsychRealtimePriority(false) restores scheduling to the state before last invocation of PsychRealtimePriority(true),
    it undos whatever the previous switch did.

    We switch to RT scheduling during PsychGetMonitorRefreshInterval() and a few other timing tests in
    PsychOpenWindow() to reduce measurement jitter caused by possible interference of other tasks.
*/
boolean PsychRealtimePriority(boolean enable_realtime)
{
    static boolean old_enable_realtime = FALSE;
    static int   oldPriority = SCHED_OTHER;
    const  int   realtime_class = SCHED_RR;
    struct sched_param param, oldparam;

    if (old_enable_realtime == enable_realtime) {
        // No transition with respect to previous state -> Nothing to do.
        return(true);
    }
    
    // Transition requested:    
    if (enable_realtime) {
      // Transition to realtime requested:
      
      // Get current scheduling policy and back it up for later restore:
      oldPriority = sched_getscheduler(0);
      sched_getparam(0, &oldparam);

      // Check if realtime scheduling isn't already active.
      // If we are already in RT mode (e.g., Priority(2) call in Matlab), we skip the switch...
      if (oldPriority != realtime_class) {
	// RT scheduling not yet active -> Switch to it.
	// We use the smallest realtime priority that's available for realtime_class.
	// This way, other processes like watchdogs can preempt us, if needed.
	param.sched_priority = sched_get_priority_min(realtime_class);
	if (sched_setscheduler(0, realtime_class, &param)) {
	  // Failed!
	  printf("PTB-INFO: Failed to enable realtime-scheduling [%s]!\n", strerror(errno));
	  if (errno==EPERM) {
	    printf("PTB-INFO: You need to run Matlab with root-privileges for this to work.\n");
	  }
	  errno=0;
	}
      }
    }
    else {
      // Transition from RT to whatever-it-was-before scheduling requested: We just reestablish the backed-up old
      // policy: If the old policy wasn't Non-RT, then we don't switch back...
      sched_setscheduler(0, oldPriority, &oldparam);      
    }
    
    // Success.
    old_enable_realtime = enable_realtime;
    return(TRUE);
}

// Callback handler for Window manager: Handles some events
/* LONG FAR PASCAL WndProc(HWND hWnd, unsigned uMsg, unsigned wParam, LONG lParam) */
/* { */
/*   static PAINTSTRUCT ps; */
/*   PsychWindowRecordType	**windowRecordArray; */
/*   int i, numWindows;  */

/*   // What event happened? */
/*   switch(uMsg) */
/*     { */
/*     case WM_SYSCOMMAND: */
/*       // System command received: We intercept system commands that would start */
/*       // the screensaver or put the display into powersaving sleep-mode: */
/*       switch(wParam) */
/* 	{ */
/* 	case SC_SCREENSAVE: */
/* 	case SC_MONITORPOWER: */
/* 	  return(0); */
/* 	} */
/*       break; */

/*     case WM_PAINT: */
/*       // Repaint event: This happens if a previously covered non-fullscreen window */
/*       // got uncovered, so part of it needs to be redrawn. PTB's rendering model */
/*       // doesn't have a concept of redrawing a stimulus. As this is mostly useful */
/*       // for debugging, we just do a double doublebuffer swap in the hope that this */
/*       // will restore the frontbuffer... */
/*       BeginPaint(hWnd, &ps); */
/*       EndPaint(hWnd, &ps); */
/*       // Scan the list of windows to find onscreen window with handle hWnd: */
/*       PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray); */
/*       for(i = 0; i < numWindows; i++) { */
/* 	if (PsychIsOnscreenWindow(windowRecordArray[i]) && */
/* 	    windowRecordArray[i]->targetSpecific.windowHandle == hWnd) { */
/* 	  // This is it! Initiate bufferswap twice: */
/* 	  PsychOSFlipWindowBuffers(windowRecordArray[i]); */
/* 	  PsychOSFlipWindowBuffers(windowRecordArray[i]); */
/* 	} */
/*       } */
/*       PsychDestroyVolatileWindowRecordPointerList(windowRecordArray); */
/*       // Done. */
/*       return 0; */

/*     case WM_SIZE: */
/*       // Window resize event: Only happens in debug-mode (non-fullscreen). */
/*       // We resize the viewport accordingly and then trigger a repaint-op. */
/*       glViewport(0, 0, LOWORD(lParam), HIWORD(lParam)); */
/*       PostMessage(hWnd, WM_PAINT, 0, 0); */
/*       // printf("\nPTB-INFO: Onscreen window resized to: %i x %i.\n", (int) LOWORD(lParam), (int) HIWORD(lParam)); */
/*       return 0; */

/*     case WM_CLOSE: */
/*       // WM_CLOSE falls through to WM_CHAR and emulates an Abort-key press. */
/*       // -> Manually closing an onscreen window does the same as pressing the Abort-key. */
/*       wParam='@'; */
/*     case WM_CHAR: */
/*       // Character received. We only care about one key, the '@' key. */
/*       // Pressing '@' will immediately close all onscreen windows, show */
/*       // the cursor and such. It is the emergency stop key. */
/*       if (wParam=='@') { */
/* 	// Emergency shutdown: */
/* 	printf("\nPTB-INFO: Master-Abort key '@' pressed by user.\n"); */
/* 	printf("PTB-INFO: Enforcing script abortion and restoring desktop by executing Screen('CloseAll') now!\n"); */
/* 	printf("PTB-INFO: Please ignore the false error message (INTERNAL PSYCHTOOLBOX ERROR) caused by this...\n"); */
/* 	ScreenCloseAllWindows(); */
/* 	return(0); */
/*       } */
/*       break; */
/*     } */

/*     return DefWindowProc(hWnd, uMsg, wParam, lParam); */
/* } */

/* boolean ChangeScreenResolution (int width, int height, int bitsPerPixel, int fps)	// Change The Screen Resolution */
/* { */
/*   DEVMODE dmScreenSettings; // Device mode structure */

/*   // Clear structure: */
/*   memset (&dmScreenSettings, 0, sizeof (DEVMODE)); */
/*   dmScreenSettings.dmSize		= sizeof (DEVMODE); */
/*   dmScreenSettings.dmDriverExtra	= 0; */
/*  */
/*   // Query current display settings and init struct with them: */
/*   EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmScreenSettings); */
/*  */
/*   // Override current settings with the requested settings, if any: */
/*   if (width>0)  dmScreenSettings.dmPelsWidth  = width;  // Select Screen Width */
/*   if (height>0) dmScreenSettings.dmPelsHeight = height; // Select Screen Height */
/*   if (bitsPerPixel>0) dmScreenSettings.dmBitsPerPel = bitsPerPixel; // Select Bits Per Pixel */
/*   if (fps>0) dmScreenSettings.dmDisplayFrequency = fps; // Select display refresh rate in Hz */
/*    */
/*   // All provided values should be honored: We need to spec the refresh explicitely, */
/*   // because otherwise the system will select the lowest fps for a given display mode. */
/*   dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY; */
/*  */
/*   // Perform the change: */
/*   if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) { */
/*     return(FALSE);	// Display Change Failed, Return False */
/*   } */
/*  */
/*   // Display Change Was Successful, Return True */
/*   return(TRUE); */
/* } */

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
  PsychRectType             screenrect;
  CGDirectDisplayID         dpy;
  int scrnum;
  XSetWindowAttributes attr;
  unsigned long mask;
  Window root;
  Window win;
  GLXContext ctx;
  XVisualInfo *visinfo;
  int x, y, width, height;
  boolean fullscreen = FALSE;
  int attrib[30];
  int attribcount=0;

  // Map the logical screen number to the corresponding X11 display connection handle
  // for the corresponding X-Server connection.
  PsychGetCGDisplayIDFromScreenNumber(&dpy, screenSettings->screenNumber);

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
    
    // Switch system to fullscreen-mode without changing any settings:
    fullscreen = TRUE;
  }
  else {
    // Window size different from current screen size:
    // A regular desktop window with borders and control icons is requested, e.g., for debugging:
    // Extract settings:
    x=windowRecord->rect[kPsychLeft];
    y=windowRecord->rect[kPsychTop];
    width=PsychGetWidthFromRect(windowRecord->rect);
    height=PsychGetHeightFromRect(windowRecord->rect);
    fullscreen = FALSE;
  }

  // Setup pixelformat descriptor for selection of GLX visual:
  attrib[attribcount++]= GLX_RGBA;       // Use RGBA true-color visual.
  attrib[attribcount++]= GLX_RED_SIZE;   // At least 8 bits per RGBA component...
  attrib[attribcount++]= 8;
  attrib[attribcount++]= GLX_GREEN_SIZE;
  attrib[attribcount++]= 8;
  attrib[attribcount++]= GLX_BLUE_SIZE;
  attrib[attribcount++]= 8;
  attrib[attribcount++]= GLX_ALPHA_SIZE;
  attrib[attribcount++]= 8;

  // Stereo display support: If stereo display output is requested with OpenGL native stereo,
  // we request a stereo-enabled rendering context.
  if(stereomode==kPsychOpenGLStereo) {
    attrib[attribcount++]= GLX_STEREO;
  }

  // Support for OpenGL 3D rendering requested?
  if (PsychPrefStateGet_3DGfx()) {
    // Yes. Allocate and attach a 24 bit depth buffer and 8 bit stencil buffer:
    attrib[attribcount++]= GLX_DEPTH_SIZE;
    attrib[attribcount++]= 24;
    attrib[attribcount++]= GLX_STENCIL_SIZE;
    attrib[attribcount++]= 8;

  }

  // Double buffering requested?
  if(numBuffers>=2) {
    // Enable double-buffering:
    attrib[attribcount++]= GLX_DOUBLEBUFFER;

    // AUX buffers for Flip-Operations needed?
    if ((conserveVRAM & kPsychDisableAUXBuffers) == 0) {
      // Allocate one or two (mono vs. stereo) AUX buffers for new "don't clear" mode of Screen('Flip'):
      // Not clearing the framebuffer after "Flip" is implemented by storing a backup-copy of
      // the backbuffer to AUXs before flip and restoring the content from AUXs after flip.
      attrib[attribcount++]= GLX_AUX_BUFFERS;
      attrib[attribcount++]=(stereomode==kPsychOpenGLStereo || stereomode==kPsychCompressedTLBRStereo || stereomode==kPsychCompressedTRBLStereo) ? 2 : 1;
    }
  }

  // It's important that GLX_AUX_BUFFERS is the last entry in the attrib array, see code for glXChooseVisual below...

  // Finalize attric array:
  attrib[attribcount++]= None;

  scrnum = screenSettings->screenNumber; // Original: DefaultScreen(cgDisplayID);
  root = RootWindow( dpy, scrnum );

  // Select matching visual for our pixelformat:
  visinfo = glXChooseVisual( dpy, scrnum, attrib );
  if (!visinfo) {
    // Failed to find matching visual: This can happen if we request AUX buffers on a system
    // that doesn't support AUX-buffers. In that case we retry without requesting AUX buffers
    // and output a proper warning instead of failing. For 99% of all applications one can
    // do without AUX buffers anyway...

    // Terminate attrib array where the GLX_AUX_BUFFERS entry used to be...
    attrib[attribcount-3] = None;
    // Retry...
    visinfo = glXChooseVisual( dpy, scrnum, attrib );
    if (!visinfo) {
      printf("\nPTB-ERROR[glXChooseVisual() failed]: Couldn't get a suitable visual from X-Server.\n\n");
      return(FALSE);
    }
    else {
      // Suceeded without AUX buffers...
      printf("PTB-WARNING: Couldn't enable AUX buffers on onscreen window due to limitations of your gfx-hardware or driver. Some features may be disabled or limited...\n");
      fflush(NULL);
    }
  }

  // Setup window attributes:
  attr.background_pixel = 0;  // Background color defaults to black.
  attr.border_pixel = 0;      // Border color as well.
  attr.colormap = XCreateColormap( dpy, root, visinfo->visual, AllocNone);  // Dummy colormap assignment.
  attr.event_mask = KeyPressMask; // | StructureNotifyMask | ExposureMask;  // We're only interested in keypress events for GetChar().
  attr.override_redirect = (fullscreen) ? 1 : 0;                            // Lock out window manager if fullscreen window requested.
  mask = CWOverrideRedirect | CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

  // Create our onscreen window:
  win = XCreateWindow( dpy, root, x, y, width, height,
		       0, visinfo->depth, InputOutput,
		       visinfo->visual, mask, &attr );

  // Set hints and properties:
  {
    XSizeHints sizehints;
    sizehints.x = x;
    sizehints.y = y;
    sizehints.width  = width;
    sizehints.height = height;
    sizehints.flags = USSize | USPosition;
    XSetNormalHints(dpy, win, &sizehints);
    XSetStandardProperties(dpy, win, "PTB Onscreen window", "PTB Onscreen window",
			   None, (char **)NULL, 0, &sizehints);
  }

  // Create associated GLX OpenGL rendering context: We don't use ressource
  // sharing of textures and display lists yet (NULL) and we request a direct
  // rendering context (True) if possible:
  ctx = glXCreateContext(dpy, visinfo, NULL, True );
  if (!ctx) {
    printf("\nPTB-ERROR:[glXCreateContext() failed] OpenGL context creation failed!\n\n");
    return(FALSE);
  }

  // Release visual info:
  XFree(visinfo);

  // Store the handles...
  windowRecord->targetSpecific.windowHandle = win;
  windowRecord->targetSpecific.deviceContext = dpy;
  windowRecord->targetSpecific.contextObject = ctx;

  // Show our new window:
  XMapWindow(dpy, win);
  
  // Activate the associated rendering context:
  PsychOSSetGLContext(windowRecord);

  // Give it higher priority as other applications windows:
  // SetForegroundWindow(hWnd);

  // Set the focus on it:
  // SetFocus(hWnd);

  // Capture the window if it is a fullscreen one, whatever that means...
  // if (fullscreen) SetCapture(hWnd);

  // Increase our own open window counter:
  x11_windowcount++;

  // Some info for the user regarding non-fullscreen and ATI hw:
  if (!fullscreen && (strstr(glGetString(GL_VENDOR), "ATI"))) {
    printf("PTB-INFO: Some ATI graphics cards may not support proper syncing to vertical retrace when\n");
    printf("PTB-INFO: running in windowed mode (non-fullscreen). If PTB aborts with 'Synchronization failure'\n");
    printf("PTB-INFO: you can disable the sync test via call to Screen('Preference', 'SkipSyncTests', 1); .\n");
    printf("PTB-INFO: You won't get proper stimulus onset timestamps though, so windowed mode may be of limited use.\n");
  }

  // Check for availability of VSYNC extension:
  PTBglXSwapIntervalSGI=(GLXEXTVSYNCCONTROLPROC) glXGetProcAddressARB("glXSwapIntervalSGI");
  if (PTBglXSwapIntervalSGI==NULL || strstr(glXQueryExtensionsString(dpy, scrnum), "GLX_SGI_swap_control")==NULL) {
    printf("PTB-WARNING: Your graphics driver doesn't allow me to control syncing wrt. vertical retrace!\n");
    printf("PTB-WARNING: Please update your display graphics driver as soon as possible to fix this.\n");
    printf("PTB-WARNING: Until then, you can manually enable syncing to VBL somehow in a manner that is\n");
    printf("PTB-WARNING: dependent on the type of gfx-card and driver. Google is your friend...\n");
  }

  // Ok, we should be ready for OS independent setup...
  printf("\nPTB-INFO: Low-level GNU/Linux X11 setup of onscreen window finished!\n");
  fflush(NULL);

  // Wait for X-Server to settle...
  XSync(dpy, 1);

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
  // This function is obsolete and does nothing.
  return(FALSE);
}


void PsychOSCloseWindow(PsychWindowRecordType *windowRecord)
{
  Display* dpy = windowRecord->targetSpecific.deviceContext;

  // Detach OpenGL rendering context again - just to be safe!
  glXMakeCurrent(windowRecord->targetSpecific.deviceContext, None, NULL);

  // Delete rendering context:
  glXDestroyContext(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.contextObject);
  windowRecord->targetSpecific.contextObject=NULL;

  // Wait for X-Server to settle...
  XSync(dpy, 0);

  // Close & Destroy the window:
  XUnmapWindow(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle);

  // Wait for X-Server to settle...
  XSync(dpy, 0);

  XDestroyWindow(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle);
  windowRecord->targetSpecific.windowHandle=NULL;

  // Wait for X-Server to settle...
  XSync(dpy, 0);

  // Release device context: We just release the reference. The connection to the display is
  // closed somewhere else.
  windowRecord->targetSpecific.deviceContext=NULL;

  // Was this the last window?
  x11_windowcount--;

  if (x11_windowcount<=0) {
    x11_windowcount=0;

    // This is currently a No-Op...
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
  unsigned int vsync_counter = 0;

  //  glXGetVideoSyncSGI(&vsync_counter);
  //  printf("PREFLIP-VSYNC: %i\n", vsync_counter);
  //  glXWaitVideoSyncSGI(2000000000, (int) vsync_counter + 10, &vsync_counter);
  //  printf("POSTFLIP-VSYNC: %i\n", vsync_counter);

  // Trigger the "Front <-> Back buffer swap (flip) (on next vertical retrace)":
  glXSwapBuffers(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle);
}

/* Enable/disable syncing of buffer-swaps to vertical retrace. */
void PsychOSSetVBLSyncLevel(PsychWindowRecordType *windowRecord, int swapInterval)
{
  // Enable rendering context of window:
  PsychSetGLContext(windowRecord);

  // Try to set requested swapInterval if swap-control extension is supported on
  // this Linux machine. Otherwise this will be a no-op...
  if (PTBglXSwapIntervalSGI) PTBglXSwapIntervalSGI(swapInterval);

  return;
}

/*
    PsychOSSetGLContext()
    
    Set the window to which GL drawing commands are sent.  
*/
void PsychOSSetGLContext(PsychWindowRecordType *windowRecord)
{
  if (glXGetCurrentContext() != windowRecord->targetSpecific.contextObject) {
    glXMakeCurrent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle, windowRecord->targetSpecific.contextObject);
  }
}

/*
    PsychOSUnsetGLContext()
    
    Clear the drawing context.  
*/
void PsychOSUnsetGLContext(PsychWindowRecordType* windowRecord)
{
  glXMakeCurrent(windowRecord->targetSpecific.deviceContext, None, NULL);
}

int CGDisplayBeamPosition(CGDirectDisplayID cgDisplayId)
{
  // FIXME: Don't know how to do this on Linux/X11. Don't think this is supported.
  // We return -1 as an indicator to high-level routines that we don't
  // know the rasterbeam position.
  return(-1);
}
