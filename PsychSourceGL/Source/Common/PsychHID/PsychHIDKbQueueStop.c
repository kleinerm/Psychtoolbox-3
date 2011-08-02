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

static char useString[]= "PsychHID('KbQueueStop' [, deviceIndex])";
static char synopsisString[] = 
		"Stops sending keypresses to a queue.\n"
        "PsychHID('KbQueueCreate') must be called before this routine.\n"
        "On Linux, the optional 'deviceIndex' is the index of the keyboard device whose queue should be stopped. "
        "If omitted, the default keyboard's queue will be stopped. On other systems, the last queue will be stopped.\n";
        
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

#if PSYCH_SYSTEM == PSYCH_OSX

#include "PsychHIDKbQueue.h"
extern HIDDataRef hidDataRef;

void PsychHIDOSKbQueueStop(int deviceIndex)
{
	if(!hidDataRef){
		PsychErrorExitMsg(PsychError_user, "Queue has not been created.");
	}
	{
		IOHIDQueueInterface **queue=(hidDataRef->hidQueueInterface);
		if(!queue){
			PsychErrorExitMsg(PsychError_user, "Queue has not been created.");
		}
		{
			// Stop the queue
			HRESULT result = (*queue)->stop(queue);
			if (kIOReturnSuccess != result){
				PsychErrorExitMsg(PsychError_system, "Failed to stop event queues.");
			}
		}
	}
}

#endif
