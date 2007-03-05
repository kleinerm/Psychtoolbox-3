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
static char useString[] = "transtexid = Screen('TransformTexture', sourceTexture, transformProxyPtr [, targetTexture] [, specialFlags]);";
//                                                                 1              2                 3				   4
static char synopsisString[] =
	"CAUTION! EXPERIMENTAL FEATURE IN EARLY BETA STAGE, DON'T TRUST IT BLINDLY!\n\n"
	"Apply an image processing operation to a texture 'sourceTexture' and store the processed result either in 'targetTexture' if "
	"provided, or in a new texture (if 'targetTexture' is not provided). Return a handle 'transtexid' to the processed texture. "
	"The image processing operation is defined in the processing hook chain 'UserDefinedBlit' of the proxy object 'transformProxyPtr'. "
	"'specialFlags' optional flags to alter operation of this function: kPsychAssumeTextureNormalized."
	"Read 'help PsychGLImageProcessing' for more infos on how to use this function.";
	
static char seeAlsoString[] = "";

PsychError SCREENTransformTexture(void) 
{
	PsychWindowRecordType	*sourceRecord, *targetRecord, *proxyRecord;
	int testarg, tmpimagingmode, specialFlags;
	PsychFBO *fboptr;
	GLint fboInternalFormat;
	Boolean needzbuffer;
	
    // All subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
    
    PsychErrorExit(PsychCapNumInputArgs(4));   	
    PsychErrorExit(PsychRequireNumInputArgs(2)); 	
    PsychErrorExit(PsychCapNumOutputArgs(1));  
	
	// OpenGL FBO's supported? Otherwise this is a no-go...
	if (glBindFramebufferEXT == NULL || glUseProgram == NULL) {
		// Game over!
		printf("PTB-ERROR: Sorry, your graphics driver & hardware does not support the required OpenGL framebuffer object extension or\n");
		printf("PTB-ERROR: the OpenGL shading language for hardware accelerated fragment processing. This function is therefore disabled.\n");
		printf("PTB-ERROR: You will need at least a NVidia GeforceFX-5200, a ATI Radeon 9600 or a Intel GMA-950 graphics card for this\n");
		printf("PTB-ERROR: to work. If you have such a card (or a more recent one) then you'll need to update your graphics drivers.\n\n");
		
		PsychErrorExitMsg(PsychError_user, "Screen('TransformTexture') command unsupported on your combination of graphics hardware & driver.");
	}
	
    // Get the window structure for the source texture.
    PsychAllocInWindowRecordArg(1, TRUE, &sourceRecord);
	if (!PsychIsTexture(sourceRecord)) PsychErrorExitMsg(PsychError_user, "'sourceTexture' argument must be a handle to a texture or offscreen window.");

    // Get the window structure for the proxy object.
    PsychAllocInWindowRecordArg(2, TRUE, &proxyRecord);
	if (proxyRecord->windowType!=kPsychProxyWindow) PsychErrorExitMsg(PsychError_user, "'transformProxyPtr' argument must be a handle to a proxy object.");


	// Test if optional specialFlags are provided:
    specialFlags=0;
    PsychCopyInIntegerArg(4, FALSE, &specialFlags);

	// Activate rendering context of the proxy object and soft-reset the
	// drawing engine, so we're in a well defined state:
	PsychSetGLContext(proxyRecord);
	PsychSetDrawingTarget(NULL);
	glUseProgram(0);
	
	// Transform sourceRecord source texture into a normalized, upright texture if it isn't already in
	// that format. We require this standard orientation for simplified shader design.
	if (!(specialFlags & 1)) PsychNormalizeTextureOrientation(sourceRecord);
	
	// Restore proper rendering context:
	PsychSetGLContext(proxyRecord);

	// Test if optional target texture is provided:
    testarg=0;
    PsychCopyInIntegerArg(3, FALSE, &testarg);

	// Do we need to create a new one from scratch?
    if (testarg==0) {
        // No valid textureHandle provided. Create a new empty textureRecord which clones some
		// of the properties of the sourceRecord
		targetRecord = NULL;
        PsychCreateWindowRecord(&targetRecord);
		PsychInitWindowRecordTextureFields(targetRecord);

        targetRecord->windowType=kPsychTexture;
        targetRecord->screenNumber = sourceRecord->screenNumber;
        targetRecord->targetSpecific.contextObject = sourceRecord->targetSpecific.contextObject;
        targetRecord->targetSpecific.deviceContext = sourceRecord->targetSpecific.deviceContext;
        targetRecord->targetSpecific.glusercontextObject = sourceRecord->targetSpecific.glusercontextObject;
		
		targetRecord->colorRange = sourceRecord->colorRange;
		
		// Copy imaging mode flags from parent:
		targetRecord->imagingMode = sourceRecord->imagingMode;
		
		targetRecord->depth = sourceRecord->depth;
		
		// Assume this texture has four channels. FIXME: Is this problematic?
		targetRecord->nrchannels = 4;
		
		PsychCopyRect(targetRecord->rect, sourceRecord->rect);
		
		targetRecord->texturetarget = sourceRecord->texturetarget;

		// Orientation is set to 2 - like an upright Offscreen window texture:
		targetRecord->textureOrientation = 2;
		
        // Mark it valid and return handle to userspace:
        PsychSetWindowRecordValid(targetRecord);
	}
	else {
	    // Get the window structure for the target texture.
		PsychAllocInWindowRecordArg(3, TRUE, &targetRecord);
		if (!PsychIsTexture(targetRecord)) PsychErrorExitMsg(PsychError_user, "'targetTexture' argument must be a handle to a texture or offscreen window.");
	}
	
	
	// Make sure our source texture has at least a pseudo FBO for read-access:
	PsychCreateShadowFBOForTexture(sourceRecord, FALSE, -1);
	
	// Make sure our target texture has a full-blown FBO attached as a rendertarget.
	// As our proxy object defines the image processing ops, it also defines the
	// required imagingMode properties for the target texture:
	PsychCreateShadowFBOForTexture(targetRecord, TRUE, proxyRecord->imagingMode);
	
	// Make sure our proxy has suitable bounce buffers if we need any:
	if (proxyRecord->imagingMode & (kPsychNeedDualPass | kPsychNeedMultiPass)) {
		// Needs multi-pass processing. Create bounce buffer if neccessary:
		PsychCopyRect(proxyRecord->rect, targetRecord->rect);
		PsychCreateShadowFBOForTexture(proxyRecord, TRUE, proxyRecord->imagingMode);
	}
	
	// Make sure we don't have VRAM memory feedback loops:
	if (sourceRecord->textureNumber == targetRecord->textureNumber) {
		PsychErrorExitMsg(PsychError_user, "Source texture and target texture must be different!");
	}
	
	// Apply image processing operation: Use ressources and OpenGL context of proxyRecord, run user defined blit chain,
	// Don't supply user specific data (NULL), don't supply override blitter (NULL), source is read-only (TRUE), no
	// swizzle allowed (FALSE), sourceRecord is source, targetRecord is destination, bounce buffers provided by proxyRecord,
	// no secondary FBO available (NULL).
	PsychPipelineExecuteHook(proxyRecord, kPsychUserDefinedBlit, NULL, NULL, TRUE, FALSE, &(sourceRecord->fboTable[sourceRecord->drawBufferFBO[0]]), NULL, &(targetRecord->fboTable[targetRecord->drawBufferFBO[0]]), (proxyRecord->drawBufferFBO[0]!=-1) ? &(proxyRecord->fboTable[proxyRecord->drawBufferFBO[0]]) : NULL);

	//Return the window index and the rect argument.
    PsychCopyOutDoubleArg(1, FALSE, targetRecord->windowIndex);

    // Done.
    return(PsychError_none);
}
