/*

  Psychtoolbox3/Source/Common/SCREENNull.c		

  AUTHORS:

  Allen.Ingling@nyu.edu		awi 

  PLATFORMS:	

  This file should build on any platform. 

  HISTORY:

  11/26/02  awi		Created. 

  DESCRIPTION:

  Arbitrary code to be executed for debugging/testing/hacking PTB. This subfunction is not
  documented or mentioned in official docs and only here for the purpose of internal testing
  and as a container for quick & dirty hacks and tests by the core developers.

  Therefore its syntax and semantics can change in any way, anytime without warning.

*/

#include "Screen.h"

static char useString[] = "[[value1], [value2] ..]=SCREEN('Null',[value1],[value2],...);";
static char synopsisString[] = 
	"Special test function for Psychtoolbox testing and developments. Normal users: DO NOT USE!";
static char seeAlsoString[] = "";

PsychError SCREENNull(void) 
{
#define RADEON_D1CRTC_INTERRUPT_CONTROL 0x60DC

#define RADEON_R500_GEN_INT_CNTL   0x100
#define RADEON_R500_GEN_INT_STATUS 0x104
//#define RADEON_R500_GEN_INT_CNTL   0x040
//#define RADEON_R500_GEN_INT_STATUS 0x044
//#define RADEON_R500_GEN_INT_CNTL   0x200
//#define RADEON_R500_GEN_INT_STATUS 0x204

	const double defaultMatrix[] = {1.1, 1.2, 1.3, 1.4, 2.1, 2.2, 2.3, 2.4};
	const double defaultM=2, defaultN=4; 
	double tempValue; 
	double *array;
	int i, m,n, p, numInArgs, numOutArgs, numNamedOutArgs;
	char *str;
	PsychArgFormatType format;
	const char defaultString[] = "I am the default string\n";

	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

	#if PSYCH_SYSTEM == PSYCH_OSX
		printf("PTB-DEBUG: Shutting down connection to PsychtoolboxKernelDriver, if any...\n");
		PsychOSShutdownPsychtoolboxKernelDriverInterface();
	#endif
	
	#if PSYCH_SYSTEM == PSYCH_LINUX
		printf("PTB-DEBUG: PreRADEON_R500_GEN_INT_CNTL: %x\n", PsychOSKDReadRegister(0, 0x040, NULL));
		PsychOSKDWriteRegister(0, 0x040, PsychOSKDReadRegister(0, 0x040, NULL) | 0x1, NULL);
		PsychWaitIntervalSeconds(0.1);
		printf("PTB-DEBUG: PostRADEON_R500_GEN_INT_CNTL: %x\n", PsychOSKDReadRegister(0, 0x040, NULL));

		printf("PTB-DEBUG: RADEON_R500_GEN_INT_STATUS: %x\n", PsychOSKDReadRegister(0, 0x044, NULL));
		printf("PTB-DEBUG: RADEON_R500_GEN_INT_CNTL: %x\n", PsychOSKDReadRegister(0, RADEON_R500_GEN_INT_CNTL, NULL));
		printf("PTB-DEBUG: RADEON_R500_GEN_INT_STATUS: %x\n", PsychOSKDReadRegister(0, RADEON_R500_GEN_INT_STATUS, NULL));
		printf("PTB-DEBUG: RADEON_D1CRTC_INTERRUPT_CONTROL: %x\n", PsychOSKDReadRegister(0, RADEON_D1CRTC_INTERRUPT_CONTROL, NULL));
	#endif

	return(PsychError_none);

	//demonstrate how we find the function and subfunction names
	//printf("Psychtoolbox function: %s, subfunction %s\n", PsychGetModuleName(), PsychGetFunctionName() );

	//copy all the input argument to their outputs if we have doubles, if not error.  
	numInArgs = PsychGetNumInputArgs();
	numOutArgs = PsychGetNumOutputArgs();
	numNamedOutArgs = PsychGetNumNamedOutputArgs();
	PsychErrorExit(PsychCapNumOutputArgs(numInArgs));

	/*
	printf("number of input arguments: %d\n", numInArgs);
	printf("number of output arguments: %d\n", numOutArgs);
	printf("number of named output arguments: %d\n", numNamedOutArgs);
	*/

	//iterate over each of the supplied inputs.  If the input is a two-dimensional array 
	//of doubles or a character array, then copy it to the output.  
	for(i=1;i<=numInArgs;i++){

		format = PsychGetArgType(i);

		switch(format){

			case PsychArgType_double:

				if(PsychGetArgM(i)==1 && PsychGetArgN(i)==1){

					tempValue=i;  //if 1x1 double then the default return value is the arg position.

					PsychCopyInDoubleArg(i, FALSE, &tempValue);

					PsychCopyOutDoubleArg(i, FALSE, tempValue);

				}else{

					PsychAllocInDoubleMatArg(i, FALSE, &m, &n, &p, &array);

					PsychCopyOutDoubleMatArg(i, FALSE, m, n, p, array);

				}

				break;

			case PsychArgType_char:

				str=NULL; //This tells PsychGetCharArg() to use its own (volatile) memory. 

				PsychAllocInCharArg(i, FALSE, &str); 

				PsychCopyOutCharArg(i, FALSE, str);

				break;

			case PsychArgType_default:

				PsychCopyOutCharArg(i, FALSE, defaultString);

				break;

		}

	}

	return(PsychError_none);
}
