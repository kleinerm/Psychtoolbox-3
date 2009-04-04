/*
 *  PsychHIDGenericUSBOpen.c
 *  PsychToolbox
 *
 */

#include "PsychHID.h"

static char useString[] = "usbHandle = PsychHID('OpenUSBDevice', vendorID, deviceID)";
//																 1		   2
static char synopsisString[] = "Tries to open and initialize a generic USB device specified by 'vendorID' and 'deviceID'.\n"
							   "On success, a 'usbHandle' to the opened device is returned.\n"
							   "'vendorID' and 'deviceID' must be numeric (integer) values which identify the "
							   "target device by the official vendor id of the device manufacturer, and the "
							   "device id of the specific model of a device. ";
static char seeAlsoString[] = "";

PsychError PSYCHHIDOpenUSBDevice(void) 
{
	int deviceID, vendorID;
	int usbHandle = -1;
	PsychUSBDeviceRecord *usbDev = NULL;
	
	// Setup the help features.
	PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) {
		PsychGiveHelp();
		return PsychError_none;
	}
	
	// Make sure the correct number of input arguments is supplied.
    PsychErrorExit(PsychCapNumInputArgs(2));
	PsychErrorExit(PsychRequireNumInputArgs(2));
    PsychErrorExit(PsychCapNumOutputArgs(1));
	
	// Grab the mandatory vendor and device IDs.
	PsychCopyInIntegerArg(1, TRUE, &vendorID);
	PsychCopyInIntegerArg(2, TRUE, &deviceID);

	// Try to get free slot in internal device bank: This will error-exit if no capacity left.
	usbDev = PsychHIDGetFreeUSBDeviceSlot(&usbHandle);

	// Try to open the device. This will init the device structure properly and
	// also set the valid flag to "active/open" if open succeeds:
	if (!PsychHIDOSOpenUSBDevice(usbDev, vendorID, deviceID)) {
		PsychErrorExitMsg(PsychError_user, "Failed to open the specified type of generic USB device. Make sure it is plugged in or not already open.");
	}

	// Return device handle:
	PsychCopyOutDoubleArg(1, FALSE, (double) usbHandle);

	return PsychError_none;
}
