/*
	SCREENTextSize.c	
  
	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:
	
		Only OS X for now.
    

	HISTORY:

		11/18/03	awi		Wrote it.
		10/12/04	awi		In useString: changed "SCREEN" to "Screen", and moved commas to inside [].

   
	DESCRIPTION:
  
		Sets the text size for the specified window record.
  
	NOTES:

*/


#include "Screen.h"
// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] ="oldTextSize=Screen('TextSize', windowPtr [,textSize]);";
//                                            0          1           2
static char synopsisString[] = 
    "Read/Set the text size for the specified window. The OS-X and M$-Windows versions accept "
    "any text size as they use TrueType fonts for representing text. The GNU/Linux version "
    "currently is pretty picky. You need to supply one of a limited number of available text "
    "sizes for the current font, otherwise Screen('DrawText') will fail with an error message. ";
static char seeAlsoString[] = "";

PsychError SCREENTextSize(void) 
{

    boolean						doSetSize;
    PsychWindowRecordType		*windowRecord;
    int							oldTextSize, newTextSize;
    
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check for valid number of arguments
    PsychErrorExit(PsychRequireNumInputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));   	
    PsychErrorExit(PsychCapNumOutputArgs(1)); 
    
    //Get the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
    
    //Save the old text size value and return it.
    oldTextSize=windowRecord->textAttributes.textSize;
    PsychCopyOutDoubleArg(1, FALSE, (double)oldTextSize);
    
    //Fetch and set the new size if it is specified. 
    doSetSize= PsychCopyInIntegerArg(2, FALSE, &newTextSize);
    if(doSetSize) {
      windowRecord->textAttributes.needsRebuild=(windowRecord->textAttributes.textSize != newTextSize) ? TRUE : FALSE;
      windowRecord->textAttributes.textSize=newTextSize;
    }

    return(PsychError_none);

}


	






