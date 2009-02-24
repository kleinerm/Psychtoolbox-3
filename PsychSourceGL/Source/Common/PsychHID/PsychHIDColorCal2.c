/*
 *  PsychHIDColorCal2.c
 *  PsychToolbox
 *
 *  Created by Christopher Broussard on 2/23/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "PsychHID.h"

static char useString[] = "output = PsychHID('ColorCal2', 'command', [varargin])";
static char synopsisString[] = "Controls the ColorCal 2 USB device.";
static char seeAlsoString[] = "";

// Global ColorCal 2 device object.
IOUSBDeviceInterface **g_ColorCal2Device = NULL;


PsychError PSYCHHIDColorCal2(void)
{
	char *command;
	
	// Setup the help features.
	PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) {
		PsychGiveHelp();
		return PsychError_none;
	}
	
	// Makes sure that the number of input arguments contains at least 1 command.
	PsychErrorExit(PsychRequireNumInputArgs(1));
	
	// Grab the command.
	PsychAllocInCharArg(1, TRUE, &command);
	
	// Open the ColorCal 2 device if it isn't already.
	if (g_ColorCal2Device == NULL) {
		if (PSYCHHIDColorCal2OpenDevice() == false) {
			PsychErrMsgTxt("Failed to open the ColorCal 2 device.  Make sure it is plugged in.");
		}
	}
	
	if (PsychMatch(command, "LEDOn")) {
		PsychHIDColorCal2MakeRequest(0x40, 2, 0, 0, NULL);
	}
	else {
		PsychErrMsgTxt("Invalid command");
	}
		
	
	return PsychError_none;
}
