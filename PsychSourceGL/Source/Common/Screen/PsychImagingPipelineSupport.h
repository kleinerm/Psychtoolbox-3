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

// Symbolic names for Hook-Chains:
typedef enum {
	kPsychCloseWindowPreGLShutdown =				0,
	kPsychCloseWindowPostGLShutdown =				1,
	kPsychUserspaceBufferDrawingFinished =			2,
	kPsychStereoLeftCompositingBlit =				3,
	kPsychStereoRightCompositingBlit =				4,
	kPsychStereoCompositingBlit =					5,
	kPsychPostCompositingBlit =						6,
	kPsychFinalOutputFormattingBlit =				7,
	kPsychUserspaceBufferDrawingPrepare =			8,
} PsychHookType;

// API for PTB core:
void	PsychInitImagingPipelineDefaultsForWindowRecord(PsychWindowRecordType *windowRecord);
void	PsychInitializeImagingPipeline(PsychWindowRecordType *windowRecord, int imagingmode);
void	PsychShutdownImagingPipeline(PsychWindowRecordType *windowRecord, Boolean openglpart);
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

boolean	PsychPipelineExecuteHook(PsychWindowRecordType *windowRecord, int hookId, void* hookUserData, void* hookBlitterFunction, boolean srcIsReadonly, boolean allowFBOSwizzle, PsychFBO** srcfbo1, PsychFBO** srcfbo2, PsychFBO** dstfbo, PsychFBO** bouncefbo);
boolean	PsychPipelineExecuteHookSlot(PsychWindowRecordType *windowRecord, int hookId, PsychHookFunction* hookfunc, void* hookUserData, void* hookBlitterFunction, boolean srcIsReadonly, boolean allowFBOSwizzle, PsychFBO** srcfbo1, PsychFBO** srcfbo2, PsychFBO** dstfbo, PsychFBO** bouncefbo);

// Internal helper functions:
PsychHookFunction* PsychAddNewHookFunction(PsychWindowRecordType *windowRecord, const char* hookString, const char* idString, int where, int hookfunctype);
int		PsychGetHookByName(const char* hookName);
// Setup source -> rendertarget binding for next rendering pass:
void	PsychPipelineSetupRenderFlow(PsychFBO* srcfbo1, PsychFBO* srcfbo2, PsychFBO* dstfbo);
// Create OpenGL framebuffer object for internal rendering, setup PTB info struct for it:
Boolean PsychCreateFBO(PsychFBO** fbo, GLenum fboInternalFormat, Boolean needzbuffer, int width, int height);

//end include once
#endif

