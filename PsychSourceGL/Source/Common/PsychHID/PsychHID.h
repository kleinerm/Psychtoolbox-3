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


//includes for Apple's HID utilities which lies above the HID device interface.  These are taken from 
//Apple's "HID Explorer" example

/*
#include <IOKit/hid/IOHIDLib.h>
*/


#include <Carbon/Carbon.h>
#include <IOKit/HID/IOHIDKeys.h>
#include "HID_Utilities_External.h"



//Define constants for use by PsychHID files. 
#define PSYCH_HID_MAX_DEVICES					256
#define PSYCH_HID_MAX_DEVICE_ELEMENT_TYPE_NAME_LENGTH 		256
#define PSYCH_HID_MAX_DEVICE_ELEMENT_USAGE_NAME_LENGTH		256
#define PSYCH_HID_MAX_DEVICE_ELEMENTS				1024
#define PSYCH_HID_MAX_KEYBOARD_DEVICES				64	
	

//function prototypes for module subfunctions.
PsychError MODULEVersion(void);					// MODULEVersion.c 
PsychError PSYCHHIDGetNumDevices(void);				// PSYCHHIDGetNumDevices.c
PsychError PSYCHHIDGetNumElements(void);			// PSYCHHIDGetNumElements.c
PsychError PSYCHHIDGetNumCollections(void);			// PSYCHHIDGetNumCollections.c
PsychError PSYCHHIDGetDevices(void);				// PsychHIDGetDeviceList.c 
PsychError PSYCHHIDGetElements(void);				// PsychHIDGetElementList.c
PsychError PSYCHHIDGetRawState(void);				// PsychHIDGetRawElementState.c
PsychError PSYCHHIDGetCalibratedState(void);		// PsychHIDGetCalibratedState.c
PsychError PSYCHHIDGetCollections(void);			// PsychHIDGetCollections.c
PsychError PSYCHHIDKbCheck(void);				// PsychHIDKbCheck.c
PsychError PSYCHHIDKbWait(void);				// PsychHIDKbWait.c 
PsychError PSYCHHIDGetReport(void);				// PsychHIDGetReport.c
PsychError PSYCHHIDSetReport(void);				// PsychHIDSetReport.c
PsychError PSYCHHIDReceiveReports(void);		// PsychHIDReceiveReports.c
PsychError PSYCHHIDReceiveReportsStop(void);	// PsychHIDReceiveReportsStop.c
PsychError PSYCHHIDGiveMeReports(void);			// PsychHIDGiveMeReports.c


//internal function protototypes
PsychError PsychHIDReceiveReportsCleanup(void); // PsychHIDReceiveReports.c
PsychError ReceiveReports(int deviceIndex); // PsychHIDReceiveReports.c
PsychError GiveMeReport(int deviceIndex,boolean *reportAvailablePtr,unsigned char *reportBuffer,UInt32 *reportBytesPtr,double *reportTimePtr); // PsychHIDReceiveReports.c
PsychError GiveMeReports(int deviceIndex,int reportBytes); // PsychHIDReceiveReports.c
PsychError	ReceiveReportsStop(int deviceIndex);
PsychError  PsychHIDReceiveReportsCleanup(void); // PsychHIDReceiveReportsCleanup.c
PsychError 	PsychHIDCleanup(void);												// PsychHIDHelpers.c 
void 		PsychHIDVerifyInit(void);											// PsychHIDHelpers.c 
pRecDevice 	PsychHIDGetDeviceRecordPtrFromIndex(int deviceIndex);								// PsychHIDHelpers.c 
int 		PsychHIDGetIndexFromRecord(pRecDevice deviceRecord, pRecElement elementRecord, HIDElementTypeMask typeMask);	// PsychHIDHelpers.c 
boolean 	PsychHIDCheckOpenDeviceInterfaceFromDeviceIndex(int deviceIndex);						// PsychHIDHelpers.c 
boolean 	PsychHIDCheckOpenDeviceInterfaceFromDeviceRecordPtr(pRecDevice deviceRecord);					// PsychHIDHelpers.c 
void 		PsychHIDVerifyOpenDeviceInterfaceFromDeviceRecordPtr(pRecDevice deviceRecord);					// PsychHIDHelpers.c 
void 		PsychHIDGetTypeMaskStringFromTypeMask(HIDElementTypeMask maskValue, char **pStr);				// PsychHIDHelpers.c
pRecElement 	PsychHIDGetElementRecordFromDeviceRecordAndElementIndex(pRecDevice deviceRecord, int elementIndex);		// PsychHIDHelpers.c
pRecElement 	PsychHIDGetCollectionRecordFromDeviceRecordAndCollectionIndex(pRecDevice deviceRecord, int elementIndex);	// PsychHIDHelpers.c
int 		PsychHIDCountCollectionElements(pRecElement collectionRecord, HIDElementTypeMask elementTypeMask);		// PsychHIDHelpers.c
int 		PsychHIDFindCollectionElements(pRecElement collectionRecord, HIDElementTypeMask elementTypeMask, pRecElement *collectionMembers, int maxListElements);  // PsychHIDHelpers.c
void 		PsychHIDGetDeviceListByUsage(long usagePage, long usage, int *numDeviceIndices, int *deviceIndices, pRecDevice *deviceRecords);  //// PsychHIDHelpers.c
void InitializeSynopsis();
PsychError PsychDisplayPsychHIDSynopsis(void);
boolean PsychHIDQueryOpenDeviceInterfaceFromDeviceIndex(int deviceIndex);	// PsychHIDHelpers.c
boolean PsychHIDQueryOpenDeviceInterfaceFromDeviceRecordPtr(pRecDevice deviceRecord);	// PsychHIDHelpers.c
void PsychHIDVerifyOpenDeviceInterfaceFromDeviceIndex(int deviceIndex);	// PsychHIDHelpers.c
int PsychHIDErrors(int error,char **namePtr,char **descriptionPtr);


//end include once
#endif


/*
struct IOHIDEventStruct
{
    IOHIDElementType	type;
    IOHIDElementCookie	elementCookie;
    SInt32		value;
    AbsoluteTime	timestamp;
    UInt32		longValueSize;
    void *		longValue;
};
typedef struct IOHIDEventStruct IOHIDEventStruct;

*/


	
