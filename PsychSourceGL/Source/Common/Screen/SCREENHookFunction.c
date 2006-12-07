/*
	SCREENHookFunction.c	
  
    AUTHORS:
    
		Mario.Kleiner@tuebingen.mpg.de  mk
  
    PLATFORMS:	
	
		All.
    
    HISTORY:
    
		12/05/06	mk		Wrote it.
	
    DESCRIPTION:

		Manage special callback functions (processing hooks). Screen allows to "hook" specific
		OpenGL GLSL shaders, C callback functions or Matlab M-File functions into its internal
		processing chain at well defined points in a (hopefully) well defined manner. This allows
		to extend or customize Screens behaviour. Useful applications: Automatic transparent
		on-the-fly image processing on stimuli just before stimulus onset, e.g., filtering.
		Customization of drawing commands. Advanced stereo stimulus output algorithms. Support for
		special output devices like BrightSide HDR, Bits++, Pelli & Zhang attenuator..., interfacing
		with 3rd party hardware like trigger systems...
		
		We provide well defined hook points, each one with a unique descriptive name. Each hook is
		associated with a queue or chain of callback functions which is executed from the beginning
		to the end whenever that hook is processed. A callback can be a GLSL OpenGL shader program
		for image processing, a Matlab M-Function for high-level processing or a C callback function
		for complex low-level or realtime operations like triggering some output device or similar.
		
		This Screen subfunction provides subfunctions to add callbacks to the chain, reset the chain,
		enable or disable the chain, query properties of single hooks or dump the whole chain state
		to the command window for debug purpose.
  
    NOTES:

    TO DO:

*/

#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "[ret1, ret2] = Screen('HookFunction', windowPtr, 'Subcommand', 'HookName', arg1, arg2, arg3, arg4);";
//                                                               1           2             3          4     5     6     7
static char synopsisString[] = 
    "Manage Screen processing hook chains. "
	"TODO - Write proper online help.";
	
static char seeAlsoString[] = "";

PsychError SCREENHookFunction(void) 
{
	PsychWindowRecordType	*windowRecord;
	char					*cmdString, *hookString, *idString, *blitterString;
	int						i, cmd, slotid;
	double					doubleptr;
	double					shaderid, luttexid1 = 0;

	blitterString = NULL;

    // All subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
    
    PsychErrorExit(PsychCapNumInputArgs(7));   	
    PsychErrorExit(PsychRequireNumInputArgs(2)); 	
    PsychErrorExit(PsychCapNumOutputArgs(2));  

    // Get the window structure for the onscreen window.
    PsychAllocInWindowRecordArg(1, TRUE, &windowRecord);
    
    // Get the subcommand string:
	PsychAllocInCharArg(2, kPsychArgRequired, &cmdString);
	
	// Subcommand dispatcher:
	cmd=0;
	if (strstr(cmdString, "Append"))  cmd=1;
	if (strstr(cmdString, "Prepend")) cmd=2;
	if (strcmp(cmdString, "Reset")==0)   cmd=3;
	if (strcmp(cmdString, "Enable")==0)  cmd=4;
	if (strcmp(cmdString, "Disable")==0) cmd=5;
	if (strcmp(cmdString, "Query")==0)   cmd=6;
	if (strcmp(cmdString, "Dump")==0)    cmd=7;
	if (strcmp(cmdString, "DumpAll")==0) cmd=8;
	if (strcmp(cmdString, "ListAll")==0) cmd=9;
	if(cmd==0) PsychErrorExitMsg(PsychError_user, "Unknown subcommand specified to 'HookFunction'.");
	
	// Need hook name?
	if(cmd!=9) {
		// Get it:
		PsychAllocInCharArg(3, kPsychArgRequired, &hookString);
	}
	
	switch(cmd) {
		case 1: // Append:
		case 2: // Prepend:
			// Add a new hook function callback to chain, either at beginning or end.
			
			// What type of callback/handler is to be added?
			if(strstr(cmdString, "Shader")) {
				// GLSL shader program object:
				
				// The id string:
				PsychAllocInCharArg(4, kPsychArgRequired, &idString);
				
				// The shader object handle:
				PsychCopyInDoubleArg(5, TRUE, &shaderid);

				// The blitter config string:
				PsychAllocInCharArg(6, FALSE, &blitterString);

				// (Optionally) a texture handle for a texture to attach to 2nd unit:
				PsychCopyInDoubleArg(7, FALSE, &luttexid1);
				
				// Add shader: 
				PsychPipelineAddShaderToHook(windowRecord, hookString, idString, (cmd==1) ? 1:0, (unsigned int) shaderid, blitterString, (unsigned int) luttexid1);
			}
			else if(strstr(cmdString, "CFunction")) {
				// C callback function:

				// First the id string:
				PsychAllocInCharArg(4, kPsychArgRequired, &idString);
				
				// Then the void* to the function, encoded as a double value:
				PsychCopyInDoubleArg(5, TRUE, &doubleptr);
				
				// Add the function void* to the chain:
				PsychPipelineAddCFunctionToHook(windowRecord, hookString, idString, (cmd==1) ? 1:0, PsychDoubleToPtr(doubleptr));
				
			}
			else if(strstr(cmdString, "MFunction")) {
				// Matlab/Octave/Whatever runtime environment callback function:

				// First the id string:
				PsychAllocInCharArg(4, kPsychArgRequired, &idString);
				
				// Then the call string (for feval()) for the function:
				PsychAllocInCharArg(5, TRUE, &blitterString);
				
				// Add the function to the chain:
				PsychPipelineAddRuntimeFunctionToHook(windowRecord, hookString, idString, (cmd==1) ? 1:0, blitterString);
				
			}
			else if(strstr(cmdString, "Builtin")) {
				// Built in special function:

				// First the id string:
				PsychAllocInCharArg(4, kPsychArgRequired, &idString);
				
				// Then the options-string:
				PsychAllocInCharArg(5, TRUE, &blitterString);
				
				// Add the function to the chain:
				PsychPipelineAddBuiltinFunctionToHook(windowRecord, hookString, idString, (cmd==1) ? 1:0, blitterString);				
			}
			else {
				// Unknown?!?
				PsychErrorExitMsg(PsychError_user, "Unknown callback type specified to 'HookFunction'.");
			}
		break;
		
		case 3: // Reset hook-chain:
			PsychPipelineResetHook(windowRecord, hookString);
		break;

		case 4: // Enable hook-chain:
			PsychPipelineEnableHook(windowRecord, hookString);
		break;

		case 5: // Disable hook-chain:
			PsychPipelineDisableHook(windowRecord, hookString);
		break;
		
		case 6: // Query properties of a slot in a specific hook-chain:
			// Get the id string:
			PsychAllocInCharArg(4, kPsychArgRequired, &idString);
			
			// Query everything that's there and copy it out:
			slotid = PsychPipelineQueryHookSlot(windowRecord, hookString, &idString, &blitterString, &doubleptr, &shaderid, &luttexid1);
			
			// Copy out all infos:
			PsychCopyOutDoubleArg(1, FALSE, slotid);
			PsychCopyOutCharArg(2, FALSE, (idString) ? idString : "NONE");
			PsychCopyOutCharArg(3, FALSE, (blitterString) ? blitterString : "NONE");
			PsychCopyOutDoubleArg(4, FALSE, doubleptr);
			PsychCopyOutDoubleArg(5, FALSE, shaderid);
			PsychCopyOutDoubleArg(6, FALSE, luttexid1);
		break;

		case 7: // Dump specific hook-chain:
			PsychPipelineDumpHook(windowRecord, hookString);
		break;

		case 8: // Dump all hook-chains:
			PsychPipelineDumpAllHooks(windowRecord);
		break;
		
		case 9: // List all hook-chains:
			PsychPipelineListAllHooks(windowRecord);
		break;
	}
	
    // Done.
    return(PsychError_none);
}
