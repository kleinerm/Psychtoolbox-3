/*
	PsychtoolboxGL/Source/Common/PsychHID/PsychHIDKbQueue.h	
  
	PROJECTS: 
	
		PsychHID only.
  
	PLATFORMS:  
	
		Only OS X for now.  
  
	AUTHORS:
	
		rwoods@ucla.edu		rpw 
      
	HISTORY:
		8/12/07  rpw		Created.
  
	NOTES:


		---
    

*/

#include <IOKit/hid/IOHIDUsageTables.h>
#include <pthread.h>

typedef struct HIDData
{
    IOHIDDeviceInterface122 ** 	hidDeviceInterface;
    IOHIDQueueInterface **      hidQueueInterface;
    CFDictionaryRef             hidElementDictionary;
	CFRunLoopSourceRef			eventSource;
} HIDData;

typedef HIDData *HIDDataRef;

typedef struct HIDElement {
    //SInt32		currentValue;
    SInt32		usagePage;
    SInt32		usage;
    // IOHIDElementType	type;
    IOHIDElementCookie	cookie;
    //HIDDataRef          owner;
}HIDElement;

typedef HIDElement *HIDElementRef;

