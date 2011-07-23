/*
 
  PsychSourceGL/Source/Common/PsychHID/PsychHIDOpenUSBDevice.c
 
  PROJECTS: PsychHID
  
  PLATFORMS:  All.
  
  AUTHORS:

	chrg@sas.upenn.edu	cgb
	
  HISTORY:

	4.4.2009	Created.

  TO DO:
  
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
	PsychUSBSetupSpec deviceSpec;
	int deviceID, vendorID;
	int errcode;
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

	// Setup specification of wanted device:
	// So far we only match against vendorID and deviceID, but may want to extend this
	// to more options in the future. That's why its passed via a PsychUSBSetupSpec struct.
	// -> Can rather easily extend that struct (and corresponding open routines) with new
	// fields without major code rewrites in other places.
	deviceSpec.vendorID = vendorID;
	deviceSpec.deviceID = deviceID;

	// This is the index of the device configuration to choose:
	// MK: For now always config zero. Might want to make this default to zero but allow
	// usercode to select some other index?
	deviceSpec.configurationID = 0;

    // Make sure hid infrastructure is online:
    PsychHIDVerifyInit();

	// Try to open the device. This will init the device structure properly and
	// also set the valid flag to "active/open" if open succeeds:
	if (!PsychHIDOSOpenUSBDevice(usbDev, &errcode, &deviceSpec)) {
		// MK TODO: We don't set or use 'errcode' yet.
		PsychErrorExitMsg(PsychError_user, "Failed to open the specified type of generic USB device. Make sure it is plugged in or not already open.");
	}

	// Return device handle:
	PsychCopyOutDoubleArg(1, FALSE, (double) usbHandle);

	return PsychError_none;
}
