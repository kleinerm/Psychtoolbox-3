/*
  PsychToolbox3/Source/Common/PsychHID/PsychHIDGetElements.c		
  
  PROJECTS: PsychHID
  
  PLATFORMS:  OSX  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
      
  HISTORY:
  4/29/03  awi		Created.
  
  TO DO:
  

*/

#include "PsychHID.h"


static char useString[]= "elements=PsychHID('Elements',deviceNumber)";
static char synopsisString[] = 
        "Return a struct array describing each element of a connected USB HID device.";
static char seeAlsoString[] = "";


PsychError PSYCHHIDGetElements(void) 
{
    pRecDevice 			specDevice=NULL;
    UInt32                      numDeviceElements;
    
    const char 			*elementFieldNames[]={"typeMaskName", "name", "deviceIndex", "elementIndex", "typeValue", "typeName", "usagePageValue", "usageValue", 							"usageName", "dataSize", "rangeMin", "rangeMax", "scaledRangeMin", "scaledRangeMax", "relative", 
                                                        "wrapping", "nonLinear", "preferredState", "nullState", "calMin", "calMax", "scalingMin", "scalingMax"};
    int 			numElementStructElements, numElementStructFieldNames=23, elementIndex, deviceIndex;
    PsychGenericScriptType	*elementStruct;	
    pRecElement			currentElement;
    char			elementTypeName[PSYCH_HID_MAX_DEVICE_ELEMENT_TYPE_NAME_LENGTH];	
    char			usageName[PSYCH_HID_MAX_DEVICE_ELEMENT_USAGE_NAME_LENGTH];
    char			*typeMaskName;
    HIDElementTypeMask		typeMask;
    	 
    //all subfunctions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));
    
    PsychCopyInIntegerArg(1, TRUE, &deviceIndex);
    PsychHIDVerifyInit();
    specDevice= PsychHIDGetDeviceRecordPtrFromIndex(deviceIndex);
    PsychHIDVerifyOpenDeviceInterfaceFromDeviceRecordPtr(specDevice);
    numDeviceElements= HIDCountDeviceElements(specDevice, kHIDElementTypeIO);
    numElementStructElements = (int)numDeviceElements;
    PsychAllocOutStructArray(1, FALSE, numElementStructElements, numElementStructFieldNames, elementFieldNames, &elementStruct);
    elementIndex=0;
    for(currentElement=HIDGetFirstDeviceElement(specDevice,kHIDElementTypeIO); 
        currentElement != NULL; 
        currentElement=HIDGetNextDeviceElement(currentElement, kHIDElementTypeIO))
        {
        typeMask=HIDConvertElementTypeToMask (currentElement->type);
 	PsychHIDGetTypeMaskStringFromTypeMask(typeMask, &typeMaskName);
        PsychSetStructArrayStringElement("typeMaskName",	elementIndex, 	typeMaskName,	 			elementStruct);
        PsychSetStructArrayStringElement("name",		elementIndex, 	currentElement->name,	 		elementStruct);
        PsychSetStructArrayDoubleElement("deviceIndex",		elementIndex, 	(double)deviceIndex, 			elementStruct);
        PsychSetStructArrayDoubleElement("elementIndex",	elementIndex, 	(double)elementIndex+1, 		elementStruct);
        PsychSetStructArrayDoubleElement("typeValue",		elementIndex, 	(double)currentElement->type, 		elementStruct);
        HIDGetTypeName(currentElement->type, elementTypeName);
        PsychSetStructArrayStringElement("typeName",		elementIndex, 	elementTypeName,	 		elementStruct);
        PsychSetStructArrayDoubleElement("usagePageValue",	elementIndex, 	(double)currentElement->usagePage, 	elementStruct);
        PsychSetStructArrayDoubleElement("usageValue",		elementIndex, 	(double)currentElement->usage, 		elementStruct);
        HIDGetUsageName (currentElement->usagePage, currentElement->usage, usageName);
        PsychSetStructArrayStringElement("usageName",		elementIndex, 	usageName,	 			elementStruct);
        PsychSetStructArrayDoubleElement("dataSize",		elementIndex, 	(double)currentElement->size, 		elementStruct);
        PsychSetStructArrayDoubleElement("rangeMin",		elementIndex, 	(double)currentElement->min, 		elementStruct);
        PsychSetStructArrayDoubleElement("rangeMax",		elementIndex, 	(double)currentElement->max, 		elementStruct);
        PsychSetStructArrayDoubleElement("scaledRangeMin",	elementIndex, 	(double)currentElement->scaledMin, 	elementStruct);
        PsychSetStructArrayDoubleElement("scaledRangeMax",	elementIndex, 	(double)currentElement->scaledMax,	elementStruct);
        PsychSetStructArrayDoubleElement("relative",		elementIndex, 	(double)currentElement->relative,	elementStruct);	//boolean flag
        PsychSetStructArrayDoubleElement("wrapping",		elementIndex, 	(double)currentElement->wrapping,	elementStruct);	//boolean flag	
        PsychSetStructArrayDoubleElement("nonLinear",		elementIndex, 	(double)currentElement->nonLinear,	elementStruct);	//boolean flag
        PsychSetStructArrayDoubleElement("preferredState",	elementIndex, 	(double)currentElement->preferredState,	elementStruct);	//boolean flag
        PsychSetStructArrayDoubleElement("nullState",		elementIndex, 	(double)currentElement->nullState,	elementStruct);	//boolean flag

        PsychSetStructArrayDoubleElement("calMin",		elementIndex, 	(double)currentElement->calMin,		elementStruct);	
        PsychSetStructArrayDoubleElement("calMax",		elementIndex, 	(double)currentElement->calMax,		elementStruct);	
        PsychSetStructArrayDoubleElement("scalingMin",		elementIndex, 	(double)currentElement->userMin,	elementStruct);	
        PsychSetStructArrayDoubleElement("scalingMax",		elementIndex, 	(double)currentElement->userMax,	elementStruct);	
                                
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




