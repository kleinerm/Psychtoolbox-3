/*
  PsychToolbox3/Source/Common/PsychHID/PsychHIDGetNumElements.c		
  
  PROJECTS: PsychHID only.
  
  PLATFORMS:  Only OS X so far.  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
      
  HISTORY:
  4/29/03  awi		Created.
  
  TO DO:

*/

#include "PsychHID.h"

#if PSYCH_SYSTEM == PSYCH_OSX

static char useString[]= "numberOfElements=PsychHID('NumElements', deviceNumber)";
static char synopsisString[] = 
        "Return the the number of elements on the specified USB HID device.";
static char seeAlsoString[] = "";

PsychError PSYCHHIDGetNumElements(void) 
{
    int			deviceIndex;
    UInt32		numElements;
    pRecDevice		deviceRecord; 	
    	 
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));
    
    PsychCopyInIntegerArg(1, TRUE, &deviceIndex);
    PsychHIDVerifyInit();
    deviceRecord= PsychHIDGetDeviceRecordPtrFromIndex(deviceIndex);
    numElements=HIDCountDeviceElements(deviceRecord, kHIDElementTypeIO);
    PsychCopyOutDoubleArg(1, FALSE, (double)numElements);
    
    return(PsychError_none);	
}

#endif
