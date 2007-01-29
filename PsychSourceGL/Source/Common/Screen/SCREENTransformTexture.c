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
	"CAUTION! EXPERIMENTAL FEATURE IN EARLY BETA STAGE, DON'T TRUST IT BLINDLY!\n\n"
	"Apply an image processing operation to a texture 'sourceTexture' and store the processed result either in 'targetTexture' if "
	"provided, or in a new texture (if 'targetTexture' is not provided). Return a handle 'transtexid' to the processed texture. "
	"The image processing operation is defined in the processing hook chain 'UserDefinedBlit' of the proxy object 'transformProxyPtr'. "
	"Read 'help PsychGLImageProcessing' for more infos on how to use this function.";
	
static char seeAlsoString[] = "";

PsychError SCREENTransformTexture(void) 
{
	PsychWindowRecordType	*sourceRecord, *targetRecord, *proxyRecord;
	int testarg, tmpimagingmode;
	PsychFBO *fboptr;
	GLint fboInternalFormat;
	Boolean needzbuffer;
	
    // All subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
    
    PsychErrorExit(PsychCapNumInputArgs(3));   	
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

	// Activate rendering context of the proxy object and soft-reset the
	// drawing engine, so we're in a well defined state:
	PsychSetGLContext(proxyRecord);
	PsychSetDrawingTarget(NULL);
	glUseProgram(0);
	
	// The source texture sourceRecord could be in any of PTB's supported
	// internal texture orientations. It may be upright as an Offscreen window,
	// or flipped upside down as some textures from the video grabber or Quicktime,
	// or transposed, as textures from Matlab/Octave. However, handling all those
	// cases for image processing would be a debug and maintenance nightmare.
	// Therefore we check the format of the source texture and require it to be
	// a normal upright orientation. If this isn't the case, we perform a preprocessing
	// step to transform the texture into normalized orientation.
	if (sourceRecord->textureOrientation != 2) {
		if (PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: In SCREENTransformTexture(): Performing GPU renderswap for source gl-texture %i\n", sourceRecord->textureNumber);
		
		// Normalization needed. Create a suitable FBO as rendertarget:
		needzbuffer = FALSE;
		
		// First delete FBO of texture if one already exists:
		fboptr = sourceRecord->fboTable[0];
		if (fboptr!=NULL) { 			
			// Detach and delete color buffer texture:

			needzbuffer = (fboptr->ztexid) ? TRUE : FALSE;

			// if (fboptr->coltexid) glDeleteTextures(1, &(fboptr->coltexid));
			// Detach and delete depth buffer (and probably stencil buffer) texture, if any:
			if (fboptr->ztexid) glDeleteTextures(1, &(fboptr->ztexid));
			// Detach and delete stencil renderbuffer, if a separate stencil buffer was needed:
			if (fboptr->stexid) glDeleteRenderbuffersEXT(1, &(fboptr->stexid));
			// Delete FBO itself:
			if (fboptr->fboid) glDeleteFramebuffersEXT(1, &(fboptr->fboid));
						
			// Delete PsychFBO struct associated with this FBO:
			free(fboptr); fboptr = NULL;
		}
		
		// Now recreate with proper format:

		// First need to know internal format of texture...
		glBindTexture(PsychGetTextureTarget(sourceRecord), sourceRecord->textureNumber);
		glGetTexLevelParameteriv(PsychGetTextureTarget(sourceRecord), 0, GL_TEXTURE_INTERNAL_FORMAT, &fboInternalFormat);
		glBindTexture(PsychGetTextureTarget(sourceRecord), 0);
		
		// Renderable format? Pure luminance or luminance+alpha formats are not renderable on most hardware.
		
		// Upgrade 8 bit luminace to 8 bit RGBA:
		if (fboInternalFormat == GL_LUMINANCE8 || sourceRecord->depth == 8) fboInternalFormat = GL_RGBA8;
		
		// Upgrade non-renderable floating point formats to their RGB or RGBA counterparts of matching precision:
		if (sourceRecord->nrchannels < 3 && fboInternalFormat != GL_RGBA8) {
			// Unsupported format for FBO rendertargets. Need to upgrade to something suitable...
			if (sourceRecord->textureexternalformat == GL_LUMINANCE) {
				// Upgrade luminance to RGB of matching precision:
				fboInternalFormat = (sourceRecord->textureinternalformat == GL_LUMINANCE_FLOAT16_APPLE) ? GL_RGB_FLOAT16_APPLE : GL_RGB_FLOAT32_APPLE;
			}
			else {
				// Upgrade luminance+alpha to RGBA of matching precision:
				fboInternalFormat = (sourceRecord->textureinternalformat == GL_LUMINANCE_ALPHA_FLOAT16_APPLE) ? GL_RGBA_FLOAT16_APPLE : GL_RGBA_FLOAT32_APPLE;
			}
		}
		
		// Now create proper FBO:
		PsychCreateFBO(&(sourceRecord->fboTable[0]), (GLenum) fboInternalFormat, needzbuffer, PsychGetWidthFromRect(sourceRecord->rect), PsychGetHeightFromRect(sourceRecord->rect));
		sourceRecord->drawBufferFBO[0] = 0;
		sourceRecord->fboCount = 1;
		
		tmpimagingmode = sourceRecord->imagingMode;
		sourceRecord->imagingMode = 1;

		// Set FBO of sourceRecord as rendertarget, including proper setup of render geometry:
		PsychSetDrawingTarget(sourceRecord);

		// Now blit the old "disoriented" texture into the new FBO: The textureNumber of sourceRecord
		// references the old texture, the PsychFBO of sourceRecord defines the new texture...
		if (glIsEnabled(GL_BLEND)) {
			// Alpha blending enabled. Disable it, blit texture, reenable it:
			glDisable(GL_BLEND);
			PsychBlitTextureToDisplay(sourceRecord, sourceRecord, sourceRecord->rect, sourceRecord->rect, 0, 0, 1);
			glEnable(GL_BLEND);
		}
		else {
			// Alpha blending not enabled. Just blit it:
			PsychBlitTextureToDisplay(sourceRecord, sourceRecord, sourceRecord->rect, sourceRecord->rect, 0, 0, 1);
		}
		
		sourceRecord->imagingMode = tmpimagingmode;
		PsychSetDrawingTarget(NULL);

		// At this point the color attachment of the sourceRecords FBO contains the properly oriented texture.
		// Delete the old texture, attach the FBO texture as new one:
		glDeleteTextures(1, &(sourceRecord->textureNumber));
		sourceRecord->textureNumber = sourceRecord->fboTable[0]->coltexid;
		
		// Restore proper rendering context:
		PsychSetGLContext(proxyRecord);

		// Finally sourceRecord has the proper orientation:
		sourceRecord->textureOrientation = 2;
		
		// GPU renderswap finished.
	}

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
