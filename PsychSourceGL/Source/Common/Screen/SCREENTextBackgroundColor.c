/*

	SCREENTextBackgroundColor.c	
  
	AUTHORS:

		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
	
		Only OS X for now.
    

	HISTORY:
	
		3/11/04		awi		Wrote it.

	DESCRIPTION:
  
		Sets the text color for the specified window record.
  
	NOTES:

*/


#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] ="oldTextBackgroundColor=Screen('TextBackgroundColor', windowPtr [,colorVector]);";
//                                                        1           2
static char synopsisString[] = 
    "Read/Set the text background color for the specified window";
static char seeAlsoString[] = "";


PsychError SCREENTextBackgroundColor(void) 
{

    Boolean					doSetColor;
    PsychWindowRecordType	*winRec;
	PsychColorType			colorArg;
	
    
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check for valid number of arguments
    PsychErrorExit(PsychRequireNumInputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));   	
    PsychErrorExit(PsychCapNumOutputArgs(1)); 
    
    //Get the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &winRec);
	
	//Coerce the current color record to the correct type in case it has not been accessed yet.
	//  and return it. 
	PsychSetTextBackgroundColorInWindowRecord(&(winRec->textAttributes.textBackgroundColor),  winRec);
	PsychCopyOutColorArg(1, kPsychArgOptional, &(winRec->textAttributes.textBackgroundColor));
	
    //Get the new color record, coerce it to the correct mode, and store it.  
    doSetColor=PsychCopyInColorArg(2, kPsychArgOptional, &colorArg);
	if(doSetColor)
		PsychSetTextBackgroundColorInWindowRecord(&colorArg,  winRec);
        
    return(PsychError_none);

}


	






