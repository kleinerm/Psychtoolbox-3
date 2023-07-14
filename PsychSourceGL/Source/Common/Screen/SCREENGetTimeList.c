/*
	SCREENGetTimeList.c	
  
	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:
	
		Only OS X for now.
    

	HISTORY:

		1/19/05	awi		Created.  
  
 
	DESCRIPTION:
  
		Returns a vector of doubles holding times as reported by GetSecs. Used for internal testing of Screen. 

*/


#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "timeList= Screen('GetTimelist');";
//                          
static char synopsisString[] = 
	"Return a vector of doubles holding times as reported by GetSecs.  When debugging is enabled for particular  "
	"Screen subfunctions using a Screen preference setting, diagnostics may store time values in an array held by Screen."
	" GetTimelist returns that array. The array is cleared by using the Screen 'ClearTimeList' command.";
static char seeAlsoString[] = "ClearTimeList";
	 

PsychError SCREENGetTimeList(void) 
{
	unsigned int	numTimeValues;
	double			*timeValueArray;
	
	
	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//cap the numbers of inputs and outputs
	PsychErrorExit(PsychCapNumInputArgs(0));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(1));  //The maximum number of outputs
	
	//return the array
	numTimeValues=GetNumTimeValues();
	PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, numTimeValues, 1, &timeValueArray);
	CopyTimeArray(timeValueArray,numTimeValues);
	
	return(PsychError_none);
	
}


	
	






