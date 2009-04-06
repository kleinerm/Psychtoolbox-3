/*
 
  PsychSourceGL/Source/Common/PsychHID/PsychHIDCloseUSBDevice.c

  PROJECTS: PsychHID
  
  PLATFORMS:  All.
  
  AUTHORS:

	chrg@sas.upenn.edu	cgb
	
  HISTORY:

	4.4.2009	Created.

  TO DO:
  
*/

#include "PsychHID.h"

static char useString[] = "PsychHID('CloseUSBDevice' [, usbHandle])";
//														1
static char synopsisString[] = "Closes a generic USB device specified by 'usbHandle'. "
							   "Closes all generic USB devices if 'usbHandle' is omitted. ";
static char seeAlsoString[] = "";

PsychError PSYCHHIDCloseUSBDevice(void)
{
	int usbHandle;
	
	// Setup the help features.
	PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) {
		PsychGiveHelp();
		return PsychError_none;
	}
	
	// Make sure the correct number of input arguments is supplied.
    PsychErrorExit(PsychCapNumInputArgs(1));
	PsychErrorExit(PsychRequireNumInputArgs(0));
    PsychErrorExit(PsychCapNumOutputArgs(0));
	
	// Grab the optional device handle:
	if (PsychCopyInIntegerArg(1, FALSE, &usbHandle)) {
		// Specific device given. Try to close it. This will error-out if no such
		// device is open:
		PsychHIDOSCloseUSBDevice(PsychHIDGetUSBDevice(usbHandle));
	}
	else {
		// No specific handle given: Close and release all open generic USB devices:
		PsychHIDCloseAllUSBDevices();
	}

	return PsychError_none;
}
