/*
  SCREENInitIntrinsicPreferences.c		
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	Mac OS X MATLAB only.
    

  HISTORY:
  2/28/04  awi		Created.   
 
  
  DESCRIPTION:
  
  Initiailize all the Psychtoolbox preferences which Screen intialises.
  
  This File is completely useless and should be removed! (MK)
  
*/


#include "Screen.h"

void PsychSetIntrinsicScreenPreferences(void);


static char useString[] = "Screen('InitIntrinsicPreferences')";
//                                                   
static char synopsisString[] = 
	"Part of the intialization of MATLAB preferences for the Psychtoolbox.  Users should call this function.  "
	"Instead, use \"PreparePsychtoolboxPreferences\" to intialized preferrences.  Though Psychtoolbox functions "
	"Which access the preferences will automatically call PreparePsychPreferences, so that should not "
	"usually be necessary.";
static char seeAlsoString[] = "";	

PsychError SCREENInitIntrinsicPreferences(void)  
{

	
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString,seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous or missing arguments
	PsychErrorExit(PsychCapNumInputArgs(0));			

	//	PsychSetIntrinsicScreenPreferences();
	
	return(PsychError_none);
}





