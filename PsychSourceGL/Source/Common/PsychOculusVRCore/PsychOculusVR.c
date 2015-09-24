/*
 * PsychToolbox/Source/Common/PsychOculusVRCore/PsychOculusVR.c
 *
 * PROJECTS: PsychOculusVRCore only.
 *
 * AUTHORS:
 *
 * mario.kleiner.de@gmail.com   mk
 *
 * PLATFORMS:   All.
 *
 * HISTORY:
 *
 * 1.09.2015   mk      Created.
 *
 * DESCRIPTION:
 *
 * A Psychtoolbox driver for the Oculus VR virtual reality
 * head sets. The initial version will support the Rift DK2,
 * and possibly the old Rift DK1, although that hasn't been
 * tested.
 *
 */

#include "PsychOculusVR.h"

// Need this _USE_MATH_DEFINES so MSVC 2010 knows M_PI
#define _USE_MATH_DEFINES
#include <math.h>

// Includes from Oculus SDK 0.5:
#include "OVR_CAPI.h"

#ifdef  __cplusplus
// Only needed for C++ math support routines in OVR:: namespace.
#include "Extras/OVR_Math.h"
#endif

// Number of maximum simultaneously open kinect devices:
#define MAX_PSYCH_OCULUS_DEVS 10
#define MAX_SYNOPSIS_STRINGS 40

//declare variables local to this file.
static const char *synopsisSYNOPSIS[MAX_SYNOPSIS_STRINGS];

// Our device record:
typedef struct PsychOculusDevice {
    ovrHmd hmd;
    psych_bool isTracking;
    ovrSizei texSize[2];
    ovrFovPort ofov[2];
    ovrEyeRenderDesc eyeRenderDesc[2];
    ovrDistortionMesh eyeDistortionMesh[2];
    ovrVector2f UVScaleOffset[2][2];
    ovrMatrix4f timeWarpMatrices[2];
    ovrPosef headPose[2];
    ovrFrameTiming frameTiming;
    uint32_t frameIndex;
    ovrPosef outEyePoses[2];
} PsychOculusDevice;

PsychOculusDevice oculusdevices[MAX_PSYCH_OCULUS_DEVS];
static int available_devices = 0;
static unsigned int devicecount = 0;
static int verbosity = 3;
static psych_bool initialized = FALSE;

void InitializeSynopsis(void)
{
    int i = 0;
    const char **synopsis = synopsisSYNOPSIS;

    synopsis[i++] = "PsychOculusVRCore - A Psychtoolbox driver for Oculus VR hardware.\n";
    synopsis[i++] = "This driver allows to control Oculus Rift DK1/DK2 and future Oculus devices.\n";
    synopsis[i++] = "The PsychOculusVRCore driver is licensed to you under the terms of the MIT license.";
    synopsis[i++] = "See 'help License.txt' in the Psychtoolbox root folder for more details.\n";
    synopsis[i++] = "\n";
    synopsis[i++] = "Usage:";
    synopsis[i++] = "\n";
    synopsis[i++] = "oldVerbosity = PsychOculusVRCore('Verbosity' [, verbosity]);";
    synopsis[i++] = "numHMDs = PsychOculusVRCore('GetCount');";
    synopsis[i++] = "oculusPtr = PsychOculusVRCore('Open' [, deviceIndex=0]);";
    synopsis[i++] = "PsychOculusVRCore('Close' [, oculusPtr]);";
    synopsis[i++] = "showHSW = PsychOculusVRCore('GetHSWState', oculusPtr [, dismiss]);";
    synopsis[i++] = "PsychOculusVRCore('Start', oculusPtr);";
    synopsis[i++] = "PsychOculusVRCore('Stop', oculusPtr);";
    synopsis[i++] = "state = PsychOculusVRCore('GetTrackingState', oculusPtr [, predictionTime=0]);";
    synopsis[i++] = "[width, height, fovPort] = PsychOculusVRCore('GetFovTextureSize', oculusPtr, eye [, fov=[HMDRecommended]][, pixelsPerDisplay=1]);";
    synopsis[i++] = "[width, height, viewPx, viewPy, viewPw, viewPh, pptax, pptay, hmdShiftx, hmdShifty, hmdShiftz, meshVertices, meshIndices, uvScaleX, uvScaleY, uvOffsetX, uvOffsetY] = PsychOculusVRCore('GetUndistortionParameters', oculusPtr, eye, inputWidth, inputHeight [, tanfov]);";
    synopsis[i++] = "[projL, projR] = PsychOculusVRCore('GetStaticRenderParameters', oculusPtr [, clipNear=0.01][, clipFar=10000.0]);";
    synopsis[i++] = "[eyeRotStartMatrix, eyeRotEndMatrix] = PsychOculusVRCore('GetEyeTimewarpMatrices', oculusPtr, eye [, waitForTimewarpPoint=0]);";
    synopsis[i++] = "[eyePoseL, eyePoseR, modelviewL, modelviewR] = PsychOculusVRCore('StartRender', oculusPtr);";
    synopsis[i++] = "[eyePose, eyeIndex] = PsychOculusVRCore('GetEyePose', oculusPtr, renderPass);";
    synopsis[i++] = "PsychOculusVRCore('EndFrameTiming', oculusPtr);";
    synopsis[i++] = NULL;  //this tells PsychOculusVRDisplaySynopsis where to stop

    if (i > MAX_SYNOPSIS_STRINGS) {
        PrintfExit("%s: increase dimension of synopsis[] from %ld to at least %ld and recompile.", __FILE__, (long) MAX_SYNOPSIS_STRINGS, (long) i);
    }
}

PsychError PsychOculusVRDisplaySynopsis(void)
{
    int i;

    for (i = 0; synopsisSYNOPSIS[i] != NULL; i++)
        printf("%s\n",synopsisSYNOPSIS[i]);

    return(PsychError_none);
}

static double deg2rad(double deg)
{
    return deg / 360.0 * 2 * M_PI;
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

void PsychOculusVRCheckInit(psych_bool dontfail)
{
    // Already initialized? No op then.
    if (initialized) return;

    // Initialize Oculus VR runtime with default parameters:
    if (ovr_Initialize(NULL)) {
        if (verbosity >= 3) printf("PsychOculusVRCore-INFO: Oculus VR runtime version '%s' initialized.\n", ovr_GetVersionString());

        // Get count of available devices:
        available_devices = ovrHmd_Detect();
        if (available_devices < 0) {
            available_devices = 0;
            if (verbosity >= 2) printf("PsychOculusVRCore-WARNING: Could not connect to Oculus VR server process yet. Did you forget to start it?\n");
        }

        if (verbosity >= 3) printf("PsychOculusVRCore-INFO: At startup there are %i Oculus HMDs available.\n", available_devices);
        initialized = TRUE;
    }
    else {
        if (!dontfail)
            PsychErrorExitMsg(PsychError_system, "PsychOculusVRCore-ERROR: Initialization of VR runtime failed. Driver disabled!");
    }
}

void PsychOculusStop(int handle)
{
    PsychOculusDevice* oculus;
    oculus = PsychGetOculus(handle, TRUE);
    if (NULL == oculus || !oculus->isTracking) return;

    // Request stop of tracking:
    if (!ovrHmd_ConfigureTracking(oculus->hmd, 0, 0)) {
        if (verbosity >= 0) printf("PsychOculusVRCore-ERROR: Failed to stop tracking on device with handle %i [%s].\n", handle, ovrHmd_GetLastError(oculus->hmd));
        PsychErrorExitMsg(PsychError_system, "Stop of Oculus HMD tracking failed for reason given above.");
    }
    else if (verbosity >= 3) printf("PsychOculusVRCore-INFO: Tracking stopped on device with handle %i.\n", handle);

    oculus->isTracking = FALSE;

    return;
}

void PsychOculusClose(int handle)
{
    PsychOculusDevice* oculus;
    oculus = PsychGetOculus(handle, TRUE);
    if (NULL == oculus) return;

    // Stop device:
    PsychOculusStop(handle);

    // Release distortion meshes, if any:
    if (oculus->eyeDistortionMesh[0].pVertexData) {
        ovrHmd_DestroyDistortionMesh(&(oculus->eyeDistortionMesh[0]));
    }

    if (oculus->eyeDistortionMesh[1].pVertexData) {
        ovrHmd_DestroyDistortionMesh(&(oculus->eyeDistortionMesh[1]));
    }

    // Close the HMD:
    ovrHmd_Destroy(oculus->hmd);
    oculus->hmd = NULL;
    if (verbosity >= 3) printf("PsychOculusVRCore-INFO: Closed Oculus HMD with handle %i.\n", handle);

    // Done with this device:
    devicecount--;
}

void PsychOculusVRInit(void) {
    int handle;

    for (handle = 0 ; handle < MAX_PSYCH_OCULUS_DEVS; handle++)
        oculusdevices[handle].hmd = NULL;

    available_devices = 0;
    devicecount = 0;
    initialized = FALSE;
}

PsychError PsychOculusVRShutDown(void) {
    int handle;

    if (initialized) {
        for (handle = 0 ; handle < MAX_PSYCH_OCULUS_DEVS; handle++)
            PsychOculusClose(handle);

        // Shutdown runtime:
        ovr_Shutdown();

        if (verbosity >= 3) printf("PsychOculusVRCore-INFO: Oculus VR runtime shutdown complete. Bye!\n");
    }
    initialized = FALSE;

    return(PsychError_none);
}

PsychError PSYCHOCULUSVRVerbosity(void)
{
    static char useString[] = "oldVerbosity = PsychOculusVRCore('Verbosity' [, verbosity]);";
    static char synopsisString[] = "Returns and optionally sets level of 'verbosity' for driver debug output.\n"
                                   "'verbosity' = New level of verbosity: 0 = Silent, 1 = Errors only, 2 = Warnings, 3 = Info, 4 = Debug.\n";
    static char seeAlsoString[] = "";

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if( PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Return optional old verbosity setting:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, verbosity);

    // Get optional new verbosity setting:
    PsychCopyInIntegerArg(1, kPsychArgOptional, &verbosity);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVRGetCount(void)
{
    static char useString[] = "numHMDs = PsychOculusVRCore('GetCount');";
    static char synopsisString[] =  "Returns count of currently connected HMDs.\n"
                                    "Returns -1 if the runtime or server couldn't get initialized.\n";
    static char seeAlsoString[] = "Open";

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if( PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(TRUE);
    if (!initialized) {
        available_devices = -1;
    }
    else {
        available_devices = ovrHmd_Detect();
        if (available_devices < 0) {
            available_devices = -1;
            if (verbosity >= 2) printf("PsychOculusVRCore-WARNING: Could not connect to Oculus VR server process yet. Did you forget to start it?\n");
        }
    }

    PsychCopyOutDoubleArg(1, kPsychArgOptional, available_devices);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVROpen(void)
{
    static char useString[] = "oculusPtr = PsychOculusVRCore('Open' [, deviceIndex=0]);";
    //                                                          1
    static char synopsisString[] =
        "Open connection to Oculus VR HMD, return a 'oculusPtr' handle to it.\n\n"
        "The call tries to open the HMD with index 'deviceIndex', or the "
        "first detected HMD, if 'deviceIndex' is omitted. You can pass in a 'deviceIndex' "
        "of -1 to open an emulated HMD. It doesn't provide any sensor input, but allows "
        "some basic testing and debugging of VR software nonetheless.\n"
        "The returned handle can be passed to the other subfunctions to operate the device.\n";

    static char seeAlsoString[] = "GetCount Close";

    PsychOculusDevice* oculus;
    int deviceIndex = 0;
    int handle = 0;
    unsigned int oldCaps, newCaps;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if( PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Find a free device slot:
    for (handle = 0; (handle < MAX_PSYCH_OCULUS_DEVS) && oculusdevices[handle].hmd; handle++);
    if ((handle >= MAX_PSYCH_OCULUS_DEVS) || oculusdevices[handle].hmd) PsychErrorExitMsg(PsychError_internal, "Maximum number of simultaneously open Oculus VR devices reached.");

    // Get optional Oculus device index:
    PsychCopyInIntegerArg(1, kPsychArgOptional, &deviceIndex);

    // Don't support anything than a single "default" OculusVR Rift yet - A limitation of the current SDK:
    if (deviceIndex < -1) PsychErrorExitMsg(PsychError_user, "Invalid 'deviceIndex' provided. Must be greater or equal to zero!");

    available_devices = ovrHmd_Detect();
    if (available_devices < 0) {
        available_devices = 0;
        if (verbosity >= 2) printf("PsychOculusVRCore-WARNING: Could not connect to Oculus VR server process yet. Did you forget to start it?\n");
    }

    if ((deviceIndex >= 0) && (deviceIndex >= available_devices)) {
        if (verbosity >= 0) printf("PsychOculusVRCore-ERROR: Invalid deviceIndex %i >= number of available HMDs %i.\n", deviceIndex, available_devices);
        PsychErrorExitMsg(PsychError_user, "Invalid 'deviceIndex' provided. Not enough HMDs available!");
    }

    // Zero init device structure:
    memset(&oculusdevices[handle], 0, sizeof(PsychOculusDevice));

    oculus = &oculusdevices[handle];

    // Try to open real or emulated HMD with deviceIndex:
    if (deviceIndex >= 0) {
        // The real thing:
        oculusdevices[handle].hmd = ovrHmd_Create(deviceIndex);
        if (NULL == oculusdevices[handle].hmd) {
            if (verbosity >= 0) {
                printf("PsychOculusVRCore-ERROR: Failed to connect to Oculus Rift with deviceIndex %i. This could mean that the device\n", deviceIndex);
                printf("PsychOculusVRCore-ERROR: is already in use by another application or driver.\n");
            }
            PsychErrorExitMsg(PsychError_user, "Could not connect to Rift device with given 'deviceIndex'! [ovrHmd_Create() failed]");
        }
        else if (verbosity >= 3) {
            printf("PsychOculusVRCore-INFO: Opened Oculus Rift with deviceIndex %i as handle %i.\n", deviceIndex, handle + 1);
        }
    }
    else {
        // Emulated: Simulate a Rift DK2.
        oculusdevices[handle].hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
        if (verbosity >= 3) printf("PsychOculusVRCore-INFO: Opened an emulated Oculus Rift DK2 as handle %i.\n", handle + 1);
    }

    // Query current enabled caps:
    oldCaps = ovrHmd_GetEnabledCaps(oculus->hmd);

    // Set enabled HMD caps:
    ovrHmd_SetEnabledCaps(oculus->hmd, oldCaps | ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);

    // Requery to double-check if we got what we wanted:
    newCaps = ovrHmd_GetEnabledCaps(oculus->hmd);

    // Stats for nerds:
    if (verbosity >= 3) {
        printf("PsychOculusVRCore-INFO: Product: %s - Manufacturer: %s - SerialNo: %s [VID: 0x%x PID: 0x%x]\n",
               oculus->hmd->ProductName, oculus->hmd->Manufacturer, oculus->hmd->SerialNumber, (int) oculus->hmd->VendorId, (int) oculus->hmd->ProductId);
        printf("PsychOculusVRCore-INFO: Firmware version: %i.%i\n", (int) oculus->hmd->FirmwareMajor, (int) oculus->hmd->FirmwareMinor);
        printf("PsychOculusVRCore-INFO: CameraFrustumHFovInRadians: %f - CameraFrustumVFovInRadians: %f\n", oculus->hmd->CameraFrustumHFovInRadians, oculus->hmd->CameraFrustumVFovInRadians);
        printf("PsychOculusVRCore-INFO: CameraFrustumNearZInMeters: %f - CameraFrustumFarZInMeters:  %f\n", oculus->hmd->CameraFrustumNearZInMeters, oculus->hmd->CameraFrustumFarZInMeters);
        printf("PsychOculusVRCore-INFO: Panel size in pixels w x h = %i x %i [WindowPos %i x %i]\n", oculus->hmd->Resolution.w, oculus->hmd->Resolution.h, oculus->hmd->WindowsPos.x, oculus->hmd->WindowsPos.y);
        printf("PsychOculusVRCore-INFO: DisplayDeviceName: %s\n", oculus->hmd->DisplayDeviceName);
        printf("PsychOculusVRCore-INFO: OldCaps: LowPersistence=%i : DynamicPrediction=%i\n", (oldCaps & ovrHmdCap_LowPersistence) ? 1 : 0, (oldCaps & ovrHmdCap_DynamicPrediction) ? 1 : 0);
        printf("PsychOculusVRCore-INFO: NewCaps: LowPersistence=%i : DynamicPrediction=%i\n", (newCaps & ovrHmdCap_LowPersistence) ? 1 : 0, (newCaps & ovrHmdCap_DynamicPrediction) ? 1 : 0);
        printf("PsychOculusVRCore-INFO: ----------------------------------------------------------------------------------\n");
    }

    // Increment count of open devices:
    devicecount++;

    // Return device handle: We use 1-based handle indexing to make life easier for Octave/Matlab:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, handle + 1);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVRClose(void)
{
    static char useString[] = "PsychOculusVR('Close' [, oculusPtr]);";
    //                                                  1
    static char synopsisString[] =
        "Close connection to Oculus Rift device 'oculusPtr'. Do nothing if no such device is open.\n"
        "If the optional 'oculusPtr' is omitted, then close all open devices and shutdown the driver, "
        "ie. perform the same cleanup as if 'clear PsychOculusVR' would be executed.\n";
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
        PsychOculusVRShutDown();
    }

    return(PsychError_none);
}

PsychError PSYCHOCULUSVRStart(void)
{
    static char useString[] = "PsychOculusVR('Start', oculusPtr);";
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
        if (verbosity >= 0) printf("PsychOculusVRCore-ERROR: Tried to start tracking on device %i, but tracking is already started.\n", handle);
        PsychErrorExitMsg(PsychError_user, "Tried to start tracking on HMD, but tracking already active.");
    }

    // Request start of tracking for retrieval of head orientation and position, with drift correction, e.g., via magnetometer.
    // Do not fail if retrieval of any of this information isn't supported by the given hardware, ie., the required set of caps is empty == 0.
    // Rift DK1 only had orientation tracking, with magnetometer based drift correction. Rift DK2 also has vision based position tracking and
    // drift correction. The software emulated Rift has none of these and just returns a "static" head. This will start tracking:
    if (!ovrHmd_ConfigureTracking(oculus->hmd, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0)) {
        if (verbosity >= 0) printf("PsychOculusVRCore-ERROR: Failed to start tracking on device with handle %i [%s].\n", handle, ovrHmd_GetLastError(oculus->hmd));
        PsychErrorExitMsg(PsychError_system, "Start of Oculus HMD tracking failed for reason given above.");
    }
    else if (verbosity >= 3) printf("PsychOculusVRCore-INFO: Tracking started on device with handle %i.\n", handle);

    oculus->frameIndex = 0;
    ovrHmd_ResetFrameTiming(oculus->hmd, oculus->frameIndex);

    oculus->isTracking = TRUE;

    // Tracking is running.
    return(PsychError_none);
}

PsychError PSYCHOCULUSVRStop(void)
{
    static char useString[] = "PsychOculusVR('Stop', oculusPtr);";
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

PsychError PSYCHOCULUSVRGetTrackingState(void)
{
    static char useString[] = "state = PsychOculusVRCore('GetTrackingState', oculusPtr [, predictionTime=0]);";
    static char synopsisString[] =
        "Return current state of head position and orientation tracking for Oculus device 'oculusPtr'.\n"
        "Head position and orientation is predicted for target time 'predictionTime' in seconds if provided, "
        "based on the latest measurements from the tracking hardware. If 'predictionTime' is omitted or set "
        "to zero, then no prediction is performed and the current state based on latest measurements is returned.\n\n"
        "'state' is a row vector with the following values reported at given index:\n"
        "1 = Time in seconds of predicted tracking state.\n"
        "[2,3,4] = Head position [x,y,z] in meters.\n"
        "[5,6,7,8] = Head orientation [x,y,z,w] as quaternion.\n"
        "[9,10,11] = Linear velocity [vx,vy,vz] in meters/sec.\n"
        "[12,13,14] = Angular velocity [rx,ry,rz] in radians/sec\n"
        "[15,16,17] = Linear acceleration [ax,ay,az] in meters/sec^2.\n"
        "[18,19,20] = Angular acceleration [rax,ray,raz] in radians/sec^2\n";

    static char seeAlsoString[] = "Start Stop";

    int handle;
    double predictionTime = 0.0;
    PsychOculusDevice *oculus;
    ovrTrackingState state;
    double* v;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    PsychCopyInDoubleArg(2, kPsychArgOptional, &predictionTime);

    // Get current tracking status info at time 0 ie., current measurements:
    state = ovrHmd_GetTrackingState(oculus->hmd, predictionTime);

    // Print out tracking status:
    if (verbosity >= 4) {
        printf("PsychOculusVRCore-INFO: Tracking state predicted for device %i at time %f.\n", handle, predictionTime);
        printf("PsychOculusVRCore-INFO: LastCameraFrameCounter = %i : Time %f : Status %i\n", state.LastCameraFrameCounter, state.HeadPose.TimeInSeconds, state.StatusFlags);
        printf("PsychOculusVRCore-INFO: HeadPose: Position    [x,y,z]   = [%f, %f, %f]\n", state.HeadPose.ThePose.Position.x, state.HeadPose.ThePose.Position.y, state.HeadPose.ThePose.Position.z);
        printf("PsychOculusVRCore-INFO: HeadPose: Orientation [x,y,z,w] = [%f, %f, %f, %f]\n", state.HeadPose.ThePose.Orientation.x, state.HeadPose.ThePose.Orientation.y, state.HeadPose.ThePose.Orientation.z, state.HeadPose.ThePose.Orientation.w);

        /// Current pose of the external camera (if present).
        /// This pose includes camera tilt (roll and pitch). For a leveled coordinate
        /// system use LeveledCameraPose.
        //ovrPosef       CameraPose;

        /// Camera frame aligned with gravity.
        /// This value includes position and yaw of the camera, but not roll and pitch.
        /// It can be used as a reference point to render real-world objects in the correct location.
        //ovrPosef       LeveledCameraPose;

        /// The most recent sensor data received from the HMD.
        //ovrSensorData  RawSensorData;

        /// Tracking status described by ovrStatusBits.
        //unsigned int   StatusFlags;

        /// Tag the vision processing results to a certain frame counter number.
        //uint32_t LastCameraFrameCounter;

    }

    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, 20, 1, &v);
    v[0] = state.HeadPose.TimeInSeconds;

    v[1] = state.HeadPose.ThePose.Position.x;
    v[2] = state.HeadPose.ThePose.Position.y;
    v[3] = state.HeadPose.ThePose.Position.z;

    v[4] = state.HeadPose.ThePose.Orientation.x;
    v[5] = state.HeadPose.ThePose.Orientation.y;
    v[6] = state.HeadPose.ThePose.Orientation.z;
    v[7] = state.HeadPose.ThePose.Orientation.w;

    v[8]  = state.HeadPose.LinearVelocity.x;
    v[9]  = state.HeadPose.LinearVelocity.y;
    v[10] = state.HeadPose.LinearVelocity.z;

    v[11] = state.HeadPose.AngularVelocity.x;
    v[12] = state.HeadPose.AngularVelocity.y;
    v[13] = state.HeadPose.AngularVelocity.z;

    v[14] = state.HeadPose.LinearAcceleration.x;
    v[15] = state.HeadPose.LinearAcceleration.y;
    v[16] = state.HeadPose.LinearAcceleration.z;

    v[17] = state.HeadPose.AngularAcceleration.x;
    v[18] = state.HeadPose.AngularAcceleration.y;
    v[19] = state.HeadPose.AngularAcceleration.z;

    return(PsychError_none);
}

PsychError PSYCHOCULUSVRGetFovTextureSize(void)
{
    static char useString[] = "[width, height, fovPort] = PsychOculusVRCore('GetFovTextureSize', oculusPtr, eye [, fov=[HMDRecommended]][, pixelsPerDisplay=1]);";
    //                          1      2       3                                             1          2      3                       4
    static char synopsisString[] =
    "Return recommended size of client renderbuffers for Oculus device 'oculusPtr'.\n"
    "'eye' which eye to provide the size for: 0 = Left, 1 = Right.\n"
    "'fov' Optional field of view in degrees, from line of sight: [leftdeg, rightdeg, updeg, downdeg]. "
    "If 'fov' is omitted, the Oculus runtime will be asked for a good default field of view and that "
    "will be used.\n"
    "'pixelsPerDisplay' Ratio of the number of render target pixels to display pixels at the center "
    "of distortion. Defaults to 1.0 if omitted. Lower values can improve performance, higher values "
    "give improved quality.\n"
    "\n"
    "Return values are 'width' for minimum recommended width of framebuffer in pixels and "
    "'height' for minimum recommended height of framebuffer in pixels. 'fovPort' is the field of view "
    "finally used for calculation of 'width' x 'height'. 'fovPort' is the tan()gens of the angles, not "
    "the angles themselves.\n";
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
        oculus->ofov[eyeIndex] = oculus->hmd->DefaultEyeFov[eyeIndex];
    }

    // Get optional pixelsPerDisplay parameter:
    pixelsPerDisplay = 1.0;
    PsychCopyInDoubleArg(4, kPsychArgOptional, &pixelsPerDisplay);
    if (pixelsPerDisplay <= 0.0) PsychErrorExitMsg(PsychError_user, "Invalid 'pixelsPerDisplay' specified. Must be greater than zero.");

    // Ask the api for optimal texture size, aka the size of the client draw buffer:
    oculus->texSize[eyeIndex] = ovrHmd_GetFovTextureSize(oculus->hmd, (ovrEyeType) eyeIndex, oculus->ofov[eyeIndex], (float) pixelsPerDisplay);

    // Return recommended width and height of drawBuffer:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, oculus->texSize[eyeIndex].w);
    PsychCopyOutDoubleArg(2, kPsychArgOptional, oculus->texSize[eyeIndex].h);

    PsychAllocOutDoubleMatArg(3, kPsychArgOptional, 4, 1, 1, &outFov);
    outFov[0] = oculus->ofov[eyeIndex].LeftTan;
    outFov[1] = oculus->ofov[eyeIndex].RightTan;
    outFov[2] = oculus->ofov[eyeIndex].UpTan;
    outFov[3] = oculus->ofov[eyeIndex].DownTan;

    return(PsychError_none);
}

PsychError PSYCHOCULUSVRGetUndistortionParameters(void)
{
    static char useString[] = "[width, height, viewPx, viewPy, viewPw, viewPh, pptax, pptay, hmdShiftx, hmdShifty, hmdShiftz, meshVertices, meshIndices, uvScaleX, uvScaleY, uvOffsetX, uvOffsetY] = PsychOculusVRCore('GetUndistortionParameters', oculusPtr, eye, inputWidth, inputHeight [, tanfov]);";
    //                          1      2       3       4       5       6       7      8      9          10         11         12            13           14        15        16         17                                                      1          2    3           4              5
    static char synopsisString[] =
    "Return parameters needed for rendering and undistortion for Oculus device 'oculusPtr'.\n"
    "'eye' which eye to provide the data: 0 = Left, 1 = Right.\n"
    "'inputWidth' = Width of the rendered input image buffer in pixels.\n"
    "'inputHeight' = Height of the rendered input image buffer in pixels.\n"
    "'tanfov' Optional field of view in tan()gens of degrees, from line of sight: [leftdeg, rightdeg, updeg, downdeg]. "
    "You can pass in the 'fovPort' value returned from PsychOculusVR('GetFovTextureSize'); "
    "Defaults to whatever has been set for the given eye in the last call to PsychOculusVR('GetFovTextureSize'); if omitted.\n"
    "\n"
    "Return values are lots [TBD].\n";
    static char seeAlsoString[] = "GetFovTextureSize";

    int handle, eyeIndex;
    PsychOculusDevice *oculus;
    int n, m, p, i;
    double *fov;
    unsigned int distortionCaps;
    ovrDistortionVertex* pVertexData;
    unsigned short* pIndexData;
    double *outVertexMesh, *outIndexMesh;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(17));
    PsychErrorExit(PsychCapNumInputArgs(5));
    PsychErrorExit(PsychRequireNumInputArgs(4));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    // Get eye index - left = 0, right = 1:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    // Get input texture width:
    PsychCopyInIntegerArg(3, kPsychArgRequired, &(oculus->texSize[eyeIndex].w));
    if (oculus->texSize[eyeIndex].w < 1) PsychErrorExitMsg(PsychError_user, "Invalid 'inputWidth' specified. Must be greater than zero.");

    // Get input texture height:
    PsychCopyInIntegerArg(4, kPsychArgRequired, &(oculus->texSize[eyeIndex].h));
    if (oculus->texSize[eyeIndex].h < 1) PsychErrorExitMsg(PsychError_user, "Invalid 'inputHeight' specified. Must be greater than zero.");

    // Get optional tanFov tangens of field of view in degrees in left,right,up,down direction from line of sight:
    if (PsychAllocInDoubleMatArg(5, kPsychArgOptional, &n, &m, &p, &fov)) {
        // Validate and assign:
        if (n * m * p != 4) PsychErrorExitMsg(PsychError_user, "Invalid 'tanfov' specified. Must be a 4-component vector of form [leftdeg, rightdeg, updeg, downdeg].");
        oculus->ofov[eyeIndex].LeftTan  = (float) fov[0];
        oculus->ofov[eyeIndex].RightTan = (float) fov[1];
        oculus->ofov[eyeIndex].UpTan    = (float) fov[2];
        oculus->ofov[eyeIndex].DownTan  = (float) fov[3];
    }

    // Return width and height of input texture - Just mirror out what we got:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, oculus->texSize[eyeIndex].w);
    PsychCopyOutDoubleArg(2, kPsychArgOptional, oculus->texSize[eyeIndex].h);

    // Get eye render description for this eye:
    oculus->eyeRenderDesc[eyeIndex] = ovrHmd_GetRenderDesc(oculus->hmd, (ovrEyeType) eyeIndex, oculus->ofov[eyeIndex]);

    // Override viewport size with the provided input texture size, as ovrHmd_GetRenderDesc() makes the
    // assumption that we render to a half-width viewport of the input renderbuffer, when we actually
    // use 2 separate input renderbuffers of appropriate size:
    oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Pos.x = 0;
    oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Pos.y = 0;
    oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Size.w = oculus->texSize[eyeIndex].w;
    oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Size.h = oculus->texSize[eyeIndex].h;

    if (verbosity > 3) {
        printf("PsychOculusVRCore-INFO: For HMD %i, eye %i - RenderDescription:\n", handle, eyeIndex);
        printf("PsychOculusVRCore-INFO: FoV: %f %f %f %f - %f %f %f %f\n", oculus->ofov[eyeIndex].LeftTan, oculus->ofov[eyeIndex].RightTan, oculus->ofov[eyeIndex].UpTan, oculus->ofov[eyeIndex].DownTan, oculus->eyeRenderDesc[eyeIndex].Fov.LeftTan, oculus->eyeRenderDesc[eyeIndex].Fov.RightTan, oculus->eyeRenderDesc[eyeIndex].Fov.UpTan, oculus->eyeRenderDesc[eyeIndex].Fov.DownTan);
        printf("PsychOculusVRCore-INFO: DistortedViewport: [x,y,w,h] = [%i, %i, %i, %i]\n", oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Pos.x, oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Pos.y, oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Size.w, oculus->eyeRenderDesc[eyeIndex].DistortedViewport.Size.h);
        printf("PsychOculusVRCore-INFO: PixelsPerTanAngleAtCenter: %f x %f\n", oculus->eyeRenderDesc[eyeIndex].PixelsPerTanAngleAtCenter.x, oculus->eyeRenderDesc[eyeIndex].PixelsPerTanAngleAtCenter.y);
        printf("PsychOculusVRCore-INFO: HmdToEyeViewOffset: [x,y,z] = [%f, %f, %f]\n", oculus->eyeRenderDesc[eyeIndex].HmdToEyeViewOffset.x, oculus->eyeRenderDesc[eyeIndex].HmdToEyeViewOffset.y, oculus->eyeRenderDesc[eyeIndex].HmdToEyeViewOffset.z);
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
    PsychCopyOutDoubleArg(9,  kPsychArgOptional, oculus->eyeRenderDesc[eyeIndex].HmdToEyeViewOffset.x);
    PsychCopyOutDoubleArg(10, kPsychArgOptional, oculus->eyeRenderDesc[eyeIndex].HmdToEyeViewOffset.y);
    PsychCopyOutDoubleArg(11, kPsychArgOptional, oculus->eyeRenderDesc[eyeIndex].HmdToEyeViewOffset.z);

    // See enum ovrDistortionCaps: ovrDistortionCap_TimeWarp | ovrDistortionCap_Vignette | ovrDistortionCap_Overdrive
    // distortionCaps = ovrDistortionCap_LinuxDevFullscreen | ovrDistortionCap_HqDistortion | ovrDistortionCap_FlipInput;
    // TODO: distortionCaps are not used by SDK version 0.5 for ovrHmd_CreateDistortionMesh(), but maybe by later
    // versions? Need to recheck once we use higher SDK versions.
    distortionCaps = 0;
    if (!ovrHmd_CreateDistortionMesh(oculus->hmd, oculus->eyeRenderDesc[eyeIndex].Eye, oculus->eyeRenderDesc[eyeIndex].Fov, distortionCaps, &(oculus->eyeDistortionMesh[eyeIndex]))) {
        if (verbosity > 0) printf("PsychOculusVRCore-ERROR: Failed to compute distortion mesh for HMD %i, eye %i: [%s]\n", handle, eyeIndex, ovrHmd_GetLastError(oculus->hmd));
        PsychErrorExitMsg(PsychError_system, "Failed to compute distortion mesh for eye.");
    }

    if (verbosity > 3) {
        printf("PsychOculusVRCore-INFO: Distortion mesh has %i vertices, %i indices for triangles.\n", oculus->eyeDistortionMesh[eyeIndex].VertexCount, oculus->eyeDistortionMesh[eyeIndex].IndexCount);
    }

    // Return vertex data for the distortion mesh:

    // Each mesh has 10 parameters per vertex:
    m = 10;
    // For given number of vertices:
    n = oculus->eyeDistortionMesh[eyeIndex].VertexCount;
    // And one layer for a 2D matrix:
    p = 1;
    PsychAllocOutDoubleMatArg(12, kPsychArgOptional, m, n, p, &outVertexMesh);

    pVertexData = oculus->eyeDistortionMesh[eyeIndex].pVertexData;
    for (i = 0; i < n; i++) {
        // Store i'th column for i'th vertex:

        // output vertex 2D (x,y) position:
        *(outVertexMesh++) = pVertexData->ScreenPosNDC.x;
        *(outVertexMesh++) = pVertexData->ScreenPosNDC.y;

        // Timewarp lerp factor:
        *(outVertexMesh++) = pVertexData->TimeWarpFactor;

        // Vignette fade factor:
        *(outVertexMesh++) = pVertexData->VignetteFactor;

        // The tangents of the horizontal and vertical eye angles for the red channel.
        *(outVertexMesh++) = pVertexData->TanEyeAnglesR.x;
        *(outVertexMesh++) = pVertexData->TanEyeAnglesR.y * -1.0;

        // The tangents of the horizontal and vertical eye angles for the green channel.
        *(outVertexMesh++) = pVertexData->TanEyeAnglesG.x;
        *(outVertexMesh++) = pVertexData->TanEyeAnglesG.y * -1.0;

        // The tangents of the horizontal and vertical eye angles for the blue channel.
        *(outVertexMesh++) = pVertexData->TanEyeAnglesB.x;
        *(outVertexMesh++) = pVertexData->TanEyeAnglesB.y * -1.0;

        // Advance to next vertex in mesh:
        pVertexData++;
    }

    // Return index data for the distortion mesh: The mesh is composed of triangles.
    m = p = 1;
    n = oculus->eyeDistortionMesh[eyeIndex].IndexCount;
    PsychAllocOutDoubleMatArg(13, kPsychArgOptional, m, n, p, &outIndexMesh);

    pIndexData = oculus->eyeDistortionMesh[eyeIndex].pIndexData;
    for (i = 0; i < n; i++) {
        // Store i'th index - Convert uint16 to double:
        *(outIndexMesh++) = (double) *(pIndexData++);
    }

    // Get UV texture sampling scale and offset:
    ovrHmd_GetRenderScaleAndOffset(oculus->eyeRenderDesc[eyeIndex].Fov,
                                   oculus->texSize[eyeIndex], oculus->eyeRenderDesc[eyeIndex].DistortedViewport,
                                   (ovrVector2f*) &(oculus->UVScaleOffset[eyeIndex]));

    // EyeToSourceUVScale:
    PsychCopyOutDoubleArg(14, kPsychArgOptional, oculus->UVScaleOffset[eyeIndex][0].x);
    PsychCopyOutDoubleArg(15, kPsychArgOptional, oculus->UVScaleOffset[eyeIndex][0].y);

    // EyeToSourceUVOffset:
    PsychCopyOutDoubleArg(16, kPsychArgOptional, oculus->UVScaleOffset[eyeIndex][1].x);
    PsychCopyOutDoubleArg(17, kPsychArgOptional, oculus->UVScaleOffset[eyeIndex][1].y);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVRGetEyeTimewarpMatrices(void)
{
    static char useString[] = "[eyeRotStartMatrix, eyeRotEndMatrix] = PsychOculusVRCore('GetEyeTimewarpMatrices', oculusPtr, eye [, waitForTimewarpPoint=0]);";
    //                          1                  2                                                          1          2      3
    static char synopsisString[] =
    "Return eye warp rotation matrices for timewarped undistortion for Oculus device 'oculusPtr'.\n"
    "'eye' which eye to provide the data: 0 = Left, 1 = Right.\n"
    "'waitForTimewarpPoint' If set to 1, stall execution of calling thread until next time warp point is reached. Defaults to zero.\n"
    "Return values are 4x4 'eyeRotStartMatrix' and 'eyeRotEndMatrix' for given eye.\n";
    static char seeAlsoString[] = "";

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

    if (waitForTimewarpPoint) {
        // Wait till time-warp point to reduce latency.
        tNow = ovr_GetTimeInSeconds();
        if (verbosity > 3)
            printf("PsychOculusVRCore-INFO: Waiting for %f msecs until next TimewarpPointSeconds %f.\n", 1000.0 * (oculus->frameTiming.TimewarpPointSeconds - tNow), oculus->frameTiming.TimewarpPointSeconds);

        ovr_WaitTillTime(oculus->frameTiming.TimewarpPointSeconds);
    }

    eye = oculus->hmd->EyeRenderOrder[eyeIndex];
    oculus->headPose[eye] = ovrHmd_GetHmdPosePerEye(oculus->hmd, eye);

    ovrHmd_GetEyeTimewarpMatrices(oculus->hmd, (ovrEyeType) eyeIndex,
                                  oculus->headPose[eyeIndex],
                                  oculus->timeWarpMatrices);

    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 4, 4, 1, &startMatrix);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(startMatrix++) = (double) oculus->timeWarpMatrices[0].M[j][i];

    PsychAllocOutDoubleMatArg(2, kPsychArgOptional, 4, 4, 1, &endMatrix);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(endMatrix++) = (double) oculus->timeWarpMatrices[1].M[j][i];

    return(PsychError_none);
}

PsychError PSYCHOCULUSVRGetStaticRenderParameters(void)
{
    static char useString[] = "[projL, projR] = PsychOculusVRCore('GetStaticRenderParameters', oculusPtr [, clipNear=0.01][, clipFar=10000.0]);";
    //                          1      2                                                   1            2                3
    static char synopsisString[] =
    "Retrieve static rendering parameters for Oculus device 'oculusPtr' at current settings.\n"
    "'clipNear' Optional near clipping plane for OpenGL. Defaults to 0.01.\n"
    "'clipFar' Optional far clipping plane for OpenGL. Defaults to 10000.0.\n"
    "Return arguments:\n"
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
    M = ovrMatrix4f_Projection(oculus->eyeRenderDesc[0].Fov, (float) clip_near, (float) clip_far, ovrProjection_RightHanded | ovrProjection_ClipRangeOpenGL);
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 4, 4, 1, &outM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(outM++) = (double) M.M[j][i];

    // Return right projection matrix as return argument 2:
    M = ovrMatrix4f_Projection(oculus->eyeRenderDesc[1].Fov, (float) clip_near, (float) clip_far, ovrProjection_RightHanded | ovrProjection_ClipRangeOpenGL);
    PsychAllocOutDoubleMatArg(2, kPsychArgOptional, 4, 4, 1, &outM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(outM++) = (double) M.M[j][i];

    return(PsychError_none);
}


PsychError PSYCHOCULUSVRStartRender(void)
{
    static char useString[] = "[eyePoseL, eyePoseR, modelviewL, modelviewR] = PsychOculusVRCore('StartRender', oculusPtr);";
    //                          1         2         3           4                                              1
    static char synopsisString[] =
    "Mark start of a new 3D head tracked render cycle for Oculus device 'oculusPtr'.\n"
    "Return values are the vectors which define the two cameras positions and orientations "
    "for the left eye and right eye 'eyePoseL' and 'eyePoseR'.\n";
    static char seeAlsoString[] = "GetEyePose EndFrameTiming";

    int handle;
    PsychOculusDevice *oculus;
    ovrTrackingState *outHmdTrackingState = NULL; // Return value not used for now.
    ovrVector3f hmdToEyeViewOffset[2];
    #ifdef  __cplusplus
    int i, j;
    #endif
    double *outM;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(4));
    PsychErrorExit(PsychCapNumInputArgs(1));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOculusVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    oculus = PsychGetOculus(handle, FALSE);

    //ovrEyeType eye = oculus->hmd->EyeRenderOrder[eyeIndex];
    //oculus->headPose[eye] = ovrHmd_GetHmdPosePerEye(oculus->hmd, eye);

    // Mark beginning of frame rendering. This takes timstamps and stuff:
    oculus->frameTiming = ovrHmd_BeginFrameTiming(oculus->hmd, oculus->frameIndex);

    // Get current eye poses for both eyes:
    hmdToEyeViewOffset[0] = oculus->eyeRenderDesc[0].HmdToEyeViewOffset;
    hmdToEyeViewOffset[1] = oculus->eyeRenderDesc[1].HmdToEyeViewOffset;
    ovrHmd_GetEyePoses(oculus->hmd, oculus->frameIndex, hmdToEyeViewOffset,
                       oculus->outEyePoses, outHmdTrackingState);

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

    // Only compile this if C++ compiled:
    #ifdef  __cplusplus
    {
        // Invert camera translation vectors:
        oculus->outEyePoses[0].Position.x *= -1;
        oculus->outEyePoses[0].Position.y *= -1;
        oculus->outEyePoses[0].Position.z *= -1;

        oculus->outEyePoses[1].Position.x *= -1;
        oculus->outEyePoses[1].Position.y *= -1;
        oculus->outEyePoses[1].Position.z *= -1;

        // Return left modelview matrix as return argument 3:
        OVR::Quatf orientationL = OVR::Quatf(oculus->outEyePoses[0].Orientation);
        OVR::Matrix4f viewL = OVR::Matrix4f(orientationL.Inverted()) * OVR::Matrix4f::Translation(oculus->outEyePoses[0].Position);

        PsychAllocOutDoubleMatArg(3, kPsychArgOptional, 4, 4, 1, &outM);
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                *(outM++) = (double) viewL.M[j][i];

        // Return right modelview matrix as return argument 4:
        OVR::Quatf orientationR = OVR::Quatf(oculus->outEyePoses[1].Orientation);
        OVR::Matrix4f viewR = OVR::Matrix4f(orientationR.Inverted()) * OVR::Matrix4f::Translation(oculus->outEyePoses[1].Position);

        PsychAllocOutDoubleMatArg(4, kPsychArgOptional, 4, 4, 1, &outM);
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                *(outM++) = (double) viewR.M[j][i];
    }
    #endif

    return(PsychError_none);
}

PsychError PSYCHOCULUSVREndFrameTiming(void)
{
    static char useString[] = "PsychOculusVR('EndFrameTiming', oculusPtr);";
    //                                                         1
    static char synopsisString[] =
    "Mark start of a new 3D head tracked render cycle for Oculus device 'oculusPtr'.\n"
    "Return values are the 4x4 OpenGL modelview matrices which define the two cameras "
    "for the left eye and right eye 'modelviewL' and 'modelviewR'.\n";
    static char seeAlsoString[] = "StartRender";

    int handle;
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

    // Tell runtime that the last rendered frame was just presented onscreen,
    // aka OpenGL bufferswap completed:
    ovrHmd_EndFrameTiming(oculus->hmd);
    oculus->frameIndex++;

    return(PsychError_none);
}

PsychError PSYCHOCULUSVRGetEyePose(void)
{
    static char useString[] = "[eyePose, eyeIndex] = PsychOculusVRCore('GetEyePose', oculusPtr, renderPass);";
    //                          1        2                                           1          2
    static char synopsisString[] =
    "Return current predicted pose vector for an eye for Oculus device 'oculusPtr'.\n"
    "'renderPass' is the view render pass for which to provide the data: 0 = First pass, 1 = Second pass.\n"
    "Return value is the vector 'eyePose' which defines the position and orientation for the eye corresponding "
    "to the requested renderPass ie. 'eyePose' = [posX, posY, posZ, rotX, rotY, rotZ, rotW].\n"
    "The second return value is the 'eyeIndex', the index of the eye whose view should be rendered. This would "
    "be 0 for left eye, and 1 for right eye, and could be used to select the target render view via, e.g., "
    "Screen('SelectStereoDrawBuffer', window, eyeIndex);\n"
    "Which 'eyeIndex' corresponds to the first or second 'renderPass', ie., if the left eye should be rendered "
    "first, or if the right eye should be rendered first, depends on the visual scanning order of the HMDs "
    "display panel - is it top to bottom, left to right, or right to left? This function provides that optimized "
    "mapping.\n";

    static char seeAlsoString[] = "StartRender";

    int handle, renderPass;
    PsychOculusDevice *oculus;
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
    eye = oculus->hmd->EyeRenderOrder[renderPass];
    oculus->headPose[eye] = ovrHmd_GetHmdPosePerEye(oculus->hmd, eye);

    // Eye pose as raw data:
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, 7, 1, &outM);

    // Position (x,y,z):
    outM[0] = oculus->headPose[eye].Position.x;
    outM[1] = oculus->headPose[eye].Position.y;
    outM[2] = oculus->headPose[eye].Position.z;

    // Orientation as a quaternion (x,y,z,w):
    outM[3] = oculus->headPose[eye].Orientation.x;
    outM[4] = oculus->headPose[eye].Orientation.y;
    outM[5] = oculus->headPose[eye].Orientation.z;
    outM[6] = oculus->headPose[eye].Orientation.w;

    // Copy out preferred eye render order for info:
    PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) eye);

    return(PsychError_none);
}

PsychError PSYCHOCULUSVRGetHSWState(void)
{
    static char useString[] = "showHSW = PsychOculusVRCore('GetHSWState', oculusPtr [, dismiss]);";
    //                         1                                          1            2
    static char synopsisString[] =
    "Return if health and safety warnings popup should be displayed for Oculus device 'oculusPtr'.\n"
    "'showHSW' is 1 if the popup should be displayed at the moment, 0 otherwise.\n"
    "You can set the optional parameter 'dismiss' to 1 to tell the runtime that the user "
    "dismissed the health and safety warning popup.\n"
    "Display of these warnings is mandated by the Oculus VR terms of the license.\n";

    static char seeAlsoString[] = "";

    int handle, dismiss;
    PsychOculusDevice *oculus;
    ovrHSWDisplayState hasWarningState;

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
        ovrHmd_DismissHSWDisplay(oculus->hmd);
    }

    ovrHmd_GetHSWDisplayState(oculus->hmd, &hasWarningState);
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) (hasWarningState.Displayed) ? 1 : 0);

    return(PsychError_none);
}
