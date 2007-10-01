/*
	PsychtoolboxGL/Source/Common/PsychHID/PsychHIDKbQueueFlush.c		
  
	PROJECTS: 
	
		PsychHID only.
  
	PLATFORMS:  
	
		Only OS X for now.  
  
	AUTHORS:
	
		rwoods@ucla.edu		rpw 
      
	HISTORY:
		8/23/07  rpw        Created.
  
	NOTES:
	
		The routines PsychHIDKbQueueCreate, PsychHIDKbQueueStart, PsychHIDKbQueueCheck, PsychHIDKbQueueStop
		and PsychHIDKbQueueRelease comprise a replacement for PsychHIDKbCheck, providing the following
		advantages:
		
			1) Brief key presses that would be missed by PsychHIDKbCheck are reliably detected
			2) The times of key presses are recorded more accurately
			3) Key releases are also recorded
		
		Requires Mac OS X 10.3 or later. The Matlab wrapper functions (KbQueueCreate, KbQueueStart,
		KbQueueCheck, KbQueueStop and KbQueueRelease screen away Mac OS X 10.2 and earlier, and the C code 
		does nothing to verify the Mac OS X version.
		
		Only a single device can be monitored at any given time. The deviceNumber can be specified only
		in the call to PsychHIDKbQueueCreate. The other routines then relate to that specified device. If
		deviceNumber is not specified, the first device is the default (like PyschHIDKbCheck). If
		PsychHIDKbQueueCreate has not been called first, the other routines will generate an error 
		message. Likewise, if PsychHIDKbQueueRelease has been called more recently than PsychHIDKbQueueCreate,
		the other routines will generate error messages.
		
		It is acceptable to cal PsychHIDKbQueueCreate at any time (e.g., to switch to a new device) without
		calling PsychKbQueueRelease.
		
		PsychHIDKbQueueCreate:
			Creates the queue for the specified (or default) device number
			No events are delivered to the queue until PsychHIDKbQueueStart is called
			Can be called again at any time
			
		PsychHIDKbQueueStart:
			Starts delivering keyboard events from the specified device to the queue
			
		PsychHIDKbQueueStop:
			Stops delivery of new keyboard events from the specified device to the queue.
			Data regarding events already queued is not cleared and can be recovered by PsychHIDKbQueueCheck
			
		PsychHIDKbQueueCheck:
			Obtains data about keypresses on the specified device since the most recent call to
			this routine or to PsychHIDKbQueueStart
			
			Clears all currently scored events (unscored events may still be in the queue)
			
		PsychHIDKbQueueFlush:
			Flushes unscored events from the queue and zeros all previously scored events
			
		PsychHIDKbQueueRelease:
			Releases queue-associated resources; once called, PsychHIDKbQueueCreate must be invoked
			before using any of the other routines
			
			This routine is called automatically at clean-up and can be omitted at the potential expense of
			keeping memory allocated unnecesarily
				

		---

*/

#include "PsychHID.h"
#include "PsychHIDKbQueue.h"

static char useString[]= "PsychHID('KbQueueFlush')";
static char synopsisString[] = 
		"Flushes all scored and unscored keyboard events from a queue."
        "PsychHID('KbQueueCreate') must be called before this routine ";
        
static char seeAlsoString[] = "KbQueueCreate, KbQueueStart, KbQueueStop, KbQueueCheck, KbQueueRelease";

extern AbsoluteTime *psychHIDKbQueueFirstPress;
extern AbsoluteTime *psychHIDKbQueueFirstRelease;
extern AbsoluteTime *psychHIDKbQueueLastPress;
extern AbsoluteTime *psychHIDKbQueueLastRelease;
extern HIDDataRef hidDataRef;

extern pthread_mutex_t psychHIDKbQueueMutex;
 
PsychError PSYCHHIDKbQueueFlush(void) 
{
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(0));
    
	if(!hidDataRef || !psychHIDKbQueueFirstPress || !psychHIDKbQueueFirstRelease || !psychHIDKbQueueLastPress || !psychHIDKbQueueLastRelease){
		PsychErrorExitMsg(PsychError_user, "Queue has not been created.");
	}
	
	// Drain the queue of any unprocessed events
	{
		IOHIDEventStruct event;
		AbsoluteTime zeroTime= {0,0};
		
		while(  (*hidDataRef->hidQueueInterface)->getNextEvent(hidDataRef->hidQueueInterface, &event, zeroTime, 0) == kIOReturnSuccess){
			if ((event.longValueSize != 0) && (event.longValue != NULL)) free(event.longValue);
		}
	}
	
	pthread_mutex_lock(&psychHIDKbQueueMutex);

	// Zero out the scored values
	{
		int i;
		for(i=0; i<256; i++){
			
			psychHIDKbQueueFirstPress[i].hi=0;
			psychHIDKbQueueFirstPress[i].lo=0;

			psychHIDKbQueueFirstRelease[i].hi=0;
			psychHIDKbQueueFirstRelease[i].lo=0;

			psychHIDKbQueueLastPress[i].hi=0;
			psychHIDKbQueueLastPress[i].lo=0;

			psychHIDKbQueueLastRelease[i].hi=0;
			psychHIDKbQueueLastRelease[i].lo=0;
		}
	}
	pthread_mutex_unlock(&psychHIDKbQueueMutex);
    return(PsychError_none);	
}

