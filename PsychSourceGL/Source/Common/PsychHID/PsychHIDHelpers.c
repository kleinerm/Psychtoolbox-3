/*
  PsychToolbox/Source/Common/PsychHID/PsychHIDHelpers.c		
  
  PROJECTS: PsychHID
  
  PLATFORMS:  All
  
  AUTHORS:
  Allen.Ingling@nyu.edu             awi 
  mario.kleiner@tuebingen.mpg.de    mk
      
  HISTORY:
  5/05/03  awi		Created.
  4/19/05  dgp      cosmetic.
  8/23/07  rpw      added PsychHIDKbQueueRelease() to PsychHIDCleanup()
  4/04/09  mk		added support routines for generic USB devices and usbDeviceRecordBank.

  TO DO:

*/

#include "PsychHID.h"

#if PSYCH_SYSTEM != PSYCH_OSX

// Global variable which holds a reference to the last
// accessed hid_device* on non-OS/X. This is for error
// handling:
hid_device* last_hid_device = NULL;

// List with all known low level USB-HID devices from enumeration:
struct hid_device_info* hidlib_devices = NULL;
pRecDevice hid_devices = NULL;
hid_device* source[MAXDEVICEINDEXS];

#endif

// Tracker used to maintain references to open generic USB devices.
// PsychUSBDeviceRecord is currently defined in PsychHID.h.
PsychUSBDeviceRecord usbDeviceRecordBank[PSYCH_HID_MAX_GENERIC_USB_DEVICES];

/* PsychInitializePsychHID()
 *
 * Master init routine - Called at module load time / first time init.
 *
 */
void PsychInitializePsychHID(void)
{
	int i;

	// Initialize the generic USB tracker to "all off" state:
	for (i = 0; i < PSYCH_HID_MAX_GENERIC_USB_DEVICES; i++) {
		usbDeviceRecordBank[i].valid = 0;
	}

	// Initialize OS specific interfaces and routines:
	PsychHIDInitializeHIDStandardInterfaces();

	return;
}

/* PsychHIDGetFreeUSBDeviceSlot();
 *
 * Return a device record pointer to a free generic USB device
 * slot, as well as the associated numeric usbHandle.
 *
 * Abort with error if no more slots are free.
 */
PsychUSBDeviceRecord* PsychHIDGetFreeUSBDeviceSlot(int* usbHandle)
{
	int i;
	
	// Find the next available USB slot:
	for (i = 0; i < PSYCH_HID_MAX_GENERIC_USB_DEVICES; i++) {
		if (usbDeviceRecordBank[i].valid == 0) {
			*usbHandle = i;
			return( &(usbDeviceRecordBank[i]) );
		}
	}

	// If we reach this point, then all slots are occupied: Fail!
	PsychErrorExitMsg(PsychError_user, "Unable to open another generic USB device! Too many devices open. Please close one and retry.");
	return(NULL);
}

/* PsychHIDGetFreeUSBDeviceSlot();
 *
 * Return a device record pointer to a free generic USB device
 * slot, as well as the associated numeric usbHandle.
 *
 * Abort with error if no more slots are free.
 */
PsychUSBDeviceRecord* PsychHIDGetUSBDevice(int usbHandle)
{
	// Child protection:
	if (usbHandle < 0 || usbHandle >= PSYCH_HID_MAX_GENERIC_USB_DEVICES) PsychErrorExitMsg(PsychError_user, "Invalid generic USB device handle provided! Handle outside valid range.");
	if (usbDeviceRecordBank[usbHandle].valid == 0) PsychErrorExitMsg(PsychError_user, "Invalid generic USB device handle provided! The handle doesn't correspond to an open device.");

	// Valid handle for slot corresponding to an open device. Return PsychUSBDeviceRecord* to it:
	return( &(usbDeviceRecordBank[usbHandle]) );
}

void PsychHIDCloseAllUSBDevices(void)
{
	int i;
	for (i = 0; i < PSYCH_HID_MAX_GENERIC_USB_DEVICES; i++) {
		if (usbDeviceRecordBank[i].valid) {
			PsychHIDOSCloseUSBDevice(PsychHIDGetUSBDevice(i));
		}
	}
}

/*
    PsychHIDCleanup() 
    
    Cleanup before flushing the mex file.   
*/
PsychError PsychHIDCleanup(void) 
{
	long error;
	pRecDevice curdev = NULL;
    
	// Disable online help system:
	PsychClearGiveHelp();
	
	// Shutdown keyboard queue functions on OS/X:
	#if PSYCH_SYSTEM == PSYCH_OSX
	error = PSYCHHIDKbQueueRelease();	// PsychHIDKbQueueRelease.c, but has to be called with uppercase PSYCH because that's how it's registered (otherwise crashes on clear mex)
	#endif
    
	// Shutdown USB-HID report low-level functions, e.g., for DAQ toolbox on OS/X:
	error = PsychHIDReceiveReportsCleanup(); // PsychHIDReceiveReport.c
	
	// Release all other HID device data structures:
	#if PSYCH_SYSTEM == PSYCH_OSX
	// Via Apple HIDUtils:
        if(HIDHaveDeviceList()) HIDReleaseDeviceList();
	#else        
        // Then our high-level list:
        while (hid_devices) {
            // Get current device record to release:
            curdev = hid_devices;
            
            // Advance to the next one for next loop iteration:
            hid_devices = hid_devices->pNext;
            
            // Interface attached aka device opened? If so we need to close the device handle:
            if (curdev->interface) hid_close((hid_device*) curdev->interface);
            
            // Release:
            free(curdev);
        }
        
        // Reset last hid device for error handling:
        last_hid_device = NULL;
        
        // Last the HIDLIB low-level list: This will also trigger
        // HIDAPI shutdown and cleanup:
        if (hidlib_devices) hid_free_enumeration(hidlib_devices);
        hidlib_devices = NULL;
	#endif
    
	// Close and release all open generic USB devices:
	PsychHIDCloseAllUSBDevices();

	// Shutdown os specific interfaces and routines:
	PsychHIDShutdownHIDStandardInterfaces();

	return(PsychError_none);
}

/* 
    PsychHIDGetDeviceRecordPtrFromIndex()
    
    The inverse of PsychHIDGetIndexFromRecord()
    
    Accept the index from the list of device records and return a pointer to the indicated record.  Externally the list is one-indexed.  
*/
pRecDevice PsychHIDGetDeviceRecordPtrFromIndex(int deviceIndex)
{
    int				i;
    pRecDevice 		currentDevice=NULL;

    PsychHIDVerifyInit();
    i=1;
    for(currentDevice=HIDGetFirstDevice(); currentDevice != NULL; currentDevice=HIDGetNextDevice(currentDevice)){    
        if(i==deviceIndex) {
            #if PSYCH_SYSTEM != PSYCH_OSX
                if (!currentDevice->interface) {
                    currentDevice->interface = (void*) hid_open_path(currentDevice->transport);
                    if (!currentDevice->interface) PsychErrorExitMsg(PsychError_system, "HIDLIB Failed to open USB device!");

                    // Set read ops on device to non-blocking:
                    hid_set_nonblocking((hid_device*) currentDevice->interface, 1);
                }
            #endif
            return(currentDevice);
        }
        ++i;
    }
    
    PsychErrorExitMsg(PsychError_internal, "Invalid device index specified.  Has a device has been unplugged? Try rebuilding the device list");
    return(NULL);  //make the compiler happy.
}

#if PSYCH_SYSTEM == PSYCH_OSX

void PsychHIDInitializeHIDStandardInterfaces(void)
{
    return;
}

void PsychHIDShutdownHIDStandardInterfaces(void)
{
    return;
}

/*
    PSYCHHIDCheckInit() 
    
    Check to see if we need to create the USB-HID device list. If it has not been created then create it.   
*/
void PsychHIDVerifyInit(void)
{
    if(!HIDHaveDeviceList()) HIDBuildDeviceList( 0, 0);
	PsychHIDWarnInputDisabled(NULL);
}

/*
	PsychHIDWarnInputDisabled()
	
	Check if HID event input is disabled by external processes, e.g., due to
	secure password entry protection. Return TRUE and output a warning message
	to user if HID input won't work due to active security measures. Return FALSE
	and stay silent if PsychHID can work as expected.
	
*/
psych_bool PsychHIDWarnInputDisabled(const char* callerName)
{
	if (IsSecureEventInputEnabled()) {
		printf("PTB-WARNING: During %s: Some other running application is preventing me from accessing the keyboard/keypad/mouse/...!\n", (callerName) ? callerName : "PsychHID invocation");
		printf("PTB-WARNING: This is likely a security measure, e.g., to protect some active password entry field.\n");
		printf("PTB-WARNING: Please identify and quit the offending application. E.g., some versions of Firefox are known to cause such problems...\n");
		return(TRUE);
	}

	return(FALSE);
}

/*
    PsychHIDGetDeviceListByUsage()
*/ 
void PsychHIDGetDeviceListByUsage(long usagePage, long usage, int *numDeviceIndices, int *deviceIndices, pRecDevice *deviceRecords)
{
    pRecDevice 			currentDevice;
    int				currentDeviceIndex;

    PsychHIDVerifyInit();
    currentDeviceIndex=0;
    *numDeviceIndices=0;
    for(currentDevice=HIDGetFirstDevice(); currentDevice != NULL; currentDevice=HIDGetNextDevice(currentDevice)){    
        ++currentDeviceIndex;     
        if(currentDevice->usagePage==usagePage && currentDevice->usage==usage){
            deviceRecords[*numDeviceIndices]=currentDevice;
            deviceIndices[*numDeviceIndices]=currentDeviceIndex;  //the array is 0-indexed, devices are 1-indexed.   
            ++(*numDeviceIndices);
        }
    }
}
 
/*
    PsychHIDGetDeviceListByUsages()
 */ 
void PsychHIDGetDeviceListByUsages(int numUsages, long *usagePages, long *usages, int *numDeviceIndices, int *deviceIndices, pRecDevice *deviceRecords)
{
    pRecDevice 			currentDevice;
    int				currentDeviceIndex;
    int				currentUsage;
    long 			*usagePage;
    long			*usage;
	
    PsychHIDVerifyInit();
    *numDeviceIndices=0;
    for(usagePage=usagePages, usage=usages, currentUsage=0; currentUsage<numUsages; usagePage++, usage++, currentUsage++){
		currentDeviceIndex=0;
		for(currentDevice=HIDGetFirstDevice(); currentDevice != NULL; currentDevice=HIDGetNextDevice(currentDevice)){    
			++currentDeviceIndex;     
			if(currentDevice->usagePage==*usagePage && currentDevice->usage==*usage){
				deviceRecords[*numDeviceIndices]=currentDevice;
				deviceIndices[*numDeviceIndices]=currentDeviceIndex;  //the array is 0-indexed, devices are 1-indexed.   
				++(*numDeviceIndices);
			}
		}
	}
}


/*
    PsychHIDGetIndexFromRecord()
    
    The inverse of PsychHIDGetDeviceRecordPtrFromIndex. 
    
    This O(n) where n is the number of device elements.   We could make it O(1) if we modified
    the element structure in the HID Utilities library to include a field specifying the index of the element or 
    collection.
    
    Note that if PsychHIDGetIndexFromRecord() is O(n) then its caller, PsychHIDGetCollections, is O(n^2) for each
    device, whereas if PsychHIDGetIndexFromRecord() is O(1) then psychHIDGetCollections becomes O(n) for each 
    device.   
*/
int PsychHIDGetIndexFromRecord(pRecDevice deviceRecord, pRecElement elementRecord, HIDElementTypeMask typeMask)
{
    int 		elementIndex;
    pRecElement		currentElement;						
    
    if(elementRecord==NULL)
        return(0);
    elementIndex=1;
    for(currentElement=HIDGetFirstDeviceElement(deviceRecord, typeMask);
        currentElement != elementRecord && currentElement != NULL;
        currentElement=HIDGetNextDeviceElement(currentElement, typeMask))
        ++elementIndex;
    if(currentElement==elementRecord)
        return(elementIndex);
    else{
        PsychErrorExitMsg(PsychError_internal, "Element record not found within device record");
        return(0); //make the compiler happy
    }    
    
}


pRecElement PsychHIDGetElementRecordFromDeviceRecordAndElementIndex(pRecDevice deviceRecord, int elementIndex)
{
    int				i;
    pRecElement			currentElement;

    PsychHIDVerifyInit();
    i=1;
    for(currentElement=HIDGetFirstDeviceElement(deviceRecord, kHIDElementTypeIO); 
        currentElement != NULL; 
        currentElement=HIDGetNextDeviceElement (currentElement, kHIDElementTypeIO))
    {    
        if(i==elementIndex)
            return(currentElement);
        ++i;
    }
    PsychErrorExitMsg(PsychError_internal, "Invalid device index specified.  Has a device has been unplugged? Try rebuilding the device list");
    return(NULL);  //make the compiler happy.

}


pRecElement PsychHIDGetCollectionRecordFromDeviceRecordAndCollectionIndex(pRecDevice deviceRecord, int elementIndex)
{
    int				i;
    pRecElement			currentElement;

    PsychHIDVerifyInit();
    i=1;
    for(currentElement=HIDGetFirstDeviceElement(deviceRecord, kHIDElementTypeCollection); 
        currentElement != NULL; 
        currentElement=HIDGetNextDeviceElement (currentElement, kHIDElementTypeCollection))
    {    
        if(i==elementIndex)
            return(currentElement);
        ++i;
    }
    PsychErrorExitMsg(PsychError_internal, "Invalid collection index specified.  Has a device has been unplugged? Try rebuilding the device list");
    return(NULL);  //make the compiler happy.

}


/*
        PsychHIDQueryOpenDeviceInterfaceFromDeviceIndex()

        Check the interface field of the libHIDUtilities device structure for NULLness.  libHIDUtilities.h seems to indicate that it is neccessary for application
        to invoke HIDCreateOpenDeviceInterface() before accessing a device.  However,
        1) libHIDUtilities provides no way to obtain a value for the required first argument to HIDCreateOpenDeviceInterface().  
        2) Apple's example HID Explorer application does not call HIDCreateOpenDeviceInterface().   
        3) Internally, libHIDUtilities invokes HIDCreateOpenDeviceInterface() itself when HIDBuildDeviceList() is called.
        
        Because the call lies within mysterious conditionals there is some uncertainty about whether HIDCreateOpenDeviceInterface() will always 
        invoke HIDBuildDeviceList().  Therefore, PsychHID verifies that the device interface has been opened before accessing the elements of a device.
        
*/ 
psych_bool PsychHIDQueryOpenDeviceInterfaceFromDeviceIndex(int deviceIndex)
{
    pRecDevice 			deviceRecord;

    PsychHIDVerifyInit();
    deviceRecord=PsychHIDGetDeviceRecordPtrFromIndex(deviceIndex);
    return(deviceRecord->interface != NULL);    
}

psych_bool PsychHIDQueryOpenDeviceInterfaceFromDeviceRecordPtr(pRecDevice deviceRecord)
{
    PsychHIDVerifyInit();
    return(deviceRecord->interface != NULL);
}

void PsychHIDVerifyOpenDeviceInterfaceFromDeviceIndex(int deviceIndex)
{
    if(!PsychHIDQueryOpenDeviceInterfaceFromDeviceIndex(deviceIndex))
        PsychErrorExitMsg(PsychError_internal, "Device interface field is NULL.  libHIDUtilities failed to open the device interface ?");
}

void PsychHIDVerifyOpenDeviceInterfaceFromDeviceRecordPtr(pRecDevice deviceRecord)
{
    if(!PsychHIDQueryOpenDeviceInterfaceFromDeviceRecordPtr(deviceRecord))
        PsychErrorExitMsg(PsychError_internal, "Device interface field is NULL.  libHIDUtilities failed to open the device interface ?");
}


/*
    PsychHIDGetTypeMaskStringFromTypeMask()
    
    Apple's HID Utilities library uses two different specificationos of the device type:
        - enumerated type HIDElementTypeMask, used to specify which element types to return.
        - unsigned long type identifier constants.  
        
    The mask values will not mask the unsigned long type identifiers.  
*/
void PsychHIDGetTypeMaskStringFromTypeMask(HIDElementTypeMask maskValue, char **pStr)
{
    char *maskNames[]={"input", "output", "feature", "collection", "io", "all", };
    
    switch(maskValue)
    {
        case kHIDElementTypeInput:
            *pStr=maskNames[0];
            break;
        case kHIDElementTypeOutput:
            *pStr=maskNames[1];
            break;
        case kHIDElementTypeFeature:
            *pStr=maskNames[2];
            break;
        case kHIDElementTypeCollection:
            *pStr=maskNames[3];
            break;
        case kHIDElementTypeIO:
            *pStr=maskNames[4];
            break;
        case kHIDElementTypeAll:
            *pStr=maskNames[5];
            break;
    }
}


/*
    PsychHIDCountCollectionElements()
    
    Non-recursively count all elements of a collection which are of the specified type.
    
    HID element records hold three pointers to other element records: pPrevious, pChild and pSibling.  PsychHIDCountCollectionElements() 
    operates on the theory that the members of a collection are its child and all of that child's siblings.
    
  
*/
int PsychHIDCountCollectionElements(pRecElement collectionRecord, HIDElementTypeMask elementTypeMask)
{
    pRecElement		currentElement;
    int			numElements=0;
    HIDElementTypeMask	currentElementMaskValue;
    
    for(currentElement=collectionRecord->pChild; currentElement != NULL; currentElement= currentElement->pSibling)
    {
        currentElementMaskValue=HIDConvertElementTypeToMask(currentElement->type);  
        if(currentElementMaskValue & elementTypeMask)
            ++numElements;
    }
    return(numElements);
}


/*
    FindCollectionElements()
    
    Non-recursively return of a list of a collection's memember elements.
    
    HID element records hold three pointers to other element records: pPrevious, pChild and pSibling.  FindCollectionElements() 
    operates on the theory that the members of a collection are its child and all of that child's siblings.
    
*/
int PsychHIDFindCollectionElements(pRecElement collectionRecord, HIDElementTypeMask elementTypeMask, pRecElement *collectionMembers, int maxListElements)
{
    pRecElement		currentElement;
    int			numElements=0;
    HIDElementTypeMask	currentElementMaskValue;
    
    for(currentElement=collectionRecord->pChild; currentElement != NULL; currentElement= currentElement->pSibling)
    {
        currentElementMaskValue=HIDConvertElementTypeToMask(currentElement->type);  
        if(currentElementMaskValue & elementTypeMask){
            if(numElements == maxListElements)
                PsychErrorExitMsg(PsychError_internal, "Number of collection elements exceeds allocated storage space." );
            collectionMembers[numElements]=currentElement;
            ++numElements;
        }
    }
    return(numElements);
}
	
#else

/* Linux and MS-Windows support via HIDLIB: */
/* ======================================== */

/*
    PSYCHHIDCheckInit() 
    
    Check to see if we need to create the USB-HID device list. If it has not been created then create it.   
*/
void PsychHIDVerifyInit(void)
{
    int busId, devId, intId;
    pRecDevice currentDevice = NULL;
    struct hid_device_info* hid_dev = NULL;
    
    // If hid_devices list of all HID devices not yet initialized,
    // perform device enumeration:
    if (!hidlib_devices) {        
        // Low-Level enumeration by HIDLIB:
        hidlib_devices = hid_enumerate(0x0, 0x0);
        
        // Build our own higher-level device list filled with info
        // from the low-level list:
        for (hid_dev = hidlib_devices; hid_dev != NULL; hid_dev = hid_dev->next) {
            // Allocate and zero-init high level struct currentDevice:
            currentDevice = calloc(1, sizeof(recDevice));
            
            // Copy low-level props to corresponding high-level props:
            currentDevice->usagePage = hid_dev->usage_page;
            currentDevice->usage = hid_dev->usage;
            // Abuse the "transport" string for the device path. On OS/X this just contains "USB":
            sprintf(&currentDevice->transport[0], "%s", hid_dev->path);
            currentDevice->vendorID = hid_dev->vendor_id;
            currentDevice->productID = hid_dev->product_id;
            currentDevice->version = hid_dev->release_number;
            if (hid_dev->manufacturer_string) wcstombs(&currentDevice->manufacturer[0], hid_dev->manufacturer_string, 256);
            if (hid_dev->product_string) wcstombs(&currentDevice->product[0], hid_dev->product_string, 256);
            if (hid_dev->serial_number) wcstombs(&currentDevice->serial[0], hid_dev->serial_number, 256);

            // Convert unique device path into unique numeric location id:
            sscanf(hid_dev->path, "%i:%i:%i", &busId, &devId, &intId);
            currentDevice->locID = busId * 1000000 + devId * 100 + intId;

            // Interface number is great for identifying DAQ devices, but not available
            // on OS/X, so this will be a Linux/Windows only thing.
            currentDevice->interfaceId = hid_dev->interface_number;

            // Enqueue record into linked list:
            currentDevice->pNext = hid_devices;
            hid_devices = currentDevice;
        }
    }
    
    return;
}

pRecDevice HIDGetFirstDevice(void)
{
    return hid_devices;
}

pRecDevice HIDGetNextDevice(pRecDevice pDevice)
{
    return pDevice->pNext;
}

/* HIDCountDevices(): Return count of all enumerated HID devices: */
psych_uint32 HIDCountDevices(void)
{
    pRecDevice cur_dev = hid_devices;
    psych_uint32 count = 0;
    
    while (cur_dev) {
        count++;
        cur_dev = cur_dev->pNext;
    }
    
    return count;
}

#endif
