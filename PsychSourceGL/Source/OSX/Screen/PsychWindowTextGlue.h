/*
	PsychToolbox3/Source/OSX/Screen/PsychWindowTextGlue.h
	
	PLATFORMS:	
	
		This is the OS X Core Graphics version.  
				
	AUTHORS:

		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
		
		11/18/03		awi		Wrote it 
							
	DESCRIPTION:
        
	NOTES: 
        
	TO DO:
	
		Some of this stuff looks platform neutral so we should consider moving that part of it outside of the the glue layer which is
        only for functions which abstract up platform-specific calls into platform neutral calls invoked by Screen subcommands.
*/

//include once
#ifndef PSYCH_IS_INCLUDED_PsychTextGlue
#define PSYCH_IS_INCLUDED_PsychTextGlue

#include "Screen.h"

// These Screen('TextMode') constants are identical to CGTextDrawingMode constants on OSX:
typedef enum {
   kPsychTextFill = kCGTextFill,
   kPsychTextStroke = kCGTextStroke,
   kPsychTextFillStroke = kCGTextFillStroke,
   kPsychTextInvisible = kCGTextInvisible,
   kPsychTextFillClip = kCGTextFillClip,
   kPsychTextStrokeClip = kCGTextStrokeClip,
   kPsychTextFillStrokeClip = kCGTextFillStrokeClip,
   kPsychTextClip = kCGTextClip
} PsychTextDrawingModeType;

#define kPsychNumTextDrawingModes   8
#define kPsychNoFont                -1
extern const char *PsychTextDrawingModeNames[];		//PsychWindowTextGlue.c
extern const PsychTextDrawingModeType	PsychTextDrawingModes[];

// typedef for parameters specifying text characteristics. Enclosed within window record structure because windows have text properties.
typedef struct {
        //when adding new fields remember to initialize them to sane values within PsychInitTextRecordSettings() which is called with
        //	each new window.  
        PsychTextDrawingModeType	textMode;
        double						textPositionX;  
        double						textPositionY; 	
        int							textSize;
        int							textStyle;  // 0=normal,1=bold,2=italic,4=underline,8=outline,32=condense,64=extend		
        Str255						textFontName;
        int							textFontNumber;
        PsychColorType				textColor;
	PsychColorType				textBackgroundColor;
        GLuint                          DisplayList;         // Base-Id for font display lists: Handle for ASCII character zero.
        psych_bool                         needsRebuild;  // If set to true, the display lists need to be rebuild becaue font has changed.

} PsychTextAttributes;

//function prototypes
void PsychInitTextRecordSettings(PsychTextAttributes *settings);		
void PsychGetTextDrawingModeNameFromTextDrawingModeConstant(char *modeNameStr, int modeNameStrSize, PsychTextDrawingModeType mode);
psych_bool PsychGetTextDrawingModeConstantFromTextDrawingModeName(PsychTextDrawingModeType *mode, char *modeNameStr);

//end include once
#endif
