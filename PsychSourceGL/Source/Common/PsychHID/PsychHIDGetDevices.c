/*
  PsychToolbox3/Source/Common/PsychHID/PsychHIDGetDevices.c		
  
  PROJECTS: PsychHID
  
  PLATFORMS:  All  
  
  AUTHORS:
  Allen.Ingling@nyu.edu             awi 
  mario.kleiner@tuebingen.mpg.de    mk
      
  HISTORY:
  4/29/03  awi		Created.
  
  TO DO:
  
*/

#include "PsychHID.h"

static char useString[]= "devices=PsychHID('Devices' [, deviceClass])";
static char synopsisString[] =  "Return a struct array describing each connected USB HID device.\n"
				"'deviceClass' optionally selects for the class of input device. "
				"This is not supported on all operating systems and will be silently "
				"ignored if unsupported. On Linux you can select the following classes "
				"of input devices: 1 = MasterPointer, 2 = MasterKeyboard, 3 = SlavePointer "
				"4 = SlaveKeyboard, 5 = Floating slave device.\n\n"
                "Not all device properties are returned on all operating systems. A zero, "
                "empty or -1 value for a property in the returned structs can mean that "
                "the information could not be returned.\n";
static char seeAlsoString[] = "";

PsychError PSYCHHIDGetDevices(void) 
{
    pRecDevice 			currentDevice=NULL;
    
    const char *deviceFieldNames[]={"usagePageValue", "usageValue", "usageName", "index", "transport", "vendorID", "productID", "version", 
                                    "manufacturer", "product", "serialNumber", "locationID", "interfaceID", "totalElements", "features", "inputs", 
                                    "outputs", "collections", "axes", "buttons", "hats", "sliders", "dials", "wheels"};
    int numDeviceStructElements, numDeviceStructFieldNames=24, deviceIndex, deviceClass;
    PsychGenericScriptType	*deviceStruct;		
    char usageName[PSYCH_HID_MAX_DEVICE_ELEMENT_USAGE_NAME_LENGTH];

    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));

    if (PsychCopyInIntegerArg(1, FALSE, &deviceClass)) {
	// Operating system specific enumeration of devices, selected by
	// deviceClass:
	return(PsychHIDEnumerateHIDInputDevices(deviceClass));
    }

    PsychHIDVerifyInit();
    numDeviceStructElements=(int)HIDCountDevices();
    PsychAllocOutStructArray(1, FALSE, numDeviceStructElements, numDeviceStructFieldNames, deviceFieldNames, &deviceStruct);
    deviceIndex=0;
    for(currentDevice=HIDGetFirstDevice(); currentDevice != NULL; currentDevice=HIDGetNextDevice(currentDevice)){
        PsychSetStructArrayDoubleElement("usagePageValue",	deviceIndex, 	(double)currentDevice->usagePage,	deviceStruct);
        PsychSetStructArrayDoubleElement("usageValue",		deviceIndex, 	(double)currentDevice->usage, 		deviceStruct);
        #if PSYCH_SYSTEM == PSYCH_OSX
            HIDGetUsageName (currentDevice->usagePage, currentDevice->usage, usageName);
        #else
            // TODO FIXME Usage name: Mapping of usagePage + usage to human readable string
            // is to be done for Linux/Windows: HIDGetUsageName (currentDevice->usagePage, currentDevice->usage, usageName);
            sprintf(usageName, "TBD");        
        #endif
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
	#if PSYCH_SYSTEM == PSYCH_OSX
	// Store dummy value -1 to mark interfaceID as invalid/unknown:
        PsychSetStructArrayDoubleElement("interfaceID",		deviceIndex, 	(double) -1,                             deviceStruct);
	#else
	// USB interface id only available on non OS/X:
        PsychSetStructArrayDoubleElement("interfaceID",		deviceIndex, 	(double)currentDevice->interfaceId,     deviceStruct);
	#endif
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
