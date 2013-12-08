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
static char synopsisString[] =  "Set video capture parameter 'parameterName' on video capture device 'capturePtr'. "
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
                                "'GetFramerate' Returns the nominal capture rate of the capture device.\n"
                                "'GetBandwidthUsage' Returns firewire bandwidth used by camera at current settings in "
                                "so called bandwidth units. "
                                "The 1394 bus has 4915 bandwidth units available per cycle. Each unit corresponds to "
                                "the time it takes to send one quadlet at ISO speed S1600. The bandwidth usage at S400 "
                                "is thus four times the number of quadlets per packet. In other words, it is complicated. "
                                "This returns normalized bus bandwidth usage between 0.0 for 0% and 1.0 for 100%, assuming "
                                "4915 units correspond to 100% bus utilization.\n"
                                "'SetNewMoviename=xxx' Will change the name of the videofile used for video "
                                "recording to xxx. This allows you to change target files for video recordings "
                                "without the need to close and reopen the capture device. You must stop capture "
                                "though and then restart it after assigning a new filename.\n"
                                "'DataConversionMode' What kind of image data should be requested from the camera and "
                                "how should it be postprocessed? This controls the tradeoff between required bus-bandwidth "
                                "on Firewire/USB busses and processing load on the cpu or gpu of the host computer, allowing "
                                "to prioritize one over the other. Possible settings: 0 = Don't care (Default setting), "
                                "1 = Request raw data and return it unprocessed. 2 = Request raw data but post-process "
                                "it into a standard format (Luminance for 1 layer, RGB for 3 layer images), 3 = Request "
                                "mono/rgb filtered data from camera and return it unprocessed (as processing should not be needed), "
                                "4 = Request mono data but post-process it as if it were raw data (convert to mono or rgb). "
                                "Option 1 minimizes bus bandwidth, memory consumption "
                                "and cpu load but requires manual post processing by you at some time. Option 2 minimizes "
                                "bus bandwidth but not memory consumption and it significantly increases cpu load. Option "
                                "3 minimizes cpu load but maximizes bus bandwidth consumption. The default option 0 will "
                                "do whatever is convenient for the attached camera. Option 1 only makes sense if you "
                                "request 1 layer data. Option 4 is like option 2, but for broken cameras which deliver raw "
                                "data in mono format instead of the spec-compliant raw format. Option 3 in combination with "
                                "1 layer image format can be used to get raw data from broken cameras which deliver raw data "
                                "mislabeled as mono data.\n"
                                "'DebayerMethod' Select method of bayer filtering for conversion of raw sensor data to RGB images. "
                                "Different methods represent different tradeoffs between quality and computation time. Method 0 "
                                "(the Default) is the fastest and lowest quality method, whereas higher numbers select higher "
                                "quality and more cpu load. Currently values 0 to 7 may be valid for your system.\n"
                                "'OverrideBayerPattern' If you choose color image output from raw sensor input, via one of the "
                                "'DataConversionMode' settings, then a bayer filter operation must be performed to convert raw "
                                "sensor data to RGB data. The filter operation must know the bayer filter layout of your cameras "
                                "sensor, but this layout can't get auto-detected on all cameras in all modes of operation. If the "
                                "bayer conversion aborts due to this, you may need to manually specify the bayer filter pattern "
                                "to use with the 'OverrideBayerPattern' setting: 0 = RGGB, 1 = GBRG, 2 = GRBG, 3 = BGGR.\n"
                                "'SyncMode' Query or set mode flags for synchronization of the video capture "
                                "operation of multiple cameras. This setting is currently only supported with "
                                "the dedicated libdc1394 video capture engine (engine id 1), and only for some "
                                "types of cameras on some operating systems, specifically as of Nov 2013 it only "
                                "works on Linux with firewire cameras. The default setting for 'SyncMode' is zero, "
                                "which means the camera is free-running, independent of any other camera. Non-zero "
                                "values allow to synchronize the capture operation of the camera with other cameras. "
                                "Each camera can be either a sync-master, which means it controls all capture operations "
                                "(start, stop and timing of capture), or a sync-slave, which means its timing is controlled "
                                "by the master camera. There can be at most one master for synchronized operation. If hardware "
                                "sync is selected and an external trigger signal source is used, no master camera is needed. "
                                "For the master camera, set the 'SyncMode' to 1, for a slave camera set it to 2. There are "
                                "three different sync strategies. Select exactly one by adding the value 4, 8 or 16 to the 'SyncMode' "
                                "value of 1 or 2, e.g., 1+4 to select soft-sync mode and define a soft-sync master:\n"
                                "4 = Soft-Sync: Synchronisation happens purely in software. Works always but less accurate!\n"
                                "8 = Bus-Sync: All cameras on a given firewire bus are synchronized down to ~125 usecs. This only "
                                "works with some cameras which support bus-sync, and usually only if they are identical camera models "
                                "with identical settings. Also it only works for cameras on the same firewire bus, e.g., multiple cams "
                                "connected to the same port on the computer via a common firewire hub or via daisy-chaining. Bus sync "
                                "is elegant if it works, with no need for extra cabling and electronics.\n"
                                "16 = Hw-Sync: All slave cameras are connected via some dedicated trigger signal distribution cables "
                                "to a master camera or to an external trigger signal source. The master camera or external source drives "
                                "capture operations and timing of all connected slave cameras. This is most reliable and accurate, but "
                                "requires extra cabling and hardware. If you use this option, also use the trigger configuration "
                                "commmands below to configure each camera individually as a master or slave, the trigger source port, "
                                "and the type and polarity of the external trigger signal to use.\n"
                                "In any case, for synchronized operation you must always first start capture on all participating slave "
                                "cameras, then finally on the designated master - which will actually truly start capture of the whole "
                                "pack. For stopping capture you first stop the master, which will actually stop capture of the whole setup, "
                                "then all slaves to disengage them.\n"
                                "The way trigger signals are used if 'SyncMode' is selected as mode 16 aka hardware sync, can be controlled "
                                "via the following settings:\n"
                                "'TriggerMode' The way a trigger signal controls camera capture operation: 0 = Start of exposure is triggered "
                                "by falling edge of trigger signal, duration of exposure is controlled by shutter setting. 1 = Exposure starts "
                                "with falling edge and stops with rising edge. Modes 2, 3, 4 and 5 are multiple exposure modes. Modes 6 and 7 "
                                "are vendor specific modes.\n"
                                "'TriggerPolarity' 0 = Active low / falling edge. 1 = Active high / rising edge = Inverted polarity.\n"
                                "'TriggerSource' Which source input port to use: Sources 0 to 3 are available, corresponding to ports 0 - 3.\n"
                                "'GetTriggerSources' Returns a list of supported source ports for given camera.\n";

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
