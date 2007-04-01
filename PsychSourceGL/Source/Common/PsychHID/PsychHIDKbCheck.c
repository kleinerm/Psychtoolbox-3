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

static char useString[]= "[keyIsDown,secs,keyCode]=PsychHID('KbCheck', [deviceNumber])";
static char synopsisString[] = 
        "Scan a keyboard device and return a vector of logical values indicating the "
        "state of each key.  By default the first keyboard device (the one with the lowest device number) is "
        "scanned.  Optionally, the device number of any keyboard may be specified "
        "The PsychHID('KbCheck') implements the KbCheck command a provided by the  OS 9 Psychtoolbox."
        "KbCheck is defined in the OS X Psychtoolbox and invokes PsychHID('KbCheck').  For backwards "
        "compatability with earlier Psychtoolbox we recommend using KbCheck instead of PsychHID('KbCheck'). ";
        
static char seeAlsoString[] = "";

 
PsychError PSYCHHIDKbCheck(void) 
{
    pRecDevice          	deviceRecord;
    pRecElement			currentElement;
    int				i, deviceIndex, numDeviceIndices;
    long			KeysUsagePage=7;
    long			KbDeviceUsagePage= 1, KbDeviceUsage=6; 
    int				deviceIndices[PSYCH_HID_MAX_KEYBOARD_DEVICES]; 
    pRecDevice			deviceRecords[PSYCH_HID_MAX_KEYBOARD_DEVICES];
    boolean 			isDeviceSpecified, foundUserSpecifiedDevice, isKeyArgPresent, isTimeArgPresent;
    double			*timeValueOutput, *isKeyDownOutput, *keyArrayOutput;
    	 

    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(1));  	//Specifies the number of the keyboard to scan.  
    
    PsychHIDVerifyInit();
    
    //Choose the device index and its record
    PsychHIDGetDeviceListByUsage(KbDeviceUsagePage, KbDeviceUsage, &numDeviceIndices, deviceIndices, deviceRecords);  
    isDeviceSpecified=PsychCopyInIntegerArg(1, FALSE, &deviceIndex);
    if(isDeviceSpecified){  //make sure that the device number provided by the user is really a keyboard.
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

    //Allocate and init out return arguments.  
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
        if(currentElement->usagePage==KeysUsagePage && currentElement->usage <= 256 && currentElement->usage >=1){
            //printf("usage: %x value: %d \n", currentElement->usage, HIDGetElementValue(deviceRecord, currentElement));
            keyArrayOutput[currentElement->usage - 1]=((int) HIDGetElementValue(deviceRecord, currentElement) || (int) keyArrayOutput[currentElement->usage - 1]);
            *isKeyDownOutput= keyArrayOutput[currentElement->usage - 1] || *isKeyDownOutput; 
        }
    }
        
        
    return(PsychError_none);	
}

