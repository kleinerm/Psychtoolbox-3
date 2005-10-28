/*
  SCREENTexturizeWindow.c	
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	All
    

  HISTORY:
  3/13/03  awi		Created.    
 
  DESCRIPTION:
  
	
  NOTES:
  
  
  RESOURCES:

*/


#include "Screen.h"


static char useString[] = "SCREEN('TexturizeOffscreenWindows');";
static char synopsisString[] = 
	"For all offscreen windows create textures which will accelerate copying by CopyWindow to "
	"to target windows.";
	 
static char seeAlsoString[] = "";


PsychError SCREENTexturizeOffscreenWindows(void) 
{
	PsychWindowRecordType	**windowRecordArray;
	int						i, numWindows; 

    
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous or missing arguments
	PsychErrorExit(PsychCapNumInputArgs(0));   	
    PsychErrorExit(PsychRequireNumInputArgs(0)); 	

   	PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);
	for(i=0;i<numWindows;i++){
			if(PsychIsOffscreenWindow(windowRecordArray[i])){
				PsychUpdateTargetWindowFromTargetDisplay(windowRecordArray[i]);
				PsychAllocateTexture(windowRecordArray[i]);
				PsychBindTexture(windowRecordArray[i]);
				PsychUpdateTexture(windowRecordArray[i]);
			}
	}
	PsychDestroyVolatileWindowRecordPointerList(windowRecordArray);

	
    return(PsychError_none);

}



	
	





