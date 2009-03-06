/*
 *  PsychHIDGenericUSBOpen.c
 *  PsychToolbox
 *
 */

#include "PsychHID.h"

static char useString[] = "usbHandle = PsychHID('OpenUSBDevice', vendorID, deviceID)";
static char synopsisString[] = "Opens a generic USB device specified by 'vendorID' and 'deviceID'.  A handle to the device is returned.";
static char seeAlsoString[] = "";

extern GENERIC_USB_TYPE g_GenericUSBTracker[PSYCH_HID_MAX_GENERIC_USB_DEVICES];

PsychError PSYCHHIDGenericUSBOpen(void) 
{
	int deviceID, vendorID, i, usbHandle = -1;
	GENERIC_USB_TYPE usbDevPointer;
	
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
		if (g_GenericUSBTracker[i] == NULL) {
			usbHandle = i;
			break;
		}
	}
	
	// Make sure that an available handle was located.
	if (usbHandle == -1) {
		PsychErrMsgTxt("(PSYCHHIDGenericUSBOpen) Too many USB devices open.  Please close one before opening again.");
	}
	
	// Open the device.
	usbDevPointer = (GENERIC_USB_TYPE)PSYCHHIDOpenUSBDevice(vendorID, deviceID);
	if (usbDevPointer == NULL) {
		PsychErrMsgTxt("(PSYCHHIDGenericUSBOpen) Failed to open the USB device.  Make sure it is plugged in or not already open.");
	}
	else {
		g_GenericUSBTracker[usbHandle] = usbDevPointer;
	}
	
	PsychCopyOutDoubleArg(1, FALSE, (double)usbHandle);
	
	return PsychError_none;
}
