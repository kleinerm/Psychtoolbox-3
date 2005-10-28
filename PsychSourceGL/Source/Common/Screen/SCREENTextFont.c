/*
	SCREENTextFont.c	

	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 

	PLATFORMS:	
	
		Only OS X for now.

	HISTORY:

		11/19/03	awi		Wrote it.

	DESCRIPTION:

		Sets the text font for the specified window record.

	NOTES:

		TO DO: Make sure that the thing which instantiates window records inits the text style to normal. 

*/




#include "Screen.h"
// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] ="[oldFontName,oldFontNumber]=Screen('TextFont', windowPtr [,fontNameOrNumber]);";
//                                                           0          1           2
static char synopsisString[] = 
    "Get/set the font for future text draws in this window. You can ask what the "
    "current font is, or specify the desired font by number or by name (e.g. "
    "'Helvetica'). Font numbers are not consistent from Mac to Mac, so use font names "
    "for reliability. The font name can be a string, e.g. 'Helvetica', or a list "
    "containing one string, e.g. {'Helvetica'}. The default font (set by the Mac OS) "
    "is 1 (the \"application\" font, typically Geneva) for an on-screen window and 0 "
    "(the \"system\" font, typically Charcoal) for an off-screen window. It's ok to "
    "request a non-existent font; this will have no effect. If you care, call "
    "TextFont again to find out whether you got the font you requested. See FontDemo. ";

static char seeAlsoString[] = "";

PsychError SCREENTextFont(void) 
{

    boolean			doSetByName, doSetByNumber, foundFont;
    PsychWindowRecordType	*windowRecord;
    PsychFontStructType		*fontRecord;
    int				oldTextFontNumber, inputTextFontNumber;
    char			*oldTextFontName, *inputTextFontName;
    
    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check for valid number of arguments
    PsychErrorExit(PsychRequireNumInputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));   	
    PsychErrorExit(PsychCapNumOutputArgs(2)); 
    
    //Get the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
    
    //Save the old text size value and return it.
    oldTextFontNumber=windowRecord->textAttributes.textFontNumber;
    PsychCopyOutDoubleArg(2, FALSE, (double)oldTextFontNumber);
    oldTextFontName=windowRecord->textAttributes.textFontName;
    PsychCopyOutCharArg(1, FALSE, oldTextFontName); 
    
	
    //Fetch and set the new font if specified by name or number
    PsychCheckInputArgType(2, kPsychArgOptional, PsychArgType_double | PsychArgType_char);  //if the argument is there check that it is the right type.
    doSetByNumber= PsychCopyInIntegerArg(2, kPsychArgAnything, &inputTextFontNumber);
    doSetByName= PsychAllocInCharArg(2, kPsychArgAnything, &inputTextFontName);
    foundFont=0;
    if(doSetByNumber)
        foundFont=PsychGetFontRecordFromFontNumber(inputTextFontNumber, &fontRecord);
    if(doSetByName)
        foundFont=PsychGetFontRecordFromFontFamilyNameAndFontStyle(inputTextFontName, windowRecord->textAttributes.textStyle, &fontRecord);
    if(foundFont){
        strncpy(windowRecord->textAttributes.textFontName, fontRecord->fontFMFamilyName, 255);
        windowRecord->textAttributes.textFontNumber= fontRecord->fontNumber;
    }
    
    return(PsychError_none);

}


	





