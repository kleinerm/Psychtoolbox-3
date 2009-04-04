/*
 *  PsychHIDGenericUSBControlTransfer.c
 *  PsychToolbox
 *
 *  Created by Christopher Broussard on 3/2/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "PsychHID.h"

static char useString[] = "outData = PsychHID('USBControlTransfer', usbHandle, bmRequestType, wValue, wIndex, wLength, inData)";
//																	1		   2			  3		  4		  5		   6
static char synopsisString[] = "Communicates with a USB device via the control endpoint.  Returns the data array result from the operation.";
static char seeAlsoString[] = "";

PsychError PSYCHHIDUSBControlTransfer(void) 
{
	PsychUSBDeviceRecord	*dev;
	int usbHandle, bmRequestType, wValue, wIndex, wLength, dataElementWidth;
	int m, n, p;
	ubyte *buffer = NULL;
	
	// Setup the help features.
	PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) {
		PsychGiveHelp();
		return PsychError_none;
	}
		
	// Make sure the correct number of input arguments is supplied.
	PsychErrorExit(PsychRequireNumInputArgs(5));
	PsychErrorExit(PsychCapNumInputArgs(6));
    PsychErrorExit(PsychCapNumOutputArgs(1));
	
	// Copy all input values.  The input data is interpreted as a byte array.
	PsychCopyInIntegerArg(1, TRUE, &usbHandle);
	PsychCopyInIntegerArg(2, TRUE, &bmRequestType);
	PsychCopyInIntegerArg(3, TRUE, &wValue);
	PsychCopyInIntegerArg(4, TRUE, &wIndex);
	PsychCopyInIntegerArg(5, TRUE, &wLength);
	
	// Get 'dev'icerecord for handle: This will error-out if no such device open:
	dev = PsychHIDGetUSBDevice(usbHandle);
	
	// For out commands, we don't care about the input buffer argument to this function.  We just dynamically
	// create enough memory to hold the out result.  If we're performing an in command, check to see if an
	// input buffer was specified and grab a reference to it to pass to the actual control transfer function.
	// In commands without an input buffer are assumed to be requests that don't require a buffer to function.
	if (bmRequestType == 0xC0) {
		if (wLength <= 0) {
			PsychErrorExitMsg(PsychError_user, "Argument wLength must be > 0 for an out command!");
		}
		
		buffer = (ubyte*)mxMalloc(wLength);
		m = 1; n = wLength; p = 1;
	}
	else if (bmRequestType == 0x40) {
		// Get the input buffer if it was specified.
		if (PsychGetNumInputArgs() == 6) {
			PsychAllocInUnsignedByteMatArg(6, TRUE, &m, &n, &p, &buffer);
		}
	}
	else {
		PsychErrorExitMsg(PsychError_user, "Argument bmRequestType must be 0x40 or 0xC0 !");
	}
	
	// Make the actual control request.
	if (PsychHIDControlTransfer(dev, (psych_uint8) bmRequestType, (psych_uint16) wValue, (psych_uint16) wIndex, (psych_uint16) wLength, (void*) buffer) == false) {
		PsychErrorExitMsg(PsychError_system, "The USB Control transfer failed.");
	}
		
	// Copy the buffer to output if not NULL and we've run an out request. 
	if ((buffer != NULL) && (bmRequestType == 0xC0)) {
		int i;
		ubyte *outBuffer;
		mxArray **mxpp;
		
		// Copy the buffer to the Matlab output.  For some reason the Psych function that allocates
		// a byte matrix was causing memory problems in Matlab, so I just took the guts out of it and
		// put them here.  Now things seems to be OK.
		mxpp = PsychGetOutArgMxPtr(1);
		*mxpp = mxCreateByteMatrix3D(m, n, p);
		outBuffer = (ubyte*)mxGetData(*mxpp);
		for (i = 0; i < n; i++) {
			//printf("char %d: %c\n", i, (char)buffer[i]);
			outBuffer[i] = buffer[i];
		}
	}
	
	return PsychError_none;
}
