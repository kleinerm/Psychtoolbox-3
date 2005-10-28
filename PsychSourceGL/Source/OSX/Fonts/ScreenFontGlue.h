/*
	PsychToolbox2/Source/OSX/Fonts/ScreenFontGlue.h
	
	PLATFORMS:	This is the OS X  version.  
				
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	
		7/8/04		awi		Split it of from PsychFontGlue because PsychWindowRecordType in PsychSetATSUStyleAttributes
							was causing trouble for the FontInfo project
							
	DESCRIPTION:
        

*/

//include once
#ifndef PSYCH_IS_INCLUDED_ScreenFontGlue
#define PSYCH_IS_INCLUDED_ScreenFontGlue

#include "Psych.h"

void PsychSetATSUStyleAttributesFromPsychWindowRecord(ATSUStyle atsuStyle,  PsychWindowRecordType *winRec);

#endif





    
