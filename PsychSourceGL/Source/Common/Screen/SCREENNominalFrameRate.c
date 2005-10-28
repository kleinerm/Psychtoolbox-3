/*

	Psychtoolbox3/Source/Common/SCREENNominalFrameRate.c		

  

	AUTHORS:



		Allen.Ingling@nyu.edu		awi 

  

	PLATFORMS:	



		Only OS X for now.

		

	HISTORY:

		12/04/02  awi		Created. 

 

	DESCRIPTION:

  

		Return the the nominal frame rate as reported by the video subsystem/driver.  

  

	TO DO:

  

*/





#include "Screen.h"



// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c

static char useString[] = "hz=Screen('NominalFrameRate', windowPtrOrScreenNumber);";

static char synopsisString[] = 

	"The nominal vide frame rate in Hz, as reported by your computer's video driver.  On OS 9, 'FrameRate' returns the measured frame rate, "

	"which is more precise than the nominal rate.  On Windows and and OS X, 'FrameRate' is an alias for 'NominalFrameRate'."; 



static char seeAlsoString[] = "FrameRate";

	 

PsychError SCREENNominalFramerate(void) 

{

    int		screenNumber;

    double	*rate;

    

	

    //all sub functions should have these two lines

    PsychPushHelp(useString, synopsisString, seeAlsoString);

    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};



    //check to see if the user supplied superfluous arguments

    PsychErrorExit(PsychCapNumOutputArgs(1));

    PsychErrorExit(PsychCapNumInputArgs(1));

    

    //get specified screen number and sanity check the number against the number of connected displays.

    PsychCopyInScreenNumberArg(kPsychUseDefaultArgPosition, TRUE, &screenNumber);

    

    //Allocate a return matrix.       

    PsychAllocOutDoubleArg(1, FALSE, &rate);

    

    //get the frame rate and put it into the return value.

    *rate=(double)PsychGetNominalFramerate(screenNumber);



    return(PsychError_none);	

}













