/*
	PsychToolbox2/Source/Common/Screen/ScreenExit.cpp		

	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 

	PLATFORMS:	
	
		Only OS X fow now.  


	HISTORY:
	
		12/20/01	awi		Created.
		1/25/04		awi		Added update provided by mk. It makes the ScreenCloseAllWindows call.  
                7/22/05         mk              Added call to CloseWindowBank() to free dynamic window bank array.
	DESCRIPTION:
	
		ScreenExitFunction is called before the Screen module is flushed.

	T0 DO:
		
		


*/

#include "Screen.h"

PsychError ScreenExitFunction(void)
{
  CGDirectDisplayID dpy, last_dpy;
  int i;

	//The timing array holds time values set by Screen internal diagnostics.  It allocates memory with 
	//malloc to hold the array of times.  This call frees the memory prior to unloading Screen
	ClearTimingArray();
  
	// Close all open onscreen windows and release their resources,
	// -> Perform exactly the same cleanup that Screen('CloseAll') would do.
	ScreenCloseAllWindows();
	CloseWindowBank();

	#if PSYCH_SYSTEM == PSYCH_LINUX
	// Linux specific hack. Close display connection(s) to X-Server(s). This is a bit unclean.
	last_dpy = NULL;
	// Go trough full screen list:
	for (i=0; i < PsychGetNumDisplays(); i++) {
	  // Get display-ptr for this screen:
	  PsychGetCGDisplayIDFromScreenNumber(&dpy, i);
	  // Did we close this connection already (dpy==last_dpy)?
	  if (dpy != last_dpy) {
	    // Nope. Keep track of it...
	    last_dpy=dpy;
	    // ...and close display connection to X-Server:
	    XCloseDisplay(dpy);
	  }	  
	}

	// All connections should be closed now. We can't NULL-out the display list, but
	// Matlab will flush the Screen - Mexfile anyway...

	#endif

	return(PsychError_none);
}

