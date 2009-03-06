/*
  PsychToolbox3/Source/Common/PsychHID/RegisterProject.c		
  
  PROJECTS: PsychHID
  
  PLATFORMS:  OSX
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
      
  HISTORY:
  4/16/03  awi		Created. 
  4/15/05  dgp		Added Get/SetReport. 
  8/23/07  rpw      Added PsychHIDKbQueue suite and PsychHIDKbTriggerWait

*/

#include "Psych.h"
#include "PsychHID.h"

// Tracker used to maintain references to open generic USB devices.
GENERIC_USB_TYPE g_GenericUSBTracker[PSYCH_HID_MAX_GENERIC_USB_DEVICES];

PsychError PsychModuleInit(void)
{
	InitializeSynopsis();  //first initialize the list of synopsis strings.

	//register the project function which is called when the module
	//is invoked with no arguments:
	PsychRegister(NULL,  &PsychDisplayPsychHIDSynopsis);

	//register the module name
	PsychErrorExit(PsychRegister("PsychHID", NULL));

	//register the module exit function
	PsychRegisterExit(&PsychHIDCleanup);
	
	// Make sure that the generic USB tracker is initialized to NULL.
	int i;
	for (i = 0; i < PSYCH_HID_MAX_GENERIC_USB_DEVICES; i++) {
		//printf("init usb %d\n", i);
		g_GenericUSBTracker[i] = NULL;
	}

	//register module subfunctions
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));
	PsychErrorExit(PsychRegister("NumDevices",  &PSYCHHIDGetNumDevices));
	PsychErrorExit(PsychRegister("NumElements",  &PSYCHHIDGetNumElements));
	PsychErrorExit(PsychRegister("Devices",  &PSYCHHIDGetDevices));
	PsychErrorExit(PsychRegister("Elements",  &PSYCHHIDGetElements));
	PsychErrorExit(PsychRegister("RawState",  &PSYCHHIDGetRawState));
	PsychErrorExit(PsychRegister("NumCollections",  &PSYCHHIDGetNumCollections));
	PsychErrorExit(PsychRegister("Collections",  &PSYCHHIDGetCollections));
	PsychErrorExit(PsychRegister("CalibratedState",  &PSYCHHIDGetCalibratedState));
	PsychErrorExit(PsychRegister("KbCheck",  &PSYCHHIDKbCheck));
	PsychErrorExit(PsychRegister("KbWait",  &PSYCHHIDKbWait));
	PsychErrorExit(PsychRegister("KbTriggerWait", &PSYCHHIDKbTriggerWait));
	PsychErrorExit(PsychRegister("KbQueueCreate", &PSYCHHIDKbQueueCreate));
	PsychErrorExit(PsychRegister("KbQueueStart", &PSYCHHIDKbQueueStart));
	PsychErrorExit(PsychRegister("KbQueueStop", &PSYCHHIDKbQueueStop));
	PsychErrorExit(PsychRegister("KbQueueCheck", &PSYCHHIDKbQueueCheck));
	PsychErrorExit(PsychRegister("KbQueueFlush", &PSYCHHIDKbQueueFlush));
	PsychErrorExit(PsychRegister("KbQueueRelease", &PSYCHHIDKbQueueRelease));
	PsychErrorExit(PsychRegister("GetReport",  &PSYCHHIDGetReport));
	PsychErrorExit(PsychRegister("ReceiveReports",  &PSYCHHIDReceiveReports));
	PsychErrorExit(PsychRegister("ReceiveReportsStop",  &PSYCHHIDReceiveReportsStop));
	PsychErrorExit(PsychRegister("GiveMeReports",  &PSYCHHIDGiveMeReports));
	PsychErrorExit(PsychRegister("SetReport",  &PSYCHHIDSetReport));
	PsychErrorExit(PsychRegister("OpenUSBDevice", &PSYCHHIDGenericUSBOpen));
	PsychErrorExit(PsychRegister("CloseUSBDevice", &PSYCHHIDGenericUSBClose));
	PsychErrorExit(PsychRegister("USBControlTransfer", &PSYCHHIDGenericUSBControlTransfer));

	PsychSetModuleAuthorByInitials("awi");
	PsychSetModuleAuthorByInitials("dgp");
	PsychSetModuleAuthorByInitials("mk");
	PsychSetModuleAuthorByInitials("rpw");
	PsychSetModuleAuthorByInitials("cgb");

	return(PsychError_none);
}
