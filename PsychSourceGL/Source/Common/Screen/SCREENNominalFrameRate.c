/*

	Psychtoolbox3/Source/Common/SCREENNominalFrameRate.c		

	AUTHORS:

		Allen.Ingling@nyu.edu		awi
		mario.kleiner@tuebingen.mpg.de  mk

	PLATFORMS:	

		All systems.

	HISTORY:

		12/04/02  awi		Created.
		06/07/06  mk            Extended to return float precision values on request.
		                        Extended to allow setting the framerate on systems that
					support this (Currently Linux only).

	DESCRIPTION:

		Return the the nominal frame rate as reported by the video subsystem/driver, or
		change the nominal frame rate on systems that support this.

	TO DO:

*/

#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "hz=Screen('NominalFrameRate', windowPtrOrScreenNumber [, mode] [, reqFrameRate]);";
static char synopsisString[] = 
	"Returns or sets the nominal video frame rate in Hz, as reported by your computer's video driver. "
	"'FrameRate' is an alias for 'NominalFrameRate'. By default, this function returns the nominal "
        "framerate, as reported by your operating system, rounded to the closest integral value. If you "
        "set the optional 'mode' flag to 1, then the framerate is returned without rounding to the closest "
        "integer, but at floating point precision, on systems that support this (MacOS-X and Linux). "
        "GNU/Linux only: If you set the 'mode' flag to 2 and specify 'reqFrameRate', then Psychtoolbox "
        "will try to change the current video refresh rate of the display: If 'reqFrameRate' is above "
        "10.0, Screen will try to switch to a framerate as close as possible to 'reqFrameRate' Hz. If "
        "'reqFrameRate' is a positive or negative value smaller than 10.0, then the video setting will be "
        "changed by 'reqFrameRate' system dependent units. The new settings that would result are validated "
        "to make sure they are safe for your display. Invalid settings are rejected, returning a value of "
        "-1. On systems other than Linux, 0 is always returned to signal failure. On successfull framerate "
        "change, the new resulting nominal framerate is returned with double precision. NOTE: 'reqFrameRate' "
        "must be pretty close to the initial framerate, e.g. initial +/- 2 Hz. It is not possible to apply big "
        "changes, e.g., from 60 Hz to 75 Hz. This function is meant for fine-tuning the video refresh interval "
        "for the purpose of synchronizing different displays or the display and other stimulation- or acquisition "
        "devices. You can compensate for small phase-shifts or deviations... "
        "Due to manufacturing tolerances and other noise in your system, the real monitor refresh interval can "
        "differ slightly from the nominal values returned by this function. To query the real, measured framerate "
        "use Screen('GetFlipInterval') instead. "; 

static char seeAlsoString[] = "FrameRate GetFlipInterval";

PsychError SCREENNominalFramerate(void) 
{
    int		screenNumber, opmode;
    double	*rate;
    double      requestedHz;

    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(3));

    //get specified screen number and sanity check the number against the number of connected displays.
    PsychCopyInScreenNumberArg(kPsychUseDefaultArgPosition, TRUE, &screenNumber);

    // Get opmode. Defaults to zero for integral precision query:
    opmode = 0;
    PsychCopyInIntegerArg(2, FALSE, &opmode);

    //Allocate a return matrix.       
    PsychAllocOutDoubleArg(1, FALSE, &rate);

    // Query mode (0 or 1)?
    if (opmode<=1) {
      // Query the float precision nominal frame rate and put it into the return value:
      *rate=(double) PsychGetNominalFramerate(screenNumber);
    }
    else {
      // Query new requested framerate or mode increment:
      PsychCopyInDoubleArg(3, TRUE, &requestedHz);
      
      // Set mode: This is currently only supported on GNU/Linux:
      #if PSYCH_SYSTEM == PSYCH_LINUX
      // Call the Set-function, it will process and return the new nominal framerate:
      *rate=(double) PsychSetNominalFramerate(screenNumber, (float) requestedHz);
      #else
      // Return 0 to signal the unsupported feature:
      *rate = 0;
      #endif
    }

    // Round it to closest integer, if opmode <= 0:
    if (opmode <= 0) *rate = (double) ((int) (*rate + 0.5));

    // Done.
    return(PsychError_none);
}

