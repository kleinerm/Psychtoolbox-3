/*
	SCREENTextBounds.c	
  
    AUTHORS:
    
		Allen.Ingling@nyu.edu			awi 
		Mario.Kleiner@tuebingen.mpg.de	mk
  
    PLATFORMS:	
	
		All.

    HISTORY:
    
		1/19/04		awi		Wrote it.
		11/1/05     mk		Resynced implementation with changes in SCREENDrawText.
		1/29/06     mk		Implemented Windows-Version of it.
		3/7/06		awi		Changed references from "Font" to "FontInfo".  The function had been previously renamed, but not all references updated.   
		12/27/09	mk		Massive refactoring of code for all platforms and support for plugin-based textrenderers. Now one shared implementation for
							all platforms, because PsychDrawUnicodeText() does the main share of work if called in special "boundingbox" mode.

    DESCRIPTION:

		Compute local and global bounding box for a given textstring at given location, size, font and style.

    NOTES:
	
		According to Apple's ATSU reference:
		"... use the Font Manager functions that translate font family numbers to FMFont values, which are equivalent to ATSUFontID values.
		Font family numbers were used by QuickDraw to represent fonts to the Font Manager. Some of these fonts, even if compatible with ATSUI, may not have font IDs"
		
		This is confusing because font families are not equivalent to fonts.  FMFont values describe fonts and not font families.  A font is a member of a font family.
		They must be talking about 
		
		OSStatus FMGetFontFromFontFamilyInstance (
			FMFontFamily iFontFamily, 
			FMFontStyle iStyle, 
			FMFont *oFont, 
			FMFontStyle *oIntrinsicStyle
		);
	
  
    TO DO:
		
*/

#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "[normBoundsRect, offsetBoundsRect]= Screen('TextBounds', windowPtr, text [,x] [,y] [,yPositionIsBaseline] [,swapTextDirection]);";
//                          1               2                                      1		   2	  3	   4    5						6
static char synopsisString[] = 
    "Accepts a window pointer and a 'text' string.  Return in 'normBoundsRect' a rect defining the size of the text "
    "in units of pixels.  Returns in 'offsetBoundsRect' offsets of the text bounds from the origin, assuming "
    "that the text will be drawn at the current position of the text drawing cursor. Only the default high "
    "quality text renderers returns a perfect bounding box. The optionally selectable low-quality, fast renderers "
	"on Windows and Linux return a bounding box which doesn't take letters with descenders into account "
    "- Descenders are outside the returned box.\n"
	"See help for Screen('DrawText') for info about accepted text string formats and all additional parameters... ";
	
static char seeAlsoString[] = "DrawText TextSize TextFont TextStyle TextColor TextBackgroundColor";

PsychError SCREENTextBounds(void) 
{
	PsychWindowRecordType	*winRec;
	PsychRectType			resultPsychRect, resultPsychNormRect;
	int						yPositionIsBaseline, swapTextDirection;
	int						stringLengthChars;
	double*					textUniDoubleString;

	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
	//check for correct the number of arguments before getting involved
	PsychErrorExit(PsychCapNumInputArgs(6));   	
	PsychErrorExit(PsychRequireNumInputArgs(2)); 	
	PsychErrorExit(PsychCapNumOutputArgs(2));
	
	// Get the window record:
	PsychAllocInWindowRecordArg(1, kPsychArgRequired, &winRec);
	
	// Check if input text string is present, valid and non-empty, get it as double vector
	// of unicode characters: If this returns false then there ain't any work for us to do:
	if(!PsychAllocInTextAsUnicode(2, kPsychArgRequired, &stringLengthChars, &textUniDoubleString)) {
		PsychErrorExitMsg(PsychError_user, "You asked me to compute the bounding box of an empty text string?!? Sorry, that's a no no...");
	}

	// Get starting position for text cursor: This is optional.
	PsychCopyInDoubleArg(3, kPsychArgOptional, &(winRec->textAttributes.textPositionX));
    PsychCopyInDoubleArg(4, kPsychArgOptional, &(winRec->textAttributes.textPositionY));

	// Define the meaning of the y position of the specified drawing cursor.
	// We get the global setting from the Screen preference, but allow to override
	// it on a per-invocation basis via the optional 7th argument to 'DrawText':
	yPositionIsBaseline = PsychPrefStateGet_TextYPositionIsBaseline();
	PsychCopyInIntegerArg(5, kPsychArgOptional, &yPositionIsBaseline);

	// Get optional text writing direction flag: Defaults to left->right aka 0:
	swapTextDirection = 0;
	PsychCopyInIntegerArg(6, kPsychArgOptional, &swapTextDirection);

	// This will perform the bounding box measurement and return the absolute bounding box in "resultPsychRect":
	PsychDrawUnicodeText(winRec, &resultPsychRect, stringLengthChars, textUniDoubleString, &(winRec->textAttributes.textPositionX), &(winRec->textAttributes.textPositionY), yPositionIsBaseline, &(winRec->textAttributes.textColor), &(winRec->textAttributes.textBackgroundColor), swapTextDirection);

	// Create normalized version with top-left corner in (0,0):
	PsychNormalizeRect(resultPsychRect, resultPsychNormRect);

	// Return resulting bounding boxes:
	PsychCopyOutRectArg(1, FALSE, resultPsychNormRect);
	PsychCopyOutRectArg(2, FALSE, resultPsychRect);

    return(PsychError_none);
}
