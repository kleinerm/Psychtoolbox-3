/*
  PsychToolbox3/Source/Common/PsychHID/PsychHIDKbCheck.c		
  
  PROJECTS: PsychHID only.
  
  PLATFORMS:  Only OS X so far.  
  
  AUTHORS:
  Allen.Ingling@nyu.edu		awi 
      
  HISTORY:
  5/12/03  awi		Created.
  
  TO DO:
  
    write PsychHIDGetDeviceListByUsage
  
    define variables
  

*/

#include "PsychHID.h"

static char useString[]= "[keyIsDown,secs,keyCode]=PsychHID('KbCheck' [, deviceNumber][, scanList])";
static char synopsisString[] = 
        "Scan a keyboard device and return a vector of logical values indicating the "
        "state of each key.  By default the first keyboard device (the one with the lowest device number) is "
        "scanned.  Optionally, the device number of any keyboard may be specified in the argument "
		"'deviceNumber'. As checking all potentially 256 keys on a HID device is a time consuming process, "
		"which can easily take up to 1 msec on modern hardware, you can restrict the scan to a subset of "
		"the 256 keys by providing the optional 'scanList' parameter: 'scanList' must be a vector of 256 "
		"doubles, where the i'th element corresponds to the i'th key and a zero value means: Ignore this "
		"key during scan, whereas a positive non-zero value means: Scan this key.\n"
        "The PsychHID('KbCheck') implements the KbCheck command as provided by the  OS 9 Psychtoolbox. "
        "KbCheck is defined in the OS X Psychtoolbox and invokes PsychHID('KbCheck'). "
        "Always use KbCheck instead of directly calling PsychHID('KbCheck'), unless you have very good "
		"reasons to do otherwise and really know what you're doing!";
        
static char seeAlsoString[] = "";

 
PsychError PSYCHHIDKbCheck(void) 
{
    pRecDevice			deviceRecord;
    pRecElement			currentElement;
    int					i, deviceIndex, debuglevel = 0;
	static int			numDeviceIndices = -1;
    long				KeysUsagePage=7;
    long				KbDeviceUsagePage= 1, KbDeviceUsage=6; 
    static int			deviceIndices[PSYCH_HID_MAX_KEYBOARD_DEVICES]; 
    static pRecDevice	deviceRecords[PSYCH_HID_MAX_KEYBOARD_DEVICES];
    boolean				isDeviceSpecified, foundUserSpecifiedDevice, isKeyArgPresent, isTimeArgPresent;
    double				*timeValueOutput, *isKeyDownOutput, *keyArrayOutput;
	int					m, n, p;
	double				*scanList = NULL;
	
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(2));
	
	// We query keyboard devices only on first invocation, then cache and recycle the data:
	if (numDeviceIndices == -1) {
		PsychHIDVerifyInit();
        PsychHIDGetDeviceListByUsage(KbDeviceUsagePage, KbDeviceUsage, &numDeviceIndices, deviceIndices, deviceRecords);
	}
	
    // Choose the device index and its record
    isDeviceSpecified=PsychCopyInIntegerArg(1, FALSE, &deviceIndex);
    if(isDeviceSpecified){  //make sure that the device number provided by the user is really a keyboard.
		if (deviceIndex < 0) {
			debuglevel = 1;
			deviceIndex = -deviceIndex;
		}

        for(i=0;i<numDeviceIndices;i++){
            if(foundUserSpecifiedDevice=(deviceIndices[i]==deviceIndex))
                break;
        }
        if(!foundUserSpecifiedDevice)
            PsychErrorExitMsg(PsychError_user, "Specified device number is not a keyboard device.");
    }else{ // set the keyboard device to be the first keyboard device
        i=0;
        if(numDeviceIndices==0)
            PsychErrorExitMsg(PsychError_user, "No keyboard devices detected.");
        else{
            deviceIndex=deviceIndices[i];
        }
    }
    deviceRecord=deviceRecords[i]; 

	// Optional 2nd argument 'scanlist' provided?
	if (PsychAllocInDoubleMatArg(2, FALSE, &m, &n, &p, &scanList)) {
		// Yep. Matching size?
		if (p!=1 || m * n != 256) PsychErrorExitMsg(PsychError_user, "Provided 'scanList' parameter is not a vector of 256 doubles, as required!");
	}

    // Allocate and init out return arguments.
    isKeyArgPresent = PsychAllocOutDoubleMatArg(3, FALSE, 1, 256, 1, &keyArrayOutput);
    isTimeArgPresent = PsychAllocOutDoubleArg(2, FALSE, &timeValueOutput);
    PsychGetPrecisionTimerSeconds(timeValueOutput);
    PsychAllocOutDoubleArg(1, FALSE, &isKeyDownOutput);
    *isKeyDownOutput=(double)FALSE;
	memset((void*) keyArrayOutput, 0, sizeof(double) * 256);
	
    //step through the elements of the device.  Set flags in the return array for down keys.
    for(currentElement=HIDGetFirstDeviceElement(deviceRecord, kHIDElementTypeInput); 
        currentElement != NULL; 
        currentElement=HIDGetNextDeviceElement(currentElement, kHIDElementTypeInput))
    {
        if((currentElement->usagePage==KeysUsagePage && currentElement->usage <= 256 && currentElement->usage >=1) &&
			( (scanList == NULL) || (scanList[currentElement->usage - 1] > 0) ) ) {
            if (debuglevel > 0) printf("PTB-DEBUG: [KbCheck]: usage: %x value: %d \n", currentElement->usage, HIDGetElementValue(deviceRecord, currentElement));
            keyArrayOutput[currentElement->usage - 1]=((int) HIDGetElementValue(deviceRecord, currentElement) || (int) keyArrayOutput[currentElement->usage - 1]);
            *isKeyDownOutput= keyArrayOutput[currentElement->usage - 1] || *isKeyDownOutput; 
        }
    }

    return(PsychError_none);	
}
