/*
  SCREENSetGLSynchonous.cpp		
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
  
  PLATFORMS:	All
    

  HISTORY:
  12/18/01  awi		Created.  Copied the Synopsis string from old version of psychtoolbox.  
 
  TO DO:
  

*/


#include "Screen.h"


//define variables local to SCREENSetGLSynchonous.cpp
static boolean isSynchSETGLSYNC=0;  

//declare local functions.
void PsychGLFlush(void);

void PsychGLFlush(void)
{
	if(isSynchSETGLSYNC)
		glFinish();
	else
		glFlush();
}


static char useString[] = "synchFlagRead = SCREEN('SetGLSynchronous', [synchFlagSet]);";
static char synopsisString[] =
	"This command is useless, Don't use it! ";  
static char seeAlsoString[] = "";

PsychError SCREENSetGLSynchronous(void) 
{

	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//get argument values from inputs. 
	PsychAllocInFlagArg(1, FALSE, &isSynchSETGLSYNC);
	

	//return the window index and the rect
	PsychCopyOutFlagArg(1, FALSE, isSynchSETGLSYNC);
	
	return(PsychError_none);
}




