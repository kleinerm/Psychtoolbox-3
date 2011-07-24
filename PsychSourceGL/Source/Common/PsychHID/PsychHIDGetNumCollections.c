/*
  PsychToolbox3/Source/Common/PsychHID/PsychHIDGetNumCollections.c		
  
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

static char useString[]= "numberOfCollections=PsychHID('NumCollections', deviceNumber)";
static char synopsisString[] = 
        "Return the the number of collections on the specified USB HID device.  A collection is "
        "a named grouping of elements on a device."; 
static char seeAlsoString[] = "";


PsychError PSYCHHIDGetNumCollections(void) 
{
    int			deviceIndex;
    UInt32		numElements;
    pRecDevice		deviceRecord; 	
    	 
    //all subfunctions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));
    
    PsychCopyInIntegerArg(1, TRUE, &deviceIndex);
    PsychHIDVerifyInit();
    deviceRecord= PsychHIDGetDeviceRecordPtrFromIndex(deviceIndex);
    numElements=HIDCountDeviceElements(deviceRecord, kHIDElementTypeCollection);
    PsychCopyOutDoubleArg(1, FALSE, (double)numElements);
    
    return(PsychError_none);	
}

#endif
