/*
  Psychtoolbox3/Source/Common/SCREENOpenMovie.c		
  
  AUTHORS:
    mario.kleiner at tuebingen.mpg.de   mk
  
  PLATFORMS:	
  This file should build on any platform. 

  HISTORY:
  10/23/05  mk		Created. 
 
  DESCRIPTION:
  
  Open a named movie file from the filesystem, create and initialize a corresponding movie object
  and return a handle to it to MATLAB space.
 
  On OS-X, all movie/multimedia handling functions are implemented via the Apple Quicktime API,
  version 7 or later. On a later Windows port we'll probably do the same, but for other OS'es,
  e.g., Linux, we would use a different multimedia engine.
  
  TO DO:
  
  
  

*/


#include "Screen.h"


static char useString[] = "[ moviePtr [count] [duration] [fps]]=Screen('OpenMovie', windowPtr, moviefile);";
static char synopsisString[] = 
	"Open the named multimediafile 'moviefile' for visual playback in onscreen window 'windowPtr' and "
        "return a handle 'moviePtr' on successfull completion. On OS-X, media files are handled by use of "
        "Apples Quicktime-7 API. On other platforms, the playback engine may be different from Quicktime. "
        "'count' Total number of videoframes in the movie. 'duration' Total duration of movie in seconds. "
        "'fps' Video playback framerate, assuming a linear spacing of videoframes in time.";

static char seeAlsoString[] = "CloseMovie PlayMovie GetMovieImage GetMovieTimeIndex SetMovieTimeIndex";
	 

PsychError SCREENOpenMovie(void) 
{
        PsychWindowRecordType			*windowRecord;
        char                                    *moviefile;
        int                                     moviehandle = -1;
        int                                     framecount;
        double                                  durationsecs;
        double                                  framerate;
        
	// All sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

        PsychErrorExit(PsychCapNumInputArgs(2));            // Max. 2 input args.
        PsychErrorExit(PsychRequireNumInputArgs(2));        // Min. 2 input args required.
        PsychErrorExit(PsychCapNumOutputArgs(4));           // Max. 4 output args.
        
        // Get the window record from the window record argument and get info from the window record
        PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
        // Only onscreen windows allowed:
        if(!PsychIsOnscreenWindow(windowRecord)) {
            PsychErrorExitMsg(PsychError_user, "OpenMovie called on something else than an onscreen window.");
        }
        
        // Get the movie name string:
        moviefile = NULL;
        PsychAllocInCharArg(2, kPsychArgRequired, &moviefile);

	// Try to open the named 'moviefile' and create & initialize a corresponding movie object.
        // A MATLAB handle to the movie object is returned upon successfull operation.
        // TESTCODE fuer Movies:
        PsychCreateMovie(windowRecord, moviefile, &moviehandle);
        
        // Upon sucessfull completion, we'll have a valid handle in 'moviehandle'.
        // Return it to Matlab-world:
        PsychCopyOutDoubleArg(1, TRUE, (double) moviehandle);

        // Retrieve infos about new movie:
        PsychGetMovieInfos(moviehandle, &framecount, &durationsecs, &framerate, NULL);
        PsychCopyOutDoubleArg(2, FALSE, (double) framecount);
        PsychCopyOutDoubleArg(3, FALSE, (double) durationsecs);
        PsychCopyOutDoubleArg(4, FALSE, (double) framerate);
        
	// Ready!
        return(PsychError_none);
}




