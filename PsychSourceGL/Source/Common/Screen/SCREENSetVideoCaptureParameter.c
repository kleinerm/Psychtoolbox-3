/*
 
 Psychtoolbox3/Source/Common/SCREENSetVideoCaptureParameter.c		
 
 AUTHORS:
 
 mario.kleiner at tuebingen.mpg.de   mk
 
 PLATFORMS:	
 
 This file should build on any platform. 
 
 HISTORY:
 4/18/06  mk		Created. 
 
 DESCRIPTION:
 
 Change capture setting on a previously opened video capture device.
 
 TO DO:
 
 */

#include "Screen.h"
#include <float.h>

static char useString[] = "oldvalue = Screen('SetVideoCaptureParameter', capturePtr, 'parameterName' [, value]);";
static char synopsisString[] = "Set video capture parameter 'parameterName' on video capture device 'capturePtr'. "
                               "If 'value' is provided, then the parameter is set to 'value' and the parameter is "
                               "switched to manual control mode. If 'value' is left out, then the current setting "
                               "of 'parameterName' is queried and returned. If 'parameterName' starts with the word "
                               "'Auto' then the parameter is switched to automatic control, if supported. In any "
                               "case, the old value is returned. Type and range of supported capture parameters "
                               "varies between different capture devices and operating systems. Your specific device "
                               "may only support a subset (or none) of the available parameters. If you specify an "
                               "unsupported 'parameterName', your request will be silently ignored, except that the "
                               "return value is set to DBL_MAX - a very high number. The following settings are "
                               "currently available on devices that support them: 'Brightness', 'Gain', "
                               "'Exposure', 'Shutter', 'Sharpness', 'Saturation', 'Gamma'. The special setting "
                               "'PrintParameters' prints all features to the command window. 'GetVendorname' and 'GetModelname' "
                               "return the name of the device vendor, resp. the device model name. 'GetROI' returns "
                               "the capture region of interest (ROI), which can deviate from the ROI requested in "
                               "Screen('OpenVideoCapture'), depending on the capabilities of the capture device. "
                               "'GetFramerate' Returns the nominal capture rate of the capture device. ";

static char seeAlsoString[] = "OpenVideoCapture CloseVideoCapture StartVideoCapture StopVideoCapture GetCapturedImage";
	 
PsychError SCREENSetVideoCaptureParameter(void) 
{
    int capturehandle = -1;
    double value = DBL_MAX;
    double oldvalue;
    char* pname = NULL;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    PsychErrorExit(PsychCapNumInputArgs(3));            // Max. 3 input args.
    PsychErrorExit(PsychRequireNumInputArgs(2));        // Min. 2 input args required.
    PsychErrorExit(PsychCapNumOutputArgs(1));           // One output arg.
    
    // Get the device handle:
    PsychCopyInIntegerArg(1, TRUE, &capturehandle);
    if (capturehandle==-1) {
        PsychErrorExitMsg(PsychError_user, "SetVideoCaptureParameter called without valid handle to a capture object.");
    }
    
    // Copy in parameter name string:
    PsychAllocInCharArg(2, TRUE, &pname);
    if (pname == NULL) {
        PsychErrorExitMsg(PsychError_user, "SetVideoCaptureParameter called without a parameter name string.");
    }

    // Copy in (optional) value for parameter:
    PsychCopyInDoubleArg(3, FALSE, &value);

    // Try to set parameter:
    oldvalue = PsychVideoCaptureSetParameter(capturehandle, pname, value);

    // Return old value of capture parameter:
    if (strstr(pname, "Get")==NULL) {
      PsychCopyOutDoubleArg(1, FALSE, oldvalue);
    }

    // Ready!    
    return(PsychError_none);
}

