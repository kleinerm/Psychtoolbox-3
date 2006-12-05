/*

	SCREENBlendFunction.c	

  

	AUTHORS:



		Allen.Ingling@nyu.edu		awi 

  

	PLATFORMS:	

	

		Only OS X for now.  

    

	HISTORY:



		01/08/05  awi		Wrote it.

                04/22/05  mk            Update of help text: Explain relationship to DrawTexture, DrawDots and DrawLines.

 

	DESCRIPTION:

	

		Set the GL blending function using glBlendFunc() for a specific window.  

  

*/





#include "Screen.h"



// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c

static char useString[] = "[sourceFactorOld, destinationFactorOld]=('BlendFunction', windowIndex, [sourceFactorNew], [destinationFactorNew]);";

//                                                                                   1             2                  3

static char synopsisString[] = 

	"Set the rule for choosing source and destination alpha blending factors.  If new settings are supplied, 'BlendFunction'  "

	"invokes the OpenGL function glBlendFunc() within the glContext of windowIndex. Alpha blending is disabled by default. "

        "The most common alpha-blending factors are sourceFactorNew = GL_SRC_ALPHA and destinationFactorNew = GL_ONE_MINUS_SRC_ALPHA "

        "They are needed for proper anti-aliasing (smoothing) by Screen('DrawLines'), Screen('DrawDots') and for drawing masked "

        "stimuli with the Screen('DrawTexture') command. ";



static char seeAlsoString[] = "DrawDots, DrawLines, DrawTexture";	 



PsychError SCREENBlendFunction(void)

{

	PsychWindowRecordType 	*windowRecord;

	GLenum					oldSource, oldDestination, newSource, newDestination;

	char					*oldSoureStr, *oldDestinationStr, *newSourceStr, *newDestinationStr;

	int						oldSourceStrSize, oldDestinationStrSize, isSourceStringValid, isDestinationStringValid;

	Boolean					isSourceSupplied, isDestinationSupplied, isSourceChoiceValid, isDestinationChoiceValid;

	

	//all subfunctions should have these two lines.  

	PsychPushHelp(useString, synopsisString, seeAlsoString);

	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

	

	PsychErrorExit(PsychCapNumInputArgs(3));   //The maximum number of inputs

	PsychErrorExit(PsychRequireNumInputArgs(1));   //The maximum number of inputs

	PsychErrorExit(PsychCapNumOutputArgs(2));  //The maximum number of outputs

	

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

		if(!isSourceStringValid)

			PsychErrorExitMsg(PsychError_user, "Supplied string argument 'sourceFactorNew' is invalid");

		isSourceChoiceValid=PsychValidateBlendingConstantForSource(newSource);

		if(!isSourceChoiceValid)

			PsychErrorExitMsg(PsychError_user, "The blending factor supplied for the source is only valid only for the destination");



	}

	if(isDestinationSupplied){

		isDestinationStringValid=PsychGetAlphaBlendingFactorConstantFromString(newDestinationStr, &newDestination);

		if(!isDestinationStringValid)

			PsychErrorExitMsg(PsychError_user, "Supplied string argument 'destinationFactorNew' is invalid");

		isDestinationChoiceValid=PsychValidateBlendingConstantForDestination(newDestination);

		if(!isDestinationChoiceValid)

			PsychErrorExitMsg(PsychError_user, "The blending factor supplied for the destination is only valid only for the source");

	}

	PsychStoreAlphaBlendingFactorsForWindow(windowRecord, newSource, newDestination);

	

	

	return(PsychError_none);	

}






