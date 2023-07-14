/*
  SCREENUpdateShadow.c	
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	All
    

  HISTORY:
  4/22/04  awi		Created.    
 
  DESCRIPTION:
  
	
  NOTES:
  
  
  RESOURCES:

*/


#include "Screen.h"


static char useString[] = "SCREEN('UpdateShadow', windowPtr);";
static char synopsisString[] = 
	"For the window designated by sourceWindowPntr create and fill a shadow surface for accelerated copying between windows using 'CopyWindow' "
	"If the surface already exists then make sure that its contents are up to date.";
	 
static char seeAlsoString[] = "";

PsychError SCREENUpdateShadow(void) 
{
    PsychWindowRecordType 	*sourceWin;
    
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous or missing arguments
	PsychErrorExit(PsychCapNumInputArgs(2));   	
    PsychErrorExit(PsychRequireNumInputArgs(1)); 	

	PsychAllocInWindowRecordArg(1, TRUE, &sourceWin);
	//	PsychUpdateShadow(sourceWin);				//update will allocate if necessary
	
    return(PsychError_none);

}



	
	






