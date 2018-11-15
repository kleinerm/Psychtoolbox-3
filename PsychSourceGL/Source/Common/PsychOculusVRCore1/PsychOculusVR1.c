/*
 * PsychToolbox/Source/Common/PsychOculusVRCore1/PsychOculusVR1.c
 *
 * PROJECTS: PsychOculusVRCore1 only.
 *
 * AUTHORS:
 *
 * mario.kleiner.de@gmail.com   mk
 *
 * PLATFORMS:   All.
 *
 * DESCRIPTION:
 *
 * A Psychtoolbox driver for the Oculus VR virtual reality
 * head sets, using the OculusVR 1.0 SDK and runtime and later.
 *
 */

#include "PsychOculusVR1.h"
#include "PsychTimeGlue.h"

// Need this _USE_MATH_DEFINES so MSVC 2010 knows M_PI
#define _USE_MATH_DEFINES
#include <math.h>

// Includes from Oculus SDK 1:
#include "OVR_CAPI.h"
#include "OVR_CAPI_GL.h"
#include "Extras/OVR_CAPI_Util.h"

// Number of maximum simultaneously open VR devices:
#define MAX_PSYCH_OCULUS_DEVS 10
#define MAX_SYNOPSIS_STRINGS 40

//declare variables local to this file.
static const char *synopsisSYNOPSIS[MAX_SYNOPSIS_STRINGS];

// Our device record:
typedef struct PsychOculusDevice {
    ovrSession          hmd;
    ovrHmdDesc          hmdDesc;
    ovrTextureSwapChain textureSwapChain[2];
    ovrMirrorTexture    mirrorTexture;
    psych_bool          isStereo;
    int                 textureWidth;
    int                 textureHeight;
    psych_bool          isTracking;
    ovrSizei            texSize[2];
    ovrFovPort          ofov[2];
    ovrEyeRenderDesc    eyeRenderDesc[2];
    ovrMatrix4f         timeWarpMatrices[2];
    ovrPosef            headPose;
    uint32_t            frameIndex;
    ovrPosef            outEyePoses[2];
    double              sensorSampleTime;
    unsigned char       rgbColorOut[3];
    psych_bool          latencyTestActive;
    //TODO    ovrFrameTiming frameTiming;
} PsychOculusDevice;

PsychOculusDevice oculusdevices[MAX_PSYCH_OCULUS_DEVS];
static int available_devices = 0;
static unsigned int devicecount = 0;
static int verbosity = 3;
static psych_bool initialized = FALSE;
static ovrErrorInfo errorInfo;

void InitializeSynopsis(void)
{
    int i = 0;
    const char **synopsis = synopsisSYNOPSIS;

    synopsis[i++] = "PsychOculusVRCore1 - A Psychtoolbox driver for Oculus VR hardware.\n";
    synopsis[i++] = "This driver allows to control devices supported by the Oculus runtime V1.0 and higher.\n";
    synopsis[i++] = "The PsychOculusVRCore1 driver is licensed to you under the terms of the MIT license.";
    synopsis[i++] = "See 'help License.txt' in the Psychtoolbox root folder for more details.\n";
    synopsis[i++] = "\n";
    synopsis[i++] = "The driver requires the Oculus VR runtime version 1.0 or later to work.\n";
    synopsis[i++] = "\n";
    synopsis[i++] = "Usage:";
    synopsis[i++] = "\n";
    synopsis[i++] = "Functions used by regular user scripts:\n";
    synopsis[i++] = "\n";
    synopsis[i++] = "oldVerbosity = PsychOculusVRCore1('Verbosity' [, verbosity]);";
    synopsis[i++] = "numHMDs = PsychOculusVRCore1('GetCount');";
    synopsis[i++] = "[oculusPtr, modelName, resolutionX, resolutionY, refreshHz] = PsychOculusVRCore1('Open' [, deviceIndex=0]);";
    synopsis[i++] = "PsychOculusVRCore1('Close' [, oculusPtr]);";
    synopsis[i++] = "showHSW = PsychOculusVRCore1('GetHSWState', oculusPtr [, dismiss]);";
    synopsis[i++] = "oldPersistence = PsychOculusVRCore1('SetLowPersistence', oculusPtr [, lowPersistence]);";
    synopsis[i++] = "oldDynamicPrediction = PsychOculusVRCore1('SetDynamicPrediction', oculusPtr [, dynamicPrediction]);";
    synopsis[i++] = "PsychOculusVRCore1('Start', oculusPtr);";
    synopsis[i++] = "PsychOculusVRCore1('Stop', oculusPtr);";
    synopsis[i++] = "state = PsychOculusVRCore1('GetTrackingState', oculusPtr [, predictionTime=nextFrame]);";
    synopsis[i++] = "[projL, projR] = PsychOculusVRCore1('GetStaticRenderParameters', oculusPtr [, clipNear=0.01][, clipFar=10000.0]);";
    synopsis[i++] = "[eyePoseL, eyePoseR, tracked, frameTiming] = PsychOculusVRCore1('StartRender', oculusPtr);";
    synopsis[i++] = "[eyePose, eyeIndex] = PsychOculusVRCore1('GetEyePose', oculusPtr, renderPass);";
    synopsis[i++] = "\n";
    synopsis[i++] = "Functions usually only used internally by Psychtoolbox:\n";
    synopsis[i++] = "\n";
    synopsis[i++] = "[width, height, fovPort] = PsychOculusVRCore1('GetFovTextureSize', oculusPtr, eye [, fov=[HMDRecommended]][, pixelsPerDisplay=1]);";
    synopsis[i++] = "[width, height, numTextures] = PsychOculusVRCore1('CreateRenderTextureChain', oculusPtr, eye, width, height);";
    synopsis[i++] = "texObjectHandle = PsychOculusVRCore1('GetNextTextureHandle', oculusPtr, eye);";
    synopsis[i++] = "texObjectHandle = PsychOculusVRCore1('CreateMirrorTexture', oculusPtr, width, height);";
    synopsis[i++] = "[width, height, viewPx, viewPy, viewPw, viewPh, pptax, pptay, hmdShiftx, hmdShifty, hmdShiftz] = PsychOculusVRCore1('GetUndistortionParameters', oculusPtr, eye [, inputWidth][, inputHeight][, fov]);";
    synopsis[i++] = "[eyeRotStartMatrix, eyeRotEndMatrix] = PsychOculusVRCore1('GetEyeTimewarpMatrices', oculusPtr, eye [, waitForTimewarpPoint=0]);";
    synopsis[i++] = "PsychOculusVRCore1('EndFrameRender', oculusPtr);";
    synopsis[i++] = "frameTiming = PsychOculusVRCore1('PresentFrame', oculusPtr);";
    synopsis[i++] = "result = PsychOculusVRCore1('LatencyTester', oculusPtr, cmd);";
    synopsis[i++] = NULL;  //this tells PsychOculusVRDisplaySynopsis where to stop

    if (i > MAX_SYNOPSIS_STRINGS) {
        PrintfExit("%s: Increase dimension of synopsis[] from %ld to at least %ld and recompile.", __FILE__, (long) MAX_SYNOPSIS_STRINGS, (long) i);
    }
}

PsychError PsychOculusVR1DisplaySynopsis(void)
{
    int i;

    for (i = 0; synopsisSYNOPSIS[i] != NULL; i++)
        printf("%s\n",synopsisSYNOPSIS[i]);

    return(PsychError_none);
}

static double deg2rad(double deg)
{
    return deg / 180.0 * M_PI;
}

static double rad2deg(double rad)
{
    return rad / M_PI * 180.0;
}

PsychOculusDevice* PsychGetOculus(int handle, psych_bool dontfail)
{
    if (handle < 1 || handle > MAX_PSYCH_OCULUS_DEVS || oculusdevices[handle-1].hmd == NULL) {
        if (!dontfail) {
            printf("PTB-ERROR: Invalid Oculus device handle %i passed. No such device open.\n", handle);
            PsychErrorExitMsg(PsychError_user, "Invalid Oculus handle.");
        }

        return(NULL);
    }

    return(&(oculusdevices[handle-1]));
}

static void OVR_CDECL PsychOculusLogCB(uintptr_t userData, int level, const char* message)
{
    (void) userData;

    if ((level == ovrLogLevel_Error && verbosity > 0) ||
        (level == ovrLogLevel_Info && verbosity > 2)  ||
        (level == ovrLogLevel_Debug && verbosity > 4)) {
            printf("PsychOculusVRCore1-Runtime: %s\n", message);
    }
}

void PsychOculusVRCheckInit(psych_bool dontfail)
{
    ovrResult result;
    ovrHmdDesc hmdDesc;
    ovrInitParams iparms;
    memset(&iparms, 0, sizeof(iparms));
    iparms.Flags = ovrInit_RequestVersion | ovrInit_Debug; // Use debug libraries. TODO: Remove for final release!
    iparms.RequestedMinorVersion = OVR_MINOR_VERSION;
    iparms.LogCallback = PsychOculusLogCB;
    iparms.UserData = 0;  // Userdata pointer, currently NULL.
    iparms.ConnectionTimeoutMS = 0; // Default timeout.

    // Already initialized? No op then.
    if (initialized) return;

    // Initialize Oculus VR runtime with default parameters:
    result = ovr_Initialize(&iparms);
    if (OVR_SUCCESS(result)) {
        if (verbosity >= 3) printf("PsychOculusVRCore1-INFO: Oculus VR runtime (version '%s') initialized.\n", ovr_GetVersionString());

        // Poll for existence of a HMD (detection timeout = 0 msecs == poll only).
        // The 1.11 SDK still seems to be unable to enumerate more than 1 HMD,
        // so we still can't count how many there are:
        ovrDetectResult result = ovr_Detect(0);
        available_devices = (result.IsOculusHMDConnected) ? 1 : 0;
        if (!result.IsOculusServiceRunning) {
            available_devices = -1;
            if (verbosity >= 2) printf("PsychOculusVRCore1-WARNING: Could not connect to Oculus VR server process yet. Did you forget to start it?\n");
        }

        if (verbosity >= 3) printf("PsychOculusVRCore1-INFO: At startup there are %i Oculus HMDs available.\n", available_devices);
        initialized = TRUE;
    }
    else {
        if (!dontfail) {
            //ovrErrorInfo errorInfo;
            //ovr_GetLastErrorInfo(&errorInfo);
            printf("PsychOculusVRCore1-ERROR: ovr_Initialize failed: %i\n", result); //errorInfo.ErrorString);
            PsychErrorExitMsg(PsychError_system, "PsychOculusVRCore1-ERROR: Initialization of VR runtime failed. Driver disabled!");
        }
    }
}

void PsychOculusStop(int handle)
{
    PsychOculusDevice* oculus;
    oculus = PsychGetOculus(handle, TRUE);
    if (NULL == oculus || !oculus->isTracking) return;

    if (verbosity >= 4) printf("PsychOculusVRCore1-INFO: Tracking stopped on device with handle %i.\n", handle);

    // Mark tracking as stopped:
    oculus->isTracking = FALSE;

    // Initialize eye poses to an identity mapping, where both eyes
    // just stare straight ahead from the origin. All values need to
    // be zero - provided by memset zero init below - except
    // for the w component of the Orientation quaternion.
    // This provides proper head-locking of frames submitted to the VR
    // compositor if rendering without head tracking is in use, e.g., for
    // standard 2D stereo rendering, movie playback etc., or for (ab)use
    // of the HMD as a "strapped onto the head" mono- or stereo-monitor:
    memset(oculus->outEyePoses, 0, 2 * sizeof(oculus->outEyePoses[0]));
    oculus->outEyePoses[0].Orientation.w = 1;
    oculus->outEyePoses[1].Orientation.w = 1;

    return;
}

void PsychOculusClose(int handle)
{
    PsychOculusDevice* oculus;
    oculus = PsychGetOculus(handle, TRUE);
    if (NULL == oculus) return;

    // Stop device:
    PsychOculusStop(handle);

    // Destroy/Release texture swap chain to compositor:
    if (oculus->textureSwapChain[0]) {
        ovr_DestroyTextureSwapChain(oculus->hmd, oculus->textureSwapChain[0]);
        oculus->textureSwapChain[0] = NULL;
    }

    if (oculus->isStereo && oculus->textureSwapChain[1]) {
        ovr_DestroyTextureSwapChain(oculus->hmd, oculus->textureSwapChain[1]);
        oculus->textureSwapChain[1] = NULL;
    }

    // Destroy mirror texture if any:
    if (oculus->mirrorTexture) {
        ovr_DestroyMirrorTexture(oculus->hmd, oculus->mirrorTexture);
        oculus->mirrorTexture = NULL;
    }

    // Close the HMD:
    ovr_Destroy(oculus->hmd);
    oculus->hmd = NULL;
    if (verbosity >= 4) printf("PsychOculusVRCore1-INFO: Closed Oculus HMD with handle %i.\n", handle);

    // Done with this device:
    devicecount--;
}

void PsychOculusVR1Init(void) {
    int handle;

    for (handle = 0 ; handle < MAX_PSYCH_OCULUS_DEVS; handle++)
        oculusdevices[handle].hmd = NULL;

    available_devices = 0;
    devicecount = 0;
    initialized = FALSE;
}

PsychError PsychOculusVR1ShutDown(void) {
    int handle;

    if (initialized) {
        for (handle = 0 ; handle < MAX_PSYCH_OCULUS_DEVS; handle++)
            PsychOculusClose(handle);

        // Shutdown runtime:
        ovr_Shutdown();

        if (verbosity >= 4) printf("PsychOculusVRCore1-INFO: Oculus VR runtime shutdown complete.\n");
    }
    initialized = FALSE;

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1Verbosity(void)
{
    static char useString[] = "oldVerbosity = PsychOculusVRCore1('Verbosity' [, verbosity]);";
    static char synopsisString[] = "Returns and optionally sets level of 'verbosity' for driver debug output.\n"
                                   "'verbosity' = New level of verbosity: 0 = Silent, 1 = Errors only, 2 = Warnings, 3 = Info, 4 = Debug.\n";
    static char seeAlsoString[] = "";

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Return optional old verbosity setting:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, verbosity);

    // Get optional new verbosity setting:
    PsychCopyInIntegerArg(1, kPsychArgOptional, &verbosity);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1GetCount(void)
{
    static char useString[] = "numHMDs = PsychOculusVRCore1('GetCount');";
    static char synopsisString[] =  "Returns count of currently connected HMDs.\n"
                                    "Returns -1 if the runtime or server couldn't get initialized.\n";
    static char seeAlsoString[] = "Open";

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if( PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));

    ovrDetectResult result = ovr_Detect(0);
    available_devices = (result.IsOculusHMDConnected) ? 1 : 0;
    if (!result.IsOculusServiceRunning) {
        available_devices = -1;
        if (verbosity >= 2) printf("PsychOculusVRCore1-WARNING: Could not connect to Oculus VR server process yet. Did you forget to start it?\n");
    }

    PsychCopyOutDoubleArg(1, kPsychArgOptional, available_devices);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1Open(void)
{
    static char useString[] = "[oculusPtr, modelName, resolutionX, resolutionY, refreshHz] = PsychOculusVRCore1('Open' [, deviceIndex=0]);";
    //                          1          2          3            4            5                                         1
    static char synopsisString[] =
        "Open connection to Oculus VR HMD, return a 'oculusPtr' handle to it.\n\n"
        "The call tries to open the HMD with index 'deviceIndex', or the "
        "first detected HMD, if 'deviceIndex' is omitted. You can pass in a 'deviceIndex' "
        "of -1 to open an emulated HMD. It doesn't provide any sensor input, but allows "
        "some basic testing and debugging of VR software nonetheless.\n"
        "The returned handle can be passed to the other subfunctions to operate the device.\n"
        "A second return argument 'modelName' returns the model name string of the Oculus device.\n"
        "'resolutionX' and 'resolutionY' return the HMD display panels horizontal and vertical resolution.\n"
        "'refreshHz' is the nominal refresh rate of the display in Hz.\n";

    static char seeAlsoString[] = "GetCount Close";

    ovrDetectResult result;
    PsychOculusDevice* oculus;
    int deviceIndex = 0;
    int handle = 0;
    unsigned int oldCaps;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(5));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Find a free device slot:
    for (handle = 0; (handle < MAX_PSYCH_OCULUS_DEVS) && oculusdevices[handle].hmd; handle++);
    if ((handle >= MAX_PSYCH_OCULUS_DEVS) || oculusdevices[handle].hmd) PsychErrorExitMsg(PsychError_internal, "Maximum number of simultaneously open Oculus VR devices reached.");

    // Get optional Oculus device index:
    PsychCopyInIntegerArg(1, kPsychArgOptional, &deviceIndex);

    // Don't support anything than a single "default" OculusVR Rift yet - A limitation of the current SDK:
    if (deviceIndex < 0) PsychErrorExitMsg(PsychError_user, "Invalid 'deviceIndex' provided. Must be greater or equal to zero!");

    result = ovr_Detect(0);
    available_devices = (result.IsOculusHMDConnected) ? 1 : 0;
    if (!result.IsOculusServiceRunning) {
        available_devices = -1;
        if (verbosity >= 2) printf("PsychOculusVRCore1-WARNING: Could not connect to Oculus VR server process yet. Did you forget to start it?\n");
    }

    if ((deviceIndex >= 0) && (deviceIndex >= available_devices)) {
        if (verbosity >= 0) printf("PsychOculusVRCore1-ERROR: Invalid deviceIndex %i >= number of available HMDs %i.\n", deviceIndex, available_devices);
        PsychErrorExitMsg(PsychError_user, "Invalid 'deviceIndex' provided. Not enough HMDs available!");
    }

    // Zero init device structure:
    memset(&oculusdevices[handle], 0, sizeof(PsychOculusDevice));

    oculus = &oculusdevices[handle];

    // Try to open real or emulated HMD with deviceIndex:
    if (deviceIndex >= 0) {
        // The real thing:
        ovrGraphicsLuid Luid;
        ovrResult rc = ovr_Create(&(oculusdevices[handle].hmd), &Luid);
        if (OVR_FAILURE(rc) || (NULL == oculusdevices[handle].hmd)) {
            if (verbosity >= 0) {
                printf("PsychOculusVRCore1-ERROR: Failed to connect to Oculus Rift with deviceIndex %i. This could mean that the device\n", deviceIndex);
                printf("PsychOculusVRCore1-ERROR: is already in use by another application or driver.\n");
            }
            PsychErrorExitMsg(PsychError_user, "Could not connect to Rift device with given 'deviceIndex'! [ovr_Create() failed]");
        }
        else if (verbosity >= 3) {
            printf("PsychOculusVRCore1-INFO: Opened Oculus Rift with deviceIndex %i as handle %i.\n", deviceIndex, handle + 1);
        }
    }
    else {
        // Emulated: Simulate a Rift DK2.
        // TODO REMOVE oculusdevices[handle].hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
        if (verbosity >= 3) printf("PsychOculusVRCore1-INFO: Opened an emulated Oculus Rift DK2 as handle %i.\n", handle + 1);
    }

    // Query current enabled caps:
    oculus->hmdDesc = ovr_GetHmdDesc(oculus->hmd);
    oldCaps = oculus->hmdDesc.AvailableHmdCaps;

    // Stats for nerds:
    if (verbosity >= 3) {
        printf("PsychOculusVRCore1-INFO: Product: %s - Manufacturer: %s - SerialNo: %s [VID: 0x%x PID: 0x%x]\n",
               oculus->hmdDesc.ProductName, oculus->hmdDesc.Manufacturer, oculus->hmdDesc.SerialNumber, (int) oculus->hmdDesc.VendorId, (int) oculus->hmdDesc.ProductId);
        printf("PsychOculusVRCore1-INFO: Firmware version: %i.%i\n", (int) oculus->hmdDesc.FirmwareMajor, (int) oculus->hmdDesc.FirmwareMinor);
        //TODO printf("PsychOculusVRCore1-INFO: CameraFrustumHFovInRadians: %f - CameraFrustumVFovInRadians: %f\n", oculus->hmdDesc.CameraFrustumHFovInRadians, oculus->hmdDesc.CameraFrustumVFovInRadians);
        //TODO printf("PsychOculusVRCore1-INFO: CameraFrustumNearZInMeters: %f - CameraFrustumFarZInMeters:  %f\n", oculus->hmd->CameraFrustumNearZInMeters, oculus->hmd->CameraFrustumFarZInMeters);
        printf("PsychOculusVRCore1-INFO: Panel size in pixels w x h = %i x %i - Refresh rate = %f fps\n", oculus->hmdDesc.Resolution.w, oculus->hmdDesc.Resolution.h, oculus->hmdDesc.DisplayRefreshRate);
        printf("PsychOculusVRCore1-INFO: Caps: Debug device=%i\n", (oldCaps & ovrHmdCap_DebugDevice) ? 1 : 0);
        printf("PsychOculusVRCore1-INFO: ----------------------------------------------------------------------------------\n");
    }

    // Initialize eye poses to an identity mapping, where both eyes
    // just stare straight ahead from the origin. All values need to
    // be zero - provided by zero init of the oculus struct - except
    // for the w component of the Orientation quaternion.
    // This provides proper head-locking of frames submitted to the VR
    // compositor if rendering without head tracking is in use, e.g., for
    // standard 2D stereo rendering, movie playback etc., or for (ab)use
    // of the HMD as a "strapped onto the head" mono- or stereo-monitor:
    oculus->outEyePoses[0].Orientation.w = 1;
    oculus->outEyePoses[1].Orientation.w = 1;

    // Increment count of open devices:
    devicecount++;

    // Return device handle: We use 1-based handle indexing to make life easier for Octave/Matlab:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, handle + 1);

    // Return product name:
    PsychCopyOutCharArg(2, kPsychArgOptional, (const char*) oculus->hmdDesc.ProductName);

    // Horizontal and vertical panel resolution:
    PsychCopyOutDoubleArg(3, kPsychArgOptional, oculus->hmdDesc.Resolution.w);
    PsychCopyOutDoubleArg(4, kPsychArgOptional, oculus->hmdDesc.Resolution.h);

    // Panel refresh rate:
    PsychCopyOutDoubleArg(5, kPsychArgOptional, oculus->hmdDesc.DisplayRefreshRate);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1Close(void)
{
    static char useString[] = "PsychOculusVRCore1('Close' [, oculusPtr]);";
    //                                                      1
    static char synopsisString[] =
        "Close connection to Oculus Rift device 'oculusPtr'. Do nothing if no such device is open.\n"
        "If the optional 'oculusPtr' is omitted, then close all open devices and shutdown the driver, "
        "ie. perform the same cleanup as if 'clear PsychOculusVRCore1' would be executed.\n";
    static char seeAlsoString[] = "Open";

    int handle = -1;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get optional device handle:
    PsychCopyInIntegerArg(1, kPsychArgOptional, &handle);

    if (handle >= 1) {
        // Close device:
        PsychOculusClose(handle);
    }
    else {
        // No handle provided: Close all devices, shutdown driver.
        PsychOculusVR1ShutDown();
    }

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1SetDynamicPrediction(void)
{
    static char useString[] = "oldDynamicPrediction = PsychOculusVRCore1('SetDynamicPrediction', oculusPtr [, dynamicPrediction]);";
    //                                                                                          1            2
    static char synopsisString[] =
        "Enable or disable dynamic prediction mode on Oculus device 'oculusPtr'.\n"
        "'dynamicPrediction' 1 = Enable dynamic prediction, 0 = Disable dynamic prediction.\n"
        "Returns previous 'dynamic prediction ' setting.\n";
    static char seeAlsoString[] = "";

    int handle, dynamicPrediction;
    PsychOculusDevice *oculus;
    unsigned int oldCaps;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);
/* TODO REMOVE
    // Query current enabled caps:
    oldCaps = ovrHmd_GetEnabledCaps(oculus->hmd);
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) (oldCaps & ovrHmdCap_DynamicPrediction) ? 1 : 0);

    // Set new enabled HMD caps:
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &dynamicPrediction)) {
        oldCaps &= ~ovrHmdCap_DynamicPrediction;
        ovrHmd_SetEnabledCaps(oculus->hmd, oldCaps | ((dynamicPrediction > 0) ? ovrHmdCap_DynamicPrediction : 0));
    }
*/
    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1SetLowPersistence(void)
{
    static char useString[] = "oldPersistence = PsychOculusVRCore1('SetLowPersistence', oculusPtr [, lowPersistence]);";
    //                                                                                 1            2
    static char synopsisString[] =
        "Enable or disable low persistence mode on display panel of Oculus device 'oculusPtr'.\n"
        "'lowPersistence' 1 = Enable low persistence, 0 = Disable low persistence.\n"
        "In low persistence mode, the pixels will only light up for a fraction of a video "
        "refresh duration, thereby reducing motion blur due to smooth pursuit eye movements "
        "or other shuttering effects. Brightness of the display will be reduced though, and "
        "flicker sensitive people may perceive more flicker.\n"
        "Returns previous 'oldPersistence' setting.\n";
    static char seeAlsoString[] = "";

    int handle, lowPersistence;
    PsychOculusDevice *oculus;
    unsigned int oldCaps;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);
/* TODO REMOVE
    // Query current enabled caps:
    oldCaps = ovrHmd_GetEnabledCaps(oculus->hmd);
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) (oldCaps & ovrHmdCap_LowPersistence) ? 1 : 0);

    // Set new enabled HMD caps:
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &lowPersistence)) {
        oldCaps &= ~ovrHmdCap_LowPersistence;
        ovrHmd_SetEnabledCaps(oculus->hmd, oldCaps | ((lowPersistence > 0) ? ovrHmdCap_LowPersistence : 0));
    }
*/
    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1Start(void)
{
    static char useString[] = "PsychOculusVRCore1('Start', oculusPtr);";
    //                                                1
    static char synopsisString[] =
        "Start head orientation and position tracking operation on Oculus device 'oculusPtr'.\n\n";
    static char seeAlsoString[] = "Stop";

    int handle;
    PsychOculusDevice *oculus;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    if (oculus->isTracking) {
        if (verbosity >= 0) printf("PsychOculusVRCore1-ERROR: Tried to start tracking on device %i, but tracking is already started.\n", handle);
        PsychErrorExitMsg(PsychError_user, "Tried to start tracking on HMD, but tracking already active.");
    }

/* TODO REMOVE
    // Request start of tracking for retrieval of head orientation and position, with drift correction, e.g., via magnetometer.
    // Do not fail if retrieval of any of this information isn't supported by the given hardware, ie., the required set of caps is empty == 0.
    // Rift DK1 only had orientation tracking, with magnetometer based drift correction. Rift DK2 also has vision based position tracking and
    // drift correction. The software emulated Rift has none of these and just returns a "static" head. This will start tracking:
    if (!ovrHmd_ConfigureTracking(oculus->hmd, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0)) {
        if (verbosity >= 0) printf("PsychOculusVRCore1-ERROR: Failed to start tracking on device with handle %i [%s].\n", handle, ovrHmd_GetLastError(oculus->hmd));
        PsychErrorExitMsg(PsychError_system, "Start of Oculus HMD tracking failed for reason given above.");
    }
    else if (verbosity >= 4) printf("PsychOculusVRCore1-INFO: Tracking started on device with handle %i.\n", handle);
*/
    oculus->frameIndex = 0;
//    ovrHmd_ResetFrameTiming(oculus->hmd, oculus->frameIndex);

    oculus->isTracking = TRUE;

    // Tracking is running.
    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1Stop(void)
{
    static char useString[] = "PsychOculusVRCore1('Stop', oculusPtr);";
    static char synopsisString[] =
        "Stop head tracking operation on Oculus device 'oculusPtr'.\n\n";
    static char seeAlsoString[] = "Start";

    int handle;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);

    // Stop device:
    PsychOculusStop(handle);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1GetTrackingState(void)
{
    static char useString[] = "state = PsychOculusVRCore1('GetTrackingState', oculusPtr [, predictionTime=nextFrame]);";
    //                         1                                              2            3
    static char synopsisString[] =
        "Return current state of head position and orientation tracking for Oculus device 'oculusPtr'.\n"
        "Head position and orientation is predicted for target time 'predictionTime' in seconds if provided, "
        "based on the latest measurements from the tracking hardware. If 'predictionTime' is set to zero, "
        "then no prediction is performed and the current state based on latest measurements is returned.\n"
        "If 'predictionTime' is omitted, then the prediction is performed for the mid-point of the next "
        "possible video frame of the HMD, ie. the most likely presentation time for rendered images.\n\n"
        "'state' is a struct with fields reporting the following values:\n"
        "'Time' = Time in seconds of predicted tracking state.\n"
        "'Status' = Tracking status flags. +1 = Head orientation tracked, +2 = Head position tracked, +4 = Camera pose tracked "
        "+32 = Position tracking hardware connected, +128 = HMD display is connected and available.\n"
        "'HeadPose' = Head position [x, y, z] in meters and rotation as quaternion [rx, ry, rz, rw], all as a vector [x,y,z,rx,ry,rz,rw].\n"
        "'HeadLinearSpeed' = Linear velocity [vx,vy,vz] in meters/sec.\n"
        "'HeadAngularSpeed' = Angular velocity [rx,ry,rz] in radians/sec.\n"
        "'HeadLinearAcceleration' = Linear acceleration [ax,ay,az] in meters/sec^2.\n"
        "'HeadAngularAcceleration' = Angular acceleration [rax,ray,raz] in radians/sec^2.\n"
        "'CameraPose' as vector with position and orientation quaternion, like 'HeadPose'.\n"
        "'LeveledCameraPose' Like 'CameraPose' but aligned to the gravity vector of the world.\n"
        "'CameraFrustumHVFov' Horizontal and vertical field of view of the tracking camera in radians.\n"
        "'CameraFrustumNearFarZInMeters' Near and far limit of the camera view frustum in meters.\n"
        "'LastCameraFrameCounter' Last camera framecounter value of tracking camera.\n"
        "'RawSensorAcceleration' = Raw measured accelerometer reading in m/sec^2.\n"
        "'RawSensorGyroRate' = Raw gyrometer reading in rad/s.\n"
        "'RawMagnetometer' = Raw magnetic field in gauss.\n"
        "'SensorTemperature' = Sensor temperature in degrees Celsius.\n"
        "'IMUReadoutTime' = Readout time of the last IMU sample in seconds.\n"
        "\n";

    static char seeAlsoString[] = "Start Stop";

    PsychGenericScriptType *status;
    const char *FieldNames[] = {"Time", "Status", "HeadPose", "HeadLinearSpeed", "HeadAngularSpeed", "HeadLinearAcceleration", "HeadAngularAcceleration",
                                "CameraPose", "LeveledCameraPose", "LastCameraFrameCounter", "RawSensorAcceleration", "RawSensorGyroRate", "RawMagnetometer",
                                "SensorTemperature", "IMUReadoutTime", "CameraFrustumHVFov", "CameraFrustumNearFarZInMeters"};
    const int FieldCount = 17;
    PsychGenericScriptType *outMat;
    double *v;
    int handle;
    double predictionTime;
    PsychOculusDevice *oculus;
    ovrTrackingState state;
    ovrVector3f HmdToEyeOffset[2];

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // Get optional target time for predicted tracking state. Default to the
    // predicted state for the predicted mid-point of the next video frame:
    if (!PsychCopyInDoubleArg(2, kPsychArgOptional, &predictionTime))
        predictionTime = ovr_GetPredictedDisplayTime(oculus->hmd, 0);

    // Get current tracking status info at time predictionTime. Mark this point
    // as time from which motion to photon latency is measured (latencymarker = TRUE):
    state = ovr_GetTrackingState(oculus->hmd, predictionTime, TRUE);
    oculus->sensorSampleTime = ovr_GetTimeInSeconds();

    // Translate to per eye position and orientation:
    HmdToEyeOffset[0] = oculus->eyeRenderDesc[0].HmdToEyeOffset;
    HmdToEyeOffset[1] = oculus->eyeRenderDesc[1].HmdToEyeOffset;
    ovr_CalcEyePoses(state.HeadPose.ThePose, HmdToEyeOffset, oculus->outEyePoses);

    // Print out tracking status:
    if (verbosity >= 4) {
        printf("PsychOculusVRCore1-INFO: Tracking state predicted for device %i at time %f.\n", handle, predictionTime);
        printf("PsychOculusVRCore1-INFO: Time %f : Status %i\n", state.HeadPose.TimeInSeconds, state.StatusFlags);
        printf("PsychOculusVRCore1-INFO: HeadPose: Position    [x,y,z]   = [%f, %f, %f]\n", state.HeadPose.ThePose.Position.x, state.HeadPose.ThePose.Position.y, state.HeadPose.ThePose.Position.z);
        printf("PsychOculusVRCore1-INFO: HeadPose: Orientation [x,y,z,w] = [%f, %f, %f, %f]\n", state.HeadPose.ThePose.Orientation.x, state.HeadPose.ThePose.Orientation.y, state.HeadPose.ThePose.Orientation.z, state.HeadPose.ThePose.Orientation.w);
    }

    PsychAllocOutStructArray(1, kPsychArgOptional, 1, FieldCount, FieldNames, &status);

    PsychSetStructArrayDoubleElement("Time", 0, state.HeadPose.TimeInSeconds, status);
    PsychSetStructArrayDoubleElement("Status", 0, state.StatusFlags, status);

    // Head pose:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);
    v[0] = state.HeadPose.ThePose.Position.x;
    v[1] = state.HeadPose.ThePose.Position.y;
    v[2] = state.HeadPose.ThePose.Position.z;

    v[3] = state.HeadPose.ThePose.Orientation.x;
    v[4] = state.HeadPose.ThePose.Orientation.y;
    v[5] = state.HeadPose.ThePose.Orientation.z;
    v[6] = state.HeadPose.ThePose.Orientation.w;
    PsychSetStructArrayNativeElement("HeadPose", 0, outMat, status);

    // Linear velocity:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
    v[0] = state.HeadPose.LinearVelocity.x;
    v[1] = state.HeadPose.LinearVelocity.y;
    v[2] = state.HeadPose.LinearVelocity.z;
    PsychSetStructArrayNativeElement("HeadLinearSpeed", 0, outMat, status);

    // Angular velocity:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
    v[0] = state.HeadPose.AngularVelocity.x;
    v[1] = state.HeadPose.AngularVelocity.y;
    v[2] = state.HeadPose.AngularVelocity.z;
    PsychSetStructArrayNativeElement("HeadAngularSpeed", 0, outMat, status);

    // Linear acceleration:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
    v[0] = state.HeadPose.LinearAcceleration.x;
    v[1] = state.HeadPose.LinearAcceleration.y;
    v[2] = state.HeadPose.LinearAcceleration.z;
    PsychSetStructArrayNativeElement("HeadLinearAcceleration", 0, outMat, status);

    // Angular acceleration:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
    v[0] = state.HeadPose.AngularAcceleration.x;
    v[1] = state.HeadPose.AngularAcceleration.y;
    v[2] = state.HeadPose.AngularAcceleration.z;
    PsychSetStructArrayNativeElement("HeadAngularAcceleration", 0, outMat, status);

/* TODOREMOVE
    // Camera pose:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);
    v[0] = state.CameraPose.Position.x;
    v[1] = state.CameraPose.Position.y;
    v[2] = state.CameraPose.Position.z;

    v[3] = state.CameraPose.Orientation.x;
    v[4] = state.CameraPose.Orientation.y;
    v[5] = state.CameraPose.Orientation.z;
    v[6] = state.CameraPose.Orientation.w;
    PsychSetStructArrayNativeElement("CameraPose", 0, outMat, status);

    // Camera leveled pose:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);
    v[0] = state.LeveledCameraPose.Position.x;
    v[1] = state.LeveledCameraPose.Position.y;
    v[2] = state.LeveledCameraPose.Position.z;

    v[3] = state.LeveledCameraPose.Orientation.x;
    v[4] = state.LeveledCameraPose.Orientation.y;
    v[5] = state.LeveledCameraPose.Orientation.z;
    v[6] = state.LeveledCameraPose.Orientation.w;
    PsychSetStructArrayNativeElement("LeveledCameraPose", 0, outMat, status);

    // LastCameraFrameCounter:
    PsychSetStructArrayDoubleElement("LastCameraFrameCounter", 0, (double) state.LastCameraFrameCounter, status);

    // RawSensorAcceleration:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
    v[0] = state.RawSensorData.Accelerometer.x;
    v[1] = state.RawSensorData.Accelerometer.y;
    v[2] = state.RawSensorData.Accelerometer.z;
    PsychSetStructArrayNativeElement("RawSensorAcceleration", 0, outMat, status);

    // RawSensorGyroRate:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
    v[0] = state.RawSensorData.Gyro.x;
    v[1] = state.RawSensorData.Gyro.y;
    v[2] = state.RawSensorData.Gyro.z;
    PsychSetStructArrayNativeElement("RawSensorGyroRate", 0, outMat, status);

    // RawMagnetometer:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
    v[0] = state.RawSensorData.Magnetometer.x;
    v[1] = state.RawSensorData.Magnetometer.y;
    v[2] = state.RawSensorData.Magnetometer.z;
    PsychSetStructArrayNativeElement("RawMagnetometer", 0, outMat, status);

    // SensorTemperature:
    PsychSetStructArrayDoubleElement("SensorTemperature", 0, (double) state.RawSensorData.Temperature, status);

    // IMU readout time:
    PsychSetStructArrayDoubleElement("IMUReadoutTime", 0, (double) state.RawSensorData.TimeInSeconds, status);
*/

/* TODO
    // Camera frustum HFov and VFov in radians:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = oculus->hmd->CameraFrustumHFovInRadians;
    v[1] = oculus->hmd->CameraFrustumVFovInRadians;
    PsychSetStructArrayNativeElement("CameraFrustumHVFov", 0, outMat, status);

    // Camera frustum near and far clip plane in meters:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = oculus->hmd->CameraFrustumNearZInMeters;
    v[1] = oculus->hmd->CameraFrustumFarZInMeters;
    PsychSetStructArrayNativeElement("CameraFrustumNearFarZInMeters", 0, outMat, status);
*/
    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1GetFovTextureSize(void)
{
    static char useString[] = "[width, height, fovPort] = PsychOculusVRCore1('GetFovTextureSize', oculusPtr, eye [, fov=[HMDRecommended]][, pixelsPerDisplay=1]);";
    //                          1      2       3                                                  1          2      3                       4
    static char synopsisString[] =
    "Return recommended size of client renderbuffers for Oculus device 'oculusPtr'.\n"
    "'eye' which eye to provide the size for: 0 = Left, 1 = Right.\n"
    "'fov' Optional field of view in degrees, from line of sight: [leftdeg, rightdeg, updeg, downdeg]. "
    "If 'fov' is omitted, the Oculus runtime will be asked for a good default field of view and that "
    "will be used. The field of view may be dependent on the settings in the Oculus user profile of the "
    "currently selected user.\n"
    "'pixelsPerDisplay' Ratio of the number of render target pixels to display pixels at the center "
    "of distortion. Defaults to 1.0 if omitted. Lower values can improve performance, at lower quality.\n"
    "\n"
    "Return values are 'width' for minimum recommended width of framebuffer in pixels and "
    "'height' for minimum recommended height of framebuffer in pixels. 'fovPort' is the field of view "
    "in degrees finally used for calculation of 'width' x 'height'.\n";
    static char seeAlsoString[] = "GetUndistortionParameters";

    int handle, eyeIndex;
    int n, m, p;
    PsychOculusDevice *oculus;
    double *fov;
    double pixelsPerDisplay;
    double *outFov;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(4));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // Get eye index - left = 0, right = 1:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    // Get optional field of view in degrees in left,right,up,down direction from line of sight:
    if (PsychAllocInDoubleMatArg(3, kPsychArgOptional, &n, &m, &p, &fov)) {
        // Validate and assign:
        if (n * m * p != 4) PsychErrorExitMsg(PsychError_user, "Invalid 'fov' specified. Must be a 4-component vector of form [leftdeg, rightdeg, updeg, downdeg].");
        oculus->ofov[eyeIndex].LeftTan  = (float) tan(deg2rad(fov[0]));
        oculus->ofov[eyeIndex].RightTan = (float) tan(deg2rad(fov[1]));
        oculus->ofov[eyeIndex].UpTan    = (float) tan(deg2rad(fov[2]));
        oculus->ofov[eyeIndex].DownTan  = (float) tan(deg2rad(fov[3]));
    }
    else {
        // None specified: Ask the runtime for good defaults.
        oculus->ofov[eyeIndex] = oculus->hmdDesc.DefaultEyeFov[eyeIndex];
    }

    // Get optional pixelsPerDisplay parameter:
    pixelsPerDisplay = 1.0;
    PsychCopyInDoubleArg(4, kPsychArgOptional, &pixelsPerDisplay);
    if (pixelsPerDisplay <= 0.0) PsychErrorExitMsg(PsychError_user, "Invalid 'pixelsPerDisplay' specified. Must be greater than zero.");

    // Ask the api for optimal texture size, aka the size of the client draw buffer:
    oculus->texSize[eyeIndex] = ovr_GetFovTextureSize(oculus->hmd, (ovrEyeType) eyeIndex, oculus->ofov[eyeIndex], (float) pixelsPerDisplay);

    // Return recommended width and height of drawBuffer:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, oculus->texSize[eyeIndex].w);
    PsychCopyOutDoubleArg(2, kPsychArgOptional, oculus->texSize[eyeIndex].h);

    PsychAllocOutDoubleMatArg(3, kPsychArgOptional, 4, 1, 1, &outFov);
    outFov[0] = rad2deg(atan(oculus->ofov[eyeIndex].LeftTan));
    outFov[1] = rad2deg(atan(oculus->ofov[eyeIndex].RightTan));
    outFov[2] = rad2deg(atan(oculus->ofov[eyeIndex].UpTan));
    outFov[3] = rad2deg(atan(oculus->ofov[eyeIndex].DownTan));

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1CreateRenderTextureChain(void)
{
    static char useString[] = "[width, height, numTextures] = PsychOculusVRCore1('CreateRenderTextureChain', oculusPtr, eye, width, height);";
    //                          1      2       3                                                             1          2    3      4
    static char synopsisString[] =
    "Create texture present chains for Oculus device 'oculusPtr'.\n"
    "'eye' Eye for which chain should get created: 0 = Left/Mono, 1 = Right.\n"
    "If only a chain for eye = 0 is created then the driver operates in monoscopic "
    "presentation mode for use with Screen() stereomode 0, showing the same mono image to both "
    "eyes. If a 2nd chain for eye = 1 is created then the driver switches to stereoscopic "
    "presentation mode for use with Screen() stereomode 12, presenting separate images to the left "
    "and right eye.\n"
    "'width' and 'height' are the width x height of the texture into which Psychtoolbox "
    "Screen() image processing pipeline will render the output image of an eye for submission "
    "to the VR compositor. Left and right eye must use identical 'width' and 'height'.\n"
    "'numTextures' returns the total number of compositor textures in the swap chain.\n"
    "\n"
    "Return values are 'width' for selected width of output texture in pixels and "
    "'height' for height of output texture in pixels.\n";
    static char seeAlsoString[] = "GetNextTextureHandle";

    int handle, eyeIndex;
    int width, height, out_Length;
    PsychOculusDevice *oculus;
    ovrTextureSwapChainDesc chainDesc;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(4));
    PsychErrorExit(PsychRequireNumInputArgs(4));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // Get eye:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1)
        PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    // Get texture dimensions:
    PsychCopyInIntegerArg(3, kPsychArgRequired, &width);
    if (width < 1)
        PsychErrorExitMsg(PsychError_user, "Invalid width, smaller than 1 texel!");

    PsychCopyInIntegerArg(4, kPsychArgRequired, &height);
    if (height < 1)
        PsychErrorExitMsg(PsychError_user, "Invalid height, smaller than 1 texel!");

    if (oculus->textureSwapChain[eyeIndex])
        PsychErrorExitMsg(PsychError_user, "Tried to create already created texture swap chain for given eye.");

    if (eyeIndex > 0 && (width != oculus->textureWidth || height != oculus->textureHeight))
        PsychErrorExitMsg(PsychError_user, "Given width x height for 2nd eye does not match width x height of 1st eye, as required.");

    // Build OpenGL texture chain descriptor:
    memset(&chainDesc, 0, sizeof(chainDesc));
    chainDesc.Type = ovrTexture_2D;
    chainDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM; // TODO OVR_FORMAT_R8G8B8A8_UNORM_SRGB
    chainDesc.ArraySize = 1;
    chainDesc.MipLevels = 1;
    chainDesc.SampleCount = 1;
    chainDesc.StaticImage = ovrFalse;
    chainDesc.MiscFlags = ovrTextureMisc_None;
    chainDesc.BindFlags = ovrTextureBind_None;
    chainDesc.Width = width;
    chainDesc.Height = height;

    // Create texture swap chain:
    if (OVR_FAILURE(ovr_CreateTextureSwapChainGL(oculus->hmd, &chainDesc, &oculus->textureSwapChain[eyeIndex]))) {
        ovr_GetLastErrorInfo(&errorInfo);
        if (verbosity > 0) printf("PsychOculusVRCore1-ERROR: ovr_CreateTextureSwapChainGL failed: %s\n", errorInfo.ErrorString);
        PsychErrorExitMsg(PsychError_system, "Failed to create texture swap chain for VR compositor.");
    }

    // Mark driver as in stereo mode if a swap chain for the right eye was created:
    if (eyeIndex > 0) {
        if (verbosity > 2) printf("PsychOculusVRCore1-INFO: Right eye swap chain created. Switching to stereo mode.\n");
        oculus->isStereo = TRUE;
    }

    ovr_GetTextureSwapChainLength(oculus->hmd, oculus->textureSwapChain[eyeIndex], &out_Length);

    if (verbosity > 3)
        printf("PsychOculusVRCore1-INFO: Allocated texture swap chain has %i buffers.\n", out_Length);

    // Assign total texture buffer width/height for the frame submission later on:
    oculus->textureWidth = chainDesc.Width;
    oculus->textureHeight = chainDesc.Height;

    // Return recommended width and height of drawBuffer:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, chainDesc.Width);
    PsychCopyOutDoubleArg(2, kPsychArgOptional, chainDesc.Height);

    // Return number of textures in swap chain:
    PsychCopyOutDoubleArg(3, kPsychArgOptional, out_Length);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1GetNextTextureHandle(void)
{
    static char useString[] = "texObjectHandle = PsychOculusVRCore1('GetNextTextureHandle', oculusPtr, eye);";
    //                         1                                                            1          2
    static char synopsisString[] =
    "Retrieve OpenGL texture object handle for next target texture for Oculus device 'oculusPtr'.\n"
    "'eye' Eye for which handle of next texture should be returned: 0 = Left/Mono, 1 = Right.\n"
    "Returns a GL_TEXTURE2D texture object name/handle in 'texObjectHandle' for the texture "
    "to which the next VR frame should be rendered. Returns -1 if busy.\n";
    static char seeAlsoString[] = "CreateRenderTextureChain";

    int handle, eyeIndex;
    unsigned int texObjectHandle;
    PsychOculusDevice *oculus;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // Get eye:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1)
        PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    if (eyeIndex > 0 && !(oculus->isStereo))
        PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0, as mono display mode is selected.");

    // Get next free/non-busy buffer for this eyes texture swap chain:
    if (OVR_FAILURE(ovr_GetTextureSwapChainBufferGL(oculus->hmd, oculus->textureSwapChain[eyeIndex], -1, &texObjectHandle))) {
        ovr_GetLastErrorInfo(&errorInfo);
        if (verbosity > 0) printf("PsychOculusVRCore1-ERROR: eye %i ovr_GetTextureSwapChainBufferGL failed: %s\n", eyeIndex, errorInfo.ErrorString);
        PsychErrorExitMsg(PsychError_system, "Failed to retrieve next OpenGL texture from swap chain.");
    }

    // Return texture object handle:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) texObjectHandle);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1CreateMirrorTexture(void)
{
    static char useString[] = "texObjectHandle = PsychOculusVRCore1('CreateMirrorTexture', oculusPtr, width, height);";
    //                         1                                                           1          2      3
    static char synopsisString[] =
    "Create mirror texture for Oculus device 'oculusPtr'.\n"
    "A mirror texture is a auto-updating texture which receives the same image content "
    "which is sent to the HMD for display. It can be used for displaying a debug image "
    "on the regular windowing system, ie., inside a regular Psychtoolbox onscreen window.\n"
    "'width' and 'height' are the width x height of the texture into which "
    "the VR compositor will render the output image which is also sent to the HMD.\n"
    "'texObjectHandle' returns the OpenGL texture handle for accessing the mirror texture.\n";
    static char seeAlsoString[] = "";

    int handle;
    int width, height;
    unsigned int texObjectHandle;
    PsychOculusDevice *oculus;
    ovrMirrorTextureDesc mirrorDesc;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(3));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // Get texture dimensions:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &width);
    if (width < 1)
        PsychErrorExitMsg(PsychError_user, "Invalid width, smaller than 1 texel!");

    PsychCopyInIntegerArg(3, kPsychArgRequired, &height);
    if (height < 1)
        PsychErrorExitMsg(PsychError_user, "Invalid height, smaller than 1 texel!");

    if (oculus->mirrorTexture)
        PsychErrorExitMsg(PsychError_user, "Tried to create already created mirror texture.");

    // Build OpenGL texture chain descriptor:
    memset(&mirrorDesc, 0, sizeof(mirrorDesc));
    mirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
    mirrorDesc.MiscFlags = ovrTextureMisc_None;
    mirrorDesc.Width = width;
    mirrorDesc.Height = height;

    // Create texture swap chain:
    if (OVR_FAILURE(ovr_CreateMirrorTextureGL(oculus->hmd, &mirrorDesc, &oculus->mirrorTexture))) {
        ovr_GetLastErrorInfo(&errorInfo);
        if (verbosity > 0) printf("PsychOculusVRCore1-ERROR: ovr_CreateMirrorTextureGL failed: %s\n", errorInfo.ErrorString);
        PsychErrorExitMsg(PsychError_system, "Failed to create mirror texture for VR compositor.");
    }

    if (OVR_FAILURE(ovr_GetMirrorTextureBufferGL(oculus->hmd, oculus->mirrorTexture, &texObjectHandle))) {
        ovr_GetLastErrorInfo(&errorInfo);
        if (verbosity > 0) printf("PsychOculusVRCore1-ERROR: ovr_GetMirrorTextureBufferGL failed: %s\n", errorInfo.ErrorString);
        PsychErrorExitMsg(PsychError_system, "Failed to get OpenGL texture handle of mirror texture for VR compositor.");
    }

    // Return texture object handle:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) texObjectHandle);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1GetUndistortionParameters(void)
{
    static char useString[] = "[width, height, viewPx, viewPy, viewPw, viewPh, pptax, pptay, hmdShiftx, hmdShifty, hmdShiftz] = PsychOculusVRCore1('GetUndistortionParameters', oculusPtr, eye [, inputWidth][, inputHeight][, fov]);";
    //                          1      2       3       4       5       6       7      8      9          10         11                                                           1          2      3             4              5
    static char synopsisString[] =
    "Return parameters needed for rendering for output on Oculus device 'oculusPtr'.\n"
    "'eye' which eye to provide the data: 0 = Left, 1 = Right.\n"
    "'inputWidth' = Width of the rendered input image buffer in pixels.\n"
    "'inputHeight' = Height of the rendered input image buffer in pixels.\n"
    "'fov' Optional field of view in degrees, from line of sight: [leftdeg, rightdeg, updeg, downdeg]. "
    "You can pass in the 'fovPort' value returned from PsychOculusVR1('GetFovTextureSize'); "
    "Defaults to whatever has been set for the given eye in the last call to PsychOculusVR1('GetFovTextureSize').\n"
    "\n"
    "Return values:\n"
    "[width, height] = Width and height of client renderbuffers in pixels. Same as the provided 'inputWidth' and 'inputHeight'.\n"
    "[viewPx, viewPy, viewPw, viewPh] Render viewport [x,y,w,h] start (x,y) position, width and height. Mostly useless atm., as "
    "viewPx and viewPy are always zero, viewPw and viewPh are identical to width and height in the current driver design.\n"
    "[pptax pptay] = Pixels per tangens angle at display center in x direction and y direction.\n"
    "[hmdShiftx, hmdShifty, hmdShiftz] = HmdToEyeOffset 3D translation vector. Defines the location of the optical center of the eye "
    "relative to the origin of the local head reference frame, ie. the tracked head position.\n";
    static char seeAlsoString[] = "GetFovTextureSize";

    int handle, eyeIndex;
    PsychOculusDevice *oculus;
    int m, n, p;
    double *fov;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(11));
    PsychErrorExit(PsychCapNumInputArgs(5));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // Get eye index - left = 0, right = 1:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    // Get input texture width:
    if (PsychCopyInIntegerArg(3, kPsychArgOptional, &(oculus->texSize[eyeIndex].w))) {
        if (oculus->texSize[eyeIndex].w < 1) PsychErrorExitMsg(PsychError_user, "Invalid 'inputWidth' specified. Must be greater than zero.");
    }

    // Get input texture height:
    if (PsychCopyInIntegerArg(4, kPsychArgOptional, &(oculus->texSize[eyeIndex].h))) {
        if (oculus->texSize[eyeIndex].h < 1) PsychErrorExitMsg(PsychError_user, "Invalid 'inputHeight' specified. Must be greater than zero.");
    }

    // Get optional field of view in degrees in left,right,up,down direction from line of sight:
    if (PsychAllocInDoubleMatArg(5, kPsychArgOptional, &n, &m, &p, &fov)) {
        // Validate and assign:
        if (n * m * p != 4) PsychErrorExitMsg(PsychError_user, "Invalid 'fov' specified. Must be a 4-component vector of form [leftdeg, rightdeg, updeg, downdeg].");
        oculus->ofov[eyeIndex].LeftTan  = (float) tan(deg2rad(fov[0]));
        oculus->ofov[eyeIndex].RightTan = (float) tan(deg2rad(fov[1]));
        oculus->ofov[eyeIndex].UpTan    = (float) tan(deg2rad(fov[2]));
        oculus->ofov[eyeIndex].DownTan  = (float) tan(deg2rad(fov[3]));
    }

    // Return width and height of input texture - Just mirror out what we got:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, oculus->texSize[eyeIndex].w);
    PsychCopyOutDoubleArg(2, kPsychArgOptional, oculus->texSize[eyeIndex].h);

    // Get eye render description for this eye:
    oculus->eyeRenderDesc[eyeIndex] = ovr_GetRenderDesc(oculus->hmd, (ovrEyeType) eyeIndex, oculus->ofov[eyeIndex]);

    // Override viewport size with the provided input texture size, as ovr_GetRenderDesc() makes the
    // assumption that we render to a half-width viewport of the input renderbuffer, when we actually
    // use 2 separate input renderbuffers of appropriate size:
    oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Pos.x = 0;
    oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Pos.y = 0;
    oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Size.w = oculus->texSize[eyeIndex].w;
    oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Size.h = oculus->texSize[eyeIndex].h;

    if (verbosity > 3) {
        printf("PsychOculusVRCore1-INFO: For HMD %i, eye %i - RenderDescription:\n", handle, eyeIndex);
        printf("PsychOculusVRCore1-INFO: FoV: %f %f %f %f - %f %f %f %f\n", oculus->ofov[eyeIndex].LeftTan, oculus->ofov[eyeIndex].RightTan, oculus->ofov[eyeIndex].UpTan, oculus->ofov[eyeIndex].DownTan, oculus->eyeRenderDesc[eyeIndex].Fov.LeftTan, oculus->eyeRenderDesc[eyeIndex].Fov.RightTan, oculus->eyeRenderDesc[eyeIndex].Fov.UpTan, oculus->eyeRenderDesc[eyeIndex].Fov.DownTan);
        printf("PsychOculusVRCore1-INFO: DistortedViewport: [x,y,w,h] = [%i, %i, %i, %i]\n", oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Pos.x, oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Pos.y, oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Size.w, oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Size.h);
        printf("PsychOculusVRCore1-INFO: PixelsPerTanAngleAtCenter: %f x %f\n", oculus->eyeRenderDesc[eyeIndex].PixelsPerTanAngleAtCenter.x, oculus->eyeRenderDesc[eyeIndex].PixelsPerTanAngleAtCenter.y);
        printf("PsychOculusVRCore1-INFO: HmdToEyeViewOffset: [x,y,z] = [%f, %f, %f]\n", oculus->eyeRenderDesc[eyeIndex].HmdToEyeOffset.x, oculus->eyeRenderDesc[eyeIndex].HmdToEyeOffset.y, oculus->eyeRenderDesc[eyeIndex].HmdToEyeOffset.z);
    }

    // DistortedViewport [x,y,w,h]:
    PsychCopyOutDoubleArg(3, kPsychArgOptional, oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Pos.x);
    PsychCopyOutDoubleArg(4, kPsychArgOptional, oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Pos.y);
    PsychCopyOutDoubleArg(5, kPsychArgOptional, oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Size.w);
    PsychCopyOutDoubleArg(6, kPsychArgOptional, oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Size.h);

    // PixelsPerTanAngleAtCenter:
    PsychCopyOutDoubleArg(7, kPsychArgOptional, oculus->eyeRenderDesc[eyeIndex].PixelsPerTanAngleAtCenter.x);
    PsychCopyOutDoubleArg(8, kPsychArgOptional, oculus->eyeRenderDesc[eyeIndex].PixelsPerTanAngleAtCenter.y);

    // HmdToEyeViewOffset: [x,y,z]:
    PsychCopyOutDoubleArg(9,  kPsychArgOptional, oculus->eyeRenderDesc[eyeIndex].HmdToEyeOffset.x);
    PsychCopyOutDoubleArg(10, kPsychArgOptional, oculus->eyeRenderDesc[eyeIndex].HmdToEyeOffset.y);
    PsychCopyOutDoubleArg(11, kPsychArgOptional, oculus->eyeRenderDesc[eyeIndex].HmdToEyeOffset.z);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1GetEyeTimewarpMatrices(void)
{
    static char useString[] = "[eyeRotStartMatrix, eyeRotEndMatrix] = PsychOculusVRCore1('GetEyeTimewarpMatrices', oculusPtr, eye [, waitForTimewarpPoint=0]);";
    //                          1                  2                                                              1          2      3
    static char synopsisString[] =
    "Return eye warp rotation matrices for timewarped undistortion for Oculus device 'oculusPtr'.\n"
    "'eye' which eye to provide the data: 0 = Left, 1 = Right.\n"
    "'waitForTimewarpPoint' If set to 1, stall execution of calling thread until next time warp point is reached. Defaults to zero.\n"
    "Return values are 4x4 'eyeRotStartMatrix' and 'eyeRotEndMatrix' for given eye.\n";
    static char seeAlsoString[] = "";
//TODO REMOVE
    int handle, eyeIndex, waitForTimewarpPoint;
    PsychOculusDevice *oculus;
    ovrEyeType eye;
    double tNow;
    int i, j;
    double *startMatrix, *endMatrix;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // Get eye index - left = 0, right = 1:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    // Get waitForTimewarpPoint:
    waitForTimewarpPoint = 0;
    PsychCopyInIntegerArg(3, kPsychArgOptional, &waitForTimewarpPoint);
    if (waitForTimewarpPoint < 0 || waitForTimewarpPoint > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'waitForTimewarpPoint' specified. Must be 0 or 1.");

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1GetStaticRenderParameters(void)
{
    static char useString[] = "[projL, projR] = PsychOculusVRCore1('GetStaticRenderParameters', oculusPtr [, clipNear=0.01][, clipFar=10000.0]);";
    //                          1      2                                                       1            2                3
    static char synopsisString[] =
    "Retrieve static rendering parameters for Oculus device 'oculusPtr' at current settings.\n"
    "'clipNear' Optional near clipping plane for OpenGL. Defaults to 0.01.\n"
    "'clipFar' Optional far clipping plane for OpenGL. Defaults to 10000.0.\n"
    "\nReturn arguments:\n\n"
    "'projL' is the 4x4 OpenGL projection matrix for the left eye rendering.\n"
    "'projR' is the 4x4 OpenGL projection matrix for the right eye rendering.\n"
    "Please note that projL and projR are usually identical for typical rendering scenarios.\n";
    static char seeAlsoString[] = "";

    int handle;
    PsychOculusDevice *oculus;
    ovrMatrix4f M;
    int i, j;
    double clip_near, clip_far;
    double *outM;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // Get optional near clipping plane:
    clip_near = 0.01;
    PsychCopyInDoubleArg(2, kPsychArgOptional, &clip_near);

    // Get optional far clipping plane:
    clip_far = 10000.0;
    PsychCopyInDoubleArg(3, kPsychArgOptional, &clip_far);

    // Return left projection matrix as return argument 1:
    M = ovrMatrix4f_Projection(oculus->eyeRenderDesc[0].Fov, (float) clip_near, (float) clip_far, ovrProjection_ClipRangeOpenGL);
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 4, 4, 1, &outM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(outM++) = (double) M.M[j][i];

    // Return right projection matrix as return argument 2:
    M = ovrMatrix4f_Projection(oculus->eyeRenderDesc[1].Fov, (float) clip_near, (float) clip_far, ovrProjection_ClipRangeOpenGL);
    PsychAllocOutDoubleMatArg(2, kPsychArgOptional, 4, 4, 1, &outM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(outM++) = (double) M.M[j][i];

    return(PsychError_none);
}


PsychError PSYCHOCULUSVR1StartRender(void)
{
    static char useString[] = "[eyePoseL, eyePoseR, tracked, frameTiming] = PsychOculusVRCore1('StartRender', oculusPtr);";
    //                          1         2         3        4                                               1
    static char synopsisString[] =
    "Mark start of a new 3D head tracked render cycle for Oculus device 'oculusPtr'.\n"
    "Return values are the vectors which define the two eye cameras positions and orientations "
    "for the left eye and right eye 'eyePoseL' and 'eyePoseR'. The vectors are of form "
    "[tx, ty, tz, rx, ry, rz, rw] - A 3 component 3D position followed by a 4 component rotation "
    "quaternion.\n"
    "'tracked' Tracking status flags: 0 = Head not tracked at the moment. 1 = Head orientation tracked. "
    "2 = Head position tracked (DK2 and later). 3 = Head position and orientation tracked (DK2 and later). "
    "4 = Camera pose tracked, 7 = 1+2+4 = Camera pose and Head position and orientation tracked.\n"
    "\n"
    "'frameTiming' Vector with predicted timing information for this frame with following elements:\n"
    "[1] = DeltaSeconds since last frame.\n"
    "[2] = ThisFrameSeconds start of scanout of this frame.\n"
    "[3] = TimewarpPointSeconds\n"
    "[4] = NextFrameSeconds\n"
    "[5] = ScanoutMidpointSeconds\n"
    "[6] = EyeScanoutSeconds[0]\n"
    "[7] = EyeScanoutSeconds[1]\n"
    "\n";
    static char seeAlsoString[] = "GetEyePose EndFrameRender";

    int handle;
    PsychOculusDevice *oculus;
    ovrTrackingState os;
    ovrVector3f hmdToEyeViewOffset[2];
    double *outM;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(6));
    PsychErrorExit(PsychCapNumInputArgs(1));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);
/*
    // Mark beginning of frame rendering. This takes timstamps and stuff:
    oculus->frameTiming = ovrHmd_BeginFrameTiming(oculus->hmd, oculus->frameIndex);

    // Get current eye poses for both eyes:
    hmdToEyeViewOffset[0] = oculus->eyeRenderDesc[0].HmdToEyeViewOffset;
    hmdToEyeViewOffset[1] = oculus->eyeRenderDesc[1].HmdToEyeViewOffset;
    ovrHmd_GetEyePoses(oculus->hmd, oculus->frameIndex, hmdToEyeViewOffset,
                       oculus->outEyePoses, &os);

    // Left eye pose as raw data:
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, 7, 1, &outM);

    // Position (x,y,z):
    outM[0] = oculus->outEyePoses[0].Position.x;
    outM[1] = oculus->outEyePoses[0].Position.y;
    outM[2] = oculus->outEyePoses[0].Position.z;

    // Orientation as a quaternion (x,y,z,w):
    outM[3] = oculus->outEyePoses[0].Orientation.x;
    outM[4] = oculus->outEyePoses[0].Orientation.y;
    outM[5] = oculus->outEyePoses[0].Orientation.z;
    outM[6] = oculus->outEyePoses[0].Orientation.w;

    // Right eye pose as raw data:
    PsychAllocOutDoubleMatArg(2, kPsychArgOptional, 1, 7, 1, &outM);

    // Position (x,y,z):
    outM[0] = oculus->outEyePoses[1].Position.x;
    outM[1] = oculus->outEyePoses[1].Position.y;
    outM[2] = oculus->outEyePoses[1].Position.z;

    // Orientation as a quaternion (x,y,z,w):
    outM[3] = oculus->outEyePoses[1].Orientation.x;
    outM[4] = oculus->outEyePoses[1].Orientation.y;
    outM[5] = oculus->outEyePoses[1].Orientation.z;
    outM[6] = oculus->outEyePoses[1].Orientation.w;

    // Report tracking state of head orientation, head position and camera pose:
    PsychCopyOutDoubleArg(3, kPsychArgOptional, (double)
                          (os.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked | ovrStatus_CameraPoseTracked)));

    // Report frame timing info:
    PsychAllocOutDoubleMatArg(4, kPsychArgOptional, 1, 7, 1, &outM);
    outM[0] = oculus->frameTiming.DeltaSeconds;
    outM[1] = oculus->frameTiming.ThisFrameSeconds;
    outM[2] = oculus->frameTiming.TimewarpPointSeconds;
    outM[3] = oculus->frameTiming.NextFrameSeconds;
    outM[4] = oculus->frameTiming.ScanoutMidpointSeconds;
    outM[5] = oculus->frameTiming.EyeScanoutSeconds[0];
    outM[6] = oculus->frameTiming.EyeScanoutSeconds[1];
*/
    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1EndFrameRender(void)
{
    static char useString[] = "PsychOculusVRCore1('EndFrameRender', oculusPtr);";
    //                                                              1
    static char synopsisString[] =
    "Mark end of a render cycle for Oculus HMD device 'oculusPtr'.\n\n"
    "This will commit the current set of 2D textures with new rendered content "
    "to the texture swapchains, for consumption by the VR runtime/compositor. "
    "The swapchains will advance, providing new unused image textures as new "
    "render targets for the next rendering cycle.\n\n"
    "You usually won't call this function yourself, but Screen('Flip') will "
    "call it automatically for you at the appropriate moment.\n";
    static char seeAlsoString[] = "StartRender PresentFrame";

    int handle, eyeIndex;
    PsychOculusDevice *oculus;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // Commit current target textures to chain:
    for (eyeIndex = 0; eyeIndex < ((oculus->isStereo) ? 2 : 1); eyeIndex++) {
        if (OVR_FAILURE(ovr_CommitTextureSwapChain(oculus->hmd, oculus->textureSwapChain[eyeIndex]))) {
            ovr_GetLastErrorInfo(&errorInfo);
            if (verbosity > 0)
                printf("PsychOculusVRCore1-ERROR: eye %i ovr_CommitTextureSwapChain() failed: %s\n", eyeIndex, errorInfo.ErrorString);
            PsychErrorExitMsg(PsychError_system, "Failed to commit finished OpenGL texture to swap chain.");
        }
    }

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1PresentFrame(void)
{
    static char useString[] = "frameTiming = PsychOculusVRCore1('PresentFrame', oculusPtr);";
    //                         1                                                1
    static char synopsisString[] =
    "Present last rendered frame to Oculus HMD device 'oculusPtr'.\n\n"
    "Returns a 'frameTiming' with information about the presentation "
    "timing for this presentation cycle.\n\n"
    "You usually won't call this function yourself, but Screen('Flip') "
    "will call it automatically for you at the appropriate moment.\n\n"
    "'frameTiming' is an array of structs, where each struct contains presentation "
    "timing and performance info for one presented frame, with the most recently "
    "presented frame in frameTiming(1), older ones at higher indices if the script "
    "falls behind the compositor in Screen('Flip')ping frames in time for the HMD.\n"
    "The following fields are currently supported in each struct:\n\n"
    "HmdVsyncIndex: Vsync counter of the HMD, increments at each refresh.\n"
    "AppFrameIndex: Running count of 'Flip'ped frames. Increments at each Screen('Flip').\n"
    "HMDTime: Time in the HMDs time base in seconds.\n"
    "GetSecsTime: Time in Psychtoolbox standard GetSecs timebase\n"
    "AppMotionToPhotonLatency: Latency between frame submission (or last head tracker query) "
    "and presentation of the associated frame.\n"
    "RecentSensorSampleTime: Baseline for AppMotionToPhotonLatency, if app can keep up with compositor.\n"
    "StimulusOnsetTime: Estimated visual stimulus onset time for the frame - the mid-point of scanout!\n"
    "VBlankTime: Screen('Flip') vblTime equivalent.\n"
    "AppQueueAheadTime: How much ahead the application has to queue a frame for presentation. In some sense "
    "a measure of compositor lag. 0 msecs = No lag, presenting at next VBLANK boundary. 1 refresh duration = "
    "1 frame lag, etc.\n"
    "\n\n";
    static char seeAlsoString[] = "EndFrameRender";

    PsychGenericScriptType *frameT;
    const char *FieldNames[] = {"HmdVsyncIndex", "AppFrameIndex", "HMDTime", "GetSecsTime", "AppMotionToPhotonLatency",
                                "RecentSensorSampleTime", "StimulusOnsetTime", "VBlankTime", "AppQueueAheadTime" };
    const int FieldCount = 9;

    int handle, i;
    double tNow, tHMD, tStimOnset, tVBL, tDeadline;
    PsychOculusDevice *oculus;
    ovrLayerEyeFov layer0;
    const ovrLayerHeader *layers[1] = { &layer0.Header };
    ovrPerfStats perfStats;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // If HMD tracking is disabled then set sensorSampleTime to "now" - reasonable:
    if (!oculus->isTracking)
        oculus->sensorSampleTime = ovr_GetTimeInSeconds();

    // Setup layer headers:
    layer0.Header.Type = ovrLayerType_EyeFov;
    layer0.Header.Flags = ovrLayerFlag_HighQuality | ovrLayerFlag_TextureOriginAtBottomLeft;

    // Use head locked layer if head tracking is disabled, so our frames stay in a fixed
    // position wrt. the HMD - the HMD simply acts as a strapped on mono- or stereo monitor:
    layer0.Header.Flags |= ((!oculus->isTracking) ? ovrLayerFlag_HeadLocked : 0);

    layer0.ColorTexture[0] = oculus->textureSwapChain[0];
    layer0.ColorTexture[1] = (oculus->isStereo) ? oculus->textureSwapChain[1] : NULL;
    layer0.Viewport[0].Pos.x = 0;
    layer0.Viewport[0].Pos.y = 0;
    layer0.Viewport[0].Size.w = oculus->textureWidth;
    layer0.Viewport[0].Size.h = oculus->textureHeight;
    layer0.Viewport[1].Pos.x = 0;
    layer0.Viewport[1].Pos.y = 0;
    layer0.Viewport[1].Size.w = oculus->textureWidth;
    layer0.Viewport[1].Size.h = oculus->textureHeight;
    layer0.Fov[0] = oculus->ofov[0];
    layer0.Fov[1] = oculus->ofov[1];
    layer0.RenderPose[0] = oculus->outEyePoses[0];
    layer0.RenderPose[1] = oculus->outEyePoses[1];
    layer0.SensorSampleTime = oculus->sensorSampleTime;

    // Submit frame to compositor for display at earliest possible time:
    if (OVR_FAILURE(ovr_SubmitFrame(oculus->hmd, 0, NULL, layers, 1))) {
        ovr_GetLastErrorInfo(&errorInfo);
        if (verbosity > 0)
            printf("PsychOculusVRCore1-ERROR: ovr_SubmitFrame() failed: %s\n", errorInfo.ErrorString);
        PsychErrorExitMsg(PsychError_system, "Failed to submit finished frame to VR compositor.");
    }

    // Retrieve performance stats, mostly for our timestamping:
    tDeadline = ovr_GetTimeInSeconds() + 0.5;
    do {
        if (OVR_FAILURE(ovr_GetPerfStats(oculus->hmd, &perfStats))) {
            ovr_GetLastErrorInfo(&errorInfo);
            if (verbosity > 0)
                printf("PsychOculusVRCore1-ERROR: ovr_GetPerfStats() failed: %s\n", errorInfo.ErrorString);
            PsychErrorExitMsg(PsychError_system, "Failed to get performance and timing info from VR compositor.");
        }

    } while ((oculus->frameIndex > 0) && (perfStats.FrameStatsCount < 1) && (ovr_GetTimeInSeconds() < tDeadline));

    if (verbosity > 3) {
        printf("\nPsychOculusVRCore1: DEBUG: FrameStatsCount=%i, AnyFrameStatsDropped=%i\n", perfStats.FrameStatsCount,
               (int) perfStats.AnyFrameStatsDropped);
    }

    if (perfStats.FrameStatsCount > 0) {
        PsychAllocOutStructArray(1, kPsychArgOptional, perfStats.FrameStatsCount, FieldCount, FieldNames, &frameT);
        for (i = 0; i < perfStats.FrameStatsCount; i++) {
            // HMD Vsync counter:
            PsychSetStructArrayDoubleElement("HmdVsyncIndex", i, perfStats.FrameStats[i].HmdVsyncIndex, frameT);

            // Our sbc counter:
            PsychSetStructArrayDoubleElement("AppFrameIndex", i, perfStats.FrameStats[i].AppFrameIndex, frameT);

            // Clock-Sync PTB timebase vs. Oculus timebase:
            PsychGetAdjustedPrecisionTimerSeconds(&tNow);
            tHMD = ovr_GetTimeInSeconds();
            PsychSetStructArrayDoubleElement("HMDTime", i, tHMD, frameT);
            PsychSetStructArrayDoubleElement("GetSecsTime", i, tNow, frameT);

            // Time between oculus->sensorSampleTime and visual onset (video frame midpoint of scanout):
            PsychSetStructArrayDoubleElement("AppMotionToPhotonLatency", i, perfStats.FrameStats[i].AppMotionToPhotonLatency, frameT);
            PsychSetStructArrayDoubleElement("RecentSensorSampleTime", i, oculus->sensorSampleTime, frameT);

            // Compute absolute stimulus onset (mid-point), remap to PTB GetSecs time:
            tStimOnset = (tNow - tHMD)  + oculus->sensorSampleTime + perfStats.FrameStats[i].AppMotionToPhotonLatency;
            PsychSetStructArrayDoubleElement("StimulusOnsetTime", i, tStimOnset, frameT);

            // Compute virtual start of VBLANK time as stimulus onset - half a HMD video refresh duration:
            tVBL = tStimOnset - 0.5 * (1.0 / oculus->hmdDesc.DisplayRefreshRate);
            PsychSetStructArrayDoubleElement("VBlankTime", i, tVBL, frameT);

            // Queue ahead for application. Citation from the v 1.11 SDK docs:
            // "Amount of queue-ahead in seconds provided to the app based on performance and overlap of CPU & GPU utilization
            // A value of 0.0 would mean the CPU & GPU workload is being completed in 1 frame's worth of time, while
            // 11 ms (on the CV1) of queue ahead would indicate that the app's CPU workload for the next frame is
            // overlapping the app's GPU workload for the current frame."
            PsychSetStructArrayDoubleElement("AppQueueAheadTime", i, perfStats.FrameStats[i].AppQueueAheadTime, frameT);
        }
    }

    oculus->frameIndex++;

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1GetEyePose(void)
{
    static char useString[] = "[eyePose, eyeIndex] = PsychOculusVRCore1('GetEyePose', oculusPtr, renderPass);";
    //                          1        2                                           1          2
    static char synopsisString[] =
    "Return current predicted pose vector for an eye for Oculus device 'oculusPtr'.\n"
    "'renderPass' is the view render pass for which to provide the data: 0 = First pass, 1 = Second pass.\n"
    "Return value is the vector 'eyePose' which defines the position and orientation for the eye corresponding "
    "to the requested renderPass ie. 'eyePose' = [posX, posY, posZ, rotX, rotY, rotZ, rotW].\n"
    "The second return value is the 'eyeIndex', the index of the eye whose view should be rendered. This would "
    "be 0 for left eye, and 1 for right eye, and could be used to select the target render view via, e.g.,\n"
    "Screen('SelectStereoDrawBuffer', window, eyeIndex);\n"
    "Which 'eyeIndex' corresponds to the first or second 'renderPass', ie., if the left eye should be rendered "
    "first, or if the right eye should be rendered first, depends on the visual scanning order of the HMD "
    "display panel - is it top to bottom, left to right, or right to left? This function provides that optimized "
    "mapping. Using this function to query the parameters for render setup of an eye can provide a bit more "
    "accuracy in rendering, at the expense of more complex usercode. If you want the more simple solution, just "
    "omit this function and use the eyePose's provided for both eyes by the 'StartRender' method.\n";

    static char seeAlsoString[] = "StartRender";

    int handle, renderPass;
    PsychOculusDevice *oculus;
    ovrVector3f HmdToEyeOffset[2];
    ovrEyeType eye;
    double *outM;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // Get renderPass:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &renderPass);
    if (renderPass < 0 || renderPass > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'renderPass' specified. Must be 0 or 1 for first or second pass.");

    // Get eye pose:
    eye = renderPass;
    HmdToEyeOffset[0] = oculus->eyeRenderDesc[0].HmdToEyeOffset;
    HmdToEyeOffset[1] = oculus->eyeRenderDesc[1].HmdToEyeOffset;
    ovr_GetEyePoses(oculus->hmd, 0, FALSE, HmdToEyeOffset, oculus->outEyePoses, &oculus->sensorSampleTime);

    // Eye pose as raw data:
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, 7, 1, &outM);

    // Position (x,y,z):
    outM[0] = oculus->outEyePoses[eye].Position.x;
    outM[1] = oculus->outEyePoses[eye].Position.y;
    outM[2] = oculus->outEyePoses[eye].Position.z;

    // Orientation as a quaternion (x,y,z,w):
    outM[3] = oculus->outEyePoses[eye].Orientation.x;
    outM[4] = oculus->outEyePoses[eye].Orientation.y;
    outM[5] = oculus->outEyePoses[eye].Orientation.z;
    outM[6] = oculus->outEyePoses[eye].Orientation.w;

    // Copy out preferred eye render order for info:
    PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) eye);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1GetHSWState(void)
{
    static char useString[] = "showHSW = PsychOculusVRCore1('GetHSWState', oculusPtr [, dismiss]);";
    //                         1                                          1            2
    static char synopsisString[] =
    "Return if health and safety warnings popup should be displayed for Oculus device 'oculusPtr'.\n"
    "'showHSW' is 1 if the popup should be displayed at the moment, 0 otherwise.\n"
    "You can set the optional parameter 'dismiss' to 1 to tell the runtime that the user "
    "dismissed the health and safety warning popup.\n"
    "Display of these warnings is mandated by the Oculus VR terms of the license.\n";

    static char seeAlsoString[] = "";
// TODOREMOVE MOST OF IT.
    int handle, dismiss;
    PsychOculusDevice *oculus;
//    ovrHSWDisplayState hasWarningState;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // Get optional 'dismiss' flag:
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &dismiss) && dismiss) {
        // Tell runtime the user accepted and dismissed the HSD display:
  //      ovrHmd_DismissHSWDisplay(oculus->hmd);
    }

//    ovrHmd_GetHSWDisplayState(oculus->hmd, &hasWarningState);
//    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) (hasWarningState.Displayed) ? 1 : 0);
    PsychCopyOutDoubleArg(1, kPsychArgOptional, 0);
    return(PsychError_none);
}

PsychError PSYCHOCULUSVR1LatencyTester(void)
{
    static char useString[] = "result = PsychOculusVRCore1('LatencyTester', oculusPtr, cmd);";
    //                         1                                            1          2
    static char synopsisString[] =
    "Drive or query latency tester for Oculus Rift DK2 device 'oculusPtr'.\n"
    "'cmd' is the command code:\n"
    "0 = Get next test draw color from 0 - 255 as [r,g,b] vector.\n"
    "1 = Get most recent latency test results in seconds as a 5 component vector 'latency':\n"
    "latency[1] = LatencyRender:     (seconds) Last time between render IMU sample and scanout.\n"
    "latency[2] = LatencyTimewarp:   (seconds) Last time between timewarp IMU sample and scanout.\n"
    "latency[3] = LatencyPostPresent (seconds) Average time between Vsync and scanout.\n"
    "latency[4] = ErrorRender        (seconds) Last error in render predicted scanout time.\n"
    "latency[5] = ErrorTimewarp      (seconds) Last error in timewarp predicted scanout time.\n"
    "\n"
    "If 'cmd' can't get executed, then an empty result [] is returned.\n"
    "\n";

    static char seeAlsoString[] = "";

    int handle, cmd;
    float latencies[5];
    PsychOculusDevice *oculus;
    double *outM;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // Get cmd code:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &cmd);
/* TODO REMOVE or REWRITE TOTALLY?
    if (cmd == 0) {
        // Get next latency test marker pixel color, update the runtimes internal
        // target match color with it, return color to us for our client rendering:
        // Return required clear color for latency test quad if latencytest is enabled, empty otherwise:
        if (ovrHmd_GetLatencyTest2DrawColor(oculus->hmd, &(oculus->rgbColorOut[0]))) {
            PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, 3, 1, &outM);
            outM[0] = (double) oculus->rgbColorOut[0];
            outM[1] = (double) oculus->rgbColorOut[1];
            outM[2] = (double) oculus->rgbColorOut[2];
            oculus->latencyTestActive = TRUE;
        }
        else {
            PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 0, 0, 1, &outM);
            oculus->latencyTestActive = FALSE;
        }
    }

    if (cmd == 1) {
        // Shall we do processing for the latency tester and return results?
        if (oculus->latencyTestActive) {
            // Seems so:
            if (ovrHmd_GetFloatArray(oculus->hmd, "DK2Latency", latencies, 5) == 5) {
                if (verbosity > 3) {
                    printf("PsychOculusVRCore1-EndFrameRender: Latency test result is: %f : %f : %f : %f : %f\n",
                           latencies[0] * 1000.0, latencies[1] * 1000.0, latencies[2] * 1000.0, latencies[3] * 1000.0,
                           latencies[4] * 1000.0);
                }

                // Return results:
                PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, 5, 1, &outM);
                outM[0] = (double) latencies[0];
                outM[1] = (double) latencies[1];
                outM[2] = (double) latencies[2];
                outM[3] = (double) latencies[3];
                outM[4] = (double) latencies[4];
            }
        }
        else {
            PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 0, 0, 1, &outM);
        }
    }
*/
    return(PsychError_none);
}
