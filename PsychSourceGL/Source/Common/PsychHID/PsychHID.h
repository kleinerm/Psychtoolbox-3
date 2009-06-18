	/*
	PsychToolbox3/Source/Common/PsychHID/PsychHID.h		

	PROJECTS: PsychHID

	PLATFORMS:  OSX 

	AUTHORS:

	Allen.Ingling@nyu.edu		awi 
	  
	HISTORY:
		4/29/03  awi		Created.
		3/15/05  dgp        Added Get/SetReport
		3/15/05  dgp        Added missing prototypes from PsychHIDHelpers.c to make compiler happy.
        4/3/05   dgp        Added prototype for PsychHIDReceiveReportsCleanup.
		8/23/07  rpw		Added prototypes for PsychHIDKbTriggerWait and PsychHIDKbQueue suite.

	TO DO:

		DONE	1- Emulate KbCheck
				2- Add Queues
		DONE	3- Have structure returned b GetCollections include list of constituent elements
				4- Try outputs, such as force feedback joysticks
				5- Accept structs as input args as alternative to  device, element or collection indices.
				6- Report vendor-specific usages. 
				7- Return logical arrays
				8- Maintain an internal queue beyond the 50-elements stored by the kernel 
				9- KbCheck could read any device with elements of type button.
        


*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychHID
#define PSYCH_IS_INCLUDED_PsychHID

//project includes
#include "Psych.h" 
#include "PsychTimeGlue.h"

// OS/X specific includes:
#if PSYCH_SYSTEM == PSYCH_OSX

// Includes for Apple's HID utilities which are layered above the HID device interface.  These are taken from 
// Apple's "HID Explorer" example and used for all generic USB-HID interface support:
#include <Carbon/Carbon.h>
#include <IOKit/HID/IOHIDKeys.h>
#include "HID_Utilities_External.h"

// Required for generic USB device support:
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <mach/mach.h>
#include <string.h>

#endif

// Structure which carries all required setup and matching parameters for
// finding, opening and configuring a generic USB device. This is passed
// to PsychHIDOSOpenUSBDevice(); to define what device should be opened,
// and with what parameters. It can be easily extended in the future:
struct PsychUSBSetupSpec_Struct {
	int vendorID;			// USB vendor ID.
	int deviceID;			// USB device ID / product ID.
	// More specs go here...
	int configurationID;	// Index of USB configuration to select when configuring the device.
};

typedef struct PsychUSBSetupSpec_Struct PsychUSBSetupSpec;

// Structure to keep track of a generic USB device:
struct PsychUSBDeviceRecord_Struct {
    int    valid;    // 0 = Unused/Free device record, 1 = Active device record.

	// OS-Specific parts of the struct:
	
	// OS/X stuff:
	#if PSYCH_SYSTEM == PSYCH_OSX
		IOUSBDeviceInterface**    device;            // Actual low-level device specific pointer for OS/X.
	#endif
};

typedef struct PsychUSBDeviceRecord_Struct PsychUSBDeviceRecord;

// Define constants for use by PsychHID files. 
#define PSYCH_HID_MAX_DEVICES								256
#define PSYCH_HID_MAX_DEVICE_ELEMENT_TYPE_NAME_LENGTH 		256
#define PSYCH_HID_MAX_DEVICE_ELEMENT_USAGE_NAME_LENGTH		256
#define PSYCH_HID_MAX_DEVICE_ELEMENTS						1024
#define PSYCH_HID_MAX_KEYBOARD_DEVICES						64
#define PSYCH_HID_MAX_GENERIC_USB_DEVICES					64

// Function prototypes for module subfunctions.
PsychError MODULEVersion(void);						// MODULEVersion.c 
PsychError PSYCHHIDGetNumDevices(void);				// PSYCHHIDGetNumDevices.c
PsychError PSYCHHIDGetNumElements(void);			// PSYCHHIDGetNumElements.c
PsychError PSYCHHIDGetNumCollections(void);			// PSYCHHIDGetNumCollections.c
PsychError PSYCHHIDGetDevices(void);				// PsychHIDGetDeviceList.c 
PsychError PSYCHHIDGetElements(void);				// PsychHIDGetElementList.c
PsychError PSYCHHIDGetRawState(void);				// PsychHIDGetRawElementState.c
PsychError PSYCHHIDGetCalibratedState(void);		// PsychHIDGetCalibratedState.c
PsychError PSYCHHIDGetCollections(void);			// PsychHIDGetCollections.c
PsychError PSYCHHIDKbCheck(void);					// PsychHIDKbCheck.c
PsychError PSYCHHIDKbWait(void);					// PsychHIDKbWait.c 
PsychError PSYCHHIDKbTriggerWait(void);				// PsychTriggerWait.c
PsychError PSYCHHIDKbQueueCreate(void);				// PsychHIDKbQueueCreate.c
PsychError PSYCHHIDKbQueueStart(void);				// PsychHIDKbQueueStart.c
PsychError PSYCHHIDKbQueueStop(void);				// PsychHIDKbQueueStop.c
PsychError PSYCHHIDKbQueueCheck(void);				// PsychHIDKbQueueCheck.c
PsychError PSYCHHIDKbQueueFlush(void);				// PsychHIDKbQueueFlush.c
PsychError PSYCHHIDKbQueueRelease(void);			// PsychHIDKbQueueRelease.c
PsychError PSYCHHIDGetReport(void);					// PsychHIDGetReport.c
PsychError PSYCHHIDSetReport(void);					// PsychHIDSetReport.c
PsychError PSYCHHIDReceiveReports(void);			// PsychHIDReceiveReports.c
PsychError PSYCHHIDReceiveReportsStop(void);		// PsychHIDReceiveReportsStop.c
PsychError PSYCHHIDGiveMeReports(void);				// PsychHIDGiveMeReports.c
PsychError PSYCHHIDOpenUSBDevice(void);				// PSYCHHIDOpenUSBDevice.c
PsychError PSYCHHIDCloseUSBDevice(void);			// PSYCHHIDCloseUSBDevice.c
PsychError PSYCHHIDUSBControlTransfer(void);		// PSYCHHIDUSBControlTransfer.c

//internal function protototypes
PsychError PsychHIDReceiveReportsCleanup(void); // PsychHIDReceiveReports.c
PsychError ReceiveReports(int deviceIndex); // PsychHIDReceiveReports.c
PsychError GiveMeReport(int deviceIndex,psych_bool *reportAvailablePtr,unsigned char *reportBuffer,UInt32 *reportBytesPtr,double *reportTimePtr); // PsychHIDReceiveReports.c
PsychError GiveMeReports(int deviceIndex,int reportBytes); // PsychHIDReceiveReports.c
PsychError	ReceiveReportsStop(int deviceIndex);
PsychError  PsychHIDReceiveReportsCleanup(void); // PsychHIDReceiveReportsCleanup.c
PsychError 	PsychHIDCleanup(void);												// PsychHIDHelpers.c 
void 		PsychHIDVerifyInit(void);											// PsychHIDHelpers.c 
pRecDevice 	PsychHIDGetDeviceRecordPtrFromIndex(int deviceIndex);								// PsychHIDHelpers.c 
int 		PsychHIDGetIndexFromRecord(pRecDevice deviceRecord, pRecElement elementRecord, HIDElementTypeMask typeMask);	// PsychHIDHelpers.c 
psych_bool 	PsychHIDCheckOpenDeviceInterfaceFromDeviceIndex(int deviceIndex);						// PsychHIDHelpers.c 
psych_bool 	PsychHIDCheckOpenDeviceInterfaceFromDeviceRecordPtr(pRecDevice deviceRecord);					// PsychHIDHelpers.c 
void 		PsychHIDVerifyOpenDeviceInterfaceFromDeviceRecordPtr(pRecDevice deviceRecord);					// PsychHIDHelpers.c 
void 		PsychHIDGetTypeMaskStringFromTypeMask(HIDElementTypeMask maskValue, char **pStr);				// PsychHIDHelpers.c
pRecElement 	PsychHIDGetElementRecordFromDeviceRecordAndElementIndex(pRecDevice deviceRecord, int elementIndex);		// PsychHIDHelpers.c
pRecElement 	PsychHIDGetCollectionRecordFromDeviceRecordAndCollectionIndex(pRecDevice deviceRecord, int elementIndex);	// PsychHIDHelpers.c
int 		PsychHIDCountCollectionElements(pRecElement collectionRecord, HIDElementTypeMask elementTypeMask);		// PsychHIDHelpers.c
int 		PsychHIDFindCollectionElements(pRecElement collectionRecord, HIDElementTypeMask elementTypeMask, pRecElement *collectionMembers, int maxListElements);  // PsychHIDHelpers.c
void 		PsychHIDGetDeviceListByUsage(long usagePage, long usage, int *numDeviceIndices, int *deviceIndices, pRecDevice *deviceRecords);  //// PsychHIDHelpers.c
void InitializeSynopsis();
PsychError PsychDisplayPsychHIDSynopsis(void);
psych_bool PsychHIDQueryOpenDeviceInterfaceFromDeviceIndex(int deviceIndex);	// PsychHIDHelpers.c
psych_bool PsychHIDQueryOpenDeviceInterfaceFromDeviceRecordPtr(pRecDevice deviceRecord);	// PsychHIDHelpers.c
void PsychHIDVerifyOpenDeviceInterfaceFromDeviceIndex(int deviceIndex);	// PsychHIDHelpers.c
int PsychHIDErrors(int error,char **namePtr,char **descriptionPtr);
void PsychInitializePsychHID(void); // PsychHIDHelpers.c
void PsychHIDCloseAllUSBDevices(void);
PsychUSBDeviceRecord* PsychHIDGetFreeUSBDeviceSlot(int* usbHandle);
PsychUSBDeviceRecord* PsychHIDGetUSBDevice(int usbHandle);

// These must be defined for each OS in their own PsychHIDGenericUSBSupport.c.
psych_bool PsychHIDOSOpenUSBDevice(PsychUSBDeviceRecord* devRecord, int* errorcode, PsychUSBSetupSpec* spec);
void PsychHIDOSCloseUSBDevice(PsychUSBDeviceRecord* devRecord);
int PsychHIDOSControlTransfer(PsychUSBDeviceRecord* devRecord, psych_uint8 bmRequestType, psych_uint8 bRequest, psych_uint16 wValue, psych_uint16 wIndex, psych_uint16 wLength, void *pData);

//end include once
#endif
