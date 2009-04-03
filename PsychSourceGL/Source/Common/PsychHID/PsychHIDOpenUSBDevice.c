/*
 *  PsychHIDGenericUSBOpen.c
 *  PsychToolbox
 *
 */

#include "PsychHID.h"

static char useString[] = "usbHandle = PsychHID('OpenUSBDevice', vendorID, deviceID)";
static char synopsisString[] = "Opens a generic USB device specified by 'vendorID' and 'deviceID'.  A handle to the device is returned.";
static char seeAlsoString[] = "";

// Globals
extern PsychUSBDeviceRecord usbDeviceRecordBank[PSYCH_HID_MAX_GENERIC_USB_DEVICES];

PsychError PSYCHHIDOpenUSBDevice(void) 
{
	int deviceID, vendorID, i, usbHandle = -1;
	PsychUSBDeviceRecord usbDev;
	
	// Setup the help features.
	PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) {
		PsychGiveHelp();
		return PsychError_none;
	}
	
	// Make sure the correct number of input arguments is supplied.
	PsychErrorExit(PsychRequireNumInputArgs(2));
	
	// Grab the vendor and device IDs.
	PsychCopyInIntegerArg(1, TRUE, &vendorID);
	PsychCopyInIntegerArg(2, TRUE, &deviceID);
	
	// Find the next available USB handle.
	for (i = 0; i < PSYCH_HID_MAX_GENERIC_USB_DEVICES; i++) {
		if (usbDeviceRecordBank[i].valid == 0) {
			usbHandle = i;
			break;
		}
	}
	
	// Make sure that an available handle was located.
	if (usbHandle == -1) {
		PsychErrMsgTxt("(PSYCHHIDGenericUSBOpen) Too many USB devices open.  Please close one before opening again.");
	}
	
	// Open the device.
	usbDev = PsychHIDOSOpenUSBDevice(vendorID, deviceID);
	if (usbDev.valid == 0) {
		PsychErrMsgTxt("(PSYCHHIDGenericUSBOpen) Failed to open the USB device.  Make sure it is plugged in or not already open.");
	}
	else {
		usbDeviceRecordBank[usbHandle] = usbDev;
	}
	
	PsychCopyOutDoubleArg(1, FALSE, (double)usbHandle);
	
	return PsychError_none;
}
