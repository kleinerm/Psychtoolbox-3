/*
	PsychToolbox3/Source/Common/Screen/PsychImagingPipelineSupport.c
	
	PLATFORMS:	
	
		All. Well, all with sufficiently advanced graphics hardware...
				
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
	
		We could implement a less capable minimal fallback-path for gfx-hardware that doesn't support
		framebuffer objects, but does support rectangle textures. Such hw would be limited to the initial
		pipeline stage (image processing) and would only allow single-pass processing, no multi-pass algs
		due to the lack of bounce buffers. But it could still allow for slightly more precise textures,
		and simple shaders (if supported) or basic fixed-function processing, e.g., simple contrast
		corrections or geometric undistortion.

*/

#include "Screen.h"

// Source code for our GLSL anaglyph stereo shader:
char anaglyphshadersrc[] = 
"/* Weight vector for conversion from RGB to Luminance, according to NTSC spec. */ \n"
"uniform vec3 ColorToGrayWeights; \n"
"/* Bias to add to final product - The background color (normally (0,0,0)). */ \n"
"uniform vec3 ChannelBias; \n"
"/* Left image channel and right image channel: */ \n"
"uniform sampler2DRect Image1; \n"
"uniform sampler2DRect Image2; \n"
"uniform vec3 Gains1;\n"
"uniform vec3 Gains2;\n"
"\n"
"void main()\n"
"{\n"
"    /* Lookup RGBA pixel colors in left- and right buffer and convert to Luminance */\n"
"    vec3 incolor1 = texture2DRect(Image1, gl_TexCoord[0].st).rgb;\n"
"    float luminance1 = dot(incolor1, ColorToGrayWeights);\n"
"    vec3 incolor2 = texture2DRect(Image2, gl_TexCoord[0].st).rgb;\n"
"    float luminance2 = dot(incolor2, ColorToGrayWeights);\n"
"    /* Replicate in own RGBA tupel */\n"
"    vec3 channel1 = vec3(luminance1);\n"
"    vec3 channel2 = vec3(luminance2);\n"
"    /* Mask with per channel weights: */\n"
"    channel1 = channel1 * Gains1;\n"
"    channel2 = channel2 * Gains2;\n"
"    /* Add them up to form final output fragment: */\n"
"    gl_FragColor.rgb = channel1 + channel2 + ChannelBias;\n"
"    /* Alpha is forced to 1 - It does not matter anymore: */\n"
"    gl_FragColor.a = 1.0;\n"
"}\n\0";

char passthroughshadersrc[] =
"uniform sampler2DRect Image1; \n"
"\n"
"void main()\n"
"{\n"
"    gl_FragColor.rgb = texture2DRect(Image1, gl_TexCoord[0].st).rgb;\n"
"    gl_FragColor.a = 1.0;\n"
"}\n\0";

// This array maps hook point name strings to indices. The symbolic constants in
// PsychImagingPipelineSupport.h define symbolic names for the indices for fast
// lookup by name:
#define MAX_HOOKNAME_LENGTH 40
#define MAX_HOOKSYNOPSIS_LENGTH 1024

char PsychHookPointNames[MAX_SCREEN_HOOKS][MAX_HOOKNAME_LENGTH] = {
	"CloseOnscreenWindowPreGLShutdown",
	"CloseOnscreenWindowPostGLShutdown",
	"UserspaceBufferDrawingFinished",
	"StereoLeftCompositingBlit",
	"StereoRightCompositingBlit",
	"StereoCompositingBlit",
	"PostCompositingBlit",
	"FinalOutputFormattingBlit",
	"UserspaceBufferDrawingPrepare",
	"IdentityBlitChain",
	"LeftFinalizerBlitChain",
	"RightFinalizerBlitChain",
	"UserDefinedBlit"
};

char PsychHookPointSynopsis[MAX_SCREEN_HOOKS][MAX_HOOKSYNOPSIS_LENGTH] = {
	"OpenGL based actions to be performed when an onscreen window is closed, e.g., teardown for special output devices.",
	"Non-graphics actions to be performed when an onscreen window is closed, e.g., teardown for special output devices.",
	"Operations to be performed after last drawing command, i.e. in Screen('Flip') or Screen('DrawingFinshed').",
	"Perform generic user-defined image processing on image content of left-eye (or mono) buffer.",
	"Perform generic user-defined image processing on image content of right-eye buffer.",
	"Internal(preinitialized): Compose left- and right-eye view into one combined image for all stereo modes except quad-buffered flip-frame stereo.",
	"Not yet used.",
	"Perform post-processing indifferent of stereo mode, e.g., special data formatting for devices like BrightSideHDR, Bits++, Video attenuators...",
	"Operations to be performed immediately after Screen('Flip') in order to prepare drawing commands of users script.",
	"Internal(preinitialized): Only for internal use. Only modify for debugging and testing of pipeline itself!",
	"Internal(preinitialized): Perform last time operation on left (or mono) channel, e.g., draw blue-sync lines.",
	"Internal(preinitialized): Perform last time operation on right channel, e.g., draw blue-sync lines.",
	"Defines a user defined image processing operation for the Screen('TransformTexture') command."
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
	Boolean needzbuffer, needoutputconversion, needimageprocessing, needseparatestreams, needfastbackingstore, targetisfinalFB;
	GLuint glsl;
	float rg, gg, bg;	// Gains for color channels and color masking for anaglyph shader setup.
	char blittercfg[1000];

	// Processing ends here after minimal "all off" setup, if pipeline is disabled:
	if (imagingmode<=0) {
		imagingmode=0;
		return;
	}
	
	// Safe default:
	targetisfinalFB = FALSE;
	
	// Activate rendering context of this window:
	PsychSetGLContext(windowRecord);

	// Specific setup of pipeline if real imaging ops are requested:
	if (PsychPrefStateGet_Verbosity()>2) printf("PTB-INFO: Psychtoolbox imaging pipeline starting up for window with requested imagingmode %i ...\n", imagingmode);
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
	
	// Consolidate settings: Most settings imply kPsychNeedFastBackingStore.
	if (needoutputconversion || needimageprocessing || windowRecord->stereomode > 0 || fboInternalFormat!=GL_RGBA8) {
		imagingmode|=kPsychNeedFastBackingStore;
		needfastbackingstore = TRUE;
	}
	
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

	// Another child protection:
	if ((windowRecord->windowType != kPsychDoubleBufferOnscreen) || PsychPrefStateGet_EmulateOldPTB()>0) {
		PsychErrorExitMsg(PsychError_user, "Imaging Pipeline setup: Sorry, imaging pipeline only supported on double buffered onscreen windows in non-emulation mode for old PTB.\n");
	}

	// Try to allocate and configure proper FBO's:
	fbocount = 0;
	
	// Define final default output buffers as system framebuffers: We create some pseudo-FBO's for these
	// which describe the system framebuffer (backbuffer). This is done to simplify pipeline design:

	// Allocate empty FBO info struct and assign it:
	winwidth=PsychGetWidthFromRect(windowRecord->rect);
	winheight=PsychGetHeightFromRect(windowRecord->rect);

	if (!PsychCreateFBO(&(windowRecord->fboTable[fbocount]), 0, FALSE, winwidth, winheight)) {
		// Failed!
		PsychErrorExitMsg(PsychError_internal, "Imaging Pipeline setup: Could not setup stage 0 of imaging pipeline.");
	}

	// The pseudo-FBO initially contains a fboid of zero == system framebuffer, and empty (zero) attachments.
	// The up to now only useful information is the viewport geometry ie winwidth and winheight.

	// We use the same struct for both buffers, because in the end, there is only one backbuffer. Separate channels
	// with same mapping allow some interesting extensions in the future for additional stereo modes or snapshot
	// creation...
	windowRecord->finalizedFBO[0]=fbocount;
	windowRecord->finalizedFBO[1]=fbocount;
	fbocount++;

	// Now we preinit all further stages with the finalizedFBO assignment. This way 
	
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
		
		// Windows with fast backing store always have 4 color channels RGBA, regardless what the
		// associated system framebuffer has:
		windowRecord->nrchannels = 4;

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

		// Is the target of imageprocessing (our processedDrawBufferFBO) the final destination? This is true if there is no further need
		// for more rendering passes in later processing stages and we don't do any processing here with more than 2 rendering passes. In that
		// case we can directly output to the finalizedFBO without need for one more intermediate buffer -- faster!
		// In all other cases, we'll need an additional buffer. We also exclude quad-buffered stereo, because the image processing blit chains
		// cannot switch between left- and right backbuffer of the system framebuffer...
		targetisfinalFB = ( !(imagingmode & kPsychNeedMultiPass) && (windowRecord->stereomode == kPsychMonoscopic) && !needoutputconversion ) ? TRUE : FALSE;

		if (!targetisfinalFB) {
			// These FBO's don't need z- or stencil buffers anymore:
			if (!PsychCreateFBO(&(windowRecord->fboTable[fbocount]), fboInternalFormat, FALSE, winwidth, winheight)) {
				// Failed!
				PsychErrorExitMsg(PsychError_internal, "Imaging Pipeline setup: Could not setup stage 2 of imaging pipeline.");
			}			

			// Assign this FBO as processedDrawBuffer for left-eye or mono channel:
			windowRecord->processedDrawBufferFBO[0] = fbocount;
			fbocount++;			
		}
		else {
			// Can assign final destination:
			windowRecord->processedDrawBufferFBO[0] = windowRecord->finalizedFBO[0];
		}
				
		// If we are in stereo mode, we'll need a 2nd buffer for the right-eye channel:
		if (windowRecord->stereomode > 0) {
			if (!targetisfinalFB) {
				// These FBO's don't need z- or stencil buffers anymore:
				if (!PsychCreateFBO(&(windowRecord->fboTable[fbocount]), fboInternalFormat, FALSE, winwidth, winheight)) {
					// Failed!
					PsychErrorExitMsg(PsychError_internal, "Imaging Pipeline setup: Could not setup stage 2 of imaging pipeline.");
				}			
				
				// Assign this FBO as processedDrawBuffer for right-eye channel:
				windowRecord->processedDrawBufferFBO[1] = fbocount;
				fbocount++;
			}
			else {
				// Can assign final destination:
				windowRecord->processedDrawBufferFBO[1] = windowRecord->finalizedFBO[1];
			}
		}
		else {
			// Mono mode: No right-eye buffer:
			windowRecord->processedDrawBufferFBO[1] = -1;
		}
		
		// Allocate a bounce-buffer as well if multi-pass rendering is requested:
		if (imagingmode & kPsychNeedDualPass || imagingmode & kPsychNeedMultiPass) {
			if (!PsychCreateFBO(&(windowRecord->fboTable[fbocount]), fboInternalFormat, FALSE, winwidth, winheight)) {
				// Failed!
				PsychErrorExitMsg(PsychError_internal, "Imaging Pipeline setup: Could not setup stage 2 of imaging pipeline.");
			}
			
			// Assign this FBO as processedDrawBuffer for bounce buffer ops in multi-pass rendering:
			windowRecord->processedDrawBufferFBO[2] = fbocount;
			fbocount++;
		}
		else {
			// No need for bounce-buffers, only single-pass processing requested.
			windowRecord->processedDrawBufferFBO[2] = -1;
		}
	}
	else {
		// No image processing: Set 2nd stage FBO's to 1st stage FBO's:
		windowRecord->processedDrawBufferFBO[0] = windowRecord->drawBufferFBO[0];
		windowRecord->processedDrawBufferFBO[1] = windowRecord->drawBufferFBO[1];
		windowRecord->processedDrawBufferFBO[2] = -1;
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

		// Request bounce buffer:
		windowRecord->preConversionFBO[2] = -1000;
	}
	else {
		if ((windowRecord->stereomode > 0) && (!needseparatestreams)) {
			// Need to merge two streams, but don't need any output conversion on them. We
			// don't need an extra FBO for the merge results! We just write our merge results
			// into whatever the final framebuffer is - Could be another FBO if framebuffer
			// snapshotting is requested, but most likely its the pseudo-FBO of the system
			// backbuffer. Anyway, the proper ones are stored in finalizedFBO[]:
			windowRecord->preConversionFBO[0] = windowRecord->finalizedFBO[0];
			windowRecord->preConversionFBO[1] = windowRecord->finalizedFBO[1];
			// Request bounce buffer:
			windowRecord->preConversionFBO[2] = -1000;
		}
		else {		
			// No merge operation needed. Do we need output conversion?
			if (needoutputconversion) {
				// Output conversion needed. Set input for this stage to output of the
				// image processing.
				windowRecord->preConversionFBO[0] = windowRecord->processedDrawBufferFBO[0];
				windowRecord->preConversionFBO[1] = windowRecord->processedDrawBufferFBO[1];
				// Request bounce buffer:
				windowRecord->preConversionFBO[2] = -1000;
			}
			else {
				// No merge and no output conversion needed. In that case, PsychPreFlipOperations()
				// will behave as if output conversion is requested, but with the identity blit chain,
				// and merge stage is skipped, so we need to set the preConversionFBO's as if conversion
				// is done.
				windowRecord->preConversionFBO[0] = windowRecord->processedDrawBufferFBO[0];
				windowRecord->preConversionFBO[1] = windowRecord->processedDrawBufferFBO[1];
				// No bounce buffer needed:
				windowRecord->preConversionFBO[2] = -1;
			}
			
		}		
	}

	// Do we need a bounce buffer for merging and/or conversion?
	if (windowRecord->preConversionFBO[2] == -1000) {
		// Yes. We can reuse/share the bounce buffer of the image processing stage if
		// one exists and is of suitable size i.e. we're not in dual-view stereo - in
		// that case all buffers are of same size.
		if ((windowRecord->processedDrawBufferFBO[2]!=-1) && 
			!(windowRecord->stereomode==kPsychFreeFusionStereo || windowRecord->stereomode==kPsychFreeCrossFusionStereo)) {
			// Stage 1 bounce buffer is suitable for sharing, assign it:
			windowRecord->preConversionFBO[2] = windowRecord->processedDrawBufferFBO[2];
		}
		else {
			// We need a new, private bounce-buffer:
			if (!PsychCreateFBO(&(windowRecord->fboTable[fbocount]), fboInternalFormat, FALSE, winwidth, winheight)) {
				// Failed!
				PsychErrorExitMsg(PsychError_internal, "Imaging Pipeline setup: Could not setup stage 3 of imaging pipeline.");
			}
			
			windowRecord->preConversionFBO[2] = fbocount;
			fbocount++;				
		}
	}		
	
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
	
	// Set new final imaging mode and fbocount:
	windowRecord->imagingMode = newimagingmode;
	windowRecord->fboCount = fbocount;

	// The pipelines buffers and information flow are configured now...
	if (PsychPrefStateGet_Verbosity()>4) {
		printf("PTB-DEBUG: Buffer mappings follow...\n");
		printf("fboCount = %i\n", windowRecord->fboCount);
		printf("finalizedFBO = %i, %i\n", windowRecord->finalizedFBO[0], windowRecord->finalizedFBO[1]);
		printf("preConversionFBO = %i, %i, %i\n", windowRecord->preConversionFBO[0], windowRecord->preConversionFBO[1], windowRecord->preConversionFBO[2]);
		printf("processedDrawBufferFBO = %i %i %i\n", windowRecord->processedDrawBufferFBO[0], windowRecord->processedDrawBufferFBO[1], windowRecord->processedDrawBufferFBO[2]);
		printf("drawBufferFBO = %i %i \n", windowRecord->drawBufferFBO[0], windowRecord->drawBufferFBO[1]);
		printf("-------------------------------------\n\n");
		fflush(NULL);
	}

	// Setup our default chain: This chain is executed if some stage of the imaging pipe is set up according
	// to imagingMode, but the corresponding hook-chain is empty or disabled. In that case we need to copy
	// the data for that stage from its input buffers to its output buffers via a simple blit operation.
	PsychPipelineAddBuiltinFunctionToHook(windowRecord, "IdentityBlitChain", "Builtin:IdentityBlit", TRUE, "");
	PsychPipelineEnableHook(windowRecord, "IdentityBlitChain");

	// Setup of GLSL stereo shaders for stereo modes that need some merging operations:
	// Quad-buffered stereo and mono mode don't need these...
	if (windowRecord->stereomode > kPsychOpenGLStereo) {
		// Merged stereo mode requested.
		glsl = 0;
		
		// Which mode?
		switch(windowRecord->stereomode) {
			// Anaglyph mode?
			case kPsychAnaglyphRGStereo:
			case kPsychAnaglyphGRStereo:
			case kPsychAnaglyphRBStereo:
			case kPsychAnaglyphBRStereo:
				// These share all code...
				
				// Create anaglyph shader and set proper defaults: These can be changed from the M-File if wanted.
				if (PsychPrefStateGet_Verbosity()>4) printf("PTB-INFO: Creating internal anaglyph stereo compositing shader...\n");
				glsl = PsychCreateGLSLProgram(anaglyphshadersrc, NULL, NULL);
				if (glsl) {
					// Bind it:
					glUseProgram(glsl);

					// Set channel to texture units assignments:
					glUniform1i(glGetUniformLocation(glsl, "Image1"), 0);
					glUniform1i(glGetUniformLocation(glsl, "Image2"), 1);
					
					// Set per-channel color gains: 0 masks the channel, >0 enables it. The values can be used
					// to compensate for differences in the color reproduction of different monitors to reduce
					// cross-talk / ghosting:
					
					// Left-eye channel (channel 1):
					rg = (windowRecord->stereomode==kPsychAnaglyphRGStereo || windowRecord->stereomode==kPsychAnaglyphRBStereo) ? 1.0 : 0.0;
					gg = (windowRecord->stereomode==kPsychAnaglyphGRStereo) ? 1.0 : 0.0;
					bg = (windowRecord->stereomode==kPsychAnaglyphBRStereo) ? 1.0 : 0.0;
					glUniform3f(glGetUniformLocation(glsl, "Gains1"), rg, gg, bg);

					// Right-eye channel (channel 2):
					rg = (windowRecord->stereomode==kPsychAnaglyphGRStereo || windowRecord->stereomode==kPsychAnaglyphBRStereo) ? 1.0 : 0.0;
					gg = (windowRecord->stereomode==kPsychAnaglyphRGStereo) ? 1.0 : 0.0;
					bg = (windowRecord->stereomode==kPsychAnaglyphRBStereo) ? 1.0 : 0.0;
					glUniform3f(glGetUniformLocation(glsl, "Gains2"), rg, gg, bg);
					
					// Define default weights for RGB -> Luminance conversion: We default to the standardized NTSC color weights.
					glUniform3f(glGetUniformLocation(glsl, "ColorToGrayWeights"), 0.3, 0.59, 0.11);
					// Define background bias color to add: Normally zero for standard anaglyph:
					glUniform3f(glGetUniformLocation(glsl, "ChannelBias"), 0.0, 0.0, 0.0);

					// Unbind it, its ready!
					glUseProgram(0);

					if (glsl) {
						// Add shader to processing chain:
						PsychPipelineAddShaderToHook(windowRecord, "StereoCompositingBlit", "StereoCompositingShaderAnaglyph", TRUE, glsl, "", 0) ;
						
						// Enable stereo compositor:
						PsychPipelineEnableHook(windowRecord, "StereoCompositingBlit");		
					}
				}
				else {
					PsychErrorExitMsg(PsychError_user, "PTB-ERROR: Failed to create anaglyph stereo processing shader -- Anaglyph stereo won't work!\n");
				}
			break;
			
			case kPsychFreeFusionStereo:
			case kPsychFreeCrossFusionStereo:
				if (PsychPrefStateGet_Verbosity()>4) printf("PTB-INFO: Creating internal dualview stereo compositing shader...\n");
				
				glsl = PsychCreateGLSLProgram(passthroughshadersrc, NULL, NULL);
				if (glsl) {
					// Bind it:
					glUseProgram(glsl);
					// Set channel to texture units assignments:
					glUniform1i(glGetUniformLocation(glsl, "Image1"), (windowRecord->stereomode == kPsychFreeFusionStereo) ? 0 : 1);
					glUseProgram(0);
					
					// Add shader to processing chain:
					sprintf(blittercfg, "Builtin:IdentityBlit:Offset:%i:%i", 0, 0);
					PsychPipelineAddShaderToHook(windowRecord, "StereoCompositingBlit", "StereoCompositingShaderDualViewLeft", TRUE, glsl, blittercfg, 0);
				}
				else {
					PsychErrorExitMsg(PsychError_user, "PTB-ERROR: Failed to create left channel dualview stereo processing shader -- Dualview stereo won't work!\n");
				}
				
				glsl = PsychCreateGLSLProgram(passthroughshadersrc, NULL, NULL);
				if (glsl) {
					// Bind it:
					glUseProgram(glsl);
					// Set channel to texture units assignments:
					glUniform1i(glGetUniformLocation(glsl, "Image1"),  (windowRecord->stereomode == kPsychFreeFusionStereo) ? 1 : 0);
					glUseProgram(0);
					
					// Add shader to processing chain:
					sprintf(blittercfg, "Builtin:IdentityBlit:Offset:%i:%i", (int) PsychGetWidthFromRect(windowRecord->rect)/2, 0);
					PsychPipelineAddShaderToHook(windowRecord, "StereoCompositingBlit", "StereoCompositingShaderDualViewRight", TRUE, glsl, blittercfg, 0);
				}
				else {
					PsychErrorExitMsg(PsychError_user, "PTB-ERROR: Failed to create right channel dualview stereo processing shader -- Dualview stereo won't work!\n");
				}

				// Enable stereo compositor:
				PsychPipelineEnableHook(windowRecord, "StereoCompositingBlit");		
			break;
			
			case kPsychCompressedTLBRStereo:
			case kPsychCompressedTRBLStereo:
				if (PsychPrefStateGet_Verbosity()>4) printf("PTB-INFO: Creating internal vertical split stereo compositing shader...\n");
				
				glsl = PsychCreateGLSLProgram(passthroughshadersrc, NULL, NULL);
				if (glsl) {
					// Bind it:
					glUseProgram(glsl);
					// Set channel to texture units assignments:
					glUniform1i(glGetUniformLocation(glsl, "Image1"), (windowRecord->stereomode == kPsychCompressedTLBRStereo) ? 0 : 1);
					glUseProgram(0);
					
					// Add shader to processing chain:
					sprintf(blittercfg, "Builtin:IdentityBlit:Offset:%i:%i:Scaling:%f:%f", 0, 0, 1.0, 0.5);
					PsychPipelineAddShaderToHook(windowRecord, "StereoCompositingBlit", "StereoCompositingShaderCompressedTop", TRUE, glsl, blittercfg, 0);
				}
				else {
					PsychErrorExitMsg(PsychError_user, "PTB-ERROR: Failed to create left channel dualview stereo processing shader -- Dualview stereo won't work!\n");
				}
				
				glsl = PsychCreateGLSLProgram(passthroughshadersrc, NULL, NULL);
				if (glsl) {
					// Bind it:
					glUseProgram(glsl);
					// Set channel to texture units assignments:
					glUniform1i(glGetUniformLocation(glsl, "Image1"),  (windowRecord->stereomode == kPsychCompressedTLBRStereo) ? 1 : 0);
					glUseProgram(0);
					
					// Add shader to processing chain:
					sprintf(blittercfg, "Builtin:IdentityBlit:Offset:%i:%i:Scaling:%f:%f", 0, (int) PsychGetHeightFromRect(windowRecord->rect)/2, 1.0, 0.5);
					PsychPipelineAddShaderToHook(windowRecord, "StereoCompositingBlit", "StereoCompositingShaderCompressedBottom", TRUE, glsl, blittercfg, 0);
				}
				else {
					PsychErrorExitMsg(PsychError_user, "PTB-ERROR: Failed to create right channel dualview stereo processing shader -- Dualview stereo won't work!\n");
				}

				// Enable stereo compositor:
				PsychPipelineEnableHook(windowRecord, "StereoCompositingBlit");		
			break;

			case kPsychOpenGLStereo:
				// Nothing to do for now: Setup of blue-line syncing is done in SCREENOpenWindow.c, because it also
				// applies to non-imaging mode...
			break;
			
			default:
				PsychErrorExitMsg(PsychError_internal, "Unknown stereo mode encountered! FIX SCREENOpenWindow.c to catch this at the appropriate place!\n");
		}
		
	}
	

	//PsychPipelineAddBuiltinFunctionToHook(windowRecord, "FinalOutputFormattingBlit", "Builtin:IdentityBlit", TRUE, "");
	//PsychPipelineEnableHook(windowRecord, "FinalOutputFormattingBlit");

	//PsychPipelineAddBuiltinFunctionToHook(windowRecord, "StereoLeftCompositingBlit", "Builtin:IdentityBlit", TRUE, "");
	//PsychPipelineAddBuiltinFunctionToHook(windowRecord, "StereoLeftCompositingBlit", "Builtin:FlipFBOs", TRUE, "");
	//PsychPipelineAddBuiltinFunctionToHook(windowRecord, "StereoLeftCompositingBlit", "Builtin:IdentityBlit", TRUE, "");
	//PsychPipelineEnableHook(windowRecord, "StereoLeftCompositingBlit");

	// Perform a full reset of current drawing target. This is a warm-start of PTB's drawing
	// engine, so the next drawing command will trigger binding the proper FBO of our pipeline.
	// Before this point (==OpenWindow time), all drawing was directly directed to the system
	// framebuffer - important for all the timing tests and calibrations to work correctly.
	PsychSetDrawingTarget(NULL);

	// Well done.
	return;
}

/* PsychCreateGLSLProgram()
 *  Try to create GLSL shader from source strings and return handle to new shader.
 *  Returns the shader handle if it worked, 0 otherwise.
 *
 *  fragmentsrc - Source string for fragment shader. NULL if none needed.
 *  vertexsrc   - Source string for vertex shader. NULL if none needed.
 *  primitivesrc - Source string for primitive shader. NULL if none needed.
 *
 */
GLuint PsychCreateGLSLProgram(const char* fragmentsrc, const char* vertexsrc, const char* primitivesrc)
{
	GLuint glsl = 0;
	GLuint shader;
	GLint status;
	char errtxt[10000];
	
	// Reset error state:
	while (glGetError());
	
	// Supported at all on this hardware?
	if (!glewIsSupported("GL_ARB_shader_objects") || !glewIsSupported("GL_ARB_shading_language_100")) {
		printf("PTB-ERROR: Your graphics hardware does not support GLSL fragment shaders! Use of imaging pipeline with current settings impossible!\n");
		return(0);
	}
	
	// Create GLSL program object:
	glsl = glCreateProgram();
	
	// Fragment shader wanted?
	if (fragmentsrc) {
		if (PsychPrefStateGet_Verbosity()>4)  printf("PTB-INFO: Creating the following fragment shader, GLSL source code follows:\n\n%s\n\n", fragmentsrc);

		// Supported on this hardware?
		if (!glewIsSupported("GL_ARB_fragment_shader")) {
			printf("PTB-ERROR: Your graphics hardware does not support GLSL fragment shaders! Use of imaging pipeline with current settings impossible!\n");
			return(0);
		}
		
		// Create shader object:
		shader = glCreateShader(GL_FRAGMENT_SHADER);
		// Feed it with GLSL source code:
		glShaderSource(shader, 1, (const char**) &fragmentsrc, NULL);
		
		// Compile shader:
		glCompileShader(shader);
		
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			printf("PTB-ERROR: Shader compilation for builtin fragment shader failed:\n");
			glGetShaderInfoLog(shader, 9999, NULL, (GLchar*) &errtxt);
			printf("%s\n\n", errtxt);
			glDeleteShader(shader);
			glDeleteProgram(glsl);
			// Failed!
			while (glGetError());

			return(0);
		}
		
		// Attach it to program object:
		glAttachShader(glsl, shader);
	}

	// Vertex shader wanted?
	if (vertexsrc) {
		if (PsychPrefStateGet_Verbosity()>4)  printf("PTB-INFO: Creating the following vertex shader, GLSL source code follows:\n\n%s\n\n", vertexsrc);

		// Supported on this hardware?
		if (!glewIsSupported("GL_ARB_vertex_shader")) {
			printf("PTB-ERROR: Your graphics hardware does not support GLSL vertex shaders! Use of imaging pipeline with current settings impossible!\n");
			return(0);
		}

		// Create shader object:
		shader = glCreateShader(GL_VERTEX_SHADER);
		
		// Feed it with GLSL source code:
		glShaderSource(shader, 1, (const char**) &vertexsrc, NULL);
		
		// Compile shader:
		glCompileShader(shader);
		
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			printf("PTB-ERROR: Shader compilation for builtin vertex shader failed:\n");
			glGetShaderInfoLog(shader, 9999, NULL, (GLchar*) &errtxt);
			printf("%s\n\n", errtxt);
			glDeleteShader(shader);
			glDeleteProgram(glsl);
			// Failed!
			while (glGetError());
			return(0);
		}
		
		// Attach it to program object:
		glAttachShader(glsl, shader);
	}

	// Link into final program object:
	glLinkProgram(glsl);

	// Check link status:
	glGetProgramiv(glsl, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		printf("PTB-ERROR: Shader link operation for builtin glsl program failed:\n");
		glGetProgramInfoLog(glsl, 9999, NULL, (GLchar*) &errtxt);
		printf("Error output follows:\n\n%s\n\n", errtxt);
		glDeleteProgram(glsl);
		// Failed!
		while (glGetError());

		return(0);
	}
	
	while (glGetError());

	// Return new GLSL program object handle:
	return(glsl);
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
		
		(*fbo)->width = width;
		(*fbo)->height = height;
		
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
    glTexParameterf(GL_TEXTURE_RECTANGLE_EXT,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_RECTANGLE_EXT,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	
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
		printf("PTB-ERROR[Imaging pipeline]: Failed to setup color buffer attachment of internal FBO when trying to prepare drawing into a texture or window.\n");
		if (fborc==GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT) {
			printf("PTB-ERROR: Your graphics hardware does not support the provided or requested texture- or offscreen window format for drawing into it.\n");
			printf("PTB-ERROR: Most graphics cards do not support drawing into textures or offscreen windows which are not true-color, i.e. 1 layer pure\n");
			printf("PTB-ERROR: luminance or 2 layer luminance+alpha textures or offscreen windows won't work. Choose a 3-layer RGB or 4-layer RGBA texture\n");
			printf("PTB-ERROR: or offscreen window and retry.\n");
		}
		else if (fborc==GL_FRAMEBUFFER_UNSUPPORTED_EXT) {
			printf("PTB-ERROR: Your graphics hardware does not support the provided or requested texture- or offscreen window format for drawing into it.\n");
			printf("PTB-ERROR: Could be that the specific color depth of the texture or offscreen window is not supported for drawing on your hardware:\n");
			printf("PTB-ERROR: 8 bits per color component are supported on nearly all hardware, 16 bpc or 32 bpc floating point formats only on recent\n");
			printf("PTB-ERROR: hardware. 16 bpc fixed precision is not supported on any NVidia hardware, and on many systems only under restricted conditions.\n");
			printf("PTB-ERROR: Retry with the lowest acceptable (for your study) size and depth of the onscreen window or offscreen window.\n");
		}
		else {
			printf("PTB-ERROR: Exact reason for failure is unknown, most likely a Psychtoolbox bug, GL-driver bug or unintented use. glCheckFramebufferStatus() returns code %i\n", fborc);
		}
		return(FALSE);
	}
	
	// Do we need additional buffers for 3D rendering?
	if (needzbuffer) {
		// Yes. Try to setup and attach them:
		if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: Trying to attach depth+stencil attachments to FBO...\n"); 
		if (!glewIsSupported("GL_ARB_depth_texture")) {
			printf("PTB-ERROR: Failed to setup internal framebuffer object for imaging pipeline! Your graphics hardware does not support\n");
			printf("PTB-ERROR: the required GL_ARB_depth_texture extension. You'll need at least a NVidia GeforceFX 5200, ATI Radeon 9600\n");
			printf("PTB-ERROR: or Intel GMA-950 with recent graphics-drivers for this to work.\n");
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
					printf("PTB-WARNING: all gfx-cards starting with GeForceFX 5200 on Windows and Linux and all cards on Intel-Macs except the Intel GMA cards should work, whereas\n");
					printf("PTB-WARNING: none of the PowerPC hardware is supported as of OS-X 10.4.9.\n"); 
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
		printf("PTB-DEBUG: FBO has %i bits precision per color component in ", bpc);
		if (glewIsSupported("GL_ARB_color_buffer_float")) {
			glGetBooleanv(GL_RGBA_FLOAT_MODE_ARB, &isFloatBuffer);
			if (isFloatBuffer) {
				printf("floating point format ");
			}
			else {
				printf("fixed point format ");
			}
		}
		else if (glewIsSupported("GL_APPLE_float_pixels")) { 
			glGetBooleanv(GL_COLOR_FLOAT_APPLE, &isFloatBuffer);
			if (isFloatBuffer) {
				printf("floating point format ");
			}
			else {
				printf("fixed point foramt ");
			}
		}
		else {
			isFloatBuffer = FALSE;
			printf("unknown format ");
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

/* PsychCreateShadowFBOForTexture()
 * Check if provided PTB texture already has a PsychFBO attached. Do nothing if so.
 * If a FBO is missing, create one.
 *
 * If asRendertarget is FALSE, we only create the data structure, not a real OpenGL FBO,
 * so the texture is only suitable as image source for image processing.
 *
 * If asRendertraget is TRUE, we create a full blown FBO, so the texture can be used as
 * rendertarget.
 *
 */
void PsychCreateShadowFBOForTexture(PsychWindowRecordType *textureRecord, Boolean asRendertarget, int forImagingmode)
{
	GLenum fboInternalFormat;
	
	// Do we already have a framebuffer object for this texture? All textures start off without one,
	// because most textures are just used for drawing them, not drawing *into* them. Therefore we
	// only create a full blown FBO on demand here.
	if (textureRecord->drawBufferFBO[0]==-1) {
		// No. This texture is used the first time as a drawing target.
		// Need to create a framebuffer object for it first.
		
		if (textureRecord->textureNumber > 0) {
			// Allocate and assign FBO object info structure PsychFBO:
			PsychCreateFBO(&(textureRecord->fboTable[0]), (GLenum) 0, (PsychPrefStateGet_3DGfx() > 0) ? TRUE : FALSE, PsychGetWidthFromRect(textureRecord->rect), PsychGetHeightFromRect(textureRecord->rect));
			
			// Manually set up the color attachment texture id to our texture id:
			textureRecord->fboTable[0]->coltexid = textureRecord->textureNumber;
		}
		else {
			// No texture yet. Create suitable one for given imagingmode:

			// Start off with standard 8 bpc fixed point:
			fboInternalFormat = GL_RGBA8;
			
			// Need 16 bpc fixed point precision?
			if (forImagingmode & kPsychNeed16BPCFixed) fboInternalFormat = GL_RGBA16;
			
			// Need 16 bpc floating point precision?
			if (forImagingmode & kPsychNeed16BPCFloat) fboInternalFormat = GL_RGBA_FLOAT16_APPLE;
			
			// Need 32 bpc floating point precision?
			if (forImagingmode & kPsychNeed32BPCFloat) fboInternalFormat = GL_RGBA_FLOAT32_APPLE;
			
			PsychCreateFBO(&(textureRecord->fboTable[0]), fboInternalFormat, (PsychPrefStateGet_3DGfx() > 0) ? TRUE : FALSE, PsychGetWidthFromRect(textureRecord->rect), PsychGetHeightFromRect(textureRecord->rect));
			
			// Manually set up the texture id from our color attachment texture id:
			textureRecord->textureNumber = textureRecord->fboTable[0]->coltexid;
		}
				
		// Worked. Set up remaining state:
		textureRecord->fboCount = 1;
		textureRecord->drawBufferFBO[0]=0;
	}
	
	// Does it need to be suitable as a rendertarget? If so, check if it is already, upgrade it if neccessary:
	if (asRendertarget && textureRecord->fboTable[0]->fboid==0) {
		// Initialize and setup real FBO object (optionally with z- and stencilbuffer) and attach the texture
		// as color attachment 0, aka main colorbuffer:				
		if (!PsychCreateFBO(&(textureRecord->fboTable[0]), (GLenum) 1, (PsychPrefStateGet_3DGfx() > 0) ? TRUE : FALSE, PsychGetWidthFromRect(textureRecord->rect), PsychGetHeightFromRect(textureRecord->rect))) {
			// Failed!
			PsychErrorExitMsg(PsychError_internal, "Preparation of drawing into an offscreen window or texture failed when trying to create associated framebuffer object!");
			
		}					
	}
	
	return;
}

/* PsychNormalizeTextureOrientation() - On demand texture reswapping.
 *
 * PTB supports multiple different ways of orienting and formatting textures,
 * optimized for different purposes. However, textures that one wants to draw
 * to as if they were Offscreen windows or textures to be fed into the image
 * processing pipeline (Screen('TransformTexture')) need to be in a standardized
 * upright, non-transposed format. This routine checks the orientation of a
 * texture and - if neccessary - transforms the texture from its current format
 * to the upright standard format. As a side effect, it also converts such textures
 * from Luminance or Luminance+Alpha formats into RGB or RGBA formats. This is
 * important, because only RGB(A) textures are suitable as FBO color buffer attachments.
 *
 */ 
void PsychNormalizeTextureOrientation(PsychWindowRecordType *sourceRecord)
{
	int tmpimagingmode;
	PsychFBO *fboptr;
	GLint fboInternalFormat;
	Boolean needzbuffer;
	int width, height;
	
	// The source texture sourceRecord could be in any of PTB's supported
	// internal texture orientations. It may be upright as an Offscreen window,
	// or flipped upside down as some textures from the video grabber or Quicktime,
	// or transposed, as textures from Matlab/Octave. However, handling all those
	// cases for image processing would be a debug and maintenance nightmare.
	// Therefore we check the format of the source texture and require it to be
	// a normal upright orientation. If this isn't the case, we perform a preprocessing
	// step to transform the texture into normalized orientation. We also perform a
	// preprocessing step on any CoreVideo texture from Quicktime. Although such a
	// texture may be properly oriented, it is of a non-renderable YUV color format, so
	// we need to recreate it in a RGB renderable format.
	if (sourceRecord->textureOrientation != 2 || sourceRecord->targetSpecific.QuickTimeGLTexture != NULL) {
		if (PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: In PsychNormalizeTextureOrientation(): Performing GPU renderswap for source gl-texture %i --> ", sourceRecord->textureNumber);
		
		// Soft-reset drawing engine:
		PsychSetDrawingTarget(NULL);

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

		// Need to query real size of underlying texture, not the logical size from sourceRecord->rect, otherwise we'd screw
		// up for padded textures (from Quicktime movie/vidcap) where the real texture is a bit bigger than its logical size.
		if (sourceRecord->textureOrientation > 1) {
			// Non-transposed textures, width and height are correct:
			glGetTexLevelParameteriv(PsychGetTextureTarget(sourceRecord), 0, GL_TEXTURE_WIDTH, &width);
			glGetTexLevelParameteriv(PsychGetTextureTarget(sourceRecord), 0, GL_TEXTURE_HEIGHT, &height);
		}
		else {
			// Transposed textures: Need to swap meaning of width and height:
			glGetTexLevelParameteriv(PsychGetTextureTarget(sourceRecord), 0, GL_TEXTURE_WIDTH, &height);
			glGetTexLevelParameteriv(PsychGetTextureTarget(sourceRecord), 0, GL_TEXTURE_HEIGHT, &width);
		}
		
		glBindTexture(PsychGetTextureTarget(sourceRecord), 0);
		
		// Renderable format? Pure luminance or luminance+alpha formats are not renderable on most hardware.
		
		// Upgrade 8 bit luminace to 8 bit RGBA:
		if (fboInternalFormat == GL_LUMINANCE8 || fboInternalFormat == GL_LUMINANCE8_ALPHA8|| sourceRecord->depth == 8) fboInternalFormat = GL_RGBA8;
		
		// Upgrade non-renderable floating point formats to their RGB or RGBA counterparts of matching precision:
		if (sourceRecord->nrchannels < 3 && fboInternalFormat != GL_RGBA8) {
			// Unsupported format for FBO rendertargets. Need to upgrade to something suitable...
			if (sourceRecord->textureexternalformat == GL_LUMINANCE) {
				// Upgrade luminance to RGB of matching precision:
				// printf("UPGRADING TO RGBFloat %i\n", (sourceRecord->textureinternalformat == GL_LUMINANCE_FLOAT16_APPLE) ? 0:1);
				fboInternalFormat = (sourceRecord->textureinternalformat == GL_LUMINANCE_FLOAT16_APPLE) ? GL_RGB_FLOAT16_APPLE : GL_RGB_FLOAT32_APPLE;
			}
			else {
				// Upgrade luminance+alpha to RGBA of matching precision:
				// printf("UPGRADING TO RGBAFloat %i\n", (sourceRecord->textureinternalformat == GL_LUMINANCE_ALPHA_FLOAT16_APPLE) ? 0:1);
				fboInternalFormat = (sourceRecord->textureinternalformat == GL_LUMINANCE_ALPHA_FLOAT16_APPLE) ? GL_RGBA_FLOAT16_APPLE : GL_RGBA_FLOAT32_APPLE;
			}
		}
		
		// Special case: Quicktime movie or video texture, created by CoreVideo in Apple specific YUV format.
		// This is a non-framebuffer renderable color format. Need to upgrade it to something safe:
		if (fboInternalFormat == GL_YCBCR_422_APPLE) fboInternalFormat = GL_RGBA8;
		
		// Now create proper FBO:
		if (!PsychCreateFBO(&(sourceRecord->fboTable[0]), (GLenum) fboInternalFormat, needzbuffer, width, height)) {
			PsychErrorExitMsg(PsychError_internal, "Failed to normalize texture orientation - Creation of framebuffer object failed!");
		}
		
		sourceRecord->drawBufferFBO[0] = 0;
		sourceRecord->fboCount = 1;
		
		tmpimagingmode = sourceRecord->imagingMode;
		sourceRecord->imagingMode = 1;

		// Set FBO of sourceRecord as rendertarget, including proper setup of render geometry:
		// We can't use PsychSetDrawingTarget() here, as we might get called by that function, i.e.
		// infinite recursion or other side effects if we tried to use it.
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, sourceRecord->fboTable[0]->fboid);
		PsychSetupView(sourceRecord);
		// Reset MODELVIEW matrix, after backing it up...
		glPushMatrix();
		glLoadIdentity();
		
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
		
		// Restore modelview matrix:
		glPopMatrix();
		
		sourceRecord->imagingMode = tmpimagingmode;
		PsychSetDrawingTarget(NULL);

		// At this point the color attachment of the sourceRecords FBO contains the properly oriented texture.
		// Delete the old texture, attach the FBO texture as new one:
		if (sourceRecord->targetSpecific.QuickTimeGLTexture != NULL) {
			// Special case: CoreVideo texture:
			PsychFreeMovieTexture(sourceRecord);
		}
		else {
			// Standard case:
			glDeleteTextures(1, &(sourceRecord->textureNumber));
		}
		
		// Assign new texture:
		sourceRecord->textureNumber = sourceRecord->fboTable[0]->coltexid;
		
		// Finally sourceRecord has the proper orientation:
		sourceRecord->textureOrientation = 2;
		
		// GPU renderswap finished.
		if (PsychPrefStateGet_Verbosity()>5) printf("%i.\n", sourceRecord->textureNumber);
	}

	return;
}

/* PsychShutdownImagingPipeline()
 * Shutdown imaging pipeline for a windowRecord and free all ressources associated with it.
 */
void PsychShutdownImagingPipeline(PsychWindowRecordType *windowRecord, Boolean openglpart)
{
	int i;
	PtrPsychHookFunction hookfunc, hookiter;
	PsychFBO* fboptr;
	
	// Do OpenGL specific cleanup:
	if (openglpart) {
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
	hookfunc->idString = (idString) ? strdup(idString) : strdup("");
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
	hookfunc->pString1 =  (blitterString) ? strdup(blitterString) : strdup("");
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
	hookfunc->pString1 =  (evalString) ? strdup(evalString) : strdup("");
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
	hookfunc->pString1 =  (configString) ? strdup(configString) : strdup("");
	return;
}

/* PsychPipelineQueryHookSlot
 * Query properties of a specific hook slot in a specific hook chain of a specific window:
 * windowRecord - Query for this window/texture.
 * hookString   - Query this named chain.
 * idString     - This string defines the specific slot to query. Can contain an integral number, then the associated slot is
 *				  queried, or a idString (as assigned during creation), then a slot with that name is queried. Partial name
 *				  matches are also accepted to search for substrings...
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
	while(hookfunc && ((targetidx>-1 && idx<targetidx) || (targetidx==-1 && strstr(hookfunc->idString, *idString)==NULL))) {
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

boolean PsychIsHookChainOperational(PsychWindowRecordType *windowRecord, int hookid)
{
	// Child protection:
	if (hookid<0 || hookid>=MAX_SCREEN_HOOKS) PsychErrorExitMsg(PsychError_internal, "In PsychIsHookChainOperational: Was asked to check unknown (non-existent) hook chain with invalid id!");

	// Hook chain enabled for processing and contains at least one hook slot?
	if ((!windowRecord->HookChainEnabled[hookid]) || (windowRecord->HookChain[hookid] == NULL)) {
		// Chain is empty or disabled.
		return(FALSE);
	}
	
	// Chain operational:
	return(TRUE);
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
		if (hookfunc->hookfunctype == kPsychBuiltinFunc && strcmp(hookfunc->idString, "Builtin:FlipFBOs")==0) pendingFBOpingpongs++;
		// Process next hookfunc slot in chain, if any:
		hookfunc = hookfunc->next;
	}

	if (gfxprocessing) {
		// Prepare gfx-processing:

		// If this is a multi-pass chain we'll need a bounce buffer FBO:
		if ((pendingFBOpingpongs > 0 && bouncefbo == NULL) || (pendingFBOpingpongs > 1 && ((*dstfbo)->fboid == 0))) {
			printf("PTB-ERROR: Hook processing chain '%s' is a multi-pass processing chain with %i passes,\n", PsychHookPointNames[hookId], pendingFBOpingpongs + 1);
			printf("PTB-ERROR: but imaging pipeline is not configured for multi-pass processing! You need to supply the additional flag\n");
			printf("PTB-ERROR: %s as imagingmode to Screen('OpenWindow') to tell PTB about these requirements and then restart.\n\n",
					(pendingFBOpingpongs > 1) ? "kPsychNeedMultiPass" : "kPsychNeedDualPass");
			// Ok, abort...
			PsychErrorExitMsg(PsychError_user, "Insufficient pipeline configuration for processing. Adapt the 'imagingmode' flag according to my tips!");
		}
		
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
				
		// Enable associated GL context:
		PsychSetGLContext(windowRecord);
		
		// Save current FBO bindings for later restore:
		if (glBindFramebufferEXT) {
			glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &restorefboid);
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
			printf("Hookchain '%s' : Slot %i: Id='%s' : ", PsychHookPointNames[hookId], i, hookfunc->idString);
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
		if (hookfunc->hookfunctype == kPsychBuiltinFunc && strcmp(hookfunc->idString, "Builtin:FlipFBOs")==0) {
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
					printf("PTB-ERROR: Failed in processing of Hookchain '%s' : Slot %i: Id='%s'  --> Aborting chain processing. Set verbosity to 5 for extended debug output.\n", PsychHookPointNames[hookId], i, hookfunc->idString);
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
	boolean dispatched = FALSE;
	
	// Dispatch by hook function type:
	switch(hookfunc->hookfunctype) {
		case kPsychShaderFunc:
			// Call a GLSL shader to do some image processing: We just execute the blitter, the shader gets assigned inside
			// this function.
			if (!PsychPipelineExecuteBlitter(windowRecord, hookfunc, hookUserData, hookBlitterFunction, srcIsReadonly, allowFBOSwizzle, srcfbo1, srcfbo2, dstfbo, bouncefbo)) {
				// Blitter failed!
				return(FALSE);
			}
			dispatched=TRUE;
		break;
			
		case kPsychCFunc:
			// Call a C callback function via the given memory function pointer:
			printf("TODO: EXECUTE -- C-Callback       : void*= %p\n", hookfunc->cprocfunc);
			dispatched=TRUE;
		break;
			
		case kPsychMFunc:
			// Call the eval() function of our scripting runtime environment to evaluate
			// function string pString1. Currently supported are Matlab & Octave, so this
			// can be the call string of an arbitrary Matlab/Octave builtin or M-Function.
			// Care has to be taken that the called functions do not invoke any Screen
			// subfunctions! Screen is not reentrant, so that would likely screw seriously!

			// TODO: Substitute specific placeholders in pString1 by parameters from us.
			PsychRuntimeEvaluateString(hookfunc->pString1);
			dispatched=TRUE;
		break;
			
		case kPsychBuiltinFunc:
			// Dispatch to a builtin function:
			if (strcmp(hookfunc->idString, "Builtin:FlipFBOs")==0) { dispatched=TRUE; } // No op here. Done in upper layer...
			if (strstr(hookfunc->idString, "Builtin:IdentityBlit")) {
				// Perform the most simple blit operation: A simple one-to-one copy of input FBO to output FBO:
				if (!PsychPipelineExecuteBlitter(windowRecord, hookfunc, NULL, NULL, TRUE, FALSE, srcfbo1, NULL, dstfbo, NULL)) {
					// Blitter failed!
					return(FALSE);
				}
				dispatched=TRUE;
			}
			if (strcmp(hookfunc->idString, "Builtin:RenderClutBits++")==0) {
				// Compute the T-Lock encoded CLUT for Cambridge Research Bits++ system in Bits++ mode. The CLUT
				// is set via the standard Screen('LoadNormalizedGammaTable', ..., loadOnNextFlip) call by setting
				// loadOnNextFlip to a value of 2.
				if (!PsychPipelineBuiltinRenderClutBitsPlusPlus(windowRecord, hookfunc)) {
					// Operation failed!
					return(FALSE);
				}
				dispatched=TRUE;
			}
			
			if (strcmp(hookfunc->idString, "Builtin:RenderStereoSyncLine")==0) {
				// Draw a blue-line-sync sync line at the bottom of the current framebuffer. This is needed
				// to drive stereo shutter glasses with blueline-sync in quad-buffered frame-sequential stereo
				// mode.
				if (!PsychPipelineBuiltinRenderStereoSyncLine(windowRecord, hookfunc)) {
					// Operation failed!
					return(FALSE);
				}
				dispatched=TRUE;
			}
		break;
			
		default:
			PsychErrorExitMsg(PsychError_internal, "In PsychPipelineExecuteHookSlot: Was asked to execute unknown (non-existent) hook function type!");
	}
	
	if (!dispatched) {
		if (PsychPrefStateGet_Verbosity()>0) printf("PTB-ERROR: Failed to dispatch hook slot - Unknown command or failure in command execution.\n");
		return(FALSE);
	}
	
	return(TRUE);
}

void PsychPipelineSetupRenderFlow(PsychFBO* srcfbo1, PsychFBO* srcfbo2, PsychFBO* dstfbo)
{
	static int ow=0;
	static int oh=0;
	int w, h;

	// Select rendertarget:
	if (glBindFramebufferEXT) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, (dstfbo) ? dstfbo->fboid : 0);

	// Assign color texture of srcfbo2, if any,  to texture unit 1:
	glActiveTextureARB(GL_TEXTURE1_ARB);
	if (srcfbo2) {
		// srcfbo2 is valid: Assign its color buffer texture:
		if (PsychPrefStateGet_Verbosity()>4) printf("TexUnit 1 reading from texid -- %i\n", srcfbo2->coltexid);
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, srcfbo2->coltexid);
		// Set texture application mode to replace:
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glEnable(GL_TEXTURE_RECTANGLE_EXT);
		glDisable(GL_TEXTURE_2D);
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
		if (PsychPrefStateGet_Verbosity()>4) printf("TexUnit 0 reading from texid -- %i\n", srcfbo1->coltexid);
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, srcfbo1->coltexid);

		// Set texture application mode to replace:
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		
		glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		glEnable(GL_TEXTURE_RECTANGLE_EXT);
		glDisable(GL_TEXTURE_2D);
	}
	else {
		// srcfbo1 doesn't exist: Unbind and deactivate 1st unit:
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);
		glDisable(GL_TEXTURE_RECTANGLE_EXT);
	}

	
	if (dstfbo) {
		// Setup viewport, scissor rectangle and projection matrix for orthonormal rendering into the
		// target FBO or system framebuffer:
		w = (int) dstfbo->width;
		h = (int) dstfbo->height;
		if (PsychPrefStateGet_Verbosity()>4) printf("Blitting to Targettex = %i , w x h = %i %i\n", dstfbo->coltexid, w, h);
		
		// Settings changed? We skip if not - state changes are expensive...
		if (w!=ow || h!=oh) {
			ow=w;
			oh=h;
			
			// Setup viewport and scissor for full FBO area:
			glViewport(0, 0, w, h);
			glScissor(0, 0, w, h);
			
			// Setup projection matrix for a proper orthonormal projection for this framebuffer:
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(0, w, h, 0);

			// Switch back to modelview matrix, but leave it unaltered:
			glMatrixMode(GL_MODELVIEW);
		}
	}
	else {
		// Reset our cached settings:
		ow=0;
		oh=0;
	}

	return;
}

boolean PsychPipelineExecuteBlitter(PsychWindowRecordType *windowRecord, PsychHookFunction* hookfunc, void* hookUserData, void* hookBlitterFunction, boolean srcIsReadonly, boolean allowFBOSwizzle, PsychFBO** srcfbo1, PsychFBO** srcfbo2, PsychFBO** dstfbo, PsychFBO** bouncefbo)
{
	boolean rc = TRUE;
	PsychBlitterFunc blitterfnc = NULL;
	GLenum glerr;
	char*  pstrpos = NULL;
	int texunit, texid;
	
	// Select proper blitter function:
	
	// Any special override blitter defined in parameter string?
	if (strstr(hookfunc->pString1, "Blitter:")) {
		// Yes. Which one?
		hookBlitterFunction = NULL;
		
		// Standard blitter? This one does a one-to-one copy without special geometric transformations.
		if (strstr(hookfunc->pString1, "Blitter:IdentityBlit")) blitterfnc = &PsychBlitterIdentity; // Assign our standard one-to-one blitter.
		
		// Blitter assigned?
		if (blitterfnc == NULL) {
			if (PsychPrefStateGet_Verbosity()>0) printf("PTB-ERROR: Invalid (unknown) blitter specified in blitter string. Blit aborted.\n");
			return(FALSE);
		}
	}
	
	// Master blitter function set?
	if (hookBlitterFunction == NULL) {
		// No blitter set up to now. Assign the default blitter:
		blitterfnc = &PsychBlitterIdentity; // Assign our standard one-to-one blitter.
	} else {
		// Override blitter defined: Assign it.
		blitterfnc = (PsychBlitterFunc) hookBlitterFunction;
	}
	
	// TODO: Common setup code for texturing, filtering, alpha blending, z-test and such...
	
	// Setup code for 1D textures:
	pstrpos = hookfunc->pString1;
	while (pstrpos=strstr(pstrpos, "TEXTURE1D")) {
		if (2==sscanf(pstrpos, "TEXTURE1D(%i)=%i", &texunit, &texid)) {
			glActiveTextureARB(GL_TEXTURE0_ARB + texunit);
			glEnable(GL_TEXTURE_1D);
			glBindTexture(GL_TEXTURE_1D, texid);
			if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: Binding gltexid %i to GL_TEXTURE_1D target of texunit %i\n", texid, texunit);
		}
		pstrpos++;
	}

	// Setup code for 2D textures:
	pstrpos = hookfunc->pString1;
	while (pstrpos=strstr(pstrpos, "TEXTURE2D")) {
		if (2==sscanf(pstrpos, "TEXTURE2D(%i)=%i", &texunit, &texid)) {
			glActiveTextureARB(GL_TEXTURE0_ARB + texunit);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texid);
			if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: Binding gltexid %i to GL_TEXTURE_2D target of texunit %i\n", texid, texunit);
		}
		pstrpos++;
	}

	// Setup code for 2D rectangle textures:
	pstrpos = hookfunc->pString1;
	while (pstrpos=strstr(pstrpos, "TEXTURERECT2D")) {
		if (2==sscanf(pstrpos, "TEXTURERECT2D(%i)=%i", &texunit, &texid)) {
			glActiveTextureARB(GL_TEXTURE0_ARB + texunit);
			glEnable(GL_TEXTURE_RECTANGLE_EXT);
			glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texid);
			if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: Binding gltexid %i to GL_TEXTURE_RECTANGLE_EXT target of texunit %i\n", texid, texunit);
		}
		pstrpos++;
	}

	// Setup code for 3D textures:
	pstrpos = hookfunc->pString1;
	while (pstrpos=strstr(pstrpos, "TEXTURE3D")) {
		if (2==sscanf(pstrpos, "TEXTURE3D(%i)=%i", &texunit, &texid)) {
			glActiveTextureARB(GL_TEXTURE0_ARB + texunit);
			glEnable(GL_TEXTURE_3D);
			glBindTexture(GL_TEXTURE_3D, texid);
			if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: Binding gltexid %i to GL_TEXTURE_3D target of texunit %i\n", texid, texunit);
		}
		pstrpos++;
	}

	glActiveTextureARB(GL_TEXTURE0_ARB);
	
	// Need a shader for this blit op?
	if (hookfunc->shaderid) {
		// Setup shader, if any:
		if (!glUseProgram){
			if (PsychPrefStateGet_Verbosity()>0) printf("PTB-ERROR: Blitter invocation failed: Blitter needs to attach GLSL shaders, but shaders are not supported on your hardware!\n");
			rc = FALSE;
		} else {
			// Attach shader:
			while(glGetError());
			glUseProgram(hookfunc->shaderid);
			if ((glerr = glGetError())!=GL_NO_ERROR) {
				if (PsychPrefStateGet_Verbosity()>0) printf("PTB-ERROR: Blitter invocation failed: glUseProgram(%i) failed with error: %s\n", hookfunc->shaderid, gluErrorString(glerr));
				rc = FALSE;
			}
			
			#if PSYCH_SYSTEM == PSYCH_OSX
			// On OS-X we can query the OS if the bound shader is running on the GPU or if it is running in emulation mode on the CPU.
			// This is an expensive operation - it triggers OpenGL internal state revalidation. Only use for debugging and testing!
			if (PsychPrefStateGet_Verbosity()>5) {
				long vsgpu=0, fsgpu=0;
				CGLGetParameter(CGLGetCurrentContext(), kCGLCPGPUVertexProcessing, &vsgpu);
				CGLGetParameter(CGLGetCurrentContext(), kCGLCPGPUFragmentProcessing, &fsgpu);
				printf("PTB-DEBUG: Imaging pipeline GPU shading state: Vertex processing on %s : Fragment processing on %s.\n", (vsgpu) ? "GPU" : "CPU!!", (fsgpu) ? "GPU" : "CPU!!");
			}
			#endif
			
		}
	}
	
	// Execute blitter function:
	rc = (rc && blitterfnc(windowRecord, hookfunc, hookUserData, srcIsReadonly, allowFBOSwizzle, srcfbo1, srcfbo2, dstfbo, bouncefbo));
	if (!rc) {
		if (PsychPrefStateGet_Verbosity()>0) printf("PTB-ERROR: Blitter invocation failed: OpenGL error state is: %s\n", gluErrorString(glGetError()));
		while(glGetError());
	}
	
	// TODO: Common teardown code for texturing, filtering and such...

	// Teardown code for 1D textures:
	pstrpos = hookfunc->pString1;
	while (pstrpos=strstr(pstrpos, "TEXTURE1D")) {
		if (2==sscanf(pstrpos, "(%i)=%i", &texunit, &texid)) {
			glActiveTextureARB(GL_TEXTURE0_ARB + texunit);
			glBindTexture(GL_TEXTURE_1D, 0);
			glDisable(GL_TEXTURE_1D);
		}
		pstrpos++;
	}

	// Teardown code for 2D textures:
	pstrpos = hookfunc->pString1;
	while (pstrpos=strstr(pstrpos, "TEXTURE2D")) {
		if (2==sscanf(pstrpos, "(%i)=%i", &texunit, &texid)) {
			glActiveTextureARB(GL_TEXTURE0_ARB + texunit);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}
		pstrpos++;
	}

	// Teardown code for 2D rectangle textures:
	pstrpos = hookfunc->pString1;
	while (pstrpos=strstr(pstrpos, "TEXTURERECT2D")) {
		if (2==sscanf(pstrpos, "(%i)=%i", &texunit, &texid)) {
			glActiveTextureARB(GL_TEXTURE0_ARB + texunit);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}
		pstrpos++;
	}

	// Teardown code for 3D textures:
	pstrpos = hookfunc->pString1;
	while (pstrpos=strstr(pstrpos, "TEXTURE3D")) {
		if (2==sscanf(pstrpos, "(%i)=%i", &texunit, &texid)) {
			glActiveTextureARB(GL_TEXTURE0_ARB + texunit);
			glBindTexture(GL_TEXTURE_3D, 0);
			glDisable(GL_TEXTURE_3D);
		}
		pstrpos++;
	}

	glActiveTextureARB(GL_TEXTURE0_ARB);

	// Reset shader assignment, if any:
	if ((hookfunc->shaderid) && glUseProgram) glUseProgram(0);

	// Return result code:
	return(rc);
}

/* PsychBlitterIdentity()  -- Default blitter.
 *
 * Identity blitter: Blits from srcfbo1 color attachment to dstfbo without geometric transformations or other extras.
 * This is the most common one for one-to-one copies or simple shader image processing. It gets automatically used
 * when no special (non-default) blitter is requested by core code or users blitter parameter string:
 */
boolean PsychBlitterIdentity(PsychWindowRecordType *windowRecord, PsychHookFunction* hookfunc, void* hookUserData, boolean srcIsReadonly, boolean allowFBOSwizzle, PsychFBO** srcfbo1, PsychFBO** srcfbo2, PsychFBO** dstfbo, PsychFBO** bouncefbo)
{
	int w, h, x, y;
	float sx, sy;
	char* strp;
	
	// Child protection:
	if (!(srcfbo1 && (*srcfbo1))) {
		PsychErrorExitMsg(PsychError_internal, "In PsychBlitterIdentity(): srcfbo1 is a NULL - Pointer!!!");
	}	

	// Query dimensions of viewport:
	w = (*srcfbo1)->width;
	h = (*srcfbo1)->height;

	// Check for offset parameter in the blitterString: An integral (x,y)
	// offset for the destination of the blit. This allows to blit the srcfbo1, without
	// scaling or filtering it, to a different start location than (0,0):
	x=y=0;
	if (strp=strstr(hookfunc->pString1, "Offset:")) {
		// Parse and assign offset:
		if (sscanf(strp, "Offset:%i:%i", &x, &y)!=2) {
			PsychErrorExitMsg(PsychError_internal, "In PsychBlitterIdentity(): Offset: blit string parameter is invalid! Parse error...\n");
		}
	}

	// Check for scaling parameter:
	sx = sy = 1.0;
	if (strp=strstr(hookfunc->pString1, "Scaling:")) {
		// Parse and assign offset:
		if (sscanf(strp, "Scaling:%f:%f", &sx, &sy)!=2) {
			PsychErrorExitMsg(PsychError_internal, "In PsychBlitterIdentity(): Scaling: blit string parameter is invalid! Parse error...\n");
		}
	}

	if (x!=0 || y!=0 || sx!=1.0 || sy!=1.0) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		
		// Apply global (x,y) offset:
		glTranslatef(x, y, 0);
		
		// Apply scaling:
		glScalef(sx, sy, 1);
	}
	
	// Do the blit, using a rectangular quad:
	glBegin(GL_QUADS);
	
	// Note the swapped y-coord for textures wrt. y-coord of vertex position!
	// Texture coordinate system has origin at bottom-left, y-axis pointing upward,
	// but PTB has framebuffer coordinate system with origin at top-left, with
	// y-axis pointing downward! Normally OpenGL would have origin always bottom-left,
	// but PTB has to use a different system (changed by special gluOrtho2D) transform),
	// because our 2D coordinate system needs to conform to the standards of the old
	// Psychtoolboxes and of typical windowing systems. -- A tribute to the past.
	
	// Upper left vertex in window
	glTexCoord2f(0, h);
	glVertex2f(0, 0);		
	
	// Lower left vertex in window
	glTexCoord2f(0, 0);
	glVertex2f(0, h);		
	
	// Lower right  vertex in window
	glTexCoord2f(w, 0);
	glVertex2f(w, h);		
	
	// Upper right in window
	glTexCoord2f(w, h);
	glVertex2f(w, 0);		
	
	glEnd();
	
	if (x!=0 || y!=0 || sx!=1.0 || sy!=1.0) {
		glPopMatrix();
	}

	// Done.
	return(TRUE);
}

/* PsychPipelineBuiltinRenderClutBitsPlusPlus - Encode Bits++ CLUT into framebuffer.
 * 
 * This builtin routine takes the current gamma table for this windowRecord, encodes it into a Bits++
 * compatible T-Lock CLUT and renders it into the framebuffer.
 */
boolean PsychPipelineBuiltinRenderClutBitsPlusPlus(PsychWindowRecordType *windowRecord, PsychHookFunction* hookfunc)
{
	const int bitshift = 16; // Bits++ expects 16 bit numbers, but ignores 2 least significant bits --> Effective 14 bit.
	int i, j, x, y;
	unsigned int r, g, b;
	double t1, t2;
	
	if (windowRecord->loadGammaTableOnNextFlip != 0 || windowRecord->inRedTable == NULL) {
		if (PsychPrefStateGet_Verbosity()>0) printf("PTB-ERROR: Bits++ CLUT encoding failed. No suitable CLUT set in Screen('LoadNormalizedGammaTable'). Skipped!\n");
		return(FALSE);
	}
	
	if (windowRecord->inTableSize < 256) {
		if (PsychPrefStateGet_Verbosity()>0) printf("PTB-ERROR: Bits++ CLUT encoding failed. CLUT has less than the required 256 entries. Skipped!\n");
		return(FALSE);
	}
	
	if (PsychPrefStateGet_Verbosity() > 4) {  
		glFinish();
		PsychGetAdjustedPrecisionTimerSeconds(&t1);
	}

	// Render CLUT as sequence of single points:
	// We render it twice in two lines, the 2nd line shifted horizontally by one pixel. This way at least one of
	// the lines will always start at an even pixel location as mandated by Bits++ - More failsafe.
	for (j=1; j<=2 ; j++) {
		x=j;
		y=j;
		
		glPointSize(1);
		glBegin(GL_POINTS);
		
		// First the T-Lock unlock key:
		glColor3ub(36, 106, 133);
		glVertex2i(x++, y);
		glColor3ub(63, 136, 163);
		glVertex2i(x++, y);
		glColor3ub(8, 19, 138);
		glVertex2i(x++, y);
		glColor3ub(211, 25, 46);
		glVertex2i(x++, y);
		glColor3ub(3, 115, 164);
		glVertex2i(x++, y);
		glColor3ub(112, 68, 9);
		glVertex2i(x++, y);
		glColor3ub(56, 41, 49);
		glVertex2i(x++, y);
		glColor3ub(34, 159, 208);
		glVertex2i(x++, y);
		glColor3ub(0, 0, 0);
		glVertex2i(x++, y);
		glColor3ub(0, 0, 0);
		glVertex2i(x++, y);
		glColor3ub(0, 0, 0);
		glVertex2i(x++, y);
		glColor3ub(0, 0, 0);
		glVertex2i(x++, y);
		
		// Now the encoded CLUT: We encode 16 bit values in a high and a low pixel,
		// Bits++ throws away the two least significant bits - get 14 bit output resolution.
		for (i=0; i<256; i++) {
			// Convert 0.0 - 1.0 float value into 0 - 2^14 -1 integer range of Bits++
			r = (unsigned int)(windowRecord->inRedTable[i] * (float)((1 << bitshift) - 1) + 0.5f);
			g = (unsigned int)(windowRecord->inGreenTable[i] * (float)((1 << bitshift) - 1) + 0.5f);
			b = (unsigned int)(windowRecord->inBlueTable[i] * (float)((1 << bitshift) - 1) + 0.5f);
			
			// Pixel with high-byte of 16 bit value:
			glColor3ub((GLubyte) ((r >> 8) & 0xff), (GLubyte) ((g >> 8) & 0xff), (GLubyte) ((b >> 8) & 0xff));
			glVertex2i(x++, y);
			
			// Pixel with low-byte of 16 bit value:
			glColor3ub((GLubyte) (r & 0xff), (GLubyte) (g  & 0xff), (GLubyte) (b  & 0xff));
			glVertex2i(x++, y);
		}
		
		glEnd();
	}		
	
	if (PsychPrefStateGet_Verbosity() > 4) {  
		glFinish();
		PsychGetAdjustedPrecisionTimerSeconds(&t2);
		printf("PTB-DEBUG: Execution time of built-in Bits++ CLUT encoder was %lf ms.\n", (t2 - t1) * 1000.0f);
	}

	// Done.
	return(TRUE);
}

/* PsychPipelineBuiltinRenderStereoSyncLine() -- Render sync trigger lines for quad-buffered stereo contexts.
 *
 * A builtin function to be called for drawing of blue-line-sync marker lines in quad-buffered stereo mode.
 */
boolean PsychPipelineBuiltinRenderStereoSyncLine(PsychWindowRecordType *windowRecord, PsychHookFunction* hookfunc)
{
	GLenum draw_buffer;
	char* strp;
	float blackpoint, r, g, b;
	float fraction = 0.25;
	float w = PsychGetWidthFromRect(windowRecord->rect);
	float h = PsychGetHeightFromRect(windowRecord->rect);
	r=g=b=1.0;
	
	// Options provided?
	
	// Check for override vertical position for sync line. Default is last scanline of display.
	if (strp=strstr(hookfunc->pString1, "yPosition=")) {
		// Parse and assign offset:
		if (sscanf(strp, "yPosition=%i", &h)!=1) {
			PsychErrorExitMsg(PsychError_user, "builtin:RenderStereoSyncLine: yPosition parameter for horizontal stereo blue-sync line position is invalid! Parse error...\n");
		}
	}

	// Check for override horizontal fraction for sync line. Default is 25% for left eye, 75% for right eye.
	if (strp=strstr(hookfunc->pString1, "hFraction=")) {
		// Parse and assign offset:
		if ((sscanf(strp, "hFraction=%f", &fraction)!=1) || (fraction < 0.0) || (fraction > 1.0)) {
			PsychErrorExitMsg(PsychError_user, "builtin:RenderStereoSyncLine: hFraction parameter for horizontal stereo blue-sync line length is invalid!\n");
		}
	}
	
	// Check for override color of sync-line. Default is white.
	if (strp=strstr(hookfunc->pString1, "Color=")) {
		// Parse and assign offset:
		if (sscanf(strp, "Color=%f %f %f", &r, &g, &b)!=3) {
			PsychErrorExitMsg(PsychError_user, "builtin:RenderStereoSyncLine: Color spec for stereo sync-line is invalid!\n");
		}
	}

	// Query current target buffer:
	glGetIntegerv(GL_DRAW_BUFFER, &draw_buffer);
	
	if (draw_buffer == GL_BACK_LEFT || draw_buffer == GL_FRONT_LEFT) {
		// Left stereo buffer:
		blackpoint = fraction;
	}
	else if (draw_buffer == GL_BACK_RIGHT || draw_buffer == GL_FRONT_RIGHT) {
		// Right stereo buffer:
		blackpoint = 1 - fraction;
	}
	else {
		// No stereo buffer! No stereo mode. This routine is a no-op...
		if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Stereo sync line renderer called on non-stereo framebuffer!?!\n");  
		return(TRUE);
	}
	
	glLineWidth(1);
	glBegin(GL_LINES);
	glColor3f(r, g, b);
	glVertex2i(0, h);
	glVertex2i(w*blackpoint, h);
	glColor3f(0, 0, 0);
	glVertex2i(w*blackpoint, h);
	glVertex2i(w, h);
	glEnd();

	return(TRUE);
}

