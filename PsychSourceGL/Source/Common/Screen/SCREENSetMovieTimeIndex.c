/*
	 Psychtoolbox3/PsychSourceGL/Source/Common/Screen/SCREENSetMovieTimeIndex.c		

	 AUTHORS:
	 
	 mario.kleiner at tuebingen.mpg.de   mk
	 
	 PLATFORMS:	
	 
	 This file should build on any platform. 
	 
	 HISTORY:
	 
	 10/23/05  mk		Created. 
	 09/03/09  mk		Add ability to seek in frames instead of seconds.
	 
	 DESCRIPTION:
	 
	 Set current playback time index in a  previously opened movie file.
	 
	 TO DO:
 
*/

#include "Screen.h"

static char useString[] = "[oldtimeindex] = Screen('SetMovieTimeIndex', moviePtr, timeindex [, indexIsFrames=0]);";
static char synopsisString[] =	"Set current time index for movie object with handle 'moviePtr'.\n\n"
								"The new time index is specified in 'timeindex'. By default, or if the "
								"optional 'indexIsFrames' flag is set to zero, 'timeindex' is in seconds "
								"of movie time. If 'indexIsFrames' is set to 1, then 'timeindex' is interpreted "
								"as a frameindex in frames since start of movie, starting with frame 0 as the "
								"first frame in the movie.\n\n"
								"Specifying a new timeindex in seconds is usually faster than specifying a "
								"timeindex in frames.\n\n"
								"The function optionally returns the old position in seconds in the return "
								"argument 'oldtimeindex'.\n";

static char seeAlsoString[] = "CloseMovie PlayMovie GetMovieImage GetMovieTimeIndex SetMovieTimeIndex";

PsychError SCREENSetMovieTimeIndex(void)
{
    int             moviehandle = -1;
    double          timeindex = 0;
	int				indexIsFrames = 0;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    PsychErrorExit(PsychCapNumInputArgs(3));            // Max. 3 input args.
    PsychErrorExit(PsychRequireNumInputArgs(2));        // Min. 2 input args required.
    PsychErrorExit(PsychCapNumOutputArgs(1));           // One output arg.

    // Get the movie handle:
    PsychCopyInIntegerArg(1, TRUE, &moviehandle);
    if (moviehandle==-1) {
        PsychErrorExitMsg(PsychError_user, "SetMovieTimeIndex called without valid handle to a movie object.");
    }

    // Get the timeindex:
    PsychCopyInDoubleArg(2, TRUE, &timeindex);
    if (timeindex < 0) {
        PsychErrorExitMsg(PsychError_user, "SetMovieTimeIndex called with invalid (negative) time index.");
    }

	// Get the optional indexIsFrames flag, which defaults to zero aka false aka index is seconds:
    PsychCopyInIntegerArg(3, FALSE, &indexIsFrames);
    if (indexIsFrames < 0 || indexIsFrames > 1) {
        PsychErrorExitMsg(PsychError_user, "SetMovieTimeIndex called with invalid 'indexIsFrames' parameter! Valid settings are 0 and 1.");
    }

    // Setup and return current movie time index:
    PsychCopyOutDoubleArg(1, FALSE, PsychSetMovieTimeIndex(moviehandle, timeindex, (psych_bool) indexIsFrames));

    // Ready!
    return(PsychError_none);
}
