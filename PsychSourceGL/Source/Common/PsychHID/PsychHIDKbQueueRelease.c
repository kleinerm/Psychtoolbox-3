/*
	PsychtoolboxGL/Source/Common/PsychHID/PsychHIDKbQueueRelease.c		
  
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

static char useString[]= "PsychHID('KbQueueRelease' [, deviceIndex])";
static char synopsisString[] = 
        "Releases queue and other resources allocated by PsychHID('KbQueueCreate').\n"
        "The optional 'deviceIndex' is the index of the HID input device whose queue should be released. "
        "If omitted, the queue of the default device will be released.\n";

static char seeAlsoString[] = "KbQueueCreate, KbQueueStart, KbQueueStop, KbQueueCheck, KbQueueFlush";

PsychError PSYCHHIDKbQueueRelease(void) 
{
    int deviceIndex;
    
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    deviceIndex = -1;
    PsychCopyInIntegerArg(1, kPsychArgOptional, &deviceIndex);
    
    PsychHIDOSKbQueueRelease(deviceIndex);

    return(PsychError_none);
}
