/*
	PsychtoolboxGL/Source/Common/PsychHID/PsychHIDKbQueueStop.c		
  
	PROJECTS: 
	
		PsychHID only.
  
	PLATFORMS:  
	
		All.  
  
	AUTHORS:
	
		rwoods@ucla.edu		rpw 
        mario.kleiner@tuebingen.mpg.de      mk
      
	HISTORY:
		8/19/07  rpw		Created.
		8/23/07  rpw        Added PsychHIDQueueFlush to documentation; removed call to PsychHIDVerifyInit()
  
	NOTES:
	
*/

#include "PsychHID.h"

static char useString[]= "PsychHID('KbQueueStop' [, deviceIndex])";
static char synopsisString[] = 
		"Stops input event collection by a queue.\n"
        "PsychHID('KbQueueCreate') must be called before calling this function.\n"
        "The optional 'deviceIndex' is the index of the HID input device whose queue should be stopped. "
        "If omitted, the queue of the default device will be stopped.\n";

static char seeAlsoString[] = "KbQueueCreate, KbQueueStart, KbQueueCheck, KbQueueFlush, KbQueueRelease";
 
PsychError PSYCHHIDKbQueueStop(void) 
{
    int deviceIndex;
    
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));

    deviceIndex = -1;
    PsychCopyInIntegerArg(1, kPsychArgOptional, &deviceIndex);

    PsychHIDOSKbQueueStop(deviceIndex);

    return(PsychError_none);
}
