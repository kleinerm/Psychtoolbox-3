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
		should be platform-neutral, despite that the calls in OS X and Linux to detect available pixel sizes are different.  The platform 
		specificity is abstracted out in C files which end it "Glue", for example PsychScreenGlue, PsychWindowGlue, PsychWindowTextClue.

	NOTES:
	
	TO DO: 
	 
*/

#include "Screen.h"

/* These are needed for realtime scheduling control: */
#include <sched.h>
#include <errno.h>

/* XAtom support for setup of transparent windows: */
#include <X11/Xatom.h>

// Number of currently open onscreen windows:
static int x11_windowcount = 0;

/** PsychRealtimePriority: Temporarily boost priority to highest available priority on Linux.
    PsychRealtimePriority(true) enables realtime-scheduling (like Priority(>0) would do in Matlab).
    PsychRealtimePriority(false) restores scheduling to the state before last invocation of PsychRealtimePriority(true),
    it undos whatever the previous switch did.

    We switch to RT scheduling during PsychGetMonitorRefreshInterval() and a few other timing tests in
    PsychOpenWindow() to reduce measurement jitter caused by possible interference of other tasks.
*/
psych_bool PsychRealtimePriority(psych_bool enable_realtime)
{
    static psych_bool old_enable_realtime = FALSE;
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
	  if(!PsychPrefStateGet_SuppressAllWarnings()) {
	    printf("PTB-INFO: Failed to enable realtime-scheduling [%s]!\n", strerror(errno));
	    if (errno==EPERM) {
	      printf("PTB-INFO: You need to run Matlab with root-privileges for this to work.\n");
	    }
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
  PsychRectType             screenrect;
  CGDirectDisplayID         dpy;
  int scrnum;
  XSetWindowAttributes attr;
  unsigned long mask;
  Window root;
  Window win;
  GLXContext ctx;
  XVisualInfo *visinfo;
  int i, x, y, width, height;
  GLenum glerr;
  psych_bool fullscreen = FALSE;
  int attrib[38];
  int attribcount=0;
  int depth, bpc;
  int windowLevel;

  // Retrieve windowLevel, an indicator of where non-fullscreen windows should
  // be located wrt. to other windows. 0 = Behind everything else, occluded by
  // everything else. 1 - 999 = At layer windowLevel -> Occludes stuff on layers "below" it.
  // 1000 - 1999 = At highest level, but partially translucent / alpha channel allows to make
  // regions transparent. 2000 or higher: Above everything, fully opaque, occludes everything.
  // 2000 is the default.
  windowLevel = PsychPrefStateGet_WindowShieldingLevel();
  
  // Init userspace GL context to safe default:
  windowRecord->targetSpecific.glusercontextObject = NULL;
  	 
  // Which display depth is requested?
  depth = PsychGetValueFromDepthStruct(0, &(screenSettings->depth));

  // Map the logical screen number to the corresponding X11 display connection handle
  // for the corresponding X-Server connection.
  PsychGetCGDisplayIDFromScreenNumber(&dpy, screenSettings->screenNumber);
  scrnum = PsychGetXScreenIdForScreen(screenSettings->screenNumber);

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

	// Mark this window as fullscreen window:
	windowRecord->specialflags |= kPsychIsFullscreenWindow;
	
	// Copy absolute screen location and area of window to 'globalrect',
	// so functions like Screen('GlobalRect') can still query the real
	// bounding gox of a window onscreen:
	PsychGetGlobalScreenRect(screenSettings->screenNumber, windowRecord->globalrect);
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
	
	// Copy absolute screen location and area of window to 'globalrect',
	// so functions like Screen('GlobalRect') can still query the real
	// bounding gox of a window onscreen:
    PsychCopyRect(windowRecord->globalrect, windowRecord->rect);
  }

  // Select requested depth per color component 'bpc' for each channel:
  bpc = 8; // We default to 8 bpc == RGBA8
  if (windowRecord->depth == 30)  { bpc = 10; printf("PTB-INFO: Trying to enable at least 10 bpc fixed point framebuffer.\n"); }
  if (windowRecord->depth == 64)  { bpc = 16; printf("PTB-INFO: Trying to enable 16 bpc fixed point framebuffer.\n"); }
  if (windowRecord->depth == 128) { bpc = 32; printf("PTB-INFO: Trying to enable 32 bpc fixed point framebuffer.\n"); }
  
  // Setup pixelformat descriptor for selection of GLX visual:
  attrib[attribcount++]= GLX_RGBA;       // Use RGBA true-color visual.
  attrib[attribcount++]= GLX_RED_SIZE;   // Setup requested minimum depth of each color channel:
  attrib[attribcount++]= (depth > 16) ? bpc : 1;
  attrib[attribcount++]= GLX_GREEN_SIZE;
  attrib[attribcount++]= (depth > 16) ? bpc : 1;
  attrib[attribcount++]= GLX_BLUE_SIZE;
  attrib[attribcount++]= (depth > 16) ? bpc : 1;
  attrib[attribcount++]= GLX_ALPHA_SIZE;
  // Alpha channel needs special treatment:
  if (bpc != 10) {
	// Non 10 bpc drawable: Request a 'bpc' alpha channel if the underlying framebuffer
	// is in true-color mode ( >= 24 cpp format). If framebuffer is in 16 bpp mode, we
	// don't have/request an alpha channel at all:
	attrib[attribcount++]= (depth > 16) ? bpc : 0; // In 16 bit mode, we don't request an alpha-channel.
  }
  else {
	// 10 bpc drawable: We have a 32 bpp pixel format with R10G10B10 10 bpc per color channel.
	// There are at most 2 bits left for the alpha channel, so we request an alpha channel with
	// minimum size 1 bit --> Will likely translate into a 2 bit alpha channel:
	attrib[attribcount++]= 1;
  }
  
  // Stereo display support: If stereo display output is requested with OpenGL native stereo,
  // we request a stereo-enabled rendering context.
  if(stereomode==kPsychOpenGLStereo) {
    attrib[attribcount++]= GLX_STEREO;
  }

  // Multisampling support:
  if (windowRecord->multiSample > 0) {
    // Request a multisample buffer:
    attrib[attribcount++]= GLX_SAMPLE_BUFFERS_ARB;
    attrib[attribcount++]= 1;
    // Request at least multiSample samples per pixel:
    attrib[attribcount++]= GLX_SAMPLES_ARB;
    attrib[attribcount++]= windowRecord->multiSample;
  }

  // Support for OpenGL 3D rendering requested?
  if (PsychPrefStateGet_3DGfx()) {
    // Yes. Allocate and attach a 24 bit depth buffer and 8 bit stencil buffer:
    attrib[attribcount++]= GLX_DEPTH_SIZE;
    attrib[attribcount++]= 24;
    attrib[attribcount++]= GLX_STENCIL_SIZE;
    attrib[attribcount++]= 8;

	// Alloc an accumulation buffer as well?
	if (PsychPrefStateGet_3DGfx() & 2) {
		// Yes: Alloc accum buffer, request 64 bpp, aka 16 bits integer per color component if possible:
		attrib[attribcount++] = GLX_ACCUM_RED_SIZE;
		attrib[attribcount++] = 16;
		attrib[attribcount++] = GLX_ACCUM_GREEN_SIZE;
		attrib[attribcount++] = 16;
		attrib[attribcount++] = GLX_ACCUM_BLUE_SIZE;
		attrib[attribcount++] = 16;
		attrib[attribcount++] = GLX_ACCUM_ALPHA_SIZE;
		attrib[attribcount++] = 16;
	}
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

  root = RootWindow( dpy, scrnum );

  // Select matching visual for our pixelformat:
  visinfo = glXChooseVisual( dpy, scrnum, attrib );
  
  if (!visinfo) {
	  // Failed to find matching visual: Could it be related to request for unsupported native 10 bpc framebuffer?
	  if ((windowRecord->depth == 30) && (bpc == 10)) {
		  // 10 bpc framebuffer requested: Let's see if we can get a visual by lowering our demand to 8 bpc:
		  for (i=0; i<attribcount && attrib[i]!=GLX_RED_SIZE; i++);
		  attrib[i+1] = 8;
		  for (i=0; i<attribcount && attrib[i]!=GLX_GREEN_SIZE; i++);
		  attrib[i+1] = 8;
		  for (i=0; i<attribcount && attrib[i]!=GLX_BLUE_SIZE; i++);
		  attrib[i+1] = 8;
		  for (i=0; i<attribcount && attrib[i]!=GLX_ALPHA_SIZE; i++);
		  attrib[i+1] = 1;
		  
		  // Retry:
		  visinfo = glXChooseVisual( dpy, scrnum, attrib );
	  }
  }
  
  if (!visinfo) {
	  // Failed to find matching visual: Could it be related to multisampling?
	  if (windowRecord->multiSample > 0) {
		  // Multisampling requested: Let's see if we can get a visual by
		  // lowering our demand:
		  for (i=0; i<attribcount && attrib[i]!=GLX_SAMPLES_ARB; i++);
		  while(!visinfo && windowRecord->multiSample > 0) {
			  attrib[i+1]--;
			  windowRecord->multiSample--;
			  visinfo = glXChooseVisual( dpy, scrnum, attrib );
		  }
		  
		  // Either we have a valid visual at this point or we still fail despite
		  // requesting zero samples.
		  if (!visinfo) {
			  // We still fail. Disable multisampling by requesting zero multisample buffers:
			  for (i=0; i<attribcount && attrib[i]!=GLX_SAMPLE_BUFFERS_ARB; i++);
			  windowRecord->multiSample = 0;
			  attrib[i+1]=0;
			  visinfo = glXChooseVisual( dpy, scrnum, attrib );
		  }
		}

    // Break out of this if we finally got one...
    if (!visinfo) {
      // Failed to find matching visual: This can happen if we request AUX buffers on a system
      // that doesn't support AUX-buffers. In that case we retry without requesting AUX buffers
      // and output a proper warning instead of failing. For 99% of all applications one can
      // do without AUX buffers anyway...
      printf("PTB-WARNING: Couldn't enable AUX buffers on onscreen window due to limitations of your gfx-hardware or driver. Some features may be disabled or limited...\n");
      fflush(NULL);
      
      // Terminate attrib array where the GLX_AUX_BUFFERS entry used to be...
      attrib[attribcount-3] = None;
      
      // Retry...
      visinfo = glXChooseVisual( dpy, scrnum, attrib );
      if (!visinfo && PsychPrefStateGet_3DGfx()) {
	// Ok, retry with a 16 bit depth buffer...
	for (i=0; i<attribcount && attrib[i]!=GLX_DEPTH_SIZE; i++);
	if (attrib[i]==GLX_DEPTH_SIZE && i<attribcount) attrib[i+1]=16;
	printf("PTB-WARNING: Have to use 16 bit depth buffer instead of 24 bit buffer due to limitations of your gfx-hardware or driver. Accuracy of 3D-Gfx may be limited...\n");
	fflush(NULL);
	
	visinfo = glXChooseVisual( dpy, scrnum, attrib );
	if (!visinfo) {
	  // Failed again. Retry with disabled stencil buffer:
	  printf("PTB-WARNING: Have to disable stencil buffer due to limitations of your gfx-hardware or driver. Some 3D Gfx algorithms may fail...\n");
	  fflush(NULL);
	  for (i=0; i<attribcount && attrib[i]!=GLX_STENCIL_SIZE; i++);
	  if (attrib[i]==GLX_STENCIL_SIZE && i<attribcount) attrib[i+1]=0;
	  visinfo = glXChooseVisual( dpy, scrnum, attrib );
	}
      }
    }
  }

  if (!visinfo) {
    printf("\nPTB-ERROR[glXChooseVisual() failed]: Couldn't get any suitable visual from X-Server.\n\n");
    return(FALSE);
  }

  // Set window to non-fullscreen mode if it is a transparent or otherwise special window.
  // This will prevent setting the override_redirect attribute, which would lock out the
  // desktop window compositor:
  if (windowLevel < 2000) fullscreen = FALSE;
  
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

  // Create associated GLX OpenGL rendering context: We use ressource
  // sharing of textures, display lists, FBO's and shaders if 'slaveWindow'
  // is assigned for that purpose as master-window. We request a direct
  // rendering context (True) if possible:
  ctx = glXCreateContext(dpy, visinfo, ((windowRecord->slaveWindow) ? windowRecord->slaveWindow->targetSpecific.contextObject : NULL), True );
  if (!ctx) {
    printf("\nPTB-ERROR:[glXCreateContext() failed] OpenGL context creation failed!\n\n");
    return(FALSE);
  }

  // Store the handles...
  windowRecord->targetSpecific.windowHandle = win;
  windowRecord->targetSpecific.deviceContext = dpy;
  windowRecord->targetSpecific.contextObject = ctx;

  // External 3D graphics support enabled?
  if (PsychPrefStateGet_3DGfx()) {
    // Yes. We need to create an extra OpenGL rendering context for the external
    // OpenGL code to provide optimal state-isolation. The context shares all
    // heavyweight ressources likes textures, FBOs, VBOs, PBOs, display lists and
    // starts off as an identical copy of PTB's context as of here.

    // Create rendering context with identical visual and display as main context, share all heavyweight ressources with it:
    windowRecord->targetSpecific.glusercontextObject = glXCreateContext(dpy, visinfo, windowRecord->targetSpecific.contextObject, True);
    if (windowRecord->targetSpecific.glusercontextObject == NULL) {
      printf("\nPTB-ERROR[UserContextCreation failed]: Creating a private OpenGL context for Matlab OpenGL failed for unknown reasons.\n\n");
      return(FALSE);
    }    
  }
  
  // Release visual info:
  XFree(visinfo);

  if ((windowLevel >= 1000) && (windowLevel < 2000)) {
	  // For windowLevels between 1000 and 1999, make the window background transparent, so standard GUI
	  // would be visible, wherever nothing is drawn, i.e., where alpha channel is zero:
	  
	  // Levels 1000 - 1499 and 1500 to 1999 map to a master opacity level of 0.0 - 1.0:	  
	  unsigned int opacity = (unsigned int) (0xffffffff * (((float) (windowLevel % 500)) / 499.0));
	  
	  // Get handle on opacity property of X11:
	  Atom atom_window_opacity = XInternAtom(dpy, "_NET_WM_WINDOW_OPACITY", False);
	  
	  // Assign new value for property:
	  XChangeProperty(dpy, win, atom_window_opacity, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &opacity, 1);
  }


  // Show our new window:
  XMapWindow(dpy, win);

  // Activate the associated rendering context:
  PsychOSSetGLContext(windowRecord);

  // Ok, the OpenGL rendering context is up and running. Auto-detect and bind all
  // available OpenGL extensions via GLEW:
  glerr = glewInit();
  if (GLEW_OK != glerr) {
    /* Problem: glewInit failed, something is seriously wrong. */
    printf("\nPTB-ERROR[GLEW init failed: %s]: Please report this to the forum. Will try to continue, but may crash soon!\n\n", glewGetErrorString(glerr));
    fflush(NULL);
  }
  else {
    printf("PTB-INFO: Using GLEW version %s for automatic detection of OpenGL extensions...\n", glewGetString(GLEW_VERSION));
  }
  
  fflush(NULL);

  // Increase our own open window counter:
  x11_windowcount++;

  // Disable X-Windows screensavers:
  if (x11_windowcount==1) {
    // First window. Disable future use of screensaver:
    XSetScreenSaver(dpy, 0, 0, DefaultBlanking, DefaultExposures);
    // If the screensaver is currently running, forcefully shut it down:
    XForceScreenSaver(dpy, ScreenSaverReset);
  }

  // Some info for the user regarding non-fullscreen and ATI hw:
  if (!(windowRecord->specialflags & kPsychIsFullscreenWindow) && (strstr(glGetString(GL_VENDOR), "ATI"))) {
    printf("PTB-INFO: Some ATI graphics cards may not support proper syncing to vertical retrace when\n");
    printf("PTB-INFO: running in windowed mode (non-fullscreen). If PTB aborts with 'Synchronization failure'\n");
    printf("PTB-INFO: you can disable the sync test via call to Screen('Preference', 'SkipSyncTests', 1); .\n");
    printf("PTB-INFO: You won't get proper stimulus onset timestamps though, so windowed mode may be of limited use.\n");
  }
  fflush(NULL);

  // Check for availability of VSYNC extension:

  // Special case: Buggy ATI driver: Supports the VSync extension and glXSwapIntervalSGI, but provides the
  // wrong extension namestring "WGL_EXT_swap_control" (from MS-Windows!), so GLEW doesn't auto-detect and
  // bind the extension. If this special case is present, we do it here manually ourselves:
  if ( (glXSwapIntervalSGI == NULL) && (strstr(glGetString(GL_EXTENSIONS), "WGL_EXT_swap_control") != NULL) ) {
	// Looks so: Bind manually...
	glXSwapIntervalSGI = glXGetProcAddressARB("glXSwapIntervalSGI");
  }

  // Extension finally supported?
  if (glXSwapIntervalSGI==NULL || ( strstr(glXQueryExtensionsString(dpy, scrnum), "GLX_SGI_swap_control")==NULL &&
	  strstr(glGetString(GL_EXTENSIONS), "WGL_EXT_swap_control")==NULL )) {
	  // No, total failure to bind extension:
	  glXSwapIntervalSGI = NULL;
	  printf("PTB-WARNING: Your graphics driver doesn't allow me to control syncing wrt. vertical retrace!\n");
	  printf("PTB-WARNING: Please update your display graphics driver as soon as possible to fix this.\n");
	  printf("PTB-WARNING: Until then, you can manually enable syncing to VBL somehow in a manner that is\n");
	  printf("PTB-WARNING: dependent on the type of gfx-card and driver. Google is your friend...\n");
  }
  fflush(NULL);

  // ATI Radeon X1000 or later AND first opened onscreen window?
  if ((x11_windowcount == 1) && strstr(glGetString(GL_VENDOR), "ATI") && strstr(glGetString(GL_RENDERER), "Radeon")) {
	  if (strstr(glGetString(GL_RENDERER), "X") || strstr(glGetString(GL_RENDERER), "HD")) {
	  	// Probably an X1000 or later or an HD 2000/3000/later series chip.
		// Try to map its PCI register space to allow for our implementation of
		// beamposition queries:
		if (PsychScreenMapRadeonCntlMemory()) printf("PTB-INFO: ATI-Radeon of model series X1000, HD2000, HD3000 or later detected -- Beamposition queries enabled.\n");
	  }
  }
  
  // Ok, we should be ready for OS independent setup...
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
psych_bool PsychOSOpenOffscreenWindow(double *rect, int depth, PsychWindowRecordType **windowRecord)
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

  // Delete userspace context, if any:
  if (windowRecord->targetSpecific.glusercontextObject) {
    glXDestroyContext(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.glusercontextObject);
    windowRecord->targetSpecific.glusercontextObject = NULL;
  }

  // Wait for X-Server to settle...
  XSync(dpy, 0);

  // Close & Destroy the window:
  XUnmapWindow(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle);

  // Wait for X-Server to settle...
  XSync(dpy, 0);

  XDestroyWindow(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle);
  windowRecord->targetSpecific.windowHandle=0;

  // Wait for X-Server to settle...
  XSync(dpy, 0);

  // Release device context: We just release the reference. The connection to the display is
  // closed somewhere else.
  windowRecord->targetSpecific.deviceContext=NULL;

  // Decrement global count of open onscreen windows:
  x11_windowcount--;

  // Was this the last window?
  if (x11_windowcount<=0) {
    x11_windowcount=0;

    // (Re-)enable X-Windows screensavers if they were enabled before opening windows:
    // Set screensaver to previous settings, potentially enabling it:
    XSetScreenSaver(dpy, -1, 0, DefaultBlanking, DefaultExposures);
    
    // Unmap/release possibly mapped device memory: Defined in PsychScreenGlue.c
    PsychScreenUnmapDeviceMemory();
  }

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
	unsigned int	screenid = windowRecord->screenNumber;
	unsigned int	vsync_counter = 0;
	psych_uint64	ust, msc, sbc;
	CGDirectDisplayID displayID;
	int scrnum;
	
	// Retrieve displayID, aka dpy for this screenid:
	PsychGetCGDisplayIDFromScreenNumber(&displayID, screenid);
	scrnum = PsychGetXScreenIdForScreen(screenid);

	#ifdef GLX_OML_sync_control
	// Ok, this will return VBL count and last VBL time via the OML GetSyncValuesOML call
	// if that extension is supported on this setup. As of mid 2009 i'm not aware of any
	// affordable graphics card that would support this extension, but who knows??
	if ((NULL != glXGetSyncValuesOML) && (glXGetSyncValuesOML((Display*) displayID, (GLXDrawable) RootWindow(displayID, scrnum), (int64_t*) &ust, (int64_t*) &msc, (int64_t*) &sbc))) {
		*vblCount = msc;
		if (PsychGetKernelTimebaseFrequencyHz() > 10000) {
			// Convert ust into regular GetSecs timestamp:
			// At least we hope this conversion is correct...
			return( ((double) ust) / PsychGetKernelTimebaseFrequencyHz() );
		}
		else {
			// Last VBL timestamp unavailable:
			return(-1);
		}
	}
	#else
	#warning GLX_OML_sync_control unsupported! Compile with -std=gnu99 to enable it!
	#endif

	// Do we have SGI video sync extensions?
	if (NULL != glXGetVideoSyncSGI) {
		// Retrieve absolute count of vbl's since startup:
		glXGetVideoSyncSGI(&vsync_counter);
		*vblCount = (psych_uint64) vsync_counter;
		
		// Retrieve absolute system time of last retrace, convert into PTB standard time system and return it:
		// Not yet supported on Linux:
		return(-1);
	}
	else {
		// Unsupported :(
		*vblCount = 0;
		return(-1);
	}
}

/*
    PsychOSFlipWindowBuffers()
    
    Performs OS specific double buffer swap call.
*/
void PsychOSFlipWindowBuffers(PsychWindowRecordType *windowRecord)
{
	// unsigned int		vsync_counter = 0;

	// Execute OS neutral bufferswap code first:
	PsychExecuteBufferSwapPrefix(windowRecord);
	
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

  // Store new setting also in internal helper variable, e.g., to allow workarounds to work:
  windowRecord->vSynced = (swapInterval > 0) ? TRUE : FALSE;

  // Try to set requested swapInterval if swap-control extension is supported on
  // this Linux machine. Otherwise this will be a no-op...
  // if (PTBglXSwapIntervalSGI) PTBglXSwapIntervalSGI(swapInterval);
  if (glXSwapIntervalSGI) glXSwapIntervalSGI(swapInterval);

  return;
}

/*
    PsychOSSetGLContext()
    
    Set the window to which GL drawing commands are sent.  
*/
void PsychOSSetGLContext(PsychWindowRecordType *windowRecord)
{
  if (glXGetCurrentContext() != windowRecord->targetSpecific.contextObject) {
    if (glXGetCurrentContext() != NULL) {
      // We need to glFlush the context before switching, otherwise race-conditions may occur:
      glFlush();
      
      // Need to unbind any FBO's in old context before switch, otherwise bad things can happen...
      if (glBindFramebufferEXT) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
    
    // Switch to new context:
    glXMakeCurrent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle, windowRecord->targetSpecific.contextObject);    
  }
}

/*
    PsychOSUnsetGLContext()
    
    Clear the drawing context.  
*/
void PsychOSUnsetGLContext(PsychWindowRecordType* windowRecord)
{
	if (glXGetCurrentContext() != NULL) {
		// We need to glFlush the context before switching, otherwise race-conditions may occur:
		glFlush();
		
		// Need to unbind any FBO's in old context before switch, otherwise bad things can happen...
		if (glBindFramebufferEXT) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glFlush();
    }
	
	glXMakeCurrent(windowRecord->targetSpecific.deviceContext, None, NULL);
}

/* Same as PsychOSSetGLContext() but for selecting userspace rendering context,
 * optionally copying state from PTBs context.
 */
void PsychOSSetUserGLContext(PsychWindowRecordType *windowRecord, psych_bool copyfromPTBContext)
{
  // Child protection:
  if (windowRecord->targetSpecific.glusercontextObject == NULL) PsychErrorExitMsg(PsychError_user,"GL Userspace context unavailable! Call InitializeMatlabOpenGL *before* Screen('OpenWindow')!");
  
  if (copyfromPTBContext) {
    // This unbind is probably not needed on X11/GLX, but better safe than sorry...
    glXMakeCurrent(windowRecord->targetSpecific.deviceContext, None, NULL);

    // Copy render context state:
    glXCopyContext(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.contextObject, windowRecord->targetSpecific.glusercontextObject, GL_ALL_ATTRIB_BITS);
  }
  
  // Setup new context if it isn't already setup. -> Avoid redundant context switch.
  if (glXGetCurrentContext() != windowRecord->targetSpecific.glusercontextObject) {
    glXMakeCurrent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle, windowRecord->targetSpecific.glusercontextObject);
  }
}
