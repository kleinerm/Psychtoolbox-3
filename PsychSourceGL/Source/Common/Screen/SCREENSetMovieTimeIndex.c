/*

Psychtoolbox3/Source/Common/SCREENSetMovieTimeIndex.c		

 

 AUTHORS:

 mario.kleiner at tuebingen.mpg.de   mk

 

 PLATFORMS:	

 This file should build on any platform. 

 

 HISTORY:

 10/23/05  mk		Created. 

 

 DESCRIPTION:

 

 Set current playback time index in a  previously opened movie file.

 

 On OS-X, all movie/multimedia handling functions are implemented via the Apple Quicktime API,

 version 7 or later. On a later Windows port we'll probably do the same, but for other OS'es,

 e.g., Linux, we would use a different multimedia engine.

 

 TO DO:

 

 

 

 

 */





#include "Screen.h"





static char useString[] = "[oldtimeindex] = Screen('SetMovieTimeIndex', moviePtr, timeindex);";

static char synopsisString[] = "Set current time index for movie object 'moviePtr'.";

static char seeAlsoString[] = "CloseMovie PlayMovie GetMovieImage GetMovieTimeIndex SetMovieTimeIndex";



PsychError SCREENSetMovieTimeIndex(void) 

{

    int             moviehandle = -1;

    double          timeindex = 0;

    

    // All sub functions should have these two lines

    PsychPushHelp(useString, synopsisString, seeAlsoString);

    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    

    PsychErrorExit(PsychCapNumInputArgs(2));            // Max. 2 input args.

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



    // Setup and return current movie time index:

    PsychCopyOutDoubleArg(1, FALSE, PsychSetMovieTimeIndex(moviehandle, timeindex));

    

    // Ready!    

    return(PsychError_none);

}


