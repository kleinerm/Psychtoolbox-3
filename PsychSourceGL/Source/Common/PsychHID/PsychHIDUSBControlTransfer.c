/*
 
  PsychSourceGL/Source/Common/PsychHID/PsychHIDUSBControlTransfer.c
 
  PROJECTS: PsychHID
  
  PLATFORMS:  All.
  
  AUTHORS:

	chrg@sas.upenn.edu	cgb
	
  HISTORY:

	4.4.2009	Created.

  TO DO:
  
*/

#include "PsychHID.h"

static char useString[] = "outData = PsychHID('USBControlTransfer', usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength, inData)";
//																	1		   2			  3		    4		5		6		 7
static char synopsisString[] = "Communicates with a USB device via the control endpoint, aka control transfer.\n"
							   "The results of out-transfers are returned in return argument 'outData' as a uint8 array.\n"
							   "'usbHandle' is the handle of the USB device to control. 'bmRequestType' is the type of "
							   "reqest: If bit 7 is set, this defines a transfer from device to host and 'outData' will be "
							   "filled with at most 'wLength' bytes received from the device. Otherwise it defines a transfer "
							   "from host to device and at most 'wLength' bytes will be transfered from 'inData' to "
							   "the device.\n"
							   "'bRequest' is the request id.\n"
							   "'wValue' and 'wIndex' are device- and request specific values. 'wLength' is the amount of "
							   "data to return at most on a out-transfer, or the amount of data provided for an in-transfer "
							   "in the optional uint8 vector 'inData'. 'inData' must have at least as many elements as the "
							   "value of 'wLength'! ";

static char seeAlsoString[] = "";

PsychError PSYCHHIDUSBControlTransfer(void) 
{
	PsychUSBDeviceRecord	*dev;
	int usbHandle, bmRequestType, bRequest, wValue, wIndex, wLength;
	int m, n, p, err;
	const int USB_OUTTRANSFER = 0x80;	// bmRequestType & USB_OUTTRANSFER? -> Receive data from device.
	psych_uint8 *buffer = NULL;
	char *name="",*description="";
	
	// Setup the help features.
	PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) {
		PsychGiveHelp();
		return PsychError_none;
	}
		
	// Make sure the correct number of input arguments is supplied.
	PsychErrorExit(PsychRequireNumInputArgs(6));
	PsychErrorExit(PsychCapNumInputArgs(7));
    PsychErrorExit(PsychCapNumOutputArgs(1));
	
	// Copy all input values.  The input data is interpreted as a byte array.
	PsychCopyInIntegerArg(1, TRUE, &usbHandle);
	PsychCopyInIntegerArg(2, TRUE, &bmRequestType);
	PsychCopyInIntegerArg(3, TRUE, &bRequest);
	PsychCopyInIntegerArg(4, TRUE, &wValue);
	PsychCopyInIntegerArg(5, TRUE, &wIndex);
	PsychCopyInIntegerArg(6, TRUE, &wLength);
	
	// Get 'dev'icerecord for handle: This will error-out if no such device open:
	dev = PsychHIDGetUSBDevice(usbHandle);
	
	// For out commands, we don't care about the input buffer argument to this function.  We just dynamically
	// create enough memory to hold the out result.  If we're performing an in command, check to see if an
	// input buffer was specified and grab a reference to it to pass to the actual control transfer function.
	// In commands without an input buffer are assumed to be requests that don't require a buffer to function.
	if (bmRequestType & USB_OUTTRANSFER) {
		if (wLength <= 0) {
			PsychErrorExitMsg(PsychError_user, "Argument wLength must be > 0 for an out command!");
		}
		
		// Allocate return buffer of sufficient size wLength:
		m = 1; n = wLength; p = 1;
		PsychAllocOutUnsignedByteMatArg(1, TRUE, m, n, p, &buffer);
	}
	else if (0 == (bmRequestType & USB_OUTTRANSFER)) {
		// Get the input buffer if it was specified.
		m=n=p=0;
		PsychAllocInUnsignedByteMatArg(7, FALSE, &m, &n, &p, &buffer);
		if (((m * n) > 0) && (p != 1)) PsychErrorExitMsg(PsychError_user, "Argument inData must be a 1D vector or 2D matrix of bytes! This is a 3D matrix!");
		
		// Is the input buffer at least as big as the provided wLength argument?
		if ((m * n) < wLength) PsychErrorExitMsg(PsychError_user, "Argument inData has less elements then provided wLength argument! This must match!");
	}
	else {
		PsychErrorExitMsg(PsychError_user, "Argument bmRequestType must be 0x40 or 0xC0 !");
	}
	
	// Make the actual control request.
	if ((err = PsychHIDOSControlTransfer(dev, (psych_uint8) bmRequestType, (psych_uint8) bRequest, (psych_uint16) wValue, (psych_uint16) wIndex, (psych_uint16) wLength, (void*) buffer)) != 0) {
		// Failed! err contains a non-zero system specific error code from the underlying OS:
		if (PSYCH_SYSTEM == PSYCH_OSX) {
            PsychHIDErrors(NULL, err, &name, &description); 
            printf("PsychHID-ERROR: Control transfer failed: Errorcode: %08x = '%s' [%s]\n\n", err, name, description);
        }
        
		PsychErrorExitMsg(PsychError_system, "The USB Control transfer failed.");
	}

	return PsychError_none;
}
