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
	
	// Setup mode switch in record:
	windowRecord->imagingMode = imagingmode;
	
	// Well done.
	return;
}

/* PsychShutdownImagingPipeline()
 * Shutdown imaging pipeline for a windowRecord and free all ressources associated with it.
 */
void PsychShutdownImagingPipeline(PsychWindowRecordType *windowRecord, Boolean openglpart)
{
	int i;
	PtrPsychHookFunction hookfunc, hookiter;
	
	// Imaging enabled?
	if (windowRecord->imagingMode>0) {
		// Yes. Mode specific cleanup:
	} 

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
void PsychPipelineExecuteHook(PsychWindowRecordType *windowRecord, int hookId, void* hookUserData, void* hookBlitterFunction, int minfbo, int maxfbo)
{
	PtrPsychHookFunction hookfunc;
	int i=0;
	int srcfbo, dstfbo;
	srcfbo = minfbo;
	dstfbo = maxfbo;
	
	// Child protection:
	if (hookId<0 || hookId>=MAX_SCREEN_HOOKS) PsychErrorExitMsg(PsychError_internal, "In PsychPipelineExecuteHook: Was asked to execute unknown (non-existent) hook chain with invalid id!");
	// Hook chain enabled for processing? We skip otherwise.
	if (!windowRecord->HookChainEnabled[hookId]) return;
	
	// Get start of enabled chain:
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
		
		// Process this hook function:
		PsychPipelineExecuteHookSlot(windowRecord, hookId, hookfunc, hookUserData, hookBlitterFunction, &srcfbo, &dstfbo);
		
		// Process next hookfunc slot in chain, if any:
		i++;
		hookfunc = hookfunc->next;
	}

	// Done.
	return;
}

/* PsychPipelineExecuteHookSlot()
 * Execute a single hookfunction slot in a hook chain for a specific window.
 */
void PsychPipelineExecuteHookSlot(PsychWindowRecordType *windowRecord, int hookId, PsychHookFunction* hookfunc, void* hookUserData, void* hookBlitterFunction, int* srcfbo, int* dstfbo)
{
	// Dispatch by hook function type:
	switch(hookfunc->hookfunctype) {
		case kPsychShaderFunc:
			printf("TODO: EXECUTE -- GLSL-Shader      : id=%i , luttex1=%i , blitter=%s\n", hookfunc->shaderid, hookfunc->luttexid1, hookfunc->pString1);
			break;
			
		case kPsychCFunc:
			printf("TODO: EXECUTE -- C-Callback       : void*= %p\n", hookfunc->cprocfunc);
			break;
			
		case kPsychMFunc:
			// printf("EXECUTING: Runtime-Function : Evalstring= %s\n", hookfunc->pString1);
			#if PSYCH_LANGUAGE == PSYCH_MATLAB
				PsychRuntimeEvaluateString(hookfunc->pString1);
			#else
				printf("TODO: Implement runtime function support in GNU/Octave!\n");
			#endif
			break;
			
		case kPsychBuiltinFunc:
			printf("TODO: EXECUTE -- Builtin-Function : Name= %s\n", hookfunc->idString);
			break;
			
		default:
			PsychErrorExitMsg(PsychError_internal, "In PsychPipelineExecuteHookSlot: Was asked to execute unknown (non-existent) hook function type!");
	}
	
	return;
}
