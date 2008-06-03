/*

  Psychtoolbox3/Source/Common/SCREENScreens.c		

  AUTHORS:

  Allen.Ingling@nyu.edu					awi 
  mario.kleiner at tuebingen.mpg.de		mk

  PLATFORMS:	

  This file should build on any platform. 

  HISTORY:

	11/26/02  awi		Created. 
	10/12/04	awi		Changed "SCREEN" to "Screen" in useString.
	

  DESCRIPTION:

  Return the number of screens.

  TO DO:

*/

#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "screenNumbers=Screen('Screens' [, physicalDisplays]);";
static char synopsisString[] = 
		"Return an array of screenNumbers. If this command is executed on Microsoft Windows in a "
        "multi-display configuration, then the following rule applies: Screen 0 is always the full "
        "Windows desktop. Screens 1 to n are corresponding to windows display monitors 1 to n. If "
        "you want to open an onscreen window only on one specific display, or you want to query or "
        "set the properties of a display (e.g., framerate, size, color depth or gamma tables), use "
        "the screen numbers 1 to n. If you want to open a window suitable for stereo display on a "
        "dual display setup, use screen zero.\n"
		"When executed on OS/X with the optional 'physicalDisplays' flag set to 1, will enumerate "
		"the set of physical displays, which can be different from the set of logical displays "
		"that is returned by default. E.g., in mirror mode or clone mode, there is only one logical "
		"display, representing the mirror set of all physical displays. This is mostly useful to "
		"set, e.g., gamma tables on a per display basis, even if in mirror mode.\n";

static char seeAlsoString[] = "";

PsychError SCREENScreens(void) 
{
    int i, numDisplays;
    double *displayNumList;
	int physicalDisplays = 0;
	int startidx = 0;
	
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));

	// Copy in optional flag:
	PsychCopyInIntegerArg(1, FALSE, &physicalDisplays);
	
	if (PSYCH_SYSTEM == PSYCH_OSX && physicalDisplays > 0) {
		// We have one-based indexing on OS/X for physical displays:
		startidx = 1;
		
		#if PSYCH_SYSTEM == PSYCH_OSX
			// Get the number of connected physical displays:
			numDisplays=PsychGetNumPhysicalDisplays();
		#endif
	}
	else {
		// Get the number of connected logical displays:
		numDisplays=PsychGetNumDisplays();
	}

    //Allocate an output matrix.  Even if argument is not there, we still get the space.
    PsychAllocOutDoubleMatArg(1, FALSE, 1, numDisplays, 0, &displayNumList);

    // Fill the return matrix
    for(i=0; i < numDisplays; i++) displayNumList[i] = i + startidx;
	
    return(PsychError_none);
}
