/*
	PsychtoolboxGL/Source/Common/PsychHID/PsychHIDKbTriggerWait.c		
  
	PROJECTS: 
	
		PsychHID only.
  
	PLATFORMS:  
	
		All.
  
	AUTHORS:
	
		rwoods@ucla.edu                     rpw
        mario.kleiner@tuebingen.mpg.de      mk
      
	HISTORY:
		8/9/07  rpw		Created.
		12/17/09 rpw	Added keypad support
  
	NOTES:
	
		This incorporates elements from PsychHIDKbCheck and PsychHIDKbWait, but is distinct in that:
			1) It waits for a specific trigger key rather than any keypress
			2) It uses a queue to check for the trigger rather than simply checking to see if the key is down
			
		The use of a queue assures that brief trigger events will not be missed by virtue of being too brief

		This routine will only listen to a single device. It would be possible to design an analogous routine
		that would listen for a particular trigger key (or set of trigger keys) on multiple devices, but 
		this would involve setting up a queue for each device and then polling each device in turn in the 
		loop that watches for the trigger (or setting callback functions).
		
		By nature, the oldest events are discarded from the queue once it reaches its full capacity.
		Consequently, the time returned by this routine is not guaranteed to be the time of the first
		event if multiple events arrive in between pollings. However, this is probably still more accurate
		than polling for the key being down. If it is desirable to guarantee that the very first event is
		the one used to extract the timestamp, it would be best to set an event callout on the queue.
		Short of this, setting the queue depth to a value larger than the maximum expected number of events
		(if this number is known) when creating the queue would suffice.
		
		If one wanted to call a function like this repeatedly with the same trigger while minimizing
		the computation time associated with setting up the queue, this funcion could be rewritten
		to declare queue static, adding an input argument to govern creating and setting up the queue, 
		starting the queue, flushing the queue, polling the queue and destroying the queue, but this would put
		the burden for remembering to destroy the queue back on the Matlab user.

		---
    

*/

#include "PsychHID.h"

static char useString[]= "secs=PsychHID('KbTriggerWait', KeysUsage, [deviceNumber])";
static char synopsisString[] = 
        "Scan a keyboard, keypad, or other HID device with buttons, and wait for a trigger key press.\n"
        "NOTE: This function should not be called directly! Use KbTriggerWait() instead.\n"
        "By default the first keyboard device (the one with the lowest device number) is "
        "scanned. If no keyboard is found, the first keypad device is scanned, followed by other "
        "devices, e.g., mice.  Optionally, the deviceNumber of any keyboard or HID device may be specified.\n"
        "The 'KeysUsage' parameter must specify the keycode of a single key to wait for on "
        "OS/X. On Linux and Windows, 'KeysUsage' can be a vector of trigger key codes and the "
        "wait will finish as soon as at least one of the keys in the vector is pressed.\n"
        "On MS-Windows XP and later, it is currently not possible to enumerate different keyboards and mice "
        "separately. Therefore the 'deviceNumber' argument is mostly useless for keyboards and mice, usually you can "
        "only check the system keyboard or mouse.\n"
        "OSX: This function is unsupported. You must use KbTriggerWait() instead.\n";

static char seeAlsoString[] = "";
 
PsychError PSYCHHIDKbTriggerWait(void) 
{
    int	deviceIndex;
    int numScankeys;
    int* scanKeys;
    
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));  	//Specify trigger key code and the deviceNumber of the keyboard or keypad to scan.  

	// Identify the mandatory trigger array:
    if (!PsychAllocInIntegerListArg(1, TRUE, &numScankeys, &scanKeys)){
        PsychErrorExitMsg(PsychError_user, "Keycode is required.");
    }
	
    // Get optional deviceIndex:
    if (!PsychCopyInIntegerArg(2, FALSE, &deviceIndex)) deviceIndex = -1;

    // Execute:
    PsychHIDOSKbTriggerWait(deviceIndex, numScankeys, scanKeys);

    return(PsychError_none);
}
