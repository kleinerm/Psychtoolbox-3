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
    psych_bool                          multiThreaded;
    psych_thread                        presenterThread;
    psych_mutex                         presenterLock;
    psych_condition                     presentedSignal;
    XrSystemId                          systemId;
    XrSession                           hmd;
    XrSwapchain                         textureSwapChain[2];
    uint32_t                            textureSwapChainLength[2];
    XrSwapchainImageOpenGLKHR*          textureSwapChainImages[2];
    psych_bool                          isStereo;
    int                                 textureWidth;
    int                                 textureHeight;
    int                                 maxWidth;
    int                                 maxHeight;
    int                                 recSamples;
    int                                 maxSamples;
    psych_bool                          isTracking;
    XrViewConfigurationType             viewType;
    XrExtent2Di                         texSize[2];
    XrFovf                              xrFov[2];
    XrVector3f                          eyeShift[2];
    XrView                              view[2];
    XrCompositionLayerProjectionView    projView[2];
    XrCompositionLayerQuad              quadViewLayer[2];
    XrSpace                             viewSpace;
    XrSpace                             worldSpace;
    XrCompositionLayerProjection        projectionLayer;
    XrCompositionLayerBaseHeader*       submitLayers[2];
    uint32_t                            submitLayersCount;
    uint32_t                            frameIndex;
    int                                 commitFrameIndex;
    int                                 needSubmit;
    //ovrPosef                          outEyePoses[2];
    double                              frameDuration;
    double                              sensorSampleTime;
    double                              lastPresentExecTime;
    double                              scheduledPresentExecTime;
    double                              VRtimeoutSecs;
    //ovrPerfStats              perfStats;
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

static int verbosity = 100;
static psych_bool initialized = FALSE;
static char errorString[XR_MAX_RESULT_STRING_SIZE];

static void* PresenterThreadMain(void* psychOpenXRDeviceToCast);
static double PresentExecute(PsychOpenXRDevice *openxr, psych_bool commitTextures, psych_bool onlyCommit);

// XR_EXT_debug_utils for runtime debug output:
PFN_xrCreateDebugUtilsMessengerEXT pxrCreateDebugUtilsMessengerEXT = NULL;
PFN_xrDestroyDebugUtilsMessengerEXT pxrDestroyDebugUtilsMessengerEXT = NULL;

// XR_KHR_opengl_enable for OpenGL rendering and interop:
PFN_xrGetOpenGLGraphicsRequirementsKHR pxrGetOpenGLGraphicsRequirementsKHR = NULL;

// XR_FB_display_refresh_rate for HMD video refresh rate query and control:
PFN_xrGetDisplayRefreshRateFB pxrGetDisplayRefreshRateFB = NULL;

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
    //synopsis[i++] = "PsychOpenXRCore('SetHUDState', openxrPtr , mode);";
    //synopsis[i++] = "[isVisible, playboundsxyz, outerboundsxyz] = PsychOpenXRCore('VRAreaBoundary', openxrPtr [, requestVisible]);";
    //synopsis[i++] = "[isTriggering, closestDistance, closestPointxyz, surfaceNormal] = PsychOpenXRCore('TestVRBoundary', openxrPtr, trackedDeviceType, boundaryType);";
    //synopsis[i++] = "[isTriggering, closestDistance, closestPointxyz, surfaceNormal] = PsychOpenXRCore('TestVRBoundaryPoint', openxrPtr, pointxyz, boundaryType);";
    //synopsis[i++] = "success = PsychOpenXRCore('RecenterTrackingOrigin', openxrPtr);";
    //synopsis[i++] = "oldType = PsychOpenXRCore('TrackingOriginType', openxrPtr [, newType]);";
    //synopsis[i++] = "PsychOpenXRCore('Start', openxrPtr);";
    //synopsis[i++] = "PsychOpenXRCore('Stop', openxrPtr);";
    //synopsis[i++] = "[state, touch] = PsychOpenXRCore('GetTrackingState', openxrPtr [, predictionTime=nextFrame]);";
    //synopsis[i++] = "input = PsychOpenXRCore('GetInputState', openxrPtr, controllerType);";
    //synopsis[i++] = "pulseEndTime = PsychOpenXRCore('HapticPulse', openxrPtr, controllerType [, duration=2.5][, freq=1.0][, amplitude=1.0]);";
    //synopsis[i++] = "[projL, projR] = PsychOpenXRCore('GetStaticRenderParameters', openxrPtr [, clipNear=0.01][, clipFar=10000.0]);";
    //synopsis[i++] = "[eyePoseL, eyePoseR, tracked, frameTiming] = PsychOpenXRCore('StartRender', openxrPtr);";
    //synopsis[i++] = "[eyePose, eyeIndex] = PsychOpenXRCore('GetEyePose', openxrPtr, renderPass);";
    //synopsis[i++] = "[adaptiveGpuPerformanceScale, frameStats, anyFrameStatsDropped, aswIsAvailable] = PsychOpenXRCore('GetPerformanceStats', openxrPtr);";
    //synopsis[i++] = "oldValues = PsychOpenXRCore('FloatsProperty', openxrPtr, propertyName [, newValues]);";
    //synopsis[i++] = "oldValues = PsychOpenXRCore('FloatProperty', openxrPtr, propertyName [, newValue]);";
    //synopsis[i++] = "oldString = PsychOpenXRCore('StringProperty', openxrPtr, propertyName [, defaultString][, newString]);";
    synopsis[i++] = "";
    synopsis[i++] = "Functions usually only used internally by Psychtoolbox:";
    synopsis[i++] = "";
    synopsis[i++] = "[width, height, recMSAASamples, fovPort, maxWidth, maxHeight, maxMSAASamples] = PsychOpenXRCore('GetFovTextureSize', openxrPtr, eye [, fov=[HMDRecommended]][, pixelsPerDisplay=1]);";
    synopsis[i++] = "[hmdShiftx, hmdShifty, hmdShiftz] = PsychOpenXRCore('GetUndistortionParameters', openxrPtr, eye);";
    synopsis[i++] = "[videoRefreshDuration] = PsychOpenXRCore('CreateAndStartSession', openxrPtr, deviceContext, openGLContext, openGLDrawable, openGLConfig, openGLVisualId);";
    synopsis[i++] = "[width, height, numTextures, imageFormat] = PsychOpenXRCore('CreateRenderTextureChain', openxrPtr, eye, width, height, floatFormat, numMSAASamples);";
    synopsis[i++] = "texObjectHandle = PsychOpenXRCore('GetNextTextureHandle', openxrPtr, eye);";
    synopsis[i++] = "PsychOpenXRCore('EndFrameRender', openxrPtr [, eye]);";
    //synopsis[i++] = "trackers = PsychOpenXRCore('GetTrackersState', openxrPtr);";
    //synopsis[i++] = "[frameTiming, tPredictedOnset, referenceFrameIndex] = PsychOpenXRCore('PresentFrame', openxrPtr [, doTimestamp=0][, when=0]);";
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
/*
    // Initialize eye poses to an identity mapping, where both eyes
    // just stare straight ahead from the origin. All values need to
    // be zero - provided by memset zero init below - except
    // for the w component of the Orientation quaternion.
    // This provides proper head-locking of frames submitted to the XR
    // compositor if rendering without head tracking is in use, e.g., for
    // standard 2D stereo rendering, movie playback etc., or for (ab)use
    // of the HMD as a "strapped onto the head" mono- or stereo-monitor:
    memset(openxr->outEyePoses, 0, 2 * sizeof(openxr->outEyePoses[0]));
    openxr->outEyePoses[0].Orientation.w = 1;
    openxr->outEyePoses[1].Orientation.w = 1;
*/
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
        PresentExecute(openxr, FALSE, FALSE);

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

    // Initialize eye poses to an identity mapping, where both eyes
    // just stare straight ahead from the origin. All values need to
    // be zero - provided by zero init of the openxr struct - except
    // for the w component of the Orientation quaternion.
    // This provides proper head-locking of frames submitted to the VR
    // compositor if rendering without head tracking is in use, e.g., for
    // standard 2D stereo rendering, movie playback etc., or for (ab)use
    // of the HMD as a "strapped onto the head" mono- or stereo-monitor:
    // TODO   openxr->outEyePoses[0].Orientation.w = 1;
    //    openxr->outEyePoses[1].Orientation.w = 1;

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
PsychError PSYCHOPENXRRecenterTrackingOrigin(void)
{
    static char useString[] = "success = PsychOpenXRCore('RecenterTrackingOrigin', openxrPtr);";
    //                         1                                                      1
    static char synopsisString[] =
        "Recenter the tracking origin for OpenXR device 'openxrPtr'.\n"
        "This uses the current pose of the HMD to redefine the tracking origin.\n"
        "Returns 1 on success, 0 on failure.\n"
        "A reason for failure can be that the HMD is not roughly level, but "
        "instead is facing upward or downward, which is not allowed.\n";
    static char seeAlsoString[] = "GetTrackersState TrackingOriginType";

    int handle;
    PsychOpenXRDevice *openxr;

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
/*
    if (OVR_FAILURE(ovr_RecenterTrackingOrigin(openxr->hmd))) {
        if (verbosity > 0) {
            ovr_GetLastErrorInfo(&errorInfo);
            printf("PsychOpenXRCore-ERROR: Recentering the tracking origin failed! %s\n", errorInfo.ErrorString);
        }
        PsychCopyOutDoubleArg(1, kPsychArgOptional, 0);
    }
    else {
        if (verbosity > 2) {
            printf("PsychOpenXRCore-INFO: Recentered the tracking origin for HMD.\n");
        }
        PsychCopyOutDoubleArg(1, kPsychArgOptional, 1);

        ovr_ClearShouldRecenterFlag(openxr->hmd);
    }
*/
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

// TODO
PsychError PSYCHOPENXRVRAreaBoundary(void)
{
    static char useString[] = "[isVisible, playboundsxyz, outerboundsxyz] = PsychOpenXRCore('VRAreaBoundary', openxrPtr [, requestVisible]);";
    //                          1          2              3                                                      1            2
    static char synopsisString[] =
        "Request visualization of the VR play area boundary for OpenXR device 'openxrPtr' and returns its current extents.\n\n"
        "'requestVisible' 1 = Request showing the boundary area markers, 0 = Don't request showing the markers.\n"
        "The driver can not prevent the boundaries to be visualized if some external setting asks for "
        "their visibility. It can cancel its own request for visibility though via 'requestVisible' setting 0.\n\n"
        "Returns in 'isVisible' the current visibility status of the VR area boundaries.\n\n"
        "'playboundsxyz' is a 3-by-n matrix defining the play area boundaries. Each column represents "
        "the [x;y;z] coordinates of one 3D definition point. Connecting successive points by line segments "
        "defines the boundary, as projected onto the floor. Points are listed in clock-wise direction. "
        "An empty return argument means that the play area is so far undefined.\n\n"
        "'outerboundsxyz' defines the outer area boundaries in the same way as 'playboundsxyz'.\n";
    static char seeAlsoString[] = "TestVRBoundary TestVRBoundaryPoint";
/*
    int handle, requestVisible;
    ovrBool isVisible;
    PsychOpenXRDevice *openxr;
    int outFloorPointsCount, i;
    ovrVector3f *outFloorPoints;
    double *xyz;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get optional 'requestVisible' flag:
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &requestVisible)) {
        if (OVR_FAILURE(ovr_RequestBoundaryVisible(openxr->hmd, (requestVisible) ? TRUE : FALSE)))
            PsychErrorExitMsg(PsychError_system, "Failed to set request for visibility of VR boundary area.");
    }

    // Copy out current visibility status:
    if (OVR_FAILURE(ovr_GetBoundaryVisible(openxr->hmd, &isVisible)) && (verbosity > 0))
        printf("PsychOpenXRCore-ERROR:VRAreaBoundary: Could not query VR area boundary visibility status! Results is undefined.\n");
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) ((isVisible) ? 1 : 0));

    // Copy out play area boundaries: A matrix of 2D points which define the boundary
    // in clock-wise order at floor height:
    if (OVR_FAILURE(ovr_GetBoundaryGeometry(openxr->hmd, ovrBoundary_PlayArea, (ovrVector3f*) NULL, &outFloorPointsCount))) {
        PsychErrorExitMsg(PsychError_system, "Failed to get VR boundary area I.");
    }

    outFloorPoints = (ovrVector3f*) PsychMallocTemp(outFloorPointsCount * sizeof(ovrVector3f));
    if (OVR_FAILURE(ovr_GetBoundaryGeometry(openxr->hmd, ovrBoundary_PlayArea, outFloorPoints, &outFloorPointsCount))) {
        PsychErrorExitMsg(PsychError_system, "Failed to get VR boundary area II.");
    }

    PsychAllocOutDoubleMatArg(2, kPsychArgOptional, 3, outFloorPointsCount, 1, &xyz);
    for (i = 0 ; i < outFloorPointsCount; i++) {
        *(xyz++) = (double) outFloorPoints[i].x;
        *(xyz++) = (double) outFloorPoints[i].y;
        *(xyz++) = (double) outFloorPoints[i].z;
    }

    // Copy out outer area boundaries: A matrix of 3D points which define the boundary
    // in clock-wise order at floor height:
    if (OVR_FAILURE(ovr_GetBoundaryGeometry(openxr->hmd, ovrBoundary_Outer, (ovrVector3f*) NULL, &outFloorPointsCount))) {
        PsychErrorExitMsg(PsychError_system, "Failed to get VR boundary area III.");
    }

    outFloorPoints = (ovrVector3f*) PsychMallocTemp(outFloorPointsCount * sizeof(ovrVector3f));
    if (OVR_FAILURE(ovr_GetBoundaryGeometry(openxr->hmd, ovrBoundary_Outer , outFloorPoints, &outFloorPointsCount))) {
        PsychErrorExitMsg(PsychError_system, "Failed to get VR boundary area IV.");
    }

    PsychAllocOutDoubleMatArg(3, kPsychArgOptional, 3, outFloorPointsCount, 1, &xyz);
    for (i = 0 ; i < outFloorPointsCount; i++) {
        *(xyz++) = (double) outFloorPoints[i].x;
        *(xyz++) = (double) outFloorPoints[i].y;
        *(xyz++) = (double) outFloorPoints[i].z;
    }
*/
    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRTestVRBoundary(void)
{
    static char useString[] = "[isTriggering, closestDistance, closestPointxyz, surfaceNormal] = PsychOpenXRCore('TestVRBoundary', openxrPtr, trackedDeviceType, boundaryType);";
    //                          1             2                3                4                                                     1          2                  3
    static char synopsisString[] =
        "Return if a tracked device associated with OpenXR device 'openxrPtr' is colliding with VR area boundaries.\n\n"
        "'trackedDeviceType' Bitmask of tracked devices to check for collision. See OVR.TrackedDeviceType_XXX constants.\n\n"
        "'boundaryType' Type of VR area boundary to test against: 0 = Play area, 1 = Outer area.\n\n"
        "Return values, assuming the specified device and areas are active and/or defined:\n"
        "'isTriggering' The boundary is triggered, ie. the guardian system shows boundaries to the subject, "
        "because one of the tracked devices is close enough to a VR area boundary.\n"
        "'closestDistance' Closest distance to specified VR area boundary.\n"
        "'closestPointxyz' [x;y;z] coordinates of the closest point on the VR area boundary.\n"
        "'surfaceNormal' [nx;ny;nz] surface normal of the VR area boundary surface at 'closestPointxyz'.\n\n"
        "Note that the guardian system must be set up properly for this function to return meaningful results.\n";
    static char seeAlsoString[] = "VRAreaBoundary TestVRBoundaryPoint";
/*
    int handle, trackedDeviceType, boundaryType;
    PsychOpenXRDevice *openxr;
    ovrBoundaryTestResult outTestResult;
    double *xyz;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(4));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(3));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Type of device:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &trackedDeviceType);

    // Type of boundary:
    PsychCopyInIntegerArg(3, kPsychArgRequired, &boundaryType);
    if (boundaryType < 0 || boundaryType > 1)
        PsychErrorExitMsg(PsychError_user, "Invalid 'boundaryType' provided. Must be 0 or 1.");

    // Check for collision:
    if (OVR_FAILURE(ovr_TestBoundary(openxr->hmd, (ovrTrackedDeviceType) trackedDeviceType,
                    (boundaryType == 1) ? ovrBoundary_Outer : ovrBoundary_PlayArea,
                    &outTestResult))) {
        PsychErrorExitMsg(PsychError_system, "ovr_TestBoundary() failed.");
    }

    // Collision detected?
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) ((outTestResult.IsTriggering) ? 1 : 0));

    // Distance to closest point - the one that triggers:
    PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) outTestResult.ClosestDistance);

    // 3D closest point:
    PsychAllocOutDoubleMatArg(3, kPsychArgOptional, 3, 1, 1, &xyz);
    *(xyz++) = (double) outTestResult.ClosestPoint.x;
    *(xyz++) = (double) outTestResult.ClosestPoint.y;
    *(xyz++) = (double) outTestResult.ClosestPoint.z;

    // 3D surface normal (nx,ny,nz) of closest point:
    PsychAllocOutDoubleMatArg(4, kPsychArgOptional, 3, 1, 1, &xyz);
    *(xyz++) = (double) outTestResult.ClosestPointNormal.x;
    *(xyz++) = (double) outTestResult.ClosestPointNormal.y;
    *(xyz++) = (double) outTestResult.ClosestPointNormal.z;
*/
    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRTestVRBoundaryPoint(void)
{
    static char useString[] = "[isTriggering, closestDistance, closestPointxyz, surfaceNormal] = PsychOpenXRCore('TestVRBoundaryPoint', openxrPtr, pointxyz, boundaryType);";
    //                          1             2                3                4                                                          1          2         3
    static char synopsisString[] =
        "Return if a 3D point is colliding with VR area boundaries associated with OpenXR device 'openxrPtr'.\n\n"
        "'pointxyz' [x,y,z] vector defining the 3D point position to test against the boundaries.\n\n"
        "'boundaryType' Type of VR area boundary to test against: 0 = Play area, 1 = Outer area.\n\n"
        "Return values, assuming the specified area is defined:\n"
        "'isTriggering' The boundary is triggered, ie. the guardian system would show boundaries for given 'pointxyz'.\n"
        "'closestDistance' Closest distance of point to specified VR area boundary.\n"
        "'closestPointxyz' [x;y;z] coordinates of the closest point on the VR area boundary.\n"
        "'surfaceNormal' [nx;ny;nz] surface normal of the VR area boundary surface at 'closestPointxyz'.\n\n"
        "Note that the guardian system must be set up properly for this function to return meaningful results.\n";
    static char seeAlsoString[] = "VRAreaBoundary TestVRBoundary";
/*
    int handle, boundaryType;
    PsychOpenXRDevice *openxr;
    ovrBoundaryTestResult outTestResult;
    ovrVector3f point;
    int m, n, p;
    double *xyz;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(4));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(3));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Test point:
    PsychAllocInDoubleMatArg(2, kPsychArgRequired, &m, &n, &p, &xyz);
    if (m * n * p != 3)
        PsychErrorExitMsg(PsychError_user, "Invalid 'pointxyz' provided. Must be a 3D vector [x,y,z].");

    point.x = (float) *(xyz++);
    point.y = (float) *(xyz++);
    point.z = (float) *(xyz++);

    // Type of boundary:
    PsychCopyInIntegerArg(3, kPsychArgRequired, &boundaryType);
    if (boundaryType < 0 || boundaryType > 1)
        PsychErrorExitMsg(PsychError_user, "Invalid 'boundaryType' provided. Must be 0 or 1.");

    // Check for collision:
    if (OVR_FAILURE(ovr_TestBoundaryPoint(openxr->hmd, (const ovrVector3f*) &point,
                    (boundaryType == 1) ? ovrBoundary_Outer : ovrBoundary_PlayArea,
                    &outTestResult))) {
        PsychErrorExitMsg(PsychError_system, "ovr_TestBoundaryPoint() failed.");
    }

    // Collision detected?
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) ((outTestResult.IsTriggering) ? 1 : 0));

    // Distance to closest point - the one that triggers:
    PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) outTestResult.ClosestDistance);

    // 3D closest point:
    PsychAllocOutDoubleMatArg(3, kPsychArgOptional, 3, 1, 1, &xyz);
    *(xyz++) = (double) outTestResult.ClosestPoint.x;
    *(xyz++) = (double) outTestResult.ClosestPoint.y;
    *(xyz++) = (double) outTestResult.ClosestPoint.z;

    // 3D surface normal (nx,ny,nz) of closest point:
    PsychAllocOutDoubleMatArg(4, kPsychArgOptional, 3, 1, 1, &xyz);
    *(xyz++) = (double) outTestResult.ClosestPointNormal.x;
    *(xyz++) = (double) outTestResult.ClosestPointNormal.y;
    *(xyz++) = (double) outTestResult.ClosestPointNormal.z;
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

// TODO
PsychError PSYCHOPENXRGetTrackingState(void)
{
    static char useString[] = "[state, touch] = PsychOpenXRCore('GetTrackingState', openxrPtr [, predictionTime=nextFrame]);";
    //                          1      2                                               1            2
    static char synopsisString[] =
        "Return current state of head position and orientation tracking for OpenXR device 'openxrPtr'.\n"
        "Head position and orientation is predicted for target time 'predictionTime' in seconds if provided, "
        "based on the latest measurements from the tracking hardware. If 'predictionTime' is set to zero, "
        "then no prediction is performed and the current state based on latest measurements is returned.\n"
        "If 'predictionTime' is omitted, then the prediction is performed for the mid-point of the next "
        "possible video frame of the HMD, ie. the most likely presentation time for immediately rendered images.\n\n"
        "'state' is a struct with fields reporting the following values:\n"
        "'Time' = Time in seconds of returned tracking state.\n"
        "'Status' = Tracking status flags:\n"
        "+1 = Head orientation tracked,\n"
        "+2 = Head position tracked,\n"
        "+4 = At least one active tracking camera has a valid pose for absolute tracking,\n"
        "+32 = At least one tracking camera is connected and online,\n"
        "+128 = HMD display is connected, available and actually on users head, displaying our content.\n\n"
        "'SessionState' = VR session status flags, added together:\n"
        "+1  = Our rendering goes to the HMD, ie. we have control over it. If the OpenXR store app would "
        "be in control, or if the Health and Safety warning would display, this flag would be missing.\n"
        "+2  = HMD is present and active.\n"
        "+4  = HMD is strapped onto users head. A Rift CV1 would switch off/blank if not on the head.\n"
        "+8  = DisplayLost condition! Some hardware/software malfunction, need to completely quit to recover.\n"
        "+16 = ShouldQuit The user interface asks us to voluntarily terminate this session.\n"
        "+32 = ShouldRecenter = The user interface asks us to recenter/recalibrate our tracking origin.\n\n"
        "'HeadPose' = Head position [x, y, z] in meters and rotation as quaternion [rx, ry, rz, rw], all as a vector [x,y,z,rx,ry,rz,rw].\n"
        "'HeadLinearSpeed' = Head linear velocity [vx,vy,vz] in meters/sec.\n"
        "'HeadAngularSpeed' = Head angular velocity [rx,ry,rz] in radians/sec.\n"
        "'HeadLinearAcceleration' = Head linear acceleration [ax,ay,az] in meters/sec^2.\n"
        "'HeadAngularAcceleration' = Head angular acceleration [rax,ray,raz] in radians/sec^2.\n"
        "'CalibratedOrigin' = The pose of the world coordinate system origin during last calibration. "
        "Units and format are like 'HeadPose' ie. a vector [x,y,z,rx,ry,rz,rw].\n"
        "\n"
        "Touch controller position and orientation:\n\n"
        "The return argument 'touch' is a struct array with 2 structs. touch(1) contains info about "
        "the tracking state and tracked pose of the left hand (= left touch controller) of the user, "
        "touch(2) contains info about the right hand (= right touch controller) of the user.\n"
        "The structs have very similar structure to the head (= HMD) tracking data returned by 'state':\n\n"
        "'Time' = Time in seconds of returned hand/controller tracking state.\n"
        "'Status' = Tracking status flags:\n"
        "0  = No tracking info for hand/controller, ie. no OpenXR touch sensor connected.\n"
        "+1 = Hand orientation tracked,\n"
        "+2 = Hand position tracked,\n"
        "'HandPose' = Position and orientation of the hand, in usual [x,y,z,rx,ry,rz,rw] vector form as with 'HeadPose'.\n"
        "'HandLinearSpeed' = Hand linear velocity [vx,vy,vz] in meters/sec.\n"
        "'HandAngularSpeed' = Hand angular velocity [rx,ry,rz] in radians/sec.\n"
        "'HandLinearAcceleration' = Hand linear acceleration [ax,ay,az] in meters/sec^2.\n"
        "'HandAngularAcceleration' = Hand angular acceleration [rax,ray,raz] in radians/sec^2.\n"
        "\n";

    static char seeAlsoString[] = "Start Stop GetTrackersState GetInputState";

    PsychGenericScriptType *status;
    const char *FieldNames1[] = {"Time", "Status", "SessionState", "HeadPose", "HeadLinearSpeed", "HeadAngularSpeed",
                                 "HeadLinearAcceleration", "HeadAngularAcceleration", "CalibratedOrigin"};
    const int FieldCount1 = 9;

    const char *FieldNames2[] = {"Time", "Status", "HandPose", "HandLinearSpeed", "HandAngularSpeed",
                                 "HandLinearAcceleration", "HandAngularAcceleration"};
    const int FieldCount2 = 7;
/*
    PsychGenericScriptType *outMat;
    double *v;
    int handle, trackerCount, i;
    double predictionTime;
    int StatusFlags = 0;
    PsychOpenXRDevice *openxr;
    ovrTrackingState state;
    ovrVector3f HmdToEyeOffset[2];
    ovrTrackerPose trackerPose;
    ovrSessionStatus sessionStatus;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    if (OVR_FAILURE(ovr_GetSessionStatus(openxr->hmd, &sessionStatus))) {
        ovr_GetLastErrorInfo(&errorInfo);
        if (verbosity > 0) printf("PsychOpenXRCore-ERROR: ovr_GetSessionStatus failed: %s\n", errorInfo.ErrorString);
        PsychErrorExitMsg(PsychError_system, "Failed to get current session status from VR compositor.");
    }

    PsychLockMutex(&(openxr->presenterLock));

    // Get optional target time for predicted tracking state. Default to the
    // predicted state for the predicted mid-point of the next video frame:
    if (!PsychCopyInDoubleArg(2, kPsychArgOptional, &predictionTime))
        predictionTime = ovr_GetPredictedDisplayTime(openxr->hmd, openxr->frameIndex);

    // Get current tracking status info at time predictionTime. Mark this point
    // as time from which motion to photon latency is measured (latencymarker = TRUE):
    state = ovr_GetTrackingState(openxr->hmd, predictionTime, TRUE);
    openxr->sensorSampleTime = ovr_GetTimeInSeconds();

    // Translate to per eye position and orientation:
    HmdToEyeOffset[0] = openxr->eyeRenderDesc[0].HmdToEyeOffset;
    HmdToEyeOffset[1] = openxr->eyeRenderDesc[1].HmdToEyeOffset;
    ovr_CalcEyePoses(state.HeadPose.ThePose, HmdToEyeOffset, openxr->outEyePoses);
    PsychUnlockMutex(&(openxr->presenterLock));

    // Print out tracking status:
    if (verbosity >= 4) {
        printf("PsychOpenXRCore-INFO: Tracking state predicted for device %i at time %f.\n", handle, predictionTime);
        printf("PsychOpenXRCore-INFO: Time %f : Status %i\n", state.HeadPose.TimeInSeconds, state.StatusFlags);
        printf("PsychOpenXRCore-INFO: HeadPose: Position    [x,y,z]   = [%f, %f, %f]\n", state.HeadPose.ThePose.Position.x, state.HeadPose.ThePose.Position.y, state.HeadPose.ThePose.Position.z);
        printf("PsychOpenXRCore-INFO: HeadPose: Orientation [x,y,z,w] = [%f, %f, %f, %f]\n", state.HeadPose.ThePose.Orientation.x, state.HeadPose.ThePose.Orientation.y, state.HeadPose.ThePose.Orientation.z, state.HeadPose.ThePose.Orientation.w);
    }

    PsychAllocOutStructArray(1, kPsychArgOptional, 1, FieldCount1, FieldNames1, &status);

    PsychSetStructArrayDoubleElement("Time", 0, state.HeadPose.TimeInSeconds, status);

    StatusFlags = state.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked);

    // HMD present, connected and online and on users head, displaying us?
    if (sessionStatus.HmdPresent && sessionStatus.IsVisible && sessionStatus.HmdMounted)
        StatusFlags |= 128;

    // At least one tracker connected?
    trackerCount = ovr_GetTrackerCount(openxr->hmd);
    for (i = 0; i < trackerCount; i++) {
        trackerPose = ovr_GetTrackerPose(openxr->hmd, i);
        StatusFlags |= trackerPose.TrackerFlags & (ovrTracker_Connected | ovrTracker_PoseTracked);
    }

    // Return head and general tracking status flags:
    PsychSetStructArrayDoubleElement("Status", 0, StatusFlags, status);

    // Return sesstion status flags:
    StatusFlags = 0;
    if (sessionStatus.IsVisible) StatusFlags |= 1;
    if (sessionStatus.HmdPresent) StatusFlags |= 2;
    if (sessionStatus.HmdMounted) StatusFlags |= 4;
    if (sessionStatus.DisplayLost) StatusFlags |= 8;
    if (sessionStatus.ShouldQuit) StatusFlags |= 16;
    if (sessionStatus.ShouldRecenter) StatusFlags |= 32;

    PsychSetStructArrayDoubleElement("SessionState", 0, StatusFlags, status);

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

    // CalibratedOrigin:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);
    v[0] = state.CalibratedOrigin.Position.x;
    v[1] = state.CalibratedOrigin.Position.y;
    v[2] = state.CalibratedOrigin.Position.z;

    v[3] = state.CalibratedOrigin.Orientation.x;
    v[4] = state.CalibratedOrigin.Orientation.y;
    v[5] = state.CalibratedOrigin.Orientation.z;
    v[6] = state.CalibratedOrigin.Orientation.w;
    PsychSetStructArrayNativeElement("CalibratedOrigin", 0, outMat, status);

    // Now the tracking info from the OpenXR touch controllers 0 and 1 for left
    // and right hand, in a separate struct array:
    PsychAllocOutStructArray(2, kPsychArgOptional, 2, FieldCount2, FieldNames2, &status);

    for (i = 0; i < 2; i++) {
        // Timestamp for when this tracking info is valid:
        PsychSetStructArrayDoubleElement("Time", i, state.HandPoses[i].TimeInSeconds, status);

        // Hand / touch controller tracking state:
        StatusFlags = state.HandStatusFlags[i] & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked);
        PsychSetStructArrayDoubleElement("Status", i, StatusFlags, status);

        // Hand pose:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);
        v[0] = state.HandPoses[i].ThePose.Position.x;
        v[1] = state.HandPoses[i].ThePose.Position.y;
        v[2] = state.HandPoses[i].ThePose.Position.z;

        v[3] = state.HandPoses[i].ThePose.Orientation.x;
        v[4] = state.HandPoses[i].ThePose.Orientation.y;
        v[5] = state.HandPoses[i].ThePose.Orientation.z;
        v[6] = state.HandPoses[i].ThePose.Orientation.w;
        PsychSetStructArrayNativeElement("HandPose", i, outMat, status);

        // Linear velocity:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
        v[0] = state.HandPoses[i].LinearVelocity.x;
        v[1] = state.HandPoses[i].LinearVelocity.y;
        v[2] = state.HandPoses[i].LinearVelocity.z;
        PsychSetStructArrayNativeElement("HandLinearSpeed", i, outMat, status);

        // Angular velocity:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
        v[0] = state.HandPoses[i].AngularVelocity.x;
        v[1] = state.HandPoses[i].AngularVelocity.y;
        v[2] = state.HandPoses[i].AngularVelocity.z;
        PsychSetStructArrayNativeElement("HandAngularSpeed", i, outMat, status);

        // Linear acceleration:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
        v[0] = state.HandPoses[i].LinearAcceleration.x;
        v[1] = state.HandPoses[i].LinearAcceleration.y;
        v[2] = state.HandPoses[i].LinearAcceleration.z;
        PsychSetStructArrayNativeElement("HandLinearAcceleration", i, outMat, status);

        // Angular acceleration:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 3, 1, &v, &outMat);
        v[0] = state.HandPoses[i].AngularAcceleration.x;
        v[1] = state.HandPoses[i].AngularAcceleration.y;
        v[2] = state.HandPoses[i].AngularAcceleration.z;
        PsychSetStructArrayNativeElement("HandAngularAcceleration", i, outMat, status);
    }
*/
    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRGetInputState(void)
{
    static char useString[] = "input = PsychOpenXRCore('GetInputState', openxrPtr, controllerType);";
    //                         1                                           1          2
    static char synopsisString[] =
        "Return current state of input device 'controllerType' associated with OpenXR device 'openxrPtr'.\n\n"
        "'controllerType' can be one of the follwing values:\n"
        "OVR.ControllerType_LTouch = Left touch controller (Left tracked hand).\n"
        "OVR.ControllerType_RTouch = Right touch controller (Right tracked hand).\n"
        "OVR.ControllerType_Remote = OpenXR remote control.\n"
        "OVR.ControllerType_XBox = XBox controller.\n"
        "OVR.ControllerType_Active = Whatever controller is connected and active.\n"
        "\n"
        "'input' is a struct with fields reporting the following status values of the controller:\n"
        "'Valid' = 1 if 'input' contains valid results, 0 if input status is invalid/unavailable.\n"
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

    static char seeAlsoString[] = "Start Stop GetTrackedState GetTrackersState";

    PsychGenericScriptType *status;
    const char *FieldNames[] = { "Valid", "Time", "Buttons", "Touches", "Trigger", "Grip", "TriggerNoDeadzone",
                                 "GripNoDeadzone", "TriggerRaw", "GripRaw", "Thumbstick",
                                 "ThumbstickNoDeadzone", "ThumbstickRaw" };
    const int FieldCount = 13;
/*
    PsychGenericScriptType *outMat;
    double *v;
    int handle, i;
    unsigned long controllerType;
    double controllerTypeD;
    PsychOpenXRDevice *openxr;
    ovrInputState state;
    ovrSessionStatus sessionStatus;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Controller type:
    PsychCopyInDoubleArg(2, kPsychArgRequired, &controllerTypeD);
    controllerType = (unsigned long) controllerTypeD;

    PsychAllocOutStructArray(1, kPsychArgOptional, 1, FieldCount, FieldNames, &status);

    // Check session status if we have VR input focus:
    if (OVR_FAILURE(ovr_GetSessionStatus(openxr->hmd, &sessionStatus))) {
        ovr_GetLastErrorInfo(&errorInfo);
        if (verbosity > 0) printf("PsychOpenXRCore-ERROR: ovr_GetSessionStatus failed: %s\n", errorInfo.ErrorString);
        PsychErrorExitMsg(PsychError_system, "Failed to get current session status from VR compositor.");
    }

    if (sessionStatus.IsVisible) {
        // Have VR input focus, try to get input state:
        if (OVR_FAILURE(ovr_GetInputState(openxr->hmd, (ovrControllerType) controllerType, &state))) {
            ovr_GetLastErrorInfo(&errorInfo);
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: ovr_GetInputState() for controller %f 0x%lx failed: %s\n",
                       controllerTypeD, controllerType, errorInfo.ErrorString);
            PsychErrorExitMsg(PsychError_system, "Failed to get some controller input status.");
        }

        // Mark as valid:
        PsychSetStructArrayDoubleElement("Valid", 0, 1, status);
    }
    else {
        // No VR input focus -> No valid input -> Mark as invalid:
        PsychSetStructArrayDoubleElement("Valid", 0, 0, status);

        // Fail gracefully, by returning an all-zero input struct:
        memset(&state, 0, sizeof(state));
    }

    // Controller update time:
    PsychSetStructArrayDoubleElement("Time", 0, state.TimeInSeconds, status);

    // Button states:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 32, 1, &v, &outMat);
    for (i = 0; i < 32; i++)
        v[i] = (state.Buttons & (1 << i)) ? 1 : 0;
    PsychSetStructArrayNativeElement("Buttons", 0, outMat, status);

    // Touch states:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 32, 1, &v, &outMat);
    for (i = 0; i < 32; i++)
        v[i] = (state.Touches & (1 << i)) ? 1 : 0;
    PsychSetStructArrayNativeElement("Touches", 0, outMat, status);

    // Trigger left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = state.IndexTrigger[0];
    v[1] = state.IndexTrigger[1];
    PsychSetStructArrayNativeElement("Trigger", 0, outMat, status);

    // Grip left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = state.HandTrigger[0];
    v[1] = state.HandTrigger[1];
    PsychSetStructArrayNativeElement("Grip", 0, outMat, status);

    // TriggerNoDeadzone left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = state.IndexTriggerNoDeadzone[0];
    v[1] = state.IndexTriggerNoDeadzone[1];
    PsychSetStructArrayNativeElement("TriggerNoDeadzone", 0, outMat, status);

    // GripNoDeadzone left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = state.HandTriggerNoDeadzone[0];
    v[1] = state.HandTriggerNoDeadzone[1];
    PsychSetStructArrayNativeElement("GripNoDeadzone", 0, outMat, status);

    // TriggerRaw left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = state.IndexTriggerRaw[0];
    v[1] = state.IndexTriggerRaw[1];
    PsychSetStructArrayNativeElement("TriggerRaw", 0, outMat, status);

    // GripRaw left/right:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
    v[0] = state.HandTriggerRaw[0];
    v[1] = state.HandTriggerRaw[1];
    PsychSetStructArrayNativeElement("GripRaw", 0, outMat, status);

    // Thumbstick: column 1 = left hand, column 2 = right hand. row 1 = x, row 2= y:
    v = NULL;
    PsychAllocateNativeDoubleMat(2, 2, 1, &v, &outMat);
    v[0] = state.Thumbstick[0].x;
    v[1] = state.Thumbstick[0].y;
    v[2] = state.Thumbstick[1].x;
    v[3] = state.Thumbstick[1].y;
    PsychSetStructArrayNativeElement("Thumbstick", 0, outMat, status);

    // ThumbstickNoDeadzone:
    v = NULL;
    PsychAllocateNativeDoubleMat(2, 2, 1, &v, &outMat);
    v[0] = state.ThumbstickNoDeadzone[0].x;
    v[1] = state.ThumbstickNoDeadzone[0].y;
    v[2] = state.ThumbstickNoDeadzone[1].x;
    v[3] = state.ThumbstickNoDeadzone[1].y;
    PsychSetStructArrayNativeElement("ThumbstickNoDeadzone", 0, outMat, status);

    // ThumbstickRaw:
    v = NULL;
    PsychAllocateNativeDoubleMat(2, 2, 1, &v, &outMat);
    v[0] = state.ThumbstickRaw[0].x;
    v[1] = state.ThumbstickRaw[0].y;
    v[2] = state.ThumbstickRaw[1].x;
    v[3] = state.ThumbstickRaw[1].y;
    PsychSetStructArrayNativeElement("ThumbstickRaw", 0, outMat, status);
*/
    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRGetFovTextureSize(void)
{
    static char useString[] = "[width, height, recMSAASamples, fovPort, maxWidth, maxHeight, maxMSAASamples] = PsychOpenXRCore('GetFovTextureSize', openxrPtr, eye [, fov=[HMDRecommended]][, pixelsPerDisplay=1]);";
    //                          1      2       3               4        5         6          7                                                      1          2      3                       4
    static char synopsisString[] =
    "Return recommended size of client renderbuffers for OpenXR device 'openxrPtr'.\n"
    "'eye' which eye to provide the size for: 0 = Left, 1 = Right.\n"
    "'fov' Optional field of view in degrees, from line of sight: [leftdeg, rightdeg, updeg, downdeg]. "
    "If 'fov' is omitted, the OpenXR runtime will be asked for a good default field of view and that "
    "will be used. The field of view may be dependent on the settings in the OpenXR user profile of the "
    "currently selected user.\n"
    "'pixelsPerDisplay' Ratio of the number of render target pixels to display pixels at the center "
    "of distortion. Defaults to 1.0 if omitted. Lower values can improve performance, at lower quality.\n"
    "\n"
    "Return values are 'width' for recommended width of framebuffer in pixels and 'height' for "
    "recommended height of framebuffer in pixels. 'fovPort' is the field of view in degrees "
    "finally used for calculation of 'width' x 'height'.\n"
    "'maxWidth' and 'maxHeight' are the maximum width and height of the framebuffer in pixels, "
    "as supported by the runtime.\n"
    "'recMSAASamples' is the recommended number of samples per pixel for MSAA anti-aliasing, where "
    "a value greater than one means to use MSAA. 'maxMSAASamples' is the maximum MSAA sample count "
    "supported by the runtime.\n";
    static char seeAlsoString[] = "GetUndistortionParameters";

    XrResult result;
    int handle, eyeIndex;
    int n, m, p;
    PsychOpenXRDevice *openxr;
    double *fov;
    double pixelsPerDisplay;
    double *outFov;
    uint32_t vc;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(7));
    PsychErrorExit(PsychCapNumInputArgs(4));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get eye index - left = 0, right = 1:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    // Get optional field of view in degrees in left,right,up,down direction from line of sight:
    if (PsychAllocInDoubleMatArg(3, kPsychArgOptional, &n, &m, &p, &fov)) {
        // Validate and assign:
        if (n * m * p != 4) PsychErrorExitMsg(PsychError_user, "Invalid 'fov' specified. Must be a 4-component vector of form [leftdeg, rightdeg, updeg, downdeg].");

        openxr->xrFov[eyeIndex].angleLeft  = (float) deg2rad(fov[0]);
        openxr->xrFov[eyeIndex].angleRight = (float) deg2rad(fov[1]);
        openxr->xrFov[eyeIndex].angleUp    = (float) deg2rad(fov[2]);
        openxr->xrFov[eyeIndex].angleDown  = (float) deg2rad(fov[3]);
    }
    else {
        // None specified: Ask the runtime for good defaults.
        // TODO openxr->xrFov[eyeIndex] = maybe useful XR_EPIC_view_configuration_fov -> Put in next chain of xrEnumerateViewConfigurationViews return structs ? Recommends based on users IPD - user adaptive!
    }

    // Get optional pixelsPerDisplay parameter:
    pixelsPerDisplay = 1.0;
    PsychCopyInDoubleArg(4, kPsychArgOptional, &pixelsPerDisplay);
    if (pixelsPerDisplay <= 0.0) PsychErrorExitMsg(PsychError_user, "Invalid 'pixelsPerDisplay' specified. Must be greater than zero.");

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

    // Ask the api for optimal texture size, aka the size of the client draw buffer:
    openxr->texSize[eyeIndex].width = views[eyeIndex].recommendedImageRectWidth;
    openxr->texSize[eyeIndex].height = views[eyeIndex].recommendedImageRectHeight;
    openxr->maxWidth = views[eyeIndex].maxImageRectWidth;
    openxr->maxHeight = views[eyeIndex].maxImageRectHeight;
    openxr->recSamples = views[eyeIndex].recommendedSwapchainSampleCount;
    openxr->maxSamples = views[eyeIndex].maxSwapchainSampleCount;

    free(views);

    // Return recommended width and height and MSAA samples of drawBuffer:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, openxr->texSize[eyeIndex].width);
    PsychCopyOutDoubleArg(2, kPsychArgOptional, openxr->texSize[eyeIndex].height);
    PsychCopyOutDoubleArg(3, kPsychArgOptional, openxr->recSamples);

    // FoV port used:
    PsychAllocOutDoubleMatArg(4, kPsychArgOptional, 4, 1, 1, &outFov);
    outFov[0] = rad2deg(openxr->xrFov[eyeIndex].angleLeft);
    outFov[1] = rad2deg(openxr->xrFov[eyeIndex].angleRight);
    outFov[2] = rad2deg(openxr->xrFov[eyeIndex].angleUp);
    outFov[3] = rad2deg(openxr->xrFov[eyeIndex].angleDown);

    // Return maximum width and height and MSAA samples of drawBuffer:
    PsychCopyOutDoubleArg(5, kPsychArgOptional, openxr->maxWidth);
    PsychCopyOutDoubleArg(6, kPsychArgOptional, openxr->maxHeight);
    PsychCopyOutDoubleArg(7, kPsychArgOptional, openxr->maxSamples);

    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRCreateAndStartSession(void)
{
    static char useString[] = "[videoRefreshDuration] = PsychOpenXRCore('CreateAndStartSession', openxrPtr, deviceContext, openGLContext, openGLDrawable, openGLConfig, openGLVisualId);";
    //                          1                                                                1          2              3              4               5             6
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
    "\n"
    "Returns the following information:\n"
    "'videoRefreshDuration' Video refresh duration in seconds of the XR display device if "
    "available. If the info can't be queried a fallback value of 0.011 secs for a 90 Hz "
    "refresh rate is returned as one of the most common values for consumer HMDs.\n";
    static char seeAlsoString[] = "Open Close";

    XrResult result;
    int handle;
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
    PsychErrorExit(PsychCapNumInputArgs(6));
    PsychErrorExit(PsychRequireNumInputArgs(6));

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

    if (verbosity > 3)
        printf("PsychOpenXRCore-INFO: OpenXR session created for XR device with OpenGL rendering.\n");

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
    // Ideally we'd also allow GL_SRGB8_A8 preferred over GL_RGBA8, but Screen() is not yet
    // necessarily ready for this as it doesn't that format for drawBufferFBO's...
    for (i = 0; i < nFormats; i++) {
        // If floatFormat requested, try to get GL_RGBA16F:
        if (swapchainFormats[i] == GL_RGBA16F && floatFormat)
            break;

        // If floatFormat requested, allow to get GL_RGBA32F:
        if (swapchainFormats[i] == GL_RGBA32F && floatFormat)
            break;

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

    // Shift away from eyes by 0.0 meters:
    openxr->quadViewLayer[eyeIndex].pose.position.z -= 0.0;

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
    openxr->projView[eyeIndex].fov = openxr->xrFov[eyeIndex];

    // Setup the static projectionLayer we use for fully tracked 3D OpenGL rendering:
    openxr->projectionLayer.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION;
    openxr->projectionLayer.next = NULL;
    openxr->projectionLayer.layerFlags = XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
    openxr->projectionLayer.space = openxr->worldSpace;
    openxr->projectionLayer.viewCount = 2;
    openxr->projectionLayer.views = openxr->projView;

    // Setup initial assignments of actual layers to submit to the compositor for display.
    // Init state is to use the quadViewLayer(s) for mono or stereo 2D display:
    openxr->submitLayers[0] = (XrCompositionLayerBaseHeader*) &openxr->quadViewLayer[0];
    openxr->submitLayers[1] = (XrCompositionLayerBaseHeader*) &openxr->quadViewLayer[1];
    openxr->submitLayersCount = (openxr->isStereo) ? 2 : 1;

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

        PsychErrorExitMsg(PsychError_system, "Failed to retrieve next OpenGL texture from swapchain I.");
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

        PsychErrorExitMsg(PsychError_system, "Failed to retrieve next OpenGL texture from swapchain II.");
    }

    PsychUnlockMutex(&(openxr->presenterLock));

    // Return texture object handle:
    if (result == XR_SUCCESS)
        PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) openxr->textureSwapChainImages[eyeIndex][texIndex].image);
    else if (verbosity > 1)
        printf("PsychOpenXRCore-WARNING: Timed out waiting for next swapchain image for eye %i: %s\n", eyeIndex, errorString);

    return(PsychError_none);
}

// TODO: Is there a point to this?
PsychError PSYCHOPENXRGetUndistortionParameters(void)
{
    static char useString[] = "[hmdShiftx, hmdShifty, hmdShiftz] = PsychOpenXRCore('GetUndistortionParameters', openxrPtr, eye);";
    //                          1          2          3                                                         1          2
    static char synopsisString[] =
    "Return parameters needed for rendering for output on OpenXR device 'openxrPtr'.\n"
    "'eye' which eye to provide the data: 0 = Left, 1 = Right.\n"
    "Return values:\n"
    "[hmdShiftx, hmdShifty, hmdShiftz] = HmdToEyeOffset 3D translation vector. Defines the location of the optical center of the eye "
    "relative to the origin of the local head reference frame, ie. the tracked head position. Unit is meters.\n";
    static char seeAlsoString[] = "GetFovTextureSize";

    int handle, eyeIndex;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(3));
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

    // Get eye render description for this eye:
    if (verbosity > 3) {
        printf("PsychOpenXRCore-INFO: For HMD %i, eye %i - RenderDescription:\n", handle, eyeIndex);
        printf("PsychOpenXRCore-INFO: FoV degrees: %f %f %f %f\n", rad2deg(openxr->xrFov[eyeIndex].angleLeft), rad2deg(openxr->xrFov[eyeIndex].angleRight), rad2deg(openxr->xrFov[eyeIndex].angleUp), rad2deg(openxr->xrFov[eyeIndex].angleDown));
        printf("PsychOpenXRCore-INFO: HmdToEyeViewOffset meters: [x,y,z] = [%f, %f, %f]\n", openxr->eyeShift[eyeIndex].x, openxr->eyeShift[eyeIndex].y, openxr->eyeShift[eyeIndex].z);
    }

    // HmdToEyeViewOffset: [x,y,z]:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, openxr->eyeShift[eyeIndex].x);
    PsychCopyOutDoubleArg(2, kPsychArgOptional, openxr->eyeShift[eyeIndex].y);
    PsychCopyOutDoubleArg(3, kPsychArgOptional, openxr->eyeShift[eyeIndex].z);

    return(PsychError_none);
}

// TODO
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
        v[0] = trackerPose.Pose.Position.x;
        v[1] = trackerPose.Pose.Position.y;
        v[2] = trackerPose.Pose.Position.z;

        v[3] = trackerPose.Pose.Orientation.x;
        v[4] = trackerPose.Pose.Orientation.y;
        v[5] = trackerPose.Pose.Orientation.z;
        v[6] = trackerPose.Pose.Orientation.w;
        PsychSetStructArrayNativeElement("CameraPose", i, outMat, status);

        // Camera leveled pose:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);
        v[0] = trackerPose.LeveledPose.Position.x;
        v[1] = trackerPose.LeveledPose.Position.y;
        v[2] = trackerPose.LeveledPose.Position.z;

        v[3] = trackerPose.LeveledPose.Orientation.x;
        v[4] = trackerPose.LeveledPose.Orientation.y;
        v[5] = trackerPose.LeveledPose.Orientation.z;
        v[6] = trackerPose.LeveledPose.Orientation.w;
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

// TODO
PsychError PSYCHOPENXRGetStaticRenderParameters(void)
{
    static char useString[] = "[projL, projR] = PsychOpenXRCore('GetStaticRenderParameters', openxrPtr [, clipNear=0.01][, clipFar=10000.0]);";
    //                          1      2                                                        1            2                3
    static char synopsisString[] =
    "Retrieve static rendering parameters for OpenXR device 'openxrPtr' at current settings.\n"
    "'clipNear' Optional near clipping plane for OpenGL. Defaults to 0.01.\n"
    "'clipFar' Optional far clipping plane for OpenGL. Defaults to 10000.0.\n"
    "\nReturn arguments:\n\n"
    "'projL' is the 4x4 OpenGL projection matrix for the left eye rendering.\n"
    "'projR' is the 4x4 OpenGL projection matrix for the right eye rendering.\n"
    "Please note that projL and projR are usually identical for typical rendering scenarios.\n";
    static char seeAlsoString[] = "";
/*
    int handle;
    PsychOpenXRDevice *openxr;
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

    // Return left projection matrix as return argument 1:
    M = ovrMatrix4f_Projection(openxr->eyeRenderDesc[0].Fov, (float) clip_near, (float) clip_far, ovrProjection_ClipRangeOpenGL);
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 4, 4, 1, &outM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(outM++) = (double) M.M[j][i];

    // Return right projection matrix as return argument 2:
    M = ovrMatrix4f_Projection(openxr->eyeRenderDesc[1].Fov, (float) clip_near, (float) clip_far, ovrProjection_ClipRangeOpenGL);
    PsychAllocOutDoubleMatArg(2, kPsychArgOptional, 4, 4, 1, &outM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(outM++) = (double) M.M[j][i];
*/
    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRStartRender(void)
{
    static char useString[] = "[eyePoseL, eyePoseR] = PsychOpenXRCore('StartRender', openxrPtr);";
    //                          1         2                                             1
    static char synopsisString[] =
    "Mark start of a new 3D head tracked render cycle for OpenXR device 'openxrPtr'.\n"
    "Return values are the vectors which define the two eye cameras positions and orientations "
    "for the left eye and right eye 'eyePoseL' and 'eyePoseR'. The vectors are of form "
    "[tx, ty, tz, rx, ry, rz, rw] - A 3 component 3D position followed by a 4 component rotation "
    "quaternion.\n"
    "\n";
    static char seeAlsoString[] = "GetEyePose GetTrackingState EndFrameRender";
/*
    int handle;
    PsychOpenXRDevice *openxr;
    ovrVector3f eyeShift[2];
    double *outM;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(1));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get current eye poses for both eyes:
    HmdToEyeOffset[0] = openxr->eyeRenderDesc[0].HmdToEyeOffset;
    HmdToEyeOffset[1] = openxr->eyeRenderDesc[1].HmdToEyeOffset;

    PsychLockMutex(&(openxr->presenterLock));
    ovr_GetEyePoses(openxr->hmd, 0, FALSE, HmdToEyeOffset, openxr->outEyePoses, &openxr->sensorSampleTime);
    PsychUnlockMutex(&(openxr->presenterLock));

    // Left eye pose as raw data:
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, 7, 1, &outM);

    // Position (x,y,z):
    outM[0] = openxr->outEyePoses[0].Position.x;
    outM[1] = openxr->outEyePoses[0].Position.y;
    outM[2] = openxr->outEyePoses[0].Position.z;

    // Orientation as a quaternion (x,y,z,w):
    outM[3] = openxr->outEyePoses[0].Orientation.x;
    outM[4] = openxr->outEyePoses[0].Orientation.y;
    outM[5] = openxr->outEyePoses[0].Orientation.z;
    outM[6] = openxr->outEyePoses[0].Orientation.w;

    // Right eye pose as raw data:
    PsychAllocOutDoubleMatArg(2, kPsychArgOptional, 1, 7, 1, &outM);

    // Position (x,y,z):
    outM[0] = openxr->outEyePoses[1].Position.x;
    outM[1] = openxr->outEyePoses[1].Position.y;
    outM[2] = openxr->outEyePoses[1].Position.z;

    // Orientation as a quaternion (x,y,z,w):
    outM[3] = openxr->outEyePoses[1].Orientation.x;
    outM[4] = openxr->outEyePoses[1].Orientation.y;
    outM[5] = openxr->outEyePoses[1].Orientation.z;
    outM[6] = openxr->outEyePoses[1].Orientation.w;
*/
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

    // Get eye:
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &eyeIndex)) {
        if (eyeIndex < 0 || eyeIndex > 1)
            PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

        if (eyeIndex > 0 && !(openxr->isStereo))
            PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0, as mono display mode is selected.");

        if (!resultOK(xrReleaseSwapchainImage(openxr->textureSwapChain[eyeIndex], NULL))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to release current swapchain image for eye %i: %s\n", eyeIndex, errorString);

            PsychErrorExitMsg(PsychError_system, "Failed to release current OpenGL texture.");
        }
    }
    else {
        for (eyeIndex = 0; eyeIndex < ((openxr->isStereo) ? 2 : 1); eyeIndex++) {
            if (!resultOK(xrReleaseSwapchainImage(openxr->textureSwapChain[eyeIndex], NULL))) {
                if (verbosity > 0)
                    printf("PsychOpenXRCore-ERROR: Failed to release current swapchain image for eye %i: %s\n", eyeIndex, errorString);

                PsychErrorExitMsg(PsychError_system, "Failed to release current OpenGL textures.");
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
static double PresentExecute(PsychOpenXRDevice *openxr, psych_bool commitTextures, psych_bool inInit)
{
/*
    int eyeIndex, rc;
    psych_bool success = TRUE;
    double tPredictedOnset = 0;
    ovrResult result;
    ovrLayerEyeFov layer0;
    const ovrLayerHeader *layers[1] = { &layer0.Header };

    // printf("PresentExecute-1 %i = %f\n", commitTextures, ovr_GetTimeInSeconds());

    // Is this a present with updated visual content - called from "userspace" runtime?
    if (commitTextures) {
        // Free capacity in swapchains?
        while ((openxr->needSubmit >= openxr->textureSwapChainLength - 1) &&
               (!openxr->isTracking) && (openxr->presenterThread != (psych_thread) NULL)) {
            // No, and the presenterThread is running and should make free space soon. Wait for it to do its job:
            if ((rc = PsychWaitCondition(&(openxr->presentedSignal), &(openxr->presenterLock)))) {
                // Failed: Log it in a hopefully not too unsafe way:
                printf("PsychOpenXRCore-ERROR: Waitcondition on presentedSignal trigger failed  [%s].\n", strerror(rc));
            }
        }

        // Commit current target textures to chain:
        for (eyeIndex = 0; eyeIndex < ((openxr->isStereo) ? 2 : 1); eyeIndex++) {
            if (OVR_FAILURE(ovr_CommitTextureSwapChain(openxr->hmd, openxr->textureSwapChain[eyeIndex]))) {
                success = FALSE;
                ovr_GetLastErrorInfo(&errorInfo);
                if (verbosity > 0)
                    printf("PsychOpenXRCore-ERROR: eye %i ovr_CommitTextureSwapChain() failed: %s\n", eyeIndex, errorInfo.ErrorString);
            }
        }

        if (!success)
            goto present_fail;

        // Mark the need for a ovr_SubmitFrame() call before we can commit textures again:
        openxr->needSubmit++;
    }

    // printf("PresentExecute-2 %i - %f\n", commitTextures, ovr_GetTimeInSeconds());

    // Always submit updated textures when called from presenter thread. If called
    // from usercode 'PresentFrame' only submit if the usercode is in full charge,
    // ie. HMD head tracking is active and driving render timing in a fast closed loop:
    if (inInit || !commitTextures || openxr->isTracking || (openxr->presenterThread == (psych_thread) NULL)) {
        // If HMD tracking is disabled then set sensorSampleTime to "now" - reasonable:
        if (!openxr->isTracking)
            openxr->sensorSampleTime = ovr_GetTimeInSeconds();

        // Setup layer headers:
        layer0.Header.Type = ovrLayerType_EyeFov;
        layer0.Header.Flags = ovrLayerFlag_HighQuality | ovrLayerFlag_TextureOriginAtBottomLeft;

        // Use head locked layer if head tracking is disabled, so our frames stay in a fixed
        // position wrt. the HMD - the HMD simply acts as a strapped on mono- or stereo monitor:
        layer0.Header.Flags |= ((!openxr->isTracking) ? ovrLayerFlag_HeadLocked : 0);

        layer0.ColorTexture[0] = openxr->textureSwapChain[0];
        layer0.ColorTexture[1] = (openxr->isStereo) ? openxr->textureSwapChain[1] : NULL;
        layer0.Viewport[0].Pos.x = 0;
        layer0.Viewport[0].Pos.y = 0;
        layer0.Viewport[0].Size.w = openxr->textureWidth;
        layer0.Viewport[0].Size.h = openxr->textureHeight;
        layer0.Viewport[1].Pos.x = 0;
        layer0.Viewport[1].Pos.y = 0;
        layer0.Viewport[1].Size.w = openxr->textureWidth;
        layer0.Viewport[1].Size.h = openxr->textureHeight;
        layer0.Fov[0] = openxr->xrFov[0];
        layer0.Fov[1] = openxr->xrFov[1];
        layer0.RenderPose[0] = openxr->outEyePoses[0];
        layer0.RenderPose[1] = openxr->outEyePoses[1];
        layer0.SensorSampleTime = openxr->sensorSampleTime;

        tPredictedOnset = ovr_GetPredictedDisplayTime(openxr->hmd, openxr->frameIndex);
        if (verbosity > 3)
            printf("PRESENT[%i]: Last %f, Next %f, dT = %f msecs\n", commitTextures, openxr->lastPresentExecTime, tPredictedOnset, 1000 * (tPredictedOnset - openxr->lastPresentExecTime));

        // Submit frame to compositor for display at earliest possible time:
        result = ovr_SubmitFrame(openxr->hmd, openxr->frameIndex, NULL, layers, 1);
        if (OVR_FAILURE(result)) {
            success = FALSE;
            ovr_GetLastErrorInfo(&errorInfo);
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: ovr_SubmitFrame() failed: %s\n", errorInfo.ErrorString);
        }
        else {
            // Update onset time again after successful submit:
            tPredictedOnset = ovr_GetPredictedDisplayTime(openxr->hmd, openxr->frameIndex);

            // Record frameIndex at which new visual content will be committed:
            if (openxr->perfStats.FrameStatsCount > 0) {
                openxr->commitFrameIndex = openxr->perfStats.FrameStats[0].AppFrameIndex + 1;
            }

            openxr->frameIndex++;
            if (openxr->needSubmit > 0)
                openxr->needSubmit--;
        }

        if (result == ovrSuccess_NotVisible) {
            // Submitted frame does not display - a no-op submit:
            if (verbosity > 3)
                printf("PsychOpenXRCore:WARNING: Frame %i not presented to HMD. HMD removed from subjects head? [Time %f]\n",
                       openxr->frameIndex, tPredictedOnset);
        }

        // Update the lastPresentExecTime timestamp:
        openxr->lastPresentExecTime = tPredictedOnset;

        // printf("PresentExecute-3 %i - %f\n", commitTextures, ovr_GetTimeInSeconds());
        if (success) {
            double tDeadline;

            // Retrieve performance stats, mostly for our timestamping:
            tDeadline = ovr_GetTimeInSeconds() + 0.005;
            do {
                result = ovr_GetPerfStats(openxr->hmd, &openxr->perfStats);
                if (OVR_FAILURE(result)) {
                    ovr_GetLastErrorInfo(&errorInfo);
                    if (verbosity > 0)
                        printf("PsychOpenXRCore-ERROR: ovr_GetPerfStats() failed: %s\n", errorInfo.ErrorString);
                }
                else if ((verbosity > 3) && (openxr->perfStats.FrameStatsCount > 0)) {
                    printf("PsychOpenXRCore: DEBUG: Oldest AppFrameIndex %i vs. commitFrameIndex %i\n",
                           openxr->perfStats.FrameStats[openxr->perfStats.FrameStatsCount-1].AppFrameIndex,
                           openxr->commitFrameIndex);
                }

                if (openxr->perfStats.FrameStatsCount < 1)
                    PsychYieldIntervalSeconds(0.001);
            } while ((openxr->frameIndex > 0) && (ovr_GetTimeInSeconds() < tDeadline) &&
                     ((openxr->perfStats.FrameStatsCount < 1) || (openxr->perfStats.FrameStats[openxr->perfStats.FrameStatsCount-1].AppFrameIndex < openxr->commitFrameIndex)));

            if (verbosity > 3) {
                printf("\nPsychOpenXRCore: [%f msecs headroom] DEBUG: FrameStatsCount=%i, AnyFrameStatsDropped=%i\n",
                       1000 * (tDeadline - ovr_GetTimeInSeconds()), openxr->perfStats.FrameStatsCount,
                       (int) openxr->perfStats.AnyFrameStatsDropped);
            }

            // Are we running on the presenterThread?
            if (!commitTextures && openxr->perfStats.FrameStatsCount) {
                // Yes. Signal end of a new submitFrame -> timestamping cycle to main-thread:
                if ((rc = PsychSignalCondition(&(openxr->presentedSignal)))) {
                    printf("PsychOpenXRCore-ERROR: PsychSignalCondition() trigger operation failed  [%s].\n", strerror(rc));
                }
            }
        }
    }

    // printf("PresentExecute-4 %i - %f\n", commitTextures, ovr_GetTimeInSeconds());

present_fail:

    return(success ? tPredictedOnset : -1);
*/
return(-1);
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
        if ((PresentExecute(openxr, FALSE, FALSE) < 0) && verbosity > 0) {
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

// TODO
PsychError PSYCHOPENXRPresentFrame(void)
{
    static char useString[] = "[frameTiming, tPredictedOnset, referenceFrameIndex] = PsychOpenXRCore('PresentFrame', openxrPtr [, doTimestamp=0][, when=0]);";
    //                          1            2                3                                                         1            2                3
    static char synopsisString[] =
    "Present last rendered frame to OpenXR HMD device 'openxrPtr'.\n\n"
    "This will commit the current set of 2D textures with new rendered content "
    "to the texture swapchains, for consumption by the VR runtime/compositor. "
    "The swapchains will advance, providing new unused image textures as new "
    "render targets for the next rendering cycle.\n\n"
    "You usually won't call this function yourself, but Screen('Flip') "
    "will call it automatically for you at the appropriate moment.\n\n"
    "Returns a 'frameTiming' struct with information about the presentation "
    "timing for this presentation cycle.\n\n"
    "'doTimestamp' If set to 1, perform timestamping of stimulus onset on the HMD, "
    "or at least try to estimate such onset time. If set to 0, do nothing timestamping-wise.\n\n"
    "'when' If provided, defines the target presentation time, as provided by Screen('Flip', win, when); "
    "a value of zero, or omission, means to present as soon as possible.\n\n"
    "'frameTiming' is an array of structs, where each struct contains presentation "
    "timing and performance info for one presented frame, with the most recently "
    "presented frame in frameTiming(1), older ones at higher indices if the script "
    "falls behind the compositor in Screen('Flip')ping frames in time for the HMD.\n"
    "If successful timestamping happened, 'referenceFrameIndex' will be the index into "
    "the 'frameTiming' array to the struct which corresponds to the just presented frame, "
    "otherwise zero is returned.\n\n"
    "'tPredictedOnset' is another predicted onset time for the just presented frame, based on "
    "very cheap and fast time-stamping which may be much less reliable though.\n\n"
    "The following fields are currently supported in each struct of 'frameTiming':\n\n"
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
    "1 frame lag, etc.\n\n"
    "'tPredictedOnset' Predicted onset time for the just submitted frame, according to compositor.\n"
    "\n\n";
    static char seeAlsoString[] = "EndFrameRender";

    PsychGenericScriptType *frameT;
    const char *FieldNames[] = {"HmdVsyncIndex", "AppFrameIndex", "HMDTime", "GetSecsTime", "AppMotionToPhotonLatency",
                                "RecentSensorSampleTime", "StimulusOnsetTime", "VBlankTime", "AppQueueAheadTime" };
    const int FieldCount = 9;

    int handle, rc;
    int doTimestamp = 0;
    int referenceFrameIndex = -1;
    double tNow, tHMD, tStimOnset, tVBL, tPredictedOnset;
    double tWhen;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get optional timestamping flag:
    PsychCopyInIntegerArg(2, kPsychArgOptional, &doTimestamp);

    // Get optional presentation target time:
    tWhen = 0;
    PsychCopyInDoubleArg(3, kPsychArgOptional, &tWhen);
/*
    // Single-threaded operation, all done on this thread?
    if (!openxr->multiThreaded) {
        // Yes. Go into a check -> submit -> check loop until target time is
        // exceeded, then present an updated frame:
        while ((tPredictedOnset = ovr_GetPredictedDisplayTime(openxr->hmd, openxr->frameIndex)) < tWhen + openxr->frameDuration / 2) {
            if (verbosity > 4)
                printf("PsychOpenXRCore:PresentFrame-DEBUG: In busy-present-wait loop: pred %f < %f tWhen\n",
                       tPredictedOnset, tWhen);
            PresentExecute(openxr, FALSE, FALSE);
        }
    }

    // Execute the present operation with the presenterThread locked out.
    // Invalidate any scheduledPresentExecTime after such a Present:
    PsychLockMutex(&(openxr->presenterLock));
    openxr->scheduledPresentExecTime = tWhen;
    tPredictedOnset = PresentExecute(openxr, TRUE, FALSE);
    openxr->scheduledPresentExecTime = -DBL_MAX;
    PsychUnlockMutex(&(openxr->presenterLock));

    if ((tPredictedOnset < 0) && (verbosity > 0))
        printf("PsychOpenXRCore-ERROR: Failed to present new frames to VR compositor.\n");

    PsychGetAdjustedPrecisionTimerSeconds(&tNow);
    tHMD = ovr_GetTimeInSeconds();

    if (doTimestamp) {
        int i;
        uint32_t timestampedFrameIndex = 0;

        PsychLockMutex(&(openxr->presenterLock));
        while (timestampedFrameIndex == 0) {
            for (i = openxr->perfStats.FrameStatsCount - 1; i >= 0; i--) {
                if (openxr->perfStats.FrameStats[i].AppFrameIndex >= openxr->commitFrameIndex) {
                    timestampedFrameIndex = openxr->perfStats.FrameStats[i].AppFrameIndex;
                    referenceFrameIndex = i;
                    break;
                }
            }

            if (timestampedFrameIndex == 0) {
                if (openxr->presenterThread != (psych_thread) NULL) {
                    // Multi-Threaded: Wait efficiently for signalling of new frame submitted by presenter thread:
                    if ((rc = PsychWaitCondition(&(openxr->presentedSignal), &(openxr->presenterLock)))) {
                        // Failed: Log it in a hopefully not too unsafe way:
                        printf("PsychOpenXRCore-ERROR: Waitcondition II on presentedSignal trigger failed  [%s].\n", strerror(rc));
                    }
                }
                else {
                    // Not multi-threaded. If we ain't got no result yet, we won't
                    // get any in the future:
                    break;
                }
            }
        }

        if (openxr->perfStats.FrameStatsCount > 0) {
            PsychAllocOutStructArray(1, kPsychArgOptional, openxr->perfStats.FrameStatsCount, FieldCount, FieldNames, &frameT);
            for (i = 0; i < openxr->perfStats.FrameStatsCount; i++) {
                // HMD Vsync counter:
                PsychSetStructArrayDoubleElement("HmdVsyncIndex", i, openxr->perfStats.FrameStats[i].HmdVsyncIndex, frameT);

                // Our sbc counter:
                PsychSetStructArrayDoubleElement("AppFrameIndex", i, openxr->perfStats.FrameStats[i].AppFrameIndex, frameT);

                // Clock-Sync PTB timebase vs. OpenXR timebase:
                PsychGetAdjustedPrecisionTimerSeconds(&tNow);
                tHMD = ovr_GetTimeInSeconds();
                PsychSetStructArrayDoubleElement("HMDTime", i, tHMD, frameT);
                PsychSetStructArrayDoubleElement("GetSecsTime", i, tNow, frameT);

                // Time between openxr->sensorSampleTime and visual onset (video frame midpoint of scanout):
                PsychSetStructArrayDoubleElement("AppMotionToPhotonLatency", i, openxr->perfStats.FrameStats[i].AppMotionToPhotonLatency, frameT);
                PsychSetStructArrayDoubleElement("RecentSensorSampleTime", i, openxr->sensorSampleTime, frameT);

                // Compute absolute stimulus onset (mid-point), remap to PTB GetSecs time:
                tStimOnset = (tNow - tHMD)  + openxr->sensorSampleTime + openxr->perfStats.FrameStats[i].AppMotionToPhotonLatency;
                PsychSetStructArrayDoubleElement("StimulusOnsetTime", i, tStimOnset, frameT);

                // Compute virtual start of VBLANK time as stimulus onset - half a HMD video refresh duration:
                tVBL = tStimOnset - 0.5 * openxr->frameDuration;
                PsychSetStructArrayDoubleElement("VBlankTime", i, tVBL, frameT);

                // Queue ahead for application. Citation from the v 1.16 SDK docs:
                // "Amount of queue-ahead in seconds provided to the app based on performance and overlap of CPU & GPU utilization
                // A value of 0.0 would mean the CPU & GPU workload is being completed in 1 frame's worth of time, while
                // 11 ms (on the CV1) of queue ahead would indicate that the app's CPU workload for the next frame is
                // overlapping the app's GPU workload for the current frame."
                PsychSetStructArrayDoubleElement("AppQueueAheadTime", i, openxr->perfStats.FrameStats[i].AppQueueAheadTime, frameT);
            }
        }

        PsychUnlockMutex(&(openxr->presenterLock));
    }
    else {
        // Return empty 'frameTiming' info struct array, so our calling scripting environment does not bomb out:
        PsychAllocOutStructArray(1, kPsychArgOptional, 0, FieldCount, FieldNames, &frameT);
    }

    // Copy out predicted onset time for the just emitted frame:
    tPredictedOnset = tPredictedOnset + (tNow - tHMD) - 0.5 * openxr->frameDuration;
    PsychCopyOutDoubleArg(2, kPsychArgOptional, tPredictedOnset);

    PsychCopyOutDoubleArg(3, kPsychArgOptional, (double) referenceFrameIndex + 1);
*/
    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRGetEyePose(void)
{
    static char useString[] = "[eyePose, eyeIndex] = PsychOpenXRCore('GetEyePose', openxrPtr, renderPass);";
    //                          1        2                                            1          2
    static char synopsisString[] =
    "Return current predicted pose vector for an eye for OpenXR device 'openxrPtr'.\n"
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
/*
    int handle, renderPass;
    PsychOpenXRDevice *openxr;
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
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get renderPass:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &renderPass);
    if (renderPass < 0 || renderPass > 1) PsychErrorExitMsg(PsychError_user, "Invalid 'renderPass' specified. Must be 0 or 1 for first or second pass.");

    // Get eye pose:
    eye = renderPass;
    HmdToEyeOffset[0] = openxr->eyeRenderDesc[0].HmdToEyeOffset;
    HmdToEyeOffset[1] = openxr->eyeRenderDesc[1].HmdToEyeOffset;

    PsychLockMutex(&(openxr->presenterLock));
    ovr_GetEyePoses(openxr->hmd, 0, FALSE, HmdToEyeOffset, openxr->outEyePoses, &openxr->sensorSampleTime);
    PsychUnlockMutex(&(openxr->presenterLock));

    // Eye pose as raw data:
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, 7, 1, &outM);

    // Position (x,y,z):
    outM[0] = openxr->outEyePoses[eye].Position.x;
    outM[1] = openxr->outEyePoses[eye].Position.y;
    outM[2] = openxr->outEyePoses[eye].Position.z;

    // Orientation as a quaternion (x,y,z,w):
    outM[3] = openxr->outEyePoses[eye].Orientation.x;
    outM[4] = openxr->outEyePoses[eye].Orientation.y;
    outM[5] = openxr->outEyePoses[eye].Orientation.z;
    outM[6] = openxr->outEyePoses[eye].Orientation.w;

    // Copy out preferred eye render order for info:
    PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) eye);
*/
    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRSetHUDState(void)
{
    static char useString[] = "PsychOpenXRCore('SetHUDState', openxrPtr , mode);";
    //                                                           1           2
    static char synopsisString[] =
    "Set mode of operation for the performance HUD for OpenXR device 'openxrPtr'.\n"
    "'mode' can be one of the following settings:\n"
    "0 = Head up display HUD off ie. invisible.\n"
    "1 = HUD shows performance summary.\n"
    "2 = HUD shows latency timing.\n"
    "3 = HUD shows application render timing.\n"
    "4 = HUD shows VR compositor render timing.\n"
    "5 = HUD shows Version information of different components.\n"
    "6 = HUD shows Asynchronous time warp (ASW) stats.\n"
    "Higher numbers may do something useful in the future.\n";

    static char seeAlsoString[] = "";
    int handle, mode;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get the new performance HUD mode:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &mode);
    if (mode < 0)
        PsychErrorExitMsg(PsychError_user, "Invalid performance HUD mode passed. Must be >= 0.");

    // Apply new mode:
//    ovr_SetInt(openxr->hmd, OVR_PERF_HUD_MODE, mode);

    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRGetPerformanceStats(void)
{
    static char useString[] = "[adaptiveGpuPerformanceScale, frameStats, anyFrameStatsDropped, aswIsAvailable] = PsychOpenXRCore('GetPerformanceStats', openxrPtr);";
    //                          1                            2           3                     4                                                           1
    static char synopsisString[] =
    "Query performance statistics for HMD device 'openxrPtr'.\n\n"
    "Return values:\n\n"
    "'adaptiveGpuPerformanceScale' is an edge-filtered value that a caller can use to adjust "
    "the graphics quality of the application to keep the GPU utilization in check. The value "
    "is calculated as:\n\n"
    "   (desired_GPU_utilization / current_GPU_utilization)\n\n"
    "As such, when this value is 1.0, the GPU is doing the right amount of work for the app. "
    "Lower values mean the app needs to pull back on the GPU utilization.\n"
    "If the app is going to directly drive render-target resolution using this value, then "
    "be sure to take the square-root of the value before scaling the resolution with it. "
    "Changing render target resolutions however is one of the many things an app can do "
    "increase or decrease the amount of GPU utilization.\n"
    "Since AdaptiveGpuPerformanceScale is edge-filtered and does not change rapidly "
    "(i.e. reports non-1.0 values once every couple of seconds) the app can make the "
    "necessary adjustments and then keep watching the value to see if it has been satisfied.\n\n"
    "'aswIsAvailable' If 1 then Asynchronous Space Warp (ASW) is supported on your system, 0 if not.\n\n"
    "'anyFrameStatsDropped' If 1 then the 'frameStats' struct array does not contain complete per frame "
    "performance statistics, because you did not call this function frequently enough. In general, calling "
    "at the native video refresh rate of the HMD is required for best results.\n\n"
    "'frameStats' is a struct array with recorded performance statistics, going some frames backwards in time.\n"
    "The number of frames this goes back is implementation dependent. Array slot 1 contains the stats of the "
    "most recent frame, slot 2 the 2nd oldest etc. Each struct contains many fields with info about the frame "
    "presentation latency, cpu and gpu utilization, VR compositor performance, ASW state and performance etc. Consult OpenXR "
    "SDK online documentation to learn about the meaning of the fields.\n";

    static char seeAlsoString[] = "";

    PsychGenericScriptType *frameT;
    const char *FieldNames[] = {"HmdVsyncIndex", "AppFrameIndex", "AppDroppedFrameCount", "AppMotionToPhotonLatency", "AppQueueAheadTime", "AppCpuElapsedTime",
                                "AppGpuElapsedTime", "CompositorFrameIndex", "CompositorDroppedFrameCount", "CompositorLatency",
                                "CompositorCpuElapsedTime", "CompositorGpuElapsedTime", "CompositorCpuStartToGpuEndElapsedTime",
                                "CompositorGpuEndToVsyncElapsedTime", "AswIsActive", "AswActivatedToggleCount",
                                "AswPresentedFrameCount", "AswFailedFrameCount" };
    const int FieldCount = 18;

    int handle, i;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(4));
    PsychErrorExit(PsychCapNumInputArgs(1));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);
/*
    // Global parameters:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) openxr->perfStats.AdaptiveGpuPerformanceScale);
    PsychCopyOutDoubleArg(3, kPsychArgOptional, (double) openxr->perfStats.AnyFrameStatsDropped);
    PsychCopyOutDoubleArg(4, kPsychArgOptional, (double) openxr->perfStats.AswIsAvailable);

    // Struct array with per frame perfStats for the last couple of frames:
    PsychAllocOutStructArray(2, kPsychArgOptional, openxr->perfStats.FrameStatsCount, FieldCount, FieldNames, &frameT);
    for (i = 0; i < openxr->perfStats.FrameStatsCount; i++) {
        PsychSetStructArrayDoubleElement("HmdVsyncIndex", i, openxr->perfStats.FrameStats[i].HmdVsyncIndex, frameT);
        PsychSetStructArrayDoubleElement("AppFrameIndex", i, openxr->perfStats.FrameStats[i].AppFrameIndex, frameT);
        PsychSetStructArrayDoubleElement("AppDroppedFrameCount", i, openxr->perfStats.FrameStats[i].AppDroppedFrameCount, frameT);
        PsychSetStructArrayDoubleElement("AppMotionToPhotonLatency", i, openxr->perfStats.FrameStats[i].AppMotionToPhotonLatency, frameT);
        PsychSetStructArrayDoubleElement("AppQueueAheadTime", i, openxr->perfStats.FrameStats[i].AppQueueAheadTime, frameT);
        PsychSetStructArrayDoubleElement("AppCpuElapsedTime", i, openxr->perfStats.FrameStats[i].AppCpuElapsedTime, frameT);
        PsychSetStructArrayDoubleElement("AppGpuElapsedTime", i, openxr->perfStats.FrameStats[i].AppGpuElapsedTime, frameT);
        PsychSetStructArrayDoubleElement("CompositorFrameIndex", i, openxr->perfStats.FrameStats[i].CompositorFrameIndex, frameT);
        PsychSetStructArrayDoubleElement("CompositorDroppedFrameCount", i, openxr->perfStats.FrameStats[i].CompositorDroppedFrameCount, frameT);
        PsychSetStructArrayDoubleElement("CompositorLatency", i, openxr->perfStats.FrameStats[i].CompositorLatency, frameT);
        PsychSetStructArrayDoubleElement("CompositorCpuElapsedTime", i, openxr->perfStats.FrameStats[i].CompositorCpuElapsedTime, frameT);
        PsychSetStructArrayDoubleElement("CompositorGpuElapsedTime", i, openxr->perfStats.FrameStats[i].CompositorGpuElapsedTime, frameT);
        PsychSetStructArrayDoubleElement("CompositorCpuStartToGpuEndElapsedTime", i, openxr->perfStats.FrameStats[i].CompositorCpuStartToGpuEndElapsedTime, frameT);
        PsychSetStructArrayDoubleElement("CompositorGpuEndToVsyncElapsedTime", i, openxr->perfStats.FrameStats[i].CompositorGpuEndToVsyncElapsedTime, frameT);
        PsychSetStructArrayDoubleElement("AswIsActive", i, openxr->perfStats.FrameStats[i].AswIsActive, frameT);
        PsychSetStructArrayDoubleElement("AswActivatedToggleCount", i, openxr->perfStats.FrameStats[i].AswActivatedToggleCount, frameT);
        PsychSetStructArrayDoubleElement("AswPresentedFrameCount", i, openxr->perfStats.FrameStats[i].AswPresentedFrameCount, frameT);
        PsychSetStructArrayDoubleElement("AswFailedFrameCount", i, openxr->perfStats.FrameStats[i].AswFailedFrameCount, frameT);
    }
*/
    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRHapticPulse(void)
{
    static char useString[] = "pulseEndTime = PsychOpenXRCore('HapticPulse', openxrPtr, controllerType [, duration=2.5][, freq=1.0][, amplitude=1.0]);";
    //                         1                                                1          2                 3               4           5
    static char synopsisString[] =
    "Execute a haptic feedback pulse on controller 'controllerType' associated with OpenXR device 'openxrPtr'.\n\n"
    "'duration' is the duration of the pulse in seconds. If omitted, the function returns immediately and "
    "the maximum duration of 2.5 seconds is used, unless you call the function again with 'freq' = 0, to cancel the "
    "currently active pulse. Otherwise, if a 'duration' other than 2.5 seconds is specified, the function executes "
    "for the specified duration, and may block execution of your script for that time span on some controller types, "
    "returning the absolute time when the pulse is expected to end.\n\n"
    "'freq' Frequency of the vibration in normalized 0.0 - 1.0 range. Currently only values 0, 0.5 and 1.0 "
    "will be used. 0 = Disable ongoing pulse immediately. Values other than 0, 0.5, or 1 will be clamped to the nearest value.\n\n"
    "'amplitude' Normalized amplitude in range 0.0 - 1.0\n\n"
    "The return argument 'pulseEndTime' contains the absolute time in seconds when the pulse is expected "
    "to end, as estimated at the time of calling the function. The precision and accuracy of pulse timing "
    "is not known.\n";
/*
    static char seeAlsoString[] = "";
    int handle, controllerType;
    PsychOpenXRDevice *openxr;
    double duration, tNow, freq, amplitude, pulseEndTime;
    ovrResult result;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(5));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get the new performance HUD mode:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &controllerType);

    // Duration:
    duration = 2.5;
    PsychCopyInDoubleArg(3, kPsychArgOptional, &duration);
    if (duration < 0)
        PsychErrorExitMsg(PsychError_user, "Invalid negative 'duration' in seconds specified. Must be positive.");

    if ((duration > 2.5) && (verbosity > 1))
        printf("PsychOpenXRCore-WARNING: 'HapticPulse' of %f seconds duration requested, but currently duration is limited to a maximum of 2.5 seconds. Clamping...\n",
                duration);

    if (duration > 2.5)
        duration = 2.5;

    freq = 1.0;
    PsychCopyInDoubleArg(4, kPsychArgOptional, &freq);
    if (freq < 0.0 || freq > 1.0)
        PsychErrorExitMsg(PsychError_user, "Invalid 'freq' frequency specified. Must be in range [0.0 ; 1.0].");

    amplitude = 1.0;
    PsychCopyInDoubleArg(5, kPsychArgOptional, &amplitude);
    if (amplitude < 0.0 || amplitude > 1.0)
        PsychErrorExitMsg(PsychError_user, "Invalid 'amplitude' specified. Must be in range [0.0 ; 1.0].");

    // Execute pulse:
    result = ovr_SetControllerVibration(openxr->hmd, (ovrControllerType) controllerType, (float) freq, (float) amplitude);
    if (OVR_FAILURE(result)) {
        ovr_GetLastErrorInfo(&errorInfo);
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: ovr_SetControllerVibration() failed: %s\n", errorInfo.ErrorString);
        PsychErrorExitMsg(PsychError_system, "Failed to initiate haptic feedback pulse.");
    }

    if ((result == ovrSuccess_DeviceUnavailable) && (verbosity > 1))
        printf("PsychOpenXRCore-WARNING: 'HapticPulse' will go nowhere, as suitable controller of type %i is not connected.\n", controllerType);

    if ((result == ovrSuccess) && (verbosity > 3))
        printf("PsychOpenXRCore-INFO: 'HapticPulse' of duration %f secs, freq %f, amplitude %f for controller of type %i started.\n",
               duration, freq, amplitude, controllerType);

    // Predict "off" time:
    PsychGetAdjustedPrecisionTimerSeconds(&pulseEndTime);
    tNow = pulseEndTime;
    pulseEndTime += duration;
    PsychCopyOutDoubleArg(1, kPsychArgOptional, pulseEndTime);

    // Pulse of predefined duration requested?
    if ((freq != 0) && (duration < 2.5)) {
        // Yes. Spin-wait until expected end time, then stop the pulse:
        // Need to spin-wait, because we must call PresentExecute() for a
        // fake VR frame submit periodically to keep the OpenXR runtime happy,
        // otherwise no haptic feedback will be triggered at all. Sad...
        while (tNow < pulseEndTime) {
            PsychYieldIntervalSeconds(0.001);
            PresentExecute(openxr, FALSE, FALSE);
            PsychGetAdjustedPrecisionTimerSeconds(&tNow);
        }

        // Stop haptic effect:
        result = ovr_SetControllerVibration(openxr->hmd, (ovrControllerType) controllerType, (float) 0, (float) 0);
        if (OVR_FAILURE(result)) {
            ovr_GetLastErrorInfo(&errorInfo);
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: ovr_SetControllerVibration() failed to stop vibration: %s\n", errorInfo.ErrorString);
            PsychErrorExitMsg(PsychError_system, "Failed to stop haptic feedback pulse.");
        }
    }
*/
    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRFloatsProperty(void)
{
    static char useString[] = "oldValues = PsychOpenXRCore('FloatsProperty', openxrPtr, propertyName [, newValues]);";
    //                         1                                                1          2               3
    static char synopsisString[] =
        "Return or set a floating point value vector property associated with OpenXR device 'openxrPtr'.\n\n"
        "'propertyName' String with name of the floats property to set/get. See OVR.KEY_XXX constants.\n\n"
        "'newValues' Optional vector with new floating point values to assign.\n\n"
        "Returns a vector 'oldValues' with the current (or old) floating point values associated with the property.\n"
        "Currently up to 100 elements can be retrieved or set for a property vector.\n";
    static char seeAlsoString[] = "";

    int handle, i, m, n, p, count;
    PsychOpenXRDevice *openxr;
    char *propertyName;
    float fvalues[100];
    double *values;
    psych_bool isscalar = FALSE;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Property name string:
    PsychAllocInCharArg(2, kPsychArgRequired, &propertyName);
    if (strlen(propertyName) == 0)
        PsychErrorExitMsg(PsychError_user, "Invalid, empty 'propertyName' provided.");
/*
    // Get current / old values in fvalues, and their count in count, at most 100 elements:
    count = ovr_GetFloatArray(openxr->hmd, (const char*) propertyName, fvalues, 100);

    // Copy them out to userspace:
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, count, 1, &values);
    for (i = 0; i < count; i++)
        *(values++) = (double) fvalues[i];

    // Optional new values provided?
    if (PsychAllocInDoubleMatArg(3, kPsychArgOptional, &m, &n, &p, &values)) {
        n = m * n * p;
        if (n > 100)
            PsychErrorExitMsg(PsychError_user, "Too many 'newValues' provided for property. Can not take more than 100.");

        if ((count > 0) && (n != count) && (verbosity > 1))
            printf("PsychOpenXRCore-WARNING: 'FloatsProperty' '%s' had %i values, but now trying to assign different number %i. Possible problem?\n",
                   propertyName, count, n);

        for (i = 0; i < n; i++)
            fvalues[i] = (float) values[i];

        if (!ovr_SetFloatArray(openxr->hmd, (const char*) propertyName, fvalues, n)) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: 'FloatsProperty' '%s' can not accept new values, as it is read-only!\n", propertyName);
            PsychErrorExitMsg(PsychError_user, "Tried to set new values for a read-only property");
        }
    }
*/
    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRFloatProperty(void)
{
    static char useString[] = "oldValues = PsychOpenXRCore('FloatProperty', openxrPtr, propertyName [, newValue]);";
    //                         1                                               1          2               3
    static char synopsisString[] =
        "Return or set a floating point value property associated with OpenXR device 'openxrPtr'.\n\n"
        "'propertyName' String with name of the float property to set/get. See OVR.KEY_XXX constants.\n\n"
        "'newValue' Optional new floating point value to assign.\n\n"
        "Returns a scalar 'oldValue' with the current (or old) floating point value associated with the property.\n";
    static char seeAlsoString[] = "";

    int handle;
    PsychOpenXRDevice *openxr;
    char *propertyName;
    float fvalue;
    double value;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Property name string:
    PsychAllocInCharArg(2, kPsychArgRequired, &propertyName);
    if (strlen(propertyName) == 0)
        PsychErrorExitMsg(PsychError_user, "Invalid, empty 'propertyName' provided.");
/*
    // Get float property:
    fvalue = ovr_GetFloat(openxr->hmd, (const char*) propertyName, FLT_MAX);

    // Copy it out to userspace:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) fvalue);

    // Optional new values provided?
    if (PsychCopyInDoubleArg(3, kPsychArgOptional, &value)) {
        if (!ovr_SetFloat(openxr->hmd, (const char*) propertyName, (float) value)) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: 'FloatProperty' '%s' can not accept new value, as it is read-only!\n", propertyName);
            PsychErrorExitMsg(PsychError_user, "Tried to set new value for a read-only property");
        }
    }
*/
    return(PsychError_none);
}

// TODO
PsychError PSYCHOPENXRStringProperty(void)
{
    static char useString[] = "oldString = PsychOpenXRCore('StringProperty', openxrPtr, propertyName [, defaultString][, newString]);";
    //                         1                                                1          2               3                4
    static char synopsisString[] =
        "Return or set a string property associated with OpenXR device 'openxrPtr'.\n\n"
        "'propertyName' String with name of the string property to set/get. See OVR.KEY_XXX constants.\n\n"
        "'defaultString' A string to return if string 'propertyName' is not defined yet.\n\n"
        "'newString' Optional new text string to assign.\n\n"
        "Returns a string 'oldString' with the current (or old, or 'defaultString') string associated with the property.\n";
    static char seeAlsoString[] = "";

    int handle;
    PsychOpenXRDevice *openxr;
    char *propertyName, *defaultVal;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(4));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Property name string:
    PsychAllocInCharArg(2, kPsychArgRequired, &propertyName);
    if (strlen(propertyName) == 0)
        PsychErrorExitMsg(PsychError_user, "Invalid, empty 'propertyName' provided.");

    // Optional default string:
    if (!PsychAllocInCharArg(3, kPsychArgOptional, &defaultVal))
        defaultVal = NULL;
/*
    // Return current / old string:
    PsychCopyOutCharArg(1, kPsychArgOptional, ovr_GetString(openxr->hmd, (const char*) propertyName, (const char*) defaultVal));

    // Optional new values provided?
    if (PsychAllocInCharArg(4, kPsychArgOptional, &defaultVal)) {
        if (!ovr_SetString(openxr->hmd, (const char*) propertyName, (const char*) defaultVal)) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: 'StringProperty' '%s' can not accept new value '%s', as it is read-only!\n", propertyName, defaultVal);
            PsychErrorExitMsg(PsychError_user, "Tried to set new values for a read-only string property");
        }
    }
*/
    return(PsychError_none);
}
