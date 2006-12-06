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
static char useString[] = "[ret1, ret2] = Screen('HookFunction', windowPtr, 'Subcommand', 'HookName', arg1, arg2);";
//                                                1           2             3          4     5        
static char synopsisString[] = 
    "Manage Screen processing hook chains. "
	"TODO - Write proper online help.";
	
static char seeAlsoString[] = "";

PsychError SCREENHookFunction(void) 
{
   PsychWindowRecordType  *windowRecord;
   char			           *textString;
   int                    stringl, i;

    // All subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
    
    PsychErrorExit(PsychCapNumInputArgs(5));   	
    PsychErrorExit(PsychRequireNumInputArgs(2)); 	
    PsychErrorExit(PsychCapNumOutputArgs(2));  

    // Get the window structure for the onscreen window.
    PsychAllocInWindowRecordArg(1, TRUE, &windowRecord);
    
    //Get the text string (it is required)
    PsychAllocInCharArg(2, kPsychArgRequired, &textString);

    // Done.
    return(PsychError_none);
}
