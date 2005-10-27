/*
  Psychtoolbox3/Source/Common/SCREENScreens.c		
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
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
static char useString[] = "screenNumbers=Screen('Screens);";
static char synopsisString[] = 
	"Return an array of screenNumbers. ";
static char seeAlsoString[] = "";
	 

PsychError SCREENScreens(void) 
{
    int i, numDisplays;
    double *displayNumList;
	
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));
    
    //get the number of connected displays
    numDisplays=PsychGetNumDisplays();
    
    //Allocate an output matrix.  Even if argument is not there, we still get the space.     
    PsychAllocOutDoubleMatArg(1, FALSE, 1, numDisplays, 0, &displayNumList);
    
    //enter the return matrix
    for(i=0;i<numDisplays;i++)
        displayNumList[i]=i;

    return(PsychError_none);	
}






