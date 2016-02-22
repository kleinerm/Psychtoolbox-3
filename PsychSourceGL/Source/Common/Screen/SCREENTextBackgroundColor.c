/*

	SCREENTextBackgroundColor.c	
  
	AUTHORS:

		Allen.Ingling@nyu.edu			awi
		mario.kleiner@tuebingen.mpg.de	mk
  
	PLATFORMS:	
	
		All.
    
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
    "Read/Set the text background color for the specified window.\n"
	"The background color defaults to [0,0,0,0], i.e., a fully transparent black. "
	"This means that the background is invisible. You'll need to assign at least a "
	"non-zero alpha-value for the background to be drawn. With some text renderers "
	"you'll also need to enable user-controlled text alpha-blending via a call to "
	"Screen('Preference', 'TextAlphaBlending', 1); for text background to be drawn, "
    "e.g., for the Apple OSX legacy text renderer 0. If you want text to be drawn "
    "with sub-pixel anti-aliasing then some renderers need you to set an opaque "
    "text background color - with an alpha value of 255, or whatever the maximum "
    "value is for the selected color mode.\n";
	
static char seeAlsoString[] = "";

PsychError SCREENTextBackgroundColor(void) 
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
	
	// Coerce the current color record to the correct type in case it has not been accessed yet.
	PsychSetTextBackgroundColorInWindowRecord(&(winRec->textAttributes.textBackgroundColor), winRec);
	PsychCopyOutColorArg(1, kPsychArgOptional, &(winRec->textAttributes.textBackgroundColor), winRec);

    //Get the new color record, coerce it to the correct mode, and store it.  
    doSetColor=PsychCopyInColorArg(2, kPsychArgOptional, &colorArg);
	if(doSetColor) PsychSetTextBackgroundColorInWindowRecord(&colorArg,  winRec);
	
    return(PsychError_none);
}
