/*
	SCREENColorRange.c		
  
	AUTHORS:

		mario.kleiner at tuebingen.mpg.de 		mk 
  
	PLATFORMS:	
	
		All.
    

	HISTORY:

		12/05/06	mk		Created.

	DESCRIPTION:
  
		Set color scaling and clamping behaviour for a specific onscreen window.
		
	TO DO:  

*/


#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "oldmaximumvalue = Screen('ColorRange', windowPtr [, maximumvalue]);";
static char synopsisString[] = 
	"Set or return the maximum color component value that PTB should allow for provided color "
	"values when drawing into a window 'windowPtr' or its associated Offscreen windows. "
	"'maximumvalue' is the optional new setting for the maximum allowed color component value: "
	"PTB expects the values of provided color components (red, green, blue, alpha or intensity) "
	"to be in the range 0 to maximumvalue. 0 is mapped to minimum output intensity, "
	"maximumvalue is mapped to maximum output intensity, values outside the range 0-maximumvalue "
	"are automatically saturated (clamped) at zero or maximumvalue. Initially the maximumvalue "
	"defaults to the biggest integral number displayable by your video hardware, e.g., 255 for a "
	"standard 8 bit per color component framebuffer as present on most consumer graphics hardware. "
	"There are two values with special meaning: A maximumvalue of 1.0 will enable PTB to pass "
	"color values in OpenGL's native floating point color range of 0.0 to 1.0: This has two "
	"advantages. First, your color values are independent of display device depth, i.e. no need "
	"to rewrite your code when running it on higher resolution hardware. Second, PTB can skip any "
	"color range remapping operations - this can speed up drawing significantly in some cases. "
	"The special value maximumvalue = 0 means: Don't remap, don't clamp colors. This allows to "
	"pass arbitrary (even negative) floating point numbers as color values. This is useful in "
	"conjunction with special high dynamic range output devices and PTB's image processing "
	"pipeline.";

static char seeAlsoString[] = "OpenWindow OpenOffscreenWindow";	 

PsychError SCREENColorRange(void) 
{
	PsychWindowRecordType *windowRecord;
	double maxvalue;
	boolean isArgPresent;

	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	PsychErrorExit(PsychCapNumInputArgs(2));     //The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); //The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 //The maximum number of outputs
        
	//get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
        
	if(!PsychIsOnscreenWindow(windowRecord)) PsychErrorExitMsg(PsychError_user, "Tried to set color range on something else than a onscreen window");
            
	// Copy out optional return value:
	PsychCopyOutDoubleArg(1, FALSE, windowRecord->colorRange);
	
	// Get the optional new value:
	isArgPresent = PsychCopyInDoubleArg(2, FALSE, &maxvalue);

	// Set it:
	if (isArgPresent) windowRecord->colorRange = maxvalue;
	
	// Successfully done:
	return(PsychError_none);
}
