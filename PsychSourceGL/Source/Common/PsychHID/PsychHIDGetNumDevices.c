/*
  PsychToolbox3/Source/Common/PsychHID/PsychHIDGetNumDevices.c		
  
  PROJECTS: PsychHID only.
  
  PLATFORMS:  All
  
  AUTHORS:
  Allen.Ingling@nyu.edu             awi 
  mario.kleiner@tuebingen.mpg.de    mk
      
  HISTORY:
  4/29/03  awi		Created.
  
  TO DO:

*/

#include "PsychHID.h"

static char useString[]= "numberOfDevices=PsychHID('NumDevices')";
static char synopsisString[] = "Return the the number of USB HID devices connected to your computer.";
static char seeAlsoString[] = "";

PsychError PSYCHHIDGetNumDevices(void) 
{
    psych_uint32 numDevices;
    
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    	 
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));
    
    PsychHIDVerifyInit();
    numDevices=HIDCountDevices();
    PsychCopyOutDoubleArg(1, FALSE, (double)numDevices);
    
    return(PsychError_none);	
}
