/*
 * Psychtoolbox-3/PsychSourceGL/Source/Common/PsychOpenXRCore/PsychOpenXRCore.c
 *
 * PROJECTS: PsychOpenXRCore only.
 *
 * AUTHORS:
 *
 * mario.kleiner.de@gmail.com   mk
 *
 * PLATFORMS:   All which are supported by an OpenXR Runtime.
 *
 * DESCRIPTION:
 *
 * A Psychtoolbox driver for virtual reality (VR), augmented reality (AR) and
 * mixed reality (MR) devices, also known as XR devices, which are supported by
 * an OpenXR-1.0 (or later versions) compliant OpenXR runtime.
 * The runtime must support OpenGL as rendering backend for XR content.
 *
 * The driver was initially derived from PsychOculusVRCore1.
 * Copyright (c) 2022 Mario Kleiner. Licensed to you under the MIT license:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "PsychOpenXRCore.h"
#include "PsychTimeGlue.h"

// Need this _USE_MATH_DEFINES so MSVC knows M_PI:
#define _USE_MATH_DEFINES
#include <math.h>

// Define indices of mapping of standard PsychVRHMD() compatible OVR buttons/touch
// sensor constants / indices to XrAction buttonAction[] slots:
#define OVR_Button_A                0
#define OVR_Button_B                1
#define OVR_Button_RThumb           2
#define OVR_Button_RShoulder        3
#define OVR_Button_X                8
#define OVR_Button_Y                9
#define OVR_Button_LThumb           10
#define OVR_Button_LShoulder        11
#define OVR_Button_Up               16
#define OVR_Button_Down             17
#define OVR_Button_Left             18
#define OVR_Button_Right            19
#define OVR_Button_Enter            20
#define OVR_Button_Back             21
#define OVR_Button_VolUp            22
#define OVR_Button_VolDown          23
#define OVR_Button_Home             24
// PTB extension to OVR official defines:
#define OVR_Button_MicMute          25

#define NUM_OVR_BUTTONS             26

#define OVR_Touch_A                 0
#define OVR_Touch_B                 1
#define OVR_Touch_RThumb            2
#define OVR_Touch_RThumbRest        3
#define OVR_Touch_RIndexTrigger     4
#define OVR_Touch_X                 8
#define OVR_Touch_Y                 9
#define OVR_Touch_LThumb            10
#define OVR_Touch_LThumbRest        11
#define OVR_Touch_LIndexTrigger     12
#define OVR_Touch_RIndexPointing    5
#define OVR_Touch_RThumbUp          6
#define OVR_Touch_LIndexPointing    13
#define OVR_Touch_LThumbUp          14

#define NUM_OVR_TOUCHES             15

// Defines and includes for/from OpenXR SDK:

// Use extension prototypes:
#define XR_EXTENSION_PROTOTYPES

// We use OpenGL for all XR image rendering:
#define XR_USE_GRAPHICS_API_OPENGL
#include "../Screen/GL/glew.h"

// On Linux we use XLib for X11/GLX interfacing to get our OpenGL rendering contexts:
#if PSYCH_SYSTEM == PSYCH_LINUX
#define XR_USE_PLATFORM_XLIB
#define XR_USE_TIMESPEC
#include <X11/Xlib.h>
#include <GL/glx.h>
#endif

// On Windows we use Win32 interfacing to get our OpenGL rendering contexts:
#if PSYCH_SYSTEM == PSYCH_WINDOWS
#define XR_USE_PLATFORM_WIN32
#endif

// OpenXR api headers:
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

// Number of maximum simultaneously open XR devices:
#define MAX_PSYCH_OPENXR_DEVS 10
#define MAX_SYNOPSIS_STRINGS 50

// Declare variables local to this file:
static const char *synopsisSYNOPSIS[MAX_SYNOPSIS_STRINGS];

// Our XR device record:
typedef struct PsychOpenXRDevice {
    psych_bool                          opened;
    psych_bool                          closing;
    psych_bool                          needFrameLoop;
    psych_bool                          sessionActive;
    psych_bool                          lossPending;
    psych_bool                          userExit;
    psych_bool                          multiThreaded;
    psych_thread                        presenterThread;
    psych_mutex                         presenterLock;
    psych_condition                     presentedSignal;
    XrSystemId                          systemId;
    XrSession                           hmd;
    XrSessionState                      state;
    XrSwapchain                         textureSwapChain[2];
    uint32_t                            textureSwapChainLength[2];
    XrSwapchainImageOpenGLKHR*          textureSwapChainImages[2];
    psych_bool                          isStereo;
    psych_bool                          use3DMode;
    int                                 textureWidth;
    int                                 textureHeight;
    int                                 maxWidth;
    int                                 maxHeight;
    int                                 recSamples;
    int                                 maxSamples;
    XrActionSet                         actionSet;
    XrAction                            hapticAction;
    XrAction                            handPoseAction;
    XrPath                              handPath[4];
    XrSpace                             handPoseSpace[2];
    psych_bool                          isTracking;
    XrViewConfigurationType             viewType;
    XrViewState                         viewState;
    XrView                              view[2];
    XrCompositionLayerProjectionView    projView[2];
    XrCompositionLayerQuad              quadViewLayer[2];
    XrSpace                             viewSpace;
    XrSpace                             worldSpace;
    XrPosef                             originPoseInPreviousSpace;
    XrCompositionLayerProjection        projectionLayer;
    const XrCompositionLayerBaseHeader* submitLayers[2];
    uint32_t                            submitLayersCount;
    uint32_t                            frameIndex;
    int                                 commitFrameIndex;
    XrFrameState                        frameState;
    int                                 needSubmit;
    double                              frameDuration;
    double                              sensorSampleTime;
    double                              lastPresentExecTime;
    double                              targetPresentTime;
    double                              VRtimeoutSecs;

    XrAction                            triggerValueAction[2];      // 1D
    XrAction                            gripValueAction[2];         // 1D
    XrAction                            thumbStick2DAction[4];      // 2 x 2D

    XrAction                            buttonAction[NUM_OVR_BUTTONS];  // Bool
    XrAction                            touchAction[NUM_OVR_TOUCHES];   // Bool
} PsychOpenXRDevice;

// Shared XrInstance for the whole process:
static XrInstance xrInstance = XR_NULL_HANDLE;
static XrInstanceProperties instanceProperties;

// Shared debug messenger for the whole process:
XrDebugUtilsMessengerEXT debugMessenger = XR_NULL_HANDLE;

// Global count and requested xr instance extensions:
static uint32_t instanceExtensionsCount = 0;
static unsigned int instanceExtensionsEnabledCount = 0;
static char *instanceExtensionNames[64];

// List and count of open and actively used XR devices:
PsychOpenXRDevice openxrdevices[MAX_PSYCH_OPENXR_DEVS];
static unsigned int devicecount = 0;

// List and count of available XR system devices for use:
XrSystemProperties availableSystems[MAX_PSYCH_OPENXR_DEVS];
static int numAvailableDevices = 0;

static int verbosity = 3;
static psych_bool initialized = FALSE;
static char errorString[XR_MAX_RESULT_STRING_SIZE];

static void* PresenterThreadMain(void* psychOpenXRDeviceToCast);
static double PresentExecute(PsychOpenXRDevice *openxr, psych_bool inInit);

// XR_EXT_debug_utils for runtime debug output:
PFN_xrCreateDebugUtilsMessengerEXT pxrCreateDebugUtilsMessengerEXT = NULL;
PFN_xrDestroyDebugUtilsMessengerEXT pxrDestroyDebugUtilsMessengerEXT = NULL;

// XR_KHR_opengl_enable for OpenGL rendering and interop:
PFN_xrGetOpenGLGraphicsRequirementsKHR pxrGetOpenGLGraphicsRequirementsKHR = NULL;

// XR_FB_display_refresh_rate for HMD video refresh rate query and control:
PFN_xrGetDisplayRefreshRateFB pxrGetDisplayRefreshRateFB = NULL;

#if defined(XR_USE_PLATFORM_WIN32)
// XR_KHR_WIN32_convert_performance_counter_time for MS-Windows timestamp mapping:
PFN_xrConvertWin32PerformanceCounterToTimeKHR pxrConvertWin32PerformanceCounterToTimeKHR = NULL;
PFN_xrConvertTimeToWin32PerformanceCounterKHR pxrConvertTimeToWin32PerformanceCounterKHR = NULL;
#endif

#if defined(XR_USE_PLATFORM_XLIB)
// XR_KHR_convert_timespec_time for Linux timestamp mapping:
PFN_xrConvertTimespecTimeToTimeKHR pxrConvertTimespecTimeToTimeKHR = NULL;
PFN_xrConvertTimeToTimespecTimeKHR pxrConvertTimeToTimespecTimeKHR = NULL;
#endif


#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                                                                                                            \
{                                                                                                                                                           \
    XrResult result;                                                                                                                                        \
    if (((result = xrGetInstanceProcAddr(inst, #entrypoint, (PFN_xrVoidFunction*) &p##entrypoint)) != XR_SUCCESS) || (p##entrypoint == NULL)) {             \
        xrResultToString(inst, result, errorString);                                                                                                        \
        printf("xrGetInstanceProcAddr failed to find " #entrypoint, " => xrGetInstanceProcAddr Failure: %s\n", errorString);                                \
    }                                                                                                                                                       \
}

// Neutral identityPose used for various purposes:
static XrPosef identityPose = {
    .orientation = { .x = 0, .y = 0, .z = 0, .w = 1.0 },
    .position = { .x = 0, .y = 0, .z = 0 }
};

void InitializeSynopsis(void)
{
    int i = 0;
    const char **synopsis = synopsisSYNOPSIS;

    synopsis[i++] = "PsychOpenXRCore - A Psychtoolbox driver for OpenXR compatible VR/AR/MR/XR hardware.";
    synopsis[i++] = "This driver allows to use XR devices supported by a suitable OpenXR runtime of version 1 and higher.\n";
    synopsis[i++] = "Copyright (c) 2022 Mario Kleiner.";
    synopsis[i++] = "The PsychOpenXRCore driver is licensed to you under the terms of the MIT license.";
    synopsis[i++] = "See 'help License.txt' in the Psychtoolbox root folder for more details.";
    synopsis[i++] = "";
    synopsis[i++] = "Usage:";
    synopsis[i++] = "";
    synopsis[i++] = "Functions used by regular user scripts (but mostly indirectly via PsychVRHMD() or PsychOpenXR()):";
    synopsis[i++] = "";
    synopsis[i++] = "oldVerbosity = PsychOpenXRCore('Verbosity' [, verbosity]);";
    synopsis[i++] = "numHMDs = PsychOpenXRCore('GetCount');";
    synopsis[i++] = "[openxrPtr, modelName, runtimeName] = PsychOpenXRCore('Open' [, deviceIndex=0][, multiThreaded=0]);";
    synopsis[i++] = "PsychOpenXRCore('Close' [, openxrPtr]);";
    synopsis[i++] = "controllerTypes = PsychOpenXRCore('Controllers', openxrPtr);";
    //synopsis[i++] = "oldType = PsychOpenXRCore('TrackingOriginType', openxrPtr [, newType]);";
    //synopsis[i++] = "PsychOpenXRCore('Start', openxrPtr);";
    //synopsis[i++] = "PsychOpenXRCore('Stop', openxrPtr);";
    synopsis[i++] = "[state, touch] = PsychOpenXRCore('GetTrackingState', openxrPtr [, predictionTime=nextFrame]);";
    synopsis[i++] = "input = PsychOpenXRCore('GetInputState', openxrPtr, controllerType);";
    synopsis[i++] = "pulseEndTime = PsychOpenXRCore('HapticPulse', openxrPtr, controllerType [, duration=2.5][, freq][, amplitude=1.0]);";
    synopsis[i++] = "[projL, projR] = PsychOpenXRCore('GetStaticRenderParameters', openxrPtr [, clipNear=0.01][, clipFar=10000.0]);";
    synopsis[i++] = "PsychOpenXRCore('StartRender', openxrPtr [, targetTime=nextFrame]);";
    synopsis[i++] = "";
    synopsis[i++] = "Functions usually only used internally by Psychtoolbox:";
    synopsis[i++] = "";
    synopsis[i++] = "[width, height, recMSAASamples, maxMSAASamples, maxWidth, maxHeight] = PsychOpenXRCore('GetFovTextureSize', openxrPtr, eye);";
    synopsis[i++] = "videoRefreshDuration = PsychOpenXRCore('CreateAndStartSession', openxrPtr, deviceContext, openGLContext, openGLDrawable, openGLConfig, openGLVisualId, use3DMode);";
    synopsis[i++] = "[width, height, numTextures, imageFormat] = PsychOpenXRCore('CreateRenderTextureChain', openxrPtr, eye, width, height, floatFormat, numMSAASamples);";
    synopsis[i++] = "texObjectHandle = PsychOpenXRCore('GetNextTextureHandle', openxrPtr, eye);";
    synopsis[i++] = "PsychOpenXRCore('EndFrameRender', openxrPtr [, eye]);";
    synopsis[i++] = "[tPredictedOnset, tPredictedFutureOnset, tDebugFlipTime] = PsychOpenXRCore('PresentFrame', openxrPtr [, when=0]);";
    synopsis[i++] = NULL; // Terminate synopsis strings.

    if (i > MAX_SYNOPSIS_STRINGS) {
        PrintfExit("%s: Increase dimension of synopsis[] from %ld to at least %ld and recompile.", __FILE__, (long) MAX_SYNOPSIS_STRINGS, (long) i);
    }
}

PsychError PsychOpenXRCoreDisplaySynopsis(void)
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

PsychOpenXRDevice* PsychGetXR(int handle, psych_bool dontfail)
{
    if (handle < 1 || handle > MAX_PSYCH_OPENXR_DEVS || !openxrdevices[handle-1].opened) {
        if (!dontfail) {
            printf("PTB-ERROR: Invalid OpenXR device handle %i passed. No such device open.\n", handle);
            PsychErrorExitMsg(PsychError_user, "Invalid OpenXR handle.");
        }

        return(NULL);
    }

    return(&(openxrdevices[handle-1]));
}

PsychOpenXRDevice* PsychGetXRForSession(XrSession sess)
{
    int handle;

    for (handle = 0; handle < MAX_PSYCH_OPENXR_DEVS; handle++)
        if (openxrdevices[handle].opened && openxrdevices[handle].hmd == sess)
            return(&(openxrdevices[handle]));

    return(NULL);
}

static psych_bool resultOK(XrResult result)
{
    if (XR_SUCCEEDED(result))
        return(TRUE);

    if (xrInstance != XR_NULL_HANDLE)
        xrResultToString(xrInstance, result, errorString);
    else
        sprintf(errorString, "Unknown error.");

    return(FALSE);
}

// General debug callback for the XR_EXT_debug_utils extension:
static XrBool32 PsychOpenXRLogCB(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity, XrDebugUtilsMessageTypeFlagsEXT messageTypes,
                                 const XrDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
{
    (void) userData;
    (void) messageTypes;

    if ((messageSeverity == XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT && verbosity > 0) ||
        (messageSeverity == XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT && verbosity > 1) ||
        (messageSeverity == XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT && verbosity > 4)  ||
        (messageSeverity == XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT && verbosity > 9)) {
            printf("PsychOpenXRCore-Runtime: %s:%s:%s\n", callbackData->messageId, callbackData->functionName, callbackData->message);
    }

    return(XR_FALSE);
}

static psych_bool addInstanceExtension(XrExtensionProperties* exts, unsigned int extCount, const char* wantedExt)
{
    unsigned int i;

    if (instanceExtensionsEnabledCount >= 64) {
        if (verbosity > 1)
            printf("PsychOpenXRCore-WARNING: Instance extension array max capacity of 64 slots reached! Skipping add of '%s'. Recompile!\n", wantedExt);
        return(FALSE);
    }

    for (i = 0; i < extCount; i++) {
        if (!strcmp(wantedExt, exts[i].extensionName)) {
            if (verbosity > 3)
                printf("PsychOpenXRCore-INFO: Found and enabled %i. instance extension '%s'.\n", instanceExtensionsEnabledCount, wantedExt);

            instanceExtensionNames[instanceExtensionsEnabledCount++] = (char*) wantedExt;
            return(TRUE);
        }
    }

    if (verbosity > 3)
        printf("PsychOpenXRCore-INFO: Extension '%s' is not supported.\n", wantedExt);

    return(FALSE);
}

static XrTime PsychTimeToXrTime(double refTime)
{
    XrTime outTime = 0;

    refTime = PsychOSRefTimeToMonotonicTime(refTime);

    #if defined(XR_USE_PLATFORM_WIN32)
    {
        LARGE_INTEGER qpc;

        // Map refTime in GetSecs to QPC:
        qpc.QuadPart = (refTime * PsychGetKernelTimebaseFrequencyHz());

        // Map QPC time to XrTime:
        if (!resultOK(pxrConvertWin32PerformanceCounterToTimeKHR(xrInstance, &qpc, &outTime))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: xrConvertWin32PerformanceCounterToTimeKHR() failed: %s\n", errorString);
        }
    }
    #endif

    #if defined(XR_USE_PLATFORM_XLIB)
    {
        struct timespec ts;

        // Map CLOCK_MONOTONIC double time to CLOCK_MONOTONIC timespec time:
        ts.tv_sec   = (unsigned long long) refTime;
        ts.tv_nsec  = ((refTime - (double) ts.tv_sec) * (double) 1e9);

        // Map CLOCK_MONOTONIC timespec time to XrTime:
        if (!resultOK(pxrConvertTimespecTimeToTimeKHR(xrInstance, &ts, &outTime))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: xrConvertTimespecTimeToTimeKHR() failed: %s\n", errorString);
        }
    }
    #endif

    return(outTime);
}

static double XrTimeToPsychTime(XrTime xrTime)
{
    double outTime = 0;

    #if defined(XR_USE_PLATFORM_WIN32)
    {
        LARGE_INTEGER qpc;

        // Map XrTime to QPC time:
        if (!resultOK(pxrConvertTimeToWin32PerformanceCounterKHR(xrInstance, xrTime, &qpc))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: pxrConvertTimeToWin32PerformanceCounterKHR() failed: %s\n", errorString);
        }
        else {
            // Map QPC time to GetSecs time:
            outTime = PsychMapPrecisionTimerTicksToSeconds(qpc.QuadPart);
        }
    }
    #endif

    #if defined(XR_USE_PLATFORM_XLIB)
    {
        struct timespec ts;

        // Map XrTime to CLOCK_MONOTONIC timespec time:
        if (!resultOK(pxrConvertTimeToTimespecTimeKHR(xrInstance, xrTime, &ts))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: xrConvertTimeToTimespecTimeKHR() failed: %s\n", errorString);
        }
        else {
            // Map timespec to CLOCK_MONOTONIC time as double:
            outTime = (double) ts.tv_sec + ((double) ts.tv_nsec / (double) 1e9);

            // Map CLOCK_MONOTONIC to GetSecs time:
            outTime = PsychOSMonotonicToRefTime(outTime);
        }
    }
    #endif

    return(outTime);
}

static int enumerateXRDevices(XrInstance instance) {
    XrSystemId systemId = XR_NULL_SYSTEM_ID;
    numAvailableDevices = 0;

    // For now we only support HMDs, not handheld "SmartPhone" style handheld devices.
    XrSystemGetInfo sysGetInfo = {
        .type = XR_TYPE_SYSTEM_GET_INFO,
        .next = NULL,
        .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY
    };

    XrResult result = xrGetSystem(instance, &sysGetInfo, &systemId);
    if (!resultOK(result) && (result != XR_ERROR_FORM_FACTOR_UNAVAILABLE)) {
        // Query failed:
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to enumerate connected and available XR system hardware: %s\n", errorString);

        // Failure return code:
        return(-1);
    }

    // Query success. Got something? If not, return zero available devices:
    if ((result == XR_ERROR_FORM_FACTOR_UNAVAILABLE) || (systemId == XR_NULL_SYSTEM_ID)) {
        return(0);
    }

    // Got a hardware XR system. Query and store its properties and systemId:
    result = xrGetSystemProperties(instance, systemId, &availableSystems[numAvailableDevices]);
    if (!resultOK(result)) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to query properties of available XR system hardware: %s\n", errorString);

        // Failure return code:
        return(-1);
    }

    if (verbosity > 3) {
        printf("PsychOpenXRCore-INFO: %i. XR system: VendorId 0x%x : \"%s\" : orientationTracking %i : positionTracking %i : %i layers of max size %i x %i.\n", numAvailableDevices,
               availableSystems[numAvailableDevices].vendorId, availableSystems[numAvailableDevices].systemName,
               availableSystems[numAvailableDevices].trackingProperties.orientationTracking,
               availableSystems[numAvailableDevices].trackingProperties.positionTracking,
               availableSystems[numAvailableDevices].graphicsProperties.maxLayerCount,
               availableSystems[numAvailableDevices].graphicsProperties.maxSwapchainImageWidth,
               availableSystems[numAvailableDevices].graphicsProperties.maxSwapchainImageHeight);
    }

    // Increment count of available devices:
    numAvailableDevices++;

    return(numAvailableDevices);
}

static XrPath toXrPath(XrInstance xrInstance, const char* pathSpec)
{
    XrPath ret = XR_NULL_PATH;

    if (!resultOK(xrStringToPath(xrInstance, pathSpec, &ret)) && (verbosity > 0))
        printf("PsychOpenXRCore-ERROR: Failed to build XrPath for string '%s': %s\n", pathSpec, errorString);

    return(ret);
}

// Get current interaction profile name for given input toplevel 'path': A non-NULL 'profile' must have at least 128 chars capacity.
// Returns TRUE if profile active, FALSE on error or no profile bound or string mapping failure if 'profile' is requested.
static psych_bool getActiveXRInteractionProfile(XrInstance xrInstance, PsychOpenXRDevice *openxr, XrPath path, char* profile)
{
    uint32_t out_Length = 0;

    XrInteractionProfileState state = {
        .type = XR_TYPE_INTERACTION_PROFILE_STATE,
        .next = NULL,
    };

    // Query active interaction profile for toplevel input path 'path':
    if (!resultOK(xrGetCurrentInteractionProfile(openxr->hmd, path, &state))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR:getActiveXRInteractionProfile(): xrGetCurrentInteractionProfile() failed: %s\n", errorString);

        return(FALSE);
    }

    // None active?
    if (state.interactionProfile == XR_NULL_PATH) {
        // This probably means no suitable input device bound:
        return(FALSE);
    }

    // Profile active - map XrPath of profile to string and return it, if 'profile' name requested, ie. non-NULL:
    if (profile && !resultOK(xrPathToString(xrInstance, state.interactionProfile, 128, &out_Length, profile))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR:getActiveXRInteractionProfile(): xrPathToString() failed: %s\n", errorString);

        return(FALSE);
    }

    return(TRUE);
}

static psych_bool suggestXRInteractionBindings(XrInstance xrInstance, const char* interactionProfile, uint32_t count, const XrActionSuggestedBinding* bindings)
{
    XrPath interactionProfilePath;

    if (!resultOK(xrStringToPath(xrInstance, interactionProfile, &interactionProfilePath))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to build path for %s interaction profile: %s\n", interactionProfile, errorString);

        // Failure return code:
        return(FALSE);
    }

    XrInteractionProfileSuggestedBinding suggestedBindings = {
        .type = XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING,
        .next = NULL,
        .interactionProfile = interactionProfilePath,
        .countSuggestedBindings = count,
        .suggestedBindings = bindings,
    };

    if (!resultOK(xrSuggestInteractionProfileBindings(xrInstance, &suggestedBindings))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to suggest action bindings for %s interaction profile: %s\n", interactionProfile, errorString);

        // Failure return code:
        return(FALSE);
    }

    return(TRUE);
}

static psych_bool createDefaultXRInputConfig(PsychOpenXRDevice* openxr)
{
    int i;

    // Build all standard path strings we may need:
    xrStringToPath(xrInstance, "/user/hand/left", &openxr->handPath[0]);
    xrStringToPath(xrInstance, "/user/hand/right", &openxr->handPath[1]);
    xrStringToPath(xrInstance, "/user/gamepad", &openxr->handPath[2]);
    xrStringToPath(xrInstance, "/user/head", &openxr->handPath[3]);

    XrPath gripPosePath[2];
    xrStringToPath(xrInstance, "/user/hand/left/input/grip/pose", &gripPosePath[0]);
    xrStringToPath(xrInstance, "/user/hand/right/input/grip/pose", &gripPosePath[1]);

    XrPath aimPosePath[2];
    xrStringToPath(xrInstance, "/user/hand/left/input/aim/pose", &aimPosePath[0]);
    xrStringToPath(xrInstance, "/user/hand/right/input/aim/pose", &aimPosePath[1]);

    XrPath hapticPath[4];
    xrStringToPath(xrInstance, "/user/hand/left/output/haptic", &hapticPath[0]);
    xrStringToPath(xrInstance, "/user/hand/right/output/haptic", &hapticPath[1]);
    xrStringToPath(xrInstance, "/user/gamepad/output/haptic_left", &hapticPath[2]);
    xrStringToPath(xrInstance, "/user/gamepad/output/haptic_right", &hapticPath[3]);

    XrPath triggerPath[4];
    xrStringToPath(xrInstance, "/user/hand/left/input/trigger", &triggerPath[0]);
    xrStringToPath(xrInstance, "/user/hand/right/input/trigger", &triggerPath[1]);
    xrStringToPath(xrInstance, "/user/gamepad/input/trigger_left/value", &triggerPath[2]);
    xrStringToPath(xrInstance, "/user/gamepad/input/trigger_right/value", &triggerPath[3]);

    XrPath gripPath[2];
    xrStringToPath(xrInstance, "/user/hand/left/input/squeeze", &gripPath[0]);
    xrStringToPath(xrInstance, "/user/hand/right/input/squeeze", &gripPath[1]);

    XrPath trackpadPath[2];
    xrStringToPath(xrInstance, "/user/hand/left/input/trackpad", &trackpadPath[0]);
    xrStringToPath(xrInstance, "/user/hand/right/input/trackpad", &trackpadPath[1]);

    XrPath thumbStickPath[4];
    xrStringToPath(xrInstance, "/user/hand/left/input/thumbstick", &thumbStickPath[0]);
    xrStringToPath(xrInstance, "/user/hand/right/input/thumbstick", &thumbStickPath[1]);
    xrStringToPath(xrInstance, "/user/gamepad/input/thumbstick_left", &thumbStickPath[2]);
    xrStringToPath(xrInstance, "/user/gamepad/input/thumbstick_right", &thumbStickPath[3]);

    // Create our action set which is the container for all our input actions:
    XrActionSetCreateInfo actionSetCreateInfo = {
        .type = XR_TYPE_ACTION_SET_CREATE_INFO,
        .next = NULL,
        .priority = 0,
        .actionSetName = "actionset",
        .localizedActionSetName = "Main set of input actions",
    };

    if (!resultOK(xrCreateActionSet(xrInstance, &actionSetCreateInfo, &openxr->actionSet))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to create main input/output actionSet: %s\n", errorString);

        // Failure return code:
        return(FALSE);
    }

    // Create action and action spaces for left/right hand (aka touch controller) pose tracking:
    XrActionCreateInfo handPoseActionInfo = {
        .type = XR_TYPE_ACTION_CREATE_INFO,
        .next = NULL,
        .actionType = XR_ACTION_TYPE_POSE_INPUT,
        .countSubactionPaths = 2,
        .subactionPaths = openxr->handPath,
        .actionName = "handposeaction",
        .localizedActionName = "Hand Pose input action"
    };

    if (!resultOK(xrCreateAction(openxr->actionSet, &handPoseActionInfo, &openxr->handPoseAction))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to create handPoseAction: %s\n", errorString);

        // Failure return code:
        return(FALSE);
    }

    for (i = 0; i < 2; i++) {
        XrActionSpaceCreateInfo actionSpaceCreateInfo = {
            .type = XR_TYPE_ACTION_SPACE_CREATE_INFO,
            .next = NULL,
            .action = openxr->handPoseAction,
            .poseInActionSpace = identityPose,
            .subactionPath = openxr->handPath[i]
        };

        if (!resultOK(xrCreateActionSpace(openxr->hmd, &actionSpaceCreateInfo, &openxr->handPoseSpace[i]))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create action space for hand/touch controller %i: %s\n", i, errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    // Define haptic feedback output action:
    XrActionCreateInfo hapticActionInfo = {
        .type = XR_TYPE_ACTION_CREATE_INFO,
        .next = NULL,
        .actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT,
        .countSubactionPaths = 3,
        .subactionPaths = openxr->handPath,
        .actionName = "handhapticaction",
        .localizedActionName = "Hand haptic output action"
    };

    if (!resultOK(xrCreateAction(openxr->actionSet, &hapticActionInfo, &openxr->hapticAction))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to create hapticAction: %s\n", errorString);

        // Failure return code:
        return(FALSE);
    }

    // Define input actions for trigger/grip/thumbsticks on hand-controllers and gamepads:
    // ===================================================================================

    // Left trigger:
    {
        XrActionCreateInfo triggerValueActionInfo = {
            .type = XR_TYPE_ACTION_CREATE_INFO,
            .next = NULL,
            .actionType = XR_ACTION_TYPE_FLOAT_INPUT,
            .countSubactionPaths = 3,
            .subactionPaths = openxr->handPath,
            .actionName = "triggervalueactionleft",
            .localizedActionName = "Left-Trigger value"
        };

        if (!resultOK(xrCreateAction(openxr->actionSet, &triggerValueActionInfo, &openxr->triggerValueAction[0]))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create triggerValueAction[0]: %s\n", errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    // Right trigger:
    {
        XrActionCreateInfo triggerValueActionInfo = {
            .type = XR_TYPE_ACTION_CREATE_INFO,
            .next = NULL,
            .actionType = XR_ACTION_TYPE_FLOAT_INPUT,
            .countSubactionPaths = 3,
            .subactionPaths = openxr->handPath,
            .actionName = "triggervalueactionright",
            .localizedActionName = "Right-Trigger value"
        };

        if (!resultOK(xrCreateAction(openxr->actionSet, &triggerValueActionInfo, &openxr->triggerValueAction[1]))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create triggerValueAction[1]: %s\n", errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    // Left grip:
    {
        XrActionCreateInfo gripValueActionInfo = {
            .type = XR_TYPE_ACTION_CREATE_INFO,
            .next = NULL,
            .actionType = XR_ACTION_TYPE_FLOAT_INPUT,
            .countSubactionPaths = 3,
            .subactionPaths = openxr->handPath,
            .actionName = "gripvalueactionleft",
            .localizedActionName = "Left-Grip value"
        };

        if (!resultOK(xrCreateAction(openxr->actionSet, &gripValueActionInfo, &openxr->gripValueAction[0]))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create gripValueAction[0]: %s\n", errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    // Right grip:
    {
        XrActionCreateInfo gripValueActionInfo = {
            .type = XR_TYPE_ACTION_CREATE_INFO,
            .next = NULL,
            .actionType = XR_ACTION_TYPE_FLOAT_INPUT,
            .countSubactionPaths = 3,
            .subactionPaths = openxr->handPath,
            .actionName = "gripvalueactionright",
            .localizedActionName = "Right-Grip value"
        };

        if (!resultOK(xrCreateAction(openxr->actionSet, &gripValueActionInfo, &openxr->gripValueAction[1]))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create gripValueAction[1]: %s\n", errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    // Left thumbstick / trackpad / joystick:
    {
        XrActionCreateInfo thumbStick2DActionInfo = {
            .type = XR_TYPE_ACTION_CREATE_INFO,
            .next = NULL,
            .actionType = XR_ACTION_TYPE_VECTOR2F_INPUT,
            .countSubactionPaths = 3,
            .subactionPaths = openxr->handPath,
            .actionName = "thumbstickactionleft",
            .localizedActionName = "Left-Thumbstick"
        };

        if (!resultOK(xrCreateAction(openxr->actionSet, &thumbStick2DActionInfo, &openxr->thumbStick2DAction[0]))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create thumbStick2DAction[0]: %s\n", errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    // Right thumbstick / trackpad / joystick:
    {
        XrActionCreateInfo thumbStick2DActionInfo = {
            .type = XR_TYPE_ACTION_CREATE_INFO,
            .next = NULL,
            .actionType = XR_ACTION_TYPE_VECTOR2F_INPUT,
            .countSubactionPaths = 3,
            .subactionPaths = openxr->handPath,
            .actionName = "thumbstickactionright",
            .localizedActionName = "Right-Thumbstick"
        };

        if (!resultOK(xrCreateAction(openxr->actionSet, &thumbStick2DActionInfo, &openxr->thumbStick2DAction[1]))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create thumbStick2DAction[1]: %s\n", errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    // Left thumbstick2 / trackpad2 / joystick2:
    {
        XrActionCreateInfo thumbStick2DActionInfo = {
            .type = XR_TYPE_ACTION_CREATE_INFO,
            .next = NULL,
            .actionType = XR_ACTION_TYPE_VECTOR2F_INPUT,
            .countSubactionPaths = 3,
            .subactionPaths = openxr->handPath,
            .actionName = "thumbstickactionleft2",
            .localizedActionName = "Left-Thumbstick2"
        };

        if (!resultOK(xrCreateAction(openxr->actionSet, &thumbStick2DActionInfo, &openxr->thumbStick2DAction[2]))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create thumbStick2DAction[2]: %s\n", errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    // Right thumbstick2 / trackpad2 / joystick2:
    {
        XrActionCreateInfo thumbStick2DActionInfo = {
            .type = XR_TYPE_ACTION_CREATE_INFO,
            .next = NULL,
            .actionType = XR_ACTION_TYPE_VECTOR2F_INPUT,
            .countSubactionPaths = 3,
            .subactionPaths = openxr->handPath,
            .actionName = "thumbstickactionright2",
            .localizedActionName = "Right-Thumbstick2"
        };

        if (!resultOK(xrCreateAction(openxr->actionSet, &thumbStick2DActionInfo, &openxr->thumbStick2DAction[3]))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create thumbStick2DAction[3]: %s\n", errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    // Buttons:
    for (i = 0; i < NUM_OVR_BUTTONS; i++) {
        XrActionCreateInfo buttonActionInfo = {
            .type = XR_TYPE_ACTION_CREATE_INFO,
            .next = NULL,
            .actionType = XR_ACTION_TYPE_BOOLEAN_INPUT,
            .countSubactionPaths = 4,
            .subactionPaths = openxr->handPath,
        };

        sprintf(buttonActionInfo.actionName, "buttonaction_%i", i);
        sprintf(buttonActionInfo.localizedActionName, "Button %i", i);

        if (!resultOK(xrCreateAction(openxr->actionSet, &buttonActionInfo, &openxr->buttonAction[i]))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create buttonAction[%i]: %s\n", i, errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    // Touch sensors:
    for (i = 0; i < NUM_OVR_TOUCHES; i++) {
        XrActionCreateInfo touchActionInfo = {
            .type = XR_TYPE_ACTION_CREATE_INFO,
            .next = NULL,
            .actionType = XR_ACTION_TYPE_BOOLEAN_INPUT,
            .countSubactionPaths = 4,
            .subactionPaths = openxr->handPath,
        };

        sprintf(touchActionInfo.actionName, "touchaction_%i", i);
        sprintf(touchActionInfo.localizedActionName, "Touch %i", i);

        if (!resultOK(xrCreateAction(openxr->actionSet, &touchActionInfo, &openxr->touchAction[i]))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create touchAction[%i]: %s\n", i, errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    #define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
    #define BBIND(a, b) { .action = openxr->buttonAction[(a)], .binding = toXrPath(xrInstance, (b)) }
    #define TBIND(a, b) { .action = openxr->touchAction[(a)], .binding = toXrPath(xrInstance, (b)) }

    // Binding set useful to most controllers and devices:
    XrActionSuggestedBinding unused_template[] = {
        { .action = openxr->handPoseAction, .binding = aimPosePath[0] },
        { .action = openxr->handPoseAction, .binding = aimPosePath[1] },
        { .action = openxr->hapticAction, .binding = hapticPath[0] }, // Not Daydream
        { .action = openxr->hapticAction, .binding = hapticPath[1] }, // Not Daydream
        { .action = openxr->triggerValueAction[0], .binding = triggerPath[0] }, // Not simple or Daydream
        { .action = openxr->triggerValueAction[1], .binding = triggerPath[1] }, // Not simple or Daydream
        { .action = openxr->gripValueAction[0], .binding = gripPath[0] }, // Not simple or Daydream
        { .action = openxr->gripValueAction[1], .binding = gripPath[1] }, // Not simple or Daydream
        { .action = openxr->thumbStick2DAction[0], .binding = thumbStickPath[0] }, // Not simple or Daydream or Vive
        { .action = openxr->thumbStick2DAction[1], .binding = thumbStickPath[1] }, // Not simple or Daydream or Vive
        { .action = openxr->thumbStick2DAction[2], .binding = trackpadPath[0] }, // Not simple or Daydream or Vive
        { .action = openxr->thumbStick2DAction[3], .binding = trackpadPath[1] }, // Not simple or Daydream or Vive
        BBIND(OVR_Button_A, "/user/hand/left/input/a/click"), // Index
        BBIND(OVR_Button_A, "/user/hand/right/input/a/click"), // Touch, Index
        BBIND(OVR_Button_B, "/user/hand/left/input/b/click"), // Index
        BBIND(OVR_Button_B, "/user/hand/right/input/b/click"), // Touch, Index
        BBIND(OVR_Button_X, "/user/hand/left/input/x/click"), // Touch
        BBIND(OVR_Button_X, "/user/hand/right/input/x/click"), // NO
        BBIND(OVR_Button_Y, "/user/hand/left/input/y/click"), // Touch
        BBIND(OVR_Button_Y, "/user/hand/right/input/y/click"), // NO
        BBIND(OVR_Button_LThumb, "/user/hand/left/input/thumbstick/click"), // Touch, Index, WMR, HPe, Cosmos, Focus
        BBIND(OVR_Button_RThumb, "/user/hand/right/input/thumbstick/click"), // Touch, Index, WMR, HPe, Cosmos, Focus
        BBIND(OVR_Button_Enter, "/user/hand/left/input/menu/click"), // Simple, Vive, WMR, Touch
        BBIND(OVR_Button_Enter, "/user/hand/right/input/menu/click"), // Simple, Vive, WMR
        BBIND(OVR_Button_Back, "/user/hand/left/input/select/click"), // Simple, Daydream
        BBIND(OVR_Button_Back, "/user/hand/right/input/select/click"), // Simple, Daydream
        BBIND(OVR_Button_Home, "/user/hand/left/input/system/click"), // Vive, Index
        BBIND(OVR_Button_Home, "/user/hand/right/input/system/click"), // Vive, Touch, Index, Cosmos, Focus

        // Extensions
        BBIND(OVR_Button_VolDown, "/user/hand/left/input/volume_down/click"), // Huawaii
        BBIND(OVR_Button_VolDown, "/user/hand/left/input/volume_down/click"), // Huawaii
        BBIND(OVR_Button_VolUp, "/user/hand/left/input/volume_up/click"), // Huawaii
        BBIND(OVR_Button_VolUp, "/user/hand/left/input/volume_up/click"), // Huawaii
        BBIND(OVR_Button_Back, "/user/hand/left/input/back/click"), // Go
        BBIND(OVR_Button_Back, "/user/hand/right/input/back/click"), // Go
        BBIND(OVR_Button_LShoulder, "/user/hand/left/input/shoulder/click"), // Vive Cosmos
        BBIND(OVR_Button_RShoulder, "/user/hand/right/input/shoulder/click"), // Vive Cosmos

        TBIND(OVR_Touch_A, "/user/hand/left/input/a/touch"), // Index
        TBIND(OVR_Touch_A, "/user/hand/right/input/a/touch"), // Touch, Index
        TBIND(OVR_Touch_B, "/user/hand/left/input/b/touch"), // Index
        TBIND(OVR_Touch_B, "/user/hand/right/input/b/touch"), // Touch, Index
        TBIND(OVR_Touch_X, "/user/hand/left/input/x/touch"), // Touch
        TBIND(OVR_Touch_Y, "/user/hand/left/input/y/touch"), // Touch
        TBIND(OVR_Touch_LIndexTrigger, "/user/hand/left/input/trigger/touch"), // Touch, Focus
        TBIND(OVR_Touch_RIndexTrigger, "/user/hand/right/input/trigger/touch"), // Touch, Focus
        TBIND(OVR_Touch_LThumb, "/user/hand/left/input/thumbstick/touch"), // Touch, Index, Cosmos, Focus
        TBIND(OVR_Touch_RThumb, "/user/hand/right/input/thumbstick/touch"), // Touch, Index, Cosmos, Focus
        TBIND(OVR_Touch_LThumbRest, "/user/hand/left/input/thumbrest/touch"), // Touch, Focus
        TBIND(OVR_Touch_RThumbRest, "/user/hand/right/input/thumbrest/touch"), // Touch, Focus
    };

    // Suggest basic action bindings for the Khronos simple_controller interaction profile,
    // which is a generic fallback expected to be supported by many different XR devices, so
    // an OpenXR app that supports this profile will have at least basic controller input and
    // tracking working with all kinds of controller devices:
    {
        XrActionSuggestedBinding simpleBinding[] = {
            { .action = openxr->handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr->handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr->hapticAction, .binding = hapticPath[0] },
            { .action = openxr->hapticAction, .binding = hapticPath[1] },
            BBIND(OVR_Button_Enter, "/user/hand/left/input/menu/click"),
            BBIND(OVR_Button_Enter, "/user/hand/right/input/menu/click"),
            BBIND(OVR_Button_Back, "/user/hand/left/input/select/click"),
            BBIND(OVR_Button_Back, "/user/hand/right/input/select/click"),
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/khr/simple_controller", ARRAY_SIZE(simpleBinding), simpleBinding))
            return(FALSE);
    }

    // Suggest basic action bindings for the Google Daydream controller interaction profile:
    {
        XrActionSuggestedBinding daydreamBinding[] = {
            { .action = openxr->handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr->handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr->thumbStick2DAction[0], .binding = trackpadPath[0] },
            { .action = openxr->thumbStick2DAction[1], .binding = trackpadPath[1] },
            BBIND(OVR_Button_Back, "/user/hand/left/input/select/click"),
            BBIND(OVR_Button_Back, "/user/hand/right/input/select/click"),
            BBIND(OVR_Button_LThumb, "/user/hand/left/input/trackpad/click"),
            BBIND(OVR_Button_RThumb, "/user/hand/right/input/trackpad/click"),
            TBIND(OVR_Touch_LThumb, "/user/hand/left/input/trackpad/touch"),
            TBIND(OVR_Touch_RThumb, "/user/hand/right/input/trackpad/touch"),
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/google/daydream_controller", ARRAY_SIZE(daydreamBinding), daydreamBinding))
            return(FALSE);
    }

    // Suggest basic action bindings for the HTC Vive controller interaction profile.
    {
        XrActionSuggestedBinding viveBinding[] = {
            { .action = openxr->handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr->handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr->hapticAction, .binding = hapticPath[0] },
            { .action = openxr->hapticAction, .binding = hapticPath[1] },
            { .action = openxr->triggerValueAction[0], .binding = triggerPath[0] },
            { .action = openxr->triggerValueAction[1], .binding = triggerPath[1] },
            { .action = openxr->gripValueAction[0], .binding = gripPath[0] },
            { .action = openxr->gripValueAction[1], .binding = gripPath[1] },
            { .action = openxr->thumbStick2DAction[0], .binding = trackpadPath[0] },
            { .action = openxr->thumbStick2DAction[1], .binding = trackpadPath[1] },
            BBIND(OVR_Button_Enter, "/user/hand/left/input/menu/click"),
            BBIND(OVR_Button_Enter, "/user/hand/right/input/menu/click"),
            BBIND(OVR_Button_LThumb, "/user/hand/left/input/trackpad/click"),
            BBIND(OVR_Button_RThumb, "/user/hand/right/input/trackpad/click"),
            TBIND(OVR_Touch_LThumb, "/user/hand/left/input/trackpad/touch"),
            TBIND(OVR_Touch_RThumb, "/user/hand/right/input/trackpad/touch"),
            // Following don't work on Windows/OculusVR, but do on Linux/Monado
            // BBIND(OVR_Button_Home, "/user/hand/left/input/system/click"),
            // BBIND(OVR_Button_Home, "/user/hand/right/input/system/click"),
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/htc/vive_controller", ARRAY_SIZE(viveBinding), viveBinding))
            return(FALSE);
    }

    // Suggest basic action bindings for the Oculus touch controller interaction profile, which
    // is used by Oculus VR touch input controllers, e.g., for Oculus Rift-CV1, Rift-S, Quest etc.:
    {
        XrActionSuggestedBinding oculusBinding[] = {
            { .action = openxr->handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr->handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr->hapticAction, .binding = hapticPath[0] },
            { .action = openxr->hapticAction, .binding = hapticPath[1] },
            { .action = openxr->triggerValueAction[0], .binding = triggerPath[0] },
            { .action = openxr->triggerValueAction[1], .binding = triggerPath[1] },
            { .action = openxr->gripValueAction[0], .binding = gripPath[0] },
            { .action = openxr->gripValueAction[1], .binding = gripPath[1] },
            { .action = openxr->thumbStick2DAction[0], .binding = thumbStickPath[0] },
            { .action = openxr->thumbStick2DAction[1], .binding = thumbStickPath[1] },
            BBIND(OVR_Button_A, "/user/hand/right/input/a/click"),
            BBIND(OVR_Button_B, "/user/hand/right/input/b/click"),
            BBIND(OVR_Button_X, "/user/hand/left/input/x/click"),
            BBIND(OVR_Button_Y, "/user/hand/left/input/y/click"),
            BBIND(OVR_Button_LThumb, "/user/hand/left/input/thumbstick/click"),
            BBIND(OVR_Button_RThumb, "/user/hand/right/input/thumbstick/click"),
            BBIND(OVR_Button_Enter, "/user/hand/left/input/menu/click"),
            BBIND(OVR_Button_Home, "/user/hand/right/input/system/click"),
            TBIND(OVR_Touch_A, "/user/hand/right/input/a/touch"),
            TBIND(OVR_Touch_B, "/user/hand/right/input/b/touch"),
            TBIND(OVR_Touch_X, "/user/hand/left/input/x/touch"),
            TBIND(OVR_Touch_Y, "/user/hand/left/input/y/touch"),
            TBIND(OVR_Touch_LIndexTrigger, "/user/hand/left/input/trigger/touch"),
            TBIND(OVR_Touch_RIndexTrigger, "/user/hand/right/input/trigger/touch"),
            TBIND(OVR_Touch_LThumb, "/user/hand/left/input/thumbstick/touch"),
            TBIND(OVR_Touch_RThumb, "/user/hand/right/input/thumbstick/touch"),
            TBIND(OVR_Touch_LThumbRest, "/user/hand/left/input/thumbrest/touch"),
            TBIND(OVR_Touch_RThumbRest, "/user/hand/right/input/thumbrest/touch"),
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/oculus/touch_controller", ARRAY_SIZE(oculusBinding), oculusBinding))
            return(FALSE);
    }

    // Suggest basic action bindings for the Oculus Go controller interaction profile, which
    // is used by Oculus controllers of the Oculus Go:
    {
        XrActionSuggestedBinding oculusGoBinding[] = {
            { .action = openxr->handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr->handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr->triggerValueAction[0], .binding = triggerPath[0] },
            { .action = openxr->triggerValueAction[1], .binding = triggerPath[1] },
            { .action = openxr->thumbStick2DAction[0], .binding = trackpadPath[0] },
            { .action = openxr->thumbStick2DAction[1], .binding = trackpadPath[1] },
            BBIND(OVR_Button_LThumb, "/user/hand/left/input/trackpad/click"),
            BBIND(OVR_Button_RThumb, "/user/hand/right/input/trackpad/click"),
            BBIND(OVR_Button_Back, "/user/hand/left/input/back/click"),
            BBIND(OVR_Button_Back, "/user/hand/right/input/back/click"),
            TBIND(OVR_Touch_LThumb, "/user/hand/left/input/trackpad/touch"),
            TBIND(OVR_Touch_RThumb, "/user/hand/right/input/trackpad/touch"),
            // Following don't work on Windows/OculusVR, but do on Linux/Monado
            // BBIND(OVR_Button_Home, "/user/hand/left/input/system/click"),
            // BBIND(OVR_Button_Home, "/user/hand/right/input/system/click"),
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/oculus/go_controller", ARRAY_SIZE(oculusGoBinding), oculusGoBinding))
            return(FALSE);
    }

    // Suggest basic action bindings for the Valve Index controller interaction profile.
    {
        XrActionSuggestedBinding valveIndexBinding[] = {
            { .action = openxr->handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr->handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr->hapticAction, .binding = hapticPath[0] }, // Not Daydream
            { .action = openxr->hapticAction, .binding = hapticPath[1] }, // Not Daydream
            { .action = openxr->triggerValueAction[0], .binding = triggerPath[0] },
            { .action = openxr->triggerValueAction[1], .binding = triggerPath[1] },
            { .action = openxr->gripValueAction[0], .binding = gripPath[0] },
            { .action = openxr->gripValueAction[1], .binding = gripPath[1] },
            { .action = openxr->thumbStick2DAction[0], .binding = thumbStickPath[0] },
            { .action = openxr->thumbStick2DAction[1], .binding = thumbStickPath[1] },
            // Trackpad is mapped to Thumbstick2:
            { .action = openxr->thumbStick2DAction[2], .binding = trackpadPath[0] },
            { .action = openxr->thumbStick2DAction[3], .binding = trackpadPath[1] },
            BBIND(OVR_Button_A, "/user/hand/left/input/a/click"),
            BBIND(OVR_Button_A, "/user/hand/right/input/a/click"),
            BBIND(OVR_Button_B, "/user/hand/left/input/b/click"),
            BBIND(OVR_Button_B, "/user/hand/right/input/b/click"),
            BBIND(OVR_Button_LThumb, "/user/hand/left/input/thumbstick/click"),
            BBIND(OVR_Button_RThumb, "/user/hand/right/input/thumbstick/click"),
            TBIND(OVR_Touch_A, "/user/hand/left/input/a/touch"),
            TBIND(OVR_Touch_A, "/user/hand/right/input/a/touch"),
            TBIND(OVR_Touch_B, "/user/hand/left/input/b/touch"),
            TBIND(OVR_Touch_B, "/user/hand/right/input/b/touch"),
            TBIND(OVR_Touch_LIndexTrigger, "/user/hand/left/input/trigger/touch"),
            TBIND(OVR_Touch_RIndexTrigger, "/user/hand/right/input/trigger/touch"),
            TBIND(OVR_Touch_LThumb, "/user/hand/left/input/thumbstick/touch"),
            TBIND(OVR_Touch_RThumb, "/user/hand/right/input/thumbstick/touch"),
            // Following don't work on Windows/OculusVR, but do on Linux/Monado
            // BBIND(OVR_Button_Home, "/user/hand/left/input/system/click"),
            // BBIND(OVR_Button_Home, "/user/hand/right/input/system/click"),
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/valve/index_controller", ARRAY_SIZE(valveIndexBinding), valveIndexBinding))
            return(FALSE);
    }

    // Suggest basic action bindings for the Microsoft Mixed Reality (WMR) controller interaction profile.
    {
        XrActionSuggestedBinding wmrBinding[] = {
            { .action = openxr->handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr->handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr->hapticAction, .binding = hapticPath[0] },
            { .action = openxr->hapticAction, .binding = hapticPath[1] },
            { .action = openxr->triggerValueAction[0], .binding = triggerPath[0] },
            { .action = openxr->triggerValueAction[1], .binding = triggerPath[1] },
            { .action = openxr->gripValueAction[0], .binding = gripPath[0] },
            { .action = openxr->gripValueAction[1], .binding = gripPath[1] },
            { .action = openxr->thumbStick2DAction[0], .binding = thumbStickPath[0] },
            { .action = openxr->thumbStick2DAction[1], .binding = thumbStickPath[1] },
            // Trackpad is mapped to Thumbstick2:
            { .action = openxr->thumbStick2DAction[2], .binding = trackpadPath[0] },
            { .action = openxr->thumbStick2DAction[3], .binding = trackpadPath[1] },
            BBIND(OVR_Button_LThumb, "/user/hand/left/input/thumbstick/click"),
            BBIND(OVR_Button_RThumb, "/user/hand/right/input/thumbstick/click"),
            BBIND(OVR_Button_Enter, "/user/hand/left/input/menu/click"),
            BBIND(OVR_Button_Enter, "/user/hand/right/input/menu/click"),

            // Map trackpad click/touch onto these, although it is not strictly correct:
            BBIND(OVR_Button_LShoulder, "/user/hand/left/input/trackpad/click"),
            BBIND(OVR_Button_RShoulder, "/user/hand/right/input/trackpad/click"),
            TBIND(OVR_Touch_LThumb, "/user/hand/left/input/trackpad/touch"),
            TBIND(OVR_Touch_RThumb, "/user/hand/right/input/trackpad/touch"),

        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/microsoft/motion_controller", ARRAY_SIZE(wmrBinding), wmrBinding))
            return(FALSE);
    }

    // Suggest action bindings for Microsoft XBox controller:
    {
        XrActionSuggestedBinding xboxBinding[] = {
            { .action = openxr->hapticAction, .binding = hapticPath[2] },
            { .action = openxr->hapticAction, .binding = hapticPath[3] },
            { .action = openxr->triggerValueAction[0], .binding = triggerPath[2] },
            { .action = openxr->triggerValueAction[1], .binding = triggerPath[3] },
            { .action = openxr->thumbStick2DAction[0], .binding = thumbStickPath[2] },
            { .action = openxr->thumbStick2DAction[1], .binding = thumbStickPath[3] },
            BBIND(OVR_Button_Enter, "/user/gamepad/input/menu/click"),
            BBIND(OVR_Button_Back, "/user/gamepad/input/view/click"),
            BBIND(OVR_Button_A, "/user/gamepad/input/a/click"),
            BBIND(OVR_Button_B, "/user/gamepad/input/b/click"),
            BBIND(OVR_Button_X, "/user/gamepad/input/x/click"),
            BBIND(OVR_Button_Y, "/user/gamepad/input/y/click"),
            BBIND(OVR_Button_Down, "/user/gamepad/input/dpad_down/click"),
            BBIND(OVR_Button_Right, "/user/gamepad/input/dpad_right/click"),
            BBIND(OVR_Button_Up, "/user/gamepad/input/dpad_up/click"),
            BBIND(OVR_Button_Left, "/user/gamepad/input/dpad_left/click"),
            BBIND(OVR_Button_LShoulder, "/user/gamepad/input/shoulder_left/click"),
            BBIND(OVR_Button_RShoulder, "/user/gamepad/input/shoulder_right/click"),
            BBIND(OVR_Button_LThumb, "/user/gamepad/input/thumbstick_left/click"),
            BBIND(OVR_Button_RThumb, "/user/gamepad/input/thumbstick_right/click")
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/microsoft/xbox_controller", ARRAY_SIZE(xboxBinding), xboxBinding))
            return(FALSE);
    }

    // Suggest action bindings for Vive Pro HMD control buttons:
    {
        XrActionSuggestedBinding viveProBinding[] = {
            BBIND(OVR_Button_VolDown, "/user/head/input/volume_down/click"),
            BBIND(OVR_Button_VolUp, "/user/head/input/volume_up/click"),
            BBIND(OVR_Button_MicMute, "/user/head/input/mute_mic/click"),
            // Following does not work on Windows/OculusVR, but does on Linux/Monado
            // BBIND(OVR_Button_Home, "/user/head/input/system/click"),
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/htc/vive_pro", ARRAY_SIZE(viveProBinding), viveProBinding))
            return(FALSE);
    }

    return(TRUE);
}

static psych_bool locateXRViews(PsychOpenXRDevice* openxr, XrTime predictionTime)
{
    XrViewLocateInfo viewLocateInfo = {
        .type = XR_TYPE_VIEW_LOCATE_INFO,
        .next = NULL,
        .viewConfigurationType = openxr->viewType,
        .displayTime = predictionTime,
        .space = openxr->worldSpace
    };

    // Init for query:
    openxr->viewState.type = XR_TYPE_VIEW_STATE;
    openxr->viewState.next = NULL;

    uint32_t viewCount = (openxr->viewType == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO) ? 2 : 1;
    if (!resultOK(xrLocateViews(openxr->hmd, &viewLocateInfo, &openxr->viewState, viewCount, &viewCount, openxr->view))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Locating Xrviews via xrLocateViews() failed: %s\n", errorString);

        return(FALSE);
    }

    return(TRUE);
}

static psych_bool syncXRActions(PsychOpenXRDevice* openxr)
{
    XrActiveActionSet activeActionSet = {
        .actionSet = openxr->actionSet,
        .subactionPath = XR_NULL_PATH
    };

    XrActionsSyncInfo actionsSyncInfo = {
        .type = XR_TYPE_ACTIONS_SYNC_INFO,
        .next = NULL,
        .countActiveActionSets = 1,
        .activeActionSets = &activeActionSet
    };

    if (!resultOK(xrSyncActions(openxr->hmd, &actionsSyncInfo))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Syncing activeActionSet via xrSyncActions() failed: %s\n", errorString);

        return(FALSE);
    }

    return(TRUE);
}

static psych_bool processXREvents(XrInstance pollInstance)
{
    while (TRUE) {
        PsychOpenXRDevice* openxr = NULL;
        XrEventDataBuffer event = {
            .type = XR_TYPE_EVENT_DATA_BUFFER,
            .next = NULL
        };

        // Poll for - and dequeue - next pending event, non-blocking:
        XrResult result = xrPollEvent(pollInstance, &event);
        if (!resultOK(result)) {
            // Failed - Error return:
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Event polling for instance %p failed: %s\n", pollInstance, errorString);

            // Failure return code:
            return(FALSE);
        }

        // No further events pending, ie. all events handled?
        if (result == XR_EVENT_UNAVAILABLE)
            return(TRUE);

        // Some event received. Process it:
        switch (event.type) {
            default: {
                if (verbosity > 1)
                    printf("PsychOpenXRCore-WARNING: Event polling for instance %p received unknown event type %i - Not handled.\n", pollInstance, event.type);

                break;
            }

            case XR_TYPE_EVENT_DATA_EVENTS_LOST: {
                XrEventDataEventsLost* lostEvent = (XrEventDataEventsLost*) &event;

                if (verbosity > 1)
                    printf("PsychOpenXRCore-WARNING: Event polling for instance %p lost %i events! Prepare for trouble!\n", pollInstance, lostEvent->lostEventCount);

                break;
            }

            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                XrEventDataInstanceLossPending* lossEvent = (XrEventDataInstanceLossPending*) &event;

                if (verbosity > 0) {
                    printf("PsychOpenXRCore-ERROR: Imminent instance %p loss event received for time %lu! No way to recover! Emergency shutdown!\n", pollInstance, lossEvent->lossTime);
                    fflush(NULL);
                }

                // Note imminent session loss:
                openxr->lossPending = TRUE;

                // The only way to deal with this would be a PsychOpenXRCoreShutDown(), but that might end badly as well!
                return(FALSE);
            }

            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
                XrEventDataInteractionProfileChanged* profileChangeEvent = (XrEventDataInteractionProfileChanged*) &event;
                openxr = PsychGetXRForSession(profileChangeEvent->session);

                if (verbosity > 1)
                    printf("PsychOpenXRCore-WARNING: Event polling for instance %p, interaction profile changed for session %p! TODO HANDLE ME!\n", pollInstance, profileChangeEvent->session);


                break;
            }

            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
                XrEventDataReferenceSpaceChangePending* refSpaceChangeEvent = (XrEventDataReferenceSpaceChangePending*) &event;
                openxr = PsychGetXRForSession(refSpaceChangeEvent->session);

                // Store where the changed reference space origin/pose will be relative to its old location in containing space:
                openxr->originPoseInPreviousSpace = refSpaceChangeEvent->poseInPreviousSpace;

                if (verbosity > 2)
                    printf("PsychOpenXRCore-INFO: Event polling for instance %p, reference space change pending for session %p.\n", pollInstance, refSpaceChangeEvent->session);

                break;
            }

            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                XrEventDataSessionStateChanged* stateEvent = (XrEventDataSessionStateChanged*) &event;
                openxr = PsychGetXRForSession(stateEvent->session);

                if (verbosity > 3)
                    printf("PsychOpenXRCore-INFO: Event polling for instance %p, state change for session %p! From %d ==> %d ", pollInstance, stateEvent->session, openxr->state, stateEvent->state);

                // Update to new state:
                openxr->state = stateEvent->state;

                // Handle transition to new state:
                switch (openxr->state) {
                    // Unknown or idle state: No need for frame presentation loop:
                    case XR_SESSION_STATE_IDLE:
                    case XR_SESSION_STATE_UNKNOWN:
                        openxr->needFrameLoop = FALSE;
                        if (verbosity > 3)
                            printf("[%s]\n", (openxr->state == XR_SESSION_STATE_IDLE) ? "XR_SESSION_STATE_IDLE" : "XR_SESSION_STATE_UNKNOWN");

                        break;

                    // Synchronized, Visible and Focused need frame presentation:
                    case XR_SESSION_STATE_FOCUSED:
                    case XR_SESSION_STATE_SYNCHRONIZED:
                    case XR_SESSION_STATE_VISIBLE:
                        openxr->needFrameLoop = TRUE;
                        if (verbosity > 3)
                            printf("[%s]\n", (openxr->state == XR_SESSION_STATE_SYNCHRONIZED) ? "XR_SESSION_STATE_SYNCHRONIZED" : ((openxr->state == XR_SESSION_STATE_VISIBLE) ? "XR_SESSION_STATE_VISIBLE" : "XR_SESSION_STATE_FOCUSED"));

                        break;

                    // Runtime is ready to start the session:
                    case XR_SESSION_STATE_READY:
                        if (verbosity > 3)
                            printf("[%s]\n", "XR_SESSION_STATE_READY");

                        // Alreay active for some reason?
                        if (!openxr->sessionActive) {
                            // No. Begin session with chosen view configuration:
                            XrSessionBeginInfo sessionBeginInfo = {
                                .type = XR_TYPE_SESSION_BEGIN_INFO,
                                .next = NULL,
                                .primaryViewConfigurationType = openxr->viewType
                            };

                            if (!resultOK(xrBeginSession(openxr->hmd, &sessionBeginInfo))) {
                                // Failed - Error return:
                                if (verbosity > 0)
                                    printf("PsychOpenXRCore-ERROR: Failed to xrBeginSession for session %p of instance %p: %s\n", openxr->hmd, pollInstance, errorString);

                                // Failure return code:
                                return(FALSE);
                            }

                            // Success. Session is now active, frame presentation loop needs to run to
                            // synchronize and keep synchronized with the runtime:
                            openxr->sessionActive = TRUE;
                            openxr->needFrameLoop = TRUE;

                            // PresentExecute() the first time on the main thread to sync us up:
                            PresentExecute(openxr, TRUE);
                        }

                        break;

                    // Runtime wants is to end the session:
                    case XR_SESSION_STATE_STOPPING:
                        if (verbosity > 3)
                            printf("[%s]\n", "XR_SESSION_STATE_STOPPING");

                        // Active?
                        if (openxr->sessionActive) {
                            // End session:
                            if (!resultOK(xrEndSession(openxr->hmd))) {
                                // Failed - Error return:
                                if (verbosity > 0)
                                    printf("PsychOpenXRCore-ERROR: Failed to xrEndSession for session %p of instance %p: %s\n", openxr->hmd, pollInstance, errorString);

                                // Failure return code:
                                return(FALSE);
                            }

                            // Success. Session is now inactive, frame presentation can stop:
                            openxr->sessionActive = FALSE;
                            openxr->needFrameLoop = FALSE;
                        }

                        break;

                    // Something went badly wrong in the runtime/hardware, need to abort:
                    case XR_SESSION_STATE_LOSS_PENDING:
                        if (verbosity > 3)
                            printf("[%s]\n", "XR_SESSION_STATE_LOSS_PENDING");

                        // Mark session inactive, frame presentation must stop:
                        openxr->sessionActive = FALSE;
                        openxr->needFrameLoop = FALSE;

                        // Note imminent session loss:
                        openxr->lossPending = TRUE;

                        // Failure return code:
                        return(FALSE);

                    case XR_SESSION_STATE_EXITING:
                        if (verbosity > 3)
                            printf("[%s]\n", "XR_SESSION_STATE_EXITING");

                        // Mark session inactive, frame presentation must stop:
                        openxr->sessionActive = FALSE;
                        openxr->needFrameLoop = FALSE;

                        // Note user exit request:
                        openxr->userExit = TRUE;

                        break;
                }

                break;
            }
        }
    }

    // Return success if we reach this point:
    return(TRUE);
}

void PsychOpenXRCheckInit(psych_bool dontfail)
{
    unsigned int i;
    XrResult result;
    psych_bool has_XR_FB_display_refresh_rate;

    // Already initialized? No op then.
    if (initialized)
        return;

    // Enumerate and select api layers: TODO Left for future extensions.

    // Enumerate and select all required instance extensions:
    memset(instanceExtensionNames, 0, sizeof(instanceExtensionNames));
    result = xrEnumerateInstanceExtensionProperties(NULL, 0, &instanceExtensionsCount, NULL);
    if (!resultOK(result) || (instanceExtensionsCount == 0)) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to enumerate instance extensions I: %s\n", errorString);
        goto instance_init_out;
    }

    if (verbosity >= 4)
        printf("PsychOpenXRCore-INFO: At startup there are %i OpenXR instance extensions available.\n", instanceExtensionsCount);

    XrExtensionProperties *instanceExtensions = malloc(sizeof(XrExtensionProperties) * instanceExtensionsCount);
    for (i = 0; i < instanceExtensionsCount; i++) {
        instanceExtensions[i].type = XR_TYPE_EXTENSION_PROPERTIES;
        instanceExtensions[i].next = NULL;
    }

    result = xrEnumerateInstanceExtensionProperties(NULL, instanceExtensionsCount, &instanceExtensionsCount, instanceExtensions);
    if (!resultOK(result)) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to enumerate instance extensions II: %s\n", errorString);

        free(instanceExtensions);
        goto instance_init_out;
    }

    // Check if minimum set of required instance extensions exist and enable/request them, if so:
    if (!addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_KHR_OPENGL_ENABLE_EXTENSION_NAME) ||
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_EXT_DEBUG_UTILS_EXTENSION_NAME) ||

        #if defined(XR_USE_PLATFORM_XLIB)
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_KHR_CONVERT_TIMESPEC_TIME_EXTENSION_NAME)
        #endif

        #if defined(XR_USE_PLATFORM_WIN32)
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME_EXTENSION_NAME)
        #endif
    ) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: At least one required instance extension is missing!\n");

        free(instanceExtensions);
        goto instance_init_out;
    }

    // The following extensions are optional. Therefore we don't care if adding them succeeds or not:
    addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME);
    has_XR_FB_display_refresh_rate = addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_FB_DISPLAY_REFRESH_RATE_EXTENSION_NAME);

    // Done enumerating instance extensions:
    free(instanceExtensions);

    XrDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
        .type = XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .next = NULL,
        .messageSeverities = XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                             XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
        .messageTypes = XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
        .userCallback = PsychOpenXRLogCB,
        .userData = NULL
    };

    XrApplicationInfo applicationInfo = {
        .applicationName = PSYCHTOOLBOX_SCRIPTING_LANGUAGE_NAME,
        .applicationVersion = 1,
        .engineName = "PsychOpenXRCore",
        .engineVersion = (PSYCHTOOLBOX_MAJOR_VERSION << 24 | PSYCHTOOLBOX_MINOR_VERSION << 16 | PSYCHTOOLBOX_POINT_VERSION),
        .apiVersion = XR_CURRENT_API_VERSION
    };

    XrInstanceCreateInfo instanceCreateInfo = {
        .type = XR_TYPE_INSTANCE_CREATE_INFO,
        .next = (verbosity > 0) ? &debugCreateInfo : NULL,
        .createFlags = 0,
        .applicationInfo = applicationInfo,
        .enabledApiLayerCount = 0,
        .enabledApiLayerNames = NULL,
        .enabledExtensionCount = instanceExtensionsEnabledCount,
        .enabledExtensionNames = (const char* const *) instanceExtensionNames,
    };

    // Initialize connection to OpenXR runtime by creating our xrInstance:
    result = xrCreateInstance(&instanceCreateInfo, &xrInstance);
    if (resultOK(result)) {
        memset(&instanceProperties, 0, sizeof(instanceProperties));
        instanceProperties.type = XR_TYPE_INSTANCE_PROPERTIES;
        instanceProperties.next = NULL;

        result = xrGetInstanceProperties(xrInstance, &instanceProperties);
        if (!resultOK(result))
            goto instance_init_out;

        if (verbosity >= 3)
            printf("PsychOpenXRCore-INFO: OpenXR runtime \"%s\", version %i.%i.%i initialized.\n", instanceProperties.runtimeName,
                   XR_VERSION_MAJOR(instanceProperties.runtimeVersion), XR_VERSION_MINOR(instanceProperties.runtimeVersion),
                   XR_VERSION_PATCH(instanceProperties.runtimeVersion));

        initialized = TRUE;
    }
    else {
        goto instance_init_out;
    }

    // Bind pfnCreateDebugUtilsMessengerEXT() for debug setup and use it to register our debug callback:
    GET_INSTANCE_PROC_ADDR(xrInstance, xrCreateDebugUtilsMessengerEXT);
    GET_INSTANCE_PROC_ADDR(xrInstance, xrDestroyDebugUtilsMessengerEXT);
    if (pxrCreateDebugUtilsMessengerEXT && pxrDestroyDebugUtilsMessengerEXT) {
        if ((!resultOK(pxrCreateDebugUtilsMessengerEXT(xrInstance, &debugCreateInfo, &debugMessenger))) && (verbosity > 1))
            printf("PsychOpenXRCore-WARNING: Failed to create debug messenger for debugging: %s\n", errorString);
    }

    // Bind OpenGL setup functions:
    GET_INSTANCE_PROC_ADDR(xrInstance, xrGetOpenGLGraphicsRequirementsKHR);

    // Bind optional refresh rate query and control functions:
    if (has_XR_FB_display_refresh_rate) {
        GET_INSTANCE_PROC_ADDR(xrInstance, xrGetDisplayRefreshRateFB);
    }

    #if defined(XR_USE_PLATFORM_WIN32)
        GET_INSTANCE_PROC_ADDR(xrInstance, xrConvertWin32PerformanceCounterToTimeKHR);
        GET_INSTANCE_PROC_ADDR(xrInstance, xrConvertTimeToWin32PerformanceCounterKHR);
    #endif

    #if defined(XR_USE_PLATFORM_XLIB)
        GET_INSTANCE_PROC_ADDR(xrInstance, xrConvertTimespecTimeToTimeKHR);
        GET_INSTANCE_PROC_ADDR(xrInstance, xrConvertTimeToTimespecTimeKHR);
    #endif

    // Perform initial XR hardware enumeration:
    enumerateXRDevices(xrInstance);

    if (verbosity >= 3)
        printf("PsychOpenXRCore-INFO: At startup there are %i OpenXR devices available.\n", numAvailableDevices);

    // Success return:
    return;

    instance_init_out:

    instanceExtensionsEnabledCount = 0;

    if (!dontfail) {
        printf("PsychOpenXRCore-ERROR: OpenXR runtime initialization failed: %s\n", errorString);
        PsychErrorExitMsg(PsychError_system, "PsychOpenXRCore-ERROR: Initialization of XR runtime failed. Driver disabled!");
    }
}

// TODO
void PsychOpenXRStop(int handle)
{
    int rc;
    PsychOpenXRDevice* openxr;
    openxr = PsychGetXR(handle, TRUE);
    if (NULL == openxr || !openxr->isTracking) return;

    if (verbosity >= 4) printf("PsychOpenXRCore-INFO: Tracking stopped on device with handle %i.\n", handle);

    PsychLockMutex(&(openxr->presenterLock));

    // Mark tracking as stopped:
    openxr->isTracking = FALSE;

    // Need to start presenterThread if this is the first Present operation:
    if ((openxr->multiThreaded) && (openxr->presenterThread == (psych_thread) NULL)) {
        // Create and startup thread:
        if ((rc = PsychCreateThread(&(openxr->presenterThread), NULL, PresenterThreadMain, (void*) openxr))) {
            PsychUnlockMutex(&(openxr->presenterLock));
            openxr->presenterThread = (psych_thread) NULL;
            printf("PsychOpenXRCore-ERROR: Could not create internal presenterThread  [%s].\n", strerror(rc));
            PsychErrorExitMsg(PsychError_system, "Insufficient system resources for thread creation as part of XR compositor init!");
        }
    }

    PsychUnlockMutex(&(openxr->presenterLock));

    return;
}

// TODO
void PsychOpenXRClose(int handle)
{
    int rc;
    PsychOpenXRDevice* openxr = PsychGetXR(handle, TRUE);
    if (openxr) {
        // presenterThread shutdown: Ask thread to terminate, wait for thread termination, cleanup and release the thread:
        PsychLockMutex(&(openxr->presenterLock));
        openxr->closing = TRUE;
        PsychUnlockMutex(&(openxr->presenterLock));

        if (openxr->presenterThread != (psych_thread) NULL) {
            if (verbosity > 5)
                printf("PTB-DEBUG: Waiting (join()ing) for helper thread of HMD %p to finish up. If this doesn't happen quickly, you'll have to kill Octave...\n", openxr);

            PsychDeleteThread(&(openxr->presenterThread));
        }

        // Ok, thread is dead. Mark it as such:
        openxr->presenterThread = (psych_thread) NULL;
        openxr->closing = FALSE;

        // PresentExecute() a last time on the main thread:
        PresentExecute(openxr, FALSE);

        // Destroy/Release texture swap chains to XR compositor:
        if (openxr->textureSwapChain[0]) {
            xrDestroySwapchain(openxr->textureSwapChain[0]);
            openxr->textureSwapChain[0] = XR_NULL_HANDLE;

            free(openxr->textureSwapChainImages[0]);
            openxr->textureSwapChainImages[0] = NULL;
            openxr->textureSwapChainLength[0] = 0;
        }

        if (openxr->isStereo && openxr->textureSwapChain[1]) {
            xrDestroySwapchain(openxr->textureSwapChain[1]);
            openxr->textureSwapChain[1] = XR_NULL_HANDLE;

            free(openxr->textureSwapChainImages[1]);
            openxr->textureSwapChainImages[1] = NULL;
            openxr->textureSwapChainLength[1] = 0;
        }

        // Close the HMD aka XrSession:
        if (openxr->hmd)
            xrDestroySession(openxr->hmd);

        openxr->hmd = XR_NULL_HANDLE;
        openxr->opened = FALSE;

        if ((rc = PsychDestroyMutex(&(openxr->presenterLock)))) {
            printf("PsychOpenXRCore-WARNING: In PsychOpenXRClose(): Could not destroy presenterLock mutex lock [%s].\n", strerror(rc));
            printf("PsychOpenXRCore-WARNING: This will cause resource leakage. Maybe you should better exit and restart Octave?");
        }

        if ((rc=PsychDestroyCondition(&(openxr->presentedSignal)))) {
            printf("PsychOpenXRCore-WARNING: In PsychOpenXRClose(): Could not destroy presentedSignal condition variable [%s].\n", strerror(rc));
            printf("PsychOpenXRCore-WARNING: This will cause resource leakage. Maybe you should better exit and restart Octave?");
        }

        if (verbosity >= 4) printf("PsychOpenXRCore-INFO: Closed OpenXR HMD with handle %i.\n", handle);

        // Done with this device:
        devicecount--;
    }

    if (initialized && (devicecount == 0)) {
        // Last HMD closed. Shutdown the runtime:
        if (debugMessenger)
            pxrDestroyDebugUtilsMessengerEXT(debugMessenger);

        debugMessenger = XR_NULL_HANDLE;

        if (xrInstance != XR_NULL_HANDLE) {
            xrDestroyInstance(xrInstance);
            xrInstance = XR_NULL_HANDLE;
        }

        instanceExtensionsEnabledCount = 0;
        initialized = FALSE;

        if (verbosity >= 4) printf("PsychOpenXRCore-INFO: OpenXR runtime shutdown complete.\n");
    }
}

void PsychOpenXRCoreInit(void) {
    int handle;

    for (handle = 0 ; handle < MAX_PSYCH_OPENXR_DEVS; handle++) {
        memset(&openxrdevices[handle], 0, sizeof(openxrdevices[0]));
        openxrdevices[handle].opened = FALSE;
        openxrdevices[handle].hmd = XR_NULL_HANDLE;
    }

    for (handle = 0 ; handle < MAX_PSYCH_OPENXR_DEVS; handle++) {
        memset(&availableSystems[handle], 0, sizeof(availableSystems[0]));
        availableSystems[handle].type = XR_TYPE_SYSTEM_PROPERTIES;
    }

    numAvailableDevices = 0;
    devicecount = 0;
    initialized = FALSE;
}

PsychError PsychOpenXRCoreShutDown(void) {
    int handle;

    if (initialized) {
        for (handle = 0 ; handle < MAX_PSYCH_OPENXR_DEVS; handle++)
            PsychOpenXRClose(handle);
    }

    return(PsychError_none);
}

PsychError PSYCHOPENXRVerbosity(void)
{
    static char useString[] = "oldVerbosity = PsychOpenXRCore('Verbosity' [, verbosity]);";
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

PsychError PSYCHOPENXRGetCount(void)
{
    static char useString[] = "numHMDs = PsychOpenXRCore('GetCount');";
    static char synopsisString[] =  "Returns count of currently connected HMDs.\n"
                                    "Returns -1 if the runtime or server couldn't get initialized.\n";
    static char seeAlsoString[] = "Open";

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Redetect/enumerate and return number of devices detected:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, enumerateXRDevices(xrInstance));

    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXROpen(void)
{
    static char useString[] = "[openxrPtr, modelName, runtimeName] = PsychOpenXRCore('Open' [, deviceIndex=0][, multiThreaded=0]);";
    //                          1          2          3                                        1                2
    static char synopsisString[] =
        "Open connection to OpenXR HMD, return a 'openxrPtr' handle to it.\n\n"
        "The call tries to open the HMD with index 'deviceIndex', or the first detected "
        "HMD if 'deviceIndex' is omitted. Please note that currently only one single HMD "
        "is supported by OpenXR-1, so this 'deviceIndex' is redundant at the moment, given "
        "that zero is the only valid value.\n"
        "'multiThreaded' if provided as non-zero value, will use an asynchronous presenter thread "
        "to improve stimulus scheduling. Highly experimental: Does not work in many cases!\n"
        "The returned handle can be passed to the other subfunctions to operate the device.\n"
        "'modelName' returns the model name string of the OpenXR device.\n"
        "'runtimeName' returns the name of the OpenXR runtime.\n";
    static char seeAlsoString[] = "GetCount Close";

    XrResult result;
    PsychOpenXRDevice* openxr;
    int deviceIndex = 0;
    int handle = 0;
    int multiThreaded = 0;
    int rc;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Find a free device slot:
    for (handle = 0; (handle < MAX_PSYCH_OPENXR_DEVS) && openxrdevices[handle].opened; handle++);
    if ((handle >= MAX_PSYCH_OPENXR_DEVS) || openxrdevices[handle].opened)
        PsychErrorExitMsg(PsychError_internal, "Maximum number of simultaneously open OpenXR devices reached.");

    // Get optional OpenXR device index:
    PsychCopyInIntegerArg(1, kPsychArgOptional, &deviceIndex);
    if (deviceIndex < 0)
        PsychErrorExitMsg(PsychError_user, "Invalid 'deviceIndex' provided. Must be greater than or equal to zero.");

    // Get optional multiThreaded arg:
    PsychCopyInIntegerArg(2, kPsychArgOptional, &multiThreaded);
    if ((multiThreaded != 0) && (multiThreaded != 1))
        PsychErrorExitMsg(PsychError_user, "Invalid 'multiThreaded' flag provided. Must be 0 or 1.");

    if (-1 == enumerateXRDevices(xrInstance)) {
        if (verbosity >= 2)
            printf("PsychOpenXRCore-WARNING: Could not connect to OpenXR server process yet. Did you forget to start it?\n");
    }

    if (deviceIndex >= numAvailableDevices) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Invalid deviceIndex %i >= number of available HMDs %i.\n", deviceIndex, numAvailableDevices);

        PsychErrorExitMsg(PsychError_user, "Invalid 'deviceIndex' provided. Not enough HMDs available!");
    }

    // Zero init device structure:
    memset(&openxrdevices[handle], 0, sizeof(PsychOpenXRDevice));

    // Get device pointer:
    openxr = &openxrdevices[handle];

    // Try to create and setup a xrSession for the xrSystem device with 'deviceIndex':
    openxr->systemId = availableSystems[deviceIndex].systemId;

    // Use a fixed stereo view type by default for now, for typical HMD use:
    openxr->viewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;

    // Stats for nerds:
    if (verbosity >= 3) {
        printf("PsychOpenXRCore-INFO: Opened OpenXR device with deviceIndex %i as handle %i.\n", deviceIndex, handle + 1);
        printf("PsychOpenXRCore-INFO: Product: \"%s\" - [VendorId: 0x%x]\n", availableSystems[deviceIndex].systemName, availableSystems[deviceIndex].vendorId);
        printf("PsychOpenXRCore-INFO: ----------------------------------------------------------------------------------\n");
    }

    // Assign multi-threading mode:
    openxr->multiThreaded = (psych_bool) multiThreaded;

    // Initialize the mutex lock:
    if ((rc = PsychInitMutex(&(openxr->presenterLock)))) {
        printf("PsychOpenXRCore-ERROR: Could not create internal presenterLock mutex lock [%s].\n", strerror(rc));
        PsychErrorExitMsg(PsychError_system, "Insufficient system resources for mutex creation as part of HMD open operation!");
    }

    if ((rc = PsychInitCondition(&(openxr->presentedSignal), NULL))) {
        printf("PsychOpenXRCore-ERROR: Could not create internal presentedSignal condition variable [%s].\n", strerror(rc));
        PsychErrorExitMsg(PsychError_system, "Insufficient system ressources for condition variable creation as part of HMD open operation!");
    }

    // Mark device as open:
    openxr->opened = TRUE;

    // Session state unknown/undefined for not yet created session:
    openxr->state = XR_SESSION_STATE_UNKNOWN;

    // Increment count of open devices:
    devicecount++;

    // Return device handle: We use 1-based handle indexing to make life easier for Octave/Matlab:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, handle + 1);

    // Return product name:
    PsychCopyOutCharArg(2, kPsychArgOptional, (const char*) availableSystems[deviceIndex].systemName);

    // Return OpenXR runtime name:
    PsychCopyOutCharArg(3, kPsychArgOptional, (const char*) instanceProperties.runtimeName);

    return(PsychError_none);
}

PsychError PSYCHOPENXRClose(void)
{
    static char useString[] = "PsychOpenXRCore('Close' [, openxrPtr]);";
    //                                                    1
    static char synopsisString[] =
        "Close connection to OpenXR device 'openxrPtr' if that device is open and active.\n"
        "If the optional 'openxrPtr' is omitted, then close all open devices. Once the last "
        "open device has been closed, shutdown the driver, ie. perform the same cleanup as "
        "if 'clear PsychOpenXRCore' would be executed.\n";
    static char seeAlsoString[] = "Open";

    int handle = -1;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get optional device handle:
    PsychCopyInIntegerArg(1, kPsychArgOptional, &handle);

    if (handle >= 1) {
        // Close device:
        PsychOpenXRClose(handle);
    }
    else {
        // No handle provided: Close all devices, shutdown driver.
        PsychOpenXRCoreShutDown();
    }

    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRTrackingOriginType(void)
{
    static char useString[] = "oldType = PsychOpenXRCore('TrackingOriginType', openxrPtr [, newType]);";
    //                         1                                                  1            2
    static char synopsisString[] =
        "Specify the type of tracking origin for OpenXR device 'openxrPtr'.\n\n"
        "This returns the current type of tracking origin in 'oldType'.\n\n"
        "Optionally you can specify a new tracking origin type as 'newType'. "
        "Type must be either:\n\n"
        "0 = Origin is at eye height (HMD height).\n"
        "1 = Origin is at floor height.\n\n"
        "The eye height or floor height gets defined by the system during "
        "calls to 'RecenterTrackingOrigin' and during sensor calibration in "
        "the OpenXR GUI application.\n";
    static char seeAlsoString[] = "RecenterTrackingOrigin GetTrackersState";

    int handle;
    int originType;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);
/*
    // Query and return old setting:
    originType = (int) ovr_GetTrackingOriginType(openxr->hmd);
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) originType);

    // New origin type provided?
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &originType)) {
        if (originType < 0 || originType > 1)
            PsychErrorExitMsg(PsychError_user, "Invalid 'newType' for tracking origin type specified. Must be 0 or 1.");

        if (OVR_FAILURE(ovr_SetTrackingOriginType(openxr->hmd, (ovrTrackingOrigin) originType))) {
            if (verbosity > 0) {
                ovr_GetLastErrorInfo(&errorInfo);
                printf("PsychOpenXRCore-ERROR: Setting new tracking origin type failed! %s\n", errorInfo.ErrorString);
            }
            PsychErrorExitMsg(PsychError_user, "Setting new tracking origin type failed.");
        }
        else {
            if (verbosity > 3)
                printf("PsychOpenXRCore-INFO: Set new tracking origin type for HMD to %i.\n", originType);
        }
    }
*/
    return(PsychError_none);
}

PsychError PSYCHOPENXRStart(void)
{
    static char useString[] = "PsychOpenXRCore('Start', openxrPtr);";
    //                                                     1
    static char synopsisString[] =
        "Start head orientation and position tracking operation on OpenXR device 'openxrPtr'.\n\n";
    static char seeAlsoString[] = "Stop";

    int handle;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    if (openxr->isTracking) {
        if (verbosity >= 0)
            printf("PsychOpenXRCore-ERROR: Tried to start tracking on device %i, but tracking is already started.\n", handle);
        PsychErrorExitMsg(PsychError_user, "Tried to start tracking on HMD, but tracking already active.");
    }

    if (verbosity >= 4)
        printf("PsychOpenXRCore-INFO: Tracking started on device with handle %i.\n", handle);

    // presenterThread shutdown: Ask thread to terminate, wait for thread termination, cleanup and release the thread:
    PsychLockMutex(&(openxr->presenterLock));
    openxr->closing = TRUE;
    PsychUnlockMutex(&(openxr->presenterLock));

    if (openxr->presenterThread != (psych_thread) NULL) {
        if (verbosity > 5)
            printf("PTB-DEBUG: Waiting (join()ing) for helper thread of HMD %p to finish up. If this doesn't happen quickly, you'll have to kill Octave...\n", openxr);

        PsychDeleteThread(&(openxr->presenterThread));
    }

    // Ok, thread is dead. Mark it as such:
    openxr->presenterThread = (psych_thread) NULL;
    openxr->closing = FALSE;
    openxr->isTracking = TRUE;

    // Tracking is running.
    return(PsychError_none);
}

PsychError PSYCHOPENXRStop(void)
{
    static char useString[] = "PsychOpenXRCore('Stop', openxrPtr);";
    static char synopsisString[] =
        "Stop head tracking operation on OpenXR device 'openxrPtr'.\n\n";
    static char seeAlsoString[] = "Start";

    int handle;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);

    // Stop device:
    PsychOpenXRStop(handle);

    return(PsychError_none);
}

PsychError PSYCHOPENXRGetTrackingState(void)
{
    static char useString[] = "[state, touch] = PsychOpenXRCore('GetTrackingState', openxrPtr [, predictionTime=nextFrame]);";
    //                          1      2                                            1            2
    static char synopsisString[] =
        "Return current state of eye position and orientation tracking for OpenXR device 'openxrPtr'.\n"
        "Position and orientation is predicted for target time 'predictionTime' in seconds if provided, "
        "based on the latest measurements from the tracking hardware. If 'predictionTime' is omitted or zero, "
        "then the prediction is performed for the mid-point of the next possible video frame of the HMD, ie. "
        "the most likely presentation time for immediately rendered images.\n\n"
        "'state' is a struct with fields reporting the following values:\n"
        "'Time' = Time in seconds of returned tracking state.\n"
        "'Status' = Tracking status flags:\n"
        "+1 = Orientation tracked for all eyes,\n"
        "+2 = Position tracked for all eyes,\n"
        "+4 = At least part of the pose is somewhat valid, even if not tracked, but just inferred,\n"
        "+128 = HMD display is connected, available and actually on users head, displaying our content.\n\n"
        "'SessionState' = VR session status flags, added together:\n"
        "+1  = Our rendering goes to the HMD, ie. we have control over it. If some other app would "
        "be in control, this flag would be missing.\n"
        "+2  = HMD is present and active.\n"
        "+4  = HMD is strapped onto users head. E.g., a Rift CV1 would switch off/blank if not on the head.\n"
        "+8  = DisplayLost condition! Some hardware/software malfunction, need to completely quit to recover.\n"
        "+16 = ShouldQuit The user interface asks us to voluntarily terminate this session.\n\n"
        "'CalibratedOrigin' = The pose of the world coordinate system origin during last recalibration, relative "
        "to its previous pose, as a vector [x,y,z,rx,ry,rz,rw].\n\n"
        "'EyePoseLeft' = Vector with position and orientation of left eye / left eye virtual camera.\n"
        "'EyePoseRight' = Vector with position and orientation of right eye / right eye virtual camera.\n"
        "The vectors are of form [tx, ty, tz, rx, ry, rz, rw] - A 3 component 3D position, followed by a 4 "
        "component rotation quaternion.\n\n"
        "\n"
        "Touch controller position and orientation:\n\n"
        "The return argument 'touch' is a struct array with 2 structs. touch(1) contains info about "
        "the tracking state and tracked pose of the left hand (= left touch controller) of the user, "
        "touch(2) contains info about the right hand (= right touch controller) of the user.\n"
        "The structs have very similar structure to the head (= HMD) tracking data returned by 'state':\n\n"
        "'Time' = Time in seconds of returned hand/controller tracking state.\n"
        "'Status' = Tracking status flags:\n"
        " 0 = No tracking info for hand/controller, ie. no OpenXR touch sensor connected.\n"
        "+1 = Hand orientation tracked,\n"
        "+2 = Hand position tracked,\n"
        "+4 = Hand linear velocity available,\n"
        "+8 = Hand angular velocity available,\n"
        "'HandPose' = Position and orientation of the hand, in usual [x,y,z,rx,ry,rz,rw] vector form.\n"
        "'HandLinearSpeed' = Hand linear velocity [vx,vy,vz] in meters/sec.\n"
        "'HandAngularSpeed' = Hand angular velocity [rx,ry,rz] in radians/sec.\n"
        "\n";
    static char seeAlsoString[] = "Start Stop GetTrackersState GetInputState";

    PsychGenericScriptType *status;
    const char *FieldNames1[] = { "Time", "Status", "SessionState", "CalibratedOrigin", "EyePoseLeft", "EyePoseRight" };
    const int FieldCount1 = 6;
    const char *FieldNames2[] = { "Time", "Status", "HandPose", "HandLinearSpeed", "HandAngularSpeed" };
    const int FieldCount2 = 5;

    int handle, i;
    PsychOpenXRDevice *openxr;
    double predictionTime;
    XrTime xrPredictionTime;
    PsychGenericScriptType *outMat;
    double *v;
    int StatusFlags = 0;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get optional target time for predicted tracking state. Default to the
    // predicted state for the predicted mid-point of the next video frame:
    if (PsychCopyInDoubleArg(2, kPsychArgOptional, &predictionTime)) {
        xrPredictionTime = PsychTimeToXrTime(predictionTime);
    }
    else {
        xrPredictionTime = openxr->frameState.predictedDisplayTime;
        predictionTime = XrTimeToPsychTime(xrPredictionTime);
    }

    // Do the actual eye pose / view update from tracking + prediction:
    // Also updates openxr->viewState with its status flags.
    PsychLockMutex(&(openxr->presenterLock));
    locateXRViews(openxr, xrPredictionTime);
    syncXRActions(openxr);
    PsychUnlockMutex(&(openxr->presenterLock));

    // Print out tracking status:
    if (verbosity >= 4)
        printf("PsychOpenXRCore-INFO: Tracking state predicted for device %i at time %f. Status = %i\n", handle, predictionTime, openxr->viewState.viewStateFlags);

    PsychAllocOutStructArray(1, kPsychArgOptional, -1, FieldCount1, FieldNames1, &status);

    // Timestamp for when this prediction is valid:
    openxr->sensorSampleTime = predictionTime;
    PsychSetStructArrayDoubleElement("Time", 0, predictionTime, status);

    // HMD tracking status:
    StatusFlags = 0;

    // Active orientation tracking?
    if (openxr->viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT && openxr->viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_TRACKED_BIT)
        StatusFlags |= 1;

    // Active position tracking?
    if (openxr->viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT && openxr->viewState.viewStateFlags & XR_VIEW_STATE_POSITION_TRACKED_BIT)
        StatusFlags |= 2;

    // Pose at least somewhat valid, ie. orientation or position valid? Could also be just inferred, not tracked, but is considered somewhat valid/useful:
    if (openxr->viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT || openxr->viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT)
        StatusFlags |= 4;

    // HMD present, connected and online and on users head, displaying us?
    if (openxr->state == XR_SESSION_STATE_VISIBLE || openxr->state == XR_SESSION_STATE_FOCUSED)
        StatusFlags |= 128;

    // Return head and general tracking status flags:
    PsychSetStructArrayDoubleElement("Status", 0, StatusFlags, status);

    // Return sesstion status flags:
    StatusFlags = 0;
    if (openxr->state == XR_SESSION_STATE_VISIBLE || openxr->state == XR_SESSION_STATE_FOCUSED) StatusFlags |= 1;
    if (openxr->needFrameLoop) StatusFlags |= (2 + 4);
    if (openxr->lossPending) StatusFlags |= 8;
    if (openxr->userExit) StatusFlags |= 16;

    PsychSetStructArrayDoubleElement("SessionState", 0, StatusFlags, status);

    // CalibratedOrigin:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);
    v[0] = openxr->originPoseInPreviousSpace.position.x;
    v[1] = openxr->originPoseInPreviousSpace.position.y;
    v[2] = openxr->originPoseInPreviousSpace.position.z;

    v[3] = openxr->originPoseInPreviousSpace.orientation.x;
    v[4] = openxr->originPoseInPreviousSpace.orientation.y;
    v[5] = openxr->originPoseInPreviousSpace.orientation.z;
    v[6] = openxr->originPoseInPreviousSpace.orientation.w;
    PsychSetStructArrayNativeElement("CalibratedOrigin", 0, outMat, status);

    // Left eye pose as raw data:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);

    // Position (x,y,z):
    v[0] = openxr->view[0].pose.position.x;
    v[1] = openxr->view[0].pose.position.y;
    v[2] = openxr->view[0].pose.position.z;

    // Orientation as a quaternion (x,y,z,w):
    v[3] = openxr->view[0].pose.orientation.x;
    v[4] = openxr->view[0].pose.orientation.y;
    v[5] = openxr->view[0].pose.orientation.z;
    v[6] = openxr->view[0].pose.orientation.w;
    PsychSetStructArrayNativeElement("EyePoseLeft", 0, outMat, status);

    // Right eye pose as raw data:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);

    // Position (x,y,z):
    v[0] = openxr->view[1].pose.position.x;
    v[1] = openxr->view[1].pose.position.y;
    v[2] = openxr->view[1].pose.position.z;

    // Orientation as a quaternion (x,y,z,w):
    v[3] = openxr->view[1].pose.orientation.x;
    v[4] = openxr->view[1].pose.orientation.y;
    v[5] = openxr->view[1].pose.orientation.z;
    v[6] = openxr->view[1].pose.orientation.w;
    PsychSetStructArrayNativeElement("EyePoseRight", 0, outMat, status);

    // Now the tracking info from the OpenXR touch controllers 0 and 1 for left
    // and right hand, in a separate struct array:
    PsychAllocOutStructArray(2, kPsychArgOptional, 2, FieldCount2, FieldNames2, &status);

    XrSpaceLocation handPoses[2] = { 0 };
    XrSpaceVelocity handVelocity[2] = { 0 };

    for (i = 0; i < 2; i++) {
        handVelocity[i].type = XR_TYPE_SPACE_VELOCITY;
        handVelocity[i].next = NULL;
        handPoses[i].type = XR_TYPE_SPACE_LOCATION;
        handPoses[i].next = &handVelocity[i];

        if (!resultOK(xrLocateSpace(openxr->handPoseSpace[i], openxr->worldSpace, xrPredictionTime, &handPoses[i]))) {
            if (verbosity > 3)
                printf("PsychOpenXRCore-DEBUG: xrLocateSpace() failed: %s\n", errorString);

            // Indicate tracking failure:
            handPoses[i].locationFlags = 0;
            handVelocity[i].velocityFlags = 0;
        }

        // Timestamp for when this tracking info is valid:
        PsychSetStructArrayDoubleElement("Time", i, predictionTime, status);

        // Hand / touch controller tracking state:
        StatusFlags = 0;

        // Active orientation tracking?
        if (handPoses[i].locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT && handPoses[i].locationFlags & XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT)
            StatusFlags |= 1;

        // Active position tracking?
        if (handPoses[i].locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT && handPoses[i].locationFlags & XR_SPACE_LOCATION_POSITION_TRACKED_BIT)
            StatusFlags |= 2;

        // Valid hand linear velocity?
        if (handVelocity[i].velocityFlags & XR_SPACE_VELOCITY_LINEAR_VALID_BIT)
            StatusFlags |= 4;

        // Valid hand angular velocity?
        if (handVelocity[i].velocityFlags & XR_SPACE_VELOCITY_ANGULAR_VALID_BIT)
            StatusFlags |= 8;

        PsychSetStructArrayDoubleElement("Status", i, StatusFlags, status);

        // Hand pose:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);
        v[0] = handPoses[i].pose.position.x;
        v[1] = handPoses[i].pose.position.y;
        v[2] = handPoses[i].pose.position.z;

        v[3] = handPoses[i].pose.orientation.x;
        v[4] = handPoses[i].pose.orientation.y;
        v[5] = handPoses[i].pose.orientation.z;
        v[6] = handPoses[i].pose.orientation.w;
        PsychSetStructArrayNativeElement("HandPose", i, outMat, status);

        // Linear velocity:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
        v[0] = handVelocity[i].linearVelocity.x;
        v[1] = handVelocity[i].linearVelocity.y;
        v[2] = handVelocity[i].linearVelocity.z;
        PsychSetStructArrayNativeElement("HandLinearSpeed", i, outMat, status);

        // Angular velocity:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
        v[0] = handVelocity[i].angularVelocity.x;
        v[1] = handVelocity[i].angularVelocity.y;
        v[2] = handVelocity[i].angularVelocity.z;
        PsychSetStructArrayNativeElement("HandAngularSpeed", i, outMat, status);
    }

    return(PsychError_none);
}


PsychError PSYCHOPENXRGetInputState(void)
{
    static char useString[] = "input = PsychOpenXRCore('GetInputState', openxrPtr, controllerType);";
    //                         1                                        1          2
    static char synopsisString[] =
        "Return current state of input device 'controllerType' associated with OpenXR device 'openxrPtr'.\n\n"
        "'controllerType' can be one of the following values:\n"
        "OVR.ControllerType_LTouch = Left touch controller (Left tracked hand).\n"
        "OVR.ControllerType_RTouch = Right touch controller (Right tracked hand).\n"
        "OVR.ControllerType_Remote = Connected remote control or similar, e.g., control buttons on HMD.\n"
        "OVR.ControllerType_XBox = Microsoft XBox controller or some equivalent gamepad.\n"
        "OVR.ControllerType_Active = Whatever controller is connected and active.\n"
        "\n"
        "'input' is a struct with fields reporting the following status values of the controller:\n"
        "'Valid' Reports 1 if 'input' contains any valid results, or 0 if input is completely unavailable, "
        "e.g., because there isn't any input hardware connected and active, or Psychtoolbox lost XR input focus.\n"
        "'ActiveInputs' = Bitmask of which 'input' contains valid results, or 0 if input completely unavailable.\n"
        "The following flags will be logical or'ed together if the corresponding input category is valid, "
        "ie. provided with actual input date from some physical input source element, controller etc.:\n"
        "+1  = 'Buttons' gets input from some real buttons or switches.\n"
        "+2  = 'Touches' gets input from some real touch/proximity sensors or gesture recognizers.\n"
        "+4  = 'Trigger' gets input from some real analog trigger sensor or gesture recognizer.\n"
        "+8  = 'Grip' gets input from some real analog grip sensor or gesture recognizer.\n"
        "+16 = 'Thumbstick' gets input from some real thumbstick, joystick or trackpad or similar 2D sensor.\n"
        "+32 = 'Thumbstick2' gets input from some real secondary thumbstick, joystick or trackpad or similar 2D sensor.\n"
        "\n"
        "'Time' = Time in seconds when controller state was last updated.\n"
        "'Buttons' = Vector with each positions value corresponding to a specifc button being pressed (1) "
        "or released (0). The OVR.Button_XXX constants map button names to vector indices (like KbName() "
        "does for KbCheck()).\n"
        "'Touches' = Vector with touch values as described by the OVR.Touch_XXX constants. Works like 'Buttons'.\n"
        "'Trigger'(1/2) = Left (1) and Right (2) trigger: Value range 0.0 - 1.0.\n"
        "'Grip'(1/2) = Left (1) and Right (2) grip button: Value range 0.0 - 1.0.\n"
        "'Thumbstick' = 2x2 matrix: Column 1 contains left thumbsticks [x ; y] axis values, column 2 contains "
        "right sticks [x ; y] axis values. Values are in range -1 to +1. Note that some controllers do not have "
        "thumbsticks, but trackpads instead. These would be exposed as Thumbstick as well, being 2D input.\n"
        "'Thumbstick2' = 2x2 matrix: Column 1 contains left thumbsticks [x ; y] axis values, column 2 contains "
        "right thumbsticks [x ; y] axis values. Values are in range -1 to +1. Only a few controllers have a 2nd "
        "thumbstick for each hand, and it is often a trackpad instead of a thumbstick.\n"
        "\n";
    static char seeAlsoString[] = "Start Stop GetTrackingState GetTrackersState";

    PsychGenericScriptType *status;
    const char *FieldNames[] = { "Valid", "ActiveInputs", "Time", "Buttons", "Touches", "Trigger", "Grip", "Thumbstick", "Thumbstick2" };
    const int FieldCount = 9;

    int handle;
    PsychOpenXRDevice *openxr;
    psych_int64 controllerType;
    XrPath path;
    PsychGenericScriptType *outMat;
    int i;
    double *v;
    XrTime time = 0;
    int valid = 0;

    XrActionStateBoolean bv = {
        .type = XR_TYPE_ACTION_STATE_BOOLEAN,
        .next = NULL,
    };

    XrActionStateFloat fv = {
        .type = XR_TYPE_ACTION_STATE_FLOAT,
        .next = NULL,
    };

    XrActionStateVector2f vv = {
        .type = XR_TYPE_ACTION_STATE_VECTOR2F,
        .next = NULL,
    };

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get controller type:
    PsychCopyInIntegerArg64(2, kPsychArgRequired, &controllerType);
    switch (controllerType) {
        case 1: // Left hand controller? OVR.ControllerType_LTouch
            path = openxr->handPath[0];
            break;

        case 2: // Right hand controller? OVR.ControllerType_RTouch
            path = openxr->handPath[1];
            break;

        case 4: // Remote control or equivalent? OVR.ControllerType_Remote
            path = openxr->handPath[3];
            break;

        case 16: // Gamepad / Microsoft X-Box controller or equivalent? OVR.ControllerType_XBox
            path = openxr->handPath[2];
            break;

        case 0xffffffff: // All active controllers? OVR.ControllerType_Active
            // XR_NULL_PATH queries all active devices:
            path = XR_NULL_PATH;
            break;

        default:
            printf("PsychOpenXRCore-ERROR: 'GetInputState' invalid controllerType 0x%x.\n", (unsigned int) controllerType);
            PsychErrorExitMsg(PsychError_user, "Invalid controllerType for GetInputState query specified.");
    }

    XrActionStateGetInfo actionStateGetInfo = {
        .type = XR_TYPE_ACTION_STATE_GET_INFO,
        .next = NULL,
        .action = XR_NULL_HANDLE,
        .subactionPath = path,
    };

    // Prepare output struct "input":
    PsychAllocOutStructArray(1, kPsychArgOptional, -1, FieldCount, FieldNames, &status);

    // Session has XR input focus, so try to get input state:
    PsychLockMutex(&(openxr->presenterLock));
    syncXRActions(openxr);
    PsychUnlockMutex(&(openxr->presenterLock));

    // All input states updated by syncXRActions(). Query current values:

    // Button states:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 32, 1, &v, &outMat);
    for (i = 0; i < NUM_OVR_BUTTONS; i++) {
        // Skip indices without actual actions assigned:
        if (openxr->buttonAction[i] == XR_NULL_HANDLE) {
            v[i] = 0;
            continue;
        }

        actionStateGetInfo.action = openxr->buttonAction[i];
        if (!resultOK(xrGetActionStateBoolean(openxr->hmd, &actionStateGetInfo, &bv))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: 'Button' query %i failed: %s\n", i, errorString);

            PsychErrorExitMsg(PsychError_system, "GetInputState query failed.");
        }

        // Meaningful result for this component from active input source?
        if (bv.isActive) {
            valid |= 1;
            v[i] = (double) bv.currentState;
            time = (bv.lastChangeTime > time) ? bv.lastChangeTime : time;
            if (verbosity > 6)
                printf("PsychOpenXRCore-DEBUG: 'Button' query %i = %i\n", i, (int) v[i]);
        }
        else {
            v[i] = 0;
        }
    }

    PsychSetStructArrayNativeElement("Buttons", 0, outMat, status);

    // Touch states:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 32, 1, &v, &outMat);
    for (i = 0; i < NUM_OVR_TOUCHES; i++) {
        // Skip indices without actual actions assigned:
        if (openxr->touchAction[i] == XR_NULL_HANDLE) {
            v[i] = 0;
            continue;
        }

        actionStateGetInfo.action = openxr->touchAction[i];
        if (!resultOK(xrGetActionStateBoolean(openxr->hmd, &actionStateGetInfo, &bv))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: 'Touches' query %i failed: %s\n", i, errorString);

            PsychErrorExitMsg(PsychError_system, "GetInputState query failed.");
        }

        // Meaningful result for this component from active input source?
        if (bv.isActive) {
            valid |= 2;
            v[i] = (double) bv.currentState;
            time = (bv.lastChangeTime > time) ? bv.lastChangeTime : time;
            if (verbosity > 6)
                printf("PsychOpenXRCore-DEBUG: 'Touches' query %i = %i\n", i, (int) v[i]);
        }
        else {
            v[i] = 0;
        }
    }

    PsychSetStructArrayNativeElement("Touches", 0, outMat, status);

    // Trigger left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    for (i = 0; i < 2; i++) {
        actionStateGetInfo.action = openxr->triggerValueAction[i];
        if (!resultOK(xrGetActionStateFloat(openxr->hmd, &actionStateGetInfo, &fv))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: 'Trigger' query %i failed: %s\n", i, errorString);

            PsychErrorExitMsg(PsychError_system, "GetInputState query failed.");
        }

        // Meaningful result for this component from active input source?
        if (fv.isActive) {
            valid |= 4;
            v[i] = fv.currentState;
            time = (fv.lastChangeTime > time) ? fv.lastChangeTime : time;
            if (verbosity > 6)
                printf("PsychOpenXRCore-DEBUG: 'Trigger' query %i = %f\n", i, v[i]);
        }
        else {
            v[i] = 0;
        }
    }

    PsychSetStructArrayNativeElement("Trigger", 0, outMat, status);

    // Grip left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    for (i = 0; i < 2; i++) {
        actionStateGetInfo.action = openxr->gripValueAction[i];
        if (!resultOK(xrGetActionStateFloat(openxr->hmd, &actionStateGetInfo, &fv))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: 'Grip' query %i failed: %s\n", i, errorString);

            PsychErrorExitMsg(PsychError_system, "GetInputState query failed.");
        }

        // Meaningful result for this component from active input source?
        if (fv.isActive) {
            valid |= 8;
            v[i] = fv.currentState;
            time = (fv.lastChangeTime > time) ? fv.lastChangeTime : time;
            if (verbosity > 6)
                printf("PsychOpenXRCore-DEBUG: 'Grip' query %i = %f\n", i, v[i]);
        }
        else {
            v[i] = 0;
        }
    }

    PsychSetStructArrayNativeElement("Grip", 0, outMat, status);

    // Thumbstick: column 1 = left hand, column 2 = right hand. row 1 = x, row 2= y:
    v = NULL;
    PsychAllocateNativeDoubleMat(2, 2, 1, &v, &outMat);
    for (i = 0; i < 2; i++) {
        actionStateGetInfo.action = openxr->thumbStick2DAction[i];
        if (!resultOK(xrGetActionStateVector2f(openxr->hmd, &actionStateGetInfo, &vv))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: 'Thumbstick' query %i failed: %s\n", i, errorString);

            PsychErrorExitMsg(PsychError_system, "GetInputState query failed.");
        }

        // Meaningful result for this component from active input source?
        if (vv.isActive) {
            valid |= 16;
            *(v++) = vv.currentState.x;
            *(v++) = vv.currentState.y;
            time = (vv.lastChangeTime > time) ? vv.lastChangeTime : time;
            if (verbosity > 6)
                printf("PsychOpenXRCore-DEBUG: 'Thumbstick' query %i = [%f ; %f]\n", i, vv.currentState.x, vv.currentState.y);
        }
        else {
            *(v++) = 0;
            *(v++) = 0;
        }
    }

    PsychSetStructArrayNativeElement("Thumbstick", 0, outMat, status);

    // Thumbstick2: column 1 = left hand, column 2 = right hand. row 1 = x, row 2= y:
    v = NULL;
    PsychAllocateNativeDoubleMat(2, 2, 1, &v, &outMat);
    for (i = 2; i < 4; i++) {
        actionStateGetInfo.action = openxr->thumbStick2DAction[i];
        if (!resultOK(xrGetActionStateVector2f(openxr->hmd, &actionStateGetInfo, &vv))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: 'Thumbstick2' query %i failed: %s\n", i, errorString);

            PsychErrorExitMsg(PsychError_system, "GetInputState query failed.");
        }

        // Meaningful result for this component from active input source?
        if (vv.isActive) {
            valid |= 32;
            *(v++) = vv.currentState.x;
            *(v++) = vv.currentState.y;
            time = (vv.lastChangeTime > time) ? vv.lastChangeTime : time;
            if (verbosity > 6)
                printf("PsychOpenXRCore-DEBUG: 'Thumbstick2' query %i = [%f ; %f]\n", i, vv.currentState.x, vv.currentState.y);
        }
        else {
            *(v++) = 0;
            *(v++) = 0;
        }
    }

    PsychSetStructArrayNativeElement("Thumbstick2", 0, outMat, status);

    // Controller update time:
    PsychSetStructArrayDoubleElement("Time", 0, (time > 0) ? XrTimeToPsychTime(time) : 0, status);

    // Data valid status:
    PsychSetStructArrayDoubleElement("Valid", 0, (valid != 0) ? 1 : 0, status);
    PsychSetStructArrayDoubleElement("ActiveInputs", 0, valid, status);

    return(PsychError_none);
}

PsychError PSYCHOPENXRGetFovTextureSize(void)
{
    static char useString[] = "[width, height, recMSAASamples, maxMSAASamples, maxWidth, maxHeight] = PsychOpenXRCore('GetFovTextureSize', openxrPtr, eye);";
    //                          1      2       3               4               5         6                                                 1          2
    static char synopsisString[] =
    "Return recommended size of client renderbuffers for OpenXR device 'openxrPtr'.\n"
    "'eye' which eye to provide the size for: 0 = Left, 1 = Right.\n"
    "Return values are 'width' for recommended width of framebuffer in pixels and 'height' for "
    "recommended height of framebuffer in pixels.\n"
    "'recMSAASamples' is the recommended number of samples per pixel for MSAA anti-aliasing, where "
    "a value greater than one means to use MSAA. 'maxMSAASamples' is the maximum MSAA sample count "
    "supported by the runtime.\n"
    "'maxWidth' and 'maxHeight' are the maximum width and height of the framebuffer in pixels, "
    "as supported by the runtime.\n";
    static char seeAlsoString[] = "";

    XrResult result;
    int handle, eyeIndex;
    uint32_t vc;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(6));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get eye index - left = 0, right = 1:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    result = xrEnumerateViewConfigurationViews(xrInstance, openxr->systemId, openxr->viewType, 0, &vc, NULL);
    if (!resultOK(result) || (vc < eyeIndex + 1)) {
        if (verbosity > 0) {
            if (result == XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED)
                printf("PsychOpenXRCore-ERROR: View enumeration failed: Requested view configuration type not supported by OpenXR runtime and device.\n");
            else
                printf("PsychOpenXRCore-ERROR: View enumeration failed: %s\n", errorString);
        }

        PsychErrorExitMsg(PsychError_system, "View enumeration I for selected view configuration failed.");
    }

    XrViewConfigurationView* views = malloc(sizeof(XrViewConfigurationView) * vc);
    for (uint32_t i = 0; i < vc; i++) {
        views[i].type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
        views[i].next = NULL;
    }

    result = xrEnumerateViewConfigurationViews(xrInstance, openxr->systemId, openxr->viewType, vc, &vc, views);
    if (!resultOK(result) || (vc < eyeIndex + 1)) {
        if (verbosity > 0) {
            if (result == XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED)
                printf("PsychOpenXRCore-ERROR: View enumeration failed: Requested view configuration type not supported by OpenXR runtime and device.\n");
            else
                printf("PsychOpenXRCore-ERROR: View enumeration failed: %s\n", errorString);
        }

        free(views);
        PsychErrorExitMsg(PsychError_system, "View enumeration II for selected view configuration failed.");
    }

    // Ask the api for optimal texture parameters:
    openxr->maxWidth = views[eyeIndex].maxImageRectWidth;
    openxr->maxHeight = views[eyeIndex].maxImageRectHeight;
    openxr->recSamples = views[eyeIndex].recommendedSwapchainSampleCount;
    openxr->maxSamples = views[eyeIndex].maxSwapchainSampleCount;

    free(views);

    // Return recommended width and height and MSAA samples of drawBuffer:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, views[eyeIndex].recommendedImageRectWidth);
    PsychCopyOutDoubleArg(2, kPsychArgOptional, views[eyeIndex].recommendedImageRectHeight);
    PsychCopyOutDoubleArg(3, kPsychArgOptional, openxr->recSamples);

    // Return maximum width and height and MSAA samples of drawBuffer:
    PsychCopyOutDoubleArg(4, kPsychArgOptional, openxr->maxSamples);
    PsychCopyOutDoubleArg(5, kPsychArgOptional, openxr->maxWidth);
    PsychCopyOutDoubleArg(6, kPsychArgOptional, openxr->maxHeight);

    return(PsychError_none);
}

PsychError PSYCHOPENXRCreateAndStartSession(void)
{
    static char useString[] = "videoRefreshDuration = PsychOpenXRCore('CreateAndStartSession', openxrPtr, deviceContext, openGLContext, openGLDrawable, openGLConfig, openGLVisualId, use3DMode);";
    //                         1                                                               1          2              3              4               5             6               7
    static char synopsisString[] =
    "Create, initialize and start XR session for OpenXR device 'openxrPtr'.\n"
    "The following parameters are needed to setup OpenGL <=> OpenXR interop. They "
    "define the OpenGL context which will be used to share to-be-displayed image "
    "textures, and its configuration, either using X11/GLX/XLib on Linux, or Win32 "
    "windowing on Windows:\n"
    "'deviceContext' OS specific device context: A X11 X-Server Display* on Linux, "
    "the HDC used for WGL context creation on Windows.\n"
    "'openGLContext' The OpenGL context handle, a GLXContext on Linux/X11/GLX, or a "
    "HGLRC for Windows WGL.\n"
    "'openGLDrawable' The GLXDrawable on Linux/X11/GLX, the HWND window handle on Windows.\n"
    "'openGLConfig' The GLXFBConfig on Linux/X11/GLX, and zero (unused) on Windows.\n"
    "'openGLVisualId' The X11 visual id used for creating the GLXFBConfig / GLXDrawable on "
    "Linux/X11/GLX, zero (unused) on Windows.\n"
    "'use3DMode' must be 0 if pure monoscopic or stereoscopic rendering is requested, or 1 "
    "to request full 3D perspective correct rendering, e.g., for 3D scenes via OpenGL.\n"
    "\n"
    "Returns the following information:\n"
    "'videoRefreshDuration' Video refresh duration in seconds of the XR display device if "
    "available. If the info can't be queried a fallback value of 0.011 secs for a 90 Hz "
    "refresh rate is returned as one of the most common values for consumer HMDs.\n";
    static char seeAlsoString[] = "Open Close";

    XrResult result;
    int handle;
    int use3DMode;
    float displayRefreshRate;
    void* deviceContext;
    void* openGLContext;
    void* openGLDrawable;
    void* openGLConfig;
    void* openGLVisualId;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(7));
    PsychErrorExit(PsychRequireNumInputArgs(7));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Must call pxrGetOpenGLGraphicsRequirementsKHR() or xr instance creation would fail:
    XrGraphicsRequirementsOpenGLKHR reqs = {
        .type = XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR,
        .next = NULL
    };

    if (!resultOK(pxrGetOpenGLGraphicsRequirementsKHR(xrInstance, openxr->systemId, &reqs))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: xrGetOpenGLGraphicsRequirementsKHR() failed: %s\n", errorString);
        PsychErrorExitMsg(PsychError_system, "OpenGL <=> OpenXR interop setup failed.");
    }

    if (verbosity > 3)
        printf("PsychOpenXRCore-INFO: OpenXR runtime supports at least OpenGL versions %i.%i to %i.%i for interop. Later versions may work, but are untested.\n",
               XR_VERSION_MAJOR(reqs.minApiVersionSupported), XR_VERSION_MINOR(reqs.minApiVersionSupported),
               XR_VERSION_MAJOR(reqs.maxApiVersionSupported), XR_VERSION_MINOR(reqs.maxApiVersionSupported));

    // Get all OpenGL interop info for interop with Screen():
    PsychCopyInPointerArg(2, kPsychArgRequired, &deviceContext);
    PsychCopyInPointerArg(3, kPsychArgRequired, &openGLContext);
    PsychCopyInPointerArg(4, kPsychArgRequired, &openGLDrawable);
    PsychCopyInPointerArg(5, kPsychArgRequired, &openGLConfig);
    PsychCopyInPointerArg(6, kPsychArgRequired, &openGLVisualId);

    // Get flag if we should setup initially for 3D perspective projection rendering (1),
    // or just for mono-/stereoscopic drawing with 2D quad views (0):
    PsychCopyInIntegerArg(7, kPsychArgRequired, &use3DMode);
    if (use3DMode < 0 || use3DMode > 1)
        PsychErrorExitMsg(PsychError_user, "Invalid use3DMode flag specified. Must be 0 (no) or 1 (yes).");

    openxr->use3DMode = (psych_bool) use3DMode;

    // Linux X11/GLX/XLib specific setup of OpenGL interop:
    #ifdef XR_USE_PLATFORM_XLIB
    XrGraphicsBindingOpenGLXlibKHR glbinding = {
        .type = XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR,
        .next = NULL,
        .xDisplay = (Display*) deviceContext,
        .visualid = (XID) openGLVisualId,
        .glxFBConfig = (GLXFBConfig) openGLConfig,
        .glxDrawable = (GLXDrawable) openGLDrawable,
        .glxContext = (GLXContext) openGLContext
    };
    #endif

    // Windows Win32 specific setup of OpenGL interop:
    #ifdef XR_USE_PLATFORM_WIN32
    XrGraphicsBindingOpenGLWin32KHR glbinding = {
        .type = XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR,
        .next = NULL,
        .hDC = (HDC) deviceContext,
        .hGLRC = (HGLRC) openGLContext
    };
    #endif

    XrSessionCreateInfo sessionInfo = {
        .type = XR_TYPE_SESSION_CREATE_INFO,
        .next = &glbinding,
        .systemId = openxr->systemId
    };

    if (!resultOK(xrCreateSession(xrInstance, &sessionInfo, &openxr->hmd))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: xrCreateSession() failed: %s\n", errorString);
        PsychErrorExitMsg(PsychError_system, "OpenXR session creation for OpenGL rendered XR failed.");
    }

    // Try to query video refresh rate of the HMD from runtime:
    if (pxrGetDisplayRefreshRateFB && resultOK(pxrGetDisplayRefreshRateFB(openxr->hmd, &displayRefreshRate))) {
        if (displayRefreshRate > 0)
            openxr->frameDuration = 1.0 / (double) displayRefreshRate;
        else
            openxr->frameDuration = 1.0; // Fallback for headless system: 1 Hz.

        if (verbosity > 3)
            printf("PsychOpenXRCore-INFO: Queried XR display refresh rate reported as %f Hz.\n", displayRefreshRate);
    }
    else {
        // Fallback to assumed 90 Hz as something reasonable for many consumer HMDs:
        openxr->frameDuration = 1.0 / 90.0;

        if (verbosity > 3)
            printf("PsychOpenXRCore-INFO: Could not query XR display refresh rate [%s], assuming 90 Hz.\n", (pxrGetDisplayRefreshRateFB) ? errorString : "Not supported");
    }

    // Assume the timeout for the compositor thinking we are unresponsive is 2 HMD frame durations:
    openxr->VRtimeoutSecs = 2 * openxr->frameDuration;

    // Return video refresh duration of the XR display:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, openxr->frameDuration);

    // Create and initialize our standard space for rendering the views in HMD reference frame:
    XrReferenceSpaceCreateInfo refSpaceCreateInfo = {
        .type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO,
        .next = NULL,
        .referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW,
        .poseInReferenceSpace = identityPose
    };

    if (!resultOK(xrCreateReferenceSpace(openxr->hmd, &refSpaceCreateInfo, &openxr->viewSpace))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: xrCreateReferenceSpace() failed for VIEW space: %s\n", errorString);
        PsychErrorExitMsg(PsychError_system, "OpenXR session creation for OpenGL rendered XR failed in xrCreateReferenceSpace for VIEW.");
    }

    // Create and initialize our standard space for defining the world reference frame:
    // TODO: Could also optionally allow XR_REFERENCE_SPACE_TYPE_STAGE for room-scale VR, but only after
    // check if this is supported by runtime, as STAGE support is not mandatory as of OpenXR 1.0.
    refSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    if (!resultOK(xrCreateReferenceSpace(openxr->hmd, &refSpaceCreateInfo, &openxr->worldSpace))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: xrCreateReferenceSpace() failed for WORLD space: %s\n", errorString);
        PsychErrorExitMsg(PsychError_system, "OpenXR session creation for OpenGL rendered XR failed in xrCreateReferenceSpace for WORLD space.");
    }

    // Init origin pose to identity:
    openxr->originPoseInPreviousSpace = identityPose;

    // Create default setup for all supported input/output devices, e.g., touch input controllers for both hands,
    // gamepads, treadmills, HMD buttons, remotes, etc.:
    if (!createDefaultXRInputConfig(openxr)) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to setup default input/output device configuration.\n");

        PsychErrorExitMsg(PsychError_system, "OpenXR session creation failed when trying to setup input/output device configuration.");
    }

    // TODO: Could have setup of alternate action sets / actions / interaction profile bindings under
    // control of future API and user scripts here, to use as alternative to the setup made in
    // createDefaultXRInputConfig().

    // At this point, our openxr->actionSet or future other action sets are ready for attachment,
    // filled with actions, and with all suggested action bindings for different device interaction
    // profiles set up and "suggested".

    // Attach our main/final actionSet(s) to our session:
    // This makes all the actions and bindings and suggested interaction profile bindings
    // immutable for the remaining life-time of the session, ie. this is quite final.
    XrSessionActionSetsAttachInfo actionSetsAttachInfo = {
        .type = XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO,
        .next = NULL,
        .countActionSets = 1,
        .actionSets = &openxr->actionSet
    };

    if (!resultOK(xrAttachSessionActionSets(openxr->hmd, &actionSetsAttachInfo))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to xrAttachSessionActionSets(): %s\n", errorString);

        // Failure return code:
        return(FALSE);
    }

    if (verbosity > 3)
        printf("PsychOpenXRCore-INFO: OpenXR session created for XR device with OpenGL rendering.\n");

    return(PsychError_none);
}


PsychError PSYCHOPENXRControllers(void)
{
    static char useString[] = "controllerTypes = PsychOpenXRCore('Controllers', openxrPtr);";
    //                         1                                                1
    static char synopsisString[] =
    "Return currently available and active controllers for OpenXR device 'openxrPtr'.\n"
    "The returned 'controllerTypes' can be the logical OR of one of these flags:\n"
    "OVR.ControllerType_LTouch = Left touch controller (Left tracked hand).\n"
    "OVR.ControllerType_RTouch = Right touch controller (Right tracked hand).\n"
    "OVR.ControllerType_Remote = Connected remote control or similar, e.g., control buttons on HMD.\n"
    "OVR.ControllerType_XBox = Microsoft XBox controller or some equivalent gamepad.\n"
    "\n";
    static char seeAlsoString[] = "GetInputState HapticPulse";

    char profile[128];
    int handle, i;
    PsychOpenXRDevice *openxr;
    int controllerTypes = 0;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Need sync actions to make sure proper interaction profiles are bound:
    if (!processXREvents(xrInstance) && (verbosity > 0))
        printf("PsychOpenXRCore-ERROR:Controllers: Failed to poll events, or session state reports error abort!");

    PsychLockMutex(&(openxr->presenterLock));
    syncXRActions(openxr);
    PsychUnlockMutex(&(openxr->presenterLock));

    if (!processXREvents(xrInstance) && (verbosity > 0))
        printf("PsychOpenXRCore-ERROR:Controllers: Failed to poll events, or session state reports error abort!");

    for (i = 0; i < 4; i++) {
        if (getActiveXRInteractionProfile(xrInstance, openxr, openxr->handPath[i], profile)) {
            switch (i) {
                case 0: // Left controller: OVR.ControllerType_LTouch
                    controllerTypes |= 1;
                    break;

                case 1: // Right controller: OVR.ControllerType_RTouch
                    controllerTypes |= 2;
                    break;

                case 2: // Gamepad / "XBox" controller: OVR.ControllerType_XBox
                    controllerTypes |= 16;
                    break;

                case 3: // Controls on HMD, or remote control: OVR.ControllerType_Remote
                    controllerTypes |= 4;
                    break;
            }

            if (verbosity > 3)
                printf("PsychOpenXRCore-INFO: Active interaction profile for [%i]: %s\n", i, profile);
        }
    }

    // Return controllerTypes mask:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, controllerTypes);

    return(PsychError_none);
}


PsychError PSYCHOPENXRCreateRenderTextureChain(void)
{
    static char useString[] = "[width, height, numTextures, imageFormat] = PsychOpenXRCore('CreateRenderTextureChain', openxrPtr, eye, width, height, floatFormat, numMSAASamples);";
    //                          1      2       3            4                                                          1          2    3      4       5            6
    static char synopsisString[] =
    "Create texture present chains for OpenXR device 'openxrPtr'.\n\n"
    "'eye' Eye for which chain should get created: 0 = Left/Mono, 1 = Right.\n"
    "If only a chain for eye = 0 is created then the driver operates in monoscopic "
    "presentation mode for use with Screen() stereomode 0, showing the same mono image to both "
    "eyes. If a 2nd chain for eye = 1 is created then the driver switches to stereoscopic "
    "presentation mode for use with Screen() stereomode 12, presenting separate images to the left "
    "and right eye.\n\n"
    "'width' and 'height' are the width x height of the texture into which Psychtoolbox "
    "Screen() image processing pipeline will render the output image of an eye for submission "
    "to the VR compositor. Left and right eye must use identical 'width' and 'height'.\n\n"
    "'floatFormat' Texture format: 0 = RGBA8 sRGB format for sRGB rendering and output. 1 = 16 bpc "
    "half-float RGBA16F in linear format.\n\n"
    "'numMSAASamples' is the number of samples to use per texel. Must be at least 1, and can be more "
    "on implementations which support MSAA anti-aliasing.\n"
    "'numTextures' returns the total number of compositor textures in the swap chain.\n"
    "\n"
    "Return values are 'width' for selected width of output texture in pixels and "
    "'height' for height of output texture in pixels.\n\n"
    "'imageFormat' is the chosen OpenGL internal texture format for the swapchain textures.\n";
    static char seeAlsoString[] = "GetNextTextureHandle";

    int handle, eyeIndex;
    int width, height, out_Length, floatFormat, numMSAASamples;
    uint32_t nFormats, i;
    int64_t *swapchainFormats = NULL;
    int64_t imageFormat = -1;
    PsychOpenXRDevice *openxr;
    XrResult result;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(4));
    PsychErrorExit(PsychCapNumInputArgs(6));
    PsychErrorExit(PsychRequireNumInputArgs(6));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Need a valid XrSession:
    if (!openxr->hmd)
        PsychErrorExitMsg(PsychError_user, "Must successfully call CreateAndStartSession first.");

    // Get eye:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1)
        PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    // Get texture dimensions:
    PsychCopyInIntegerArg(3, kPsychArgRequired, &width);
    if (width < 1)
        PsychErrorExitMsg(PsychError_user, "Invalid width, smaller than 1 texel!");

    if (width > openxr->maxWidth) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Requested texture width %i > %i maximum supported by runtime.\n", width, openxr->maxWidth);

        PsychErrorExitMsg(PsychError_user, "Invalid width, bigger than allowed maximum!");
    }

    PsychCopyInIntegerArg(4, kPsychArgRequired, &height);
    if (height < 1)
        PsychErrorExitMsg(PsychError_user, "Invalid height, smaller than 1 texel!");

    if (height > openxr->maxHeight) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Requested texture height %i > %i maximum supported by runtime.\n", height, openxr->maxHeight);

        PsychErrorExitMsg(PsychError_user, "Invalid height, bigger than allowed maximum!");
    }

    // Get texture format: 0 = sRGB RGBA8, 1 = linear RGBA16F half-float:
    PsychCopyInIntegerArg(5, kPsychArgRequired, &floatFormat);
    if (floatFormat < 0 || floatFormat > 1)
        PsychErrorExitMsg(PsychError_user, "Invalid floatFormat flag provided. Must be 0 or 1.");

    // Get number of MSAA samples to use for swapchain images:
    PsychCopyInIntegerArg(6, kPsychArgRequired, &numMSAASamples);
    if (numMSAASamples < 1 || numMSAASamples > openxr->maxSamples) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Requested texture MSAA sample count %i > %i maximum count supported by runtime, or smaller than minimum of 1 sample.\n", numMSAASamples, openxr->maxSamples);

        PsychErrorExitMsg(PsychError_user, "Invalid numMSAASamples sample count provided.");
    }

    if (openxr->textureSwapChain[eyeIndex])
        PsychErrorExitMsg(PsychError_user, "Tried to create already created texture swap chain for given eye.");

    if (eyeIndex > 0 && (width != openxr->textureWidth || height != openxr->textureHeight))
        PsychErrorExitMsg(PsychError_user, "Given width x height for 2nd eye does not match width x height of 1st eye, as required.");

    // Find suitable swapchain format aka OpenGL texture format:
    if (!resultOK(xrEnumerateSwapchainFormats(openxr->hmd, 0, &nFormats, NULL))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to get number of supported swapchain formats: %s\n", errorString);
        PsychErrorExitMsg(PsychError_system, "Swapchain format enumeration failed I.");
    }
    else if (verbosity > 3)
        printf("PsychOpenXRCore-INFO: Runtime supports %i formats for swapchain.\n", nFormats);

    swapchainFormats = malloc(nFormats * sizeof(int64_t));
    if (!swapchainFormats || !resultOK(xrEnumerateSwapchainFormats(openxr->hmd, nFormats, &nFormats, swapchainFormats))) {
        free(swapchainFormats);

        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to enumerate supported swapchain formats II: %s\n", errorString);

        PsychErrorExitMsg(PsychError_system, "Swapchain format enumeration failed II.");
    }

    // Print all supported swapchain formats:
    if (verbosity > 4) {
        for (i = 0; i < nFormats; i++)
            printf("PsychOpenXRCore-INFO: %i'th supported OpenGL internal format is 0x%x.\n", i, swapchainFormats[i]);
    }

    // Select color texture format:
    // TODO This may need more finesse or a different approach in the future...
    // We want to choose a format that is preferred by the runtime, suitable for the task
    // in terms of precision and range (PTB user request) and ideally directly usable for
    // the Screen drawBufferFBO's, so we can do zero-copy there instead of an extra blit.
    for (i = 0; i < nFormats; i++) {
        // Regardless if floatFormat requested or not, allow to get GL_RGBA32F:
        if (swapchainFormats[i] == GL_RGBA32F)
            break;

        // Regardless if floatFormat requested or not, try to get GL_RGBA16F:
        if (swapchainFormats[i] == GL_RGBA16F)
            break;

        // Only if floatFormat requested, allow to get GL_RGB16F without alpha, as this will
        // trigger a more expensive non-zero-copy unshare operation in Screen():
        if (swapchainFormats[i] == GL_RGB16F && floatFormat)
            break;

        // Prefer 16 bpc RGBA linear unorm for non-float only:
        if (swapchainFormats[i] == GL_RGBA16 && !floatFormat)
            break;

        // Prefer 8 bpc sRGB with 8 bit SRGB + 8 bit linear alpha for non-float only:
        if (swapchainFormats[i] == GL_SRGB8_ALPHA8 && !floatFormat)
            break;

        // Fallback to 8 bpc unorm linear as a last resort for non-float only:
        if (swapchainFormats[i] == GL_RGBA8 && !floatFormat)
            break;
    }

    if (i < nFormats) {
        imageFormat = swapchainFormats[i];
    } else {
        imageFormat = swapchainFormats[0];
        if (verbosity > 2)
            printf("PsychOpenXRCore-INFO: Failed to find optimal swapchain format. Choosing fallback format 0x%x\n", imageFormat);
    }

    if (verbosity > 3)
        printf("PsychOpenXRCore-INFO: Choosing OpenGL format 0x%x for renderbuffer textures.\n", imageFormat);

    // TODO Select depth + stencil buffer format...

    // No need for swapchainFormats anymore:
    free(swapchainFormats);

    // Create texture swap chain:
    PsychLockMutex(&(openxr->presenterLock));

    // Setup spec for swapchain:
    XrSwapchainCreateInfo swapchainCreateInfo = {
        .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
        .next = NULL,
        .createFlags = 0,
        .usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
        .format = imageFormat,
        .sampleCount = numMSAASamples,
        .width = width,
        .height = height,
        .faceCount = 1,
        .arraySize = 1,
        .mipCount = 1,
    };

    if(!resultOK(xrCreateSwapchain(openxr->hmd, &swapchainCreateInfo, &openxr->textureSwapChain[eyeIndex]))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: xrCreateSwapchain for eye %i failed: %s\n", eyeIndex, errorString);

        PsychUnlockMutex(&(openxr->presenterLock));
        PsychErrorExitMsg(PsychError_system, "Failed to create swap chain for XR compositor.");
    }

    // Mark driver as in stereo mode if a swap chain for the right eye was created:
    if (eyeIndex > 0) {
        if (verbosity > 2)
            printf("PsychOpenXRCore-INFO: Right eye swap chain created. Switching to stereo mode.\n");

        openxr->isStereo = TRUE;
    }

    // Enumerate / query swapchain images, aka OpenGL texture handles:
    if (!resultOK(xrEnumerateSwapchainImages(openxr->textureSwapChain[eyeIndex], 0, &openxr->textureSwapChainLength[eyeIndex], NULL))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to get number of swapchain images for eye %i: %s\n", eyeIndex, errorString);
        PsychErrorExitMsg(PsychError_system, "Swapchain image enumeration failed I.");
    }
    else if (verbosity > 3)
        printf("PsychOpenXRCore-INFO: Swapchain for eye %i has %i images.\n", eyeIndex, openxr->textureSwapChainLength[eyeIndex]);


    // Allocate array of XrSwapchainImageOpenGLKHR structs for receiving the OpenGL texture handles for the swapchain images:
    openxr->textureSwapChainImages[eyeIndex] = malloc(openxr->textureSwapChainLength[eyeIndex] * sizeof(XrSwapchainImageOpenGLKHR));
    if (!openxr->textureSwapChainImages[eyeIndex]) {
        openxr->textureSwapChainLength[eyeIndex] = 0;

        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to enumerate swapchain images for eye %i due to out of memory.\n", eyeIndex);

        PsychErrorExitMsg(PsychError_outofMemory, "Swapchain image enumeration failed II.");
    }

    for (i = 0; i < openxr->textureSwapChainLength[eyeIndex]; i++) {
        openxr->textureSwapChainImages[eyeIndex][i].type = XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR;
        openxr->textureSwapChainImages[eyeIndex][i].next = NULL;
    }

    if (!resultOK(xrEnumerateSwapchainImages(openxr->textureSwapChain[eyeIndex], openxr->textureSwapChainLength[eyeIndex],
        &openxr->textureSwapChainLength[eyeIndex], (XrSwapchainImageBaseHeader*) openxr->textureSwapChainImages[eyeIndex]))) {
        free(openxr->textureSwapChainImages[eyeIndex]);
        openxr->textureSwapChainImages[eyeIndex] = NULL;
        openxr->textureSwapChainLength[eyeIndex] = 0;

        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to enumerate swapchain images for eye %i II: %s\n", eyeIndex, errorString);

        PsychErrorExitMsg(PsychError_system, "Swapchain image enumeration failed II.");
    }

    if (verbosity > 3)
        printf("PsychOpenXRCore-INFO: Allocated texture swap chain for eye %i has %i images with %i samples per texel.\n",
               eyeIndex, openxr->textureSwapChainLength[eyeIndex], numMSAASamples);

    // Assign total texture buffer width/height for the frame submission later on:
    openxr->textureWidth = width;
    openxr->textureHeight = height;

    // Initialize views, projection views and quad views with what can be statically initialized:

    // These are used to receive head/eye position + orientation information from 3D 6-DoF head tracking
    // and possibly eye tracking later on to establish the virtual cameras location/pose/FoV:
    openxr->view[eyeIndex].type = XR_TYPE_VIEW;
    openxr->view[eyeIndex].next = NULL;

    // Quad views are used for monoscopic or stereoscopic rendering usually without taking
    // head tracking / pose into account. These quads float in a head-locked fixed position
    // in front of the viewers eyes, to act as mono display, or as stereo displays for monoscopic
    // or stereoscopic stimulation, but usually not rendered 3D content.
    openxr->quadViewLayer[eyeIndex].type = XR_TYPE_COMPOSITION_LAYER_QUAD;
    openxr->quadViewLayer[eyeIndex].next = NULL;
    // Correct for chromatic lens aberration:
    openxr->quadViewLayer[eyeIndex].layerFlags = XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
    // Reference space to use for defining view position and orientation:
    openxr->quadViewLayer[eyeIndex].space = openxr->viewSpace;

    // Switch to stereo mode just happened?
    if (openxr->isStereo) {
        // Assign each eye view to only one eye:
        openxr->quadViewLayer[0].eyeVisibility = XR_EYE_VISIBILITY_LEFT;
        openxr->quadViewLayer[1].eyeVisibility = XR_EYE_VISIBILITY_RIGHT;
    }
    else {
        // No, monoscopic mode active, both eyes are fed by this eyeIndex == 0 view:
        openxr->quadViewLayer[eyeIndex].eyeVisibility = XR_EYE_VISIBILITY_BOTH;
    }

    // Assign image source for the view - swapchain to sample from and full swapchain image layer 0:
    openxr->quadViewLayer[eyeIndex].subImage.swapchain = openxr->textureSwapChain[eyeIndex];
    openxr->quadViewLayer[eyeIndex].subImage.imageRect.offset.x = 0;
    openxr->quadViewLayer[eyeIndex].subImage.imageRect.offset.y = 0;
    openxr->quadViewLayer[eyeIndex].subImage.imageRect.extent.width = openxr->textureWidth;
    openxr->quadViewLayer[eyeIndex].subImage.imageRect.extent.height = openxr->textureHeight;
    openxr->quadViewLayer[eyeIndex].subImage.imageArrayIndex = 0;

    // Initial pose is identity pose, ie. position (0,0,0) with neutral orientation:
    openxr->quadViewLayer[eyeIndex].pose = identityPose;

    // Shift away from eyes by 0.5 meters:
    openxr->quadViewLayer[eyeIndex].pose.position.z -= 0.5;

    // Size of virtual projection screen / display monitor is 1 x 1 meters:
    openxr->quadViewLayer[eyeIndex].size.width = 1.0;
    openxr->quadViewLayer[eyeIndex].size.height = 1.0;

    // projViews are used for full 3D OpenGL rendering, usually driven by full tracking
    // of head pose and potentially individual eye pose (gaze tracking). Do equivalent
    // setup as for quadViewLayers:
    openxr->projView[eyeIndex].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
    openxr->projView[eyeIndex].next = NULL;
    openxr->projView[eyeIndex].subImage.swapchain = openxr->textureSwapChain[eyeIndex];
    openxr->projView[eyeIndex].subImage.imageRect.offset.x = 0;
    openxr->projView[eyeIndex].subImage.imageRect.offset.y = 0;
    openxr->projView[eyeIndex].subImage.imageRect.extent.width = openxr->textureWidth;
    openxr->projView[eyeIndex].subImage.imageRect.extent.height = openxr->textureHeight;
    openxr->projView[eyeIndex].subImage.imageArrayIndex = 0;

    // Pose and FoV init defaults:
    openxr->projView[eyeIndex].pose = identityPose;

    // Setup the static projectionLayer we use for fully tracked 3D OpenGL rendering:
    openxr->projectionLayer.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION;
    openxr->projectionLayer.next = NULL;
    openxr->projectionLayer.layerFlags = XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
    openxr->projectionLayer.space = openxr->worldSpace;
    openxr->projectionLayer.viewCount = 2;
    openxr->projectionLayer.views = openxr->projView;

    // Setup initial assignments of actual layers to submit to the compositor for display:
    if (openxr->use3DMode) {
        // 3D perspective projected mode: Assign stereo projectionLayer as init state:
        openxr->submitLayers[0] = (XrCompositionLayerBaseHeader*) &openxr->projectionLayer;
        openxr->submitLayersCount = 1;
    }
    else {
        // 2D mode: Init state is to use the quadViewLayer(s) for mono or stereo 2D display:
        openxr->submitLayers[0] = (XrCompositionLayerBaseHeader*) &openxr->quadViewLayer[0];
        openxr->submitLayers[1] = (XrCompositionLayerBaseHeader*) &openxr->quadViewLayer[1];
        openxr->submitLayersCount = (openxr->isStereo) ? 2 : 1;
    }

    PsychUnlockMutex(&(openxr->presenterLock));

    // Return recommended width and height of drawBuffer:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, openxr->textureWidth);
    PsychCopyOutDoubleArg(2, kPsychArgOptional, openxr->textureHeight);

    // Return number of textures in swap chain:
    PsychCopyOutDoubleArg(3, kPsychArgOptional, openxr->textureSwapChainLength[eyeIndex]);

    // Return chosen swapchain OpenGL internal texture image format:
    PsychCopyOutDoubleArg(4, kPsychArgOptional, (double) (uint32_t) imageFormat);

    return(PsychError_none);
}


PsychError PSYCHOPENXRGetNextTextureHandle(void)
{
    static char useString[] = "texObjectHandle = PsychOpenXRCore('GetNextTextureHandle', openxrPtr, eye);";
    //                         1                                                         1          2
    static char synopsisString[] =
    "Retrieve OpenGL texture object handle for next target texture for OpenXR device 'openxrPtr'.\n"
    "'eye' Eye for which handle of next texture should be returned: 0 = Left/Mono, 1 = Right.\n"
    "Returns a GL_TEXTURE_2D texture object name/handle in 'texObjectHandle' for the texture "
    "to which the next VR frame should be rendered. Returns -1 if busy.\n";
    static char seeAlsoString[] = "CreateRenderTextureChain";

    XrResult result;
    int handle, eyeIndex;
    uint32_t texIndex;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get eye:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1)
        PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    if (eyeIndex > 0 && !(openxr->isStereo))
        PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0, as mono display mode is selected.");

    // Get next free/non-busy buffer for this eyes texture swap chain:
    PsychLockMutex(&(openxr->presenterLock));

    // Acquire next texture handle:
    XrSwapchainImageAcquireInfo acquireInfo = {
        .type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO,
        .next = NULL
    };

    if (!resultOK(xrAcquireSwapchainImage(openxr->textureSwapChain[eyeIndex], &acquireInfo, &texIndex))) {
        PsychUnlockMutex(&(openxr->presenterLock));

        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to acquire next swapchain image for eye %i: %s\n", eyeIndex, errorString);
    }

    // Wait for its availability for up to 1 second = 1e9 nsecs:
    XrSwapchainImageWaitInfo waitTexInfo = {
        .type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO,
        .next = NULL,
        .timeout = 1e9
    };

    result = xrWaitSwapchainImage(openxr->textureSwapChain[eyeIndex], &waitTexInfo);
    if (!resultOK(result)) {
        PsychUnlockMutex(&(openxr->presenterLock));

        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to wait for next swapchain image for eye %i: %s\n", eyeIndex, errorString);
    }

    PsychUnlockMutex(&(openxr->presenterLock));

    // Return texture object handle:
    if (result == XR_SUCCESS)
        PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) openxr->textureSwapChainImages[eyeIndex][texIndex].image);
    else {
        PsychCopyOutDoubleArg(1, kPsychArgOptional, -1);

        if ((result == XR_TIMEOUT_EXPIRED) && (verbosity > 1))
            printf("PsychOpenXRCore-WARNING: Timed out waiting for next swapchain image for eye %i: %s\n", eyeIndex, errorString);
    }

    return(PsychError_none);
}

// TODO: Maybe convert into a generic object tracking function, e.g., using XR_VARJO_marker_tracking, XR_HTC_vive_wrist_tracker_interaction?
PsychError PSYCHOPENXRGetTrackersState(void)
{
    static char useString[] = "trackers = PsychOpenXRCore('GetTrackersState', openxrPtr);";
    //                         1                                                 1
    static char synopsisString[] =
    "Return info about all connected trackers for OpenXR device 'openxrPtr'.\n\n"
    "'trackers' is an array of structs, with one struct for each connected tracker camera. "
    "Each struct contains the following fields, describing static and dynamic properties "
    "of the corresponding tracking camera:\n\n"
    "'Status': Sum of +4 = The tracking camera has a valid pose, +32 = The camera is present and online.\n"
    "'CameraPose' as vector with position [tx,ty,tz] in meters and orientation quaternion [rx,ry,rz,rw] in radians, "
    "concatenated together as [tx,ty,tz,rx,ry,rz,rw].\n"
    "'LeveledCameraPose' Like 'CameraPose', but aligned to the gravity vector of the world.\n"
    "'CameraFrustumHVFov' Horizontal and vertical field of view of the tracking camera in radians.\n"
    "'CameraFrustumNearFarZInMeters' Near and far limit of the camera view frustum in meters.\n"
    "\n";
    static char seeAlsoString[] = "GetTrackingState";

    PsychGenericScriptType *status;
    const char *FieldNames[] = {"Status", "CameraPose", "LeveledCameraPose", "CameraFrustumHVFov", "CameraFrustumNearFarZInMeters"};
    const int FieldCount = 5;
    PsychGenericScriptType *outMat;
    double *v;
/*
    int handle, trackerCount, i;
    PsychOpenXRDevice *openxr;
    int StatusFlags = 0;
    ovrTrackerPose trackerPose;
    ovrTrackerDesc trackerDesc;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Number of available tracker cameras:
    trackerCount = ovr_GetTrackerCount(openxr->hmd);

    PsychAllocOutStructArray(1, kPsychArgOptional, trackerCount, FieldCount, FieldNames, &status);

    for (i = 0; i < trackerCount; i++) {
        trackerPose = ovr_GetTrackerPose(openxr->hmd, (unsigned int) i);
        StatusFlags = trackerPose.TrackerFlags & (ovrTracker_Connected | ovrTracker_PoseTracked);

        // Return head and general tracking status flags:
        PsychSetStructArrayDoubleElement("Status", i, StatusFlags, status);

        // Camera pose:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);
        v[0] = trackerPose.Pose.pose.position.x;
        v[1] = trackerPose.Pose.pose.position.y;
        v[2] = trackerPose.Pose.pose.position.z;

        v[3] = trackerPose.Pose.pose.orientation.x;
        v[4] = trackerPose.Pose.pose.orientation.y;
        v[5] = trackerPose.Pose.pose.orientation.z;
        v[6] = trackerPose.Pose.pose.orientation.w;
        PsychSetStructArrayNativeElement("CameraPose", i, outMat, status);

        // Camera leveled pose:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);
        v[0] = trackerPose.LeveledPose.pose.position.x;
        v[1] = trackerPose.LeveledPose.pose.position.y;
        v[2] = trackerPose.LeveledPose.pose.position.z;

        v[3] = trackerPose.LeveledPose.pose.orientation.x;
        v[4] = trackerPose.LeveledPose.pose.orientation.y;
        v[5] = trackerPose.LeveledPose.pose.orientation.z;
        v[6] = trackerPose.LeveledPose.pose.orientation.w;
        PsychSetStructArrayNativeElement("LeveledCameraPose", i, outMat, status);

        trackerDesc = ovr_GetTrackerDesc(openxr->hmd, (unsigned int) i);

        // Camera frustum HFov and VFov in radians:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
        v[0] = trackerDesc.FrustumHFovInRadians;
        v[1] = trackerDesc.FrustumVFovInRadians;
        PsychSetStructArrayNativeElement("CameraFrustumHVFov", i, outMat, status);

        // Camera frustum near and far clip plane in meters:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
        v[0] = trackerDesc.FrustumNearZInMeters;
        v[1] = trackerDesc.FrustumFarZInMeters;
        PsychSetStructArrayNativeElement("CameraFrustumNearFarZInMeters", i, outMat, status);
    }
*/
    return(PsychError_none);
}

// Manual reimplementation of OpenGL's glFrustum(), based on my self-made tried and
// proven glFrustum.m implementation for the OpenGL-ES use case, which was in turn based
// on the matrix math and definition for glFrustum() in the official OpenGL spec. This
// additionally uses tan() mapping of angles and removes zNear as a multiplier for elements
// M[0][0] and M[1][1], so we are invariant to the value of zNear, as far as the projection
// is concerned. Ie. zNear is the OpenGL clip plane, but doesn't influence otherwise. So this
// is a bit of a modified glFrustum for building suitable GL_PROJECTION matrices for OpenGL:
static void buildProjectionMatrix(double M[4][4], XrFovf fov, double zNear, double zFar)
{
    double angleRight = tan(fov.angleRight);
    double angleLeft = tan(fov.angleLeft);
    double angleDown = tan(fov.angleDown);
    double angleUp = tan(fov.angleUp);

    memset(M, 0, 4 * 4 * sizeof(double));
    M[0][0] = 2 / (angleRight - angleLeft);
    M[1][1] = 2 / (angleUp - angleDown);
    M[0][2] = (angleRight + angleLeft) / (angleRight - angleLeft);
    M[1][2] = (angleUp + angleDown) / (angleUp - angleDown);
    M[2][2] = - (zFar + zNear) / (zFar - zNear);
    M[2][3] = - 2 * zFar * zNear / (zFar - zNear);
    M[3][2] = -1;
}

PsychError PSYCHOPENXRGetStaticRenderParameters(void)
{
    static char useString[] = "[projL, projR] = PsychOpenXRCore('GetStaticRenderParameters', openxrPtr [, clipNear=0.01][, clipFar=10000.0]);";
    //                          1      2                                                     1            2                3
    static char synopsisString[] =
    "Retrieve static rendering parameters for OpenXR device 'openxrPtr' at current settings.\n"
    "'clipNear' Optional near clipping plane for OpenGL. Defaults to 0.01.\n"
    "'clipFar' Optional far clipping plane for OpenGL. Defaults to 10000.0.\n"
    "\nReturn arguments:\n\n"
    "'projL' is the 4x4 OpenGL projection matrix for the left eye rendering.\n"
    "'projR' is the 4x4 OpenGL projection matrix for the right eye rendering.\n"
    "Please note that projL and projR are usually identical for typical rendering scenarios.\n";
    static char seeAlsoString[] = "";

    int handle;
    PsychOpenXRDevice *openxr;
    double M[4][4];
    int i, j;
    double clip_near, clip_far;
    double *outM;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get optional near clipping plane:
    clip_near = 0.01;
    PsychCopyInDoubleArg(2, kPsychArgOptional, &clip_near);

    // Get optional far clipping plane:
    clip_far = 10000.0;
    PsychCopyInDoubleArg(3, kPsychArgOptional, &clip_far);

    // Make sure our views field of view .fov is well defined:
    locateXRViews(openxr, openxr->frameState.predictedDisplayTime);

    if (verbosity > 4) {
        int eyeIndex;

        for (eyeIndex = 0; eyeIndex < 2; eyeIndex++) {
            printf("PsychOpenXRCore-INFO: HMD %i, eye %i - FoV degrees: %f %f %f %f\n", handle, eyeIndex,
                   rad2deg(openxr->view[eyeIndex].fov.angleLeft), rad2deg(openxr->view[eyeIndex].fov.angleRight),
                   rad2deg(openxr->view[eyeIndex].fov.angleUp), rad2deg(openxr->view[eyeIndex].fov.angleDown));
        }
    }

    // Return left projection matrix as return argument 1:
    buildProjectionMatrix(M, openxr->view[0].fov, clip_near, clip_far);
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 4, 4, 1, &outM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(outM++) = M[j][i];

    // Return right projection matrix as return argument 2:
    buildProjectionMatrix(M, openxr->view[1].fov, clip_near, clip_far);
    PsychAllocOutDoubleMatArg(2, kPsychArgOptional, 4, 4, 1, &outM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(outM++) = M[j][i];

    return(PsychError_none);
}

PsychError PSYCHOPENXRStartRender(void)
{
    static char useString[] = "PsychOpenXRCore('StartRender', openxrPtr [, targetTime=nextFrame]);";
    //                                                        1            2
    static char synopsisString[] =
    "Mark start of a new 3D head/eye tracked render cycle for OpenXR device 'openxrPtr'.\n"
    "The render is for an eye position and orientation at target time 'targetTime' in seconds if provided. "
    "If 'targetTime' is omitted or zero, then the render is performed for the mid-point of the next "
    "possible video frame of the HMD, ie. the most likely presentation time for immediately rendered images.\n";
    static char seeAlsoString[] = "GetTrackingState EndFrameRender";

    int handle;
    PsychOpenXRDevice *openxr;
    double predictionTime;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get optional target time for predicted tracking state. Default to the
    // predicted state for the predicted mid-point of the next video frame:
    if (!PsychCopyInDoubleArg(2, kPsychArgOptional, &predictionTime))
        predictionTime = 0;

    // This function does not do anything meaningful yet. Just return success:
    return(PsychError_none);
}

PsychError PSYCHOPENXREndFrameRender(void)
{
    static char useString[] = "PsychOpenXRCore('EndFrameRender', openxrPtr [, eye]);";
    //                                                           1            2
    static char synopsisString[] =
    "Mark end of a render cycle for a swapchain of an OpenXR HMD device 'openxrPtr'.\n\n"
    "'eye' Eye for which currently used texture should be released to its swapchain: 0 = Left/Mono, 1 = Right. "
    "If omitted, all current textures for all eyes are released.\n"
    "You usually won't call this function yourself, but Screen('Flip') will call it automatically "
    "for you at the appropriate moment.\n";
    static char seeAlsoString[] = "StartRender PresentFrame";

    int handle;
    PsychOpenXRDevice *openxr;
    int eyeIndex = -1;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    if (!processXREvents(xrInstance) && (verbosity > 0))
        printf("PsychOpenXRCore-ERROR:EndFrameRender: Failed to poll events, or session state reports error abort!");

    if (verbosity > 3)
        printf("PsychOpenXRCore-INFO:EndFrameRender: Session state for handle %i: Session %s, frame loop needs to be %s.\n", handle,
               openxr->sessionActive ? "ACTIVE" : "STOPPED", openxr->needFrameLoop ? "RUNNING" : "STOPPED");

    // If frame loop is supposed to be inactive, skip the xrReleaseSwapchainImage calls:
    if (!openxr->needFrameLoop)
        return(PsychError_none);

    // Get eye:
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &eyeIndex)) {
        if (eyeIndex < 0 || eyeIndex > 1)
            PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

        if (eyeIndex > 0 && !(openxr->isStereo))
            PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0, as mono display mode is selected.");

        if (!resultOK(xrReleaseSwapchainImage(openxr->textureSwapChain[eyeIndex], NULL))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to release current swapchain image for eye %i: %s\n", eyeIndex, errorString);
        }
    }
    else {
        for (eyeIndex = 0; eyeIndex < ((openxr->isStereo) ? 2 : 1); eyeIndex++) {
            if (!resultOK(xrReleaseSwapchainImage(openxr->textureSwapChain[eyeIndex], NULL))) {
                if (verbosity > 0)
                    printf("PsychOpenXRCore-ERROR: Failed to release current swapchain image for eye %i: %s\n", eyeIndex, errorString);
            }
        }
    }

    return(PsychError_none);
}

// TODO
// Execute VR present operation, possibly committing new content to the swapchains:
// Must be called with the presenterLock locked!
// Called by idle presenterThread to keep VR compositor timeout handling from kicking in,
// and directly from PSYCHOPENXRPresentFrame when userspace wants to present new content.
static double PresentExecute(PsychOpenXRDevice *openxr, psych_bool inInit)
{
    int eyeIndex;
    psych_bool success = TRUE;
    double tPredictedOnset = 0;
    XrResult result;

    // Skip if no frame present cycle is wanted:
    if (!openxr->needFrameLoop) {
        success = TRUE;
        goto present_out;
    }

    if (!inInit) {
        // Validate requested presentation time targetPresentTime. If it is valid, convert from Psychtoolbox GetSecs time
        // to absolute XrTime targetDisplayTime. Otherwise select targetDisplayTime as zero:
        XrTime targetDisplayTime = (openxr->targetPresentTime > 0) ? PsychTimeToXrTime(openxr->targetPresentTime) : 0;

        // If targetDisplayTime is invalid (== 0) or earlier than earliest next possible display time predictedDisplayTime,
        // then set it to earliest possible display time:
        if (targetDisplayTime < openxr->frameState.predictedDisplayTime)
            targetDisplayTime = openxr->frameState.predictedDisplayTime;

        // Enforce view[] update with proper fov, pose for desired targetDisplayTime:
        locateXRViews(openxr, targetDisplayTime);

        // Prepare frameEndInfo, assign desired optimal targetDisplayTime. Compositor should present at the earliest
        // possible time no earlier than targetDisplayTime:
        XrFrameEndInfo frameEndInfo = {
            .type = XR_TYPE_FRAME_END_INFO,
            .next = NULL,
            .displayTime = targetDisplayTime,
            .environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
            .layerCount = (openxr->frameState.shouldRender) ? openxr->submitLayersCount : 0,
            .layers = openxr->submitLayers
        };

        // Update pose and FoV for projection views:
        for (eyeIndex = 0; eyeIndex < ((openxr->isStereo) ? 2 : 1); eyeIndex++) {
            openxr->projView[eyeIndex].pose = openxr->view[eyeIndex].pose;
            openxr->projView[eyeIndex].fov = openxr->view[eyeIndex].fov;
        }

        result = xrEndFrame(openxr->hmd, &frameEndInfo);
        if (!resultOK(result)) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to xrEndFrame: %s\n", errorString);

            success = FALSE;
            goto present_fail;
        }
    }

    // Do the frame present cycle:
    openxr->frameState.type = XR_TYPE_FRAME_STATE;
    openxr->frameState.next = NULL;

    XrFrameWaitInfo frameWaitInfo = {
        .type = XR_TYPE_FRAME_WAIT_INFO,
        .next = NULL
    };

    result = xrWaitFrame(openxr->hmd, &frameWaitInfo, &openxr->frameState);
    if (!resultOK(result)) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to xrWaitFrame: %s\n", errorString);

        success = FALSE;
        goto present_fail;
    }

    // xrWaitFrame success. openxr->frameState.predictedDisplayTime contains the earliest predicted
    // display time of the next to-be-submitted (via xrEndFrame) frame, assuming xrEndFrame
    // would happen almost immediately, and no overload or other glitches. Our working assumption
    // here is that if we performed our blocking xrWaitFrame immediately after the xrEndFrame
    // which submitted our latest stimulus frame, ie. back-to-back, then xrWaitFrame would block
    // until our just submitted stimulus frame was latched and composited by the compositor into
    // the system framebuffer for the HMD at the right time to hit the requested targetDisplayTime
    // as well as possible, the pageflip was executed and signalled complete by the OS to the
    // compositor, and that the flip completion timestamp for our just-submitted stimulus image was
    // used for calculating frameState.predictedDisplayTime for the next to-be-submitted frame.
    // This all assumes back-to-back operation and no other parallel running XR apps on the HMD,
    // and is as good as it gets, given OpenXR does not provide explicit api for querying true
    // presentation time of a past submitted frame.
    //
    // Another reasonable assumption of ours is that the predictedDisplayTime of this next frame
    // is exactly one HMD video refresh duration later than the defined display onset time of our
    // just submitted/waited on frame, ie. we get the stimulus onset timestamp of our past submitted
    // frame by subtracting one refresh duration from predictedDisplayTime. Why? Lets see what the
    // compositor has to work with:
    //
    // The compositor gets the most recent vblank/pageflip completion timestamp tLastFlip of its latest
    // compose+present cycle as a starting point from the OS. This is the time when active scanout to the
    // HMD panel started for the previous image tLastFlip. The earliest possible new submitted frame by
    // us could get latched + composited + submitted to the OS via a pageflip request during the ongoing
    // refresh cycle interval [tLastFlip; tLastFlip + videoRefreshDuration] if the stimulus image is
    // simple enough / system cpu/gpu load low enough etc. to make it before the next vblank. Its earliest
    // start of scanout to the HMD panel would be tScanout = tLastFlip + videoRefreshDuration. A HMD that
    // would immediately display every updated stimulus pixel would therefore have
    // predictedDisplayTime = tScanout + 0.5 * videoRefreshDuration, because the OpenXR
    // spec defines predictedDisplayTime not as "the top-left" pixel updating to the new frame content,
    // but the mid-point of the display updating, ie. the center of the display. This happens earliest
    // at half a refresh cycle after start of scanout tScanout.
    //
    // However, in reality, unless a high-perf OLED panel is used, predictedDisplayTime will be further delayed to:
    // predictedDisplayTime = tScanout + 0.5 * videoRefreshDuration + HMDdelay.
    // HMDdelay will depend on the specific model, technology and specs of the HMD or other XR display
    // device. It could be a few msecs pixel switching time on a LCD panel, specific to panel model.
    // It could be that, but hidden by some scanning backlight / rolling shutter mechanism, where the
    // backlight illumination onset trails the scanout position/time by some predicted pixel switching
    // delay for avoidance of artifacts.
    // It could also be that the HMD has a global shutter, e.g., a blinking backlight, which will
    // only flash the new image for a short exposure time of a millisecond or two after the whole
    // LCD panel matrix has stabilized to the new image, ie. all pixels seem to update at once, typically
    // during the vblank period at end of scanout.
    //
    // Whatever the panel tech is, LCD (slow) or OLED (fast), no shutter, rolling shutter (scanning backlight)
    // or global shutter (blinking backlight), we can summarize the compositors optimistic prediction as
    //
    // predictedDisplayTime = tScanout + delay, with delay being the unknown sum of delay = 0.5 * videoRefreshDuration + HMDdelay.
    //
    // With tScanout = tLastFlip + videoRefreshDuration, we finally get
    //
    // predictedDisplayTime = tLastFlip + videoRefreshDuration + delay.
    //
    // But we actually want the display time of our just-submitted-and-waited-on frame, which would be
    //
    // tPredictedOnset = tLastFlip + delay
    //
    // with tScanout being when it was presumably pageflipped and started scanning out to the HMD,
    // and the same HMD specific, but unknown to us, 'delay' between start of scanout and defined
    // image onset (mid-point of image) according to OpenXR spec applied.
    //
    // Iow. we have predictedDisplayTime = tLastFlip + videoRefreshDuration + delay from xrWaitFrame(),
    // but want tPredictedOnset = tLastFlip + delay. We don't know tLastFlip or delay, only the compositor
    // does. Comparing both terms and substituting one into the other shows us that:
    //
    // tPredictedOnset = predictedDisplayTime - videoRefreshDuration
    //
    // With both right-hand terms known to us, we can get the wanted tPredictedOnset. Or at least
    // this is our best guess/approximation at/of the right value, given no explicit OpenXR api or
    // extension to get what we want directly and with absolut certainty and precision.

    // Mark frame as not skipped, ie. set tPredictedOnset to greater than zero timestamp:
    tPredictedOnset = XrTimeToPsychTime(openxr->frameState.predictedDisplayTime) - openxr->frameDuration;

    result = xrBeginFrame(openxr->hmd, NULL);
    if (!resultOK(result)) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to xrBeginFrame: %s\n", errorString);

        success = FALSE;
        goto present_fail;
    }

present_out:
present_fail:

    return(success ? tPredictedOnset : -1);
}

static void* PresenterThreadMain(void* psychOpenXRDeviceToCast)
{
    int rc;
    PsychOpenXRDevice* openxr = (PsychOpenXRDevice*) psychOpenXRDeviceToCast;

    // Assign a name to ourselves, for debugging:
    PsychSetThreadName("PsychOpenXRCorePresenterThread");

    // VR compositor timeout prevention loop: Repeats infinitely, well, not infinitely,
    // but until we receive a shutdown request and terminate ourselves...
    while (TRUE) {
        // Try to lock, block until available if not available:
        if ((rc = PsychLockMutex(&(openxr->presenterLock)))) {
            // This could potentially kill the runtime, as we're printing from outside the main interpreter thread.
            // Use fprintf() instead of the overloaded printf() (aka mexPrintf()) in the hope that we don't
            // wreak havoc -- maybe it goes to the system log, which should be safer...
            fprintf(stderr, "PsychOpenXRCore-ERROR: In PresenterThreadMain(): First mutex_lock in init failed  [%s].\n", strerror(rc));
            return(NULL);
        }

        // Shall we terminate? If so break out of our main loop:
        if (openxr->closing) {
            PsychUnlockMutex(&(openxr->presenterLock));
            break;
        }

        // Non-tracked presentation mode for pure monoscopic or stereoscopic presentation
        // without use of head tracking - The HMD is just used as a strapped on stereo or
        // mono monitor, not in a closed-loop setup. Subject will just sit on a chair and
        // likely not move the head much at all. Here we expect that usercode may pass
        // target presentation times 'when' into the Screen('Flip', window, when, ...);
        // command to ask for stimulus presentation in the (potentially far) future, ie.,
        // more than a HMD video refresh duration into the future. We want to prevent the
        // compositor timeout mechanisms and performance optimizations like Asynchronous space
        // warp (ASW) to trigger, as that would totally disrupt the intended experiment timing.
        //
        // Our purpose is to prevent timeouts by executing ovr_SubmitFrame() at least once per
        // HMD refresh interval / compositor duty cycle, with the old/current frame content in
        // the swapchains.
        if ((PresentExecute(openxr, FALSE) < 0) && verbosity > 0) {
            fprintf(stderr, "PsychOpenXRCore-ERROR: In PresenterThreadMain(): PresentExecute() for timeout prevention failed!\n");
        }

        // Unlock and sleep for a frameDuration:
        PsychUnlockMutex(&(openxr->presenterLock));
        PsychYieldIntervalSeconds(openxr->frameDuration);

        // Next dispatch loop iteration...
    }

    // Exit path from thread at thread termination. Go and die peacefully...
    return(NULL);
}

PsychError PSYCHOPENXRPresentFrame(void)
{
    static char useString[] = "[tPredictedOnset, tPredictedFutureOnset, tDebugFlipTime] = PsychOpenXRCore('PresentFrame', openxrPtr [, when=0]);";
    //                          1                2                      3                                                 1            2
    static char synopsisString[] =
    "Present last rendered frame to OpenXR HMD device 'openxrPtr'.\n\n"
    "This will commit the current set of 2D textures with new rendered content "
    "to the texture swapchains, for consumption by the XR runtime/compositor, "
    "and a present is requested to the compositor. The swapchains will advance, "
    "providing new unused image textures as new render targets for the next cycle.\n\n"
    "You usually won't call this function yourself, but Screen('Flip') will call it "
    "automatically for you at the appropriate moment.\n\n"
    "'when' If provided, defines the target presentation time, as provided by Screen('Flip', win, when); "
    "A value of zero, or omission, means to present as soon as possible. The earliest possibility for such "
    "an immediate present would be the 'tPredictedFutureOnset' time from the preceeding 'PresentFrame' call.\n"
    "\n"
    "Return values:\n"
    "'tPredictedOnset' Predicted onset time for the just submitted frame, according to compositor, as "
    "Screen('Flip') return value. A value of 0 means the frame present was skipped. A value of -1 means "
    "presentation failure.\n"
    "'tPredictedFutureOnset' Predicted best-case onset time for the next/future submitted frame, according to compositor.\n"
    "'tDebugFlipTime' Assumed pageflip/start-of-scanout time for the just submitted frame, only applicable to special debug setups!\n"
    "\n\n";
    static char seeAlsoString[] = "EndFrameRender";

    int handle;
    double tWhen, tPredictedOnset, tPredictedFutureOnset;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get optional presentation target time:
    tWhen = 0;
    PsychCopyInDoubleArg(2, kPsychArgOptional, &tWhen);

    // Execute the present operation with a potentially running presenterThread locked out:
    PsychLockMutex(&(openxr->presenterLock));

    openxr->targetPresentTime = tWhen;
    tPredictedOnset = PresentExecute(openxr, FALSE);
    tPredictedFutureOnset = openxr->frameState.predictedDisplayTime;
    // Invalidate targetPresentTime after present:
    openxr->targetPresentTime = -DBL_MAX;

    PsychUnlockMutex(&(openxr->presenterLock));

    if ((tPredictedOnset < 0) && (verbosity > 0))
        printf("PsychOpenXRCore-ERROR: Failed to present new frame to VR compositor.\n");

    if ((tPredictedOnset == 0) && (verbosity > 4))
        printf("PsychOpenXRCore-INFO: Present of new frame to VR compositor skipped.\n");

    // Return our best estimate of visual stimulus onset time-point at centr of XR display:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, tPredictedOnset);

    // Return "best case scenario" predicted stimulus onset for the next/future presented frame:
    PsychCopyOutDoubleArg(2, kPsychArgOptional, tPredictedFutureOnset);

    // Return debug timestamp of assumed start of scanout to display for a zero-latency (e.g., OLED)
    // display without any shutter, global, rolling or otherwise. Mostly only useful for debugging
    // and testing the driver and OpenXR runtime in artifical test scenarios, e.g., when outputting
    // to a regular display monitor or special debug configuration:
    PsychCopyOutDoubleArg(3, kPsychArgOptional, (tPredictedOnset > 0) ? (tPredictedOnset - 0.5 * openxr->frameDuration) : tPredictedOnset);

    return(PsychError_none);
}

// TODO: Remove or repurpose for general object tracking? Not used by PsychOpenXR.m anymore:
PsychError PSYCHOPENXRGetEyePose(void)
{
    static char useString[] = "[eyePose, eyeIndex] = PsychOpenXRCore('GetEyePose', openxrPtr, renderPass [, predictionTime=nextFrame]);";
    //                          1        2                                         1          2             3
    static char synopsisString[] =
    "Return current predicted pose vector for an eye for OpenXR device 'openxrPtr'.\n"
    "'renderPass' is the view render pass for which to provide the data: 0 = First pass, 1 = Second pass.\n"
    "Eye position and orientation is predicted for target time 'predictionTime' in seconds if provided, "
    "based on the latest measurements from the tracking hardware. If 'predictionTime' is omitted or zero, "
    "then the prediction is performed for the mid-point of the next possible video frame of the HMD, ie. "
    "the most likely presentation time for immediately rendered images.\n\n"
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

    int handle, renderPass, eye;
    PsychOpenXRDevice *openxr;
    double *outM;
    double predictionTime;
    XrTime xrPredictionTime;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get renderPass:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &renderPass);
    if (renderPass < 0 || renderPass > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'renderPass' specified. Must be 0 or 1 for first or second pass.");

    // Get optional target time for predicted tracking state. Default to the
    // predicted state for the predicted mid-point of the next video frame:
    if (PsychCopyInDoubleArg(3, kPsychArgOptional, &predictionTime)) {
        xrPredictionTime = PsychTimeToXrTime(predictionTime);
    }
    else {
        xrPredictionTime = openxr->frameState.predictedDisplayTime;
        predictionTime = XrTimeToPsychTime(xrPredictionTime);
    }

    // Get eye pose for the renderPass. OpenXR does not provide advantages for seprate render passes,
    // so renderPass to eye mapping is meaningless for quality, and we just set arbitrarily eye = renderPass:
    eye = renderPass;

    PsychLockMutex(&(openxr->presenterLock));
    locateXRViews(openxr, xrPredictionTime);
    PsychUnlockMutex(&(openxr->presenterLock));

    // Eye pose as raw data:
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, 7, 1, &outM);

    // Position (x,y,z):
    outM[0] = openxr->view[eye].pose.position.x;
    outM[1] = openxr->view[eye].pose.position.y;
    outM[2] = openxr->view[eye].pose.position.z;

    // Orientation as a quaternion (x,y,z,w):
    outM[3] = openxr->view[eye].pose.orientation.x;
    outM[4] = openxr->view[eye].pose.orientation.y;
    outM[5] = openxr->view[eye].pose.orientation.z;
    outM[6] = openxr->view[eye].pose.orientation.w;

    // Copy out preferred eye render order for info:
    PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) eye);

    return(PsychError_none);
}

PsychError PSYCHOPENXRHapticPulse(void)
{
    static char useString[] = "pulseEndTime = PsychOpenXRCore('HapticPulse', openxrPtr, controllerType [, duration=2.5][, freq][, amplitude=1.0]);";
    //                         1                                             1          2                 3               4       5
    static char synopsisString[] =
    "Request execution of a haptic feedback pulse on controller 'controllerType' associated with OpenXR device 'openxrPtr'.\n\n"
    "The function will return immediately, but depending on OpenXR runtime will only take effect at some later point in time. "
    "E.g., the Oculus runtime on MS-Windows will only take action at the next Screen('Flip'). This is unfortunate.\n\n"
    "'duration' is the duration of the pulse in seconds. If 'duration' is omitted, a default duration of 2.5 seconds is used. "
    "If a 'duration' is specified, the function requests use of that duration. A duration of 0 is mapped to a runtime + hardware "
    "specific minimum duration, e.g., 0.1 seconds is a common minimum duration. Please note that depending on runtime and hardware, "
    "there might also be a maximum duration to which 'duration' will be clamped by the runtime. E.g., Oculus touch controllers under "
    "the Oculus OpenXR runtime for Microsoft Windows will not execute a haptic feedback pulse longer than 2.5 seconds.\n\n"
    "'freq' Requested frequency of the vibration: 'freq' is understood to be in Hz for 'freq' > 1, or in a normalized 0.0 - 1.0 range:\n"
    "0 = Disable ongoing pulse immediately.\n"
    "0 < freq <= 1 will be mapped to 0 Hz to 320 Hz for compatibility with other older VR api's, runtimes, and Oculus devices.\n"
    "freq > 1 will be passed unchanged as a requested frequency in Hz. The OpenXR runtimes and devices may clamp freq into a "
    "runtime and hardware dependent minimum and maximum range, or quantize actual freq to only a few discrete supported values. E.g., "
    "Oculus CV-1 touch controllers only support 160 Hz and 320 Hz, whereas Oculus S touch controllers only support 160 Hz and 500 Hz. "
    "Omitting 'freq' will choose XR_FREQUENCY_UNSPECIFIED, which leaves it at the discretion of the OpenXR runtime to choose some "
    "good frequency, for some working definition of good.\n\n"
    "'amplitude' Normalized amplitude in range 0.0 - 1.0 for vibration strength.\n\n"
    "The return argument 'pulseEndTime' contains the absolute time in seconds when the pulse is expected to end, "
    "as estimated at the time of calling the function. The precision and accuracy of pulse timing is runtime dependent "
    "ie. may vary. Pulses could last shorter or longer than specified and start with some delay, depending on system.\n";
    static char seeAlsoString[] = "";

    int handle;
    psych_int64 controllerType;
    PsychOpenXRDevice *openxr;
    double duration, tNow, freq, amplitude, pulseEndTime;
    XrPath outpath;
    XrResult result;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(5));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get the controller type:
    PsychCopyInIntegerArg64(2, kPsychArgRequired, &controllerType);

    switch (controllerType) {
        case 1: // Left hand controller? OVR.ControllerType_LTouch
            outpath = openxr->handPath[0];
            break;

        case 2: // Right hand controller? OVR.ControllerType_RTouch
            outpath = openxr->handPath[1];
            break;

        case 0xffffffff: // All active controllers? OVR.ControllerType_Active
            // XR_NULL_PATH triggers haptic action on all capable and active devices:
            outpath = XR_NULL_PATH;
            break;

        case 16: // Microsoft X-Box controller or equivalent? OVR.ControllerType_XBox
            outpath = openxr->handPath[2];
            break;

        // These types are all TODO: No-Op with warning for now.
        case 0x100: // OVR.ControllerType_Object0?
        case 0x200: // OVR.ControllerType_Object1?
        case 0x400: // OVR.ControllerType_Object2?
        case 0x800: // OVR.ControllerType_Object3?
            if (verbosity > 1)
                printf("PsychOpenXRCore-WARNING: 'HapticPulse' not yet handled for controllerType 0x%x - No haptic feedback.\n", (unsigned int) controllerType);

            PsychCopyOutDoubleArg(1, kPsychArgOptional, 0);
            return(PsychError_none);

        default:
            printf("PsychOpenXRCore-ERROR: 'HapticPulse' invalid controllerType 0x%x.\n", (unsigned int) controllerType);
            PsychErrorExitMsg(PsychError_user, "Invalid controllerType for haptic feedback specified.");
    }

    XrHapticActionInfo hapticActionInfo = {
        .type = XR_TYPE_HAPTIC_ACTION_INFO,
        .next = NULL,
        .action = openxr->hapticAction,
        .subactionPath = outpath,
    };

    // Duration:
    duration = 2.5;
    PsychCopyInDoubleArg(3, kPsychArgOptional, &duration);
    if (duration < 0)
        PsychErrorExitMsg(PsychError_user, "Invalid negative 'duration' in seconds specified. Must be positive.");

    freq = -1.0;
    if (PsychCopyInDoubleArg(4, kPsychArgOptional, &freq) && (freq < 0))
        PsychErrorExitMsg(PsychError_user, "Invalid 'freq' frequency specified. Must be greater or equal to zero.");

    amplitude = 1.0;
    PsychCopyInDoubleArg(5, kPsychArgOptional, &amplitude);
    if (amplitude < 0.0 || amplitude > 1.0)
        PsychErrorExitMsg(PsychError_user, "Invalid 'amplitude' specified. Must be in range [0.0 ; 1.0].");

    if (freq != 0) {
        // Start of haptic vibration requested:
        XrHapticVibration hapticVibration = {
            .type = XR_TYPE_HAPTIC_VIBRATION,
            .next = NULL,
            // Map 0 duration to minimum runtime + hardware supported pulse duration XR_MIN_HAPTIC_DURATION:
            .duration = (XrDuration) ((duration > 0) ? (duration * 1e9) : XR_MIN_HAPTIC_DURATION),
            // If freq was not specified, the leave the choice to the OpenXR runtime, aka XR_FREQUENCY_UNSPECIFIED.
            // If freq > 1 then pass it through as frequency in Hz.
            // We map the freq range (0 - 1] to the interval 0 - 320 Hz. The Oculus Rift CV1 touch controllers
            // only support 160 Hz and 320 Hz at the moment, so this is a reasonably backwards compatible mapping
            // of (0 -1] wrt. PsychOculusVRCore1() and PsychOpenHMDVRCore() and PsychVRHMD() semantic:
            .frequency = (float) ((freq > 0) ? ((freq > 1) ? freq : freq * 320.0) : XR_FREQUENCY_UNSPECIFIED),
            .amplitude = (float) amplitude,
        };

        // Engage!
        result = xrApplyHapticFeedback(openxr->hmd, &hapticActionInfo, (const XrHapticBaseHeader*) &hapticVibration);

        if ((result == XR_SESSION_NOT_FOCUSED) && (verbosity > 1))
            printf("PsychOpenXRCore-WARNING: 'HapticPulse' will go nowhere, as this OpenXR session does not have input focus.\n");

        if ((result == XR_SUCCESS) && (verbosity > 3))
            printf("PsychOpenXRCore-INFO: 'HapticPulse' of duration %f secs, freq %f, amplitude %f for controller of type 0x%x started.\n",
                   duration, freq, amplitude, (unsigned int) controllerType);
    }
    else {
        // Immediate stop of haptic activity requested:
        result = xrStopHapticFeedback(openxr->hmd, &hapticActionInfo);

        if ((result == XR_SESSION_NOT_FOCUSED) && (verbosity > 1))
            printf("PsychOpenXRCore-WARNING: 'HapticPulse' will not stop, if any, as this OpenXR session does not have input focus.\n");

        if ((result == XR_SUCCESS) && (verbosity > 3))
            printf("PsychOpenXRCore-INFO: Haptic feedback for controller of type 0x%x stopped.\n", (unsigned int) controllerType);
    }

    if (!resultOK(result) || (result == XR_SESSION_LOSS_PENDING)) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to %s haptic feedback pulse for controller 0x%x - xr%sHapticFeedback() failed: %s\n",
                   ((freq != 0) ? "initiate" : "stop"), (unsigned int) controllerType, ((freq != 0) ? "Apply" : "Stop"), errorString);

        PsychErrorExitMsg(PsychError_system, "Failed to control haptic feedback pulse.");
    }

    // Predict "off" time:
    PsychGetAdjustedPrecisionTimerSeconds(&pulseEndTime);
    tNow = pulseEndTime;
    pulseEndTime += duration;
    PsychCopyOutDoubleArg(1, kPsychArgOptional, pulseEndTime);

    return(PsychError_none);
}
