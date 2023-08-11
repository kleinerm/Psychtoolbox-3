/*
	Psychtoolbox3/PsychSourceGL/Source/Common/Screen/SCREENBlendFunction.c	

	AUTHORS:

		Allen.Ingling@nyu.edu				awi 
		mario.kleiner at tuebingen.mpg.de	mk
		
	PLATFORMS:	

		All.
		
	HISTORY:

		01/08/05  awi		Wrote it.
		04/22/05  mk        Update of help text: Explain relationship to DrawTexture, DrawDots and DrawLines.
		04/06/08  mk		Add support for setup of color buffer writemask.

	DESCRIPTION:

		Set the GL blending function using glBlendFunc() and the color mask via glColorMask() for a specific window.
		Actually, it only stores the wanted settings. The real setup is happening whenever the window is selected
		for any drawing operations.

*/

#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "[sourceFactorOld, destinationFactorOld, colorMaskOld]=Screen('BlendFunction', windowIndex, [sourceFactorNew], [destinationFactorNew], [colorMaskNew]);";
//																									1            2				   3						4
static char synopsisString[] = 
	"Return or set the current alpha-blending mode and the color buffer writemask for window 'windowIndex'.\n"
	"Alpha blending is a way to combine color values of pixels already in the window with new color values from "
	"drawing commands. Alpha blending is disabled by default: If you overdraw some pixel location (x,y) in your window "
	"with a new source color value [Rs Gs Bs As], then the window location gets that color value assigned: I(x,y) = [Rs Gs Bs As]. "
	"However, OpenGL also allows you to combine such new color values with color values previously stored in that "
	"location. The old values are called destination colors [Rd Gd Bd Ad]. The way old and new destination and source "
	"colors are combined into new destination colors is called alpha-blending. Formally:\n"
	"New color at location (x,y) is [Rn Gn Bn An] = blendequation([Rs Gs Bs As], [Rd Gd Bd Ad]);\n where "
	"blendequation is a function that describes how the new [Rs Gs Bs As] color values and previous old "
	"values [Rd Gd Bd Ad] should be combined. You can choose the 'blendequation' from a set of defined "
	"blend equations via choice of the 'sourceFactorNew' and 'destinationFactorNew' arguments. See "
	"help PsychAlphaBlending and the help texts for the functions in that folder for possible choices "
	"of blend factors. The default setting of GL_ONE, GL_ZERO disables blending.\n"
	"The most common alpha-blending factors are sourceFactorNew = GL_SRC_ALPHA and destinationFactorNew = GL_ONE_MINUS_SRC_ALPHA "
	"They are needed for proper anti-aliasing (smoothing) by Screen('DrawLines'), Screen('DrawDots') and for drawing masked "
	"stimuli with the Screen('DrawTexture') command. See DotDemo, LinesDemo, AlphaImageDemo, GazeContingentDemo for a few "
	"applications of alpha-blending.\n\n"
	"This function also allows to return and set the color write mask for a window: You can prevent Psychtoolbox from drawing "
	"into and changing the content of one or more color channels by disabling the color channel for writing. This allows to "
	"perform drawing commands that only affect, e.g., the red chahnnel, but not the blue, green or alpha channel. "
	"You choose the channels by specification of the 'colorMaskNew' vector: The first element selects if writing to the "
	"red color channel are allowed (value greater than zero) or disallowed (value equal zero). The 2nd element selects the "
	"green channels state, the 3rd element selects the blue channels state and the 4th element selects the alpha channels "
	"state. E.g., setting colorMaskNew equal to [1 1 0 0] would allow updates of the red and green channel, but not of the "
	"blue and alpha channel. A setting of [0 1 0 1] would allow updates to the green- and alpha channel, but not the red- "
	"and blue channel etc. The default writemask is 'all enabled' ie. [1 1 1 1].\n\n"
	"Settings for alpha-blending and color write mask are per window: They can be set individually for each onscreen window, "
	"offscreen window or texture. ";

static char seeAlsoString[] = "DrawDots, DrawLines, DrawTexture";	 

PsychError SCREENBlendFunction(void)
{
	PsychWindowRecordType 	*windowRecord;
	GLenum					oldSource, oldDestination, newSource, newDestination;
	char					*oldSoureStr, *oldDestinationStr, *newSourceStr, *newDestinationStr;
	int						oldSourceStrSize, oldDestinationStrSize, isSourceStringValid, isDestinationStringValid;
	psych_bool					isSourceSupplied, isDestinationSupplied, isSourceChoiceValid, isDestinationChoiceValid;
	double					*oldColorMask, *newColorMask;
	int						m, n, p;
	
	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

	PsychErrorExit(PsychCapNumInputArgs(4));		//The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1));	//The required number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(3));		//The maximum number of outputs

	//Get the window record or exit with an error if the windex was bogus.
	PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);

	//Retreive the old source and destination factors and return them from the Screen call as strings
	PsychGetAlphaBlendingFactorsFromWindow(windowRecord, &oldSource, &oldDestination);

	oldSourceStrSize=PsychGetAlphaBlendingFactorStringFromConstant(oldSource, NULL);
	oldDestinationStrSize=PsychGetAlphaBlendingFactorStringFromConstant(oldDestination, NULL);
	
	oldSoureStr=(char *)malloc(sizeof(char) * oldSourceStrSize);
	oldDestinationStr=(char *)malloc(sizeof(char) * oldDestinationStrSize);

	PsychGetAlphaBlendingFactorStringFromConstant(oldSource, oldSoureStr);
	PsychGetAlphaBlendingFactorStringFromConstant(oldDestination, oldDestinationStr);

	PsychCopyOutCharArg(1, kPsychArgOptional, oldSoureStr);
	PsychCopyOutCharArg(2, kPsychArgOptional, oldDestinationStr);
	
	free((void *)oldSoureStr);
	free((void *)oldDestinationStr);

	//Get the new settings if they are present and set them.
	newSource=oldSource;
	newDestination=oldDestination;

	isSourceSupplied= PsychAllocInCharArg(2, kPsychArgOptional, &newSourceStr);
	isDestinationSupplied= PsychAllocInCharArg(3, kPsychArgOptional, &newDestinationStr);

	if(isSourceSupplied){
		isSourceStringValid=PsychGetAlphaBlendingFactorConstantFromString(newSourceStr, &newSource);
		if(!isSourceStringValid) PsychErrorExitMsg(PsychError_user, "Supplied string argument 'sourceFactorNew' is invalid");

		isSourceChoiceValid=PsychValidateBlendingConstantForSource(newSource);
		if(!isSourceChoiceValid) PsychErrorExitMsg(PsychError_user, "The blending factor supplied for the source is only valid only for the destination");
	}

	if(isDestinationSupplied){
		isDestinationStringValid=PsychGetAlphaBlendingFactorConstantFromString(newDestinationStr, &newDestination);
		if(!isDestinationStringValid) PsychErrorExitMsg(PsychError_user, "Supplied string argument 'destinationFactorNew' is invalid");

		isDestinationChoiceValid=PsychValidateBlendingConstantForDestination(newDestination);
		if(!isDestinationChoiceValid) PsychErrorExitMsg(PsychError_user, "The blending factor supplied for the destination is only valid only for the source");
	}

	PsychStoreAlphaBlendingFactorsForWindow(windowRecord, newSource, newDestination);

	// Check if alpha blending is possible for this windowRecord:
	if ((newSource != GL_ONE || newDestination != GL_ZERO) && !((windowRecord->bpc < 16) || (windowRecord->bpc == 16 && (windowRecord->gfxcaps & kPsychGfxCapFPBlend16)) || (windowRecord->bpc == 32 && (windowRecord->gfxcaps & kPsychGfxCapFPBlend32)) || ((windowRecord->bpc == 16) && (windowRecord->imagingMode & kPsychNeed16BPCFixed)))) {
		// Nope. Alpha blending requested but not possible for this windowRecord with this gfx-hardware.
		if (PsychPrefStateGet_Verbosity() > 1) {
			printf("PTB-WARNING: Screen('Blendfunction') called to enable alpha-blending on a window (handle=%i) which doesn't support\n", windowRecord->windowIndex);
			printf("PTB-WARNING: alpha-blending at its current color resolution of %i bits per color component on your hardware.\n", windowRecord->bpc);
			printf("PTB-WARNING: Won't enable blending. Either lower the color resolution of the window (see help PsychImaging) or\n");
			printf("PTB-WARNING: upgrade your graphics hardware.\n\n");
		}
	}
	
	// Create return array with encoded old colormask:
	PsychAllocOutDoubleMatArg(3, kPsychArgOptional, 1, 4, 1, &oldColorMask);
	oldColorMask[0] = (windowRecord->colorMask[0]) ? 1 : 0;
	oldColorMask[1] = (windowRecord->colorMask[1]) ? 1 : 0;
	oldColorMask[2] = (windowRecord->colorMask[2]) ? 1 : 0;
	oldColorMask[3] = (windowRecord->colorMask[3]) ? 1 : 0;

	// Any new colormask provided?
	if (PsychAllocInDoubleMatArg(4, kPsychArgOptional, &m, &n, &p, &newColorMask)) {
		// Yes. Assign it:
		if (p!=1 || m*n != 4)  PsychErrorExitMsg(PsychError_user, "The colorMaskNew argument must be a 4 element row- or column vector!");

		windowRecord->colorMask[0] = (newColorMask[0] > 0) ? GL_TRUE : GL_FALSE;
		windowRecord->colorMask[1] = (newColorMask[1] > 0) ? GL_TRUE : GL_FALSE;
		windowRecord->colorMask[2] = (newColorMask[2] > 0) ? GL_TRUE : GL_FALSE;
		windowRecord->colorMask[3] = (newColorMask[3] > 0) ? GL_TRUE : GL_FALSE;
	}
	
	return(PsychError_none);
}
