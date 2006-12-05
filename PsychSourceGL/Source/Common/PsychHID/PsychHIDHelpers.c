/*
  PsychToolbox/Source/Common/PsychHID/PsychHIDHelpers.c		
  
  PROJECTS: PsychHID
  
  PLATFORMS:  OSX
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
      
  HISTORY:
  5/05/03  awi		Created.
  4/19/05  dgp      cosmetic.
  
  TO DO:
  

*/

#include "PsychHID.h"



/*
    PSYCHHIDCheckInit() 
    
    Check to see if we need to create the device list.  If it has not been created then create it.   
*/
void PsychHIDVerifyInit(void)
{
    if(!HIDHaveDeviceList())HIDBuildDeviceList( 0, 0);
}


/*
    PsychHIDCleanup() 
    
    Cleanup before flushing the mex file.   
*/
PsychError PsychHIDCleanup(void) 
{
	long error;

	error=PsychHIDReceiveReportsCleanup(); // PsychHIDReceiveReport.c
    if(HIDHaveDeviceList())HIDReleaseDeviceList();
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
    pRecDevice 			currentDevice=NULL;

    PsychHIDVerifyInit();
    i=1;
    for(currentDevice=HIDGetFirstDevice(); currentDevice != NULL; currentDevice=HIDGetNextDevice(currentDevice)){    
        if(i==deviceIndex)
            return(currentDevice);
        ++i;
    }
    PsychErrorExitMsg(PsychError_internal, "Invalid device index specified.  Has a device has been unplugged? Try rebuilding the device list");
    return(NULL);  //make the compiler happy.
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
boolean PsychHIDQueryOpenDeviceInterfaceFromDeviceIndex(int deviceIndex)
{
    pRecDevice 			deviceRecord;

    PsychHIDVerifyInit();
    deviceRecord=PsychHIDGetDeviceRecordPtrFromIndex(deviceIndex);
    return(deviceRecord->interface != NULL);    
}

boolean PsychHIDQueryOpenDeviceInterfaceFromDeviceRecordPtr(pRecDevice deviceRecord)
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
	
