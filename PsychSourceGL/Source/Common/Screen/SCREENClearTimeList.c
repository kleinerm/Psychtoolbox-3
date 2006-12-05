/*
	SCREENClearTimeList.c	
  
	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:
	
		Only OS X for now.
    

	HISTORY:

		1/26/05	awi		Created.  
  
 
	DESCRIPTION:
  
		Clears the list of times held by Screen.  Time values, as returned by GetSecs, are added to the time list by 
		internal debugging routines enabled by Screen preferences. 
		Time values are read out of Screen by GetTimeList.  

*/


#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('ClearTimelist');";
//                          
static char synopsisString[] = 
		"Clears the list of times held by Screen.  Time values, as returned by GetSecs, are added to the time list by " 
		"internal debugging routines using Screen preferences. Time values are read out of Screen using GetTimeList. ";  
static char seeAlsoString[] = "GetTimeList";
	 

PsychError SCREENClearTimeList(void) 
{
	
	
	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//cap the numbers of inputs and outputs
	PsychErrorExit(PsychCapNumInputArgs(0));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs
	
	//return the array
	ClearTimingArray();
	
	return(PsychError_none);
	
}


	
	






