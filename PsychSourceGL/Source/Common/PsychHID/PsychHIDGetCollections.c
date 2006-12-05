/*
  PsychToolbox3/Source/Common/PsychHID/PsychHIDGetCollections.c		
  
  PROJECTS: PsychHID
  
  PLATFORMS:  OSX 
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
      
  HISTORY:
  5/11/03  awi		Created.
  
  TO DO:
  

*/

#include "PsychHID.h"


static char useString[]= "collections=PsychHID('Collections', deviceNumber)";
static char synopsisString[] = 
        "Return a flat list of all collections on the specified USB HID device. "
        "A collection is a grouping of elements." 
        "Collections are hierarchial. A collection can contain other collections. "
        "Use the \"memberCollectionIndices\" field of the returned structures, "
        "which indexs the member collections,  to "
        "expose the hierarchy.";

static char seeAlsoString[] = "";

PsychError PSYCHHIDGetCollections(void) 
{
    pRecDevice 			specDevice=NULL;
    UInt32                      numDeviceElements;
    
    const char 			*elementFieldNames[]={"typeMaskName", "name", "deviceIndex", "collectionIndex", "typeValue", "typeName", "usagePageValue",
                                                        "usageValue", "usageName", "memberCollectionIndices", "memberElementIndices"};
    int 			i, numElementStructElements, numElementStructFieldNames=11, elementIndex, deviceIndex;
    PsychGenericScriptType	*elementStruct, *memberCollectionIndicesMat, *memberIOElementIndicesMat;	
    pRecElement			currentElement;
    char			elementTypeName[PSYCH_HID_MAX_DEVICE_ELEMENT_TYPE_NAME_LENGTH];	
    char			usageName[PSYCH_HID_MAX_DEVICE_ELEMENT_USAGE_NAME_LENGTH];
    char			*typeMaskName;
    HIDElementTypeMask		typeMask;
    pRecElement			*memberCollectionRecords, *memberIOElementRecords;     
    double			*memberCollectionIndices, *memberIOElementIndices; 
    int				numSubCollections, numSubIOElements;
    
    	 
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));
        
    PsychCopyInIntegerArg(1, TRUE, &deviceIndex);
    PsychHIDVerifyInit();
    specDevice= PsychHIDGetDeviceRecordPtrFromIndex(deviceIndex);
    PsychHIDVerifyOpenDeviceInterfaceFromDeviceRecordPtr(specDevice);
    numDeviceElements= HIDCountDeviceElements(specDevice, kHIDElementTypeCollection);
    numElementStructElements = (int)numDeviceElements;
    PsychAllocOutStructArray(1, FALSE, numElementStructElements, numElementStructFieldNames, elementFieldNames, &elementStruct);
    elementIndex=0;
    for(currentElement=HIDGetFirstDeviceElement(specDevice,kHIDElementTypeCollection); 
        currentElement != NULL; 
        currentElement=HIDGetNextDeviceElement(currentElement, kHIDElementTypeCollection))
        {
        typeMask=HIDConvertElementTypeToMask (currentElement->type);
 	PsychHIDGetTypeMaskStringFromTypeMask(typeMask, &typeMaskName);
        PsychSetStructArrayStringElement("typeMaskName",	elementIndex, 	typeMaskName,	 			elementStruct);
        PsychSetStructArrayStringElement("name",		elementIndex, 	currentElement->name,	 		elementStruct);
        PsychSetStructArrayDoubleElement("deviceIndex",		elementIndex, 	(double)deviceIndex, 			elementStruct);
        PsychSetStructArrayDoubleElement("collectionIndex",	elementIndex, 	(double)elementIndex+1, 		elementStruct);
        PsychSetStructArrayDoubleElement("typeValue",		elementIndex, 	(double)currentElement->type, 		elementStruct);
        HIDGetTypeName(currentElement->type, elementTypeName);
        PsychSetStructArrayStringElement("typeName",		elementIndex, 	elementTypeName,	 		elementStruct);
        PsychSetStructArrayDoubleElement("usagePageValue",	elementIndex, 	(double)currentElement->usagePage, 	elementStruct);
        PsychSetStructArrayDoubleElement("usageValue",		elementIndex, 	(double)currentElement->usage, 		elementStruct);
        HIDGetUsageName (currentElement->usagePage, currentElement->usage, usageName);
        PsychSetStructArrayStringElement("usageName",		elementIndex, 	usageName,	 			elementStruct);
                          
        //find and return the indices of this collection's member collections and indices
        numSubCollections=PsychHIDCountCollectionElements(currentElement, kHIDElementTypeCollection);
        numSubIOElements=PsychHIDCountCollectionElements(currentElement, kHIDElementTypeIO);
        memberCollectionRecords=(pRecElement*)PsychMallocTemp(sizeof(pRecElement) * numSubCollections);
        memberIOElementRecords=(pRecElement*)PsychMallocTemp(sizeof(pRecElement) * numSubIOElements);
        PsychHIDFindCollectionElements(currentElement, kHIDElementTypeCollection, memberCollectionRecords, numSubCollections);
        PsychHIDFindCollectionElements(currentElement, kHIDElementTypeIO, memberIOElementRecords, numSubIOElements);
        memberCollectionIndices=NULL;
        PsychAllocateNativeDoubleMat(1, numSubCollections, 1, &memberCollectionIndices, &memberCollectionIndicesMat);
        memberIOElementIndices=NULL;
        PsychAllocateNativeDoubleMat(1, numSubIOElements, 1, &memberIOElementIndices, &memberIOElementIndicesMat);
        
        for(i=0;i<numSubCollections;i++)
            memberCollectionIndices[i]=PsychHIDGetIndexFromRecord(specDevice, memberCollectionRecords[i], kHIDElementTypeCollection);
        for(i=0;i<numSubIOElements;i++)
            memberIOElementIndices[i]=PsychHIDGetIndexFromRecord(specDevice, memberIOElementRecords[i], kHIDElementTypeIO);
        PsychFreeTemp(memberCollectionRecords);
        PsychFreeTemp(memberIOElementRecords);
        PsychSetStructArrayNativeElement("memberCollectionIndices", 	elementIndex,	memberCollectionIndicesMat,	elementStruct);
        PsychSetStructArrayNativeElement("memberElementIndices", 	elementIndex,	memberIOElementIndicesMat,	elementStruct);

        ++elementIndex; 
    }

    return(PsychError_none);	
}


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




