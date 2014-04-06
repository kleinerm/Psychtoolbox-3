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
static char synopsisString[] =  "Set video capture parameter 'parameterName' on video capture device 'capturePtr'.\n"
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
                                "'Exposure', 'Shutter', 'Sharpness', 'Hue', 'Saturation', 'Gamma', 'Iris', 'Focus', "
                                "'Zoom', 'Pan', 'Tilt', 'OpticalFilter', 'CaptureSize', 'CaptureQuality', 'FrameRate', "
                                "'TriggerDelay'. The special setting "
                                "'PrintParameters' prints all features to the command window. 'GetVendorname' and 'GetModelname' "
                                "return the name of the device vendor, resp. the device model name. 'GetROI' returns "
                                "the capture region of interest (ROI), which can deviate from the ROI requested in "
                                "Screen('OpenVideoCapture'), depending on the capabilities of the capture device. "
                                "'SetNextCaptureBinSpec=xxx' Will set the gst-launch line which describes the video "
                                "capture source to be used during the next call to Screen('OpenVideoCapture', -9, ...); "
                                "Opening a video capture device with the special deviceIndex -9 means to create a GStreamer "
                                "bin and use it as video source. The bin is created by parsing the string passed here. "
                                "Use the special 'capturePtr' value -1 when setting this bin description, as this call "
                                "may need to be made while a capture device is not yet opened, so no valid 'capturePtr' exists. "
                                "This setting is only honored on the GStreamer video capture engine.\n"
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
                                "'SetGStreamerProcessingPipeline=xxx' This assigns a GStreamer gst-launch style pipeline for "
                                "use during post-processing of captured video on a libdc1394 capture device. This is only "
                                "supported if multi-threaded video capture or recording is active via recordingflags & 16 and "
                                "video capture was started with the 'dropframes' = 0 setting, so that all captured video is "
                                "processed by a realtime thread and pushed through a GStreamer processing pipeline. You can "
                                "build a processing pipeline of standard GStreamer plugins to do something to the video frames "
                                "after they've been captured but before they are returned via Screen('GetCapturedImage'). The "
                                "processing does not have any effect on recorded video footage.\n"
                                "'StopAtFramecount' Set or retrieve the framecounter value at which the camera should stop "
                                "its capture operation. By default the camera will empty its queue of captured images before "
                                "stopping in free-running mode. In synchronized mode it will stop at the same framecounter "
                                "value as the master camera, so all cams capture the same number of frames. You can override "
                                "these defaults and specify an arbitrary value with this setting. However, be careful in synchronized "
                                "mode to not specify a target stop count which can't be reached by a slave camera once capture stops "
                                "synchronously on all cameras, otherwise stopping that camera will be impossible and result in a "
                                "hard hang of your script!\n"
                                "'GetCorruptFramecount' Retrieve current count of corrupted frames received from a camera. Not all "
                                "cameras and operating systems can detect and report corrupt frames. Currently only implemented on "
                                "libdc1394 capture engine.\n"
                                "'GetCurrentFramecount' Retrieve current count of captured frames on a camera.\n"
                                "'GetFutureMaxFramecount' Retrieve current maximum count which could be reached if you'd try to stop "
                                "capture right now.\n"
                                "'GetFetchedFramecount' Retrieve framecount of last frame fetched via Screen('GetCapturedImage'). "
                                "This is the running counter value at the time the video frame was captured, essentially a copy "
                                "of 'GetCurrentFramecount' stamped onto the retrieved video frame.\n"
                                "'PreferFormat7Modes' If set to 1, prefer Format-7 video capture modes over other modes, "
                                "even if given capture settings for fps and resolution and ROI would allow otherwise. Format-7 "
                                "modes are only supported by more high-end cams, but using them sometimes allows to save "
                                "bus-bandwidth in addition to the higher flexibility. If you find yourself starving for bandwidth "
                                "it may be worth a try to set this preference. The default setting 0 will use non-Format-7 by default "
                                "and only choose Format-7 if this is needed to satisfy given ROI, framerate and resolution settings.\n"
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
                                "quality and more cpu load. Currently values 0 to 7 may be valid for your system. This can be "
                                "also used with a 'capturePtr' of -1 to set the method used during movie playback.\n"
                                "'OverrideBayerPattern' If you choose color image output from raw sensor input, via one of the "
                                "'DataConversionMode' settings, then a bayer filter operation must be performed to convert raw "
                                "sensor data to RGB data. The filter operation must know the bayer filter layout of your cameras "
                                "sensor, but this layout can't get auto-detected on all cameras in all modes of operation. If the "
                                "bayer conversion aborts due to this, you may need to manually specify the bayer filter pattern "
                                "to use with the 'OverrideBayerPattern' setting: 0 = RGGB, 1 = GBRG, 2 = GRBG, 3 = BGGR. This can be "
                                "also used with a 'capturePtr' of -1 to set the method used during movie playback.\n"
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
                                "then all slaves to disengage them. If you need cameras to start and perform capture and recording in sync, "
                                "but you don't need them to stop in exact synchrony, you can add the flag 32 = No lock-step. This allows for "
                                "some slack and inaccuracy in stopping capture and recording, but may allow for reduced latency for realtime "
                                "applications of video capture.\n"
                                "The way trigger signals are used if 'SyncMode' is selected as mode 16 aka hardware sync, can be controlled "
                                "via the following settings:\n"
                                "'TriggerMode' The way a trigger signal controls camera capture operation: 0 = Start of exposure is triggered "
                                "by falling edge of trigger signal, duration of exposure is controlled by shutter setting. 1 = Exposure starts "
                                "with falling edge and stops with rising edge. Modes 2, 3, 4 and 5 are multiple exposure modes. Modes 6 and 7 "
                                "are vendor specific modes.\n"
                                "'TriggerPolarity' 0 = Active low / falling edge. 1 = Active high / rising edge = Inverted polarity.\n"
                                "'TriggerSource' Which source input port to use: Sources 0 to 3 are available, corresponding to ports 0 - 3.\n"
                                "'GetTriggerSources' Returns a list of supported source ports for given camera.\n"
                                "'PIO' Set or get general purpose programmable IO pins on camera. Accepts/returns a 32-Bit integer value to control "
                                "up to 32 pins. The value is provided and expected as a double, ie. a uint32 cast from/to a double.\n"
                                "'1394BModeActive' Set or get if Firewire-800 mode aka 1394B mode is active: 1 = Activate, 0 = Use default classic mode.\n"
                                "'ISOSpeed' Get/Set Firewire ISO bus speed: Valid values are 100, 200, 400, 800, 1600, 3200 MBit/s. Default is 400 MBit, "
                                "higher values require '1394BModeActive' to be set to 1 - and a camera and firewire controller which support 1394B mode "
                                "and higher speeds than 400 MBit/s.\n"
                                "'GetCycleTimer' Returns the current decoded 32-Bit firewire bus cycle count in return argument 1 and the GetSecs() time at which "
                                "that count was queried in the second return argument: [firewireSeconds, GetSecsSeconds, busSeconds, busIsoCycles, busTicks]. "
                                "A busIsoCycles takes 125 usecs, busTicks are 1/24.576 Mhz on top of that. The combination of busSeconds, busIsoCycles and "
                                "busTicks is decoded into firewireSeconds for your convenience. Firewire bus time wraps to zero every 128 seconds.\n"
                                "'ResetBus' Resets the firewire or USB bus to which a camera is attached.\n"
                                "'ResetCamera' Resets the camera.\n"
                                "'Temperature' Retrieves and/or sets the target temperature (returned in 1st argument). Returns current temperature in "
                                "2nd return argument.\n"
                                "'WhiteBalance' Get/Set white balance settings: 1st argument is U (in YUV mode) or Blue value (in RGB mode). 2nd argument "
                                "is V (in YUV mode) or Red (in RGB mode). E.g., [u, v] = Screen('...', camera, 'WhiteBalance' [, newU, newV]);\n"
                                "'WhiteShading' Sets or gets white shading correction point: In/Out values 1, 2 and 3 correspond to Red, Green and Blue, e.g., "
                                "[r,g,b] = Screen('...', camera, 'WhiteShading' [, newR, newG, newB]);\n"
                                "'BaslerChecksumEnable' Enable extra CRC checksumming on Basler cameras, to detect corrupt video frames. Compute intense!\n"
                                "'BaslerFrameTimestampEnable' Use Basler cameras builtin timestamping of start of image exposure to compute extra precise "
                                "capture timestamps. By default, the time of reception of a frame in the host computer is timestamped. Doesn't work on many "
                                "Basler cameras though.\n"
                                "'BaslerFrameCounterEnable' Retrieve framecounter values from Basler camera itself instead of using our own software frame "
                                "counter. Theoretically extra robust. In practice only useful if your Basler camera allows software controlled power-cycling, "
                                "which some Basler cameras do not allow. If the camera doesn't allow power-cycling then use of this feature will cause a hard "
                                "hang of Psychtoolbox!\n"
                                "'LoadMarkerTrackingPlugin=' Specify the name of a special markertracker plugin to load and use during video capture. The name "
                                "must be the path and filename of a shared library which implements this plugin. EXPERIMENTAL and subject to change without notice!\n"
                                "'SendCommandToMarkerTrackingPlugin=' Send an ASCII string containing commands to a loaded markertracker plugin. EXPERIMENTAL!\n";

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

    PsychErrorExit(PsychCapNumInputArgs(5));            // Max. 5 input args.
    PsychErrorExit(PsychRequireNumInputArgs(2));        // Min. 2 input args required.
    PsychErrorExit(PsychCapNumOutputArgs(5));           // Up to 5 output args.
    
    // Get the device handle:
    PsychCopyInIntegerArg(1, TRUE, &capturehandle);
    if (capturehandle < -1) {
        PsychErrorExitMsg(PsychError_user, "SetVideoCaptureParameter called without either the special handle '-1' or a valid handle to a capture object.");
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
