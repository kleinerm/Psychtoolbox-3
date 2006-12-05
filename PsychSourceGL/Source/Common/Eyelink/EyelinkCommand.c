/*

	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkCommand.c
  
	PROJECTS: Eyelink 
  
	AUTHORS:
		cburns@berkeley.edu				cdb
		E.Peters@ai.rug.nl				emp
		f.w.cornelissen@med.rug.nl		fwc
  
	PLATFORMS:	Currently only OS X  
    
	HISTORY:

		11/23/05  cdb		Created.
		30-10-06	fwc		does now actually output status

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

// Help strings
static char useString[] =  "[status =] Eyelink('Command', 'formatstring', [...])";

static char synopsisString[] = 
   "link command formatting."
   " use just like printf()."
   " Only chars and ints allowed in arguments!"
   " returns command result,"
   " allows 500 msec. for command to finish.";

static char seeAlsoString[] = "";

/*
ROUTINE: EYELINKcommand
PURPOSE:
  uses int eyecmd_printf(char *fmt, ...)
  The Eyelink tracker accepts text commands through the link.
  These commands may be used to configure the system, open
  data file, and so on. This function is used with the same formatting
  methods as printf(), allowing numbers to be included in commands.
  The function waits up to 500 msec for a success or failure code to
  be returned from the tracker, then returns the error code NO_REPLY.
  RETURNS: 0 if OK, else errorcode. 
*/

PsychError EyelinkCommand(void)
{
	int					i				= 0;
	int					iNumInArgs		= 0;
	int					iStatus			= -1;
	PsychArgFormatType	psychArgType	= PsychArgType_none;
	double				fTempValue		= 0.0;
	char				*pstrTemp		= NULL;
	char				*pstrFormat		= NULL;
	void				**pArgs			= NULL;
	char				strCommand[256];
	
	// Clear strings
	memset(strCommand, 0, sizeof(strCommand));

	// Add help strings
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	
	// Output help if asked
	if(PsychIsGiveHelp()) {
		PsychGiveHelp();
		return(PsychError_none);
	}

	// Check arguments
	PsychErrorExit(PsychRequireNumInputArgs(1));
//	PsychErrorExit(PsychCapNumOutputArgs(0));
	PsychErrorExit(PsychCapNumOutputArgs(1));
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();

	// Alloc and grab the input format string
	PsychAllocInCharArg(1, TRUE, &pstrFormat);
	iNumInArgs = PsychGetNumInputArgs();   

	// Alloc and grab input args
	if (iNumInArgs > 1) {
		pArgs = (void **)mxMalloc((iNumInArgs-1) * sizeof(char *));
		// loop over the args
		for (i = 2; i <= iNumInArgs; i++) {
			psychArgType = PsychGetArgType(i);
			switch(psychArgType) {
				case PsychArgType_double:
					if ((PsychGetArgM(i) == 1) && (PsychGetArgN(i) == 1)) {
						PsychCopyInDoubleArg(i, TRUE, &fTempValue);
						pArgs[i-2] = (void *) (int) fTempValue; 
					} else {
						PsychGiveHelp();
						return(PsychError_user);
					}
					break;
				case PsychArgType_char:
					pArgs[i-2] = NULL;
					PsychAllocInCharArg(i, TRUE, &pstrTemp);
					pArgs[i-2] = pstrTemp;
					break;
				default:
					PsychGiveHelp();
					return(PsychError_user);
					break;
			} 
		}
	}
	
	// Build eyelink command and execute
	vsprintf(strCommand, pstrFormat, (va_list)pArgs);
	iStatus = eyecmd_printf(strCommand);
	if (pArgs != NULL) {
		mxFree(pArgs);
	}
	
	// Copy out the command result
	PsychCopyOutDoubleArg(1, FALSE, iStatus);
   
	return(PsychError_none);
}

