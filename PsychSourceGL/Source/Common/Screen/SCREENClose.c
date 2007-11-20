/*

	PsychSourceGL/Source/Common/Screen/SCREENClose.c	

	AUTHORS:

		Allen.Ingling@nyu.edu				awi 
		mario.kleiner at tuebingen.mpg.de	mk
	PLATFORMS:	

		All.  

	HISTORY:

		07/26/02  awi		Created from OpenWindow
		01/30/05  mk        Closes/Deletes all textures, if no windowOrTextureIndex provided. "CloseAllTextures" - Functionality.

	DESCRIPTION:
	
		Close named onscreen windows, offscreen windows, proxy windows or textures.
		Batch close all offscreen windows, proxies and textures with a single convenience call on request.

	TO DO:

*/

#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('Close', [windowOrTextureIndex]);";
static char synopsisString[] = 
	"Close an onscreen or offscreen window or a texture. If the optional windowOrTextureIndex isn't "
	"provided, then all textures are closed/deleted while regular windows are left open.";  
static char seeAlsoString[] = "OpenWindow, OpenOffscreenWindow";	 

PsychError SCREENClose(void)
{
	PsychWindowRecordType 	*windowRecord;
	int			screenNumber;
	PsychWindowRecordType	**windowRecordArray;
	int			i, numWindows; 

	windowRecord=NULL;

	//all subfunctions should have these two lines.  
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

	PsychErrorExit(PsychCapNumInputArgs(1));      //The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0));  //The minimum required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));     //The maximum number of outputs

	//Get the window record or exit with an error if the windex was bogus.
	PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, kPsychArgOptional, &windowRecord);

	// Window handle of a specific window provided?
	if (windowRecord==NULL) {
		// No window handle provided: In this case, we close/destroy all textures:
		PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);
		for(i=0;i<numWindows;i++) {
			if (windowRecordArray[i]->windowType==kPsychTexture) PsychCloseWindow(windowRecordArray[i]);			
		}

		PsychDestroyVolatileWindowRecordPointerList(windowRecordArray);
		return(PsychError_none);
	}

	// Window handle of a specific window or texture provided: Close it...
	if(PsychIsLastOnscreenWindow(windowRecord)){

		// Check for stale texture ressources and movies. Report to user if any:
		PsychRessourceCheckAndReminder(TRUE);	

		screenNumber=windowRecord->screenNumber;
		PsychCloseWindow(windowRecord);
		PsychReleaseScreen(screenNumber);
	}
	else {	
		PsychCloseWindow(windowRecord);
	}

	return(PsychError_none);
}
