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

/*  PsychInitializeImagingPipeline()
 *
 *  Initialize imaging pipeline for windowRecord, applying the imagingmode flags. Called by Screen('OpenWindow').
 *
 *  This routine performs initial setup of an imaging pipeline for an onscreen window. It sets up reasonable
 *  default values in the windowRecord (imaging pipe disabled defaults if imagingmode is zero), based on
 *  the imagingmode flags and all the windowRecord and OpenGL settings.
 *
 *  1. All hook chains are initialized to empty & disabled.
 *  2. FBO's are setup according to the requested imagingmode, stereomode and color depth of a window.
 *  3. Depending on stereo mode and imagingmode, some default GLSL shaders may get created and attached to
 *     some hook-chains for advanced stereo processing.
 */
void PsychInitializeImagingPipeline(PsychWindowRecordType *windowRecord, int imagingmode)
{

	// Processing ends here after minimal "all off" setup, if pipeline is disabled:
	if (imagingmode<=0) return;
	
	// Specific setup of pipeline if real imaging ops are requested:
	if (PsychPrefStateGet_Verbosity()>2) printf("PTB-INFO: Psychtoolbox imaging pipeline enabled for window with imaging flags %i ...\n", imagingmode);
	fflush(NULL);
	
	// Well done.
	return;
}

void PsychPipelineListAllHooks(PsychWindowRecordType *windowRecord)
{
	printf("PTB-INFO: The Screen command currently provides the following hook functions:\n");
	printf("=============================================================================\n");
	printf("Screen('Flip'):\n");
	printf("- Flip_MasterOverride : Complete replacement for the command. C and M functions.\n");
	printf("- Flip_MasterOverride : Complete replacement for the command. C and M functions.\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("=============================================================================\n\n");	
	fflush(NULL);
	// Well done.
	return;	
}

//void	PsychPipelineDumpAllHooks(PsychWindowRecordType *windowRecord)
//void	PsychPipelineDumpHook(PsychWindowRecordType *windowRecord, const char* hookString)
//void	PsychPipelineDisableHook(PsychWindowRecordType *windowRecord, const char* hookString);
//void	PsychPipelineEnableHook(PsychWindowRecordType *windowRecord, const char* hookString);
//void	PsychPipelineResetHook(PsychWindowRecordType *windowRecord, const char* hookString);
//int		PsychPipelineQueryHookSlot(PsychWindowRecordType *windowRecord, const char* hookString, char** idString, char** blitterString, double* doubleptr, double* shaderid, double* luttexid1);
//void	PsychPipelineAddBuiltinFunctionToHook(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, const char* configString);
//void	PsychPipelineAddRuntimeFunctionToHook(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, const char* evalString);
//void	PsychPipelineAddCFunctionToHook(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, void* procPtr);
//void	PsychPipelineAddShaderToHook(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, unsigned int shaderid, const char* blitterString, unsigned int luttexid1);
