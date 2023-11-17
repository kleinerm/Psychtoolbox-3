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
 * Copyright (c) 2022-2023 Mario Kleiner. Licensed to you under the MIT license:
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

// Includes for use of nanopb for parsing of metrics protocol buffers produced
// by recent (late 2022) Monado OpenXR runtime on Linux:
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pb_decode.h>
#include <pb_common.h>
#include "monado_metrics.pb.h"
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
    psych_bool                          threadTerminate;                // MT mutex.
    psych_bool                          needFrameLoop;
    psych_bool                          sessionActive;
    psych_bool                          lossPending;
    psych_bool                          userExit;
    psych_bool                          needExtraCopyToLatch;
    int                                 multiThreaded;
    psych_thread                        presenterThread;
    psych_mutex                         presenterLock;
    psych_condition                     presentedSignal;
    int                                 handle;
    XrSystemId                          systemId;
    XrSession                           hmd;
    XrSessionState                      state;
    XrSwapchain                         textureSwapChain[2];            // MT mutex.
    uint32_t                            textureSwapChainLength[2];      // MT mutex.
    XrSwapchainImageOpenGLKHR*          textureSwapChainImages[2];      // MT mutex.
    unsigned int                        currentTextures[2];             // MT mutex.
    psych_bool                          isStereo;                       // MT mutex iff made dynamic.
    psych_bool                          use3DMode;                      // MT mutex iff made dynamic.
    int                                 textureWidth;
    int                                 textureHeight;
    int                                 maxWidth;
    int                                 maxHeight;
    int                                 recSamples;
    int                                 maxSamples;
    int                                 numMSAASamples;
    XrSpace                             gazePoseSpace;
    XrSpace                             handPoseSpace[2];
    int                                 hasEyeTracking;
    psych_bool                          isTracking;                     // MT mutex.
    psych_bool                          needLocate;                     // MT mutex.
    int                                 viewLayerType;                  // MT mutex.
    XrViewConfigurationType             viewType;
    XrViewState                         viewState;                      // MT mutex.
    XrView                              view[2];                        // MT mutex.
    XrCompositionLayerProjectionView    projView[2];                    // MT mutex.
    XrCompositionLayerQuad              quadViewLayer[2];               // MT mutex.
    XrSpace                             viewSpace;                      // MT mutex.
    XrSpace                             worldSpace;                     // MT mutex.
    XrReferenceSpaceType                worldSpaceType;                 // MT mutex.
    XrPosef                             originPoseInPreviousSpace;
    XrCompositionLayerProjection        projectionLayer;                // MT mutex.
    const XrCompositionLayerBaseHeader* submitLayers[2];                // MT mutex iff made dynamic.
    uint32_t                            submitLayersCount;              // MT mutex iff made dynamic.
    XrFrameState                        frameState;                     // Read by both threads, but not mutex protectable.
    XrTime                              predictedDisplayTime;           // MT mutex protected copy of frameState.predictedDisplayTime
    double                              frameDuration;
    double                              sensorSampleTime;
    double                              targetPresentTime;              // MT mutex.
    double                              tPredictedOnset;                // MT mutex.
    double                              VRtimeoutSecs;
    int                                 srcTexIds[2];
    int                                 srcFboIds[2];
    psych_int64                         frameIndex;

    #if PSYCH_SYSTEM == PSYCH_LINUX
    pb_istream_t                        pbstream;
    FILE*                               pbfile;
    #endif

    #ifdef XR_USE_PLATFORM_WIN32
    HDC                                 deviceContext;
    HGLRC                               openGLContext;
    #endif
} PsychOpenXRDevice;

// Set to TRUE if PsychOpenXRCoreShutDown() is executing:
static psych_bool calledFromPsychOpenXRCoreShutDown = FALSE;

// Shared XrInstance for the whole process:
static XrInstance xrInstance = XR_NULL_HANDLE;
static XrInstanceProperties instanceProperties;

// Only in SDK's of late 2022, e.g., not in Ubuntu 20.04 SDK:
#ifndef XR_HTC_VIVE_FOCUS3_CONTROLLER_INTERACTION_EXTENSION_NAME
#define XR_HTC_VIVE_FOCUS3_CONTROLLER_INTERACTION_EXTENSION_NAME "XR_HTC_vive_focus3_controller_interaction"
#endif

#ifndef XR_EXT_DPAD_BINDING_EXTENSION_NAME
#define XR_EXT_DPAD_BINDING_EXTENSION_NAME "XR_EXT_dpad_binding"
#endif

// Supported instance extensions:
static psych_bool has_XR_EXT_hp_mixed_reality_controller = FALSE;
static psych_bool has_XR_HTC_vive_cosmos_controller_interaction = FALSE;
static psych_bool has_XR_HTC_vive_focus3_controller_interaction = FALSE;
static psych_bool has_XR_EXT_eye_gaze_interaction = FALSE;
static psych_bool has_XR_EXT_dpad_binding = FALSE;

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
XrSystemEyeGazeInteractionPropertiesEXT eyeGazeAvailable[MAX_PSYCH_OPENXR_DEVS];
XrSystemProperties availableSystems[MAX_PSYCH_OPENXR_DEVS];
static int numAvailableDevices = 0;

static int verbosity = 3;
static psych_bool initialized = FALSE;

// Our errorString buffer needs to use thread local storage (TLS), so declare it
// in a compiler specific way:
#ifdef _MSC_VER
static __declspec(thread) char errorString[XR_MAX_RESULT_STRING_SIZE];
#else
static __thread char errorString[XR_MAX_RESULT_STRING_SIZE];
#endif

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

static XrPath openxr__handPath[4];
static XrActionSet openxr__actionSet;

static XrAction openxr__gazePoseAction;
static XrAction openxr__hapticAction;
static XrAction openxr__handPoseAction;
static XrAction openxr__triggerValueAction[2];      // 1D
static XrAction openxr__gripValueAction[2];         // 1D
static XrAction openxr__thumbStick2DAction[4];      // 2 x 2D
static XrAction openxr__buttonAction[NUM_OVR_BUTTONS];  // Bool
static XrAction openxr__touchAction[NUM_OVR_TOUCHES];   // Bool

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
    synopsis[i++] = "This driver allows to use XR devices supported by a suitable OpenXR runtime of version 1.x.\n";
    synopsis[i++] = "Copyright (c) 2022-2023 Mario Kleiner.";
    synopsis[i++] = "The PsychOpenXRCore driver is licensed to you under the terms of the MIT license.";
    synopsis[i++] = "";
    synopsis[i++] = "For some experimental Monado specific timestamping implementation, the driver currently";
    synopsis[i++] = "also contains a statically included copy of the nanopb protobuffer parsing library";
    synopsis[i++] = "(URL: https://jpa.kapsi.fi/nanopb) which is licensed under the zlib license, and statically ";
    synopsis[i++] = "included copies of some files from the FOSS Monado OpenXR runtime (https://monado.freedesktop.org), ";
    synopsis[i++] = "which are licensed under the Boost Software License BSL-1.0 - SPDX-License-Identifier: BSL-1.0.";
    synopsis[i++] = "The statically included files can be found in the nanopb subfolder of the PsychOpenXRCore source folder.";
    synopsis[i++] = "";
    synopsis[i++] = "See 'help License.txt' in the Psychtoolbox root folder for more details.";
    synopsis[i++] = "";
    synopsis[i++] = "Usage:";
    synopsis[i++] = "";
    synopsis[i++] = "Functions used by regular user scripts (but mostly indirectly via PsychVRHMD() or PsychOpenXR()):";
    synopsis[i++] = "";
    synopsis[i++] = "oldVerbosity = PsychOpenXRCore('Verbosity' [, verbosity]);";
    synopsis[i++] = "numDevices = PsychOpenXRCore('GetCount');";
    synopsis[i++] = "[openxrPtr, modelName, runtimeName, hasEyeTracking] = PsychOpenXRCore('Open' [, deviceIndex=0]);";
    synopsis[i++] = "PsychOpenXRCore('Close' [, openxrPtr]);";
    synopsis[i++] = "controllerTypes = PsychOpenXRCore('Controllers', openxrPtr);";
    synopsis[i++] = "[oldType, spaceSize] = PsychOpenXRCore('ReferenceSpaceType', openxrPtr [, newType]);";
    synopsis[i++] = "oldType = PsychOpenXRCore('ViewType', openxrPtr [, viewLayerType]);";
    synopsis[i++] = "[oldPosition, oldSize, oldOrientation] = PsychOpenXRCore('View2DParameters', openxrPtr, eye [, position][, size][, orientation]);";
    synopsis[i++] = "oldNeed = PsychOpenXRCore('NeedLocateForProjectionLayers', openxrPtr [, needLocate]);";
    synopsis[i++] = "oldEnable = PsychOpenXRCore('PresenterThreadEnable', openxrPtr [, enableThread]);";
    synopsis[i++] = "PsychOpenXRCore('Start', openxrPtr);";
    synopsis[i++] = "PsychOpenXRCore('Stop', openxrPtr);";
    synopsis[i++] = "[state, touch, gaze] = PsychOpenXRCore('GetTrackingState', openxrPtr [, predictionTime=nextFrame][, reqMask=all]);";
    synopsis[i++] = "input = PsychOpenXRCore('GetInputState', openxrPtr, controllerType);";
    synopsis[i++] = "pulseEndTime = PsychOpenXRCore('HapticPulse', openxrPtr, controllerType [, duration=2.5][, freq][, amplitude=1.0]);";
    synopsis[i++] = "[projL, projR, fovL, fovR] = PsychOpenXRCore('GetStaticRenderParameters', openxrPtr [, clipNear=0.01][, clipFar=10000.0]);";
    synopsis[i++] = "";
    synopsis[i++] = "Functions usually only used internally by Psychtoolbox:";
    synopsis[i++] = "";
    synopsis[i++] = "[width, height, recMSAASamples, maxMSAASamples, maxWidth, maxHeight] = PsychOpenXRCore('GetFovTextureSize', openxrPtr, eye);";
    synopsis[i++] = "videoRefreshDuration = PsychOpenXRCore('CreateAndStartSession', openxrPtr, deviceContext, openGLContext, openGLDrawable, openGLConfig, openGLVisualId, use3DMode, multiThreaded [, srcTexIds]);";
    synopsis[i++] = "[width, height, numTextures, imageFormat] = PsychOpenXRCore('CreateRenderTextureChain', openxrPtr, eye, width, height, floatFormat, numMSAASamples);";
    synopsis[i++] = "texObjectHandle = PsychOpenXRCore('GetNextTextureHandle', openxrPtr, eye);";
    synopsis[i++] = "[tPredictedOnset, tPredictedFutureOnset] = PsychOpenXRCore('PresentFrame', openxrPtr [, when=0]);";
    synopsis[i++] = "timingSupport = PsychOpenXRCore('TimingSupport' [, openxrPtr]);";
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

// Is openxr instance currently operating in multi-threaded presenter thread mode?
static psych_bool isMultithreaded(PsychOpenXRDevice *openxr)
{
    return(openxr->multiThreaded && (openxr->presenterThread != (psych_thread) NULL));
}

static void PsychOpenXRStopPresenterThread(PsychOpenXRDevice *openxr)
{
    // presenterThread shutdown, if any: Ask thread to terminate, wait for thread
    // termination, cleanup and release the thread:
    if (openxr->presenterThread != (psych_thread) NULL) {
        PsychLockMutex(&(openxr->presenterLock));
        openxr->threadTerminate = TRUE;
        PsychUnlockMutex(&(openxr->presenterLock));

        if (verbosity > 5)
            printf("PsychOpenXRCore-DEBUG: Waiting (join()ing) for helper thread on device with handle %i to finish up. If this doesn't happen quickly, you'll have to kill %s...\n",
                   openxr->handle, PSYCHTOOLBOX_SCRIPTING_LANGUAGE_NAME);

        PsychDeleteThread(&(openxr->presenterThread));
        openxr->presenterThread = (psych_thread) NULL;

        if (verbosity > 3)
            printf("PsychOpenXRCore-INFO: Stopped asynchronous XR presenter thread on device with handle %i.\n", openxr->handle);
    }

    openxr->threadTerminate = FALSE;
}

static psych_bool PsychOpenXRStartPresenterThread(PsychOpenXRDevice *openxr)
{
    int rc;

    // Start presenterThread if multi-threading allowed and not yet running:
    if ((openxr->multiThreaded) && (openxr->presenterThread == (psych_thread) NULL)) {
        // Create and startup thread, with targetPresentTime invalidated for startup:
        openxr->targetPresentTime = DBL_MAX;
        openxr->threadTerminate = FALSE;

        PsychLockMutex(&(openxr->presenterLock));

        if ((rc = PsychCreateThread(&(openxr->presenterThread), NULL, PresenterThreadMain, (void*) openxr))) {
            openxr->presenterThread = (psych_thread) NULL;
            printf("PsychOpenXRCore-ERROR: Could not create internal presenterThread on device with handle %i [%s].\n", openxr->handle, strerror(rc));
            PsychUnlockMutex(&(openxr->presenterLock));
            return(FALSE);
        }
        else if (verbosity > 3) {
            printf("PsychOpenXRCore-INFO: Started asynchronous XR presenter thread on device with handle %i.\n", openxr->handle);
        }

        // Wait for thread to signal it has started:
        if ((rc = PsychWaitCondition(&(openxr->presentedSignal), &(openxr->presenterLock)))) {
            printf("PsychOpenXRCore-ERROR: Waitcondition on presentedSignal / thread startup failed  [%s].\n", strerror(rc));
        }

        PsychUnlockMutex(&(openxr->presenterLock));
    }

    return(TRUE);
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
        qpc.QuadPart = (LONGLONG) (refTime * PsychGetKernelTimebaseFrequencyHz());

        // Map QPC time to XrTime:
        if (!resultOK(pxrConvertWin32PerformanceCounterToTimeKHR(xrInstance, &qpc, &outTime))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: xrConvertWin32PerformanceCounterToTimeKHR() failed: %s\n", errorString);
        }
    }
    #endif

    #if defined(XR_USE_PLATFORM_XLIB)
    if (pxrConvertTimespecTimeToTimeKHR) {
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
    else {
        // Fallback:
        outTime = (int64_t) (refTime * 1e9);
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
    if (pxrConvertTimeToTimespecTimeKHR) {
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
    else {
        // Fallback:
        outTime = ((double) xrTime) / 1e9;
        outTime = PsychOSMonotonicToRefTime(outTime);
    }
    #endif

    return(outTime);
}

#if PSYCH_SYSTEM == PSYCH_LINUX

// ----------------------------------------------------------------------------
// Helper functions for nanopb based protobuffer message fifo parsing of Monado
// metrics logging data for the purpose of Linux+Monado specific timestamping
// ----------------------------------------------------------------------------
static const pb_msgdesc_t* decode_unionmessage_type(pb_istream_t *stream)
{
    pb_wire_type_t wire_type;
    uint32_t tag;
    bool eof;

    while (pb_decode_tag(stream, &wire_type, &tag, &eof)) {
        if (wire_type == PB_WT_STRING) {
            pb_field_iter_t iter;
            if (pb_field_iter_begin(&iter, monado_metrics_Record_fields, NULL) &&
                pb_field_iter_find(&iter, tag))
            {
                /* Found our field. */
                return iter.submsg_desc;
            }
        }

        /* Wasn't our field.. */
        if (pb_skip_field(stream, wire_type)) {
            if (verbosity > 6)
                printf("PsychOpenXRCore-DEBUG: Metrics parse SKIPPED wire_type %d.\n", wire_type);
        }
        else if (verbosity > 6) {
            printf("PsychOpenXRCore-DEBUG: Metrics parse FAILSKIPPED wire_type %d.\n", wire_type);
        }
    }

    return NULL;
}

static bool decode_unionmessage_contents(pb_istream_t *stream, const pb_msgdesc_t *messagetype, void *dest_struct)
{
    pb_istream_t substream;
    bool status;

    if (!pb_make_string_substream(stream, &substream))
        return false;

    status = pb_decode(&substream, messagetype, dest_struct);
    pb_close_string_substream(stream, &substream);

    return status;
}

static bool monadoMetricsCallback(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    FILE *file = (FILE*) stream->state;
    bool status;

    if (buf == NULL) {
        while (count-- && fgetc(file) != EOF);
        return count == 0;
    }

    status = (fread(buf, 1, count, file) == count);

    if (feof(file))
        stream->bytes_left = 0;

    return status;
}

static double executeMonadoMetricsCycle(PsychOpenXRDevice *openxr, psych_int64 targetFrameIndex)
{
    static uint64_t oldSessionId = 0;              // Last reported OpenXR session id.
    static uint64_t waitedForClientFrame = 0;      // We wait for 1st actual XR device present of this client frame.
    static uint64_t waitedTargetTimeNsecs = 0;     // Target (desired) present time of the waitedForClientFrame / waitedForSystemFrame.
    static uint64_t waitedForSystemFrame = 0;      // We wait on present of this system frame to detect actual stimulus onset.

    const pb_msgdesc_t *type;
    uint64_t dummysize;
    double tNow;
    psych_bool status, done = FALSE;
    pb_istream_t* pbstream = &openxr->pbstream;
    uint64_t onsetTimeNsecs = 0;

    // Offset used by Monado to convert flip completion time to display time in nanoseconds. Currently 4 msecs hard-coded in Monado:
    const double scanoutStartToPhotonsOffsetNsecs = 4000000;

    // Metrics timestamping disabled or unsupported? Return no-op -1 in that case:
    if (!openxr->pbfile)
        return(-1);

    while (!done) {
        status = false;

        // Decode and throw away varint with message size info:
        pb_decode_varint(pbstream, &dummysize);

        // Decode type of message:
        type = decode_unionmessage_type(pbstream);

        tNow = PsychGetAdjustedPrecisionTimerSeconds(NULL);

        // Choose suitable decoder call:
        if (type == monado_metrics_Version_fields) {
            // Protocol version: We only support version 1.1 at the moment:
            monado_metrics_Version msg = {};

            status = decode_unionmessage_contents(pbstream, type, &msg);

            if (verbosity > 3)
                printf("PsychOpenXRCore-INFO: Using Monado metrics protocol version: %d.%d\n", msg.major, msg.minor);

            if ((msg.major != 1 || msg.minor != 1) && (verbosity >= 2))
                printf("PsychOpenXRCore-WARNING: Not required version 1.1. Expect possible severe trouble due to miscommunication Monado <-> PsychOpenXRCore!\n");
        }
        else if (type == monado_metrics_SystemFrame_fields) {
            // Something not so important. Eat up and ignore:
            monado_metrics_SystemFrame msg = {};

            status = decode_unionmessage_contents(pbstream, type, &msg);

            if (verbosity > 6)
                printf("PsychOpenXRCore-DEBUG:[%f]: Got monado_metrics_SystemFrame\n", tNow);
        }
        else if (type == monado_metrics_SystemGpuInfo_fields) {
            // Something not so important. Eat up and ignore:
            monado_metrics_SystemGpuInfo msg = {};

            status = decode_unionmessage_contents(pbstream, type, &msg);

            if (verbosity > 6)
                printf("PsychOpenXRCore-DEBUG:[%f]: Got monado_metrics_SystemGpuInfo\n", tNow);
        }
        else if (type == monado_metrics_SessionFrame_fields) {
            // MonadoXR compositor has received our latest submitted XR frame for future display:
            monado_metrics_SessionFrame msg = {};

            status = decode_unionmessage_contents(pbstream, type, &msg);

            if (verbosity > 6)
                printf("PsychOpenXRCore-DEBUG:[%f]: %s Received session frame:    session_id %ld :: session_frame_id %ld :: predicted_display_time_ns %ld :: display_time_ns %ld\n",
                       tNow, msg.discarded ? "Discarded" : "Got",  msg.session_id, msg.frame_id, msg.predicted_display_time_ns, msg.display_time_ns);

            if (msg.session_id > oldSessionId) {
                if (verbosity > 3)
                    printf("PsychOpenXRCore-INFO: [%f]: New OpenXR session %ld started -- Resetting.\n", tNow, msg.session_id);

                oldSessionId = msg.session_id;
                waitedForClientFrame = 0;
                waitedTargetTimeNsecs = 0;
                waitedForSystemFrame = 0;
            }

            // Ignore all frames older than targetFrameIndex:
            if (msg.frame_id == targetFrameIndex) {
                if (verbosity > 5)
                    printf("PsychOpenXRCore-INFO: [%f]: New frame %ld from client %s for future initial present at desired_present_time_ns %ld\n", tNow,
                           msg.frame_id, msg.discarded ? "DISCARDED" : "submitted", msg.display_time_ns);

                // Discarded frames are finished early with return code 0 == Discarded:
                if (msg.discarded)
                    return(0);

                if (waitedForClientFrame != 0) {
                    if (verbosity > 1)
                        printf("PsychOpenXRCore-INFO: [%f]: New frame %ld from client while old one %ld still pending initial present...\n",
                               tNow, msg.frame_id, waitedForClientFrame);
                }

                waitedForClientFrame = msg.frame_id;
                waitedTargetTimeNsecs = msg.display_time_ns;
                waitedForSystemFrame = 0;
            }
        }
        else if (type == monado_metrics_Used_fields) {
            // Info about a submitted frame currently latched for presentation at next flip:
            monado_metrics_Used msg = {};

            status = decode_unionmessage_contents(pbstream, type, &msg);

            if (verbosity > 6)
                printf("PsychOpenXRCore-DEBUG:[%f]: Got monado_metrics_Used:      session_id %ld :: session_frame_id %ld :: system_frame_id %ld :: when %ld nsecs\n", tNow,
                       msg.session_id, msg.session_frame_id, msg.system_frame_id, msg.when_ns);

            // If no client frame from us is registered for waiting yet, ignore this record:
            if (waitedForClientFrame > 0 && waitedForSystemFrame == 0) {
                // Client frame picked for XR present cycle the 1st time?
                if (msg.session_frame_id == waitedForClientFrame) {
                    if (verbosity > 5)
                        printf("PsychOpenXRCore-INFO: [%f]: New frame %ld from client about to do initial present in system frame %ld.\n", tNow,
                                msg.session_frame_id, msg.system_frame_id);

                    waitedForSystemFrame = msg.system_frame_id;
                }
            }
        }
        else if (type == monado_metrics_SystemPresentInfo_fields) {
            // New image present completed on the HMD, ie. kms pageflipped onto scanout and
            // reported by VK_GOOGLE_display_timing et al.:
            monado_metrics_SystemPresentInfo msg = {};

            status = decode_unionmessage_contents(pbstream, type, &msg);

            if (verbosity > 6)
                printf("PsychOpenXRCore-DEBUG:[%f]: Got monado_metrics_SystemPresentInfo: frame_id %ld :: desired_present_time_ns %ld vs. actual_present_time_ns %ld :: Delta %ld\n",
                       tNow, msg.frame_id, msg.desired_present_time_ns, msg.actual_present_time_ns, msg.actual_present_time_ns - msg.desired_present_time_ns);

            // If no client frame from us is registered for waiting yet, ignore this record:
            if (waitedForClientFrame > 0 && waitedForSystemFrame > 0) {
                // Waited for initial present of a new client frame?
                if (msg.frame_id == waitedForSystemFrame) {
                    // Yes, this is the first present of our waitedForClientFrame. Present completion!
                    onsetTimeNsecs = msg.actual_present_time_ns + scanoutStartToPhotonsOffsetNsecs;

                    // Bingo!
                    done = TRUE;

                    if (verbosity > 5)
                        printf("PsychOpenXRCore-INFO: [%f]: Frame %ld from client presented in system frame %ld at time %ld nsecs. Delay wrt. desired onset time %f msecs.\n",
                               tNow, waitedForClientFrame, waitedForSystemFrame, onsetTimeNsecs, (double) (onsetTimeNsecs - waitedTargetTimeNsecs) / 1e6);

                    // Invalidate and update:
                    waitedForClientFrame = 0;
                    waitedTargetTimeNsecs = 0;
                    waitedForSystemFrame = 0;
                }
            }
        }

        // Trouble in decode?
        if (!status) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Metrics message decode failed: %s\n", PB_GET_ERROR(pbstream));

            return(-1);
        }
    }

    // Success: Convert from CLOCK_MONOTONIC nanoseconds to GetSecs CLOCK_REALTIME seconds and add
    // the offset from start-of-scanout time to photon time when the display lights up with new image:
    return(PsychOSMonotonicToRefTime(((double) onsetTimeNsecs) / 1e9));
}
#else
// Unsupported on non-Linux:
static double executeMonadoMetricsCycle(PsychOpenXRDevice *openxr, psych_int64 targetFrameIndex) { return(-1); }
#endif

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

    if (has_XR_EXT_eye_gaze_interaction)
        availableSystems[numAvailableDevices].next = &eyeGazeAvailable[numAvailableDevices];

    // Got a hardware XR system. Query and store its properties and systemId:
    result = xrGetSystemProperties(instance, systemId, &availableSystems[numAvailableDevices]);
    if (!resultOK(result)) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to query properties of available XR system hardware: %s\n", errorString);

        // Failure return code:
        return(-1);
    }

    if (verbosity > 3) {
        printf("PsychOpenXRCore-INFO: %i. XR system: VendorId 0x%x : \"%s\" : orientationTracking %i : positionTracking %i : %i layers of max size %i x %i : Gaze tracking: %i.\n",
               numAvailableDevices,
               availableSystems[numAvailableDevices].vendorId, availableSystems[numAvailableDevices].systemName,
               availableSystems[numAvailableDevices].trackingProperties.orientationTracking,
               availableSystems[numAvailableDevices].trackingProperties.positionTracking,
               availableSystems[numAvailableDevices].graphicsProperties.maxLayerCount,
               availableSystems[numAvailableDevices].graphicsProperties.maxSwapchainImageWidth,
               availableSystems[numAvailableDevices].graphicsProperties.maxSwapchainImageHeight,
               eyeGazeAvailable[numAvailableDevices].supportsEyeGazeInteraction);
    }

    // Increment count of available devices:
    numAvailableDevices++;

    return(numAvailableDevices);
}

static void PsychAssignSubmitLayers(PsychOpenXRDevice *openxr, int viewLayerType)
{
    if (viewLayerType == 1) {
        // 3D perspective projected mode: Assign stereo projectionLayer:
        openxr->submitLayers[0] = (XrCompositionLayerBaseHeader*) &openxr->projectionLayer;
        openxr->submitLayersCount = 1;
    }
    else {
        // 2D mode: Assign the quadViewLayer(s) for mono or stereo 2D display:
        openxr->submitLayers[0] = (XrCompositionLayerBaseHeader*) &openxr->quadViewLayer[0];
        openxr->submitLayers[1] = (XrCompositionLayerBaseHeader*) &openxr->quadViewLayer[1];
        openxr->submitLayersCount = (openxr->isStereo) ? 2 : 1;
    }

    openxr->viewLayerType = viewLayerType;
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

static int getActiveControllers(XrInstance xrInstance, PsychOpenXRDevice *openxr)
{
    char profile[128];
    int i, controllerTypes;

    profile[0] = 0;
    controllerTypes = 0;

    for (i = 0; i < 4; i++) {
        if (getActiveXRInteractionProfile(xrInstance, openxr, openxr__handPath[i], profile)) {
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

    return(controllerTypes);
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

static psych_bool createDefaultXRInputConfig(XrInstance xrInstance)
{
    int i;

    // Build all standard path strings we may need:
    xrStringToPath(xrInstance, "/user/hand/left", &openxr__handPath[0]);
    xrStringToPath(xrInstance, "/user/hand/right", &openxr__handPath[1]);
    xrStringToPath(xrInstance, "/user/gamepad", &openxr__handPath[2]);
    xrStringToPath(xrInstance, "/user/head", &openxr__handPath[3]);

    XrPath openxr__gazePath;
    xrStringToPath(xrInstance, "/user/eyes_ext/input/gaze_ext/pose", &openxr__gazePath);

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

    XrPath gripValuePath[2];
    xrStringToPath(xrInstance, "/user/hand/left/input/squeeze/value", &gripValuePath[0]);
    xrStringToPath(xrInstance, "/user/hand/right/input/squeeze/value", &gripValuePath[1]);

    XrPath gripClickPath[2];
    xrStringToPath(xrInstance, "/user/hand/left/input/squeeze/click", &gripClickPath[0]);
    xrStringToPath(xrInstance, "/user/hand/right/input/squeeze/click", &gripClickPath[1]);

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

    if (!resultOK(xrCreateActionSet(xrInstance, &actionSetCreateInfo, &openxr__actionSet))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to create main input/output actionSet: %s\n", errorString);

        // Failure return code:
        return(FALSE);
    }

    // Create action and action space for eye gaze vector tracking via EXT_eye_gaze_interaction:
    XrActionCreateInfo gazePoseActionInfo = {
        .type = XR_TYPE_ACTION_CREATE_INFO,
        .next = NULL,
        .actionType = XR_ACTION_TYPE_POSE_INPUT,
        .countSubactionPaths = 0,
        .subactionPaths = NULL,
        .actionName = "gazeposeaction",
        .localizedActionName = "Gaze Pose input action"
    };

    if (!resultOK(xrCreateAction(openxr__actionSet, &gazePoseActionInfo, &openxr__gazePoseAction))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to create gazePoseAction: %s\n", errorString);

        // Failure return code:
        return(FALSE);
    }

    // Create action and action spaces for left/right hand (aka touch controller) pose tracking:
    XrActionCreateInfo handPoseActionInfo = {
        .type = XR_TYPE_ACTION_CREATE_INFO,
        .next = NULL,
        .actionType = XR_ACTION_TYPE_POSE_INPUT,
        .countSubactionPaths = 2,
        .subactionPaths = openxr__handPath,
        .actionName = "handposeaction",
        .localizedActionName = "Hand Pose input action"
    };

    if (!resultOK(xrCreateAction(openxr__actionSet, &handPoseActionInfo, &openxr__handPoseAction))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to create handPoseAction: %s\n", errorString);

        // Failure return code:
        return(FALSE);
    }

    // Define haptic feedback output action:
    XrActionCreateInfo hapticActionInfo = {
        .type = XR_TYPE_ACTION_CREATE_INFO,
        .next = NULL,
        .actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT,
        .countSubactionPaths = 3,
        .subactionPaths = openxr__handPath,
        .actionName = "handhapticaction",
        .localizedActionName = "Hand haptic output action"
    };

    if (!resultOK(xrCreateAction(openxr__actionSet, &hapticActionInfo, &openxr__hapticAction))) {
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
            .subactionPaths = openxr__handPath,
            .actionName = "triggervalueactionleft",
            .localizedActionName = "Left-Trigger value"
        };

        if (!resultOK(xrCreateAction(openxr__actionSet, &triggerValueActionInfo, &openxr__triggerValueAction[0]))) {
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
            .subactionPaths = openxr__handPath,
            .actionName = "triggervalueactionright",
            .localizedActionName = "Right-Trigger value"
        };

        if (!resultOK(xrCreateAction(openxr__actionSet, &triggerValueActionInfo, &openxr__triggerValueAction[1]))) {
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
            .subactionPaths = openxr__handPath,
            .actionName = "gripvalueactionleft",
            .localizedActionName = "Left-Grip value"
        };

        if (!resultOK(xrCreateAction(openxr__actionSet, &gripValueActionInfo, &openxr__gripValueAction[0]))) {
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
            .subactionPaths = openxr__handPath,
            .actionName = "gripvalueactionright",
            .localizedActionName = "Right-Grip value"
        };

        if (!resultOK(xrCreateAction(openxr__actionSet, &gripValueActionInfo, &openxr__gripValueAction[1]))) {
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
            .subactionPaths = openxr__handPath,
            .actionName = "thumbstickactionleft",
            .localizedActionName = "Left-Thumbstick"
        };

        if (!resultOK(xrCreateAction(openxr__actionSet, &thumbStick2DActionInfo, &openxr__thumbStick2DAction[0]))) {
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
            .subactionPaths = openxr__handPath,
            .actionName = "thumbstickactionright",
            .localizedActionName = "Right-Thumbstick"
        };

        if (!resultOK(xrCreateAction(openxr__actionSet, &thumbStick2DActionInfo, &openxr__thumbStick2DAction[1]))) {
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
            .subactionPaths = openxr__handPath,
            .actionName = "thumbstickactionleft2",
            .localizedActionName = "Left-Thumbstick2"
        };

        if (!resultOK(xrCreateAction(openxr__actionSet, &thumbStick2DActionInfo, &openxr__thumbStick2DAction[2]))) {
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
            .subactionPaths = openxr__handPath,
            .actionName = "thumbstickactionright2",
            .localizedActionName = "Right-Thumbstick2"
        };

        if (!resultOK(xrCreateAction(openxr__actionSet, &thumbStick2DActionInfo, &openxr__thumbStick2DAction[3]))) {
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
            .subactionPaths = openxr__handPath,
        };

        sprintf(buttonActionInfo.actionName, "buttonaction_%i", i);
        sprintf(buttonActionInfo.localizedActionName, "Button %i", i);

        if (!resultOK(xrCreateAction(openxr__actionSet, &buttonActionInfo, &openxr__buttonAction[i]))) {
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
            .subactionPaths = openxr__handPath,
        };

        sprintf(touchActionInfo.actionName, "touchaction_%i", i);
        sprintf(touchActionInfo.localizedActionName, "Touch %i", i);

        if (!resultOK(xrCreateAction(openxr__actionSet, &touchActionInfo, &openxr__touchAction[i]))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create touchAction[%i]: %s\n", i, errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    #define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
    #define BBIND(a, b) { .action = openxr__buttonAction[(a)], .binding = toXrPath(xrInstance, (b)) }
    #define TBIND(a, b) { .action = openxr__touchAction[(a)], .binding = toXrPath(xrInstance, (b)) }

    // Establish binding for eye gaze interaction extension if it is supported:
    if (has_XR_EXT_eye_gaze_interaction) {
        XrActionSuggestedBinding eyeGazeBinding[] = {
            { .action = openxr__gazePoseAction, .binding = openxr__gazePath }
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/ext/eye_gaze_interaction", ARRAY_SIZE(eyeGazeBinding), eyeGazeBinding))
            return(FALSE);
    }

    // Binding set useful to most controllers and devices:
    XrActionSuggestedBinding unused_template[] = {
        { .action = openxr__handPoseAction, .binding = aimPosePath[0] },
        { .action = openxr__handPoseAction, .binding = aimPosePath[1] },
        { .action = openxr__hapticAction, .binding = hapticPath[0] }, // Not Daydream
        { .action = openxr__hapticAction, .binding = hapticPath[1] }, // Not Daydream
        { .action = openxr__triggerValueAction[0], .binding = triggerPath[0] }, // Not simple or Daydream
        { .action = openxr__triggerValueAction[1], .binding = triggerPath[1] }, // Not simple or Daydream
        { .action = openxr__gripValueAction[0], .binding = gripValuePath[0] }, // Oculus Touch, Valve Index, HP mixed reality, HTC Vive Focus 3
        { .action = openxr__gripValueAction[1], .binding = gripValuePath[1] }, // Oculus Touch, Valve Index, HP mixed reality, HTC Vive Focus 3
        { .action = openxr__thumbStick2DAction[0], .binding = thumbStickPath[0] }, // Not simple or Daydream or Vive
        { .action = openxr__thumbStick2DAction[1], .binding = thumbStickPath[1] }, // Not simple or Daydream or Vive
        { .action = openxr__thumbStick2DAction[2], .binding = trackpadPath[0] }, // Not simple or Daydream or Vive
        { .action = openxr__thumbStick2DAction[3], .binding = trackpadPath[1] }, // Not simple or Daydream or Vive
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
            { .action = openxr__handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr__handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr__hapticAction, .binding = hapticPath[0] },
            { .action = openxr__hapticAction, .binding = hapticPath[1] },
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
            { .action = openxr__handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr__handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr__thumbStick2DAction[0], .binding = trackpadPath[0] },
            { .action = openxr__thumbStick2DAction[1], .binding = trackpadPath[1] },
            BBIND(OVR_Button_Enter, "/user/hand/left/input/select/click"),
            BBIND(OVR_Button_Enter, "/user/hand/right/input/select/click"),
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
            { .action = openxr__handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr__handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr__hapticAction, .binding = hapticPath[0] },
            { .action = openxr__hapticAction, .binding = hapticPath[1] },
            { .action = openxr__triggerValueAction[0], .binding = triggerPath[0] },
            { .action = openxr__triggerValueAction[1], .binding = triggerPath[1] },
            { .action = openxr__gripValueAction[0], .binding = gripClickPath[0] },
            { .action = openxr__gripValueAction[1], .binding = gripClickPath[1] },
            { .action = openxr__thumbStick2DAction[0], .binding = trackpadPath[0] },
            { .action = openxr__thumbStick2DAction[1], .binding = trackpadPath[1] },
            BBIND(OVR_Button_Enter, "/user/hand/left/input/menu/click"),        // For consistency with Oculus touch left.
            BBIND(OVR_Button_Home, "/user/hand/right/input/menu/click"),        // For consistency with Oculus touch right.
            BBIND(OVR_Button_A, "/user/hand/right/input/trigger/click"),        // Unusual to map on right trigger, but gives us a A button.
            BBIND(OVR_Button_X, "/user/hand/left/input/trigger/click"),         // Unusual to map on left trigger, but gives us a B button.
            BBIND(OVR_Button_B, "/user/hand/right/input/system/click"),         // Map B and Y to right/left system buttons. These will often not
            BBIND(OVR_Button_Y, "/user/hand/left/input/system/click"),          // work, as they are blocked by runtime, e.g., on SteamVR+Windows.
            BBIND(OVR_Button_LThumb, "/user/hand/left/input/trackpad/click"),
            BBIND(OVR_Button_RThumb, "/user/hand/right/input/trackpad/click"),
            TBIND(OVR_Touch_LThumb, "/user/hand/left/input/trackpad/touch"),
            TBIND(OVR_Touch_RThumb, "/user/hand/right/input/trackpad/touch"),

            // Only if has_XR_EXT_dpad_binding:
            BBIND(OVR_Button_Up, "/user/hand/left/input/trackpad/dpad_up"),
            BBIND(OVR_Button_Up, "/user/hand/right/input/trackpad/dpad_up"),
            BBIND(OVR_Button_Down, "/user/hand/left/input/trackpad/dpad_down"),
            BBIND(OVR_Button_Down, "/user/hand/right/input/trackpad/dpad_down"),
            BBIND(OVR_Button_Left, "/user/hand/left/input/trackpad/dpad_left"),
            BBIND(OVR_Button_Left, "/user/hand/right/input/trackpad/dpad_left"),
            BBIND(OVR_Button_Right, "/user/hand/left/input/trackpad/dpad_right"),
            BBIND(OVR_Button_Right, "/user/hand/right/input/trackpad/dpad_right"),
        };

        // Only add last 8 entries if has_XR_EXT_dpad_binding is available:
        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/htc/vive_controller",
            has_XR_EXT_dpad_binding ? ARRAY_SIZE(viveBinding) : ARRAY_SIZE(viveBinding) - 8, viveBinding))
            return(FALSE);
    }

    // Suggest basic action bindings for the Oculus touch controller interaction profile, which
    // is used by Oculus VR touch input controllers, e.g., for Oculus Rift-CV1, Rift-S, Quest etc.:
    {
        XrActionSuggestedBinding oculusBinding[] = {
            { .action = openxr__handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr__handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr__hapticAction, .binding = hapticPath[0] },
            { .action = openxr__hapticAction, .binding = hapticPath[1] },
            { .action = openxr__triggerValueAction[0], .binding = triggerPath[0] },
            { .action = openxr__triggerValueAction[1], .binding = triggerPath[1] },
            { .action = openxr__gripValueAction[0], .binding = gripValuePath[0] },
            { .action = openxr__gripValueAction[1], .binding = gripValuePath[1] },
            { .action = openxr__thumbStick2DAction[0], .binding = thumbStickPath[0] },
            { .action = openxr__thumbStick2DAction[1], .binding = thumbStickPath[1] },
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
            { .action = openxr__handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr__handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr__triggerValueAction[0], .binding = triggerPath[0] },
            { .action = openxr__triggerValueAction[1], .binding = triggerPath[1] },
            { .action = openxr__thumbStick2DAction[0], .binding = trackpadPath[0] },
            { .action = openxr__thumbStick2DAction[1], .binding = trackpadPath[1] },
            BBIND(OVR_Button_LThumb, "/user/hand/left/input/trackpad/click"),
            BBIND(OVR_Button_RThumb, "/user/hand/right/input/trackpad/click"),
            BBIND(OVR_Button_Back, "/user/hand/left/input/back/click"),
            BBIND(OVR_Button_Back, "/user/hand/right/input/back/click"),
            TBIND(OVR_Touch_LThumb, "/user/hand/left/input/trackpad/touch"),
            TBIND(OVR_Touch_RThumb, "/user/hand/right/input/trackpad/touch"),
            BBIND(OVR_Button_Enter, "/user/hand/left/input/system/click"),
            BBIND(OVR_Button_Enter, "/user/hand/right/input/system/click"),
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/oculus/go_controller", ARRAY_SIZE(oculusGoBinding), oculusGoBinding))
            return(FALSE);
    }

    // Suggest basic action bindings for the Valve Index controller interaction profile.
    {
        XrActionSuggestedBinding valveIndexBinding[] = {
            { .action = openxr__handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr__handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr__hapticAction, .binding = hapticPath[0] }, // Not Daydream
            { .action = openxr__hapticAction, .binding = hapticPath[1] }, // Not Daydream
            { .action = openxr__triggerValueAction[0], .binding = triggerPath[0] },
            { .action = openxr__triggerValueAction[1], .binding = triggerPath[1] },
            { .action = openxr__gripValueAction[0], .binding = gripValuePath[0] },
            { .action = openxr__gripValueAction[1], .binding = gripValuePath[1] },
            { .action = openxr__thumbStick2DAction[0], .binding = thumbStickPath[0] },
            { .action = openxr__thumbStick2DAction[1], .binding = thumbStickPath[1] },
            // Trackpad is mapped to Thumbstick2:
            { .action = openxr__thumbStick2DAction[2], .binding = trackpadPath[0] },
            { .action = openxr__thumbStick2DAction[3], .binding = trackpadPath[1] },
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
            BBIND(OVR_Button_Enter, "/user/hand/left/input/system/click"),
            BBIND(OVR_Button_Enter, "/user/hand/right/input/system/click"),
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/valve/index_controller", ARRAY_SIZE(valveIndexBinding), valveIndexBinding))
            return(FALSE);
    }

    // Suggest basic action bindings for the Microsoft Mixed Reality (WMR) controller interaction profile.
    {
        XrActionSuggestedBinding wmrBinding[] = {
            { .action = openxr__handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr__handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr__hapticAction, .binding = hapticPath[0] },
            { .action = openxr__hapticAction, .binding = hapticPath[1] },
            { .action = openxr__triggerValueAction[0], .binding = triggerPath[0] },
            { .action = openxr__triggerValueAction[1], .binding = triggerPath[1] },
            { .action = openxr__gripValueAction[0], .binding = gripClickPath[0] },
            { .action = openxr__gripValueAction[1], .binding = gripClickPath[1] },
            { .action = openxr__thumbStick2DAction[0], .binding = thumbStickPath[0] },
            { .action = openxr__thumbStick2DAction[1], .binding = thumbStickPath[1] },
            // Trackpad is mapped to Thumbstick2:
            { .action = openxr__thumbStick2DAction[2], .binding = trackpadPath[0] },
            { .action = openxr__thumbStick2DAction[3], .binding = trackpadPath[1] },
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
            { .action = openxr__hapticAction, .binding = hapticPath[2] },
            { .action = openxr__hapticAction, .binding = hapticPath[3] },
            { .action = openxr__triggerValueAction[0], .binding = triggerPath[2] },
            { .action = openxr__triggerValueAction[1], .binding = triggerPath[3] },
            { .action = openxr__thumbStick2DAction[0], .binding = thumbStickPath[2] },
            { .action = openxr__thumbStick2DAction[1], .binding = thumbStickPath[3] },
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
            BBIND(OVR_Button_Home, "/user/head/input/system/click"),            // Does not work on SteamVR.
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/htc/vive_pro", ARRAY_SIZE(viveProBinding), viveProBinding))
            return(FALSE);
    }

    // Suggest action bindings for XR_EXT_hp_mixed_reality_controller:
    if (has_XR_EXT_hp_mixed_reality_controller) {
        XrActionSuggestedBinding hpMRBinding[] = {
            { .action = openxr__handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr__handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr__hapticAction, .binding = hapticPath[0] },
            { .action = openxr__hapticAction, .binding = hapticPath[1] },
            { .action = openxr__triggerValueAction[0], .binding = triggerPath[0] },
            { .action = openxr__triggerValueAction[1], .binding = triggerPath[1] },
            { .action = openxr__gripValueAction[0], .binding = gripValuePath[0] },
            { .action = openxr__gripValueAction[1], .binding = gripValuePath[1] },
            { .action = openxr__thumbStick2DAction[0], .binding = thumbStickPath[0] },
            { .action = openxr__thumbStick2DAction[1], .binding = thumbStickPath[1] },
            BBIND(OVR_Button_A, "/user/hand/right/input/a/click"),
            BBIND(OVR_Button_B, "/user/hand/right/input/b/click"),
            BBIND(OVR_Button_X, "/user/hand/left/input/x/click"),
            BBIND(OVR_Button_Y, "/user/hand/left/input/y/click"),
            BBIND(OVR_Button_LThumb, "/user/hand/left/input/thumbstick/click"),
            BBIND(OVR_Button_RThumb, "/user/hand/right/input/thumbstick/click"),
            BBIND(OVR_Button_Enter, "/user/hand/left/input/menu/click"),
            BBIND(OVR_Button_Back, "/user/hand/right/input/menu/click"),
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/hp/mixed_reality_controller", ARRAY_SIZE(hpMRBinding), hpMRBinding))
            return(FALSE);
    }

    // Suggest action bindings for XR_HTC_vive_cosmos_controller_interaction:
    if (has_XR_HTC_vive_cosmos_controller_interaction) {
        XrActionSuggestedBinding viveCosmosBinding[] = {
            { .action = openxr__handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr__handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr__hapticAction, .binding = hapticPath[0] },
            { .action = openxr__hapticAction, .binding = hapticPath[1] },
            { .action = openxr__triggerValueAction[0], .binding = triggerPath[0] },
            { .action = openxr__triggerValueAction[1], .binding = triggerPath[1] },
            { .action = openxr__gripValueAction[0], .binding = gripClickPath[0] },
            { .action = openxr__gripValueAction[1], .binding = gripClickPath[1] },
            { .action = openxr__thumbStick2DAction[0], .binding = thumbStickPath[0] },
            { .action = openxr__thumbStick2DAction[1], .binding = thumbStickPath[1] },
            BBIND(OVR_Button_A, "/user/hand/right/input/a/click"),
            BBIND(OVR_Button_B, "/user/hand/right/input/b/click"),
            BBIND(OVR_Button_X, "/user/hand/left/input/x/click"),
            BBIND(OVR_Button_Y, "/user/hand/left/input/y/click"),
            BBIND(OVR_Button_LThumb, "/user/hand/left/input/thumbstick/click"),
            BBIND(OVR_Button_RThumb, "/user/hand/right/input/thumbstick/click"),
            BBIND(OVR_Button_Enter, "/user/hand/left/input/menu/click"),
            BBIND(OVR_Button_Home, "/user/hand/right/input/system/click"),
            // (Ab)use OVR_Button_Left and OVR_Button_Right for trigger click:
            BBIND(OVR_Button_Left, "/user/hand/left/input/trigger/click"),
            BBIND(OVR_Button_Right, "/user/hand/right/input/trigger/click"),
            BBIND(OVR_Button_LShoulder, "/user/hand/left/input/shoulder/click"),
            BBIND(OVR_Button_RShoulder, "/user/hand/right/input/shoulder/click"),
            TBIND(OVR_Touch_LThumb, "/user/hand/left/input/thumbstick/touch"),
            TBIND(OVR_Touch_RThumb, "/user/hand/right/input/thumbstick/touch"),
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/htc/vive_cosmos_controller", ARRAY_SIZE(viveCosmosBinding), viveCosmosBinding))
            return(FALSE);
    }

    // Suggest action bindings for XR_HTC_vive_focus3_controller_interaction:
    if (has_XR_HTC_vive_focus3_controller_interaction) {
        XrActionSuggestedBinding viveFocus3Binding[] = {
            { .action = openxr__handPoseAction, .binding = aimPosePath[0] },
            { .action = openxr__handPoseAction, .binding = aimPosePath[1] },
            { .action = openxr__hapticAction, .binding = hapticPath[0] },
            { .action = openxr__hapticAction, .binding = hapticPath[1] },
            { .action = openxr__triggerValueAction[0], .binding = triggerPath[0] },
            { .action = openxr__triggerValueAction[1], .binding = triggerPath[1] },
            { .action = openxr__gripValueAction[0], .binding = gripValuePath[0] },
            { .action = openxr__gripValueAction[1], .binding = gripValuePath[1] },
            { .action = openxr__thumbStick2DAction[0], .binding = thumbStickPath[0] },
            { .action = openxr__thumbStick2DAction[1], .binding = thumbStickPath[1] },
            BBIND(OVR_Button_A, "/user/hand/right/input/a/click"),
            BBIND(OVR_Button_B, "/user/hand/right/input/b/click"),
            BBIND(OVR_Button_X, "/user/hand/left/input/x/click"),
            BBIND(OVR_Button_Y, "/user/hand/left/input/y/click"),
            BBIND(OVR_Button_LThumb, "/user/hand/left/input/thumbstick/click"),
            BBIND(OVR_Button_RThumb, "/user/hand/right/input/thumbstick/click"),
            BBIND(OVR_Button_Enter, "/user/hand/left/input/menu/click"),
            BBIND(OVR_Button_Home, "/user/hand/right/input/system/click"),
            // (Ab)use OVR_Button_Left and OVR_Button_Right for trigger click:
            BBIND(OVR_Button_Left, "/user/hand/left/input/trigger/click"),
            BBIND(OVR_Button_Right, "/user/hand/right/input/trigger/click"),
            // (Ab)use OVR_Button_LShoulder and OVR_Button_RShoulder for squeeze/grip click:
            BBIND(OVR_Button_LShoulder, "/user/hand/left/input/squeeze/click"),
            BBIND(OVR_Button_RShoulder, "/user/hand/right/input/squeeze/click"),

            TBIND(OVR_Touch_LIndexTrigger, "/user/hand/left/input/trigger/touch"),
            TBIND(OVR_Touch_RIndexTrigger, "/user/hand/right/input/trigger/touch"),
            TBIND(OVR_Touch_LThumb, "/user/hand/left/input/thumbstick/touch"),
            TBIND(OVR_Touch_RThumb, "/user/hand/right/input/thumbstick/touch"),
            TBIND(OVR_Touch_LThumbRest, "/user/hand/left/input/thumbrest/touch"),
            TBIND(OVR_Touch_RThumbRest, "/user/hand/right/input/thumbrest/touch"),
        };

        if (!suggestXRInteractionBindings(xrInstance, "/interaction_profiles/htc/vive_focus3_controller", ARRAY_SIZE(viveFocus3Binding), viveFocus3Binding))
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
            printf("PsychOpenXRCore-ERROR: Locating Xrviews via xrLocateViews() for XrTime %ld failed: %s\n", predictionTime, errorString);

        return(FALSE);
    }
    else if (verbosity > 4)
        printf("PsychOpenXRCore-DEBUG: Locating Xrviews via xrLocateViews() for XrTime %ld success.\n", predictionTime);

    return(TRUE);
}

static psych_bool syncXRActions(PsychOpenXRDevice* openxr)
{
    XrActiveActionSet activeActionSet = {
        .actionSet = openxr__actionSet,
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

                if (verbosity > 3)
                    printf("PsychOpenXRCore-INFO: Event polling for instance %p, interaction profile changed for session %p! Active controllers now: %i\n",
                           pollInstance, profileChangeEvent->session, getActiveControllers(pollInstance, openxr));

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

                            PsychLockMutex(&(openxr->presenterLock));
                            if (!resultOK(xrBeginSession(openxr->hmd, &sessionBeginInfo))) {
                                // Failed - Error return:
                                if (verbosity > 0)
                                    printf("PsychOpenXRCore-ERROR: Failed to xrBeginSession for session %p of instance %p: %s\n", openxr->hmd, pollInstance, errorString);

                                // Failure return code:
                                PsychUnlockMutex(&(openxr->presenterLock));
                                return(FALSE);
                            }

                            // Success. Session is now active, frame presentation loop needs to run to
                            // synchronize and keep synchronized with the runtime:
                            openxr->sessionActive = TRUE;
                            openxr->needFrameLoop = TRUE;

                            PsychUnlockMutex(&(openxr->presenterLock));

                            // PresentExecute() the first time on the main thread to sync us up:
                            if (!isMultithreaded(openxr))
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
                            psych_bool thread_was_active = isMultithreaded(openxr);
                            PsychOpenXRStopPresenterThread(openxr);

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

                            if (thread_was_active)
                                PsychOpenXRStartPresenterThread(openxr);
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
    psych_bool has_XR_KHR_convert_timespec_time = FALSE;

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
        printf("PsychOpenXRCore-INFO: At startup there are %i OpenXR instance extensions available:\n", instanceExtensionsCount);

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

    if (verbosity >= 4) {
        for (i = 0; i < instanceExtensionsCount; i++)
            printf("PsychOpenXRCore-INFO: %i. instance extension '%s'.\n", i, instanceExtensions[i].extensionName);

        printf("\n");
    }

    // Check if minimum set of required instance extensions exist and enable/request them, if so:
    if (!addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_KHR_OPENGL_ENABLE_EXTENSION_NAME) ||
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_EXT_DEBUG_UTILS_EXTENSION_NAME) ||

        #if defined(XR_USE_PLATFORM_WIN32)
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME_EXTENSION_NAME)
        #else
        FALSE
        #endif
    ) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: At least one required instance extension is missing!\n");

        free(instanceExtensions);
        goto instance_init_out;
    }

    // On Linux at least with Valve's SteamVR OpenXR runtime we must do without XR_KHR_convert_timespec_time, so make this optional and have fallbacks:
    #if defined(XR_USE_PLATFORM_XLIB)
    has_XR_KHR_convert_timespec_time = addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_KHR_CONVERT_TIMESPEC_TIME_EXTENSION_NAME);
    #endif

    // The following extensions are optional. Therefore we don't care if adding them succeeds or not:
    addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME);
    has_XR_FB_display_refresh_rate = addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_FB_DISPLAY_REFRESH_RATE_EXTENSION_NAME);

    // Enable additional optional controller input extensions:
    has_XR_EXT_hp_mixed_reality_controller = addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_EXT_HP_MIXED_REALITY_CONTROLLER_EXTENSION_NAME);
    has_XR_HTC_vive_cosmos_controller_interaction = addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_HTC_VIVE_COSMOS_CONTROLLER_INTERACTION_EXTENSION_NAME);
    has_XR_HTC_vive_focus3_controller_interaction = addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_HTC_VIVE_FOCUS3_CONTROLLER_INTERACTION_EXTENSION_NAME);

    // Enable basic eye tracking extension:
    has_XR_EXT_eye_gaze_interaction = addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_EXT_EYE_GAZE_INTERACTION_EXTENSION_NAME);

    // XR_KHR_binding_modification supported? If so, try to enable extensions which depend on it:
    if (addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_KHR_BINDING_MODIFICATION_EXTENSION_NAME)) {
        has_XR_EXT_dpad_binding = addInstanceExtension(instanceExtensions, instanceExtensionsCount, XR_EXT_DPAD_BINDING_EXTENSION_NAME);
    }

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
        // applicationName must not contain / character, or the SteamVR runtime will screw up controller input, therefore we
        // can not use the (more appropriate) PSYCHTOOLBOX_SCRIPTING_LANGUAGE_NAME. Thanks Valve!
        .applicationName = "Psychtoolbox",
        .applicationVersion = 3,
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
        if (has_XR_KHR_convert_timespec_time) {
            GET_INSTANCE_PROC_ADDR(xrInstance, xrConvertTimespecTimeToTimeKHR);
            GET_INSTANCE_PROC_ADDR(xrInstance, xrConvertTimeToTimespecTimeKHR);
        }
        else {
            if (verbosity >= 3)
                printf("PsychOpenXRCore-INFO: No support for XR_KHR_convert_timespec_time on device %i. Working around it.\n", numAvailableDevices);

            pxrConvertTimespecTimeToTimeKHR = NULL;
            pxrConvertTimeToTimespecTimeKHR = NULL;
        }
    #endif

    // Perform initial XR hardware enumeration:
    enumerateXRDevices(xrInstance);

    if (verbosity >= 3)
        printf("PsychOpenXRCore-INFO: At startup there are %i OpenXR devices available.\n", numAvailableDevices);

    // Create default setup for all supported input/output devices, e.g., touch input controllers for both hands,
    // gamepads, treadmills, HMD buttons, remotes, etc.:
    if (!createDefaultXRInputConfig(xrInstance)) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to setup default input/output device configuration.\n");

        PsychErrorExitMsg(PsychError_system, "OpenXR instance creation failed when trying to setup input/output device configuration.");
    }

    // Success return:
    return;

    instance_init_out:

    instanceExtensionsEnabledCount = 0;

    if (!dontfail) {
        printf("PsychOpenXRCore-ERROR: OpenXR runtime initialization failed: %s\n", errorString);
        PsychErrorExitMsg(PsychError_system, "PsychOpenXRCore-ERROR: Initialization of XR runtime failed. Driver disabled!");
    }
}

// Force exit of the Matlab/Octave/Python/... host application process, skipping all further destructors etc.:
static void PsychOpenXRDoExit(void)
{
    fprintf(stderr, "PsychOpenXRCore(): Early _exit(0) process termination to work around SteamVR shutdown bug NOW!\n");

    // Flush all output streams manually:
    fflush(NULL);

    // Return status code 0 == Success, do exit() without calling all the atexit() handlers etc.:
    _exit(0);
}

void PsychOpenXRClose(int handle)
{
    int rc, eyeIndex;
    static psych_bool hasHadSession = FALSE;
    PsychOpenXRDevice* openxr = PsychGetXR(handle, TRUE);

    if (openxr) {
        // Stop potentially running multi-threaded presentation mode, if active:
        PsychOpenXRStopPresenterThread(openxr);

        if (openxr->hmd) {
            // Release image to swapchain before final PresentExecute():
            for (eyeIndex = 0; eyeIndex < ((openxr->isStereo) ? 2 : 1); eyeIndex++) {
                if (!resultOK(xrReleaseSwapchainImage(openxr->textureSwapChain[eyeIndex], NULL))) {
                    if (verbosity > 0)
                        printf("PsychOpenXRCore-ERROR: Failed to release current swapchain image for eye %i: %s\n", eyeIndex, errorString);
                }
            }

            // PresentExecute() a last time on the main thread:
            openxr->frameState.shouldRender = FALSE;
            openxr->targetPresentTime = PsychGetAdjustedPrecisionTimerSeconds(NULL) - 10;
            PresentExecute(openxr, FALSE);

            #if PSYCH_SYSTEM == PSYCH_LINUX
            // Shutdown our Monado proprietary timestamping, if it is active:
            if (openxr->pbfile) {
                // Close fifo file for Monado metrics timestamping:
                fclose(openxr->pbfile);
                openxr->pbfile = NULL;

                if (verbosity > 4)
                    printf("PsychOpenXRCore-DEBUG: Shutdown Monado metrics timestamping.\n");
            }
            #endif

            if (verbosity > 4)
                printf("PsychOpenXRCore-DEBUG: xrRequestExitSession().\n");

            if (openxr->sessionActive && !resultOK(xrRequestExitSession(openxr->hmd))) {
                if (verbosity > 0)
                    printf("PsychOpenXRCore-ERROR: Failed to xrRequestExitSession(): %s\n", errorString);
            }

            if (verbosity > 4)
                printf("PsychOpenXRCore-DEBUG: Waiting for session shutdown to complete.\n");

            while (!openxr->userExit && !openxr->lossPending && openxr->sessionActive) {
                if (!processXREvents(xrInstance) && (verbosity > 0))
                    printf("PsychOpenXRCore-ERROR:Close: Failed to poll events II!\n");
            }

            if (verbosity > 4)
                printf("PsychOpenXRCore-DEBUG: Session shutdown completed.\n");

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
            xrDestroySession(openxr->hmd);
            hasHadSession = TRUE;
        }

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

        if (verbosity >= 4) printf("PsychOpenXRCore-INFO: Closed OpenXR device with handle %i.\n", handle);

        // Done with this device:
        devicecount--;
    }

    if (initialized && (devicecount == 0) && (xrInstance != XR_NULL_HANDLE)) {
        // Last HMD closed. Shutdown the runtime:

        // Only call xrDestroyInstance() if the OpenXR runtime is not Linux + SteamVR's OpenXR, because
        // otherwise we hang until killed in xrDestroyInstance() due to a SteamVR bug present since at
        // least January 2021 and unfixed by Valve as of December 2022 - Strong work!
        // See: https://github.com/ValveSoftware/SteamVR-for-Linux/issues/422 for the bug report, and
        // https://github.com/cmbruns/pyopenxr/pull/60 for a similar workaround in pyopenxr:
        if ((PSYCH_SYSTEM != PSYCH_LINUX) || !hasHadSession || strcmp(instanceProperties.runtimeName, "SteamVR/OpenXR")) {
            // Sane runtime: Perform full shutdown:
            if (debugMessenger)
                pxrDestroyDebugUtilsMessengerEXT(debugMessenger);

            debugMessenger = XR_NULL_HANDLE;

            xrDestroyInstance(xrInstance);
            xrInstance = XR_NULL_HANDLE;

            instanceExtensionsEnabledCount = 0;
            initialized = FALSE;

            if (verbosity >= 4)
                printf("PsychOpenXRCore-INFO: OpenXR runtime shutdown complete.\n");
        }
        else {
            // Buggy runtime: Keep xrInstance and associated state alive "as is":
            if (!calledFromPsychOpenXRCoreShutDown) {
                // First call, ie. first regular work session close-down/driver shutdown/"clear" attempt:
                if (verbosity >= 2) {
                    printf("PsychOpenXRCore-WARNING: Skipping driver shutdown, to work around xrDestroyInstance() \"hang-bug\" in proprietary SteamVR OpenXR runtime.\n");
                    printf("PsychOpenXRCore-WARNING: See https://github.com/ValveSoftware/SteamVR-for-Linux/issues/422 for associated unresolved SteamVR issue, as of v1.24.7.\n");
                    printf("PsychOpenXRCore-WARNING: " PSYCHTOOLBOX_SCRIPTING_LANGUAGE_NAME " might malfunction when you quit it, although usually it is fine.\n");
                }

                #if PSYCH_LANGUAGE == PSYCH_MATLAB
                // Lock ourselves into host process memory:
                mexLock();
                #endif
            }
            else {
                // Successive shutdown call during force-unload of driver during host application shutdown/exit.
                // Register handler to force exit the host application process early during exit(), skipping all
                // further destructors and - most importantly - further atexit() or similar registered cleanup
                // functions - one of which is located in the SteamVR runtime and would cause a hard hang:
                atexit(PsychOpenXRDoExit);
            }
        }
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
        memset(&eyeGazeAvailable[handle], 0, sizeof(eyeGazeAvailable[0]));
        eyeGazeAvailable[handle].type = XR_TYPE_SYSTEM_EYE_GAZE_INTERACTION_PROPERTIES_EXT;
        memset(&availableSystems[handle], 0, sizeof(availableSystems[0]));
        availableSystems[handle].type = XR_TYPE_SYSTEM_PROPERTIES;
    }

    calledFromPsychOpenXRCoreShutDown = FALSE;
    numAvailableDevices = 0;
    devicecount = 0;
    initialized = FALSE;
}

PsychError PsychOpenXRCoreShutDown(void) {
    int handle;

    // Mark us as exiting:
    calledFromPsychOpenXRCoreShutDown = TRUE;

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
    static char useString[] = "numDevices = PsychOpenXRCore('GetCount');";
    static char synopsisString[] =  "Returns count of currently connected XR devices.\n"
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

PsychError PSYCHOPENXROpen(void)
{
    static char useString[] = "[openxrPtr, modelName, runtimeName, hasEyeTracking] = PsychOpenXRCore('Open' [, deviceIndex=0]);";
    //                          1          2          3            4                                           1
    static char synopsisString[] =
        "Open connection to OpenXR device, return a 'openxrPtr' handle to it.\n\n"
        "The call tries to open the device with index 'deviceIndex', or the first detected "
        "device if 'deviceIndex' is omitted. Please note that currently only one single device "
        "is supported by OpenXR-1, so this 'deviceIndex' is redundant at the moment, given "
        "that zero is the only valid value.\n"
        "The returned handle can be passed to the other subfunctions to operate the device.\n"
        "'modelName' returns the model name string of the OpenXR device.\n"
        "'runtimeName' returns the name of the OpenXR runtime.\n"
        "'hasEyeTracking' returns the level of eye tracking support: 0 = None, 1 = Basic.\n";
    static char seeAlsoString[] = "GetCount Close";

    PsychOpenXRDevice* openxr;
    int deviceIndex = 0;
    int handle = 0;
    int rc;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(4));
    PsychErrorExit(PsychCapNumInputArgs(1));

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

    if (-1 == enumerateXRDevices(xrInstance)) {
        if (verbosity >= 2)
            printf("PsychOpenXRCore-WARNING: Could not connect to OpenXR server process yet. Did you forget to start it?\n");
    }

    if (deviceIndex >= numAvailableDevices) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Invalid deviceIndex %i >= number of available devices %i.\n", deviceIndex, numAvailableDevices);

        PsychErrorExitMsg(PsychError_user, "Invalid 'deviceIndex' provided. Not enough devices available!");
    }

    // Zero init device structure:
    memset(&openxrdevices[handle], 0, sizeof(PsychOpenXRDevice));

    // Get device pointer:
    openxr = &openxrdevices[handle];

    // Try to create and setup a xrSession for the xrSystem device with 'deviceIndex':
    openxr->systemId = availableSystems[deviceIndex].systemId;

    // Record if basic eye gaze tracking is available:
    openxr->hasEyeTracking = eyeGazeAvailable[deviceIndex].supportsEyeGazeInteraction ? 1 : 0;

    // Use a fixed stereo view type by default for now, for typical HMD use:
    openxr->viewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;

    // Stats for nerds:
    if (verbosity >= 3) {
        printf("PsychOpenXRCore-INFO: Opened OpenXR device with deviceIndex %i as handle %i.\n", deviceIndex, handle + 1);
        printf("PsychOpenXRCore-INFO: Product: \"%s\" - [VendorId: 0x%x eyeTracking: %i]\n", availableSystems[deviceIndex].systemName, availableSystems[deviceIndex].vendorId, openxr->hasEyeTracking);
        printf("PsychOpenXRCore-INFO: ----------------------------------------------------------------------------------\n");
    }

    // Assign multi-threading mode as "off" by default:
    openxr->multiThreaded = 0;

    // Initialize the mutex lock:
    if ((rc = PsychInitMutex(&(openxr->presenterLock)))) {
        printf("PsychOpenXRCore-ERROR: Could not create internal presenterLock mutex lock [%s].\n", strerror(rc));
        PsychErrorExitMsg(PsychError_system, "Insufficient system resources for mutex creation as part of device open operation!");
    }

    if ((rc = PsychInitCondition(&(openxr->presentedSignal), NULL))) {
        printf("PsychOpenXRCore-ERROR: Could not create internal presentedSignal condition variable [%s].\n", strerror(rc));
        PsychErrorExitMsg(PsychError_system, "Insufficient system ressources for condition variable creation as part of device open operation!");
    }

    // Mark device as open:
    openxr->opened = TRUE;

    // Session state unknown/undefined for not yet created session:
    openxr->state = XR_SESSION_STATE_UNKNOWN;

    // Assign user script handle:
    openxr->handle = handle + 1;

    // Increment count of open devices:
    devicecount++;

    // Return device handle: We use 1-based handle indexing to make life easier for Octave/Matlab:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, handle + 1);

    // Return product name:
    PsychCopyOutCharArg(2, kPsychArgOptional, (const char*) availableSystems[deviceIndex].systemName);

    // Return OpenXR runtime name:
    PsychCopyOutCharArg(3, kPsychArgOptional, (const char*) instanceProperties.runtimeName);

    // Return eye tracking support level:
    PsychCopyOutDoubleArg(4, kPsychArgOptional, openxr->hasEyeTracking);

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

PsychError PSYCHOPENXRReferenceSpaceType(void)
{
    static char useString[] = "[oldType, spaceSize] = PsychOpenXRCore('ReferenceSpaceType', openxrPtr [, newType]);";
    //                          1        2                                                  1            2
    static char synopsisString[] =
        "Specify the type of reference space for OpenXR device 'openxrPtr'.\n"
        "This returns the current type of the current reference space in 'oldType'.\n"
        "Optionally you can specify a new reference space type as 'newType'. "
        "The number is one of the reference space types defined by OpenXR or one of "
        "its extensions. See the section \"Reference Spaces\" in the OpenXR spec.\n"
        "Common types are as follows, note that only 1 and 2 are supported by all runtimes:\n"
        "1 = Origin is at eye height and head-locked == XR_REFERENCE_SPACE_TYPE_VIEW.\n"
        "2 = Origin is at runtime defined local tracking space origin == XR_REFERENCE_SPACE_TYPE_LOCAL.\n"
        "3 = Origin is at floor height of a flat rectangular stage tracking space == XR_REFERENCE_SPACE_TYPE_STAGE.\n"
        "1000038000 = Unbounded space for large tracking areas == XR_REFERENCE_SPACE_TYPE_UNBOUNDED_MSFT.\n"
        "The origin of type 2 and 3 gets defined by the system during OpenXR runtime specific world "
        "and sensor calibration, e.g., in some setup GUI application or configuration file, or other procedure.\n"
        "\n"
        "'spaceSize' is a [width, height] vector with the width and height (in meters) of the reference spaces "
        "walking area, ie. a rectangle whose sides are aligned with the x and y axis, the origin of the space "
        "being the center of the rectangle. The rectangle defines an area that is clear of obstacles and tracked, "
        "therefore meaningfully and safely walkable by the subject during the XR session. If this information "
        "is not available then returns an empty [] 'spaceSize'.\n";
    static char seeAlsoString[] = "GetTrackingState GetInputState";

    int handle;
    int referenceSpaceType;
    double *size;
    PsychOpenXRDevice *openxr;
    XrExtent2Df bounds;
    XrResult res;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    if (!openxr->hmd)
        PsychErrorExitMsg(PsychError_user, "Setting new reference space failed, as session is not up yet! Create a OpenXR session first via 'CreateAndStartSession'.");

    // Query and return current/old setting:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) openxr->worldSpaceType);

    // New origin type provided?
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &referenceSpaceType)) {
        XrSpace oldRefSpace = openxr->worldSpace;
        XrReferenceSpaceCreateInfo refSpaceCreateInfo = {
            .type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO,
            .next = NULL,
            .referenceSpaceType = referenceSpaceType,
            .poseInReferenceSpace = identityPose
        };

        // Lock out potential presenter thread:
        PsychLockMutex(&(openxr->presenterLock));

        // Create, initialize and assign our new standard space for defining the world reference frame:
        res = xrCreateReferenceSpace(openxr->hmd, &refSpaceCreateInfo, &openxr->worldSpace);
        if (!resultOK(res)) {
            // Failed! Restore old/previous reference space:
            openxr->worldSpace = oldRefSpace;

            PsychUnlockMutex(&(openxr->presenterLock));

            if (verbosity > 0) {
                if (res == XR_ERROR_REFERENCE_SPACE_UNSUPPORTED)
                    printf("PsychOpenXRCore-ERROR:ReferenceSpaceType(): Requested reference space type %i unsupported by OpenXR runtime.\n",
                           refSpaceCreateInfo.referenceSpaceType);
                else
                    printf("PsychOpenXRCore-ERROR:ReferenceSpaceType(): Creating new reference space failed: %s\n", errorString);
            }

            PsychErrorExitMsg(PsychError_user, "Setting new reference space (== OpenXR reference space type) failed.");
        }
        else {
            // Success: Free old/previous reference space:
            xrDestroySpace(oldRefSpace);

            // Record new type for active space:
            openxr->worldSpaceType = refSpaceCreateInfo.referenceSpaceType;

            // Assign to 3D projectionLayer:
            openxr->projectionLayer.space = openxr->worldSpace;

            if (verbosity > 3)
                printf("PsychOpenXRCore-INFO: Set new reference space type for device to %i.\n", refSpaceCreateInfo.referenceSpaceType);
        }

        PsychUnlockMutex(&(openxr->presenterLock));
    }

    // Retrieve bounding rect of reference space:
    res = xrGetReferenceSpaceBoundsRect(openxr->hmd, openxr->worldSpaceType, &bounds);
    if (!resultOK(res)) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR:ReferenceSpaceType(): Retrieving bounds of current reference space failed: %s\n", errorString);

        PsychErrorExitMsg(PsychError_system, "Retrieving bounds of current reference space failed.");
    }

    // If bounds are available, return them, otherwise return an empty [] rect:
    if (res != XR_SPACE_BOUNDS_UNAVAILABLE) {
        // Return width and height of reference space clear and tracked ground rectangle:
        PsychAllocOutDoubleMatArg(2, kPsychArgOptional, 1, 2, 1, &size);
        size[0] = bounds.width;
        size[1] = bounds.height;
    }
    else {
        PsychAllocOutDoubleMatArg(2, kPsychArgOptional, 0, 0, 0, &size);
    }

    return(PsychError_none);
}

PsychError PSYCHOPENXRView2DParameters(void)
{
    static char useString[] = "[oldPosition, oldSize, oldOrientation] = PsychOpenXRCore('View2DParameters', openxrPtr, eye [, position][, size][, orientation]);";
    //                          1            2        3                                                     1          2      3           4       5
    static char synopsisString[] =
    "Query or assign 2D quad view parameters for eye 'eye' of OpenXR device 'openxrPtr'.\n\n"
    "Such 2D quad views are used in 'Monoscopic' (same view for both eyes), or 'Stereoscopic' "
    "mode (one view per eye), as well as in 3D modes when a script is 'Stop'ed and the user "
    "asked for use of these 2D quad views instead of projective views.\n"
    "This returns the current or previous settings for position and size in 'oldPosition' "
    "and 'oldSize', and for orientation in 'oldOrientation' as a quaternion.\n"
    "'eye' Mandatory: 0 = Left eye or monoscopic view, 1 = right eye in stereo mode.\n"
    "Optionally you can specify new settings, as follows:\n"
    "'position' 3D position of the center of the virtual viewscreen, relative to the eye "
    "of the subject. Unit is meters, e.g., [0, 0, -0.5] would center the view at x,y offset "
    "zero relative to the optical axis, and 0.5 meters away from the eye. Iow. the center of "
    "the viewscreen aligns with the straightforward looking direction of the eye, but the "
    "screen floats at 0.5 meters distance. If this parameter is left empty [] or omitted, then "
    "the position does not change. Default position at session startup is centered and at a "
    "comfortable viewing distance away, so staring straight forward with parallel eyes, e.g., "
    "like when looking at an infinite point in space, would cause the center of the stimulus "
    "image to be located at your fixation direction.\n"
    "'size' Size of the virtual viewscreen in meters. E.g., [0.8, 1] would have the screen "
    "at an apparent width of 0.8 meters and an apparent height of 1 meter. If the parameter "
    "is omitted or left empty [], the size won't be changed. Default size is 1 meter high "
    "and the width adjusted to preserve the aspect ratio of the Psychtoolbox onscreen window "
    "into which your script draws, so a drawn circle is actually circular instead of elliptic.\n"
    "'orientation' A 4 component vector encoding a quaternion for orientation in space, ie. "
    "a [rx, ry, rz, rw] vector. Or a scalar angle in degrees for rotation around z-axis, e.g., "
    "23 for a 23 degrees rotation around the optical axis / line of sight of the viewer.\n";
    static char seeAlsoString[] = "ViewType";

    int handle;
    int eyeIndex;
    int m, n, p;
    double *position;
    double *orientation;
    double *size;
    double *outM;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(5));
    PsychErrorExit(PsychRequireNumInputArgs(2));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get eye index:
    PsychCopyInIntegerArg(2, kPsychArgRequired, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1)
        PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    // Query and return old settings:
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, 3, 1, &outM);
    outM[0] = openxr->quadViewLayer[eyeIndex].pose.position.x;
    outM[1] = openxr->quadViewLayer[eyeIndex].pose.position.y;
    outM[2] = openxr->quadViewLayer[eyeIndex].pose.position.z;

    PsychAllocOutDoubleMatArg(2, kPsychArgOptional, 1, 2, 1, &outM);
    outM[0] = openxr->quadViewLayer[eyeIndex].size.width;
    outM[1] = openxr->quadViewLayer[eyeIndex].size.height;

    PsychAllocOutDoubleMatArg(3, kPsychArgOptional, 1, 4, 1, &outM);
    outM[0] = openxr->quadViewLayer[eyeIndex].pose.orientation.x;
    outM[1] = openxr->quadViewLayer[eyeIndex].pose.orientation.y;
    outM[2] = openxr->quadViewLayer[eyeIndex].pose.orientation.z;
    outM[3] = openxr->quadViewLayer[eyeIndex].pose.orientation.w;

    // New position provided?
    if (PsychAllocInDoubleMatArg(3, kPsychArgOptional, &m, &n, &p, &position)) {
        if (m*n*p != 3)
            PsychErrorExitMsg(PsychError_user, "Invalid 'position' specified. Must be a [x, y, z] vector.");

        if (position[2] >= 0.0)
            PsychErrorExitMsg(PsychError_user, "Invalid 'position' specified. z-Position must be smaller than zero.");

        // Assign:
        PsychLockMutex(&(openxr->presenterLock));
        openxr->quadViewLayer[eyeIndex].pose.position.x = (float) position[0];
        openxr->quadViewLayer[eyeIndex].pose.position.y = (float) position[1];
        openxr->quadViewLayer[eyeIndex].pose.position.z = (float) position[2];
        PsychUnlockMutex(&(openxr->presenterLock));
    }

    // New size provided?
    if (PsychAllocInDoubleMatArg(4, kPsychArgOptional, &m, &n, &p, &size)) {
        if (m*n*p != 2)
            PsychErrorExitMsg(PsychError_user, "Invalid 'size' specified. Must be a [width, height] vector.");

        if ((size[0] <= 0.0) || (size[1] <= 0.0))
            PsychErrorExitMsg(PsychError_user, "Invalid 'size' specified. Both width and height must be greater than zero.");

        // Assign:
        PsychLockMutex(&(openxr->presenterLock));
        openxr->quadViewLayer[eyeIndex].size.width = (float) size[0];
        openxr->quadViewLayer[eyeIndex].size.height = (float) size[1];
        PsychUnlockMutex(&(openxr->presenterLock));
    }

    // New orientation quaternion provided?
    if (PsychAllocInDoubleMatArg(5, kPsychArgOptional, &m, &n, &p, &orientation)) {
        if (m*n*p != 4 && m*n*p != 1)
            PsychErrorExitMsg(PsychError_user, "Invalid 'orientation' specified. Must be a [rx, ry, rz, rw] quaternion vector or scalar rotation angle.");

        // Assign:
        PsychLockMutex(&(openxr->presenterLock));
        if (m*n*p != 1) {
            // Quaternion:
            openxr->quadViewLayer[eyeIndex].pose.orientation.x = (float) orientation[0];
            openxr->quadViewLayer[eyeIndex].pose.orientation.y = (float) orientation[1];
            openxr->quadViewLayer[eyeIndex].pose.orientation.z = (float) orientation[2];
            openxr->quadViewLayer[eyeIndex].pose.orientation.w = (float) orientation[3];
        }
        else {
            // Rotation around z-axis, ie. essentially optical axis or line of sight:
            double angle = orientation[0] * 3.1415926535 / 180.0;
            openxr->quadViewLayer[eyeIndex].pose.orientation.x = 0;
            openxr->quadViewLayer[eyeIndex].pose.orientation.y = 0;
            openxr->quadViewLayer[eyeIndex].pose.orientation.z = (float) sin(angle / 2.0);
            openxr->quadViewLayer[eyeIndex].pose.orientation.w = (float) cos(angle / 2.0);
        }
        PsychUnlockMutex(&(openxr->presenterLock));
    }

    return(PsychError_none);
}

PsychError PSYCHOPENXRViewType(void)
{
    static char useString[] = "oldType = PsychOpenXRCore('ViewType', openxrPtr [, viewLayerType]);";
    //                         1                                     1            2
    static char synopsisString[] =
    "Specify the type of view for OpenXR device 'openxrPtr'.\n"
    "This returns the current type of view in 'oldType'. "
    "Optionally you can specify a new view type as 'viewLayerType'. "
    "viewLayerType must be either:\n"
    "0 = Use a 2D monoscopic or stereoscopic view via OpenXR quad layers, "
    "which supposedly stays stable for not actively tracked or slow "
    "updating stimulus display on OpenXR conformant runtimes. Placing "
    "these views properly for binocular / stereoscopic display and "
    "perspective correct 3D rendering is your scripts responsibility. "
    "Cfe. PsychOpenXRCore('View2DParameters') for setting these to non-"
    "default values.\n"
    "1 = Use a 3D perspective projected view via OpenXR projection layers. "
    "These are auto-configured for good perspective correct 3D rendering, "
    "but need an active track -> render -> present loop on some OpenXR runtimes "
    "to work correctly, otherwise judder and jerks will occur in the display. "
    "PsychOpenXRCore('GetTrackingState') drives this manually, usually called "
    "via PsychVRHMD('PrepareRender').\n"
    "\n";
    static char seeAlsoString[] = "View2DParameters Start Stop GetTrackingState NeedLocateForProjectionLayers";

    int handle;
    int viewLayerType;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Query and return old setting:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) openxr->viewLayerType);

    // New origin type provided?
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &viewLayerType)) {
        if (viewLayerType < 0 || viewLayerType > 1)
            PsychErrorExitMsg(PsychError_user, "Invalid 'viewLayerType' for specified. Must be 0 or 1.");

        PsychLockMutex(&(openxr->presenterLock));
        PsychAssignSubmitLayers(openxr, viewLayerType);
        PsychUnlockMutex(&(openxr->presenterLock));
    }

    return(PsychError_none);
}

PsychError PSYCHOPENXRNeedLocateForProjectionLayers(void)
{
    static char useString[] = "oldNeed = PsychOpenXRCore('NeedLocateForProjectionLayers', openxrPtr [, needLocate]);";
    //                         1                                                          1            2
    static char synopsisString[] =
    "Specify if projection layers need to be continuously updated by tracking for OpenXR device 'openxrPtr'.\n"
    "This returns the current setting in 'oldNeed'. "
    "Optionally you can specify a new setting as 'needLocate'. "
    "needLocate must be either:\n"
    "0 = Positioning of projection layers stays stable and fixed without a "
    "constantly running tracking loop on high quality runtimes.\n"
    "1 = Projection layers need a constantly running loop, or jitter, jerks, "
    "wrong positioning or timeout warnings will happen. Note that if this "
    "setting is needed, then also multi-threaded presentation must be active "
    "whenever the user script does not engage in fast active tracking via "
    "PsychOpenXRCore('GetTrackingState') calls, usually called implicitely "
    "via PsychVRHMD('PrepareRender').\n"
    "\n";
    static char seeAlsoString[] = "Start Stop GetTrackingState PresenterThreadEnable";

    int handle;
    int needLocate;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Query and return old setting:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) openxr->needLocate);

    // New origin type provided?
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &needLocate)) {
        if (needLocate < 0 || needLocate > 1)
            PsychErrorExitMsg(PsychError_user, "Invalid 'needLocate' for specified. Must be 0 or 1.");

        PsychLockMutex(&(openxr->presenterLock));
        openxr->needLocate = needLocate;
        PsychUnlockMutex(&(openxr->presenterLock));
    }

    return(PsychError_none);
}

PsychError PSYCHOPENXRPresenterThreadEnable(void)
{
    static char useString[] = "oldEnable = PsychOpenXRCore('PresenterThreadEnable', openxrPtr [, enableThread]);";
    //                         1                                                    1            2
    static char synopsisString[] =
    "Enable or disable asynchronous background presenter thread for OpenXR device 'openxrPtr'.\n"
    "The thread is required on some OpenXR runtimes in some situations to provide stable "
    "visual stimulation if a user script doesn't run an active tracking -> render -> present loop, "
    "or runs the loop too slow / with pauses. It may also be needed for timed stimulus presentation, "
    "or for half-way reasonable visual stimulus onset timestamping. This is all highly dependent "
    "on the OpenXR runtime in use. If possible, the thread should be avoided for better performance.\n"
    "This returns the current setting in 'oldEnable'. "
    "Optionally you can specify a new setting as 'enableThread'. enableThread must be either:\n"
    "0 = Disable thread, all operations are done synchronously, driven by the user script.\n"
    "1 = Enable thread for driving the visual updating loop, independent of user script.\n"
    "\n";
    static char seeAlsoString[] = "Start Stop GetTrackingState NeedLocateForProjectionLayers";

    int handle;
    int enableThread;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Query and return old setting:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) (openxr->presenterThread != (psych_thread) NULL));

    // New origin type provided?
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &enableThread)) {
        if (enableThread < 0 || enableThread > 1)
            PsychErrorExitMsg(PsychError_user, "Invalid 'enableThread' for specified. Must be 0 or 1.");

        if (enableThread)
            PsychOpenXRStartPresenterThread(openxr);
        else
            PsychOpenXRStopPresenterThread(openxr);
    }

    return(PsychError_none);
}

PsychError PSYCHOPENXRStart(void)
{
    static char useString[] = "PsychOpenXRCore('Start', openxrPtr);";
    static char synopsisString[] = "Start user-script driven head orientation and position tracking operation on OpenXR device 'openxrPtr'.\n\n"
                                   "The driver assumes that the user-script runs a tight/fast animation loop with appropriate calls to the "
                                   "head tracking functions, to drive driver internal tracking state updates and inform the rendering. "
                                   "Too slowly running user-script animation loops, stopped animation loops or loops avoiding the needed "
                                   "calls to the 'GetTrackingState', ie., PsychVRHMD('PrepareRender'), function, may cause visual artifacts "
                                   "in the device's presented imagery, e.g., judder and jerks.";
    static char seeAlsoString[] = "Stop GetTrackingState";

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
        PsychErrorExitMsg(PsychError_user, "Tried to start tracking on device, but tracking already active.");
    }

    if (verbosity >= 4)
        printf("PsychOpenXRCore-INFO: Tracking started on device with handle %i.\n", handle);

    // Mark tracking as started: Mutex protected, because a thread might be running:
    PsychLockMutex(&(openxr->presenterLock));
    openxr->isTracking = TRUE;
    PsychUnlockMutex(&(openxr->presenterLock));

    // Tracking is running.
    return(PsychError_none);
}

PsychError PSYCHOPENXRStop(void)
{
    static char useString[] = "PsychOpenXRCore('Stop', openxrPtr);";
    static char synopsisString[] = "Stop user-script driven head orientation and position tracking operation on OpenXR device 'openxrPtr'.\n\n"
                                   "This signals that the user-scripts animation loop is (about to be) stopped, will run at low framerate, "
                                   "and/or avoid calls to 'GetTrackingState', ie., PsychVRHMD('PrepareRender'). This means the script will "
                                   "not take care of the processing needed to keep tracked 3D content displaying properly. The driver will "
                                   "try to take care of needed steps itself to provide a stable picture in the device display in this case, "
                                   "if possible, given the used hardware + OpenXR and operating system software setup. Results may vary...\n";
    static char seeAlsoString[] = "Start GetTrackingState";

    int handle;
    PsychOpenXRDevice* openxr;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, TRUE);

    // No-Op return if already stopped:
    if (!openxr->isTracking)
        return(PsychError_none);

    // Mark tracking as stopped: Mutex protected, because a thread might already be running:
    PsychLockMutex(&(openxr->presenterLock));
    openxr->isTracking = FALSE;
    PsychUnlockMutex(&(openxr->presenterLock));

    if (verbosity >= 4)
        printf("PsychOpenXRCore-INFO: Tracking stopped on device with handle %i.\n", handle);

    return(PsychError_none);
}

PsychError PSYCHOPENXRGetTrackingState(void)
{
    static char useString[] = "[state, touch, gaze] = PsychOpenXRCore('GetTrackingState', openxrPtr [, predictionTime=nextFrame][, reqMask=all]);";
    //                          1      2      3                                           1            2                           3
    static char synopsisString[] =
        "Return current state of position and orientation tracking for OpenXR device 'openxrPtr'.\n"
        "Position and orientation is predicted for target time 'predictionTime' in seconds if provided, "
        "based on the latest measurements from the tracking hardware. If 'predictionTime' is omitted or zero, "
        "then the prediction is performed for the mid-point of the next possible video frame of the device, ie. "
        "the most likely presentation time for immediately rendered images.\n"
        "'reqMask' mask defining which information to return. Defaults to all information. Values are "
        "+1 for head/eye tracking, +2 for hand controller tracking, +4 for eye gaze tracking.\n\n"
        "'state' is a struct with fields reporting the following values:\n"
        "'Time' = Time in seconds of returned tracking state.\n"
        "'Status' = Tracking status flags:\n"
        "+1 = Orientation tracked for all eyes,\n"
        "+2 = Position tracked for all eyes,\n"
        "+4 = At least part of the pose is somewhat valid, even if not tracked, but just inferred,\n"
        "+128 = device display is connected, available and actually on users head, displaying our content.\n\n"
        "'SessionState' = VR session status flags, added together:\n"
        "+1  = Our rendering goes to the device, ie. we have control over it. If some other app would "
        "be in control, this flag would be missing.\n"
        "+2  = device is present and active.\n"
        "+4  = device is strapped onto users head. E.g., a Rift CV1 would switch off/blank if not on the head.\n"
        "+8  = DisplayLost condition! Some hardware/software malfunction, need to completely quit to recover.\n"
        "+16 = ShouldQuit The user interface asks us to voluntarily terminate this session.\n\n"
        "'CalibratedOrigin' = The pose of the world coordinate system origin during last recalibration, relative "
        "to its previous pose, as a vector [x,y,z,rx,ry,rz,rw].\n\n"
        "'EyePoseLeft' = Vector with position and orientation of left eye / left eye virtual camera.\n"
        "'EyePoseRight' = Vector with position and orientation of right eye / right eye virtual camera.\n"
        "The vectors are of form [tx, ty, tz, rx, ry, rz, rw] - A 3 component 3D position, followed by a 4 "
        "component rotation quaternion.\n\n"
        " \n"
        "Touch controller position and orientation:\n"
        " \n"
        "The return argument 'touch' is a struct array with 2 structs. touch(1) contains info about "
        "the tracking state and tracked pose of the left hand (= left touch controller) of the user, "
        "touch(2) contains info about the right hand (= right touch controller) of the user.\n"
        "The structs have very similar structure to the head (= device) tracking data returned by 'state':\n\n"
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
        " \n"
        "Eye gaze tracking on supported hardware:\n"
        " \n"
        "The return argument 'gaze' is a struct array with up to 2 structs. On a system with "
        "binocular eye tracking support, gaze(1) contains info about the tracking state and "
        "tracked gaze of the left eye of the user. On a system with only monocular eye tracking, "
        "it contains the only gaze information available, often the synthesized gaze of a \"cyclops eye\". "
        "On a system with binocular eye tracking, gaze(2) contains info about the gaze of the right eye.\n"
        "Each gaze struct contains the following fields:\n\n"
        "'Time' = Time in seconds of returned gaze tracking sample. This can be the actual acquisition "
        "time of the gaze sample closest in time to the requested 'predictionTime', but it can also be "
        "a time for which the users gaze location was interpolated or extrapolated, given 'predictionTime'. "
        "The behaviour wrt. sample acquisition time, returned vs. requested time, and gaze inter- / extrapolation "
        "is system dependent and may vary from device to device, runtime to runtime and across operating systems. "
        "If the gaze sample time can not be determined by the given system, a value of zero is returned.\n"
        "'Status' = Tracking status flags:\n"
        " 0 = No gaze tracking info available.\n"
        "+1 = Some gaze info available, but not based on measurements. Consider this not trustworthy at all!\n"
        "+2 = Tracked gaze position available. This is possibly subject to interpolation or extrapolation.\n"
        "'GazePose' = Position and orientation of the eye, expressing a gaze vector in its usual [x,y,z,rx,ry,rz,rw] form.\n"
        "\n";
    static char seeAlsoString[] = "Start Stop GetTrackersState GetInputState";

    PsychGenericScriptType *status;
    const char *FieldNames1[] = { "Time", "Status", "SessionState", "CalibratedOrigin", "EyePoseLeft", "EyePoseRight" };
    const int FieldCount1 = 6;
    const char *FieldNames2[] = { "Time", "Status", "HandPose", "HandLinearSpeed", "HandAngularSpeed" };
    const int FieldCount2 = 5;
    const char *FieldNames3[] = { "Time", "Status", "GazePose" };
    const int FieldCount3 = 3;

    int handle, i;
    PsychOpenXRDevice *openxr;
    XrTime xrPredictionTime;
    PsychGenericScriptType *outMat;
    double *v;
    double predictionTime = DBL_MAX;
    int StatusFlags = 0;
    int reqMask = 7;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(3));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Get optional predictionTime:
    PsychCopyInDoubleArg(2, kPsychArgOptional, &predictionTime);

    // Get optional info requirements mask:
    PsychCopyInIntegerArg(3, kPsychArgOptional, &reqMask);
    if (reqMask < 0 || reqMask > 7)
        PsychErrorExitMsg(PsychError_user, "Invalid 'reqMask' specified. Valid values are 0 to 7.");

    // Lock protect openxr->predictedDisplayTime and locateXRViews() and return of
    // all views[] info and viewState info below:
    PsychLockMutex(&(openxr->presenterLock));

    // Got optional target time for predicted tracking state? Default to the
    // predicted state for the predicted display time of the next frame:
    if (predictionTime != DBL_MAX) {
        xrPredictionTime = PsychTimeToXrTime(predictionTime);
    }
    else {
        xrPredictionTime = openxr->predictedDisplayTime;
        predictionTime = XrTimeToPsychTime(xrPredictionTime);
    }

    // Do the actual eye pose / view update from tracking + prediction:
    // Also updates openxr->viewState with its status flags.
    if (reqMask & 1) {
        locateXRViews(openxr, xrPredictionTime);

        // Print out tracking status:
        if (verbosity > 4)
            printf("PsychOpenXRCore-INFO: Tracking state predicted for device %i at time %f. Status = %i\n", handle, predictionTime, openxr->viewState.viewStateFlags);

        // TODO: All these functions for returning data to the userscript can fail, and would
        // leave our mutex lock in a desolate (== locked) state, screwing up error handling big
        // time. Also it would be better to not call them, as they do memory allocations, which
        // could take a tad more time than we want while the mutex is locked. In practice, failure
        // of the functions and long delays are unlikely, mostly only in case of massive out-of-memory
        // situations, in which we are screwed anyway. But yeah, it would be conceptually better to
        // drop the mutex already after locateXRViews -- which requires substantial refactoring though...

        PsychAllocOutStructArray(1, kPsychArgOptional, -1, FieldCount1, FieldNames1, &status);

        // Timestamp for when this prediction is valid:
        openxr->sensorSampleTime = predictionTime;
        PsychSetStructArrayDoubleElement("Time", 0, predictionTime, status);

        // device tracking status:
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

        // device present, connected and online and on users head, displaying us?
        if (openxr->state == XR_SESSION_STATE_VISIBLE || openxr->state == XR_SESSION_STATE_FOCUSED)
            StatusFlags |= 128;

        // Return head and general tracking status flags:
        PsychSetStructArrayDoubleElement("Status", 0, StatusFlags, status);

        // Return session status flags:
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
    }
    else {
        PsychAllocOutStructArray(1, kPsychArgOptional, -1, FieldCount1, FieldNames1, &status);

        // Timestamp for when this prediction is valid:
        openxr->sensorSampleTime = predictionTime;
        PsychSetStructArrayDoubleElement("Time", 0, predictionTime, status);

        // device tracking status:
        StatusFlags = 0;

        // device present, connected and online and on users head, displaying us?
        if (openxr->state == XR_SESSION_STATE_VISIBLE || openxr->state == XR_SESSION_STATE_FOCUSED)
            StatusFlags |= 128;

        // Return head and general tracking status flags:
        PsychSetStructArrayDoubleElement("Status", 0, StatusFlags, status);

        // Return session status flags:
        StatusFlags = 0;
        if (openxr->state == XR_SESSION_STATE_VISIBLE || openxr->state == XR_SESSION_STATE_FOCUSED) StatusFlags |= 1;
        if (openxr->needFrameLoop) StatusFlags |= (2 + 4);
        if (openxr->lossPending) StatusFlags |= 8;
        if (openxr->userExit) StatusFlags |= 16;

        PsychSetStructArrayDoubleElement("SessionState", 0, StatusFlags, status);
    }

    // We can unlock here, because only (HMD) display related state is shared with
    // a potentially running parallel presentation thread:
    PsychUnlockMutex(&(openxr->presenterLock));

    // Update tracking and input state from non-HMD controllers, eye trackers, etc., if such info is requested.
    // This requires a syncXRActions() call to update all input from all sources in close time-sync:
    if (reqMask > 1)
        syncXRActions(openxr);

    // Hand controllers tracking info requested?
    if (reqMask & 2) {
        // Get the tracking info from the OpenXR touch controllers 0 and 1 for left
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
    }
    else {
        // Allocate out an empty struct:
        PsychAllocOutStructArray(2, kPsychArgOptional, 2, FieldCount2, FieldNames2, &status);
    }

    // Eye gaze tracking requested?
    if ((reqMask & 4) && openxr->hasEyeTracking) {
        // Now the tracking info from the OpenXR eye gaze interaction extension:
        XrSpaceLocation gazeLocation = { 0 };
        XrEyeGazeSampleTimeEXT eyeGazeSampleTime = { 0 };
        XrActionStatePose gazeStatePose = { 0 };
        XrActionStateGetInfo getGazeStateInfo = { 0 };

        eyeGazeSampleTime.type = XR_TYPE_EYE_GAZE_SAMPLE_TIME_EXT;
        gazeLocation.type = XR_TYPE_SPACE_LOCATION;
        gazeLocation.next = &eyeGazeSampleTime;
        gazeLocation.locationFlags = 0;

        gazeStatePose.type = XR_TYPE_ACTION_STATE_POSE;
        getGazeStateInfo.type = XR_TYPE_ACTION_STATE_GET_INFO;
        getGazeStateInfo.action = openxr__gazePoseAction;

        if (!resultOK(xrGetActionStatePose(openxr->hmd, &getGazeStateInfo, &gazeStatePose))) {
            if (verbosity > 1)
                printf("PsychOpenXRCore-WARNING: In eye tracking - xrGetActionStatePose() failed: %s\n", errorString);
        }
        else if (gazeStatePose.isActive && !resultOK(xrLocateSpace(openxr->gazePoseSpace, openxr->worldSpace, xrPredictionTime, &gazeLocation))) {
            if (verbosity > 1)
                printf("PsychOpenXRCore-WARNING: In eye tracking - xrLocateSpace() failed: %s\n", errorString);
        }

        if (verbosity > 3)
            printf("PsychOpenXRCore-DEBUG: Eye gaze tracking - isActive %i : locationFlags %i : dT %li\n", gazeStatePose.isActive,
                   gazeLocation.locationFlags, xrPredictionTime - eyeGazeSampleTime.time);

        // Allocate single struct for return of data:
        PsychAllocOutStructArray(3, kPsychArgOptional, 1, FieldCount3, FieldNames3, &status);

        // Timestamp for when this tracking info is valid:
        PsychSetStructArrayDoubleElement("Time", 0, eyeGazeSampleTime.time ? XrTimeToPsychTime(eyeGazeSampleTime.time) : 0, status);

        // Eye tracking state:
        StatusFlags = 0;

        // Any position and orientation of gaze available? Could be just synthetic inter-/extrapolated data, not based
        // on actual tracked gaze, ie. not based on recent reliable sensor measurements. Could even be just fake data!
        if (gazeLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT && gazeLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT)
            StatusFlags |= 1;

        // Active gaze position and orientation from hardware eye tracking? This is trusworthy, with the caveat that all
        // current OpenXR gaze tracking extensions are allowed to return data that is based on real hardware measurements,
        // but multiple past measurements are combined to generate a result that is interpolated or extrapolated for the
        // requested target time xrPredictionTime. Also allowed is strong smoothing of data in time, ie. low-pass filtering:
        if (gazeLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT && gazeLocation.locationFlags & XR_SPACE_LOCATION_POSITION_TRACKED_BIT)
            StatusFlags |= 2;

        PsychSetStructArrayDoubleElement("Status", 0, StatusFlags, status);

        // Eye pose vector, encoding a 3 component 3D eye ball center position + 4 component orientation quaternion:
        v = NULL;
        PsychAllocateNativeDoubleMat(1, 7, 1, &v, &outMat);
        v[0] = gazeLocation.pose.position.x;
        v[1] = gazeLocation.pose.position.y;
        v[2] = gazeLocation.pose.position.z;

        v[3] = gazeLocation.pose.orientation.x;
        v[4] = gazeLocation.pose.orientation.y;
        v[5] = gazeLocation.pose.orientation.z;
        v[6] = gazeLocation.pose.orientation.w;
        PsychSetStructArrayNativeElement("GazePose", 0, outMat, status);
    }
    else {
        // Allocate out an empty struct:
        PsychAllocOutStructArray(3, kPsychArgOptional, -1, FieldCount3, FieldNames3, &status);
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
        "OVR.ControllerType_Remote = Connected remote control or similar, e.g., control buttons on a HMD.\n"
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
            path = openxr__handPath[0];
            break;

        case 2: // Right hand controller? OVR.ControllerType_RTouch
            path = openxr__handPath[1];
            break;

        case 4: // Remote control or equivalent? OVR.ControllerType_Remote
            path = openxr__handPath[3];
            break;

        case 16: // Gamepad / Microsoft X-Box controller or equivalent? OVR.ControllerType_XBox
            path = openxr__handPath[2];
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
    syncXRActions(openxr);

    // All input states updated by syncXRActions(). Query current values:

    // Button states:
    v = NULL;
    PsychAllocateNativeDoubleMat(1, 32, 1, &v, &outMat);
    for (i = 0; i < NUM_OVR_BUTTONS; i++) {
        // Skip indices without actual actions assigned:
        if (openxr__buttonAction[i] == XR_NULL_HANDLE) {
            v[i] = 0;
            continue;
        }

        actionStateGetInfo.action = openxr__buttonAction[i];
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
        if (openxr__touchAction[i] == XR_NULL_HANDLE) {
            v[i] = 0;
            continue;
        }

        actionStateGetInfo.action = openxr__touchAction[i];
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
        actionStateGetInfo.action = openxr__triggerValueAction[i];
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
        actionStateGetInfo.action = openxr__gripValueAction[i];
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
        actionStateGetInfo.action = openxr__thumbStick2DAction[i];
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
        actionStateGetInfo.action = openxr__thumbStick2DAction[i];
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
    if (!resultOK(result) || ((int) vc < eyeIndex + 1)) {
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
    if (!resultOK(result) || ((int) vc < eyeIndex + 1)) {
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

    // Return recommended width and height and MSAA samples of drawBuffer:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, views[eyeIndex].recommendedImageRectWidth);
    PsychCopyOutDoubleArg(2, kPsychArgOptional, views[eyeIndex].recommendedImageRectHeight);

    free(views);

    PsychCopyOutDoubleArg(3, kPsychArgOptional, openxr->recSamples);

    // Return maximum width and height and MSAA samples of drawBuffer:
    PsychCopyOutDoubleArg(4, kPsychArgOptional, openxr->maxSamples);
    PsychCopyOutDoubleArg(5, kPsychArgOptional, openxr->maxWidth);
    PsychCopyOutDoubleArg(6, kPsychArgOptional, openxr->maxHeight);

    return(PsychError_none);
}

PsychError PSYCHOPENXRCreateAndStartSession(void)
{
    static char useString[] = "videoRefreshDuration = PsychOpenXRCore('CreateAndStartSession', openxrPtr, deviceContext, openGLContext, openGLDrawable, openGLConfig, openGLVisualId, use3DMode, multiThreaded [, srcTexIds]);";
    //                         1                                                               1          2              3              4               5             6               7          8                9
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
    "'multiThreaded' if provided as non-zero value, will use an asynchronous presenter thread "
    "to try to improve stimulus presentation scheduling. A zero value means to not use such a thread, "
    "1 on demand, and 2 permanently for the duration of the whole session.\n"
    "'srcTexIds' Optional texture and FBO handles of the Screen() finalizedFBO backing textures and FBO's, "
    "needed for some workarounds for deficiencies of some MS-Windows OpenXR runtimes in multi-threaded mode. "
    "It has the format 'srcTexIds' = [leftTex, rightTex, leftFbo, rightFbo]. The driver tries to "
    "use an efficient texture based method, but falls back to a FBO based method if needed. This is "
    "a slower fallback path that can only handle non-MSAA content. If omitted, this workaround will "
    "not be used, achieving potentially higher performance, but likely resulting in malfunctions or "
    "hangs or crashes on some Windows OpenXR runtimes.\n"
    "\n"
    "Returns the following information:\n"
    "'videoRefreshDuration' Video refresh duration in seconds of the XR display device if "
    "available. If the info can't be queried a fallback value of 0.011 secs for a 90 Hz "
    "refresh rate is returned as one of the most common values for consumer HMDs.\n";
    static char seeAlsoString[] = "Open Close";

    int i, numTexHandles;
    int handle;
    int use3DMode;
    int multiThreaded;
    float displayRefreshRate;
    void* deviceContext;
    void* openGLContext;
    void* openGLDrawable;
    void* openGLConfig;
    void* openGLVisualId;
    int* srcTexIds;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(9));
    PsychErrorExit(PsychRequireNumInputArgs(8));

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

    // Get mandatory multiThreaded flag:
    PsychCopyInIntegerArg(8, kPsychArgRequired, &multiThreaded);
    if ((multiThreaded < 0) || (multiThreaded > 2))
        PsychErrorExitMsg(PsychError_user, "Invalid 'multiThreaded' flag provided. Must be 0, 1 or 2.");

    // Assign multi-threading mode:
    openxr->multiThreaded = multiThreaded;

    // Get optional srcTexIds list:
    if (PsychAllocInIntegerListArg(9, kPsychArgOptional, &numTexHandles, &srcTexIds)) {
        if (numTexHandles != 4)
            PsychErrorExitMsg(PsychError_user, "Invalid 'srcTexIds' list provided. Must be a vector with integer handles of length 4.");

        // Assign for use with certain workarounds:
        openxr->srcTexIds[0] = srcTexIds[0];
        openxr->srcTexIds[1] = srcTexIds[1];
        openxr->srcFboIds[0] = srcTexIds[2];
        openxr->srcFboIds[1] = srcTexIds[3];

        // Enable copy-workaround on MS-Windows for multi-threading on
        // deficient runtimes:
        openxr->needExtraCopyToLatch = TRUE;
    }
    else {
        openxr->needExtraCopyToLatch = FALSE;
    }

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

    openxr->deviceContext = deviceContext;
    openxr->openGLContext = openGLContext;
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

    // Reset submitted frame counter:
    openxr->frameIndex = 0;

    // Try to query video refresh rate of the device from runtime:
    if (pxrGetDisplayRefreshRateFB && resultOK(pxrGetDisplayRefreshRateFB(openxr->hmd, &displayRefreshRate))) {
        if (displayRefreshRate > 0)
            openxr->frameDuration = 1.0 / (double) displayRefreshRate;
        else
            openxr->frameDuration = 1.0; // Fallback for headless system: 1 Hz.

        if (verbosity > 3)
            printf("PsychOpenXRCore-INFO: Queried XR display refresh rate reported as %f Hz.\n", displayRefreshRate);
    }
    else {
        // Fallback to assumed 90 Hz as something reasonable for many consumer devices:
        openxr->frameDuration = 1.0 / 90.0;

        if (verbosity > 3)
            printf("PsychOpenXRCore-INFO: Could not query XR display refresh rate [%s], assuming 90 Hz.\n", (pxrGetDisplayRefreshRateFB) ? errorString : "Not supported");
    }

    // Assume the timeout for the compositor thinking we are unresponsive is 2 device frame durations:
    openxr->VRtimeoutSecs = 2 * openxr->frameDuration;

    // Return video refresh duration of the XR display:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, openxr->frameDuration);

    #if PSYCH_SYSTEM == PSYCH_LINUX
    // Monado proprietary metrics hack for timestamping requested?
    if (strstr(instanceProperties.runtimeName, "Monado") && getenv("XRT_METRICS_FILE") && strlen(getenv("XRT_METRICS_FILE"))) {
        // Yes.

        // Init: Try to open metrics fifo/pipe:
        errno = 0;
        openxr->pbfile = fopen(getenv("XRT_METRICS_FILE"), "rb");
        if (!openxr->pbfile) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Could not open Monado metrics protobuffer file '%s' [%s]! Monado timestamping disabled.\n",
                       getenv("XRT_METRICS_FILE"), strerror(errno));
        }
        else if (verbosity > 2)
            printf("PsychOpenXRCore-INFO: Monado metrics timestamping enabled for this session.\n");

        // Increase fifo pipe size to default maximum allowed value of 1 MB on Linux:
        if ((fcntl(fileno(openxr->pbfile), F_SETPIPE_SZ, 1024 * 1024) != 1024 * 1024) && (verbosity > 1))
            printf("PsychOpenXRCore-WARNING: Could not increase Monado metrics fifo size to maximum of 1MB. Expect trouble on long waits between flips in single-treaded mode!\n");

        // Turn it into a nanopb stream / init stream:
        openxr->pbstream.callback = &monadoMetricsCallback;
        openxr->pbstream.state = openxr->pbfile;
        openxr->pbstream.bytes_left = SIZE_MAX;
    }
    #endif

    // Create and initialize our standard space for rendering the views in device reference frame:
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
    refSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    if (!resultOK(xrCreateReferenceSpace(openxr->hmd, &refSpaceCreateInfo, &openxr->worldSpace))) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: xrCreateReferenceSpace() failed for WORLD space: %s\n", errorString);
        PsychErrorExitMsg(PsychError_system, "OpenXR session creation for OpenGL rendered XR failed in xrCreateReferenceSpace for WORLD space.");
    }

    // Init origin pose to identity:
    openxr->originPoseInPreviousSpace = identityPose;
    openxr->worldSpaceType = refSpaceCreateInfo.referenceSpaceType;

    for (i = 0; i < 2; i++) {
        // These are used to receive head/eye position + orientation information from 3D 6-DoF head tracking
        // and possibly eye tracking later on to establish the virtual cameras location/pose/FoV:
        openxr->view[i].type = XR_TYPE_VIEW;
        openxr->view[i].next = NULL;
    }

    // TODO: Could have setup of alternate action sets / actions / interaction profile bindings under
    // control of future API and user scripts here, to use as alternative to the setup made in
    // createDefaultXRInputConfig()?

    // At this point, our openxr__actionSet or future other action sets are ready for attachment,
    // filled with actions, and with all suggested action bindings for different device interaction
    // profiles set up and "suggested".

    for (i = 0; i < 2; i++) {
        XrActionSpaceCreateInfo actionSpaceCreateInfo = {
            .type = XR_TYPE_ACTION_SPACE_CREATE_INFO,
            .next = NULL,
            .action = openxr__handPoseAction,
            .poseInActionSpace = identityPose,
            .subactionPath = openxr__handPath[i]
        };

        if (!resultOK(xrCreateActionSpace(openxr->hmd, &actionSpaceCreateInfo, &openxr->handPoseSpace[i]))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create action space for hand/touch controller %i: %s\n", i, errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    // Create eye tracking action space, if eye tracking supported:
    if (openxr->hasEyeTracking) {
        XrActionSpaceCreateInfo gazeActionSpaceCreateInfo = {
            .type = XR_TYPE_ACTION_SPACE_CREATE_INFO,
            .next = NULL,
            .action = openxr__gazePoseAction,
            .poseInActionSpace = identityPose,
            .subactionPath = XR_NULL_PATH
        };

        if (!resultOK(xrCreateActionSpace(openxr->hmd, &gazeActionSpaceCreateInfo, &openxr->gazePoseSpace))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to create action space for basic eye gaze tracking: %s\n", errorString);

            // Failure return code:
            return(FALSE);
        }
    }

    // Attach our main/final actionSet(s) to our session:
    // This makes all the actions and bindings and suggested interaction profile bindings
    // immutable for the remaining life-time of the session, ie. this is quite final.
    XrSessionActionSetsAttachInfo actionSetsAttachInfo = {
        .type = XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO,
        .next = NULL,
        .countActionSets = 1,
        .actionSets = &openxr__actionSet
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
    "OVR.ControllerType_Remote = Connected remote control or similar, e.g., control buttons on a HMD.\n"
    "OVR.ControllerType_XBox = Microsoft XBox controller or some equivalent gamepad.\n"
    "\n";
    static char seeAlsoString[] = "GetInputState HapticPulse";

    int handle;
    PsychOpenXRDevice *openxr;

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
        printf("PsychOpenXRCore-ERROR:Controllers: Failed to poll events, or session state reports error abort!\n");

    syncXRActions(openxr);

    if (!processXREvents(xrInstance) && (verbosity > 0))
        printf("PsychOpenXRCore-ERROR:Controllers: Failed to poll events, or session state reports error abort!\n");

    // Return controllerTypes mask:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, getActiveControllers(xrInstance, openxr));

    return(PsychError_none);
}

PsychError PSYCHOPENXRTimingSupport(void)
{
    static char useString[] = "timingSupport = PsychOpenXRCore('TimingSupport' [, openxrPtr]);";
    //                         1                                                  1
    static char synopsisString[] =
    "Return available level of precise timing and timestamping support for OpenXR device 'openxrPtr'. "
    "If 'openxrPtr' is omitted, a more basic, general assessment of the OpenXR runtimes capabilities "
    "is returned.\n"
    "Standard unextended OpenXR-1 runtimes, as of February 2023, do not support reliable visual onset "
    "timestamping, and most tested runtimes don't support reliable onset timing either. These runtimes "
    "will return zero, hinting at the need for multi-threading tricks for bearable precision.\n"
    "The open-source Monado runtime does support precise onset timing, and may support some better "
    "onset timestamping for special customized Linux+Monado versions, or future Monado versions. This would "
    "be reported by a combination of one or more non-zero flags, as described below.\n"
    "The returned 'timingSupport' can be the logical OR of one of these flags:\n"
    "0 = None. Only use of multi-threading will allow for basic timing and\n"
    "    timestamping, but still of limited trustworthiness!\n"
    "1 = Some more reliable/trustworthy/accurate timing and timestamping precision\n"
    "    available, however, likely with drawbacks like significantly reduced\n"
    "    performance.\n"
    "\n";
    static char seeAlsoString[] = "PresentFrame PresenterThreadEnable";

    int handle, timingSupport;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));
    PsychErrorExit(PsychRequireNumInputArgs(0));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Default to response "No timing support":
    timingSupport = 0;

    // Get optional device handle, needed for more precise reporting:
    if (PsychCopyInIntegerArg(1, kPsychArgOptional, &handle)) {
        openxr = PsychGetXR(handle, FALSE);
        // TODO: As of now, no timing support in any OpenXR runtime:
    }
    else {
        // TODO: As of now, no timing support in any OpenXR runtime:
    }

    // Special Monado metrics logging based timestamping hack, only on Linux with
    // a slightly enhanced and recent (year 2023+ main branch) Monado and some
    // special config available? If so, we report some basic enhanced timestamping:
    #if PSYCH_SYSTEM == PSYCH_LINUX
    if (strstr(instanceProperties.runtimeName, "Monado") && getenv("XRT_METRICS_FILE") && strlen(getenv("XRT_METRICS_FILE")))
        timingSupport = 1;
    #endif

    // Return timingSupport mask:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, timingSupport);

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
    "to the XR compositor. Left and right eye must use identical 'width' and 'height'.\n\n"
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
    int width, height, floatFormat, numMSAASamples;
    float aspectRatio;
    uint32_t nFormats, i;
    int64_t *swapchainFormats = NULL;
    int64_t imageFormat = -1;
    PsychOpenXRDevice *openxr;

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
    openxr->numMSAASamples = numMSAASamples;

    // Initialize projection views and quad views with what can be statically initialized:

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

    // Size of virtual projection screen / display monitor is 1 meter high, and a width
    // that is 1 meter times the aspect ratio of the input framebuffer, so we don't get
    // squished or stretched looking stimuli by default:
    aspectRatio = (float) openxr->textureWidth / (float) openxr->textureHeight;
    openxr->quadViewLayer[eyeIndex].size.width = aspectRatio;
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
    PsychAssignSubmitLayers(openxr, openxr->use3DMode ? 1 : 0);

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

static XrResult releaseTextureHandles(PsychOpenXRDevice *openxr)
{
    XrResult res, outRes = XR_SUCCESS;
    int eyeIndex;

    #if PSYCH_SYSTEM == PSYCH_WINDOWS
    if (openxr->multiThreaded && openxr->needExtraCopyToLatch) {
        typedef void (*PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint handle);
        typedef void (*PFNGLCOPYIMAGESUBDATAPROC)(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);
        static PFNGLBINDFRAMEBUFFERPROC myglBindFramebuffer = NULL;
        static PFNGLCOPYIMAGESUBDATAPROC myglCopyImageSubData = NULL;

        // Windows OpenXR runtimes usually use the OpenGL-DirectX/Direct3D interop extension WGL_NV_DX_interop, which
        // has substantial limitations wrt. using the shared Direct3D resource (converted into a OpenGL GL_TEXTURE_2D
        // texture handle) on multiple OpenGL contexts, which make sharing the texture across our OpenXR OpenGL work
        // context and Screen's OpenGL contexts impossible, especially in the multi-threaded case. Both OculusVR and
        // SteamVR have serious error cases!
        // Therefore we don't let Screen's finalizedFBO's be backed by our xrSwapchain textures. Instead we keep them
        // separate and perform a manual copy-blit here from Screen's currently bound finalizedFBO framebuffer to our
        // interop texture.
        HGLRC oldContext = wglGetCurrentContext();
        if (oldContext != openxr->openGLContext)
            wglMakeCurrent(openxr->deviceContext, openxr->openGLContext);

        // If myglBindFramebuffer not yet assigned, do it now, after our OpenGL context was activated.
        // All contexts with same pixelformat return the same function pointer - so all ours do on a
        // single-gpu configuration like the one supported by OpenXR and this driver:
        if (myglBindFramebuffer == NULL)
            myglBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebufferEXT");

        if (myglCopyImageSubData == NULL)
            myglCopyImageSubData = (PFNGLCOPYIMAGESUBDATAPROC) wglGetProcAddress("glCopyImageSubData");

        for (eyeIndex = 0; eyeIndex < ((openxr->isStereo) ? 2 : 1); eyeIndex++) {
            while (glGetError());

            // Fast path supported?
            if (myglCopyImageSubData != NULL) {
                // Yes. Use glCopyImageSubData(), supported since OpenGL 4.3 and before as ARB extension
                // GL_ARB_copy_image. This is the fast path, copying textures to textures, memcpy() style:
                GLenum target = (openxr->numMSAASamples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
                myglCopyImageSubData(openxr->srcTexIds[eyeIndex], target, 0, 0, 0, 0,
                                     openxr->currentTextures[eyeIndex], target, 0, 0, 0, 0,
                                     openxr->textureWidth, openxr->textureHeight, 1);
            }

            // Unsupported, or failed due to incompatible formats between source and target texture?
            if ((myglCopyImageSubData == NULL) || (glGetError() == GL_INVALID_OPERATION)) {
                // Fallback: Can only do non-MSAA textures, and has higher overhead. It gets also triggered
                // if Screen's texture formats and OpenXR swapchains texture formats are incompatible. One
                // culprit is SteamVR on Linux, at least with OculusVR backend, which often uses highly
                // incompatible formats.
                myglBindFramebuffer(GL_READ_FRAMEBUFFER, openxr->srcFboIds[eyeIndex]);
                glBindTexture(GL_TEXTURE_2D, openxr->currentTextures[eyeIndex]);
                glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, openxr->textureWidth, openxr->textureHeight);
                glBindTexture(GL_TEXTURE_2D, 0);
                myglBindFramebuffer(GL_FRAMEBUFFER, 0);
                glFinish();
            }
        }

        // Restoring the old OpenGL context binding should always make sense, but doing so on OculusVR
        // just ends badly multi-threaded, due to OculusVR runtime bugs as of v1.79.0 / January 2023.
        // The runtime does not activate its OpenXR work OpenGL context in all relevant functions, as
        // it should, so things go sideways...
        // Therefore only restore the original (Screen()) OpenGL context if we are on the main thread:
        if ((oldContext != openxr->openGLContext) && PsychIsMasterThread())
            wglMakeCurrent(openxr->deviceContext, oldContext);
    }
    #endif

    // Release new stimulus images to swapchain before xrEndFrame() submission:
    for (eyeIndex = 0; eyeIndex < ((openxr->isStereo) ? 2 : 1); eyeIndex++) {
        res = xrReleaseSwapchainImage(openxr->textureSwapChain[eyeIndex], NULL);
        if (!resultOK(res)) {
            if (outRes == XR_SUCCESS)
                outRes = res;
        }
    }

    return(outRes);
}

static XrResult acquireTextureHandles(PsychOpenXRDevice *openxr)
{
    XrResult result;
    uint32_t texIndex;
    int eyeIndex;

    for (eyeIndex = 0; eyeIndex < ((openxr->isStereo) ? 2 : 1); eyeIndex++) {
        // Assume failure until a proper result is assigned:
        openxr->currentTextures[eyeIndex] = 0;

        // Acquire next texture handle:
        XrSwapchainImageAcquireInfo acquireInfo = {
            .type = XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO,
            .next = NULL
        };

        result = xrAcquireSwapchainImage(openxr->textureSwapChain[eyeIndex], &acquireInfo, &texIndex);
        if (!resultOK(result)) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to acquire next swapchain image for eye %i: %s\n", eyeIndex, errorString);

            return(result);
        }

        // Wait for its availability for up to 1 second = 1e9 nsecs:
        XrSwapchainImageWaitInfo waitTexInfo = {
            .type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO,
            .next = NULL,
            .timeout = (XrDuration) 1e9
        };

        result = xrWaitSwapchainImage(openxr->textureSwapChain[eyeIndex], &waitTexInfo);
        if (!resultOK(result)) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to wait for next swapchain image for eye %i: %s\n", eyeIndex, errorString);

            return(result);
        }

        if ((result == XR_TIMEOUT_EXPIRED) && (verbosity > 1))
            printf("PsychOpenXRCore-WARNING: Timed out waiting for next swapchain image for eye %i: %s\n", eyeIndex, errorString);

        // Latch latest acquired and ready textures for consumption:
        openxr->currentTextures[eyeIndex] = openxr->textureSwapChainImages[eyeIndex][texIndex].image;
    }

    return(result);
}

PsychError PSYCHOPENXRGetNextTextureHandle(void)
{
    static char useString[] = "texObjectHandle = PsychOpenXRCore('GetNextTextureHandle', openxrPtr, eye);";
    //                         1                                                         1          2
    static char synopsisString[] =
    "Retrieve OpenGL texture object handle for next target texture for OpenXR device 'openxrPtr'.\n"
    "'eye' Eye for which handle of next texture should be returned: 0 = Left/Mono, 1 = Right.\n"
    "Returns a GL_TEXTURE_2D texture object name/handle in 'texObjectHandle' for the texture "
    "to which the next XR frame should be rendered. Returns -1 if busy.\n";
    static char seeAlsoString[] = "CreateRenderTextureChain";

    int handle, eyeIndex;
    double texHandle;
    PsychOpenXRDevice *openxr;
    XrResult result;

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

    if (isMultithreaded(openxr)) {
        result = XR_SUCCESS;
    }
    else {
        // Left or mono eye triggers actual XR processing:
        if (eyeIndex == 0) {
            // Get next free/non-busy buffer for this eyes texture swap chain:
            result = acquireTextureHandles(openxr);
        }
        else {
            result = XR_SUCCESS;
        }
    }

    PsychLockMutex(&(openxr->presenterLock));
    texHandle = (double) openxr->currentTextures[eyeIndex];
    PsychUnlockMutex(&(openxr->presenterLock));

    // Return texture object handle:
    if ((result == XR_SUCCESS) && (texHandle > 0)) {
        PsychCopyOutDoubleArg(1, kPsychArgOptional, texHandle);
    }
    else {
        PsychCopyOutDoubleArg(1, kPsychArgOptional, -1);
    }

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
    static char useString[] = "[projL, projR, fovL, fovR] = PsychOpenXRCore('GetStaticRenderParameters', openxrPtr [, clipNear=0.01][, clipFar=10000.0]);";
    //                          1      2      3     4                                                    1            2                3
    static char synopsisString[] =
    "Retrieve static rendering parameters for OpenXR device 'openxrPtr' at current settings.\n"
    "'clipNear' Optional near clipping plane for OpenGL. Defaults to 0.01.\n"
    "'clipFar' Optional far clipping plane for OpenGL. Defaults to 10000.0.\n"
    "\nReturn arguments:\n\n"
    "'projL' is the 4x4 OpenGL projection matrix for the left eye rendering.\n"
    "'projR' is the 4x4 OpenGL projection matrix for the right eye rendering.\n"
    "Please note that projL and projR are usually identical for typical rendering scenarios.\n"
    "'fovL' is the [leftAngle, rightAngle, upAngle, downAngle] field of view for the left eye rendering.\n"
    "'fovR' is the [leftAngle, rightAngle, upAngle, downAngle] field of view for the right eye rendering.\n"
    "fovL and fovR angles are in units of radians, from the optical axis.\n";
    static char seeAlsoString[] = "";

    int handle;
    PsychOpenXRDevice *openxr;
    double ML[4][4];
    double MR[4][4];
    int i, j;
    double clip_near, clip_far;
    double *outM;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(4));
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
    PsychLockMutex(&(openxr->presenterLock));
    locateXRViews(openxr, openxr->predictedDisplayTime);

    if (verbosity > 4) {
        int eyeIndex;

        for (eyeIndex = 0; eyeIndex < 2; eyeIndex++) {
            printf("PsychOpenXRCore-INFO: device %i, eye %i - FoV degrees: %f %f %f %f\n", handle, eyeIndex,
                   rad2deg(openxr->view[eyeIndex].fov.angleLeft), rad2deg(openxr->view[eyeIndex].fov.angleRight),
                   rad2deg(openxr->view[eyeIndex].fov.angleUp), rad2deg(openxr->view[eyeIndex].fov.angleDown));
        }
    }

    // Build left / right projection matrices:
    buildProjectionMatrix(ML, openxr->view[0].fov, clip_near, clip_far);
    buildProjectionMatrix(MR, openxr->view[1].fov, clip_near, clip_far);

    PsychUnlockMutex(&(openxr->presenterLock));

    // Return left projection matrix as return argument 1:
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 4, 4, 1, &outM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(outM++) = ML[j][i];

    // Return right projection matrix as return argument 2:
    PsychAllocOutDoubleMatArg(2, kPsychArgOptional, 4, 4, 1, &outM);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            *(outM++) = MR[j][i];

    // Return left eye field of view angles in radians:
    PsychAllocOutDoubleMatArg(3, kPsychArgOptional, 1, 4, 1, &outM);
    outM[0] = openxr->view[0].fov.angleLeft;
    outM[1] = openxr->view[0].fov.angleRight;
    outM[2] = openxr->view[0].fov.angleUp;
    outM[3] = openxr->view[0].fov.angleDown;

    // Return right eye field of view angles in radians:
    PsychAllocOutDoubleMatArg(4, kPsychArgOptional, 1, 4, 1, &outM);
    outM[0] = openxr->view[1].fov.angleLeft;
    outM[1] = openxr->view[1].fov.angleRight;
    outM[2] = openxr->view[1].fov.angleUp;
    outM[3] = openxr->view[1].fov.angleDown;

    return(PsychError_none);
}

// Execute XR present operation, possibly committing new content to the swapchains:
// Must be called with the presenterLock locked!
// Called by idle presenterThread to keep XR compositor timeout handling from kicking in,
// and directly from PSYCHOPENXRPresentFrame when userspace wants to present new content.
static double PresentExecute(PsychOpenXRDevice *openxr, psych_bool inInit)
{
    int eyeIndex;
    psych_bool success = TRUE;
    double tPredictedOnset = -2;
    XrResult result;

    // Workaround for broken MS-Windows OpenXR runtimes. Manually manage the OpenGL
    // context, ie. transition current context -> OpenXR context. The out path has
    // a corresponding transition back:
    #if PSYCH_SYSTEM == PSYCH_WINDOWS
    HGLRC oldContext = wglGetCurrentContext();
    if (oldContext != openxr->openGLContext)
        wglMakeCurrent(openxr->deviceContext, openxr->openGLContext);
    #endif

    // Skip if no frame present cycle is wanted:
    if (!openxr->needFrameLoop) {
        success = TRUE;
        tPredictedOnset = 0;
        goto present_out;
    }

    if (!inInit) {
        // Validate requested presentation time targetPresentTime. If it is valid, convert from Psychtoolbox GetSecs time
        // to absolute XrTime targetDisplayTime. Otherwise select targetDisplayTime as now:
        XrTime targetDisplayTime = (openxr->targetPresentTime < DBL_MAX) ? PsychTimeToXrTime(openxr->targetPresentTime) : PsychTimeToXrTime(PsychGetAdjustedPrecisionTimerSeconds(NULL));

        // To my current knowledge, only Monado handles frameEndInfo.displayTime properly, so all other
        // runtimes need a manual wait here:
        if ((openxr->targetPresentTime < DBL_MAX) && !strstr(instanceProperties.runtimeName, "Monado"))
            PsychWaitUntilSeconds(openxr->targetPresentTime - openxr->frameDuration);

        // If targetDisplayTime is earlier than earliest next possible display time predictedDisplayTime, then set it
        // to earliest possible display time. This is crucial for xrEndFrame() to not fail completely under SteamVR:
        if (targetDisplayTime < openxr->predictedDisplayTime)
            targetDisplayTime = openxr->predictedDisplayTime;

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

        // Enforce projView[] update with proper fov, pose if projectionLayers are in use atm.:
        if (openxr->viewLayerType == 1) {
            // Only do initial locateXRViews() if projView has not ever been updated, ie. an initialization call,
            // so we don't feed garbage from openxr->view pose to projView pose, as runtimes don't like that.
            //
            // Or if the user script has marked its own tracking + rendering loop as paused, or not using head
            // tracking, e.g., for conventional 3D rendering without input from head tracker, and we need to do
            // the job of keeping projView pose up to date wrt. target display time, so the views stay locked
            // to the viewers head, like quadViewLayers do.
            //
            // During normal operation, this would be redundant, due to the locateXRViews() call in 'GetTrackingState',
            // as part of PsychVRHMD('PrepareRender') at start of each full 3D rendering cycle. This takes 0.2 - 0.5
            // msecs typical, but sometimes up to 5 msecs, so worth optimizing away when not strictly needed.
            if ((!openxr->isTracking && openxr->needLocate) ||
                (!memcmp(&openxr->projView[0].pose, &identityPose, sizeof(identityPose)) &&
                !memcmp(&openxr->projView[1].pose, &identityPose, sizeof(identityPose))))
                locateXRViews(openxr, frameEndInfo.displayTime);

            // Update pose and FoV for projection views by latching latest locateXRViews() result:
            for (eyeIndex = 0; eyeIndex < ((openxr->isStereo) ? 2 : 1); eyeIndex++) {
                openxr->projView[eyeIndex].pose = openxr->view[eyeIndex].pose;
                openxr->projView[eyeIndex].fov = openxr->view[eyeIndex].fov;
            }
        }

        result = xrEndFrame(openxr->hmd, &frameEndInfo);
        if (!resultOK(result)) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to xrEndFrame for GetSecs time %f [secs] == targetDisplayTime %ld [xrTime units]: %s\n",
                       openxr->targetPresentTime, targetDisplayTime, errorString);

            success = FALSE;
            goto present_fail;
        }
        else if (verbosity > 6) {
            printf("PsychOpenXRCore-INFO: [%f]: xrEndFrame done for GetSecs time %f [secs] == targetDisplayTime %ld [xrTime units]: %f secs ahead.\n",
                   PsychGetAdjustedPrecisionTimerSeconds(NULL), openxr->targetPresentTime, targetDisplayTime,
                   XrTimeToPsychTime(targetDisplayTime) - XrTimeToPsychTime(openxr->predictedDisplayTime));
        }

        success = TRUE;

        // Increment counter of submitted client XR frames:
        openxr->frameIndex++;

        // On Monado we must do a wait iff Monado specific metrics timestamping is not used, because standard Monado will
        // properly schedule frame onset as per displayTime, but won't block until frame onset, so we need an active wait
        // to guesstimate the right time to xrWaitFrame to get some semblance of a timestamp:
        if ((openxr->targetPresentTime < DBL_MAX) && (tPredictedOnset == -2) && strstr(instanceProperties.runtimeName, "Monado")) {
            // On Linux we may be able to use metrics timestamping on Monado, so can skip the wait, on other OS'es we can't:
            #if PSYCH_SYSTEM == PSYCH_LINUX
            if (!openxr->pbfile)
            #endif
                PsychWaitUntilSeconds(openxr->targetPresentTime);
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

    // Latch new predictedDisplayTime for all consumers:
    openxr->predictedDisplayTime = openxr->frameState.predictedDisplayTime;

    // If no onset timestamp is assigned yet, give the Monado metrics timestamping
    // hack a chance. It will return -1 on unsupported, 0 if frame is reported as
    // dropped or skipped, and > 0 for an actual frame onset timestamp:
    if (!inInit && success && (tPredictedOnset == -2))
        tPredictedOnset = executeMonadoMetricsCycle(openxr, openxr->frameIndex);

    // If we made it up to here and no valid onset timestamp was assigned yet, then
    // mark frame as not skipped, ie. set tPredictedOnset to greater than zero timestamp:
    // Note that this is likely somewhat wrong, but the best we can do on OpenXR runtimes
    // without builtin timestamping support:
    if (tPredictedOnset < 0)
        tPredictedOnset = XrTimeToPsychTime(openxr->predictedDisplayTime) - openxr->frameDuration;

    result = xrBeginFrame(openxr->hmd, NULL);
    if (!resultOK(result)) {
        if (verbosity > 0)
            printf("PsychOpenXRCore-ERROR: Failed to xrBeginFrame: %s\n", errorString);

        success = FALSE;
        goto present_fail;
    }

present_out:
present_fail:

    #if PSYCH_SYSTEM == PSYCH_WINDOWS
    if (oldContext != openxr->openGLContext)
        wglMakeCurrent(openxr->deviceContext, oldContext);
    #endif

    openxr->tPredictedOnset = tPredictedOnset;
    return(success ? tPredictedOnset : -1);
}

static psych_bool PresentCycle(PsychOpenXRDevice* openxr)
{
    XrResult result;
    int eyeIndex;
    psych_bool latched = FALSE;
    psych_bool success = TRUE;
    double tOnsetTimestamp = -1;
    double targetPresentTime = openxr->targetPresentTime;

    if (!openxr->sessionActive) {
        PsychUnlockMutex(&(openxr->presenterLock));
        PsychYieldIntervalSeconds(0.5 * openxr->frameDuration);
        goto presentcycle_fail;
    }

    // Validate requested presentation time targetPresentTime. If it is valid, convert from Psychtoolbox GetSecs time
    // to absolute XrTime targetDisplayTime. Otherwise select next possible predicted display time:
    XrTime targetDisplayTime = (targetPresentTime < DBL_MAX) ? PsychTimeToXrTime(targetPresentTime) : openxr->predictedDisplayTime;

    // Target display time for next stimulus frame reached or exceeded?
    if ((targetPresentTime < DBL_MAX) && (openxr->predictedDisplayTime >= targetDisplayTime)) {
        // Next xrEndFrame() submitted frame is predicted to display at or after targetDisplayTime,
        // time to latch our new content for targetDisplayTime.
        if (!resultOK(releaseTextureHandles(openxr))) {
            if (verbosity > 0)
                fprintf(stderr, "PsychOpenXRCore-ERROR:PresentCycle(): Failed to release current swapchain images: %s\n", errorString);
        }

        // Mark "new frame latched":
        latched = TRUE;

        // Invalidate target time as it is used up:
        openxr->targetPresentTime = DBL_MAX;

        // Assign predictedDisplayTime for the just latched frame as onset timestamps:
        // Must be done here, before xrWaitFrame below would update predictedDisplayTime!
        openxr->tPredictedOnset = XrTimeToPsychTime(openxr->predictedDisplayTime);
    }

    // Prepare frameEndInfo, with next possible display time:
    XrFrameEndInfo frameEndInfo = {
        .type = XR_TYPE_FRAME_END_INFO,
        .next = NULL,
        // TODO: Could pass targetDisplayTime iff it is not more than maybe < 1 msec earlier than predictedDisplayTime?
        // Might give compositors with async reprojection, time/space-warp etc. more accurate data for their algorithms.
        // Problem: SteamVR chokes if more than maybe a millisecond in the past, so current predictedDisplayTime is the
        // safe choice.
        .displayTime = openxr->predictedDisplayTime,
        .environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
        .layerCount = (openxr->frameState.shouldRender) ? openxr->submitLayersCount : 0,
        .layers = openxr->submitLayers
    };

    // Enforce projView[] update with proper fov, pose if projectionLayers are in use atm.:
    if (openxr->viewLayerType == 1) {
        // Only do initial locateXRViews() if projView has not ever been updated, ie. an initialization call,
        // so we don't feed garbage from openxr->view pose to projView pose, as runtimes don't like that.
        //
        // Or if the user script has marked its own tracking + rendering loop as paused, or not using head
        // tracking, e.g., for conventional 3D rendering without input from head tracker, and we need to do
        // the job of keeping projView pose up to date wrt. target display time, so the views stay locked
        // to the viewers head, like quadViewLayers do.
        //
        // During normal operation, this would be redundant, due to the locateXRViews() call in 'GetTrackingState',
        // as part of PsychVRHMD('PrepareRender') at start of each full 3D rendering cycle. This takes 0.2 - 0.5
        // msecs typical, but sometimes up to 5 msecs, so worth optimizing away when not strictly needed.
        if ((!openxr->isTracking && openxr->needLocate) ||
            (!memcmp(&openxr->projView[0].pose, &identityPose, sizeof(identityPose)) &&
            !memcmp(&openxr->projView[1].pose, &identityPose, sizeof(identityPose))))
            locateXRViews(openxr, frameEndInfo.displayTime);

        // Update pose and FoV for projection views by latching latest locateXRViews() result:
        for (eyeIndex = 0; eyeIndex < ((openxr->isStereo) ? 2 : 1); eyeIndex++) {
            openxr->projView[eyeIndex].pose = openxr->view[eyeIndex].pose;
            openxr->projView[eyeIndex].fov = openxr->view[eyeIndex].fov;
        }
    }

    // Submit most recently released swapchain images:
    result = xrEndFrame(openxr->hmd, &frameEndInfo);
    if (!resultOK(result)) {
        if (verbosity > 0) {
            if (latched)
                fprintf(stderr, "PsychOpenXRCore-ERROR:PresentCycle(): Failed to xrEndFrame for GetSecs time %f [secs] == targetDisplayTime %ld [xrTime units]: %s\n",
                        targetPresentTime, targetDisplayTime, errorString);
            else
                fprintf(stderr, "PsychOpenXRCore-ERROR:PresentCycle(): Failed to xrEndFrame in idle-loop for displayTime %ld [xrTime units]: %s\n",
                        frameEndInfo.displayTime, errorString);
        }

        // Mark as failed:
        success = FALSE;
    }
    else {
        if (verbosity > 6)
            fprintf(stderr, "PsychOpenXRCore-INFO: [%f] PresentCycle(): xrEndFrame done for predicted onset time %ld xrTime units: %f %s\n",
                    PsychGetAdjustedPrecisionTimerSeconds(NULL), frameEndInfo.displayTime,
                    latched ? (XrTimeToPsychTime(frameEndInfo.displayTime) - XrTimeToPsychTime(targetDisplayTime)) : 1,
                    latched ? "secs after targetDisplayTime. Frame latched!" : ".");

        // So far successfull:
        success = TRUE;

        // Increment counter of submitted client XR frames:
        openxr->frameIndex++;
    }

    // Start next frame present cycle:
    openxr->frameState.type = XR_TYPE_FRAME_STATE;
    openxr->frameState.next = NULL;

    XrFrameWaitInfo frameWaitInfo = {
        .type = XR_TYPE_FRAME_WAIT_INFO,
        .next = NULL
    };

    if (latched) {
        // Set returned onset timestamp to -1 for failure, or to 0 for skipped present:
        if (!success)
            openxr->tPredictedOnset = -1;
        else if (frameEndInfo.layerCount == 0)
            openxr->tPredictedOnset = 0;

        result = acquireTextureHandles(openxr);
        if (!resultOK(result)) {
            if (verbosity > 0)
                fprintf(stderr, "PsychOpenXRCore-ERROR:PresentCycle(): Failed to acquireTextureHandles(): %s\n", errorString);

            success = FALSE;
        }
    }

    // Wait for next compositor sync, with lock dropped:
    PsychUnlockMutex(&(openxr->presenterLock));

    if (success) {
        // Give the Monado metrics timestamping hack a chance. It will return -1
        // on unsupported, 0 if frame is reported as dropped or skipped, and > 0
        // for an actual frame onset timestamp. We must always run executeMonadoMetricsCycle,
        // to keep the metrics fifo drained:
        tOnsetTimestamp = executeMonadoMetricsCycle(openxr, openxr->frameIndex);

        // New XR stimulus frame latched this cycle?
        if (latched) {
            // If a new XR stimulus frame was latched this cycle, and some actual
            // layer visual content was submitted, and executeMonadoMetricsCycle()
            // returned a valid timestamp (or 0 for a skipped/dropped frame), then
            // update the stimulus onset timestamp returned to the user script:
            if (tOnsetTimestamp >= 0) {
                if (frameEndInfo.layerCount > 0)
                    openxr->tPredictedOnset = tOnsetTimestamp;

                // Signal completed presentation of the new stimulus frame to the
                // main-thread, to complete 'PresentFrame' aka Screen('Flip') with
                // proper timestamps:
                PsychSignalCondition(&(openxr->presentedSignal));

                // Reset latched state, so we don't PsychSignalCondition() again
                // below:
                latched = FALSE;
            }
        }
    }

    result = xrWaitFrame(openxr->hmd, &frameWaitInfo, &openxr->frameState);
    if (!resultOK(result)) {
        if (verbosity > 0)
            fprintf(stderr, "PsychOpenXRCore-ERROR:PresentCycle(): Failed to xrWaitFrame: %s\n", errorString);

        success = FALSE;
        goto presentcycle_fail;
    }

    PsychLockMutex(&(openxr->presenterLock));

    // Latch new predictedDisplayTime for all consumers under mutex protection:
    openxr->predictedDisplayTime = openxr->frameState.predictedDisplayTime;

    PsychUnlockMutex(&(openxr->presenterLock));

    // New frame latched?
    if (latched) {
        // Signal latching of new stimulus frame for presentation to main thread,
        // important to do this after acquiring new texture handles and waiting
        // for their availability. Also important that we do this after xrWaitFrame
        // has synced us up to the XR compositor work cycle, so the waiting main
        // thread resumes its rendering work at just the right time to be optimally
        // in phase with the compositor. This makes a big difference for performance!
        PsychSignalCondition(&(openxr->presentedSignal));
    }

    // Give main-thread some time to do stuff without us holding the mutex:
    PsychYieldIntervalSeconds(0.5 * openxr->frameDuration);

    result = xrBeginFrame(openxr->hmd, NULL);
    if (!resultOK(result)) {
        if (verbosity > 0)
            fprintf(stderr, "PsychOpenXRCore-ERROR:PresentCycle(): Failed to xrBeginFrame: %s\n", errorString);

        success = FALSE;
    }

    presentcycle_fail:

    PsychLockMutex(&(openxr->presenterLock));

    return(success);
}

static void* PresenterThreadMain(void* psychOpenXRDeviceToCast)
{
    int rc;
    PsychOpenXRDevice* openxr = (PsychOpenXRDevice*) psychOpenXRDeviceToCast;

    // Assign a name to ourselves, for debugging:
    PsychSetThreadName("PsychOpenXRCorePresenterThread");

    // Try to lock, block until available if not available:
    if ((rc = PsychLockMutex(&(openxr->presenterLock)))) {
        // A regular printf() would be no-opped, as we try printing from outside the main interpreter thread.
        // Use fprintf(stderr, ...) instead of the overloaded printf() (aka mexPrintf()):
        fprintf(stderr, "PsychOpenXRCore-ERROR: In PresenterThreadMain(): First mutex_lock in init failed  [%s].\n", strerror(rc));
        return(NULL);
    }

    // Signal startup completed to main thread:
    PsychSignalCondition(&(openxr->presentedSignal));

    // XR compositor timeout prevention loop: Repeats infinitely, until we receive a
    // shutdown request and terminate ourselves. PresentCycle() will execute once per
    // XR compositor work cycle. Each cycle it goes to sleep in xrWaitFrame, with the
    // presenterLock temporarily dropped, syncing up to the compositor:
    while (!openxr->threadTerminate) {
        if (!PresentCycle(openxr) && (verbosity > 0))
            fprintf(stderr, "PsychOpenXRCore-ERROR: In PresenterThreadMain(): PresentCycle() failed!\n");
    }

    // Exit path from thread at thread termination. Go and die peacefully...
    PsychUnlockMutex(&(openxr->presenterLock));

    return(NULL);
}

PsychError PSYCHOPENXRPresentFrame(void)
{
    static char useString[] = "[tPredictedOnset, tPredictedFutureOnset] = PsychOpenXRCore('PresentFrame', openxrPtr [, when=0]);";
    //                          1                2                                                        1            2
    static char synopsisString[] =
    "Present last rendered frame to OpenXR display device 'openxrPtr'.\n\n"
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
    "\n";
    static char seeAlsoString[] = "";

    int rc;
    int handle;
    double tNow, tWhen, tPredictedOnset, tPredictedFutureOnset;
    PsychOpenXRDevice *openxr;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(2));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychOpenXRCheckInit(FALSE);

    // Get device handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
    openxr = PsychGetXR(handle, FALSE);

    // Process pending OpenXR events:
    if (!processXREvents(xrInstance) && (verbosity > 0))
        printf("PsychOpenXRCore-ERROR:PresentFrame: Failed to poll events, or session state reports error abort!\n");

    if (verbosity > 4)
        printf("PsychOpenXRCore-INFO:PresentFrame: Session state for handle %i: Session %s, frame loop needs to be %s.\n", handle,
               openxr->sessionActive ? "ACTIVE" : "STOPPED", openxr->needFrameLoop ? "RUNNING" : "STOPPED");

    // During single-threaded presentation, we need to latch the to-be-presented frame to the
    // OpenXR runtime / swapchain, in multi-threaded mode the presenterThread will do that:
    if (!isMultithreaded(openxr)) {
        if (!resultOK(releaseTextureHandles(openxr))) {
            if (verbosity > 0)
                printf("PsychOpenXRCore-ERROR: Failed to release current swapchain images: %s\n", errorString);
        }
    }

    if (openxr->needFrameLoop) {
        // Get optional presentation target time and clamp it to be never more than
        // 10 seconds in the past, to simplify timing code in the present operations
        // and thread:
        tWhen = 0;
        PsychCopyInDoubleArg(2, kPsychArgOptional, &tWhen);
        tNow = PsychGetAdjustedPrecisionTimerSeconds(NULL);
        if (tWhen < tNow - 10.0)
            tWhen = tNow - 10.0;

        if (isMultithreaded(openxr)) {
            // Background parallel presenter thread will latch our new rendered frame at the
            // proper time to target visual onset at targetPresentTime, so lock the lock and
            // latch the time:
            PsychLockMutex(&(openxr->presenterLock));
            openxr->targetPresentTime = tWhen;

            // Wait for signal of present completion:
            while (openxr->targetPresentTime != DBL_MAX)
                if ((rc = PsychWaitCondition(&(openxr->presentedSignal), &(openxr->presenterLock)))) {
                    printf("PsychOpenXRCore-ERROR: Waitcondition on presentedSignal trigger failed  [%s].\n", strerror(rc));
                }

            // Retrieve estimated stimulus onset time:
            tPredictedOnset = openxr->tPredictedOnset;

            // Retrieve best-guess future stimulus onset time for future frame / Flip:
            tPredictedFutureOnset = XrTimeToPsychTime(openxr->predictedDisplayTime);

            PsychUnlockMutex(&(openxr->presenterLock));
        }
        else {
            // Define targetPresentTime and execute present on this main thread:
            openxr->targetPresentTime = tWhen;
            tPredictedOnset = PresentExecute(openxr, FALSE);
            tPredictedFutureOnset = XrTimeToPsychTime(openxr->predictedDisplayTime);
            // Invalidate targetPresentTime after present:
            openxr->targetPresentTime = DBL_MAX;
        }
    }
    else {
        tPredictedOnset = 0;
        tPredictedFutureOnset = 0;
    }

    if ((tPredictedOnset < 0) && (verbosity > 0))
        printf("PsychOpenXRCore-ERROR: Failed to present new frame to XR compositor.\n");

    if ((tPredictedOnset == 0) && (verbosity > 4))
        printf("PsychOpenXRCore-INFO: Present of new frame to XR compositor skipped.\n");

    // Return our best estimate of visual stimulus onset time-point at centr of XR display:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, tPredictedOnset);

    // Return "best case scenario" predicted stimulus onset for the next/future presented frame:
    PsychCopyOutDoubleArg(2, kPsychArgOptional, tPredictedFutureOnset);

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
    "0 < freq <= 1 will be mapped to 0 Hz to 320 Hz for compatibility with other older XR api's, runtimes, and Oculus devices.\n"
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
    double duration, freq, amplitude, pulseEndTime;
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
            outpath = openxr__handPath[0];
            break;

        case 2: // Right hand controller? OVR.ControllerType_RTouch
            outpath = openxr__handPath[1];
            break;

        case 0xffffffff: // All active controllers? OVR.ControllerType_Active
            // XR_NULL_PATH triggers haptic action on all capable and active devices:
            outpath = XR_NULL_PATH;
            break;

        case 16: // Microsoft X-Box controller or equivalent? OVR.ControllerType_XBox
            outpath = openxr__handPath[2];
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
        .action = openxr__hapticAction,
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
    pulseEndTime += duration;
    PsychCopyOutDoubleArg(1, kPsychArgOptional, pulseEndTime);

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

    // Number of available tracker cameras:
    trackerCount = XXX(openxr->hmd);

    PsychAllocOutStructArray(1, kPsychArgOptional, trackerCount, FieldCount, FieldNames, &status);

    for (i = 0; i < trackerCount; i++) {
        trackerPose = XXX(openxr->hmd, (unsigned int) i);
        StatusFlags = trackerPose.TrackerFlags & (XXX);

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

        trackerDesc = XXX(openxr->hmd, (unsigned int) i);

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
    "then the prediction is performed for the mid-point of the next possible video frame of the device, ie. "
    "the most likely presentation time for immediately rendered images.\n\n"
    "Return value is the vector 'eyePose' which defines the position and orientation for the eye corresponding "
    "to the requested renderPass ie. 'eyePose' = [posX, posY, posZ, rotX, rotY, rotZ, rotW].\n"
    "The second return value is the 'eyeIndex', the index of the eye whose view should be rendered. This would "
    "be 0 for left eye, and 1 for right eye, and could be used to select the target render view via, e.g.,\n"
    "Screen('SelectStereoDrawBuffer', window, eyeIndex);\n"
    "Which 'eyeIndex' corresponds to the first or second 'renderPass', ie., if the left eye should be rendered "
    "first, or if the right eye should be rendered first, depends on the visual scanning order of the device "
    "display panel - is it top to bottom, left to right, or right to left? This function provides that optimized "
    "mapping. Using this function to query the parameters for render setup of an eye can provide a bit more "
    "accuracy in rendering, at the expense of more complex usercode.\n";
    static char seeAlsoString[] = "";

    int handle, renderPass, eye;
    PsychOpenXRDevice *openxr;
    XrTime xrPredictionTime;
    double *outM;
    double predictionTime = DBL_MAX;

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

    // Get eye pose for the renderPass. OpenXR does not provide advantages for seprate render passes,
    // so renderPass to eye mapping is meaningless for quality, and we just set arbitrarily eye = renderPass:
    eye = renderPass;

    // Get optional predictionTime:
    PsychCopyInDoubleArg(3, kPsychArgOptional, &predictionTime);

    // Lock protect openxr->predictedDisplayTime read, and locateXRViews():
    PsychLockMutex(&(openxr->presenterLock));

    // Got optional target time for predicted tracking state? Default to the
    // predicted display time of the next video frame:
    if (predictionTime != DBL_MAX) {
        xrPredictionTime = PsychTimeToXrTime(predictionTime);
    }
    else {
        xrPredictionTime = openxr->predictedDisplayTime;
        predictionTime = XrTimeToPsychTime(xrPredictionTime);
    }

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
