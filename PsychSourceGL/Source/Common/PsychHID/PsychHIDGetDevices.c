/*
  PsychToolbox3/Source/Common/PsychHID/PsychHIDGetDevices.c		
  
  PROJECTS: PsychHID
  
  PLATFORMS:  OSX  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
      
  HISTORY:
  4/29/03  awi		Created.
  
  TO DO:
  

*/

#include "PsychHID.h"

static char useString[]= "devices=PsychHID('Devices')";
static char synopsisString[] = 
        "Return a struct array describing each connected USB HID device.";
static char seeAlsoString[] = "";

PsychError PSYCHHIDGetDevices(void) 
{
    pRecDevice 			currentDevice=NULL;
    
    const char *deviceFieldNames[]={"usagePageValue", "usageValue", "usageName", "index", "transport", "vendorID", "productID", "version", 
                                    "manufacturer", "product", "serialNumber", "locationID", "totalElements", "features", "inputs", 
                                    "outputs", "collections", "axes", "buttons", "hats", "sliders", "dials", "wheels"};
    int numDeviceStructElements, numDeviceStructFieldNames=23, deviceIndex;
    PsychGenericScriptType	*deviceStruct;		
    char usageName[PSYCH_HID_MAX_DEVICE_ELEMENT_USAGE_NAME_LENGTH];


    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));
    PsychHIDVerifyInit();
    numDeviceStructElements=(int)HIDCountDevices();
    PsychAllocOutStructArray(1, FALSE, numDeviceStructElements, numDeviceStructFieldNames, deviceFieldNames, &deviceStruct);
    deviceIndex=0;
    for(currentDevice=HIDGetFirstDevice(); currentDevice != NULL; currentDevice=HIDGetNextDevice(currentDevice)){
        PsychSetStructArrayDoubleElement("usagePageValue",	deviceIndex, 	(double)currentDevice->usagePage,	deviceStruct);
        PsychSetStructArrayDoubleElement("usageValue",		deviceIndex, 	(double)currentDevice->usage, 		deviceStruct);
        HIDGetUsageName (currentDevice->usagePage, currentDevice->usage, usageName);
        PsychSetStructArrayStringElement("usageName",		deviceIndex, 	usageName, 				deviceStruct);
        PsychSetStructArrayDoubleElement("index",		deviceIndex, 	(double)deviceIndex+1, 			deviceStruct);
        PsychSetStructArrayStringElement("transport",		deviceIndex, 	currentDevice->transport, 		deviceStruct);
        PsychSetStructArrayDoubleElement("vendorID",		deviceIndex, 	(double)currentDevice->vendorID, 	deviceStruct);
        PsychSetStructArrayDoubleElement("productID",		deviceIndex, 	(double)currentDevice->productID, 	deviceStruct);
        PsychSetStructArrayDoubleElement("version",		deviceIndex, 	(double)currentDevice->version, 	deviceStruct);
        PsychSetStructArrayStringElement("manufacturer",	deviceIndex, 	currentDevice->manufacturer, 		deviceStruct);
        PsychSetStructArrayStringElement("product",		deviceIndex, 	currentDevice->product, 		deviceStruct);
        PsychSetStructArrayStringElement("serialNumber",	deviceIndex, 	currentDevice->serial, 			deviceStruct);
        PsychSetStructArrayDoubleElement("locationID",		deviceIndex, 	(double)currentDevice->locID, 		deviceStruct);
        PsychSetStructArrayDoubleElement("totalElements",	deviceIndex, 	(double)currentDevice->totalElements, 	deviceStruct);
        PsychSetStructArrayDoubleElement("features",		deviceIndex, 	(double)currentDevice->features, 	deviceStruct);
        PsychSetStructArrayDoubleElement("inputs",		deviceIndex, 	(double)currentDevice->inputs, 		deviceStruct);
        PsychSetStructArrayDoubleElement("outputs",		deviceIndex, 	(double)currentDevice->outputs, 	deviceStruct);
        PsychSetStructArrayDoubleElement("collections",		deviceIndex, 	(double)currentDevice->collections, 	deviceStruct);
        PsychSetStructArrayDoubleElement("axes",		deviceIndex, 	(double)currentDevice->axis, 		deviceStruct);
        PsychSetStructArrayDoubleElement("buttons",		deviceIndex, 	(double)currentDevice->buttons, 	deviceStruct);
        PsychSetStructArrayDoubleElement("hats",		deviceIndex, 	(double)currentDevice->hats, 		deviceStruct);
        PsychSetStructArrayDoubleElement("sliders",		deviceIndex, 	(double)currentDevice->sliders, 	deviceStruct);
        PsychSetStructArrayDoubleElement("dials",		deviceIndex, 	(double)currentDevice->dials, 		deviceStruct);
        PsychSetStructArrayDoubleElement("wheels",		deviceIndex, 	(double)currentDevice->wheels, 		deviceStruct);
        ++deviceIndex; 
    }

    return(PsychError_none);	
}





