/*
 * PsychToolbox/Source/Common/PsychOpenHMDVRCore/PsychOpenHMDVRCore.c
 *
 * PROJECTS: PsychOpenHMDVRCore only.
 *
 * AUTHORS:
 *
 * mario.kleiner.de@gmail.com   mk
 *
 * PLATFORMS:   All.
 *
 * HISTORY:
 *
 * 30.07.2017   mk      Created. Derived from the PsychOculusVRCore driver for the v0.5 Oculus SDK.
 *
 * DESCRIPTION:
 *
 * A Psychtoolbox driver for the virtual reality head sets supported via OpenHMD.
 *
 * TODO:
 *
 * - Support for enumeration of OpenHMD devices with their properties (vendor, model, path, properties).
 * - Support for filtering out emulated HMD devices and non-HMD's for 'Open' function, unless dummy
 *   device is requested.
 *
 */

#include "PsychOpenHMDVRCore.h"

// Need this _USE_MATH_DEFINES so MSVC 2010 knows M_PI
#define _USE_MATH_DEFINES
#include <math.h>

// Includes from OpenHMD:
#include "openhmd.h"

// Number of maximum simultaneously open VR devices:
#define MAX_PSYCH_OPENHMD_DEVS 10
#define MAX_SYNOPSIS_STRINGS 40

//declare variables local to this file.
static const char *synopsisSYNOPSIS[MAX_SYNOPSIS_STRINGS];

// Our device record:
typedef struct PsychOpenHMDDevice {
    ohmd_device* hmd;
    ohmd_device* controller[3];
    psych_bool riftRemote;
    psych_bool isTracking;
    unsigned int panelSize[2];
    unsigned int texSize[2][2];
    double       ofov[2][4];
    uint32_t     frameIndex;
    double       oldpos[3];
    double       oldhandpos[2][3];
} PsychOpenHMDDevice;

PsychOpenHMDDevice openhmddevices[MAX_PSYCH_OPENHMD_DEVS];
static int available_devices = 0;
static unsigned int devicecount = 0;
static int verbosity = 3;
static psych_bool initialized = FALSE;

// The global operation context of OpenHMD:
struct ohmd_context *ctx;

void InitializeSynopsis(void)
{
    int i = 0;
    const char **synopsis = synopsisSYNOPSIS;

    synopsis[i++] = "PsychOpenHMDVRCore - A Psychtoolbox driver for VR hardware supported by OpenHMD.\n";
    synopsis[i++] = "This driver allows to control OpenHMD supported hardware like Rift DK1/DK2/CV1, HTC Vive, PSVR.\n";
    synopsis[i++] = "The PsychOpenHMDVRCore driver is licensed to you under the terms of the MIT license.";
    synopsis[i++] = "See 'help License.txt' in the Psychtoolbox root folder for more details.\n";
    synopsis[i++] = "The driver requires the OpenHMD library version 0.3+ to work, which is";
    synopsis[i++] = "distributed under the Boost 1.0 license: http://www.openhmd.net\n";
    synopsis[i++] = "Usage:";
    synopsis[i++] = "\n";
    synopsis[i++] = "oldVerbosity = PsychOpenHMDVRCore('Verbosity' [, verbosity]);";
    synopsis[i++] = "numHMDs = PsychOpenHMDVRCore('GetCount');";
    synopsis[i++] = "[openhmdPtr, modelName, panelSizeX, panelSizeY, controllerTypes, controllerFlags] = PsychOpenHMDVRCore('Open' [, deviceIndex=0]);";
    synopsis[i++] = "PsychOpenHMDVRCore('Close' [, openhmdPtr]);";
    synopsis[i++] = "oldPersistence = PsychOpenHMDVRCore('SetLowPersistence', openhmdPtr [, lowPersistence]);";
    synopsis[i++] = "PsychOpenHMDVRCore('Start', openhmdPtr);";
    synopsis[i++] = "PsychOpenHMDVRCore('Stop', openhmdPtr);";
    synopsis[i++] = "input = PsychOpenHMDVRCore('GetInputState', openhmdPtr, controllerType);";
    synopsis[i++] = "[state, touch] = PsychOpenHMDVRCore('GetTrackingState', openhmdPtr [, predictionTime=0]);";
    synopsis[i++] = "pulseEndTime = PsychOpenHMDVRCore('HapticPulse', openhmdPtr, controllerType [, duration=2.5][, freq=1.0][, amplitude=1.0]);";
    synopsis[i++] = "[projL, projR, ipd] = PsychOpenHMDVRCore('GetStaticRenderParameters', openhmdPtr [, clipNear=0.01][, clipFar=10000.0]);";
    synopsis[i++] = "[eyePose, eyeIndex, glModelviewMatrix] = PsychOpenHMDVRCore('GetEyePose', openhmdPtr, renderPass);";
    synopsis[i++] = "[width, height, fovPort] = PsychOpenHMDVRCore('GetFovTextureSize', openhmdPtr, eye, metersPerTanAngleAtCenter [, fov=[HMDRecommended]]);";
    synopsis[i++] = "[width, height, hmdShiftx, hmdShifty, hmdShiftz, abberation, distortion, scrnHorSize, scrnVertSize] = PsychOpenHMDVRCore('GetUndistortionParameters', openhmdPtr, eye [, inputWidth][, inputHeight][, fov]);";
    synopsis[i++] = "[vertexShaderSrc, fragmentShaderSrc] = PsychOpenHMDVRCore('GetCorrectionShaders', openhmdPtr);";
    synopsis[i++] = NULL;  //this tells PsychOpenHMDVRDisplaySynopsis where to stop

    if (i > MAX_SYNOPSIS_STRINGS) {
        PrintfExit("%s: Increase dimension of synopsis[] from %ld to at least %ld and recompile.", __FILE__, (long) MAX_SYNOPSIS_STRINGS, (long) i);
    }
}

PsychError PsychOpenHMDVRDisplaySynopsis(void)
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

PsychOpenHMDDevice* PsychGetOpenHMD(int handle, psych_bool dontfail)
{
    if (handle < 1 || handle > MAX_PSYCH_OPENHMD_DEVS || openhmddevices[handle-1].hmd == NULL) {
        if (!dontfail) {
            printf("PTB-ERROR: Invalid OpenHMD device handle %i passed. No such device open.\n", handle);
            PsychErrorExitMsg(PsychError_user, "Invalid OpenHMD handle.");
        }

        return(NULL);
    }

    return(&(openhmddevices[handle-1]));
}

void PsychOpenHMDVRCheckInit(psych_bool dontfail)
{
    int major, minor, patch;

    // Already initialized? No op then.
    if (initialized) return;

    ohmd_get_version(&major, &minor, &patch);

    if (OHMD_S_OK != ohmd_require_version(0, 3, 0)) {
        if (verbosity >= 1) printf("PsychOpenHMDVRCore-ERROR: OpenHMD VR runtime version %i.%i.%i is too old for us! We need at least version 0.3.0.\n", major, minor, patch);

        if (!dontfail)
            PsychErrorExitMsg(PsychError_system, "PsychOpenHMDVRCore-ERROR: Initialization of VR runtime failed, because OpenHMD version is too old! Driver disabled!");

        return;
    }

    // Initialize OpenHMD VR runtime with default parameters:
    if ((ctx = ohmd_ctx_create()) != NULL) {
        if (verbosity >= 3) printf("PsychOpenHMDVRCore-INFO: OpenHMD VR runtime version %i.%i.%i initialized.\n", major, minor, patch);

        // Get count of available devices:
        available_devices = ohmd_ctx_probe(ctx);;
        if (available_devices < 0) {
            available_devices = 0;
            if (verbosity >= 2) printf("PsychOpenHMDVRCore-WARNING: Failed to probe available OpenHMD VR devices: %s\n", ohmd_ctx_get_error(ctx));
        }

        if (verbosity >= 3) printf("PsychOpenHMDVRCore-INFO: At startup there are %i OpenHMD HMDs available.\n", available_devices);
        initialized = TRUE;
    }
    else {
        if (!dontfail)
            PsychErrorExitMsg(PsychError_system, "PsychOpenHMDVRCore-ERROR: Initialization of VR runtime failed. Driver disabled!");
    }
}

void PsychOpenHMDStop(int handle)
{
    PsychOpenHMDDevice* openhmd;
    openhmd = PsychGetOpenHMD(handle, TRUE);
    if (NULL == openhmd || !openhmd->isTracking) return;

    if (verbosity >= 4) printf("PsychOpenHMDVRCore-INFO: Tracking stopped on device with handle %i.\n", handle);

    openhmd->isTracking = FALSE;

    return;
}

void PsychOpenHMDClose(int handle)
{
    PsychOpenHMDDevice* openhmd;
    openhmd = PsychGetOpenHMD(handle, TRUE);
    if (NULL == openhmd) return;

    // Stop device:
    PsychOpenHMDStop(handle);

    if (openhmd->controller[0]) ohmd_close_device(openhmd->controller[0]);
    if (openhmd->controller[1]) ohmd_close_device(openhmd->controller[1]);
    if (openhmd->controller[2]) ohmd_close_device(openhmd->controller[2]);
    openhmd->controller[0] = NULL;
    openhmd->controller[1] = NULL;
    openhmd->controller[2] = NULL;
    openhmd->riftRemote = FALSE;

    // Close the HMD:
    ohmd_close_device(openhmd->hmd);
    openhmd->hmd = NULL;
    if (verbosity >= 4) printf("PsychOpenHMDVRCore-INFO: Closed OpenHMD HMD with handle %i.\n", handle);

    // Done with this device:
    devicecount--;
}

void PsychOpenHMDVRInit(void) {
    int handle;

    for (handle = 0 ; handle < MAX_PSYCH_OPENHMD_DEVS; handle++) {
        openhmddevices[handle].hmd = NULL;
        openhmddevices[handle].controller[0] = NULL;
        openhmddevices[handle].controller[1] = NULL;
        openhmddevices[handle].controller[2] = NULL;
    }

    available_devices = 0;
    devicecount = 0;
    ctx = NULL;
    initialized = FALSE;
}

PsychError PsychOpenHMDVRShutDown(void) {
    int handle;

    if (initialized) {
        for (handle = 0 ; handle < MAX_PSYCH_OPENHMD_DEVS; handle++)
            PsychOpenHMDClose(handle);

        // Shutdown runtime:
        ohmd_ctx_destroy(ctx);
        ctx = NULL;

        if (verbosity >= 4) printf("PsychOpenHMDVRCore-INFO: OpenHMD VR runtime shutdown complete.\n");
    }
    initialized = FALSE;

    return(PsychError_none);
}

PsychError PSYCHOPENHMDVRVerbosity(void)
{
    static char useString[] = "oldVerbosity = PsychOpenHMDVRCore('Verbosity' [, verbosity]);";
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

PsychError PSYCHOPENHMDVRGetCount(void)
{
    static char useString[] = "numHMDs = PsychOpenHMDVRCore('GetCount');";
    static char synopsisString[] = "Returns count of currently connected HMDs.\n"
                                   "Returns -1 if the runtime couldn't get initialized.\n";
    static char seeAlsoString[] = "Open";

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if( PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(TRUE);
    if (!initialized) {
        available_devices = -1;
    }
    else {
        // Only reprobe if context does not have any devices open already:
        available_devices = (devicecount == 0) ? ohmd_ctx_probe(ctx) : available_devices;
        if (available_devices < 0) {
            available_devices = -1;
            if (verbosity >= 2) printf("PsychOpenHMDVRCore-WARNING: Could not probe OpenHMD devices.\n");
        }
    }

    PsychCopyOutDoubleArg(1, kPsychArgOptional, available_devices);

    return(PsychError_none);
}

PsychError PSYCHOPENHMDVROpen(void)
{
    static char useString[] = "[openhmdPtr, modelName, panelSizeX, panelSizeY, controllerTypes, controllerFlags] = PsychOpenHMDVRCore('Open' [, deviceIndex=0]);";
    //                          1           2          3           4           5                6                                               1
    static char synopsisString[] =
        "Open connection to OpenHMD controlled VR HMD, return a 'openhmdPtr' handle to it.\n\n"
        "The call tries to open the HMD with index 'deviceIndex', or the "
        "first detected HMD, if 'deviceIndex' is omitted. You can pass in a 'deviceIndex' "
        "of -1 to open an emulated HMD. It doesn't provide any sensor input, but allows "
        "some basic testing and debugging of VR software nonetheless.\n"
        "The returned handle can be passed to the other subfunctions to operate the device.\n"
        "A second return argument 'modelName' returns the model name string of the OpenHMD device.\n"
        "'panelSizeX' is the horizontal panel resolution in pixels.\n"
        "'panelSizeY' is the vertical panel resolution in pixels.\n"
        "'controllerTypes' A bit mask of OVR.ControllerType_XXX flags describing the currently "
        "connected input controllers.\n"
        "'controllerFlags' A bit mask of controller capabilities: +1 = Rotational tracking, "
        "+2 = Positional tracking, +4 = Haptic feedback.\n";

    static char seeAlsoString[] = "GetCount Close";

    PsychOpenHMDDevice* openhmd;
    int device_class, idx_class;
    int device_flags, control_count;
    unsigned int controllerTypes = 0;
    unsigned int controllerFlags = 0;
    int trycount;
    int idx, deviceIndex = 0;
    int handle = 0;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(6));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    // Find a free device slot:
    for (handle = 0; (handle < MAX_PSYCH_OPENHMD_DEVS) && openhmddevices[handle].hmd; handle++);
    if ((handle >= MAX_PSYCH_OPENHMD_DEVS) || openhmddevices[handle].hmd) PsychErrorExitMsg(PsychError_internal, "Maximum number of simultaneously open OpenHMD VR devices reached.");

    // Get optional OpenHMD device index:
    PsychCopyInIntegerArg(1, kPsychArgOptional, &deviceIndex);

    if (deviceIndex < -1) PsychErrorExitMsg(PsychError_user, "Invalid 'deviceIndex' provided. Must be greater or equal to zero!");
    if (available_devices < 0) {
        available_devices = 0;
        if (verbosity >= 2) printf("PsychOpenHMDVRCore-WARNING: Could not find any OpenHMD supported devices.\n");
    }

    if ((deviceIndex >= 0) && (deviceIndex >= available_devices)) {
        if (verbosity >= 0) printf("PsychOpenHMDVRCore-ERROR: Invalid deviceIndex %i >= number of available HMDs %i.\n", deviceIndex, available_devices);
        PsychErrorExitMsg(PsychError_user, "Invalid 'deviceIndex' provided. Not enough HMDs available!");
    }

    // Zero init device structure:
    memset(&openhmddevices[handle], 0, sizeof(PsychOpenHMDDevice));

    openhmd = &openhmddevices[handle];

    // Try to open real or emulated HMD with deviceIndex:
    if (deviceIndex >= 0) {
        // The real thing: Try open 5 times, in case we collide with the openhmdkeepalive
        // daemon having the device opened for its keep-alive cycle at just this time:
        for (trycount = 0; trycount < 5; trycount++) {
            openhmddevices[handle].hmd = ohmd_list_open_device(ctx, deviceIndex);

            // Worked?
            if (openhmddevices[handle].hmd)
                break;

            // Nope. Wait a few milliseconds before retry, so the keeep alive daemon
            // can release the HMD if that is what keeps it from opening up to us:
            PsychWaitIntervalSeconds(0.5);
        }

        if (NULL == openhmddevices[handle].hmd) {
            if (verbosity >= 0) {
                printf("PsychOpenHMDVRCore-ERROR: Failed to connect to OpenHMD device with deviceIndex %i. This could mean that the device\n", deviceIndex);
                printf("PsychOpenHMDVRCore-ERROR: is already in use by another application or driver.\n");
            }
            PsychErrorExitMsg(PsychError_user, "Could not connect to OpenHMD device with given 'deviceIndex'! [ohmd_list_open_device() failed]");
        }
        else if (verbosity >= 3) {
            printf("PsychOpenHMDVRCore-INFO: Opened OpenHMD device with deviceIndex %i as handle %i.\n", deviceIndex, handle + 1);
        }
    }
    else {
        // Emulated:
        for (deviceIndex = 0; (deviceIndex < available_devices) && strcmp(ohmd_list_gets(ctx, deviceIndex, OHMD_PRODUCT), "Dummy Device"); deviceIndex++);
        if (deviceIndex == available_devices)
            PsychErrorExitMsg(PsychError_user, "Could not find OpenHMD dummy device!");
        openhmddevices[handle].hmd = ohmd_list_open_device(ctx, deviceIndex);
        if (verbosity >= 3) printf("PsychOpenHMDVRCore-INFO: Opened an emulated OpenHMD HMD as handle %i.\n", handle + 1);
    }

    // Get panel size in pixels:
    ohmd_device_geti(openhmd->hmd, OHMD_SCREEN_HORIZONTAL_RESOLUTION, &openhmd->panelSize[0]);
    ohmd_device_geti(openhmd->hmd, OHMD_SCREEN_VERTICAL_RESOLUTION, &openhmd->panelSize[1]);

    // Stats for nerds:
    if (verbosity >= 3) {
        printf("PsychOpenHMDVRCore-INFO: Product: %s - Manufacturer: %s - Path: %s\n",
               ohmd_list_gets(ctx, deviceIndex, OHMD_PRODUCT), ohmd_list_gets(ctx, deviceIndex, OHMD_VENDOR), ohmd_list_gets(ctx, deviceIndex, OHMD_PATH));
        printf("PsychOpenHMDVRCore-INFO: Panel size in pixels w x h = %i x %i\n", openhmd->panelSize[0], openhmd->panelSize[1]);
        printf("PsychOpenHMDVRCore-INFO: ----------------------------------------------------------------------------------\n");
    }

    // Increment count of open devices:
    devicecount++;

    // Return device handle: We use 1-based handle indexing to make life easier for Octave/Matlab:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, handle + 1);

    // Return product name:
    PsychCopyOutCharArg(2, kPsychArgOptional, (const char*) ohmd_list_gets(ctx, deviceIndex, OHMD_PRODUCT));

    // Display size h x v:
    PsychCopyOutDoubleArg(3, kPsychArgOptional, openhmd->panelSize[0]);
    PsychCopyOutDoubleArg(4, kPsychArgOptional, openhmd->panelSize[1]);

    // Is the device a HMD?
    ohmd_list_geti(ctx, deviceIndex, OHMD_DEVICE_CLASS, &device_class);
    if (device_class == OHMD_DEVICE_CLASS_HMD) {
        // Check if there are any associated controller/tracker devices, ie., non-HMD's
        // with the same device path. Iterate over all of them:
        for (idx = 0; idx < available_devices; idx++) {
            ohmd_list_geti(ctx, idx, OHMD_DEVICE_CLASS, &idx_class);
            ohmd_list_geti(ctx, idx, OHMD_DEVICE_FLAGS, &device_flags);

            if (!(device_flags & OHMD_DEVICE_FLAGS_NULL_DEVICE) &&
                (idx_class == OHMD_DEVICE_CLASS_CONTROLLER || idx_class == OHMD_DEVICE_CLASS_GENERIC_TRACKER) &&
                !strcmp(ohmd_list_gets(ctx, idx, OHMD_PATH), ohmd_list_gets(ctx, deviceIndex, OHMD_PATH))) {

                // Associated controller or generic tracker device. Open and assign it:
                if (device_flags & OHMD_DEVICE_FLAGS_LEFT_CONTROLLER) {
                    openhmd->controller[0] = ohmd_list_open_device(ctx, idx);
                    controllerTypes |= 0x0001;
                } else if (device_flags & OHMD_DEVICE_FLAGS_RIGHT_CONTROLLER) {
                    openhmd->controller[1] = ohmd_list_open_device(ctx, idx);
                    controllerTypes |= 0x0002;
                } else if (idx_class == OHMD_DEVICE_CLASS_CONTROLLER) {
                    // Rift CV-1 remote control. This is actually exposed as part
                    // of the openhmd->hmd itself, but needs different input treatment:
                    openhmd->riftRemote = TRUE;
                    controllerTypes |= 0x0004;
                } else if (!openhmd->controller[2]) {
                    // Something associated, but not a OHMD_DEVICE_CLASS_CONTROLLER,
                    // must be a OHMD_DEVICE_CLASS_GENERIC_TRACKER. Just open it:
                    openhmd->controller[2] = ohmd_list_open_device(ctx, idx);
                    controllerTypes |= 0x0008;
                }

                if (device_flags & OHMD_DEVICE_FLAGS_ROTATIONAL_TRACKING)
                    controllerFlags |= 1;

                if (device_flags & OHMD_DEVICE_FLAGS_POSITIONAL_TRACKING)
                    controllerFlags |= 2;

                #if defined OHMD_DEVICE_FLAGS_HAPTIC_FEEDBACK
                if (device_flags & OHMD_DEVICE_FLAGS_HAPTIC_FEEDBACK)
                    controllerFlags |= 4;
                #endif
            }
        }
    }

    // Query selected device itself for controls:
    ohmd_list_geti(ctx, deviceIndex, OHMD_DEVICE_FLAGS, &device_flags);
    if (!(device_flags & OHMD_DEVICE_FLAGS_NULL_DEVICE) &&
        !ohmd_device_geti(openhmd->hmd, OHMD_CONTROL_COUNT, &control_count) &&
        (control_count > 0)) {
        if (device_flags & OHMD_DEVICE_FLAGS_LEFT_CONTROLLER) {
            controllerTypes |= 0x0001;
        } else if (device_flags & OHMD_DEVICE_FLAGS_RIGHT_CONTROLLER) {
            controllerTypes |= 0x0002;
        } else if (device_class == OHMD_DEVICE_CLASS_CONTROLLER) {
            controllerTypes |= 0x0004;
        } else if (device_class == OHMD_DEVICE_CLASS_GENERIC_TRACKER) {
            controllerTypes |= 0x0008;
        } else if (device_class == OHMD_DEVICE_CLASS_HMD) {
            controllerTypes |= 0x0020;
        }

        if (device_flags & OHMD_DEVICE_FLAGS_ROTATIONAL_TRACKING)
            controllerFlags |= 1;

        if (device_flags & OHMD_DEVICE_FLAGS_POSITIONAL_TRACKING)
            controllerFlags |= 2;

        #if defined OHMD_DEVICE_FLAGS_HAPTIC_FEEDBACK
        if (device_flags & OHMD_DEVICE_FLAGS_HAPTIC_FEEDBACK)
            controllerFlags |= 4;
        #endif
    }

    // Return bitmask of connected and associated controllers:
    PsychCopyOutDoubleArg(5, kPsychArgOptional, controllerTypes);

    // Return bitmask of associated controllers tracking flags:
    PsychCopyOutDoubleArg(6, kPsychArgOptional, controllerFlags);

    return(PsychError_none);
}

PsychError PSYCHOPENHMDVRClose(void)
{
    static char useString[] = "PsychOpenHMDVRCore('Close' [, openhmdPtr]);";
    //                                                      1
    static char synopsisString[] =
        "Close connection to OpenHMD device 'openhmdPtr'. Do nothing if no such device is open.\n"
        "If the optional 'openhmdPtr' is omitted, then close all open devices and shutdown the driver, "
        "ie. perform the same cleanup as if 'clear PsychOpenHMDVRCore' would be executed.\n";
    static char seeAlsoString[] = "Open";

    int handle = -1;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    // Get optional device handle:
    PsychCopyInIntegerArg(1, kPsychArgOptional, &handle);

    if (handle >= 1) {
        // Close device:
        PsychOpenHMDClose(handle);
    }
    else {
        // No handle provided: Close all devices, shutdown driver.
        PsychOpenHMDVRShutDown();
    }

    return(PsychError_none);
}

PsychError PSYCHOPENHMDVRSetDynamicPrediction(void)
{
    static char useString[] = "oldDynamicPrediction = PsychOpenHMDVRCore('SetDynamicPrediction', openhmdPtr [, dynamicPrediction]);";
    //                                                                                           1             2
    static char synopsisString[] =
        "Enable or disable dynamic prediction mode on OpenHMD device 'openhmdPtr'.\n"
        "DOES NOTHING AT THE MOMENT!\n"
        "'dynamicPrediction' 1 = Enable dynamic prediction, 0 = Disable dynamic prediction.\n"
        "Returns previous 'dynamic prediction ' setting.\n";
    static char seeAlsoString[] = "";

    int handle, dynamicPrediction;
    PsychOpenHMDDevice *openhmd;
    unsigned int oldCaps;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openhmd = PsychGetOpenHMD(handle, FALSE);

    // Query current enabled caps:
    oldCaps = 0;
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) oldCaps);

    // Set new enabled HMD caps:
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &dynamicPrediction)) {
        // Does nothing so far.
    }

    return(PsychError_none);
}

PsychError PSYCHOPENHMDVRSetLowPersistence(void)
{
    static char useString[] = "oldPersistence = PsychOpenHMDVRCore('SetLowPersistence', openhmdPtr [, lowPersistence]);";
    //                                                                                  1             2
    static char synopsisString[] =
        "Enable or disable low persistence mode on display panel of OpenHMD device 'openhmdPtr'.\n"
        "DOES NOTHING AT THE MOMENT!\n"
        "'lowPersistence' 1 = Enable low persistence, 0 = Disable low persistence.\n"
        "In low persistence mode, the pixels will only light up for a fraction of a video "
        "refresh duration, thereby reducing motion blur due to smooth pursuit eye movements "
        "or other shuttering effects. Brightness of the display will be reduced though, and "
        "flicker sensitive people may perceive more flicker.\n"
        "Returns previous 'oldPersistence' setting.\n";
    static char seeAlsoString[] = "";

    int handle, lowPersistence;
    PsychOpenHMDDevice *openhmd;
    unsigned int oldCaps;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openhmd = PsychGetOpenHMD(handle, FALSE);

    // Query current enabled caps:
    oldCaps = 0;
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) oldCaps);

    // Set new enabled HMD caps:
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &lowPersistence)) {
        // Does nothing so far.
    }

    return(PsychError_none);
}

PsychError PSYCHOPENHMDVRStart(void)
{
    static char useString[] = "PsychOpenHMDVRCore('Start', openhmdPtr);";
    //                                                     1
    static char synopsisString[] =
        "Start head orientation and position tracking operation on OpenHMD device 'openhmdPtr'.\n\n";
    static char seeAlsoString[] = "Stop";

    int handle;
    PsychOpenHMDDevice *openhmd;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openhmd = PsychGetOpenHMD(handle, FALSE);

    if (openhmd->isTracking) {
        if (verbosity >= 0) printf("PsychOpenHMDVRCore-ERROR: Tried to start tracking on device %i, but tracking is already started.\n", handle);
        PsychErrorExitMsg(PsychError_user, "Tried to start tracking on HMD, but tracking already active.");
    }

    if (verbosity >= 4) printf("PsychOpenHMDVRCore-INFO: Tracking started on device with handle %i.\n", handle);

    openhmd->frameIndex = 0;
    openhmd->isTracking = TRUE;

    // Tracking is running.
    return(PsychError_none);
}

PsychError PSYCHOPENHMDVRStop(void)
{
    static char useString[] = "PsychOpenHMDVRCore('Stop', openhmdPtr);";
    static char synopsisString[] =
        "Stop head tracking operation on OpenHMD device 'openhmdPtr'.\n\n";
    static char seeAlsoString[] = "Start";

    int handle;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);

    // Stop device:
    PsychOpenHMDStop(handle);

    return(PsychError_none);
}

PsychError PSYCHOPENHMDVRGetTrackingState(void)
{
    static char useString[] = "[state, touch] = PsychOpenHMDVRCore('GetTrackingState', openhmdPtr [, predictionTime=0]);";
    //                         1       2                                               1             2
    static char synopsisString[] =
        "Return current state of head position and orientation tracking for OpenHMD device 'openhmdPtr'.\n"
        "Head position and orientation is predicted for target time 'predictionTime' in seconds if provided, "
        "based on the latest measurements from the tracking hardware. If 'predictionTime' is omitted or set "
        "to zero, then no prediction is performed and the current state based on latest measurements is returned. "
        "NOTE: Some fields may not get updated, depending on the type of HMD device and version of the libopenhmd library. "
        "NOTE: predictionTime is not yet implemented and silently ignored.\n\n"
        "'state' is a struct with fields reporting the following values:\n"
        "'Time' = Time in seconds of predicted tracking state.\n"
        "'Status' = Tracking status flags. +1 = Head orientation tracked, +2 = Head position tracked, +4 = Camera pose tracked "
        "+32 = Position tracking hardware connected, +128 = HMD display is connected and available.\n"
        "'HeadPose' = Head position [x, y, z] in meters and rotation as quaternion [rx, ry, rz, rw], all as a vector [x,y,z,rx,ry,rz,rw].\n"
        "'HeadLinearSpeed' = Linear velocity [vx,vy,vz] in meters/sec.\n"
        "'HeadAngularSpeed' = Angular velocity [rx,ry,rz] in radians/sec.\n"
        "'HeadLinearAcceleration' = Linear acceleration [ax,ay,az] in meters/sec^2.\n"
        "'HeadAngularAcceleration' = Angular acceleration [rax,ray,raz] in radians/sec^2.\n"
//         "'CameraPose' as vector with position and orientation quaternion, like 'HeadPose'.\n"
//         "'LeveledCameraPose' Like 'CameraPose' but aligned to the gravity vector of the world.\n"
//         "'CameraFrustumHVFov' Horizontal and vertical field of view of the tracking camera in radians.\n"
//         "'CameraFrustumNearFarZInMeters' Near and far limit of the camera view frustum in meters.\n"
//         "'LastCameraFrameCounter' Last camera framecounter value of tracking camera.\n"
//         "'RawSensorAcceleration' = Raw measured accelerometer reading in m/sec^2.\n"
//         "'RawSensorGyroRate' = Raw gyrometer reading in rad/s.\n"
//         "'RawMagnetometer' = Raw magnetic field in gauss.\n"
//         "'SensorTemperature' = Sensor temperature in degrees Celsius.\n"
//         "'IMUReadoutTime' = Readout time of the last IMU sample in seconds.\n"
        "\n"
        "Touch controller position and orientation:\n\n"
        "The return argument 'touch' is a struct array with 2 structs. touch(1) contains info about "
        "the tracking state and tracked pose of the left hand (= left touch controller) of the user, "
        "touch(2) contains info about the right hand (= right touch controller) of the user.\n"
        "The structs have very similar structure to the head (= HMD) tracking data returned by 'state':\n\n"
        "'Time' = Time in seconds of returned hand/controller tracking state.\n"
        "'Status' = Tracking status flags:\n"
        "0  = No tracking info for hand/controller, ie. no touch sensor connected.\n"
        "+1 = Hand orientation tracked,\n"
        "+2 = Hand position tracked,\n"
        "'HandPose' = Position and orientation of the hand, in usual [x,y,z,rx,ry,rz,rw] vector form as with 'HeadPose'.\n"
        "'HandLinearSpeed' = Hand linear velocity [vx,vy,vz] in meters/sec.\n"
        "'HandAngularSpeed' = Hand angular velocity [rx,ry,rz] in radians/sec.\n"
        "'HandLinearAcceleration' = Hand linear acceleration [ax,ay,az] in meters/sec^2.\n"
        "'HandAngularAcceleration' = Hand angular acceleration [rax,ray,raz] in radians/sec^2.\n"
        "\n";

    static char seeAlsoString[] = "Start Stop GetInputState";

    PsychGenericScriptType *status;
    const char *FieldNames[] = {"Time", "Status", "HeadPose", "HeadLinearSpeed", "HeadAngularSpeed", "HeadLinearAcceleration", "HeadAngularAcceleration",
                                "CameraPose", "LeveledCameraPose", "LastCameraFrameCounter", "RawSensorAcceleration", "RawSensorGyroRate", "RawMagnetometer",
                                "SensorTemperature", "IMUReadoutTime", "CameraFrustumHVFov", "CameraFrustumNearFarZInMeters"};
    const int FieldCount = 17;

    const char *FieldNames2[] = {"Time", "Status", "HandPose", "HandLinearSpeed", "HandAngularSpeed", "HandLinearAcceleration", "HandAngularAcceleration"};
    const int FieldCount2 = 7;

    PsychGenericScriptType *outMat;
    double *v;
    int i;
    int handle, hmdstatus = 0;
    double tNow, predictionTime = 0.0;
    PsychOpenHMDDevice *openhmd;
    float state[19] = { 0 };

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openhmd = PsychGetOpenHMD(handle, FALSE);

    PsychCopyInDoubleArg(2, kPsychArgOptional, &predictionTime);

    // Trigger update of device driver-private state into queryable device state:
    ohmd_ctx_update(ctx);
    PsychGetAdjustedPrecisionTimerSeconds(&tNow);

    // Get current tracked position and orientation measurements:
    ohmd_device_getf(openhmd->hmd, OHMD_POSITION_VECTOR, &state[0]);
    ohmd_device_getf(openhmd->hmd, OHMD_ROTATION_QUAT, &state[3]);

    // Print out tracking status:
    if (verbosity >= 4) {
        printf("PsychOpenHMDVRCore-INFO: Tracking state predicted for device %i at time %f.\n", handle, tNow);
        printf("PsychOpenHMDVRCore-INFO: HeadPose: Position    [x,y,z]   = [%f, %f, %f]\n", state[0], state[1], state[2]);
        printf("PsychOpenHMDVRCore-INFO: HeadPose: Orientation [x,y,z,w] = [%f, %f, %f, %f]\n", state[3], state[4], state[5], state[6]);
    }

    PsychAllocOutStructArray(1, kPsychArgOptional, -1, FieldCount, FieldNames, &status);

    PsychSetStructArrayDoubleElement("Time", 0, tNow, status);

    // Hard-code status to + 128 for "HMD connected and online":
    hmdstatus = 128;

    // Set +1 "orientation tracked" if the Quaternion is not a (0,0,0,0) or (0,0,0,1) quaternion,
    // which would hint to use of the dummy device:
    if (!(state[3] == 0 && state[4] == 0 && state[5] == 0 && (state[6] == 0 || state[6] == 1)))
        hmdstatus += 1;

    // Set +2 "position tracked" if the position vector is different in this cycle from the one in the last cycle.
    // Position vectors start at (0,0,0) and then update if tracked, and freeze on their current value if tracking
    // is lost:
    if (state[0] != openhmd->oldpos[0] || state[1] != openhmd->oldpos[1] || state[2] != openhmd->oldpos[2])
        hmdstatus += 2;

    openhmd->oldpos[0] = state[0];
    openhmd->oldpos[1] = state[1];
    openhmd->oldpos[2] = state[2];

    // Set +32 "position tracking hardware connected" if the old position vector is not (0,0,0), as with a
    // device that has not ever been tracked in this session:
    if (!(openhmd->oldpos[0] == 0 && openhmd->oldpos[1] == 0 && openhmd->oldpos[2] == 0))
        hmdstatus += 32;

    PsychSetStructArrayDoubleElement("Status", 0, hmdstatus, status);

    // Head pose:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);
    v[0] = state[0];
    v[1] = state[1];
    v[2] = state[2];

    v[3] = state[3];
    v[4] = state[4];
    v[5] = state[5];
    v[6] = state[6];
    PsychSetStructArrayNativeElement("HeadPose", 0, outMat, status);

    // Velocity/Acceleration queries need OpenHMD version of at least ?.?.? - UNKNOWN.
    // TODO: Current OpenHMD does not support these, and even the experimental tracking
    // branch only supports them nominally for the Rift CV-1 - where it also does
    // not work well yet. On other devices it will flood the console with error
    // messages, so not good!
    if (OHMD_S_OK == ohmd_require_version(1000, 0, 0)) {
        #ifdef OHMD_HAVE_VEL_ACCEL_API_v0
        // Linear velocity:
        if (OHMD_S_OK == ohmd_device_getf(openhmd->hmd, OHMD_VELOCITY_VECTOR, &state[7])) {
            v = NULL;
            PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
            v[0] = state[7];
            v[1] = state[8];
            v[2] = state[9];
            PsychSetStructArrayNativeElement("HeadLinearSpeed", 0, outMat, status);
        }

        // Linear acceleration:
        if (OHMD_S_OK == ohmd_device_getf(openhmd->hmd, OHMD_ACCELERATION_VECTOR, &state[10])) {
            v = NULL;
            PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
            v[0] = state[10];
            v[1] = state[11];
            v[2] = state[12];
            PsychSetStructArrayNativeElement("HeadLinearAcceleration", 0, outMat, status);
        }
        #endif

        #ifdef OHMD_HAVE_VEL_ACCEL_API_v1
        // Angular velocity:
        if (OHMD_S_OK == ohmd_device_getf(openhmd->hmd, OHMD_ANGULAR_VELOCITY_VECTOR, &state[13])) {
            v = NULL;
            PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
            v[0] = state[13];
            v[1] = state[14];
            v[2] = state[15];
            PsychSetStructArrayNativeElement("HeadAngularSpeed", 0, outMat, status);
        }

        // Angular acceleration:
        if (OHMD_S_OK == ohmd_device_getf(openhmd->hmd, OHMD_ANGULAR_ACCELERATION_VECTOR, &state[16])) {
            v = NULL;
            PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
            v[0] = state[16];
            v[1] = state[17];
            v[2] = state[18];
            PsychSetStructArrayNativeElement("HeadAngularAcceleration", 0, outMat, status);
        }
        #endif
    }

    // Now the tracking info from the hand/touch controllers 0 and 1 for left
    // and right hand, in a separate struct array:
    PsychAllocOutStructArray(2, kPsychArgOptional, 2, FieldCount2, FieldNames2, &status);

    for (i = 0; i < 2; i++) {
        // Timestamp for when this tracking info is valid:
        PsychSetStructArrayDoubleElement("Time", i, tNow, status);

        // Get current tracked position and orientation measurements:
        if (openhmd->controller[i]) {
            ohmd_device_getf(openhmd->controller[i], OHMD_POSITION_VECTOR, &state[0]);
            ohmd_device_getf(openhmd->controller[i], OHMD_ROTATION_QUAT, &state[3]);
        }
        else {
            memset(&state[0], 0, sizeof(state));
        }

        // Reset hand-tracked state:
        hmdstatus = 0;

        // Set +1 "orientation tracked" if the Quaternion is not a (0,0,0,0) or (0,0,0,1) quaternion,
        // which would hint to use of the dummy device:
        if (!(state[3] == 0 && state[4] == 0 && state[5] == 0 && (state[6] == 0 || state[6] == 1)))
            hmdstatus += 1;

        // Set +2 "position tracked" if the position vector is different in this cycle from the one in the last cycle.
        // Position vectors start at (0,0,0) and then update if tracked, and freeze on their current value if tracking
        // is lost:
        if (state[0] != openhmd->oldhandpos[i][0] || state[1] != openhmd->oldhandpos[i][1] || state[2] != openhmd->oldhandpos[i][2])
            hmdstatus += 2;

        openhmd->oldhandpos[i][0] = state[0];
        openhmd->oldhandpos[i][1] = state[1];
        openhmd->oldhandpos[i][2] = state[2];

        // Hand / touch controller tracking state:
        PsychSetStructArrayDoubleElement("Status", i, hmdstatus, status);

        // Hand pose:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);
        v[0] = state[0];
        v[1] = state[1];
        v[2] = state[2];

        v[3] = state[3];
        v[4] = state[4];
        v[5] = state[5];
        v[6] = state[6];
        PsychSetStructArrayNativeElement("HandPose", i, outMat, status);

        if (OHMD_S_OK == ohmd_require_version(1000, 0, 0)) {
            #ifdef OHMD_HAVE_VEL_ACCEL_API_v0
            // Linear velocity:
            if ((openhmd->controller[i]) && (OHMD_S_OK == ohmd_device_getf(openhmd->controller[i], OHMD_VELOCITY_VECTOR, &state[7]))) {
                v = NULL;
                PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
                v[0] = state[7];
                v[1] = state[8];
                v[2] = state[9];
                PsychSetStructArrayNativeElement("HandLinearSpeed", i, outMat, status);
            }

            // Linear acceleration:
            if ((openhmd->controller[i]) && (OHMD_S_OK == ohmd_device_getf(openhmd->controller[i], OHMD_ACCELERATION_VECTOR, &state[10]))) {
                v = NULL;
                PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
                v[0] = state[10];
                v[1] = state[11];
                v[2] = state[12];
                PsychSetStructArrayNativeElement("HandLinearAcceleration", i, outMat, status);
            }
            #endif

            #ifdef OHMD_HAVE_VEL_ACCEL_API_v1
            // Angular velocity:
            if ((openhmd->controller[i]) && (OHMD_S_OK == ohmd_device_getf(openhmd->controller[i], OHMD_ANGULAR_VELOCITY_VECTOR, &state[13]))) {
                v = NULL;
                PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
                v[0] = state[13];
                v[1] = state[14];
                v[2] = state[15];
                PsychSetStructArrayNativeElement("HandAngularSpeed", i, outMat, status);
            }

            // Angular acceleration:
            if ((openhmd->controller[i]) && (OHMD_S_OK == ohmd_device_getf(openhmd->controller[i], OHMD_ANGULAR_ACCELERATION_VECTOR, &state[16]))) {
                v = NULL;
                PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
                v[0] = state[16];
                v[1] = state[17];
                v[2] = state[18];
                PsychSetStructArrayNativeElement("HandAngularAcceleration", i, outMat, status);
            }
            #endif
        }
    }

    /* TODO - Probably never:
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

    // Camera frustum HFov and VFov in radians:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = openhmd->hmd->CameraFrustumHFovInRadians;
    v[1] = openhmd->hmd->CameraFrustumVFovInRadians;
    PsychSetStructArrayNativeElement("CameraFrustumHVFov", 0, outMat, status);

    // Camera frustum near and far clip plane in meters:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = openhmd->hmd->CameraFrustumNearZInMeters;
    v[1] = openhmd->hmd->CameraFrustumFarZInMeters;
    PsychSetStructArrayNativeElement("CameraFrustumNearFarZInMeters", 0, outMat, status);
    */

    return(PsychError_none);
}

PsychError PSYCHOPENHMDVRGetFovTextureSize(void)
{
    static char useString[] = "[width, height, fovPort] = PsychOpenHMDVRCore('GetFovTextureSize', openhmdPtr, eye, metersPerTanAngleAtCenter [, fov=[HMDRecommended]]);";
    //                          1      2       3                                                  1           2    3                                 4
    static char synopsisString[] =
    "Return recommended size of client renderbuffers for OpenHMD device 'openhmdPtr'.\n"
    "'eye' which eye to provide the size for: 0 = Left, 1 = Right.\n"
    "'metersPerTanAngleAtCenter' Meters per radians tan angle at the optical center of the display. "
    "Multiply by a factor > 1 to improve quality at lower performance, < 1 to reduce quality at higher performance.\n"
    "'fov' Optional field of view in degrees, from line of sight: [leftdeg, rightdeg, updeg, downdeg]. "
    "If 'fov' is omitted, the OpenHMD runtime will be asked for a good default field of view and that "
    "will be used.\n"
    "Return values are 'width' for minimum recommended width of framebuffer in pixels and "
    "'height' for minimum recommended height of framebuffer in pixels. 'fovPort' is the field of view "
    "in degrees finally used for calculation of 'width' x 'height'.\n";
    static char seeAlsoString[] = "GetUndistortionParameters";

    int handle, eyeIndex;
    int n, m, p;
    PsychOpenHMDDevice *openhmd;
    double *fov;
    double metersPerTanAngleAtCenter;
    double *outFov;
    float factor;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(4));
    PsychErrorExit(PsychRequireNumInputArgs(3));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openhmd = PsychGetOpenHMD(handle, FALSE);

    // Get eye index - left = 0, right = 1:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    // Get required 'metersPerTanAngleAtCenter' parameter:
    PsychCopyInDoubleArg(3, kPsychArgRequired, &metersPerTanAngleAtCenter);
    if (metersPerTanAngleAtCenter <= 0.0) PsychErrorExitMsg(PsychError_user, "Invalid 'metersPerTanAngleAtCenter' specified. Must be greater than zero.");

    // Get optional field of view in degrees in left,right,up,down direction from line of sight:
    if (PsychAllocInDoubleMatArg(4, kPsychArgOptional, &n, &m, &p, &fov)) {
        // Validate and assign:
        if (n * m * p != 4) PsychErrorExitMsg(PsychError_user, "Invalid 'fov' specified. Must be a 4-component vector of form [leftdeg, rightdeg, updeg, downdeg].");
        openhmd->ofov[eyeIndex][0] = deg2rad(fov[0]);
        openhmd->ofov[eyeIndex][1] = deg2rad(fov[1]);
        openhmd->ofov[eyeIndex][2] = deg2rad(fov[2]);
        openhmd->ofov[eyeIndex][3] = deg2rad(fov[3]);
    }
    else {
        // None specified: Ask the runtime for good defaults.
        float f, a;
        ohmd_device_getf(openhmd->hmd, (eyeIndex == 0) ? OHMD_LEFT_EYE_FOV : OHMD_RIGHT_EYE_FOV, &f);
        ohmd_device_getf(openhmd->hmd, (eyeIndex == 0) ? OHMD_LEFT_EYE_ASPECT_RATIO : OHMD_RIGHT_EYE_ASPECT_RATIO, &a);
        openhmd->ofov[eyeIndex][0] = f * a / 2.0;
        openhmd->ofov[eyeIndex][1] = f * a / 2.0;
        openhmd->ofov[eyeIndex][2] = f / 2.0;
        openhmd->ofov[eyeIndex][3] = f / 2.0;
    }

    // Ask the api for optimal texture size, aka the size of the client draw buffer:
    // Original PsychOculusVRCore driver would have called ovrHmd_GetFovTextureSize(openhmd->hmd, (ovrEyeType) eyeIndex, openhmd->ofov[eyeIndex], (float) pixelsPerDisplay);
    //
    // This approximates that approach. However, as input value metersPerTanAngleAtCenter we need a caller provided value
    // which is the distance (in meters) covered by 1 radians of visual angle at the center of the screen (meters per tan-angle at center)
    // multiplied by a pixelsPerDisplay scaling factor for quality vs. performance tradeoff:
    ohmd_device_getf(openhmd->hmd, OHMD_SCREEN_HORIZONTAL_SIZE, &factor);
    factor = ((float) openhmd->panelSize[0]) / factor;
    openhmd->texSize[eyeIndex][0] = (int) (0.5 + metersPerTanAngleAtCenter * (openhmd->ofov[eyeIndex][0] + openhmd->ofov[eyeIndex][1]) * factor);
    ohmd_device_getf(openhmd->hmd, OHMD_SCREEN_VERTICAL_SIZE, &factor);
    factor = ((float) openhmd->panelSize[1]) / factor;
    openhmd->texSize[eyeIndex][1] = (int) (0.5 + metersPerTanAngleAtCenter * (openhmd->ofov[eyeIndex][2] + openhmd->ofov[eyeIndex][3]) * factor);

    // Return recommended width and height of drawBuffer:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, openhmd->texSize[eyeIndex][0]);
    PsychCopyOutDoubleArg(2, kPsychArgOptional, openhmd->texSize[eyeIndex][1]);

    PsychAllocOutDoubleMatArg(3, kPsychArgOptional, 4, 1, 1, &outFov);
    outFov[0] = rad2deg(openhmd->ofov[eyeIndex][0]);
    outFov[1] = rad2deg(openhmd->ofov[eyeIndex][1]);
    outFov[2] = rad2deg(openhmd->ofov[eyeIndex][2]);
    outFov[3] = rad2deg(openhmd->ofov[eyeIndex][3]);

    return(PsychError_none);
}

PsychError PSYCHOPENHMDVRGetUndistortionParameters(void)
{
    static char useString[] = "[width, height, hmdShiftx, hmdShifty, hmdShiftz, abberation, distortion, scrnHorSize, scrnVertSize] = PsychOpenHMDVRCore('GetUndistortionParameters', openhmdPtr, eye [, inputWidth][, inputHeight][, fov]);";
    //                          1      2       3          4          5          6           7           8            9                                                               1           2      3             4              5
    static char synopsisString[] =
    "Return parameters needed for rendering and undistortion for OpenHMD device 'openhmdPtr'.\n"
    "'eye' which eye to provide the data: 0 = Left, 1 = Right.\n"
    "'inputWidth' = Width of the rendered input image buffer in pixels.\n"
    "'inputHeight' = Height of the rendered input image buffer in pixels.\n"
    "'fov' Optional field of view in degrees, from line of sight: [leftdeg, rightdeg, updeg, downdeg]. "
    "You can pass in the 'fovPort' value returned from PsychOpenHMDVR('GetFovTextureSize'); "
    "Defaults to whatever has been set for the given eye in the last call to PsychOpenHMDVR('GetFovTextureSize'); if omitted.\n"
    "\n"
    "Return values:\n"
    "[width, height] = Width and height of client renderbuffers in pixels. Same as the provided 'inputWidth' and 'inputHeight'.\n"
    "[hmdShiftx, hmdShifty, hmdShiftz] = HmdToEyeViewOffset 3D translation vector. Defines the location of the optical center of the eye "
    "relative to the origin of the local head reference frame, ie. the tracked head position.\n"
    "abberation = 3-component vector with optical lens color abberation correction coefficients.\n"
    "distortion = 4-component vector with optical lens geometric distortion correction coefficients.\n"
    "scrnHorSize = Horizontal screen size in meters.\n"
    "scrnVertSize = Vertical screen size in meters.\n"
    "\n";
    static char seeAlsoString[] = "GetFovTextureSize";

    int handle, eyeIndex;
    PsychOpenHMDDevice *openhmd;
    int n, m, p, i;
    double *fov;
    double *out;
    float sep_h, sep_v, ssize;
    float distortion_coeffs[4];
    float aberr_scale[3];

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(9));
    PsychErrorExit(PsychCapNumInputArgs(5));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openhmd = PsychGetOpenHMD(handle, FALSE);

    // Get eye index - left = 0, right = 1:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    // Get input texture width:
    if (PsychCopyInIntegerArg(3, kPsychArgOptional, &(openhmd->texSize[eyeIndex][0]))) {
        if (openhmd->texSize[eyeIndex][0] < 1) PsychErrorExitMsg(PsychError_user, "Invalid 'inputWidth' specified. Must be greater than zero.");
    }

    // Get input texture height:
    if (PsychCopyInIntegerArg(4, kPsychArgOptional, &(openhmd->texSize[eyeIndex][1]))) {
        if (openhmd->texSize[eyeIndex][1] < 1) PsychErrorExitMsg(PsychError_user, "Invalid 'inputHeight' specified. Must be greater than zero.");
    }

    // Get optional field of view in degrees in left,right,up,down direction from line of sight:
    if (PsychAllocInDoubleMatArg(5, kPsychArgOptional, &n, &m, &p, &fov)) {
        // Validate and assign:
        if (n * m * p != 4) PsychErrorExitMsg(PsychError_user, "Invalid 'fov' specified. Must be a 4-component vector of form [leftdeg, rightdeg, updeg, downdeg].");
        openhmd->ofov[eyeIndex][0] = deg2rad(fov[0]);
        openhmd->ofov[eyeIndex][1] = deg2rad(fov[1]);
        openhmd->ofov[eyeIndex][2] = deg2rad(fov[2]);
        openhmd->ofov[eyeIndex][3] = deg2rad(fov[3]);
    }

    // Return width and height of input texture - Just mirror out what we got:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, openhmd->texSize[eyeIndex][0]);
    PsychCopyOutDoubleArg(2, kPsychArgOptional, openhmd->texSize[eyeIndex][1]);

    // HmdToEyeViewOffset: [x,y,z]:
    ohmd_device_getf(openhmd->hmd, OHMD_LENS_HORIZONTAL_SEPARATION, &sep_h);
    ohmd_device_getf(openhmd->hmd, OHMD_LENS_VERTICAL_POSITION, &sep_v);

    PsychCopyOutDoubleArg(3, kPsychArgOptional, (double) ((eyeIndex == 0) ? -sep_h / 2 : +sep_h / 2));
    PsychCopyOutDoubleArg(4, kPsychArgOptional, (double) sep_v);
    PsychCopyOutDoubleArg(5, kPsychArgOptional, 0.0);

    ohmd_device_getf(openhmd->hmd, OHMD_UNIVERSAL_ABERRATION_K, &(aberr_scale[0]));
    PsychAllocOutDoubleMatArg(6, kPsychArgOptional, 1, 3, 1, &out);
    out[0] = aberr_scale[0];
    out[1] = aberr_scale[1];
    out[2] = aberr_scale[2];

    ohmd_device_getf(openhmd->hmd, OHMD_UNIVERSAL_DISTORTION_K, &(distortion_coeffs[0]));
    PsychAllocOutDoubleMatArg(7, kPsychArgOptional, 1, 4, 1, &out);
    out[0] = distortion_coeffs[0];
    out[1] = distortion_coeffs[1];
    out[2] = distortion_coeffs[2];
    out[3] = distortion_coeffs[3];

    ohmd_device_getf(openhmd->hmd, OHMD_SCREEN_HORIZONTAL_SIZE, &ssize);
    PsychCopyOutDoubleArg(8, kPsychArgOptional, ssize);

    ohmd_device_getf(openhmd->hmd, OHMD_SCREEN_VERTICAL_SIZE, &ssize);
    PsychCopyOutDoubleArg(9, kPsychArgOptional, ssize);

    return(PsychError_none);
}

PsychError PSYCHOPENHMDVRGetCorrectionShaders(void)
{
    static char useString[] = "[vertexShaderSrc, fragmentShaderSrc] = PsychOpenHMDVRCore('GetCorrectionShaders', openhmdPtr);";
    //                          1                2                                                               1
    static char synopsisString[] =
    "Return source code for GLSL shaders for VR warp pass for OpenHMD device 'openhmdPtr'.\n"
    "'vertexShaderSrc' The source code of the vertex shader to use.\n";
    "'fragmentShaderSrc' The source code of the fragment shader to use.\n";
    static char seeAlsoString[] = "";

    int handle;
    const char* vertex;
    const char* fragment;
    PsychOpenHMDDevice *openhmd;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(1));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openhmd = PsychGetOpenHMD(handle, FALSE);

    ohmd_gets(OHMD_GLSL_DISTORTION_VERT_SRC, &vertex);
    ohmd_gets(OHMD_GLSL_DISTORTION_FRAG_SRC, &fragment);

    PsychCopyOutCharArg(1, kPsychArgOptional, vertex);
    PsychCopyOutCharArg(2, kPsychArgOptional, fragment);

    return(PsychError_none);
}

PsychError PSYCHOPENHMDVRGetStaticRenderParameters(void)
{
    static char useString[] = "[projL, projR, ipd] = PsychOpenHMDVRCore('GetStaticRenderParameters', openhmdPtr [, clipNear=0.01][, clipFar=10000.0]);";
    //                          1      2      3                                                      1             2                3
    static char synopsisString[] =
    "Retrieve static rendering parameters for OpenHMD device 'openhmdPtr' at current settings.\n"
    "'clipNear' Optional near clipping plane for OpenGL. Defaults to 0.01.\n"
    "'clipFar' Optional far clipping plane for OpenGL. Defaults to 10000.0.\n"
    "\nReturn arguments:\n\n"
    "'projL' is the 4x4 OpenGL projection matrix for the left eye rendering.\n"
    "'projR' is the 4x4 OpenGL projection matrix for the right eye rendering.\n"
    "'ipd' is estimated inter-pupillar distance.\n"
    "Please note that projL and projR are usually identical for typical rendering scenarios.\n";
    static char seeAlsoString[] = "";

    int handle;
    PsychOpenHMDDevice *openhmd;
    float M[4][4];
    int i, j;
    double clip_near, clip_far;
    float  f_clip_near, f_clip_far, ipd;
    double *outM;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openhmd = PsychGetOpenHMD(handle, FALSE);

    // Get optional near clipping plane:
    clip_near = 0.01;
    PsychCopyInDoubleArg(2, kPsychArgOptional, &clip_near);
    f_clip_near   = (float) clip_near;
    ohmd_device_setf(openhmd->hmd, OHMD_PROJECTION_ZNEAR, &f_clip_near);

    // Get optional far clipping plane:
    clip_far = 10000.0;
    PsychCopyInDoubleArg(3, kPsychArgOptional, &clip_far);
    f_clip_far = (float) clip_far;
    ohmd_device_setf(openhmd->hmd, OHMD_PROJECTION_ZFAR, &f_clip_far);

    // Return left projection matrix as return argument 1:
    ohmd_device_getf(openhmd->hmd, OHMD_LEFT_EYE_GL_PROJECTION_MATRIX, &M[0][0]);
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 4, 4, 1, &outM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(outM++) = (double) M[i][j];

    // Return right projection matrix as return argument 2:
    ohmd_device_getf(openhmd->hmd, OHMD_RIGHT_EYE_GL_PROJECTION_MATRIX, &M[0][0]);
    PsychAllocOutDoubleMatArg(2, kPsychArgOptional, 4, 4, 1, &outM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(outM++) = (double) M[i][j];

    // Inter-pupillar distance:
    ohmd_device_getf(openhmd->hmd, OHMD_EYE_IPD, &ipd);
    PsychCopyOutDoubleArg(3, kPsychArgOptional, ipd);

    return(PsychError_none);
}

PsychError PSYCHOPENHMDVRGetEyePose(void)
{
    static char useString[] = "[eyePose, eyeIndex, glModelviewMatrix] = PsychOpenHMDVRCore('GetEyePose', openhmdPtr, renderPass);";
    //                          1        2         3                                                     1           2
    static char synopsisString[] =
    "Return current predicted pose vector for an eye for OpenHMD device 'openhmdPtr'.\n"
    "NOTE: As of August 2017, this function does not provide optimized predicted pose and is here just "
    "for backwards compatibility, as OpenHMD does not provide such per-eye tracking info.\n\n"
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
    "accuracy in rendering, at the expense of more complex usercode.\n"
    "'glModelviewMatrix' OpenGL modelview matrix, as computed by OpenHMD from tracking data.\n";

    static char seeAlsoString[] = "";

    int handle, renderPass;
    PsychOpenHMDDevice *openhmd;
    float state[7];
    float M[4][4];
    int i, j;
    double *outM;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openhmd = PsychGetOpenHMD(handle, FALSE);

    // Get renderPass:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &renderPass);
    if (renderPass < 0 || renderPass > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'renderPass' specified. Must be 0 or 1 for first or second pass.");

    // Trigger update of device driver-private state into queryable device state:
    ohmd_ctx_update(ctx);

    // Get current tracked position and orientation measurements:
    ohmd_device_getf(openhmd->hmd, OHMD_POSITION_VECTOR, &state[0]);
    ohmd_device_getf(openhmd->hmd, OHMD_ROTATION_QUAT, &state[3]);

    // Eye pose as raw data:
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, 7, 1, &outM);
    outM[0] = state[0];
    outM[1] = state[1];
    outM[2] = state[2];

    outM[3] = state[3];
    outM[4] = state[4];
    outM[5] = state[5];
    outM[6] = state[6];

    // Copy out preferred eye render order for info:
    PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) renderPass);

    // Return modelview matrix as computed by OpenHMD:
    ohmd_device_getf(openhmd->hmd, (renderPass == 0) ? OHMD_LEFT_EYE_GL_MODELVIEW_MATRIX : OHMD_RIGHT_EYE_GL_MODELVIEW_MATRIX, &M[0][0]);
    PsychAllocOutDoubleMatArg(3, kPsychArgOptional, 4, 4, 1, &outM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(outM++) = (double) M[i][j];

    return(PsychError_none);
}

static double getControlValue(float state[256], int control_id[256], ohmd_control_hint hint)
{
    int i;

    for (i = 0; i < 256; i++)
        if (control_id[i] == hint)
            return(state[i]);

    return(0);
}

PsychError PSYCHOPENHMDVRGetInputState(void)
{
    static char useString[] = "input = PsychOpenHMDVRCore('GetInputState', openhmdPtr, controllerType);";
    //                         1                                           1           2
    static char synopsisString[] =
    "Return current state of input device 'controllerType' associated with OpenHMD device 'openhmdPtr'.\n\n"
    "'controllerType' can be a mask of the follwing values:\n"
    "OVR.ControllerType_LTouch = Left touch controller (Left tracked hand).\n"
    "OVR.ControllerType_RTouch = Right touch controller (Right tracked hand).\n"
    "OVR.ControllerType_Remote = Oculus remote control or other remote controls / HMD controls.\n"
    "OVR.ControllerType_XBox = XBox controller - Currently not supported.\n"
    "OVR.ControllerType_Active = Whatever controller is connected and active.\n"
    "\n"
    "'input' is a struct with fields reporting the following status values of the controller:\n"
    "'Valid' = 1 if 'input' contains valid results, 0 if input status is invalid/unavailable.\n"
    "'ActiveInputs' = Bitmask of which 'input' contains valid results, or 0 if input completely unavailable.\n"
    "The following flags will be logical or'ed together if the corresponding input category is valid, "
    "ie. provided with actual input date from some physical input source element, controller etc.:\n"
    "+1  = 'Buttons' gets input from some real buttons or switches.\n"
    "+2  = 'Touches' gets input from some real touch/proximity sensors or gesture recognizers.\n"
    "+4  = 'Trigger' gets input from some real analog trigger sensor or gesture recognizer.\n"
    "+8  = 'Grip' gets input from some real analog grip sensor or gesture recognizer.\n"
    "+16 = 'Thumbstick' gets input from some real thumbstick, joystick or trackpad or similar 2D sensor.\n"
    "'Time' = Time in seconds when controller state was last updated.\n"
    "'Buttons' = Vector with each positions value corresponding to a specifc button being pressed (1) "
    "or released (0). The OVR.Button_XXX constants map button names to vector indices (like KbName() "
    "does for KbCheck()).\n"
    "'Touches' = Vector with touch values as described by the OVR.Touch_XXX constants. Works like 'Buttons'.\n"
    "'Trigger'(1/2) = Left (1) and Right (2) trigger: Value range 0.0 - 1.0, filtered and with dead-zone.\n"
    "'TriggerNoDeadzone'(1/2) = Left (1) and Right (2) trigger: Value range 0.0 - 1.0, filtered.\n"
    "'TriggerRaw'(1/2) = Left (1) and Right (2) trigger: Value range 0.0 - 1.0, raw values unfiltered.\n"
    "'Grip'(1/2) = Left (1) and Right (2) grip button: Value range 0.0 - 1.0, filtered and with dead-zone.\n"
    "'GripNoDeadzone'(1/2) = Left (1) and Right (2) grip button: Value range 0.0 - 1.0, filtered.\n"
    "'GripRaw'(1/2) = Left (1) and Right (2) grip button: Value range 0.0 - 1.0, raw values unfiltered.\n"
    "'Thumbstick' = 2x2 matrix: Column 1 contains left thumbsticks [x;y] axis values, column 2 contains "
    "right sticks [x;y] axis values. Values are in range -1 to +1, filtered and with deadzone applied.\n"
    "'ThumbstickNoDeadzone' = Like 'Thumbstick', filtered, but without a deadzone applied.\n"
    "'ThumbstickRaw' = 'Thumbstick' raw date without deadzone or filtering applied.\n"
    "\n";

    static char seeAlsoString[] = "Start Stop GetTrackingState";

    PsychGenericScriptType *status;
    const char *FieldNames[] = { "Valid", "ActiveInputs", "Time", "Buttons", "Touches", "Trigger", "Grip", "TriggerNoDeadzone",
                                 "GripNoDeadzone", "TriggerRaw", "GripRaw", "Thumbstick",
                                 "ThumbstickNoDeadzone", "ThumbstickRaw" };
    const int FieldCount = 14;

    PsychGenericScriptType *outMat;
    double *v;
    int handle, i;
    int control_count;
    unsigned long controllerType;
    double controllerTypeD;
    double tNow;
    double trigger[2];
    double grip[2];
    double stick[2][2];
    PsychOpenHMDDevice *openhmd;
    int control_id[256] = { 0 };
    float state[256] = { 0 };
    int valid = 0;
    ohmd_device *dev = NULL;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    // Device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openhmd = PsychGetOpenHMD(handle, FALSE);

    // Controller type:
    PsychCopyInDoubleArg(2, kPsychArgRequired, &controllerTypeD);
    controllerType = (unsigned long) controllerTypeD;

    PsychAllocOutStructArray(1, kPsychArgOptional, 1, FieldCount, FieldNames, &status);

    // Mark as valid:
    PsychSetStructArrayDoubleElement("Valid", 0, 1, status);

    // Controller update time:
    PsychGetAdjustedPrecisionTimerSeconds(&tNow);
    PsychSetStructArrayDoubleElement("Time", 0, tNow, status);

    // Button states:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 32, 1, &v, &outMat);

    // Left touch, if any:
    dev = openhmd->controller[0];
    if (dev && (controllerType & 1) && !ohmd_device_geti(dev, OHMD_CONTROL_COUNT, &control_count) && (control_count > 0)) {
        // Get state of all floating point value controls:
        ohmd_device_getf(dev, OHMD_CONTROLS_STATE, state);
        ohmd_device_geti(dev, OHMD_CONTROLS_HINTS, control_id);

        v[8] += getControlValue(state, control_id, OHMD_BUTTON_X);
        v[9] += getControlValue(state, control_id, OHMD_BUTTON_Y);
        v[10] += getControlValue(state, control_id, OHMD_ANALOG_PRESS);
        v[20] += getControlValue(state, control_id, OHMD_MENU);

        // Left trigger, grip, stick:
        trigger[0] = getControlValue(state, control_id, OHMD_TRIGGER);

        // Left  trigger (digital): (Nolo)
        trigger[0] += getControlValue(state, control_id, OHMD_TRIGGER_CLICK);

        grip[0] = getControlValue(state, control_id, OHMD_SQUEEZE);
        stick[0][0] = getControlValue(state, control_id, OHMD_ANALOG_X);
        stick[0][1] = getControlValue(state, control_id, OHMD_ANALOG_Y);

        valid |= (1 | 4 | 8 | 16);
    }

    // Right touch, if any:
    dev = openhmd->controller[1];
    if (dev && (controllerType & 2) && !ohmd_device_geti(dev, OHMD_CONTROL_COUNT, &control_count) && (control_count > 0)) {
        // Get state of all floating point value controls:
        ohmd_device_getf(dev, OHMD_CONTROLS_STATE, state);
        ohmd_device_geti(dev, OHMD_CONTROLS_HINTS, control_id);

        v[0] += getControlValue(state, control_id, OHMD_BUTTON_A);
        v[1] += getControlValue(state, control_id, OHMD_BUTTON_B);
        v[2] += getControlValue(state, control_id, OHMD_ANALOG_PRESS);
        v[24] += getControlValue(state, control_id, OHMD_HOME);

        // Right trigger, grip, stick:
        trigger[1] = getControlValue(state, control_id, OHMD_TRIGGER);

        // Right trigger (digital): (Nolo)
        trigger[1] += getControlValue(state, control_id, OHMD_TRIGGER_CLICK);

        grip[1] = getControlValue(state, control_id, OHMD_SQUEEZE);
        stick[1][0] = getControlValue(state, control_id, OHMD_ANALOG_X);
        stick[1][1] = getControlValue(state, control_id, OHMD_ANALOG_Y);

        valid |= (1 | 4 | 8 | 16);
    }

    // HMD integrated inputs?
    dev = openhmd->hmd;

    // Oculus Remote, or some other HMD integrated controller/input?
    if (dev && openhmd->riftRemote && (controllerType & 4) && !ohmd_device_geti(dev, OHMD_CONTROL_COUNT, &control_count) && (control_count == 9)) {
        // Oculus Rift Remote:

        // Get state of all floating point value controls:
        ohmd_device_getf(dev, OHMD_CONTROLS_STATE, state);
        ohmd_device_geti(dev, OHMD_CONTROLS_HINTS, control_id);

        // Up, Down, Left, Right:
        v[16] += getControlValue(state, control_id, OHMD_BUTTON_Y);
        v[17] += getControlValue(state, control_id, OHMD_BUTTON_A);
        v[18] += getControlValue(state, control_id, OHMD_BUTTON_X);
        v[19] += getControlValue(state, control_id, OHMD_BUTTON_B);

        // OK / Enter:
        v[20] += getControlValue(state, control_id, OHMD_GENERIC);
        v[22] += getControlValue(state, control_id, OHMD_VOLUME_PLUS);
        v[23] += getControlValue(state, control_id, OHMD_VOLUME_MINUS);

        // Back button:
        v[21] += getControlValue(state, control_id, OHMD_HOME);

        // Oculus button:
        v[24] += getControlValue(state, control_id, OHMD_MENU);

        valid |= 1;
    }

    // HMD itself for a non-Oculus/non-Nolo device. Treat like a remote control:
    if (dev && !openhmd->riftRemote && (controllerType & 4) && !ohmd_device_geti(dev, OHMD_CONTROL_COUNT, &control_count) && (control_count > 0)) {
        // Some HMD integrated inputs, other than the Rift CV-1 remote control:
        // This covers most devices apart from Oculus and Nolo, as of OpenHMD 0.3
        // in the year 2021.

        // Get state of all floating point value controls:
        ohmd_device_getf(dev, OHMD_CONTROLS_STATE, state);
        ohmd_device_geti(dev, OHMD_CONTROLS_HINTS, control_id);

        // Generic buttons:
        v[0] += getControlValue(state, control_id, OHMD_BUTTON_A);
        v[1] += getControlValue(state, control_id, OHMD_BUTTON_B);
        v[8] += getControlValue(state, control_id, OHMD_BUTTON_X);
        v[9] += getControlValue(state, control_id, OHMD_BUTTON_Y);

        // OK / Enter:
        v[20] += getControlValue(state, control_id, OHMD_GENERIC);

        // PSVR volume and mic controls:
        v[22] += getControlValue(state, control_id, OHMD_VOLUME_PLUS);
        v[23] += getControlValue(state, control_id, OHMD_VOLUME_MINUS);

        // LShoulder:
        v[3] += getControlValue(state, control_id, OHMD_MIC_MUTE);

        // RShoulder:
        v[11] += getControlValue(state, control_id, OHMD_TRIGGER);

        // Back button:
        v[21] += getControlValue(state, control_id, OHMD_HOME);

        // Oculus button:
        v[24] += getControlValue(state, control_id, OHMD_MENU);

        valid |= 1;
    }

    // TODO: openhmd->controller[2] - OHMD_DEVICE_CLASS_GENERIC_TRACKER, but no
    // such devices are supported yet as of OpenHMD 0.3, so we don't know how to
    // handle them...

    // Assign computed button state:
    PsychSetStructArrayNativeElement("Buttons", 0, outMat, status);

    // Touch states: Do not have OpenHMD support for this yet.
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 32, 1, &v, &outMat);
    PsychSetStructArrayNativeElement("Touches", 0, outMat, status);

    // Trigger left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = trigger[0];
    v[1] = trigger[1];
    PsychSetStructArrayNativeElement("Trigger", 0, outMat, status);

    // Grip left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = grip[0];
    v[1] = grip[1];
    PsychSetStructArrayNativeElement("Grip", 0, outMat, status);

    // TriggerNoDeadzone left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = trigger[0];
    v[1] = trigger[1];
    PsychSetStructArrayNativeElement("TriggerNoDeadzone", 0, outMat, status);

    // GripNoDeadzone left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = grip[0];
    v[1] = grip[1];
    PsychSetStructArrayNativeElement("GripNoDeadzone", 0, outMat, status);

    // TriggerRaw left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = trigger[0];
    v[1] = trigger[1];
    PsychSetStructArrayNativeElement("TriggerRaw", 0, outMat, status);

    // GripRaw left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = grip[0];
    v[1] = grip[1];
    PsychSetStructArrayNativeElement("GripRaw", 0, outMat, status);

    // Thumbstick: column 1 = left hand, column 2 = right hand. row 1 = x, row 2= y:
    v = NULL;
    PsychAllocateNativeDoubleMat(2, 2, 1, &v, &outMat);
    v[0] = stick[0][0];
    v[1] = stick[0][1];
    v[2] = stick[1][0];
    v[3] = stick[1][1];
    PsychSetStructArrayNativeElement("Thumbstick", 0, outMat, status);

    // ThumbstickNoDeadzone:
    v = NULL;
    PsychAllocateNativeDoubleMat(2, 2, 1, &v, &outMat);
    v[0] = stick[0][0];
    v[1] = stick[0][1];
    v[2] = stick[1][0];
    v[3] = stick[1][1];
    PsychSetStructArrayNativeElement("ThumbstickNoDeadzone", 0, outMat, status);

    // ThumbstickRaw:
    v = NULL;
    PsychAllocateNativeDoubleMat(2, 2, 1, &v, &outMat);
    v[0] = stick[0][0];
    v[1] = stick[0][1];
    v[2] = stick[1][0];
    v[3] = stick[1][1];
    PsychSetStructArrayNativeElement("ThumbstickRaw", 0, outMat, status);

    // Return bitmask of valid inputs:
    PsychSetStructArrayDoubleElement("ActiveInputs", 0, valid, status);

    return(PsychError_none);
}

PsychError PSYCHOPENHMDVRHapticPulse(void)
{
    static char useString[] = "pulseEndTime = PsychOpenHMDVRCore('HapticPulse', openhmdPtr, controllerType [, duration=2.5][, freq=1.0][, amplitude=1.0]);";
    //                         1                                                1           2                 3               4           5
    static char synopsisString[] =
        "Execute a haptic feedback pulse on controller 'controllerType' associated with OpenHMD device 'openhmdPtr'.\n\n"
        "'duration' is the duration of the pulse in seconds. If omitted, the function returns immediately and "
        "the default duration of 2.5 seconds is used, unless you call the function again with 'freq' = 0, to cancel the "
        "currently active pulse. Otherwise, if a 'duration' other than 2.5 seconds is specified, the haptic pulse executes "
        "for the specified duration, and may block execution of your script for that time span on some controller types, "
        "returning the absolute time when the pulse is expected to end.\n\n"
        "'freq' Frequency of the vibration in normalized 0.0 - 1.0 range. 0 = Disable ongoing pulse immediately.\n\n"
        "'amplitude' Normalized amplitude in range 0.0 - 1.0\n\n"
        "The return argument 'pulseEndTime' contains the absolute time in seconds when the pulse is expected "
        "to end, as estimated at the time of calling the function. The precision and accuracy of pulse timing "
        "is not known.\n";
    static char seeAlsoString[] = "";

    int handle, controllerType;
    PsychOpenHMDDevice *openhmd;
    ohmd_device *dev;
    double duration, freq, amplitude, pulseEndTime;
    int result;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(5));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenHMDVRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openhmd = PsychGetOpenHMD(handle, FALSE);

    // Get the new performance HUD mode:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &controllerType);

    // Duration:
    duration = 2.5;
    PsychCopyInDoubleArg(3, kPsychArgOptional, &duration);
    if (duration < 0)
        PsychErrorExitMsg(PsychError_user, "Invalid negative 'duration' in seconds specified. Must be positive.");

    freq = 1.0;
    PsychCopyInDoubleArg(4, kPsychArgOptional, &freq);
    if (freq < 0.0 || freq > 1.0)
        PsychErrorExitMsg(PsychError_user, "Invalid 'freq' frequency specified. Must be in range [0.0 ; 1.0].");

    amplitude = 1.0;
    PsychCopyInDoubleArg(5, kPsychArgOptional, &amplitude);
    if (amplitude < 0.0 || amplitude > 1.0)
        PsychErrorExitMsg(PsychError_user, "Invalid 'amplitude' specified. Must be in range [0.0 ; 1.0].");

    switch (controllerType) {
        case 1: // Left touch.
            dev = openhmd->controller[0];
            break;

        case 2: // Right touch.
            dev = openhmd->controller[1];
            break;

        default:
            dev = NULL;
            if (verbosity > 1)
                printf("PsychOpenHMDVRCore-WARNING: 'HapticPulse' will go nowhere, as requested controller of type %i is not supported.\n", controllerType);
            break;
    }

    // Need to have compile-time support for ohmd_device_set_haptics_on/off() and
    // runtime support as well, which is not yet the case for current v0.3 stable:
#if defined(OHMD_HAVE_HAPTICS_API_v0) && 0
    if (dev) {
        if (freq != 0) {
            // Execute pulse: We map the freq range 0 - 1 to the interval 0 - 500 Hz for now. The Rift CV1
            // touch controllers only support 160 Hz and 320 Hz at the moment...
            result = ohmd_device_set_haptics_on(dev, duration, (float) freq * 500.0, (float) amplitude);
            if (result) {
                PsychErrorExitMsg(PsychError_user, "Failed to initiate haptic feedback pulse.");
            }

            if (verbosity > 3)
                printf("PsychOpenHMDVRCore-INFO: 'HapticPulse' of duration %f secs, freq %f, amplitude %f for controller of type %i started.\n",
                    duration, freq, amplitude, controllerType);
        } else {
            // Abort pulse:
            result = ohmd_device_set_haptics_off(dev);
            if (result) {
                PsychErrorExitMsg(PsychError_user, "Failed to stop haptic feedback pulse.");
            }
        }

        // Predict "off" time:
        PsychGetAdjustedPrecisionTimerSeconds(&pulseEndTime);
        pulseEndTime += duration;
    }
    else
#endif
        pulseEndTime = 0;

    PsychCopyOutDoubleArg(1, kPsychArgOptional, pulseEndTime);

    return(PsychError_none);
}
