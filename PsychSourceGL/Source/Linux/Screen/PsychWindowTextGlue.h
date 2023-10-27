/*
	PsychToolbox3/Source/Linux/Screen/PsychWindowTextGlue.h
	
	PLATFORMS:	
	
		This is the Linux version.
				
	AUTHORS:

		Allen Ingling		awi		Allen.Ingling@nyu.edu
		Mario Kleiner           mk              mario.kleiner at tuebingen.mpg.de

	HISTORY:
		
		11/18/03		awi		Wrote it 
		02/20/06                mk              Derived it from Windows version.
							
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

//abstract up for platform independance.
typedef enum {
   kPsychTextFill=1,
   kPsychTextStroke,
   kPsychTextFillStroke,
   kPsychTextInvisible,
   kPsychTextFillClip,
   kPsychTextStrokeClip,
   kPsychTextFillStrokeClip,
   kPsychTextClip
} PsychTextDrawingModeType;

#define					kPsychNumTextDrawingModes		8
#define 				kPsychNoFont				-1				
extern const char 			*PsychTextDrawingModeNames[];		//PsychWindowTextGlue.c
extern const PsychTextDrawingModeType	PsychTextDrawingModes[];

//typedef for parameters specifying text characteristics. Enclosed within window record structure because windows have text properties.

typedef struct {
        //when adding new fields remember to initialize them to sane values within PsychInitTextRecordSettings() which is called with
        //	each new window.  
        PsychTextDrawingModeType	textMode;
        double				textPositionX;  
        double				textPositionY; 	
        int				textSize;
        int				textStyle;  // 0=normal,1=bold,2=italic,4=underline,8=outline,32=condense,64=extend		
        Str255				textFontName;
        int				textFontNumber;
	PsychColorType			textColor;
	PsychColorType			textBackgroundColor;
        GLuint                          DisplayList;         // Base-Id for font display lists: Handle for ASCII character zero.
        psych_bool                         needsRebuild;  // If set to true, the display lists need to be rebuild becaue font has changed.
        float                           glyphWidth[256];  // Width of each ASCII character glyph in GL units.
        float                           glyphHeight[256]; // Height of each ASCII character glyph in GL units.
} PsychTextAttributes;


//function prototypes
void PsychInitTextRecordSettings(PsychTextAttributes *settings);		
void PsychGetTextDrawingModeNameFromTextDrawingModeConstant(char *modeNameStr, int modeNameStrSize, PsychTextDrawingModeType mode);
psych_bool PsychGetTextDrawingModeConstantFromTextDrawingModeName(PsychTextDrawingModeType *mode, char *modeNameStr);

//end include once
#endif

