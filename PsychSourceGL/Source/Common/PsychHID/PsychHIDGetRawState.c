/*
  PsychToolbox3/Source/Common/PsychHID/PsychHIDGetRawState.c		
  
  PROJECTS: PsychHID only.
  
  PLATFORMS:  Only OS X so far.  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
      
  HISTORY:
  5/12/03  awi		Created.
  
  TO DO:
  

*/

#include "PsychHID.h"

static char useString[]= "elementState=PsychHID('RawState', deviceNumber, elementNumber)";
static char synopsisString[] = 
        "Return the immediate state of the specified element on the specified device.";
static char seeAlsoString[] = "";

 
PsychError PSYCHHIDGetRawState(void) 
{
    long 		elementValue;
    pRecDevice          deviceRecord;
    pRecElement		elementRecord;
    int			deviceIndex, elementIndex;
    	 

    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    
    PsychCopyInIntegerArg(1, TRUE, &deviceIndex);
    PsychCopyInIntegerArg(2, TRUE, &elementIndex);
    PsychHIDVerifyInit();
    deviceRecord= PsychHIDGetDeviceRecordPtrFromIndex(deviceIndex);
    elementRecord= PsychHIDGetElementRecordFromDeviceRecordAndElementIndex(deviceRecord, elementIndex);
    elementValue= HIDGetElementValue(deviceRecord, elementRecord);
    PsychCopyOutDoubleArg(1, FALSE, (double)elementValue);
    return(PsychError_none);	
}





