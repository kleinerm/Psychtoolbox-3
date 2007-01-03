/*
	PsychToolbox3/Source/Common/Screen/PsychImagingPipelineSupport.c
	
	PLATFORMS:	
	
		All.  
				
	AUTHORS:
		
		Mario Kleiner           mk              mario.kleiner at tuebingen.mpg.de

	HISTORY:
	
		12/05/06	mk	Wrote it.
		
	DESCRIPTION:
	
		Infrastructure for all Screen imaging pipeline functions, i.e., hook callback functions and chains
		and GLSL based internal image processing pipeline.
		
		The level of support for PTB's imaging pipe strongly depends on the capabilities of the gfx-hardware,
		especially GLSL support, shader support and Framebuffer object support.
		
	NOTES:
	
	TO DO: 

*/

#include "Screen.h"

// This array maps hook point name strings to indices. The symbolic constants in
// PsychImagingPipelineSupport.h define symbolic names for the indices for fast
// lookup by name:
#define MAX_HOOKNAME_LENGTH 40
#define MAX_HOOKSYNOPSIS_LENGTH 120

char PsychHookPointNames[MAX_SCREEN_HOOKS][MAX_HOOKNAME_LENGTH] = {
	"CloseOnscreenWindowPreGLShutdown",
	"CloseOnscreenWindowPostGLShutdown",
	"UserspaceBufferDrawingFinished",
	"StereoLeftCompositingBlit",
	"StereoRightCompositingBlit",
	"StereoCompositingBlit",
	"PostCompositingBlit",
	"FinalOutputFormattingBlit",
	"UserspaceBufferDrawingPrepare"
};

char PsychHookPointSynopsis[MAX_SCREEN_HOOKS][MAX_HOOKSYNOPSIS_LENGTH] = {
	"HelpCloseOnscreenWindowPreGLShutdown",
	"HelpCloseOnscreenWindowPostGLShutdown",
	"HelpUserspaceBufferDrawingFinished",
	"HelpStereoLeftCompositingBlit",
	"HelpStereoRightCompositingBlit",
	"HelpStereoCompositingBlit",
	"HelpPostCompositingBlit",
	"HelpFinalOutputFormattingBlit",
	"HelpUserspaceBufferDrawingPrepare"
};

/* PsychInitImagingPipelineDefaultsForWindowRecord()
 * Set "all-off" defaults in windowRecord. This is called during creation of a windowRecord.
 * It sets all imaging related fields to safe defaults.
 */
void PsychInitImagingPipelineDefaultsForWindowRecord(PsychWindowRecordType *windowRecord)
{
	int i;
	
	// Initialize everything to "all-off" default:
	for (i=0; i<MAX_SCREEN_HOOKS; i++) {
		windowRecord->HookChainEnabled[i]=FALSE;
		windowRecord->HookChain[i]=NULL;
	}
	
	// Disable all special framebuffer objects by default:
	windowRecord->drawBufferFBO[0]=-1;
	windowRecord->drawBufferFBO[1]=-1;
	windowRecord->processedDrawBufferFBO[0]=-1;
	windowRecord->processedDrawBufferFBO[1]=-1;
	windowRecord->processedDrawBufferFBO[2]=-1;
	windowRecord->preConversionFBO[0]=-1;
	windowRecord->preConversionFBO[1]=-1;
	windowRecord->preConversionFBO[2]=-1;
	windowRecord->finalizedFBO[0]=-1;
	windowRecord->finalizedFBO[1]=-1;
	windowRecord->fboCount = 0;

	// NULL-out fboTable:
	for (i=0; i<MAX_FBOTABLE_SLOTS; i++) windowRecord->fboTable[i] = NULL;
	
	// Setup mode switch in record to "all off":
	windowRecord->imagingMode = 0;

	return;
}

/*  PsychInitializeImagingPipeline()
 *
 *  Initialize imaging pipeline for windowRecord, applying the imagingmode flags. Called by Screen('OpenWindow').
 *
 *  This routine performs initial setup of an imaging pipeline for an onscreen window. It sets up reasonable
 *  default values in the windowRecord (imaging pipe is disabled by default if imagingmode is zero), based on
 *  the imagingmode flags and all the windowRecord and OpenGL settings.
 *
 *  1. FBO's are setup according to the requested imagingmode, stereomode and color depth of a window.
 *  2. Depending on stereo mode and imagingmode, some default GLSL shaders may get created and attached to
 *     some hook-chains for advanced stereo processing.
 */
void PsychInitializeImagingPipeline(PsychWindowRecordType *windowRecord, int imagingmode)
{	
	GLenum fboInternalFormat;
	int newimagingmode = 0;
	int fbocount = 0;
	int winwidth, winheight;
	Boolean needzbuffer, needoutputconversion, needimageprocessing, needseparatestreams, needfastbackingstore; 
		
	// Processing ends here after minimal "all off" setup, if pipeline is disabled:
	if (imagingmode<=0) {
		imagingmode=0;
		return;
	}
	
	// Activate rendering context of this window:
	PsychSetGLContext(windowRecord);

	// Specific setup of pipeline if real imaging ops are requested:
	if (PsychPrefStateGet_Verbosity()>2) printf("PTB-INFO: Psychtoolbox imaging pipeline enabled for window with requested imaging flags %i ...\n", imagingmode);
	fflush(NULL);
	
	// Setup mode switch in record:
	windowRecord->imagingMode = imagingmode;
	
	// Determine required precision for our framebuffer objects:

	// Start off with standard 8 bpc fixed point:
	fboInternalFormat = GL_RGBA8;
	
	// Need 16 bpc fixed point precision?
	if (imagingmode & kPsychNeed16BPCFixed) fboInternalFormat = GL_RGBA16;
	 
	// Need 16 bpc floating point precision?
	if (imagingmode & kPsychNeed16BPCFloat) fboInternalFormat = GL_RGBA_FLOAT16_APPLE;
	
	// Need 32 bpc floating point precision?
	if (imagingmode & kPsychNeed32BPCFloat) fboInternalFormat = GL_RGBA_FLOAT32_APPLE;

	// Do we need additional depth buffer attachments?
	needzbuffer = (PsychPrefStateGet_3DGfx()>0) ? TRUE : FALSE;
	
	// Do we need separate streams for stereo? Only for OpenGL quad-buffered mode:
	needseparatestreams = (windowRecord->stereomode == kPsychOpenGLStereo) ? TRUE : FALSE;

	// Do we need some intermediate image processing?
	needimageprocessing= (imagingmode & kPsychNeedImageProcessing) ? TRUE : FALSE;

	// Do we need some final output formatting?
	needoutputconversion = (imagingmode & kPsychNeedOutputConversion) ? TRUE : FALSE;
	
	// Do we need fast backing store?
	needfastbackingstore = (imagingmode & kPsychNeedFastBackingStore) ? TRUE : FALSE;
	
	// Consolidate settings:
	if (needoutputconversion || needimageprocessing || windowRecord->stereomode > 0) needfastbackingstore = TRUE;
	
	// Check if this system does support OpenGL framebuffer objects and rectangle textures:
	if (!glewIsSupported("GL_EXT_framebuffer_object") || (!glewIsSupported("GL_EXT_texture_rectangle") && !glewIsSupported("GL_ARB_texture_rectangle") && !glewIsSupported("GL_NV_texture_rectangle"))) {
		// Unsupported! This is a complete no-go :(
		printf("PTB-ERROR: Initialization of the built-in image processing pipeline failed. Your graphics hardware or graphics driver does not support\n");
		printf("PTB-ERROR: the required OpenGL framebuffer object extension. You may want to upgrade to the latest drivers or if that doesn't help, to a\n");
		printf("PTB-ERROR: more recent graphics card. You'll need at minimum a NVidia GeforceFX-5000 or a ATI Radeon 9600 or Intel GMA 950 for this to work.\n");
		printf("PTB-ERROR: See the www.psychtoolbox.org Wiki for recommendations. You can still use basic stereo support (with restricted performance and features)\n");
		printf("PTB-ERROR: by disabling the imaging pipeline (imagingmode = 0) but still selecting a stereomode in the 'OpenWindow' subfunction.\n");
		fflush(NULL);
		PsychErrorExitMsg(PsychError_user, "Imaging Pipeline setup: Sorry, your graphics card does not meet the minimum requirements for use of the imaging pipeline.");
	}

	// Try to allocate and configure proper FBO's:
	fbocount = 0;
	
	if (needfastbackingstore) {
		// We need at least the 1st level drawBufferFBO's as rendertargets for all
		// user-space drawing, ie Screen 2D drawing functions, MOGL OpenGL rendering and
		// C-MEX OpenGL rendering plugins...
		
		// Define dimensions of 1st stage FBO:
		winwidth=PsychGetWidthFromRect(windowRecord->rect);
		winheight=PsychGetHeightFromRect(windowRecord->rect);

		// Adapt it for some stereo modes:
		if (windowRecord->stereomode==kPsychFreeFusionStereo || windowRecord->stereomode==kPsychFreeCrossFusionStereo) {
			// Special case for stereo: Only half the real window width:
			winwidth = winwidth / 2;
		}

		// These FBO's may need a z-buffer or stencil buffer as well if 3D rendering is
		// enabled:
		if (!PsychCreateFBO(&(windowRecord->fboTable[fbocount]), fboInternalFormat, needzbuffer, winwidth, winheight)) {
			// Failed!
			PsychErrorExitMsg(PsychError_internal, "Imaging Pipeline setup: Could not setup stage 1 of imaging pipeline.");
		}
		
		// Assign this FBO as drawBuffer for left-eye or mono channel:
		windowRecord->drawBufferFBO[0] = fbocount;
		fbocount++;
		
		// If we are in stereo mode, we'll need a 2nd buffer for the right-eye channel:
		if (windowRecord->stereomode > 0) {
			if (!PsychCreateFBO(&(windowRecord->fboTable[fbocount]), fboInternalFormat, needzbuffer, winwidth, winheight)) {
				// Failed!
				PsychErrorExitMsg(PsychError_internal, "Imaging Pipeline setup: Could not setup stage 1 of imaging pipeline.");
			}
			
			// Assign this FBO as drawBuffer for right-eye channel:
			windowRecord->drawBufferFBO[1] = fbocount;
			fbocount++;
		}
	}
	
	// Do we need 2nd stage FBOs? We need them as targets for the processed data if support for misc image processing ops is requested.
	if (needimageprocessing) {
		// Need real FBO's as targets for image processing:

		// Define dimensions of 2nd stage FBO:
		winwidth=PsychGetWidthFromRect(windowRecord->rect);
		winheight=PsychGetHeightFromRect(windowRecord->rect);

		// Adapt it for some stereo modes:
		if (windowRecord->stereomode==kPsychFreeFusionStereo || windowRecord->stereomode==kPsychFreeCrossFusionStereo) {
			// Special case for stereo: Only half the real window width:
			winwidth = winwidth / 2;
		}

		// These FBO's don't need z- or stencil buffers anymore:
		if (!PsychCreateFBO(&(windowRecord->fboTable[fbocount]), fboInternalFormat, FALSE, winwidth, winheight)) {
			// Failed!
			PsychErrorExitMsg(PsychError_internal, "Imaging Pipeline setup: Could not setup stage 2 of imaging pipeline.");
		}
		
		// Assign this FBO as processedDrawBuffer for left-eye or mono channel:
		windowRecord->processedDrawBufferFBO[0] = fbocount;
		fbocount++;
		
		// If we are in stereo mode, we'll need a 2nd buffer for the right-eye channel:
		if (windowRecord->stereomode > 0) {
			if (!PsychCreateFBO(&(windowRecord->fboTable[fbocount]), fboInternalFormat, FALSE, winwidth, winheight)) {
				// Failed!
				PsychErrorExitMsg(PsychError_internal, "Imaging Pipeline setup: Could not setup stage 2 of imaging pipeline.");
			}
			
			// Assign this FBO as processedDrawBuffer for right-eye channel:
			windowRecord->processedDrawBufferFBO[1] = fbocount;
			fbocount++;
		}
		else {
			windowRecord->processedDrawBufferFBO[1] = 0;
		}
		
		// Allocate a bounce-buffer as well:
		if (!PsychCreateFBO(&(windowRecord->fboTable[fbocount]), fboInternalFormat, FALSE, winwidth, winheight)) {
			// Failed!
			PsychErrorExitMsg(PsychError_internal, "Imaging Pipeline setup: Could not setup stage 2 of imaging pipeline.");
		}
		
		// Assign this FBO as processedDrawBuffer for left-eye or mono channel:
		windowRecord->processedDrawBufferFBO[2] = fbocount;
		fbocount++;
	}
	else {
		// No image processing: Set 2nd stage FBO's to 1st stage FBO's:
		windowRecord->processedDrawBufferFBO[0] = windowRecord->drawBufferFBO[0];
		windowRecord->processedDrawBufferFBO[1] = windowRecord->drawBufferFBO[1];
		windowRecord->processedDrawBufferFBO[2] = 0;
	}
	
	// Stage 2 ready. Any need for real merged FBO's? We need a merged FBO if we are in stereo mode
	// and in need to merge output from the two views and to postprocess that output. In all other
	// cases there's no need for real merged FBO's and we do just a "pass-through" assignment.
	if ((windowRecord->stereomode > 0) && (!needseparatestreams) && (needoutputconversion)) {
		// Need real FBO's as targets for merger output.

		// Define dimensions of 3rd stage FBO:
		winwidth=PsychGetWidthFromRect(windowRecord->rect);
		winheight=PsychGetHeightFromRect(windowRecord->rect);

		// These FBO's don't need z- or stencil buffers anymore:
		if (!PsychCreateFBO(&(windowRecord->fboTable[fbocount]), fboInternalFormat, FALSE, winwidth, winheight)) {
			// Failed!
			PsychErrorExitMsg(PsychError_internal, "Imaging Pipeline setup: Could not setup stage 3 of imaging pipeline.");
		}
		
		// Assign this FBO for left-eye and right-eye channel: The FBO is shared accross channels...
		windowRecord->preConversionFBO[0] = fbocount;
		windowRecord->preConversionFBO[1] = fbocount;
		fbocount++;

		// Allocate a bounce-buffer as well:
		if (!PsychCreateFBO(&(windowRecord->fboTable[fbocount]), fboInternalFormat, FALSE, winwidth, winheight)) {
			// Failed!
			PsychErrorExitMsg(PsychError_internal, "Imaging Pipeline setup: Could not setup stage 3 of imaging pipeline.");
		}
		
		windowRecord->preConversionFBO[2] = fbocount;
		fbocount++;
	}
	else {
		// No real merger FBO's: Set 3rd stage FBO's to 2nd stage FBO's:
		windowRecord->preConversionFBO[0] = windowRecord->processedDrawBufferFBO[0];
		windowRecord->preConversionFBO[1] = windowRecord->processedDrawBufferFBO[1];
		windowRecord->preConversionFBO[2] = windowRecord->processedDrawBufferFBO[2];
	}
	
	// Define output buffers as system framebuffers:
	windowRecord->finalizedFBO[0]=0;
	windowRecord->finalizedFBO[1]=0;

	// Setup imaging mode flags:
	newimagingmode = (needseparatestreams) ? kPsychNeedSeparateStreams : 0;
	if (!needseparatestreams && (windowRecord->stereomode > 0)) newimagingmode |= kPsychNeedStereoMergeOp;
	if (needfastbackingstore) newimagingmode |= kPsychNeedFastBackingStore;
	if (needoutputconversion) newimagingmode |= kPsychNeedOutputConversion;
	if (needimageprocessing)  newimagingmode |= kPsychNeedImageProcessing;
	if (imagingmode & kPsychNeed32BPCFloat) {
		newimagingmode |= kPsychNeed32BPCFloat;
	}
	else if (imagingmode & kPsychNeed16BPCFloat) {
		newimagingmode |= kPsychNeed16BPCFloat;
	}
	else if (imagingmode & kPsychNeed16BPCFixed) {
		newimagingmode |= kPsychNeed16BPCFixed;
	}
	
	// Set new imaging mode:
	windowRecord->imagingMode = newimagingmode;
	windowRecord->fboCount = fbocount;
	
	// TODO: Setup stereo shaders for the different stereo modes...

	// Perform a full reset of current drawing target. This is a warm-start of PTB's drawing
	// engine, so the next drawing command will trigger binding the proper FBO of our pipeline.
	// Before this point (==OpenWindow time), all drawing was directly directed to the system
	// framebuffer - important for all the timing tests and calibrations to work correctly.
	PsychSetDrawingTarget(NULL);

	// Well done.
	return;
}

/* PsychCreateFBO()
 * Create OpenGL framebuffer object for internal rendering, setup PTB info struct for it.
 * This function creates an OpenGL framebuffer object, creates and attaches a texture of suitable size
 * (width x height) pixels with format fboInternalFormat as color buffer (color attachment 0). Optionally,
 * (if needzbuffer is true) it also creates and attaches suitable z-buffer and stencil-buffer attachments.
 * It checks for correct setup and then stores all relevant information in the PsychFBO struct, pointed by
 * fbo. On success it returns true, on failure it returns false.
 */
Boolean PsychCreateFBO(PsychFBO** fbo, GLenum fboInternalFormat, Boolean needzbuffer, int width, int height)
{
	GLenum fborc;
	GLint bpc;
	GLboolean isFloatBuffer;
	
	// Eat all GL errors:
	PsychTestForGLErrors();
	
	// If fboInternalFomrat!=1 then we need to allocate and assign a proper PsychFBO struct first:
	if (fboInternalFormat!=1) {
		*fbo = (PsychFBO*) malloc(sizeof(PsychFBO));
		if (*fbo == NULL) PsychErrorExitMsg(PsychError_outofMemory, "Out of system memory when trying to allocate PsychFBO struct!");

		// Start cleanly for error handling:
		(*fbo)->fboid = 0;
		(*fbo)->stexid = 0;
		(*fbo)->ztexid = 0;
		
		// fboInternalFormat == 0 --> Only allocate and assign, don't initialize FBO.
		if (fboInternalFormat==0) return(TRUE);
	}

	// Is there already a texture object defined for the color attachment?
	if (fboInternalFormat != (GLenum) 1) {
		// No, need to create one:
		
		// Build color buffer: Create a new texture handle for the color buffer attachment.
		glGenTextures(1, (GLuint*) &((*fbo)->coltexid));
		
		// Bind it as rectangle texture:
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, (*fbo)->coltexid);
		
		// Create proper texture: Just allocate proper format, don't assign data.
		glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, fboInternalFormat, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	}
	else {
		// Yes. Bind it as rectangle texture:
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, (*fbo)->coltexid);
	}
	
	if (glGetError()!=GL_NO_ERROR) {
		printf("PTB-ERROR: Failed to setup internal framebuffer objects color buffer attachment for imaging pipeline!\n");
		printf("PTB-ERROR: Most likely the requested size & depth of the window or texture is not supported by your graphics hardware.\n");
		return(FALSE);
	}
	
    // Setup texture wrapping behaviour to clamp, as other behaviours are
    // unsupported on many gfx-cards for rectangle textures:
    glTexParameterf(GL_TEXTURE_RECTANGLE_EXT,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameterf(GL_TEXTURE_RECTANGLE_EXT,GL_TEXTURE_WRAP_T,GL_CLAMP);
    // Setup filtering for the textures - Use nearest neighbour by default, as floating
    // point filtering usually unsupported.
    glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	// Texture ready, unbind it.
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);

	// Create a new framebuffer object and bind it:
	glGenFramebuffersEXT(1, (GLuint*) &((*fbo)->fboid));
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (*fbo)->fboid);
	
	// Attach the texture as color buffer zero:
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_EXT, (*fbo)->coltexid, 0);
	
	// Check for framebuffer completeness:
	fborc = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (fborc!=GL_FRAMEBUFFER_COMPLETE_EXT) {
		// Framebuffer incomplete!
		while(glGetError());
		printf("PTB-ERROR: Failed to setup internal framebuffer objects color buffer attachment for imaging pipeline!\n");
		if (fborc==GL_FRAMEBUFFER_UNSUPPORTED_EXT) {
			printf("PTB-ERROR: Your graphics hardware does not support the selected or requested format for drawing into it.\n");
		}
		else {
			printf("PTB-ERROR: Exact reason for failure is unknown.\n");
		}
		printf("PTB-ERROR: You may want to retry with the lowest acceptable (for your study) size and depth of the onscreen window or offscreen window.\n");
		return(FALSE);
	}
	
	// Do we need additional buffers for 3D rendering?
	if (needzbuffer) {
		// Yes. Try to setup and attach them:
		if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: Trying to attach depth+stencil attachments to FBO...\n"); 
		if (!glewIsSupported("GL_ARB_depth_texture")) {
			printf("PTB-ERROR: Failed to setup internal framebuffer object for imaging pipeline! Your graphics hardware does not support\n");
			printf("PTB-ERROR: the required GL_ARB_depth_texture extension. You'll need at least a NVidia GeforceFX 5000 or ATI Radeon 9600\n");
			printf("PTB-ERROR: for this to work.\n");
			return(FALSE);
		}
		
		// Create texture object for z-buffer (or z+stencil buffer) and set it up:
		glGenTextures(1, (GLuint*) &((*fbo)->ztexid));
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, (*fbo)->ztexid);
		
		// Setup texture wrapping behaviour to clamp, as other behaviours are
		// unsupported on many gfx-cards for rectangle textures:
		glTexParameterf(GL_TEXTURE_RECTANGLE_EXT,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameterf(GL_TEXTURE_RECTANGLE_EXT,GL_TEXTURE_WRAP_T,GL_CLAMP);
		
		// Setup filtering for the textures - Use nearest neighbour by default, as floating
		// point filtering usually unsupported.
		glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		
		// Just to be safe...
		PsychTestForGLErrors();

		// Do we have support for combined 24 bit depth and 8 bit stencil buffer textures?
		if (glewIsSupported("GL_EXT_packed_depth_stencil")) {
			// Yes! Create combined depth and stencil texture:
			if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: packed_depth_stencil supported. Attaching combined 24 bit depth + 8 bit stencil texture...\n"); 

			// Create proper texture: Just allocate proper format, don't assign data.
			glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_DEPTH24_STENCIL8_EXT, width, height, 0, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, NULL);
			PsychTestForGLErrors();

			// Texture ready, unbind it.
			glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);
			
			// Attach the texture as depth buffer...
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_RECTANGLE_EXT, (*fbo)->ztexid, 0);
			// ... and as stencil buffer ...
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_RECTANGLE_EXT, (*fbo)->ztexid, 0);
		}
		else {
			// Packed depth+stencil textures unsupported :( 
			// Allocate a single depth texture and attach it. Then see what we can do about the stencil attachment...
			if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: packed_depth_stencil unsupported. Attaching 24 bit depth texture and 8 bit stencil renderbuffer...\n"); 

			// Create proper texture: Just allocate proper format, don't assign data.
			glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

			// Depth texture ready, unbind it.
			glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);
			
			// Attach the texture as depth buffer...
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_RECTANGLE_EXT, (*fbo)->ztexid, 0);
			
			// Create and attach renderbuffer as a stencil buffer of 8 bit depths:
			glGenRenderbuffersEXT(1, (GLuint*) &((*fbo)->stexid));
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, (*fbo)->stexid);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX8_EXT, width, height);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, (*fbo)->stexid);
			
			// See if we are framebuffer complete:
			if (GL_FRAMEBUFFER_COMPLETE_EXT != glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)) {
				// Nope. Our trick doesn't work, this hardware won't let us attach a stencil buffer at all. Remove it
				// and live with a depth-buffer only setup.
				if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: Stencil renderbuffer attachment failed! Detaching stencil buffer...\n"); 
				if (PsychPrefStateGet_Verbosity()>1) {
					printf("PTB-WARNING: OpenGL stencil buffers not supported in imagingmode by your hardware. This won't affect Screen 2D drawing functions and won't affect\n");
					printf("PTB-WARNING: the majority of OpenGL (MOGL) 3D drawing code either, but OpenGL code that needs a stencil buffer will misbehave or fail in random ways!\n");
					printf("PTB-WARNING: If you need to use such code, you'll either have to disable the internal imaging pipeline, or carefully work-around this limitation by\n");
					printf("PTB-WARNING: proper modifications and testing of the affected code. Good luck... Alternatively, upgrade your graphics hardware. According to specs,\n");
					printf("PTB-WARNING: all gfx-cards starting with GeForceFX 5000 on Windows and Linux and all cards on Intel-Macs except the Intel GMA cards should work, whereas\n");
					printf("PTB-WARNING: none of the PowerPC hardware is supported as of OS-X 10.4.8.\n"); 
				}
				
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
				glDeleteRenderbuffersEXT(1, (GLuint*) &((*fbo)->stexid));
				(*fbo)->stexid = 0;
			}
		}
	}
	else {
		// Initialize additional buffers to zero:
		if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: Only colorbuffer texture attached to FBO, no depth- or stencil buffers requested...\n"); 
		(*fbo)->stexid = 0;
		(*fbo)->ztexid = 0;
	}
	
	// Store dimensions:
	(*fbo)->width = width;
	(*fbo)->height = height;
	
	// Check for framebuffer completeness:
	fborc = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (fborc!=GL_FRAMEBUFFER_COMPLETE_EXT) {
		// Framebuffer incomplete!
		while(glGetError());
		printf("PTB-ERROR: Failed to setup internal framebuffer object for imaging pipeline [%s]! The most likely cause is that your hardware does not support\n", (fborc==GL_FRAMEBUFFER_UNSUPPORTED_EXT) ? "Unsupported format" : "Unknown error");
		printf("PTB-ERROR: the required buffers at the given screen resolution (Additional 3D buffers for z- and stencil are %s).\n", (needzbuffer) ? "requested" : "disabled");
		printf("PTB-ERROR: You may want to retry with the lowest acceptable (for your study) display resolution or with 3D rendering support disabled.\n");
		return(FALSE);
	}
	
	if (PsychPrefStateGet_Verbosity()>4) {
		// Output framebuffer properties:
		glGetIntegerv(GL_RED_BITS, &bpc);
		printf("PTB-DEBUG: FBO has %i bits per color component in ", bpc);
		if (glewIsSupported("GL_ARB_color_buffer_float")) {
			glGetBooleanv(GL_RGBA_FLOAT_MODE_ARB, &isFloatBuffer);
		}
		else if (glewIsSupported("GL_APPLE_float_pixels")) { 
			glGetBooleanv(GL_COLOR_FLOAT_APPLE, &isFloatBuffer);
		}
		else isFloatBuffer = FALSE;

        if (isFloatBuffer) {
            printf("floating point format ");
        }
        else {
            printf("fixed point (or unknown) precision ");
        }

		glGetIntegerv(GL_DEPTH_BITS, &bpc);
		printf("with  %i depths buffer bits ", bpc);
		glGetIntegerv(GL_STENCIL_BITS, &bpc);
		printf("and %i stencil buffer bits.\n", bpc);
	}

	// Unbind FBO:
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	// Test all GL errors:
	PsychTestForGLErrors();

	// Well done.
	return(TRUE);
}

/* PsychShutdownImagingPipeline()
 * Shutdown imaging pipeline for a windowRecord and free all ressources associated with it.
 */
void PsychShutdownImagingPipeline(PsychWindowRecordType *windowRecord, Boolean openglpart)
{
	int i;
	PtrPsychHookFunction hookfunc, hookiter;
	PsychFBO* fboptr;
	
	// Imaging enabled? Do OpenGL specific cleanup:
	if (windowRecord->imagingMode>0 && openglpart) {
		// Yes. Mode specific cleanup:
		for (i=0; i<windowRecord->fboCount; i++) {
			// Delete i'th FBO, if any:
			fboptr = windowRecord->fboTable[i];
			if (fboptr!=NULL) { 
				// Delete all remaining references to this fbo:
				for (i=0; i<windowRecord->fboCount; i++) if (fboptr == windowRecord->fboTable[i]) windowRecord->fboTable[i] = NULL;
				
				// Detach and delete color buffer texture:
				if (fboptr->coltexid) glDeleteTextures(1, &(fboptr->coltexid));
				// Detach and delete depth buffer (and probably stencil buffer) texture, if any:
				if (fboptr->ztexid) glDeleteTextures(1, &(fboptr->ztexid));
				// Detach and delete stencil renderbuffer, if a separate stencil buffer was needed:
				if (fboptr->stexid) glDeleteRenderbuffersEXT(1, &(fboptr->stexid));
				// Delete FBO itself:
				if (fboptr->fboid) glDeleteFramebuffersEXT(1, &(fboptr->fboid));
				
				// Delete PsychFBO struct associated with this FBO:
				free(fboptr); fboptr = NULL;
			}
		}
	} 

	// The following cleanup must only happen after OpenGL rendering context is already detached and
	// destroyed. It's part of phase-2 "post GL shutdown" of Screen('Close') and friends...
	if (!openglpart) {
		// Clear all hook chains:
		for (i=0; i<MAX_SCREEN_HOOKS; i++) {
			windowRecord->HookChainEnabled[i]=FALSE;
			PsychPipelineResetHook(windowRecord, PsychHookPointNames[i]);
		}
		
		// Global off:
		windowRecord->imagingMode=0;
	}
	
	// Cleanup done.
	return;
}

void PsychPipelineListAllHooks(PsychWindowRecordType *windowRecord)
{
	int i;
	printf("PTB-INFO: The Screen command currently provides the following hook functions:\n");
	printf("=============================================================================\n");
	for (i=0; i<MAX_SCREEN_HOOKS; i++) {
		printf("- %s : %s\n", PsychHookPointNames[i], PsychHookPointSynopsis[i]);
	}
	printf("=============================================================================\n\n");	
	fflush(NULL);
	// Well done.
	return;	
}

/* Map a hook name string to its corresponding hook chain index:
 * Returns -1 if such a hook name doesn't exist.
 */
int	PsychGetHookByName(const char* hookName)
{
	int i;
	for (i=0; i<MAX_SCREEN_HOOKS; i++) {
		if(strcmp(PsychHookPointNames[i], hookName)==0) break; 
	}
	return((i>=MAX_SCREEN_HOOKS) ? -1 : i);
}

/* Internal: PsychAddNewHookFunction()  - Add a new hook callback function to a hook-chain.
 * This helper function allocates a hook func struct, enqueues it into a hook chain and sets
 * all common struct fields to their proper values. Then it returns a pointer to the struct, so
 * the calling routine can fill the rest of the struct with information.
 *
 * windowRecord - The windowRecord of the window to attach to.
 * hookString   - String with the human-readable name of the hook-chain to attach to.
 * idString     - Arbitrary name string for this hook function (for debugging and finding it programmatically)
 * where        - Attachment point: 0 = Prepend to chain (Recommended). 1 = Append to chain (Use with care!).
 * hookfunctype - Symbolic hook function type id, needed for interpreter to distinguish different types.
 *
 */
PsychHookFunction* PsychAddNewHookFunction(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, int hookfunctype)
{
	int hookidx;
	PtrPsychHookFunction hookfunc, hookiter;
	
	// Lookup hook-chain idx for this name, if any:
	if ((hookidx=PsychGetHookByName(hookString))==-1) PsychErrorExitMsg(PsychError_user, "AddHook: Unknown (non-existent) hook name provided.");
	
	// Allocate a hook structure:
	hookfunc =	(PtrPsychHookFunction) calloc(1, sizeof(PsychHookFunction));
	if (hookfunc==NULL) PsychErrorExitMsg(PsychError_outofMemory, "Failed to allocate memory for new hook function.");
	
	// Enqueue at beginning or end of chain:
	if (where==0) {
		// Prepend it to existing chain:
		hookfunc->next = windowRecord->HookChain[hookidx];
		windowRecord->HookChain[hookidx] = hookfunc;
	}
	else {
		// Append it to existing chain:
		hookfunc->next = NULL;
		if (windowRecord->HookChain[hookidx]==NULL) {
			windowRecord->HookChain[hookidx]=hookfunc;
		}
		else {
			hookiter = windowRecord->HookChain[hookidx];
			while (hookiter->next) hookiter = hookiter->next;
			hookiter->next = hookfunc;
		}
	}
	
	// New hookfunc struct is enqueued and zero initialized. Fill rest of its fields:
	hookfunc->idString = strdup(idString);
	hookfunc->hookfunctype = hookfunctype;
	
	// Return pointer to new hook slot:
	return(hookfunc);
}

/* PsychPipelibneDisableHook - Disable named hook chain. */
void PsychPipelineDisableHook(PsychWindowRecordType *windowRecord, const char* hookString)
{
	int hook=PsychGetHookByName(hookString);
	if (hook==-1) PsychErrorExitMsg(PsychError_user, "DisableHook: Unknown (non-existent) hook name provided.");
	windowRecord->HookChainEnabled[hook] = FALSE;
	return;
}

/* PsychPipelibneEnableHook - Enable named hook chain. */
void PsychPipelineEnableHook(PsychWindowRecordType *windowRecord, const char* hookString)
{
	int hook=PsychGetHookByName(hookString);
	if (hook==-1) PsychErrorExitMsg(PsychError_user, "EnableHook: Unknown (non-existent) hook name provided.");
	windowRecord->HookChainEnabled[hook] = TRUE;
	return;
}

/* PsychPipelineResetHook() - Reset named hook chain by deleting all assigned callback functions.
 * windowRecord - Window/Texture for which processing chain should be reset.
 * hookString   - Name string of hook chain to reset.
 */
void PsychPipelineResetHook(PsychWindowRecordType *windowRecord, const char* hookString)
{
	PtrPsychHookFunction hookfunc, hookiter;
	int hookidx=PsychGetHookByName(hookString);
	if (hookidx==-1) PsychErrorExitMsg(PsychError_user, "ResetHook: Unknown (non-existent) hook name provided.");
	hookiter = windowRecord->HookChain[hookidx]; 
	while(hookiter) {
			hookfunc = hookiter;
			hookiter = hookiter->next;
			// Delete all referenced memory:
			free(hookfunc->idString);
			free(hookfunc->pString1);
			// Delete hookfunc struct itself:
			free(hookfunc);
	}

	// Null-out hook chain:
	windowRecord->HookChain[hookidx]=NULL;
	return;
}

/* PsychPipelineAddShaderToHook()
 * Add a GLSL shader program object to a hook chain. The shader is executed when the corresponding
 * hook chain slot is executed, using the specified blitter and OpenGL context configuration and the
 * specified lut texture bound to unit 1.
 *
 * windowRecord - Execute for this window/texture.
 * hookString   - Attach to this named chain.
 * idString     - Arbitrary name string for identification (query) and debugging.
 * where        - Where to attach (0=Beginning, 1=End).
 * shaderid		- GLSL shader program object id.
 * blitterString - Config string to define the used blitter function and its config.
 * luttexid1	- Id of texture to be bound to first texture unit (unit 1).
 */
void PsychPipelineAddShaderToHook(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, unsigned int shaderid, const char* blitterString, unsigned int luttexid1)
{
	// Create and attach proper preinitialized hook function and return pointer to it for further initialization:
	PtrPsychHookFunction hookfunc = PsychAddNewHookFunction(windowRecord, hookString, idString, where, kPsychShaderFunc);
	// Init remaining fields:
	hookfunc->shaderid =  shaderid;
	hookfunc->pString1 =  strdup(blitterString);
	hookfunc->luttexid1 = luttexid1;
	return;
}

/* PsychPipelineAddCFunctionToHook()
 * Add a C callback function to a hook chain. The C callback function is executed when the corresponding
 * hook chain slot is executed, passing a set of parameters via a void* struct pointer. The set of parameters
 * is dependent on the exact hook chain, so users of this function must have knowledge of the PTB-3 source code
 * to know what to expect.
 *
 * windowRecord - Execute for this window/texture.
 * hookString   - Attach to this named chain.
 * idString     - Arbitrary name string for identification (query) and debugging.
 * where        - Where to attach (0=Beginning, 1=End).
 * procPtr		- A void* function pointer which will be cast to a proper function pointer during execution.
 */
void PsychPipelineAddCFunctionToHook(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, void* procPtr)
{
	// Create and attach proper preinitialized hook function and return pointer to it for further initialization:
	PtrPsychHookFunction hookfunc = PsychAddNewHookFunction(windowRecord, hookString, idString, where, kPsychCFunc);
	// Init remaining fields:
	hookfunc->cprocfunc =  procPtr;
	return;
}

/* PsychPipelineAddRuntimeFunctionToHook()
 * Add a runtime environment callback function to a hook chain. The function is executed when the corresponding
 * hook chain slot is executed, passing a set of parameters. The set of parameters depends on the exact hook
 * chain, so users of this function must have knowledge of the PTB-3 source code to know what to expect.
 *
 * The mechanism to execute a runtime function depends on the runtime environment. On Matlab and Octave, the
 * internal feval() functions are used to call a Matlab- or Octave function, either a builtin function or some
 * function defined as M-File or dynamically linked.
 *
 * windowRecord - Execute for this window/texture.
 * hookString   - Attach to this named chain.
 * idString     - Arbitrary name string for identification (query) and debugging.
 * where        - Where to attach (0=Beginning, 1=End).
 * evalString	- A function string to be passed to the runtime environment for evaluation during execution.
 */
void PsychPipelineAddRuntimeFunctionToHook(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, const char* evalString)
{
	// Create and attach proper preinitialized hook function and return pointer to it for further initialization:
	PtrPsychHookFunction hookfunc = PsychAddNewHookFunction(windowRecord, hookString, idString, where, kPsychMFunc);
	// Init remaining fields:
	hookfunc->pString1 =  strdup(evalString);
	return;
}

/* PsychPipelineAddBuiltinFunctionToHook()
 * Add a builtin callback function to a hook chain. The function is executed when the corresponding
 * hook chain slot is executed, passing a set of parameters. The set of parameters depends on the exact hook
 * chain, so users of this function must have knowledge of the PTB-3 source code to know what to expect.
 *
 * windowRecord - Execute for this window/texture.
 * hookString   - Attach to this named chain.
 * idString     - This idString defines the builtin function to call.
 * where        - Where to attach (0=Beginning, 1=End).
 * configString	- A string with configuration parameters.
 */
void PsychPipelineAddBuiltinFunctionToHook(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, const char* configString)
{
	// Create and attach proper preinitialized hook function and return pointer to it for further initialization:
	PtrPsychHookFunction hookfunc = PsychAddNewHookFunction(windowRecord, hookString, idString, where, kPsychBuiltinFunc);
	// Init remaining fields:
	hookfunc->pString1 =  strdup(configString);
	return;
}

/* PsychPipelineQueryHookSlot
 * Query properties of a specific hook slot in a specific hook chain of a specific window:
 * windowRecord - Query for this window/texture.
 * hookString   - Query this named chain.
 * idString     - This string defines the specific slot to query. Can contain an integral number, then the associated slot is
 *				  queried, or a idString (as assigned during creation), then a slot with that name is queried.
 *
 * On successfull return, the following values are assigned, on unsuccessfull return (=-1), nothing is assigned:
 * idString = The name string of this slot *Read-Only*
 * blitterString = Config string for this slot.
 * doubleptr = Double encoded void* to the C-Callback function, if any.
 * shaderid = Double encoded shader handle for GLSL shader, if any.
 * luttexid = Double encoded lut texture handle for unit 1, if any.
 *
 * The return value contains the hook slot type. 
 */
int PsychPipelineQueryHookSlot(PsychWindowRecordType *windowRecord, const char* hookString, char** idString, char** blitterString, double* doubleptr, double* shaderid, double* luttexid1)
{
	PtrPsychHookFunction hookfunc;
	int targetidx, idx;
	int nrassigned = sscanf((*idString), "%i", &targetidx);
	int hookidx=PsychGetHookByName(hookString);
	if (hookidx==-1) PsychErrorExitMsg(PsychError_user, "QueryHook: Unknown (non-existent) hook name provided.");
	if (nrassigned != 1) targetidx=-1;
	idx=0;
	
	// Perform linear search until proper slot reached or proper name reached:
	hookfunc = windowRecord->HookChain[hookidx]; 
	while(hookfunc && ((targetidx>-1 && idx<targetidx) || (targetidx==-1 && strcmp(*idString, hookfunc->idString)!=0))) {
			hookfunc = hookfunc->next;
			idx++;
	}
	
	// If hookfunc is non-NULL then we found our slot:
	if (hookfunc==NULL) {
		*idString = NULL;
		*blitterString = NULL;
		*doubleptr = 0;
		*shaderid = 0;
		*luttexid1 = 0;
		return(-1);
	}
	
	*idString = hookfunc->idString;
	*blitterString = hookfunc->pString1;
	*doubleptr = PsychPtrToDouble(hookfunc->cprocfunc);
	*shaderid = (double) hookfunc->shaderid;
	*luttexid1= (double) hookfunc->luttexid1;
	return(hookfunc->hookfunctype);
}

/* PsychPipelineDumpHook
 * Dump properties of a specific hook chain of a specific window in a human-readable format into
 * the console of the scripting environment:
 *
 * windowRecord - Query for this window/texture.
 * hookString   - Query this named chain.
 */
void PsychPipelineDumpHook(PsychWindowRecordType *windowRecord, const char* hookString)
{
	PtrPsychHookFunction hookfunc;
	int i=0;
	int hookidx=PsychGetHookByName(hookString);
	if (hookidx==-1) PsychErrorExitMsg(PsychError_user, "DumpHook: Unknown (non-existent) hook name provided.");
	
	hookfunc = windowRecord->HookChain[hookidx];
	printf("Hook chain %s is currently %s.\n", hookString, (windowRecord->HookChainEnabled[hookidx]) ? "enabled" : "disabled");
	if (hookfunc==NULL) {
		printf("No processing assigned to this hook-chain.\n");
	}
	else {
		printf("Following hook slots are assigned to this hook-chain:\n");
		printf("=====================================================\n");
	}
	
	while(hookfunc) {
		printf("Slot %i: Id='%s' : ", i, hookfunc->idString);
		switch(hookfunc->hookfunctype) {
			case kPsychShaderFunc:
				printf("GLSL-Shader      : id=%i , luttex1=%i , blitter=%s\n", hookfunc->shaderid, hookfunc->luttexid1, hookfunc->pString1);
			break;
			
			case kPsychCFunc:
				printf("C-Callback       : void*= %p\n", hookfunc->cprocfunc);
			break;

			case kPsychMFunc:
				printf("Runtime-Function : Evalstring= %s\n", hookfunc->pString1);
			break;
			
			case kPsychBuiltinFunc:
				printf("Builtin-Function : Name= %s\n", hookfunc->idString);
			break;
		}
		
		// Next one, if any:
		i++;
		hookfunc = hookfunc->next;
	}
	
	printf("=====================================================\n\n");
	fflush(NULL);
	return;
}

/* PsychPipelineDumpAllHooks
 * Dump current state of all hook-points for given window. See PsychPipelineDumpHook()
 * for more info.
 */
void PsychPipelineDumpAllHooks(PsychWindowRecordType *windowRecord)
{
	int i;
	for (i=0; i<MAX_SCREEN_HOOKS; i++) {
		PsychPipelineDumpHook(windowRecord, PsychHookPointNames[i]);
	}
	return;
}

/* PsychPipelineExecuteHook()
 * Execute the full hook processing chain for a specific hook and a specific windowRecord.
 * This checks if the chain is enabled. If it isn't enabled, it skips processing.
 * If it is enabled, it iterates over the full chain, executes all assigned hook functions in order and uses the FBO's between minfbo and maxfbo
 * as pingpong buffers if neccessary.
 */
boolean PsychPipelineExecuteHook(PsychWindowRecordType *windowRecord, int hookId, void* hookUserData, void* hookBlitterFunction, boolean srcIsReadonly, boolean allowFBOSwizzle, PsychFBO** srcfbo1, PsychFBO** srcfbo2, PsychFBO** dstfbo, PsychFBO** bouncefbo)
{
	PtrPsychHookFunction hookfunc;
	int i=0;
	int pendingFBOpingpongs = 0;
	PsychFBO *mysrcfbo1, *mysrcfbo2, *mydstfbo, *mynxtfbo; 
	boolean gfxprocessing;
	GLint restorefboid = 0;
	
	// Child protection:
	if (hookId<0 || hookId>=MAX_SCREEN_HOOKS) PsychErrorExitMsg(PsychError_internal, "In PsychPipelineExecuteHook: Was asked to execute unknown (non-existent) hook chain with invalid id!");

	// Hook chain enabled for processing and contains at least one hook slot?
	if ((!windowRecord->HookChainEnabled[hookId]) || (windowRecord->HookChain[hookId] == NULL)) {
		// Chain is empty or disabled.
		return(TRUE);
	}
	
	// Is this an image processing hook?
	gfxprocessing = (srcfbo1!=NULL && dstfbo!=NULL) ? TRUE : FALSE;
		
	// Get start of enabled chain:
	hookfunc = windowRecord->HookChain[hookId];

	// Count number of needed ping-pong FBO switches inside this chain:
	while(hookfunc) {
		// Pingpong command?
		if (hookfunc->hookfunctype == kPsychBuiltinFunc && strcmp(hookfunc->idString, "PINGPONGFBOS")==0) pendingFBOpingpongs++;
		// Process next hookfunc slot in chain, if any:
		hookfunc = hookfunc->next;
	}
	
	if (gfxprocessing) {
		// Prepare gfx-processing:

		if ((pendingFBOpingpongs % 2) == 0) {
			// Even number of ping-pongs needed in this chain. We stream from source fbo to
			// destination fbo in first pass.
			mysrcfbo1 = *srcfbo1;
			mysrcfbo2 = (srcfbo2) ? *srcfbo2 : NULL;
			mydstfbo  = *dstfbo;
			mynxtfbo  = (bouncefbo) ? *bouncefbo : NULL;
		}
		else {
			// Odd number of ping-pongs needed. Initially stream from source to bouncefbo:
			mysrcfbo1 = *srcfbo1;
			mysrcfbo2 = (srcfbo2) ? *srcfbo2 : NULL;
			mydstfbo  = (bouncefbo) ? *bouncefbo : NULL;
			mynxtfbo  = *dstfbo;
		}
		
		// If this is a multi-pass chain we'll need a bounce buffer FBO:
		if (pendingFBOpingpongs > 0 && bouncefbo == NULL) PsychErrorExitMsg(PsychError_internal, "In gfx-hook chain processing: Required bounce-buffer missing!!!");
		
		// Enable associated GL context:
		PsychSetGLContext(windowRecord);
		
		// Save current FBO bindings and switch to system FB:
		if (glBindFramebufferEXT) {
			glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &restorefboid);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
		
		// Setup initial source -> target binding:
		PsychPipelineSetupRenderFlow(mysrcfbo1, mysrcfbo2, mydstfbo);
	}

	
	// Reget start of enabled chain:
	hookfunc = windowRecord->HookChain[hookId];

	// Iterate over all slots:
	while(hookfunc) {
		// Debug output, if requested:
		if (PsychPrefStateGet_Verbosity()>4) {
			printf("Hookchain %i : Slot %i: Id='%s' : ", hookId, i, hookfunc->idString);
			switch(hookfunc->hookfunctype) {
				case kPsychShaderFunc:
					printf("GLSL-Shader      : id=%i , luttex1=%i , blitter=%s\n", hookfunc->shaderid, hookfunc->luttexid1, hookfunc->pString1);
					break;
					
				case kPsychCFunc:
					printf("C-Callback       : void*= %p\n", hookfunc->cprocfunc);
					break;
					
				case kPsychMFunc:
					printf("Runtime-Function : Evalstring= %s\n", hookfunc->pString1);
					break;
					
				case kPsychBuiltinFunc:
					printf("Builtin-Function : Name= %s\n", hookfunc->idString);
					break;
			}
		}
		
		// Is this a ping-pong command?
		if (hookfunc->hookfunctype == kPsychBuiltinFunc && strcmp(hookfunc->idString, "PINGPONGFBOS")==0) {
			// Ping pong buffer swap requested:
			pendingFBOpingpongs--;
			mysrcfbo1 = mydstfbo;
			mydstfbo  = mynxtfbo;
			if ((pendingFBOpingpongs % 2) == 0) {
				// Even number of ping-pongs remaining in this chain.
				mynxtfbo  = (bouncefbo) ? *bouncefbo : NULL;
			}
			else {
				// Odd number of ping-pongs remaining.
				mynxtfbo  = *dstfbo;
			}
			
			if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: SWAPPING PING-PONG FBOS, %i swaps pending...\n", pendingFBOpingpongs);
			
			// Set new src -> dst binding:
			PsychPipelineSetupRenderFlow(mysrcfbo1, mysrcfbo2, mydstfbo);
		}
		else {
			// Normal hook function - Process this hook function:
			if (!PsychPipelineExecuteHookSlot(windowRecord, hookId, hookfunc, hookUserData, hookBlitterFunction, srcIsReadonly, allowFBOSwizzle, &mysrcfbo1, &mysrcfbo2, &mydstfbo, &mynxtfbo)) {
				// Failed!
				if (PsychPrefStateGet_Verbosity()>0) {
					printf("PTB-ERROR: Failed in processing of Hookchain %i : Slot %i: Id='%s'  --> Aborting chain processing. Set verbosity to 5 for extended debug output.\n", hookId, i, hookfunc->idString);
				}
				return(FALSE);
			}
		}
		
		// Process next hookfunc slot in chain, if any:
		i++;
		hookfunc = hookfunc->next;
	}

	if (gfxprocessing) {
		// Disable renderflow:
		PsychPipelineSetupRenderFlow(NULL, NULL, NULL);
		
		// Restore old FBO bindings:
		if (glBindFramebufferEXT) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, restorefboid);
	}

	// Done.
	return(TRUE);
}

/* PsychPipelineExecuteHookSlot()
 * Execute a single hookfunction slot in a hook chain for a specific window.
 */
boolean PsychPipelineExecuteHookSlot(PsychWindowRecordType *windowRecord, int hookId, PsychHookFunction* hookfunc, void* hookUserData, void* hookBlitterFunction, boolean srcIsReadonly, boolean allowFBOSwizzle, PsychFBO** srcfbo1, PsychFBO** srcfbo2, PsychFBO** dstfbo, PsychFBO** bouncefbo)
{
	// Dispatch by hook function type:
	switch(hookfunc->hookfunctype) {
		case kPsychShaderFunc:
			// Call a GLSL shader to do some image processing:
			printf("EXECUTING GLSL-Shader             : id=%i , luttex1=%i , blitter=%s\n", hookfunc->shaderid, hookfunc->luttexid1, hookfunc->pString1);
			
			
		break;
			
		case kPsychCFunc:
			printf("TODO: EXECUTE -- C-Callback       : void*= %p\n", hookfunc->cprocfunc);
		break;
			
		case kPsychMFunc:
			// Call the eval() function of our scripting runtime environment to evaluate
			// function string pString1. Currently supported are Matlab & Octave, so this
			// can be the call string of an arbitrary Matlab/Octave builtin or M-Function.
			// Care has to be taken that the called functions do not invoke any Screen
			// subfunctions! Screen is not reentrant, so that would likely screw seriously!
			PsychRuntimeEvaluateString(hookfunc->pString1);
		break;
			
		case kPsychBuiltinFunc:
			printf("TODO: EXECUTE -- Builtin-Function : Name= %s\n", hookfunc->idString);
		break;
			
		default:
			PsychErrorExitMsg(PsychError_internal, "In PsychPipelineExecuteHookSlot: Was asked to execute unknown (non-existent) hook function type!");
	}
	
	return(TRUE);
}

void PsychPipelineSetupRenderFlow(PsychFBO* srcfbo1, PsychFBO* srcfbo2, PsychFBO* dstfbo)
{
	static int ow, oh;
	int w, h;
	
	// Assign color texture of srcfbo2, if any,  to texture unit 1:
	glActiveTextureARB(GL_TEXTURE1_ARB);
	if (srcfbo2) {
		// srcfbo2 is valid: Assign its color buffer texture:
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, srcfbo2->coltexid);
		glEnable(GL_TEXTURE_RECTANGLE_EXT);
	}
	else {
		// srcfbo2 doesn't exist: Unbind and deactivate 2nd unit:
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);
		glDisable(GL_TEXTURE_RECTANGLE_EXT);
	}
	
	// Assign color texture of srcfbo1 to texture unit 0:
	glActiveTextureARB(GL_TEXTURE0_ARB);
	if (srcfbo1) {
		// srcfbo1 is valid: Assign its color buffer texture:
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, srcfbo1->coltexid);
		glEnable(GL_TEXTURE_RECTANGLE_EXT);
	}
	else {
		// srcfbo1 doesn't exist: Unbind and deactivate 1st unit:
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);
		glDisable(GL_TEXTURE_RECTANGLE_EXT);
	}
	
	// Select rendertarget:
	if (glBindFramebufferEXT) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (dstfbo) ? dstfbo->fboid : 0);
	
	if (dstfbo) {
		// Setup viewport, scissor rectangle and projection matrix for orthonormal rendering into the
		// target FBO or system framebuffer:
		w = (int) dstfbo->width;
		h = (int) dstfbo->height;
		
		// Geometry changed? We skip if not - state changes are expensive...
		if (w!=ow || h!=oh) {
			ow=w;
			oh=h;
			
			// Setup viewport and scissor for full FBO area:
			glViewport(0, 0, w, h);
			glScissor(0, 0, w, h);
			
			// Setup projection matrix for a proper orthonormal projection for this framebuffer:
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(0, w, 0, h);
			
			// Switch back to modelview matrix, but leave it unaltered:
			glMatrixMode(GL_MODELVIEW);
		}
	}
	else {
		ow=0;
		oh=0;
	}

	return;
}
