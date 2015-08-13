/*

	SCREENTextColor.c	
  
	AUTHORS:

		Allen.Ingling@nyu.edu			awi 
  		mario.kleiner@tuebingen.mpg.de	mk

	PLATFORMS:	
	
		All.
    

	HISTORY:
	
		1/25/04		awi		Wrote it.
		10/12/04	awi		In useString: changed "SCREEN" to "Screen", and moved commas to inside [].

	DESCRIPTION:
  
		Sets the text color for the specified window record.
  
	NOTES:

*/


#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] ="oldTextColor=Screen('TextColor', windowPtr [,colorVector]);";
//                                                        1           2
static char synopsisString[] = 
    "Read/Set the text color for the specified window";
static char seeAlsoString[] = "";


PsychError SCREENTextColor(void) 
{
    psych_bool				doSetColor;
    PsychWindowRecordType	*winRec;
	PsychColorType			colorArg;

    // All subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check for valid number of arguments
    PsychErrorExit(PsychRequireNumInputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));   	
    PsychErrorExit(PsychCapNumOutputArgs(1)); 
    
    //Get the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &winRec);
	
	//Coerce the current color record to the correct type in case it has not been accessed yet.
	PsychSetTextColorInWindowRecord(&(winRec->textAttributes.textColor),  winRec);
	PsychCopyOutColorArg(1, kPsychArgOptional, &(winRec->textAttributes.textColor), winRec);
	
    //Get the new color record, coerce it to the correct mode, and store it.  
    doSetColor=PsychCopyInColorArg(2, kPsychArgOptional, &colorArg);
	if(doSetColor) PsychSetTextColorInWindowRecord(&colorArg,  winRec);
	
    return(PsychError_none);
}
