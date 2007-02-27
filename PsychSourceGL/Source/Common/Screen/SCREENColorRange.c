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
static char useString[] = "[oldmaximumvalue oldclampcolors] = Screen('ColorRange', windowPtr [, maximumvalue][, clampcolors=1]);";
static char synopsisString[] = 
	"Set or return the maximum color component value that PTB should allow for provided color "
	"values when drawing into a window 'windowPtr' or its associated Offscreen windows. "
	"'maximumvalue' is the optional new setting for the maximum allowed color component value: "
	"PTB expects the values of provided color components (red, green, blue, alpha or intensity) "
	"to be in the range 0 to maximumvalue. 0 is mapped to minimum output intensity, "
	"maximumvalue is mapped to maximum output intensity, values outside the range 0-maximumvalue "
	"are automatically saturated (clamped) at zero or maximumvalue if clamping is enabled. "
	"Initially color range clamping is enabled and the maximumvalue defaults to the biggest "
	"integral number displayable by your video hardware, e.g., 255 for a standard 8 bit "
	"per color component framebuffer as present on most consumer graphics hardware. "
	"The maximumvalue 1.0 has special meaning: A maximumvalue of 1.0 will enable PTB to pass "
	"color values in OpenGL's native floating point color range of 0.0 to 1.0: This has two "
	"advantages: First, your color values are independent of display device depth, i.e. no need "
	"to rewrite your code when running it on higher resolution hardware. Second, PTB can skip any "
	"color range remapping operations - this can speed up drawing significantly in some cases. "
	"'clampcolors': By default, OpenGL clamps colors to the range 0-maximumvalue. Negative colors "
	"are clamped to zero, color values greater than maximumvalue are clamped to maximumvalue. If "
	"you set the optional flag 'clampcolors' to 0, PTB will disable color clamping on hardware that "
	"supports unclamped colors. This allows to pass arbitrary (even negative) floating point "
	"numbers as color values. This is useful in conjunction with special high dynamic range display "
	"devices and for certain image processing operations when using PTB's image processing pipeline. "
	"CAUTION: If you change the color range of an onscreen window with this function, the changes will "
	"only affect textures and offscreen windows created *after* this function call, not ones created "
	"before. It's therefore recommended to execute this function immediately after creating an onscreen "
	"window to guarantee consistent behaviour of your code. Color values provided as uint8 arrays or as "
	"textures, e.g., from video capture, Quicktime movies or Screen('MakeTexture') are not rescaled as"
	"they are (expected) to be in a proper format for the given color depth already. ";

static char seeAlsoString[] = "OpenWindow OpenOffscreenWindow";	 

PsychError SCREENColorRange(void) 
{
	PsychWindowRecordType *windowRecord;
	double maxvalue, clampcolors, oldclampcolors;
	GLboolean enabled, enabled1, enabled2, enabled3;
	
	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	PsychErrorExit(PsychCapNumInputArgs(3));     //The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); //The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(2));	 //The maximum number of outputs
        
	//get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);
	if(!PsychIsOnscreenWindow(windowRecord)) PsychErrorExitMsg(PsychError_user, "Tried to set color range on something else than a onscreen window");
            
	// Decode current setting:
	maxvalue = windowRecord->colorRange;
	clampcolors = (maxvalue > 0) ? 1 : 0;
	maxvalue = fabs(maxvalue);
	oldclampcolors = clampcolors;
	
	// Copy out optional return value:
	PsychCopyOutDoubleArg(1, FALSE, maxvalue);
	PsychCopyOutDoubleArg(2, FALSE, clampcolors);
	
	// Get the optional new values: We default to clamping on.
	clampcolors = 1;
	PsychCopyInDoubleArg(2, FALSE, &maxvalue);
	PsychCopyInDoubleArg(3, FALSE, &clampcolors);

	// Encode into one value:
	if (maxvalue<=0) PsychErrorExitMsg(PsychError_user, "Tried to set invalid color maximumvalue (negative or zero).");
	
	// Try to set clamping behaviour:
	PsychSetGLContext(windowRecord);
	if (oldclampcolors != clampcolors) {
		if (glClampColorARB) {
			// Color clamping extension supported: Set new clamp mode.
			enabled = (clampcolors > 0) ? GL_TRUE : GL_FALSE;
			glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, enabled);
			glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, enabled);
			glClampColorARB(GL_CLAMP_READ_COLOR_ARB, enabled);
		}
		else {
			if (PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Switching and query of color clamping via glClampColorARB unsupported by your graphics hardware.\n");
		}

		// Check if the clamp en-/disable worked:
		glGetBooleanv(GL_CLAMP_VERTEX_COLOR_ARB, &enabled1);
		glGetBooleanv(GL_CLAMP_FRAGMENT_COLOR_ARB, &enabled2);
		glGetBooleanv(GL_CLAMP_READ_COLOR_ARB, &enabled3);

		if ((clampcolors==0 && (enabled1 || enabled2 || enabled3)) || (clampcolors==1 && (!enabled1 || !enabled2 || !enabled3))) {
			if (PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Could not %s color value clamping as requested. Unsupported by your graphics hardware?\n", (clampcolors==1) ? "enable" : "disable");
		}		
	}
	
	// Encode maxcolor as well as clamping mode (in sign):
	windowRecord->colorRange = (clampcolors>0) ? maxvalue : -maxvalue;
	
	// Successfully done:
	return(PsychError_none);
}
