/*
	SCREENTextModes.c	
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu			awi
  		mario.kleiner@tuebingen.mpg.de	mk

	PLATFORMS:
  
		All.
    
	HISTORY:
  
		11/17/03	awi		Wrote it.
		10/12/04	awi		Changed "SCREEN" to "Screen" in useString.

   
	DESCRIPTION:
  
		Text Modes are things like 'TextFill', 'TextStroke', 'TextFillStroke', 'TextInvisible', 'FillClip',    
		'StrokeClip', 'TextFillStrokeClip' and 'TextClip'.  They are not properites of the font but characterstics
		of how the font is rendered. 
  
	NOTES:

*/


#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "textModes = Screen('TextModes');";
static char synopsisString[] =	"Return a cell array of strings naming allowable text modes for Screen('TextMode').\n"
								"Please note that none of these modes are supported in the current Psychtoolbox, "
                                "except on OSX with its default text renderer 1 aka the Apple CoreText renderer, "
								"so whatever 'TextMode' you set will be silently ignored and have no effect. ";
static char seeAlsoString[] = "TextMode";

PsychError SCREENTextModes(void) 
{
    int							i;
    PsychGenericScriptType		*cellVector;

    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check for valid number of arguments
    PsychErrorExit(PsychCapNumInputArgs(0));   	
    PsychErrorExit(PsychCapNumOutputArgs(1)); 
    
    //generate a cell array to return
    PsychAllocOutCellVector(1, FALSE, kPsychNumTextDrawingModes,  &cellVector);
    for(i=0;i<kPsychNumTextDrawingModes;i++)
        PsychSetCellVectorStringElement(i, PsychTextDrawingModeNames[i], cellVector);
        
    return(PsychError_none);
}
