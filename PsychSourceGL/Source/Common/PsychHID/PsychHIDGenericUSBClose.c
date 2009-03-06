/*
 *  PsychHIDGenericUSBOpen.c
 *  PsychToolbox
 *
 */

#include "PsychHID.h"

static char useString[] = "PsychHID('CloseUSBDevice', usbHandle)";
static char synopsisString[] = "Closes a USB specified by 'usbHandle'.";
static char seeAlsoString[] = "";

extern GENERIC_USB_TYPE g_GenericUSBTracker[PSYCH_HID_MAX_GENERIC_USB_DEVICES];

PsychError PSYCHHIDGenericUSBClose(void)
{
	int usbHandle;
	
	// Setup the help features.
	PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) {
		PsychGiveHelp();
		return PsychError_none;
	}
	
	// Make sure the correct number of input arguments is supplied.
	PsychErrorExit(PsychRequireNumInputArgs(1));
	
	// Grab the vendor and device IDs.
	PsychCopyInIntegerArg(1, TRUE, &usbHandle);
	
	// Make sure the USB handle is in range.
	if (usbHandle < 0 || usbHandle >= PSYCH_HID_MAX_GENERIC_USB_DEVICES) {
		PsychErrMsgTxt("(PSYCHHIDGenericUSBClose) usbHandle out of range.");
	}
	
	// Check to see if we're trying to close an unopened device.
	if (g_GenericUSBTracker[usbHandle] == NULL) {
		PsychErrMsgTxt("(PSYCHHIDGenericUSBClose) Device associated with usbHandle already closed.");
	}
	
	// Close the device.
	PSYCHHIDCloseUSBDevice(usbHandle);
	
	// Make the reference in the tracker NULL so we know it's closed.
	g_GenericUSBTracker[usbHandle] = NULL;
	
	return PsychError_none;
}
