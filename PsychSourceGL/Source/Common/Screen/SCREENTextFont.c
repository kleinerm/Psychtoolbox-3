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
    "request a non-existent font on OS-X; this will have no effect. If you care, call "
    "TextFont again to find out whether you got the font you requested. See FontDemo. "
    "On M$-Windows and GNU/Linux you can only pass font names. Font numbers are ignored."
    "On Linux you can either provide a font name - PTB will select a matching font with "
    "that name that also fullfills the size and style requirements - or you can supply "
    "a full X-Windows font specifier string which encodes all kinds of properties. The "
    "xfontsel -print command under Linux allows you to query all available fonts and "
    "provides such a spec-string on request. Linux is picky about the supplied fonts "
    "- if you request a non-existent font, the DrawText command will fail with an "
    "error message. This limitation will be removed in a future release of PTB.";

static char seeAlsoString[] = "";

PsychError SCREENTextFont(void) 
{
    boolean			doSetByName, doSetByNumber, foundFont;
    PsychWindowRecordType	*windowRecord;
#if PSYCH_SYSTEM == PSYCH_OSX
    PsychFontStructType		*fontRecord;
#endif
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
    
    //Save the old text font value and return it.
    oldTextFontNumber=windowRecord->textAttributes.textFontNumber;
    PsychCopyOutDoubleArg(2, FALSE, (double)oldTextFontNumber);
    oldTextFontName=windowRecord->textAttributes.textFontName;
    PsychCopyOutCharArg(1, FALSE, oldTextFontName); 
    
	
    //Fetch and set the new font if specified by name or number
    PsychCheckInputArgType(2, kPsychArgOptional, PsychArgType_double | PsychArgType_char);  //if the argument is there check that it is the right type.
    doSetByNumber= PsychCopyInIntegerArg(2, kPsychArgAnything, &inputTextFontNumber);
    doSetByName= PsychAllocInCharArg(2, kPsychArgAnything, &inputTextFontName);
    foundFont=0;
#if PSYCH_SYSTEM == PSYCH_OSX
    if(doSetByNumber)
        foundFont=PsychGetFontRecordFromFontNumber(inputTextFontNumber, &fontRecord);
    if(doSetByName)
        foundFont=PsychGetFontRecordFromFontFamilyNameAndFontStyle(inputTextFontName, windowRecord->textAttributes.textStyle, &fontRecord);
    if(foundFont){
        strncpy(windowRecord->textAttributes.textFontName, fontRecord->fontFMFamilyName, 255);
        windowRecord->textAttributes.textFontNumber= fontRecord->fontNumber;
    }
    
    return(PsychError_none);
#else
    // Special case for M$-Windows:
    if(doSetByNumber) printf("PTB-WARNING: Sorry, selecting font by number in Screen('TextFont') is not yet supported on Windows or Linux. Command ignored.\n");
    if(doSetByName && (strncmp(windowRecord->textAttributes.textFontName, inputTextFontName, 255 )!=0)) {
      strncpy(windowRecord->textAttributes.textFontName, inputTextFontName, 255);
      windowRecord->textAttributes.textFontNumber= 0;
      // Set the rebuild flag:
      windowRecord->textAttributes.needsRebuild=TRUE;
    }

    return(PsychError_none);
#endif

}


	






