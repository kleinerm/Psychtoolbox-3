/*
	PsychtoolboxGL/Source/Common/PsychHID/PsychHIDKbQueueFlush.c		
  
	PROJECTS: 
	
		PsychHID only.
  
	PLATFORMS:  
	
		All.  
  
	AUTHORS:
	
		rwoods@ucla.edu		rpw 
        mario.kleiner@tuebingen.mpg.de      mk
      
	HISTORY:
		8/23/07  rpw        Created.
  
	NOTES:
	
*/

#include "PsychHID.h"

static char useString[]= "[navail] = PsychHID('KbQueueFlush' [, deviceIndex][, flushType=1])";
static char synopsisString[] = 
            "Flushes all scored and unscored keyboard events from a queue.\n"
            "Returns number of events 'navail' in keyboard event buffer before the flush takes place.\n"
            "If 'flushType' is 0, only the number of currently queued events will be returned.\n"
            "If 'flushType' is 1, only events returned by KbQueueCheck will be flushed. This is the default.\n"
            "If 'flushType' is 2, only events returned by KbQueueGetEvent will be flushed.\n"
            "If 'flushType' is 3, events returned by both KbQueueCheck and KbQueueGetEvent will be flushed.\n"
            "If 'flushType' & 4, only the number of key-press events with valid, mapped ASCII CookedKey field will be returned.\n"
            "PsychHID('KbQueueCreate') must be called before this routine.\n"
            "The optional 'deviceIndex' is the index of the HID input device whose queue should be flushed. "
            "If omitted, the queue of the default device will be flushed.\n";

static char seeAlsoString[] = "KbQueueCreate, KbQueueStart, KbQueueStop, KbQueueCheck, KbQueueRelease";
 
PsychError PSYCHHIDKbQueueFlush(void) 
{
    int deviceIndex, flushType;
    
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));

    deviceIndex = -1;
    PsychCopyInIntegerArg(1, kPsychArgOptional, &deviceIndex);

	flushType = 1;
    PsychCopyInIntegerArg(2, kPsychArgOptional, &flushType);
	
	// Return current count of contained events pre-flush:
	PsychCopyOutDoubleArg(1, FALSE, (double) PsychHIDAvailEventBuffer(deviceIndex, (flushType & 4) ? 1 : 0));

    if (flushType & 1) PsychHIDOSKbQueueFlush(deviceIndex);
    if (flushType & 2) PsychHIDFlushEventBuffer(deviceIndex);

    return(PsychError_none);	
}
