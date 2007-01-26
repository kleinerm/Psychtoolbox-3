/*
	SCREENTransformTexture.c	
  
    AUTHORS:
    
		Mario.Kleiner@tuebingen.mpg.de  mk
  
    PLATFORMS:	
	
		All.
    
    HISTORY:
    
		01/26/07	mk		Wrote it.
	
    DESCRIPTION:
  
    NOTES:

    TO DO:

*/

#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "transtexid = Screen('TransformTexture', sourceTexture, transformProxyPtr [, targetTexture]);";
//                                                                 1              2                 3
static char synopsisString[] = 
	"Apply an image processing operation to a texture 'sourceTexture' and store the processed result either in 'targetTexture' if "
	"provided, or in a new texture (if 'targetTexture' is not provided). Return a handle 'transtexid' to the processed texture. "
	"The image processing operation is defined in the processing hook chain of the proxy object 'transformProxyPtr'. "
	"Read 'help PsychGLImageProcessing' for more infos on how to use this function.";
	
static char seeAlsoString[] = "";

PsychError SCREENTransformTexture(void) 
{
	PsychWindowRecordType	*sourceRecord, *targetRecord, *proxyRecord;

    // All subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
    
    PsychErrorExit(PsychCapNumInputArgs(3));   	
    PsychErrorExit(PsychRequireNumInputArgs(2)); 	
    PsychErrorExit(PsychCapNumOutputArgs(1));  
	
    // Get the window structure for the source texture.
    PsychAllocInWindowRecordArg(1, TRUE, &sourceRecord);
	if (!PsychIsTexture(sourceRecord)) PsychErrorExitMsg(PsychError_user, "'sourceTexture' argument must be a handle to a texture or offscreen window.");

    // Get the window structure for the proxy object.
    PsychAllocInWindowRecordArg(2, TRUE, &proxyRecord);
	if (proxyRecord->windowType!=kPsychProxyWindow) PsychErrorExitMsg(PsychError_user, "'transformProxyPtr' argument must be a handle to a proxy object.");

    // Get the optional window structure for the target texture.
	targetRecord = NULL;
    PsychAllocInWindowRecordArg(3, FALSE, &targetRecord);

	// Do we need to create a new one from scratch?
	if (targetRecord == NULL) {
        // No valid textureHandle provided. Create a new empty textureRecord which clones some
		// of the properties of the sourceRecord
        PsychCreateWindowRecord(&targetRecord);
        targetRecord->windowType=kPsychTexture;
        targetRecord->screenNumber = sourceRecord->screenNumber;
        targetRecord->targetSpecific.contextObject = sourceRecord->targetSpecific.contextObject;
        targetRecord->targetSpecific.deviceContext = sourceRecord->targetSpecific.deviceContext;
        targetRecord->targetSpecific.glusercontextObject = sourceRecord->targetSpecific.glusercontextObject;
		
		targetRecord->colorRange = sourceRecord->colorRange;
		
		// Copy imaging mode flags from parent:
		targetRecord->imagingMode = sourceRecord->imagingMode;
		
		// Ok, setup texture record for texture:
		PsychInitWindowRecordTextureFields(targetRecord);
		targetRecord->depth = sourceRecord->depth;
		
		// Assume this texture has four channels. FIXME: Is this problematic?
		targetRecord->nrchannels = 4;
		
		PsychCopyRect(targetRecord->rect, sourceRecord->rect);
		
		targetRecord->texturetarget = sourceRecord->texturetarget;
		// Orientation is set to 2 - like an upright Offscreen window texture:
		targetRecord->textureOrientation = 2;
		
		// TODO: Create PsychFBO for this texture...

        // Mark it valid and return handle to userspace:
        PsychSetWindowRecordValid(targetRecord);
	}
	else {
		if (!PsychIsTexture(targetRecord)) PsychErrorExitMsg(PsychError_user, "'targetTexture' argument must be a handle to a texture or offscreen window.");
	}
	
	// Apply image processing operation:
	// PsychPipelineExecuteHook(proxyRecord, kPsychUserDefinedBlit, NULL, NULL, TRUE, FALSE, <#PsychFBO * * srcfbo1#>, NULL,<#PsychFBO * * dstfbo#>,<#PsychFBO * * bouncefbo#>)
	PsychErrorExitMsg(PsychError_unimplemented, "Sorry, this feature is not yet finished.");

	//Return the window index and the rect argument.
    PsychCopyOutDoubleArg(1, FALSE, targetRecord->windowIndex);

    // Done.
    return(PsychError_none);
}
