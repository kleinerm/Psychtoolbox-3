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
	CGDirectDisplayID dpy;

	//The timing array holds time values set by Screen internal diagnostics.  It allocates memory with 
	//malloc to hold the array of times.  This call frees the memory prior to unloading Screen
	ClearTimingArray();
  
	// Close all open onscreen windows and release their resources,
	// -> Perform exactly the same cleanup that Screen('CloseAll') would do.
	ScreenCloseAllWindows();
	CloseWindowBank();

	#if PSYCH_SYSTEM == PSYCH_LINUX
	// Linux specific hack. Close display connection to X-Server. This is a bit unclean.
	// If we ever get around supporting multiple display connections to multiple X11
	// displays, we should create a dedicated cleanup routine in Linux/Screen/PsychScreenGlue.c,
	// but for now this seems to be a bit overkill.
	// As the X11-display is the same for all screens in the current implementation, we
	// just query screen 0 to get a hold on the Display handle...
	PsychGetCGDisplayIDFromScreenNumber(&dpy, 0);
	XCloseDisplay(dpy);
	#endif

	return(PsychError_none);
}




