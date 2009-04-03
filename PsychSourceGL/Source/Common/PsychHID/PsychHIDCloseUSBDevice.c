/*
 *  PsychHIDGenericUSBOpen.c
 *  PsychToolbox
 *
 */

#include "PsychHID.h"

static char useString[] = "PsychHID('CloseUSBDevice', usbHandle)";
static char synopsisString[] = "Closes a USB specified by 'usbHandle'.";
static char seeAlsoString[] = "";

// Globals
extern PsychUSBDeviceRecord usbDeviceRecordBank[PSYCH_HID_MAX_GENERIC_USB_DEVICES];

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
	PsychErrorExit(PsychRequireNumInputArgs(1));
	
	// Grab the vendor and device IDs.
	PsychCopyInIntegerArg(1, TRUE, &usbHandle);
	
	// Make sure the USB handle is in range.
	if (usbHandle < 0 || usbHandle >= PSYCH_HID_MAX_GENERIC_USB_DEVICES) {
		PsychErrMsgTxt("(PSYCHHIDGenericUSBClose) usbHandle out of range.");
	}
	
	// Check to see if we're trying to close an unopened device.
	if (!usbDeviceRecordBank[usbHandle].valid) {
		PsychErrMsgTxt("(PSYCHHIDGenericUSBClose) Device associated with usbHandle already closed.");
	}
	
	// Close the device.
	PSYCHHIDOSCloseUSBDevice(usbHandle);
	
	// Make sure the device entry is invalidated.
	usbDeviceRecordBank[usbHandle].valid = 0;
	
	return PsychError_none;
}
