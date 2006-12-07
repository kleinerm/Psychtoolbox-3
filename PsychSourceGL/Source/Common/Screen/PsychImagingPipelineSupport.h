/*
	PsychToolbox3/Source/Common/Screen/PsychImagingPipelineSupport.h
	
	PLATFORMS:	
	
		All.  
				
	AUTHORS:
	
		Mario Kleiner           mk              mario.kleiner at tuebingen.mpg.de

	HISTORY:
	
		12/05/06	mk	Wrote it.
		
	DESCRIPTION:
	
		Infrastructure for all Screen imaging pipeline functions, i.e., hook callback functions and chains
		and GLSL based internal image processing pipeline.
		
	NOTES:
	
	TO DO: 
	
*/

//include once
#ifndef PSYCH_IS_INCLUDED_PsychImagingPipelineSupport
#define PSYCH_IS_INCLUDED_PsychImagingPipelineSupport

#include "Screen.h"

// Initialize imaging pipeline for windowRecord, applying the imagingmode flags. Called by Screen('OpenWindow')
void	PsychInitializeImagingPipeline(PsychWindowRecordType *windowRecord, int imagingmode);
void	PsychPipelineListAllHooks(PsychWindowRecordType *windowRecord);
void	PsychPipelineDumpAllHooks(PsychWindowRecordType *windowRecord);
void	PsychPipelineDumpHook(PsychWindowRecordType *windowRecord, const char* hookString);
void	PsychPipelineDisableHook(PsychWindowRecordType *windowRecord, const char* hookString);
void	PsychPipelineEnableHook(PsychWindowRecordType *windowRecord, const char* hookString);
void	PsychPipelineResetHook(PsychWindowRecordType *windowRecord, const char* hookString);
int		PsychPipelineQueryHookSlot(PsychWindowRecordType *windowRecord, const char* hookString, char** idString, char** blitterString, double* doubleptr, double* shaderid, double* luttexid1);
void	PsychPipelineAddBuiltinFunctionToHook(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, const char* configString);
void	PsychPipelineAddRuntimeFunctionToHook(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, const char* evalString);
void	PsychPipelineAddCFunctionToHook(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, void* procPtr);
void	PsychPipelineAddShaderToHook(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, unsigned int shaderid, const char* blitterString, unsigned int luttexid1);
//end include once
#endif

