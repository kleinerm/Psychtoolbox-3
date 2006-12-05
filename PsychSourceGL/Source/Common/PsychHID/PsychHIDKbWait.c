/*
	PsychtoolboxGL/Source/Common/PsychHID/PsychHIDWait.c		
  
	PROJECTS: 
	
		PsychHID only.
  
	PLATFORMS:  
	
		Only OS X for now.  
  
	AUTHORS:
	
		Allen.Ingling@nyu.edu		awi 
      
	HISTORY:
		6/5/03  awi		Created.
		4/7/05	awi		Added a wait to to the polling loop. 
  
	NOTES:
	
		We add a compile flag to the project MachTimebase settings: "-Wno-long-double" turns off a warning caused by using type long double.  
		The warning is:
			use of `long double' type; its size may change in a future release (Long double usage is reported only once for each file.
			To disable this warning, use -Wno-long-double.)
			
		Turning off the warning is probably ok because we do not depend on long doubles being any particular size or precision; if does change,
		that's not going to break anything.  
			
		The -Wno-long-double flag is appended to the project setting "Other Warning Flags".

		---
    

*/

#include "PsychHID.h"

static char useString[]= "secs=PsychHID('KbWait', [deviceNumber])";
static char synopsisString[] = 
        "Scan a keyboard device and return wait for a keypress "
        "By default the first keyboard device (the one with the lowest device number) is "
        "scanned.  Optionally, the device number of any keyboard may be specified "
        "The PsychHID('KbWait') implements the KbCheck command a provided by the  OS 9 Psychtoolbox."
        "KbWait is defined in the OS X Psychtoolbox and invokes PsychHID('KbWait').  For backwards "
        "compatability with earlier Psychtoolbox we recommend using KbWait instead of PsychHID('KbWait'). ";
        
static char seeAlsoString[] = "";

 
PsychError PSYCHHIDKbWait(void) 
{
    pRecDevice          	deviceRecord;
    pRecElement			currentElement;
    int				i, deviceIndex, numDeviceIndices;
    long			KeysUsagePage=7;
    long			KbDeviceUsagePage= 1, KbDeviceUsage=6; 
    int				deviceIndices[PSYCH_HID_MAX_KEYBOARD_DEVICES]; 
    pRecDevice			deviceRecords[PSYCH_HID_MAX_KEYBOARD_DEVICES];
    boolean 			isDeviceSpecified, foundUserSpecifiedDevice;
    double			*timeValueOutput;
    boolean 			isKeyDown;
    	 

    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
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
    PsychAllocOutDoubleArg(1, FALSE, &timeValueOutput);
          
    //step through the elements of the device.  Set flags in the return array for down keys.
    isKeyDown=FALSE;
    while(!isKeyDown){
		PsychWaitIntervalSeconds((double)0.004);  //surrender some time to other processes	
        for(currentElement=HIDGetFirstDeviceElement(deviceRecord, kHIDElementTypeInput); 
            currentElement != NULL; 
            currentElement=HIDGetNextDeviceElement(currentElement, kHIDElementTypeInput))
        {
            if(currentElement->usagePage==KeysUsagePage && currentElement->usage <= 256 && currentElement->usage >=1){
                if(HIDGetElementValue(deviceRecord, currentElement)){
                    isKeyDown=TRUE;
                    break;  //break out of inner for loop.  
                }
            }
        }
    }
    PsychGetPrecisionTimerSeconds(timeValueOutput);
        
    return(PsychError_none);	
}

