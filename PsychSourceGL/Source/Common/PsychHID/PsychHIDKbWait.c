/*
	PsychtoolboxGL/Source/Common/PsychHID/PsychHIDWait.c		
  
	PROJECTS: 
	
		PsychHID only.
  
	PLATFORMS:  
	
		None. This function is dead.
  
	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 
		rwoods@ucla.edu				rpw
      
	HISTORY:
		6/5/03  awi		Created.
		4/7/05	awi		Added a wait to to the polling loop. 
		12/17/09 rpw	Added support for keypads
  
	NOTES:

*/

#include "PsychHID.h"
static char useString[]= "secs=PsychHID('KbWait', [deviceNumber])";
static char synopsisString[] = 
		"THIS FUNCTION IS NO LONGER SUPPORTED! USE KbWait INSTEAD!\n\n"
        "Scan a keyboard or keypad device and return wait for a keypress "
        "By default the first keyboard device (the one with the lowest device number) is "
        "scanned. If no keyboard is found, the first keypad device is "
        "scanned.  Optionally, the device number of any keyboard or keypad may be specified "
        "The PsychHID('KbWait') implements the KbCheck command a provided by the  OS 9 Psychtoolbox."
        "KbWait is defined in the OS X Psychtoolbox and invokes PsychHID('KbWait').  For backwards "
        "compatability with earlier Psychtoolbox we recommend using KbWait instead of PsychHID('KbWait'). ";
        
static char seeAlsoString[] = "";

PsychError PSYCHHIDKbWait(void) 
{
    PsychErrorExitMsg(PsychError_unimplemented, "This function is no longer supported by PsychHID(). Use KbWait instead.");
    return(PsychError_unimplemented);
}
