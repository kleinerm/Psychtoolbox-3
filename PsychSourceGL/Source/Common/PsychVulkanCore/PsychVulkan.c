/*
 * PsychToolbox/Source/Common/PsychVulkanCore/PsychVulkan.c
 *
 * PROJECTS: PsychVulkanCore only.
 *
 * AUTHORS:
 *
 * mario.kleiner.de@gmail.com   mk
 *
 * PLATFORMS:   Linux, Windows, macOS.
 *
 * DESCRIPTION:
 *
 * A Psychtoolbox driver for interfacing with the Vulkan graphics rendering API
 * for special purpose display and compute tasks.
 *
 * Copyright (c) 2020 - 2024 Mario Kleiner. Licensed under the MIT license:
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

#include "PsychVulkan.h"
#include "PsychTimeGlue.h"

#if PSYCH_SYSTEM == PSYCH_LINUX
#define VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_DISPLAY_KHR
#define VK_USE_PLATFORM_XLIB_XRANDR_EXT
#endif

#if PSYCH_SYSTEM == PSYCH_WINDOWS
// Define for Vulkan WSI on Windows:
#define VK_USE_PLATFORM_WIN32_KHR

// Defines and includes for DXGI v1.6 low-level access and HDR output query:
#define COBJMACROS 1
#include <initguid.h>
#include <dxgi1_6.h>
#endif

#if PSYCH_SYSTEM == PSYCH_OSX
#include <MoltenVK/vk_mvk_moltenvk.h>
#endif

#include <vulkan/vulkan.h>

#ifndef VK_DRIVER_ID_NVIDIA_PROPRIETARY
#define VK_DRIVER_ID_NVIDIA_PROPRIETARY VK_DRIVER_ID_NVIDIA_PROPRIETARY_KHR
#endif

// Defines from Screen, replicated here for simplicity:
#define kPsychLeft      0
#define kPsychTop       1
#define kPsychRight     2
#define kPsychBottom    3
typedef double PsychRectType[4];

// At most 64 swapchain image buffers, but typically only 2 if possible:
#define MAX_BUFFERS 64

// Number of maximum simultaneously open Vulkan devices:
#define MAX_PSYCH_VULKAN_DEVS 100

// Number of maximum simultaneously open Vulkan output windows:
#define MAX_PSYCH_VULKAN_WINDOWS 100

// Short one-line help texts:
#define MAX_SYNOPSIS_STRINGS 50
static const char *synopsisSYNOPSIS[MAX_SYNOPSIS_STRINGS];

// Our Vulkan device record:
typedef struct PsychVulkanDevice {
    psych_bool                          isValid;
    int                                 deviceIndex;
    VkDevice                            device;
    VkQueue                             graphicsQueue;
    VkCommandPool                       commandPool;
    VkPhysicalDevice                    physicalDevice;
    VkPhysicalDeviceProperties          deviceProps;
    VkPhysicalDeviceIDProperties        physDeviceProps;
    VkPhysicalDeviceDriverPropertiesKHR driverProps;
    psych_bool                          hasHDR;
    psych_bool                          hasHDRLocalDimming;
    psych_bool                          hasTiming;
    psych_bool                          hasWait;
    uint32_t                            graphicsQueueFamilyIndex;
    VkPhysicalDeviceMemoryProperties    memoryProperties;
} PsychVulkanDevice;

#if defined(VK_USE_PLATFORM_WIN32_KHR)
typedef struct _ShareHandles {
    HANDLE memory;
    HANDLE glReady;
    HANDLE glComplete;
} ShareHandles;
#else
typedef struct _ShareHandles {
    int memory;
    int glReady;
    int glComplete;
} ShareHandles;
#endif

typedef struct PsychVulkanWindow {
    psych_bool                          isValid;
    int                                 index;
    PsychVulkanDevice*                  vulkan;
    VkDisplayKHR                        display;
    VkSurfaceKHR                        surface;
    VkSurfaceCapabilitiesKHR            surfaceCapabilities;
    psych_bool                          supports_vsync_FIFO;
    psych_bool                          supports_non_vsync;

    psych_bool                          local_dimming_supported;
    unsigned int                        localDimmmingEnable;
    unsigned int                        nativeDisplayHDRMetadataValidity;
    VkHdrMetadataEXT                    nativeDisplayHDRMetadata;
    psych_bool                          currentDisplayHDRMetadataNeedsCommit;
    VkHdrMetadataEXT                    currentDisplayHDRMetadata;

    uint32_t                            surfaceFormatCount;
    VkSurfaceFormat2KHR*                surfaceFormats;

    psych_bool                          supports_hdr10_rgba16f;
    psych_bool                          supports_hdr10_rgb10a2;
    psych_bool                          supports_hdr10_bgr10a2;

    psych_bool                          supports_scrgb_rgba16f;
    psych_bool                          supports_fs2_hdr;

    psych_bool                          supports_srgb_rgba16f;
    psych_bool                          supports_srgb_rgb10a2;
    psych_bool                          supports_srgb_bgr10a2;

    int                                 width;
    int                                 height;
    uint32_t                            numBuffers;
    psych_bool                          isStereo;
    psych_bool                          isFullscreen;
    int                                 createFlags;
    int                                 hdrMode;
    int                                 colorPrecision;
    VkColorSpaceKHR                     colorspace;
    VkFormat                            format;

    VkImage                             interopImage;
    VkFormat                            interopTextureVkFormat;
    psych_bool                          interopTextureTiled;
    VkDeviceMemory                      interopImageMemory;
    ShareHandles                        interopHandles;
    size_t                              interopMemorysize;

    VkSemaphore                         interopRenderDoneSemaphore;

    unsigned int                        frameIndex;
    VkFence                             flipDoneFence;
    double                              tPresentComplete;
    VkSwapchainKHR                      swapChain;
    VkImage*                            swapChainImages;
    uint32_t                            currentSwapChainBuffer;
    VkSemaphore                         imageAcquiredSemaphores[MAX_BUFFERS];
    VkSemaphore                         imageCopyDoneSemaphores[MAX_BUFFERS];
    VkCommandBuffer                     swapChainCommandBuffers[MAX_BUFFERS];

#if defined(VK_USE_PLATFORM_XLIB_KHR)
    Display*                            connection;
    Window                              x11PrivateWindow;
#endif

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    HWND                                win32PrivateWindow;
#endif
} PsychVulkanWindow;

// Connection to our Windowing system:
#if defined(VK_USE_PLATFORM_WIN32_KHR)
static HINSTANCE connection;            // Application instance handle.
static psych_bool windowClassRegistered = FALSE;
#endif

// Main instance, from which everything else derives:
static VkInstance vulkanInstance = VK_NULL_HANDLE;
static VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

PsychVulkanDevice vulkanDevices[MAX_PSYCH_VULKAN_DEVS];
static uint32_t physicalGpuCount = 0;

PsychVulkanWindow vulkanWindows[MAX_PSYCH_VULKAN_WINDOWS];
static int windowCount = 0;

static int verbosity = 3;
static psych_bool initialized = FALSE;

// Do we require all enumerated gpu's to have a driver with HDR support?
static psych_bool needHDR = FALSE;

static unsigned int preferredDriver = 0; //VK_DRIVER_ID_MESA_RADV_KHR;
static psych_bool isSuitableDriver[4];

// Global count and requested instance extensions:
static uint32_t instanceExtensionsCount = 0;
static unsigned int instanceExtensionsEnabledCount = 0;
static char *instanceExtensionNames[64];

// Global count and requested device extensions for currently processed physical device:
static uint32_t deviceExtensionsCount = 0;
static unsigned int deviceExtensionsEnabledCount = 0;
static char *deviceExtensionNames[64];

static char *layerNames[1];

// Instance extensions:
PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR = NULL;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR = NULL; // TODO remove.
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR = NULL;           // TODO remove.
PFN_vkGetPhysicalDeviceSurfaceFormats2KHR fpGetPhysicalDeviceSurfaceFormats2KHR = NULL;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR = NULL; // TODO remove.
PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR fpGetPhysicalDeviceSurfaceCapabilities2KHR = NULL;

// Device extensions: Could be bound by device instead of globally for the instance, to save a smidgen of
// dispatch overhead from the instance dispatch table to driver-specific entry points. Probably not worth
// it for our use case though:
PFN_vkReleaseDisplayEXT fpReleaseDisplayEXT = NULL;
PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR = NULL;
PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
PFN_vkQueuePresentKHR fpQueuePresentKHR;
PFN_vkGetRefreshCycleDurationGOOGLE fpGetRefreshCycleDurationGOOGLE = NULL;
PFN_vkGetPastPresentationTimingGOOGLE fpGetPastPresentationTimingGOOGLE = NULL;
#ifdef VK_KHR_present_id
PFN_vkWaitForPresentKHR fpWaitForPresentKHR = NULL;
#else
#if PSYCH_SYSTEM != PSYCH_OSX
    // Only warn on non-macOS, on macOS this is unsupported anyway by MoltenVK Vulkan ICD as of MoltenVK 1.2.8 in March 2024.
    #warning VK_KHR_PRESENT_ID extension not supported by ancient build system. Update the Vulkan SDK headers to at least SDK 1.2.189
#endif
#endif
PFN_vkSetHdrMetadataEXT fpSetHdrMetadataEXT = NULL;
PFN_vkSetLocalDimmingAMD fpSetLocalDimmingAMD = NULL;

#if PSYCH_SYSTEM == PSYCH_WINDOWS
PFN_vkAcquireFullScreenExclusiveModeEXT fpAcquireFullScreenExclusiveModeEXT;
PFN_vkGetMemoryWin32HandleKHR fpGetMemoryWin32HandleKHR;
PFN_vkGetSemaphoreWin32HandleKHR fpGetSemaphoreWin32HandleKHR;
#endif

#if PSYCH_SYSTEM == PSYCH_LINUX
PFN_vkGetMemoryFdKHR fpGetMemoryFdKHR;
PFN_vkGetSemaphoreFdKHR fpGetSemaphoreFdKHR;
PFN_vkGetRandROutputDisplayEXT fpGetRandROutputDisplayEXT = NULL;
PFN_vkAcquireXlibDisplayEXT fpAcquireXlibDisplayEXT = NULL;
#endif

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                                                        \
{                                                                                                       \
    fp##entrypoint = (PFN_vk##entrypoint) vkGetInstanceProcAddr(inst, "vk" #entrypoint);                \
    if (fp##entrypoint == NULL) {                                                                       \
        printf("vkGetInstanceProcAddr failed to find vk" #entrypoint, "vkGetInstanceProcAddr Failure"); \
    }                                                                                                   \
}

static void PsychCloseAllVulkanWindows(void);
static psych_bool PsychSetHDRMetaData(PsychVulkanWindow* window);

void InitializeSynopsis(void)
{
    int i = 0;
    const char **synopsis = synopsisSYNOPSIS;

    synopsis[i++] = "PsychVulkanCore - A Psychtoolbox driver for interfacing with the Vulkan graphics rendering API\n";
    synopsis[i++] = "This driver allows to utilize the Vulkan graphics API for special purpose display and compute tasks.";
    synopsis[i++] = "Copyright (c) 2020 - 2024 Mario Kleiner. Licensed to you under the terms of the MIT license.";
    synopsis[i++] = "";
    synopsis[i++] = "This driver is used internally by Psychtoolbox. You should not call its functions";
    synopsis[i++] = "directly as a regular end-user from your scripts, as the API may change at any time";
    synopsis[i++] = "in a backwards incompatible way without any warning! The driver may even go away";
    synopsis[i++] = "completely in a future version of Psychtoolbox if its functionality gets more tightly";
    synopsis[i++] = "integrated into other mex files!";
    synopsis[i++] = "";
    synopsis[i++] = "Functions used internally by Psychtoolbox:";
    synopsis[i++] = "";
    synopsis[i++] = "oldVerbosity = PsychVulkanCore('Verbosity' [, verbosity]);";
    synopsis[i++] = "numDevices = PsychVulkanCore('GetCount');";
    synopsis[i++] = "devices = PsychVulkanCore('GetDevices');";
    synopsis[i++] = "PsychVulkanCore('Close');";
    synopsis[i++] = "vulkanWindow = PsychVulkanCore('OpenWindow', gpuIndex, targetUUID, isFullscreen, screenId, rect, outputHandle, hdrMode, colorPrecision, refreshHz, colorSpace, colorFormat, flags);";
    synopsis[i++] = "PsychVulkanCore('CloseWindow' [, vulkanWindow]);";
    synopsis[i++] = "hdr = PsychVulkanCore('GetHDRProperties', vulkanWindow);";
    synopsis[i++] = "oldlocalDimmmingEnable = PsychVulkanCore('HDRLocalDimming', vulkanWindow [, localDimmmingEnable]);";
    synopsis[i++] = "[interopObjectHandle, allocationSize, formatSpec, tilingMode, memoryOffset, width, height, interopSemaphoreHandle] = PsychVulkanCore('GetInteropHandle', vulkanWindowHandle [, wantSemaphore=0][, eye=0]);";
    synopsis[i++] = "oldHdrMetadata = PsychVulkanCore('HDRMetadata', vulkanWindow, metadataType [, maxFrameAverageLightLevel][, maxContentLightLevel][, minLuminance][, maxLuminance][, colorGamut]);";
    synopsis[i++] = "[tPredictedOnset, frameIndex] = PsychVulkanCore('Present', vulkanWindowHandle [, tWhen=0][, doTimestamp=2]);";
    synopsis[i++] = NULL; // Mark end of synopsis.

    if (i > MAX_SYNOPSIS_STRINGS) {
        PrintfExit("%s: Increase dimension of synopsis[] from %ld to at least %ld and recompile.", __FILE__, (long) MAX_SYNOPSIS_STRINGS, (long) i);
    }
}

PsychError PsychVulkanDisplaySynopsis(void)
{
    int i;

    for (i = 0; synopsisSYNOPSIS[i] != NULL; i++)
        printf("%s\n", synopsisSYNOPSIS[i]);

    return(PsychError_none);
}

PsychVulkanDevice* PsychGetVulkan(int handle, psych_bool dontfail)
{
    if (handle < 1 || handle > MAX_PSYCH_VULKAN_DEVS || handle > physicalGpuCount || !vulkanDevices[handle - 1].isValid) {
        if (!dontfail) {
            printf("PTB-ERROR: Invalid Vulkan device handle %i passed. No such device open.\n", handle);
            PsychErrorExitMsg(PsychError_user, "Invalid Vulkan handle.");
        }

        return(NULL);
    }

    return(&(vulkanDevices[handle - 1]));
}

PsychVulkanWindow* PsychGetVulkanWindow(int handle, psych_bool dontfail)
{
    if ((handle < 1) || (handle > MAX_PSYCH_VULKAN_WINDOWS) || !vulkanWindows[handle - 1].isValid) {
        if (!dontfail) {
            printf("PTB-ERROR: Invalid Vulkan window handle %i passed. No such window open.\n", handle);
            PsychErrorExitMsg(PsychError_user, "Invalid Vulkan window handle.");
        }

        return(NULL);
    }

    return(&(vulkanWindows[handle - 1]));
}

static VkBool32 PsychVulkanLogCB(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    (void) messageTypes;
    (void) pUserData;

    if ((messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT && verbosity > 0) ||
        (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT && verbosity > 1) ||
        (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT && verbosity > 4) ||
        (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT && verbosity > 9)) {
        printf("PsychVulkanCore-Runtime:[%i] %s : %s.\n", pCallbackData->messageIdNumber, pCallbackData->pMessageIdName, pCallbackData->pMessage);
    }

    return(VK_FALSE);
}

static psych_bool addInstanceExtension(VkExtensionProperties* exts, unsigned int extCount, const char* wantedExt)
{
    unsigned int i;

    if (instanceExtensionsEnabledCount >= 64) {
        printf("Instance extension array max capacity of 64 slots reached! Skipping add of '%s'. Recompile!\n", wantedExt);
        return (FALSE);
    }

    for (i = 0; i < extCount; i++) {
        if (!strcmp(wantedExt, exts[i].extensionName)) {
            if (verbosity > 3)
                printf("PsychVulkanCore-INFO: Found and enabled %i. instance extension '%s'.\n", instanceExtensionsEnabledCount, wantedExt);

            instanceExtensionNames[instanceExtensionsEnabledCount++] = (char*) wantedExt;
            return (TRUE);
        }
    }

    if (verbosity > 3)
        printf("PsychVulkanCore-INFO: Extension '%s' is not supported.\n", wantedExt);

    return (FALSE);
}

static psych_bool addDeviceExtension(VkExtensionProperties* exts, unsigned int extCount, const char* wantedExt)
{
    unsigned int i;

    if (deviceExtensionsEnabledCount >= 64) {
        printf("PsychVulkanCore-CRITICAL: Device extension array max capacity of 64 slots reached! Skipping add of '%s'. Recompile!\n", wantedExt);
        return (FALSE);
    }

    for (i = 0; i < extCount; i++) {
        // printf("EXT %i: %s\n", i, exts[i].extensionName);
        if (!strcmp(wantedExt, exts[i].extensionName)) {
            if (verbosity > 3)
                printf("PsychVulkanCore-INFO: Found and enabled %i. device extension '%s'.\n", deviceExtensionsEnabledCount, wantedExt);

            deviceExtensionNames[deviceExtensionsEnabledCount++] = (char*) wantedExt;
            return (TRUE);
        }
    }

    if (verbosity > 3)
        printf("PsychVulkanCore-INFO: Device extension '%s' is not supported.\n", wantedExt);

    return (FALSE);
}

psych_bool checkAndRequestDeviceExtensions(VkPhysicalDevice* gpus, int gpuIndex, psych_bool* hasHDR, psych_bool* hasHDRLocalDimming, psych_bool* hasTiming, psych_bool* hasWait)
{
    int i;
    VkResult result;
    psych_bool rc = FALSE;

    // Reset count of requested device extensions:
    deviceExtensionsEnabledCount = 0;

    // Enumerate device extensions:
    result = vkEnumerateDeviceExtensionProperties(gpus[gpuIndex], NULL, &deviceExtensionsCount, NULL);
    if (result != VK_SUCCESS) {
        if (verbosity > 3)
            printf("PsychVulkanCore-INFO: GPU %i: Skipped, because could not enumerate device extensions (I): %i\n", gpuIndex, result);

        return(FALSE);
    }

    VkExtensionProperties *deviceExtensions = malloc(sizeof(VkExtensionProperties) * deviceExtensionsCount);

    result = vkEnumerateDeviceExtensionProperties(gpus[gpuIndex], NULL, &deviceExtensionsCount, deviceExtensions);
    if (result != VK_SUCCESS) {
        if (verbosity > 3)
            printf("PsychVulkanCore-INFO: GPU %i: Skipped, because could not enumerate device extensions (II): %i\n", gpuIndex, result);

        goto deviceExtensions_out;
    }

    if (verbosity > 4) {
        printf("PsychVulkanCore-INFO: GPU %i: Probing the following %i device extensions against required set.\n", gpuIndex, deviceExtensionsCount);
        for (i = 0; i < deviceExtensionsCount; i++)
            printf("PsychVulkanCore-INFO: %i. device extension is '%s'.\n", i, deviceExtensions[i].extensionName);
    }

    // First request all mandatory extensions we need for any Psychtoolbox use case:
    if (!addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_KHR_SWAPCHAIN_EXTENSION_NAME) ||
        !addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME) ||      // Already in core 1.1
        !addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME) ||   // Already in core 1.1
        !addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_KHR_MAINTENANCE1_EXTENSION_NAME) ||         // Already in core 1.1

        // For Linux OpenGL <-> Vulkan interop:
        #if PSYCH_SYSTEM == PSYCH_LINUX
        !addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME) ||
        !addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME)
        #endif

        #if PSYCH_SYSTEM == PSYCH_WINDOWS
        // For Windows OpenGL <-> Vulkan interop:
        !addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME) ||
        !addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME) ||
        // For Windows fullscreen display with good performance and timing:
        !addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME)
        #endif

        #if PSYCH_SYSTEM == PSYCH_OSX
        // Do need the portability subset device extensions on macOS / MoltenVK:
        !addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)
        #endif
    ) {
        if (verbosity > 3)
            printf("PsychVulkanCore-INFO: GPU %i: Skipped, because at least one mandatory device extension is missing.\n", gpuIndex);

        goto deviceExtensions_out;
    }

    // The HDR metadata extension is a must-have if HDR is requested:
    *hasHDR = addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_EXT_HDR_METADATA_EXTENSION_NAME);

    // The AMD specific extension for native display HDR and local backlight dimming control is optional and only makes
    // sense if we already have basic HDR support:
    if (*hasHDR)
        *hasHDRLocalDimming = addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_AMD_DISPLAY_NATIVE_HDR_EXTENSION_NAME);
    else
        *hasHDRLocalDimming = FALSE;

    if (hasTiming) {
        // The Google display timing extension is a must-have if precise timing is requested:
        *hasTiming = addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME);
    }

    if (hasWait) {
        #ifdef VK_KHR_present_id
        *hasWait = addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_KHR_PRESENT_WAIT_EXTENSION_NAME);
        *hasWait &= addDeviceExtension(deviceExtensions, deviceExtensionsCount, VK_KHR_PRESENT_ID_EXTENSION_NAME);
        #else
        *hasWait = FALSE;
        if (PSYCH_SYSTEM != PSYCH_OSX) {
            static unsigned int oneTimeInfoDone = 0;
            if ((verbosity > 2) && !(oneTimeInfoDone++))
                printf("PsychVulkanCore-INFO: Lacking support for efficient flip-wait. For maximum efficiency, try DownloadPsychtoolbox for upstream PTB.\n");
        }
        #endif
    }

    // Success: Fall through to cleanup with success return code:
    rc = TRUE;

deviceExtensions_out:
    deviceExtensionsCount = 0;
    free(deviceExtensions);

    return(rc);
}

void PsychVulkanCheckInit(psych_bool dontfail)
{
    VkResult result;
    PsychVulkanDevice* vulkan = NULL;

    uint32_t instanceVersion = 0;
    uint32_t probedCount = 0;
    uint32_t graphicsQueueFamilyIndex;
    uint32_t queueFamilyCount;
    int pass;
    int i, j;

    // Already initialized? No op then.
    if (initialized)
        return;

    memset(isSuitableDriver, 0, sizeof(isSuitableDriver));

    // Establish a connection to the host windowing system and get a connection handle:
    #if defined(VK_USE_PLATFORM_WIN32_KHR)
    connection = GetModuleHandle(NULL);
    if (connection == NULL) {
        printf("PsychVulkanCore-ERROR: Failed to get process module handle! Game over!\n");
        goto instance_init_out;
    }
    #endif

    // Enumerate and select all required instance extensions:
    memset(instanceExtensionNames, 0, sizeof(instanceExtensionNames));
    result = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionsCount, NULL);
    if ((result != VK_SUCCESS) || (instanceExtensionsCount == 0)) {
        printf("PsychVulkanCore-ERROR: Failed to enumerate instance extensions I!\n");
        goto instance_init_out;
    }

    if (verbosity >= 4)
        printf("PsychVulkanCore-INFO: At startup there are %i Vulkan instance extensions available.\n", instanceExtensionsCount);

    VkExtensionProperties *instanceExtensions = malloc(sizeof(VkExtensionProperties) * instanceExtensionsCount);

    result = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionsCount, instanceExtensions);
    if (result != VK_SUCCESS) {
        printf("PsychVulkanCore-ERROR: Failed to enumerate instance extensions II!\n");
        goto instance_init_out;
    }

    if (verbosity >= 5) {
        for (i = 0; i < instanceExtensionsCount; i++)
            printf("PsychVulkanCore-INFO: %i. instance extension is '%s'.\n", i, instanceExtensions[i].extensionName);
    }

    // Check if minimum set of required instance extensions exist and enable/request them, if so:
    if (!addInstanceExtension(instanceExtensions, instanceExtensionsCount, VK_KHR_SURFACE_EXTENSION_NAME) ||
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME) ||
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME) ||       // Already in core 1.1
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME) ||    // Already in core 1.1
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) ||   // Already in core 1.1
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) ||

        #if defined(VK_USE_PLATFORM_XLIB_KHR)
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, VK_KHR_XLIB_SURFACE_EXTENSION_NAME) ||
        #endif

        #if defined(VK_USE_PLATFORM_XLIB_XRANDR_EXT)
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, VK_EXT_ACQUIRE_XLIB_DISPLAY_EXTENSION_NAME) ||
        #endif

        #if defined(VK_USE_PLATFORM_DISPLAY_KHR)
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, VK_KHR_DISPLAY_EXTENSION_NAME) ||
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME)
        #endif

        #if defined(VK_USE_PLATFORM_WIN32_KHR)
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, VK_KHR_WIN32_SURFACE_EXTENSION_NAME)
        #endif

        #if PSYCH_SYSTEM == PSYCH_OSX
        !addInstanceExtension(instanceExtensions, instanceExtensionsCount, VK_EXT_METAL_SURFACE_EXTENSION_NAME)
        #endif
    ) {
        printf("PsychVulkanCore-ERROR: At least one required instance extension is missing!\n");

        free(instanceExtensions);
        goto instance_init_out;
    }

    // The swapchain color space extension is optional and has no associated entry points, just defines some additional
    // color spaces. Therefore we don't care if getting it succeeds or not:
    addInstanceExtension(instanceExtensions, instanceExtensionsCount, VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME);

    // Done enumerating instance extensions:
    free(instanceExtensions);

    // Create Vulkan instance which is the mother of all to come:
    VkApplicationInfo appInfo = { 0 };
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Psychtoolbox";
    appInfo.applicationVersion = VK_MAKE_VERSION(PsychGetMajorVersionNumber(), PsychGetMinorVersionNumber(), PsychGetPointVersionNumber());
    appInfo.pEngineName = "PsychVulkanCore";
    appInfo.engineVersion = appInfo.applicationVersion;
    appInfo.apiVersion = VK_API_VERSION_1_1;

    layerNames[0] = "VK_LAYER_KHRONOS_validation";

    VkInstanceCreateInfo createInfo = { 0 };
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = instanceExtensionsEnabledCount;
    createInfo.ppEnabledExtensionNames = (const char *const *) instanceExtensionNames;
    createInfo.enabledLayerCount = (verbosity > 10) ? 1 : 0;
    createInfo.ppEnabledLayerNames = (const char *const *) layerNames;

    // Get Vulkan instance version:
    vkEnumerateInstanceVersion(&instanceVersion);
    if ((VK_VERSION_MAJOR(instanceVersion) == 1) && (VK_VERSION_MINOR(instanceVersion) == 0)) {
        // Only Vulkan 1.0 - Not good enough for us:
        printf("PsychVulkanCore-ERROR: Vulkan instance only supports Vulkan v1.0, not the minimum required v1.1. Game over!\n");
        goto instance_init_out;
    }

    if (vulkanInstance == VK_NULL_HANDLE) {
        result = vkCreateInstance(&createInfo, NULL, &vulkanInstance);
        if (result != VK_SUCCESS) {
            printf("PsychVulkanCore-ERROR: Vulkan instance creation failed: %i\n", result);
            goto instance_init_out;
        }

        if (verbosity >= 3)
            printf("PsychVulkanCore-INFO: Vulkan instance (version %i.%i.%i) created.\n", VK_VERSION_MAJOR(instanceVersion),
                    VK_VERSION_MINOR(instanceVersion), VK_VERSION_PATCH(instanceVersion));
    }

    if (verbosity > 4) {
        PFN_vkCreateDebugUtilsMessengerEXT pfnCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance, "vkCreateDebugUtilsMessengerEXT");

        VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,  // sType
            .pNext = NULL,                                                     // pNext
            .flags = 0,                                                        // flags
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | ((verbosity > 10) ? VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT : 0),
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | ((verbosity > 10) ? VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT : 0),
            .pfnUserCallback = PsychVulkanLogCB,
            .pUserData = NULL,
        };

        result = pfnCreateDebugUtilsMessengerEXT(vulkanInstance, &messengerCreateInfo, NULL, &debugMessenger);
        if (result != VK_SUCCESS) {
            printf("PsychVulkanCore-ERROR: Could not enable debug messenger callback: %i\n", result);
            // goto instance_init_out;
        }
    }

    // Bind entry points for functions used from instance extensions:
    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetPhysicalDeviceSurfaceSupportKHR);
    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetPhysicalDeviceSurfaceCapabilities2KHR);
    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetPhysicalDeviceSurfaceFormatsKHR);
    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetPhysicalDeviceSurfaceFormats2KHR);
    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetPhysicalDeviceSurfacePresentModesKHR);

    // Bind entry points for functions used from device extensions, but use the global instance
    // entry points and query, for simplicity atm.:

    // Swapchain extension:
    GET_INSTANCE_PROC_ADDR(vulkanInstance, CreateSwapchainKHR);
    GET_INSTANCE_PROC_ADDR(vulkanInstance, DestroySwapchainKHR);
    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetSwapchainImagesKHR);
    GET_INSTANCE_PROC_ADDR(vulkanInstance, AcquireNextImageKHR);
    GET_INSTANCE_PROC_ADDR(vulkanInstance, QueuePresentKHR);

    #if PSYCH_SYSTEM == PSYCH_WINDOWS
    // External memory handle extension:
    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetMemoryWin32HandleKHR);
    // External semaphore handle extension:
    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetSemaphoreWin32HandleKHR);

    // Needed to switch a fullscreen window to fullscreen exclusive mode:
    GET_INSTANCE_PROC_ADDR(vulkanInstance, AcquireFullScreenExclusiveModeEXT);
    #endif

    #if PSYCH_SYSTEM == PSYCH_LINUX
    // External memory fd extension:
    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetMemoryFdKHR);
    // External semaphore handle extension:
    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetSemaphoreFdKHR);

    // Needed to switch to a fullscreen direct mode display mode:
    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetRandROutputDisplayEXT);
    GET_INSTANCE_PROC_ADDR(vulkanInstance, AcquireXlibDisplayEXT);
    GET_INSTANCE_PROC_ADDR(vulkanInstance, ReleaseDisplayEXT);
    #endif

    // Enumerate physical devices - actually combos of Vulkan driver + physical device:
    result = vkEnumeratePhysicalDevices(vulkanInstance, &probedCount, NULL);
    if (result != VK_SUCCESS) {
        if (verbosity > 0) {
            printf("PsychVulkanCore-ERROR: Could not enumerate physical Vulkan devices (I): %i\n", result);
            printf("PsychVulkanCore-ERROR: Most likely your system does not support Vulkan for your graphics cards.\n");
        }
        goto instance_init_out;
    }

    if (verbosity >= 4)
        printf("PsychVulkanCore-INFO: At startup there are %i Vulkan devices available.\n", probedCount);

    // Enumerate all available gpu+driver combos:
    if (probedCount > 0) {
        int oldVerbosity = verbosity;

        VkPhysicalDevice *physicalDevices = malloc(sizeof(VkPhysicalDevice) * probedCount);
        result = vkEnumeratePhysicalDevices(vulkanInstance, &probedCount, physicalDevices);
        if (result != VK_SUCCESS) {
            free(physicalDevices);
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: Could not enumerate physical Vulkan devices (II): %i\n", result);
            goto instance_init_out;
        }

        for (pass = 0; pass < 2; pass++) {
            verbosity = (pass == 0) ? 1 : oldVerbosity;

            for (i = 0; i < probedCount; i++) {
                psych_bool hasHDR, hasHDRLocalDimming, hasTiming, hasWait;

                // Get physical device properties:
                // Note: These daisy-chained structs might be useful in the future:
                // VkPhysicalDevicePCIBusInfoPropertiesEXT = PCI domain:bus:device:function of the gpu. [VK_EXT_pci_bus_info]

                // VkPhysicalDeviceIDProperties == Identification data (UUID's) for physical GPU [Vulkan 1.1 core]
                VkPhysicalDeviceIDProperties physDeviceProps = {
                    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES,
                    .pNext = NULL
                };

                VkPhysicalDeviceDriverPropertiesKHR driverprops = {
                    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR,
                    .pNext = &physDeviceProps,
                    .driverID = 0,
                };

                VkPhysicalDeviceProperties2 pdevprops2 = {
                    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
                    .pNext = &driverprops,
                };

                vkGetPhysicalDeviceProperties2(physicalDevices[i], &pdevprops2);

                if (verbosity > 3) {
                    printf("PsychVulkanCore-INFO: GPU %i: '%s' : Vulkan %i.%i.%i : VendorId 0x%x : DeviceId 0x%x : ",
                        i, pdevprops2.properties.deviceName,
                        VK_VERSION_MAJOR(pdevprops2.properties.apiVersion), VK_VERSION_MINOR(pdevprops2.properties.apiVersion),
                        VK_VERSION_PATCH(pdevprops2.properties.apiVersion), pdevprops2.properties.vendorID, pdevprops2.properties.deviceID);

                    switch (pdevprops2.properties.deviceType) {
                        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                            printf("Integrated graphics chip.\n");
                            break;

                        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                            printf("Discrete graphics card.\n");
                            break;

                        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                            printf("Virtual graphics card.\n");
                            break;

                        case VK_PHYSICAL_DEVICE_TYPE_CPU:
                            printf("Software renderer.\n");
                            break;

                        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                        default:
                            printf("Unknown gpu type.\n");
                            break;
                    }

                    // Driver info available? Needs VK_KHR_driver_properties device extension or Vulkan 1.2.
                    if (driverprops.driverID > 0) {
                        printf("PsychVulkanCore-INFO: GPU %i: Driver '%s' [%s] : Driver version 0x%x\n", i, driverprops.driverName,
                               driverprops.driverInfo, pdevprops2.properties.driverVersion);
                    }
                }

                // Skip devices that are unsuitable for us:

                // Only Vulkan 1.0 supported? Reject.
                if ((VK_VERSION_MAJOR(pdevprops2.properties.apiVersion) == 1) && (VK_VERSION_MINOR(pdevprops2.properties.apiVersion) == 0)) {
                    if (verbosity > 3)
                        printf("PsychVulkanCore-INFO: GPU %i: Skipped, because only supports Vulkan v1.0, not the minimum required v1.1.\n", i);
                    continue;
                }

                // Not a integrated or discrete gpu, but a software renderer or virtualized gpu?
                // Not suitable for stimulus display at all (no display connected) or at least not for high performance display
                // with proper timing and precision -> Reject:
                if ((pdevprops2.properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) &&
                    (pdevprops2.properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)) {
                    if (verbosity > 3)
                        printf("PsychVulkanCore-INFO: GPU %i: Skipped, because not a suitable standard discrete or integrated gpu.\n", i);
                    continue;
                }

                // Check if minimum set of required device extensions is available on this gpu. This will also check if
                // HDR extensions and timing extensions are available, report back in hasHDR and hasTiming if they are
                // supported, and enable/request them if they are supported:
                if (!checkAndRequestDeviceExtensions(physicalDevices, i, &hasHDR, &hasHDRLocalDimming, &hasTiming, &hasWait))
                    continue;

                if (needHDR && !hasHDR) {
                    if (verbosity > 3)
                        printf("PsychVulkanCore-INFO: GPU %i: Skipped, because does not support HDR as required upon user-code request.\n", i);
                    continue;
                }

                // Query queue family properties for the GPU:
                vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, NULL);
                VkQueueFamilyProperties* queueProps = (VkQueueFamilyProperties*) malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
                vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, queueProps);

                // Try to find a queue family with graphics queues:
                graphicsQueueFamilyIndex = 0xffffffff;
                for (j = 0; j < queueFamilyCount; j++) {
                    if (queueProps[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                        graphicsQueueFamilyIndex = j;
                        break;
                    }
                }

                free(queueProps);

                if (graphicsQueueFamilyIndex == 0xffffffff) {
                    if (verbosity > 3)
                        printf("PsychVulkanCore-INFO: GPU %i: Skipped, because does not support graphics queue family.\n", i);
                    continue;
                }

                // Ok, this is a suitable driver + gpu combo. Any user preferences if multiple suitable drivers exist for the same gpu vendor?
                if (driverprops.driverID > 0) {
                    // We can identify the kind of driver. Let's see if it matches set preferences:
                    switch (driverprops.driverID) {
                        case VK_DRIVER_ID_AMD_PROPRIETARY_KHR:
                        case VK_DRIVER_ID_AMD_OPEN_SOURCE_KHR:
                        case VK_DRIVER_ID_MESA_RADV_KHR:
                            // Driver for an AMD gpu. User choice decides:
                            if (pass == 0) {
                                // Mark this driver as suitable for the task at hand:
                                isSuitableDriver[driverprops.driverID] = TRUE;
                                // Next enumerated gpu + driver combo...
                                continue;
                            } else {
                                // Pass 2, we know already which isSuitableDriver's are available.
                                if ((preferredDriver > 0) && isSuitableDriver[preferredDriver] && (driverprops.driverID != preferredDriver)) {
                                    // A preferredDriver is specified by user code and the preferredDriver is suitable and this one
                                    // is not it. Therefore skip this one.
                                    if (verbosity > 3)
                                        printf("PsychVulkanCore-INFO: GPU %i: Skipped, because '%s' is not the user-code preferred driver.\n", i, driverprops.driverName);

                                    continue;
                                }
                            }
                            break;

                        default:
                            // Non AMD gpu. There is only one driver for a gpu, so just skip forward to pass 1:
                            if (pass == 0)
                                continue;
                    }
                } else {
                    // Skip forward to pass 1 if we don't have driver preferences handling:
                    if (pass == 0)
                        continue;
                }

                if (pass == 0)
                    printf("IMPLEMENTATION BUG: Reached code in pass 0 which should only be reachable in pass 1 !!!\n");

                // Basic HDR support:
                if (hasHDR && !fpSetHdrMetadataEXT)
                    GET_INSTANCE_PROC_ADDR(vulkanInstance, SetHdrMetadataEXT);

                // AMD specific local backlight dimming control suppport on FreeSync2:
                if (hasHDRLocalDimming && !fpSetLocalDimmingAMD)
                    GET_INSTANCE_PROC_ADDR(vulkanInstance, SetLocalDimmingAMD);

                // Enhanced presentation timing support via VK_GOOGLE_DISPLAY_TIMING extension:
                if (hasTiming && (!fpGetRefreshCycleDurationGOOGLE || !fpGetPastPresentationTimingGOOGLE)) {
                    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetRefreshCycleDurationGOOGLE);
                    GET_INSTANCE_PROC_ADDR(vulkanInstance, GetPastPresentationTimingGOOGLE);
                }

                #ifdef VK_KHR_present_id
                // Ability to wait for present completion via VK_KHR_present_wait extension:
                if (hasWait && !fpWaitForPresentKHR)
                    GET_INSTANCE_PROC_ADDR(vulkanInstance, WaitForPresentKHR);
                #endif

                // Add a record about this GPU's basic properties:
                vulkan = &(vulkanDevices[physicalGpuCount]);
                vulkan->deviceIndex = physicalGpuCount + 1;
                vulkan->physicalDevice = physicalDevices[i];
                vulkan->deviceProps = pdevprops2.properties;
                vulkan->physDeviceProps = physDeviceProps;
                vulkan->driverProps = driverprops;
                vulkan->hasHDR = hasHDR;
                vulkan->hasHDRLocalDimming = hasHDRLocalDimming;
                vulkan->hasTiming = hasTiming;
                vulkan->hasWait = hasWait;
                vulkan->graphicsQueueFamilyIndex = graphicsQueueFamilyIndex;

                // Create logical vulkan device -- the "gpu" for our physical device:
                {
                    float priority = 1.0;
                    VkDeviceQueueCreateInfo queueCreateInfo = {
                        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                        .pNext = NULL,
                        .queueFamilyIndex = vulkan->graphicsQueueFamilyIndex,
                        .queueCount = 1,
                        .pQueuePriorities = &priority,
                        .flags = 0,
                    };

                    #ifdef VK_KHR_present_id
                    VkPhysicalDevicePresentIdFeaturesKHR presentIdFeatureEnable = {
                        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR,
                        .pNext = NULL,
                        .presentId = VK_TRUE,
                    };

                    VkPhysicalDevicePresentWaitFeaturesKHR waitFeatureEnable = {
                        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR,
                        .pNext = &presentIdFeatureEnable,
                        .presentWait = VK_TRUE,
                    };
                    #else
                    void *waitFeatureEnable = NULL;
                    #endif

                    VkDeviceCreateInfo deviceCreateInfo = {
                        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                        .pNext = (hasWait) ? &waitFeatureEnable : NULL,             // TODO: Add support for VK_EXT_global_priority ?
                        .queueCreateInfoCount = 1,
                        .pQueueCreateInfos = &queueCreateInfo,
                        .enabledLayerCount = (verbosity > 10) ? 1 : 0,              // Deprecated since v1.1, ignored.
                        .ppEnabledLayerNames = (const char *const *) layerNames,    // Deprecated since v1.1, ignored.
                        .enabledExtensionCount = deviceExtensionsEnabledCount,
                        .ppEnabledExtensionNames = (const char *const *) deviceExtensionNames,
                        .pEnabledFeatures = NULL,
                    };

                    result = vkCreateDevice(physicalDevices[i], &deviceCreateInfo, NULL, &vulkan->device);
                    if (result != VK_SUCCESS) {
                        if (verbosity > 3)
                            printf("PsychVulkanCore-INFO: GPU %i: Skipped, because vkCreateDevice() failed: %i\n", i, result);

                        continue;
                    }

                    // Bind device extensions:
                    // TODO: For the time being we bind the global instance entry points. I doubt this has any
                    // measureable performance impact for our use case, so using device specific binding may be
                    // overkill.

                    // Get the command queue we will use:
                    vkGetDeviceQueue(vulkan->device, vulkan->graphicsQueueFamilyIndex, 0, &vulkan->graphicsQueue);

                    // Create command buffer pool:
                    VkCommandPoolCreateInfo poolInfo = {
                        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                        .pNext = NULL,
                        .queueFamilyIndex = vulkan->graphicsQueueFamilyIndex,
                        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                    };

                    result = vkCreateCommandPool(vulkan->device, &poolInfo, NULL, &vulkan->commandPool);
                    if (result != VK_SUCCESS) {
                        if (verbosity > 3)
                            printf("PsychVulkanCore-INFO: GPU %i: Skipped, because vkCreateCommandPool() failed: %i\n", i, result);

                        vkDestroyDevice(vulkan->device, NULL);

                        continue;
                    }

                }

                // Get Memory information and properties
                vkGetPhysicalDeviceMemoryProperties(vulkan->physicalDevice, &vulkan->memoryProperties);

                // Make it valid:
                vulkan->isValid = TRUE;

                // Commit this new Vulkan device:
                physicalGpuCount++;

                // If we made it to here then this gpu i must be added to the list of selectable gpus:
                if (verbosity > 3)
                    printf("PsychVulkanCore-INFO: GPU %i: ACCEPTED AND ACTIVATED! Count now %i\n", i, physicalGpuCount);

                if (physicalGpuCount >= MAX_PSYCH_VULKAN_DEVS) {
                    printf("PsychVulkanCore-ERROR: Reached maximum supported count %i of simultaneously open Vulkan devices! Aborting enumeration.\n", MAX_PSYCH_VULKAN_DEVS);
                    break;
                }
            }
        }
    }

    initialized = TRUE;

    return;

instance_init_out:

    deviceExtensionsEnabledCount = 0;
    instanceExtensionsEnabledCount = 0;

    if (!dontfail) {
        PsychErrorExitMsg(PsychError_system, "PsychVulkanCore-ERROR: Initialization of Vulkan API failed. Driver disabled!");
    }
}

// "Does nothing, goes nowhere" so far...
void PsychVulkanClose(int handle)
{
    PsychVulkanDevice* vulkan;
    vulkan = PsychGetVulkan(handle, TRUE);
    if (NULL == vulkan)
        return;
}

void PsychVulkanInit(void) {
    int handle;

    memset(&vulkanDevices, 0, sizeof(vulkanDevices));
    physicalGpuCount = 0;

    memset(&vulkanWindows, 0, sizeof(vulkanWindows));
    windowCount = 0;

    vulkanInstance = VK_NULL_HANDLE;

    initialized = FALSE;
}

PsychError PsychVulkanShutDown(void) {
    int handle, i;

    if (initialized) {
        // Close all open Vulkan windows and release their associated resources:
        PsychCloseAllVulkanWindows();

        // TODO FIXME
        // for (handle = 0; handle < MAX_PSYCH_VULKAN_DEVS; handle++)
        //    PsychVulkanClose(handle);

        // Destroy all Vulkan devices:
        for (i = 0; i < physicalGpuCount; i++) {
            vkDeviceWaitIdle(vulkanDevices[i].device);
            vkDestroyCommandPool(vulkanDevices[i].device, vulkanDevices[i].commandPool, NULL);
            vkDestroyDevice(vulkanDevices[i].device, NULL);
        }

        physicalGpuCount = 0;
        windowCount = 0;
        instanceExtensionsEnabledCount = 0;

        // Disable error callback:
        PFN_vkDestroyDebugUtilsMessengerEXT pfnDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (debugMessenger != VK_NULL_HANDLE)
            pfnDestroyDebugUtilsMessengerEXT(vulkanInstance, debugMessenger, NULL);
        debugMessenger = VK_NULL_HANDLE;

        // Destroy our Vulkan instance:
        vkDestroyInstance(vulkanInstance, NULL);
        vulkanInstance = VK_NULL_HANDLE;

        if (verbosity >= 4)
            printf("PsychVulkanCore-INFO: Vulkan instance destruction and driver shutdown complete.\n");

        // Destroy our Windowing system connection:
        #if defined(VK_USE_PLATFORM_WIN32_KHR)
            if (windowClassRegistered) {
                UnregisterClass("PTB-Vulkan", connection);
                windowClassRegistered = FALSE;
            }

            connection = 0;
        #endif

        initialized = FALSE;
    }

    return(PsychError_none);
}

#if PSYCH_SYSTEM == PSYCH_WINDOWS
VkSurfaceFullScreenExclusiveWin32InfoEXT fullscreenExclusiveInfoWin32 = {
    .sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT,
    .pNext = NULL,
};

VkSurfaceFullScreenExclusiveInfoEXT fullscreenExclusiveInfo = {
    .sType = VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT,
    .pNext = &fullscreenExclusiveInfoWin32,
    .fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT,
};

void PsychInitFullScreenExlusiveStructs(PsychVulkanWindow* window)
{
    fullscreenExclusiveInfoWin32.hmonitor = MonitorFromWindow(window->win32PrivateWindow, MONITOR_DEFAULTTOPRIMARY);
    if (window->isFullscreen && !(window->createFlags & 0x2))
        fullscreenExclusiveInfo.fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT;
    else
        fullscreenExclusiveInfo.fullScreenExclusive = VK_FULL_SCREEN_EXCLUSIVE_DISALLOWED_EXT;
}

void PsychMSDXGIQueryOutputHDR(PsychVulkanWindow* window, PsychVulkanDevice* vulkan)
{
    // Could not query HDR display properties via Vulkan on Windows. Try to
    // use DXGI v1.6 on Windows-10 instead:
    IDXGIFactory4 *dxgiFactory = NULL;

    // Try to create DXGI factory:
    if (S_OK != CreateDXGIFactory2(0, &IID_IDXGIFactory4, (void**) &dxgiFactory)) {
        if (verbosity > 1)
            printf("PsychVulkanCore-WARNING: Failed to create a DXGI factory for HDR display properties fallback query!\n");
    }
    else {
        DXGI_ADAPTER_DESC1 desc1;
        IDXGIAdapter1 *dxgiAdapter = NULL;
        unsigned int index = 0;

        // Enumerate all DXGI adapters, check for matching one for our Vulkan/OpenGL gpu:
        while ((S_OK == IDXGIFactory4_EnumAdapters1(dxgiFactory, index, &dxgiAdapter)) && (S_OK == IDXGIAdapter1_GetDesc1(dxgiAdapter, &desc1))) {
            if (verbosity > 4)
                printf("PsychVulkanCore-DEBUG: Name of DXGI adapter %i is '%S'.\n", index, (wchar_t*) desc1.Description);

            // LUID valid and match between Vulkan and DXGI?
            if (vulkan->physDeviceProps.deviceLUIDValid && !memcmp(&desc1.AdapterLuid, &vulkan->physDeviceProps.deviceLUID[0], sizeof(desc1.AdapterLuid)))
                break;

            // Wrong DXGI gpu, not the same as Vulkan/OpenGL gpu:
            IDXGIAdapter1_Release(dxgiAdapter);
            dxgiAdapter = NULL;
            index++;
        }

        if (dxgiAdapter) {
            DXGI_OUTPUT_DESC outDesc;
            DXGI_OUTPUT_DESC1 outDesc1;
            IDXGIOutput *dxgiOutput = NULL;
            index = 0;

            while ((S_OK == IDXGIAdapter1_EnumOutputs(dxgiAdapter, index, &dxgiOutput)) && (S_OK == IDXGIOutput_GetDesc(dxgiOutput, &outDesc))) {
                if (outDesc.Monitor == fullscreenExclusiveInfoWin32.hmonitor) {
                    break;
                }

                IDXGIOutput_Release(dxgiOutput);
                dxgiOutput = NULL;
                index++;
            }

            // Found proper DXGI output? Able to query its HDR properties?
            if (dxgiOutput) {
                if (S_OK == IDXGIOutput6_GetDesc1((IDXGIOutput6*) dxgiOutput, &outDesc1)) {
                    if (verbosity > 3)
                        printf("PsychVulkanCore-DEBUG: Getting HDR properties via DXGI from output %i '%S':\n", index, outDesc1.DeviceName);

                    window->nativeDisplayHDRMetadata.displayPrimaryRed.x = outDesc1.RedPrimary[0];
                    window->nativeDisplayHDRMetadata.displayPrimaryRed.y = outDesc1.RedPrimary[1];

                    window->nativeDisplayHDRMetadata.displayPrimaryGreen.x = outDesc1.GreenPrimary[0];
                    window->nativeDisplayHDRMetadata.displayPrimaryGreen.y = outDesc1.GreenPrimary[1];

                    window->nativeDisplayHDRMetadata.displayPrimaryBlue.x = outDesc1.BluePrimary[0];
                    window->nativeDisplayHDRMetadata.displayPrimaryBlue.y = outDesc1.BluePrimary[1];

                    window->nativeDisplayHDRMetadata.whitePoint.x = outDesc1.WhitePoint[0];
                    window->nativeDisplayHDRMetadata.whitePoint.y = outDesc1.WhitePoint[1];

                    window->nativeDisplayHDRMetadata.minLuminance = outDesc1.MinLuminance;
                    window->nativeDisplayHDRMetadata.maxLuminance = outDesc1.MaxLuminance;
                    window->nativeDisplayHDRMetadata.maxFrameAverageLightLevel = outDesc1.MaxFullFrameLuminance;
                    window->nativeDisplayHDRMetadata.maxContentLightLevel = 0.0;

                    // Mark HDR data as valid:
                    window->nativeDisplayHDRMetadataValidity = 1;
                }

                IDXGIOutput_Release(dxgiOutput);
                dxgiOutput = NULL;
            }

            IDXGIAdapter1_Release(dxgiAdapter);
            dxgiAdapter = NULL;
        }

        IDXGIFactory4_Release(dxgiFactory);
        dxgiFactory = NULL;
    }
}
#else
void PsychInitFullScreenExlusiveStructs(PsychVulkanWindow* window) {}
#endif

psych_bool PsychIsColorSpaceFormatComboSupported(PsychVulkanWindow* window, VkColorSpaceKHR space, VkFormat format)
{
    int i;

    for (i = 0; i < window->surfaceFormatCount; i++) {
        if ((window->surfaceFormats[i].surfaceFormat.colorSpace == space) &&
            (window->surfaceFormats[i].surfaceFormat.format == format))
            return(TRUE);
    }

    return(FALSE);
}

psych_bool PsychProbeSurfaceProperties(PsychVulkanWindow* window, PsychVulkanDevice* vulkan)
{
    VkResult result;
    int i;

    // Clean up / Reset from previous call if needed:
    if (window->surfaceFormats) {
        free(window->surfaceFormats);
        window->surfaceFormats = NULL;
        window->surfaceFormatCount = 0;
    }

    window->supports_hdr10_rgba16f = FALSE;
    window->supports_hdr10_rgb10a2 = FALSE;
    window->supports_hdr10_bgr10a2 = FALSE;
    window->supports_scrgb_rgba16f = FALSE;
    window->supports_fs2_hdr = FALSE;
    window->supports_srgb_rgba16f = FALSE;
    window->supports_srgb_rgb10a2 = FALSE;
    window->supports_srgb_bgr10a2 = FALSE;

    memset(&window->nativeDisplayHDRMetadata, 0, sizeof(window->nativeDisplayHDRMetadata));
    window->nativeDisplayHDRMetadata.sType = VK_STRUCTURE_TYPE_HDR_METADATA_EXT;

    memset(&window->surfaceCapabilities, 0, sizeof(window->surfaceCapabilities));
    window->local_dimming_supported = FALSE;

    window->supports_vsync_FIFO = FALSE;
    window->supports_non_vsync = FALSE;

    const VkPhysicalDeviceSurfaceInfo2KHR surfaceInfo = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR,
        .surface = window->surface,
        #if PSYCH_SYSTEM == PSYCH_WINDOWS
        .pNext = (window->isFullscreen && !(window->createFlags & 0x2)) ? &fullscreenExclusiveInfo : NULL,
        #endif
    };

    VkDisplayNativeHdrSurfaceCapabilitiesAMD nativeHDRCapabilitiesAMD = {
        .sType = VK_STRUCTURE_TYPE_DISPLAY_NATIVE_HDR_SURFACE_CAPABILITIES_AMD,
        .localDimmingSupport = VK_FALSE,
        // amdvlk as of 2020-Q2-4 on Linux crashes in windowed mode if we set this non-NULL, because of a driver bug, so be careful to not provoke this:
        // Note: This has been fixed as of amdvlk 2020-Q2-6, so no crash anymore in windowed mode, but returns some hard-coded values instead of
        // real monitor data, so enabling it again is currently of little additional value.
        // Also skip for non-AMD on MS-Windows, otherwise we get validation warnings.
        .pNext = (((PSYCH_SYSTEM == PSYCH_WINDOWS) && (vulkan->driverProps.driverID == VK_DRIVER_ID_AMD_PROPRIETARY_KHR)) || (window->display != VK_NULL_HANDLE)) ? &window->nativeDisplayHDRMetadata : NULL,
    };

    VkSurfaceCapabilities2KHR surfaceCapabilities2 = {
        .sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR,
        .pNext = &nativeHDRCapabilitiesAMD,
    };

    result = fpGetPhysicalDeviceSurfaceCapabilities2KHR(vulkan->physicalDevice, &surfaceInfo, &surfaceCapabilities2);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: Failed to get vkGetPhysicalDeviceSurfaceCapabilities2KHR: %i\n", result);

        return(FALSE);
    }

    window->local_dimming_supported = nativeHDRCapabilitiesAMD.localDimmingSupport;
    window->surfaceCapabilities = surfaceCapabilities2.surfaceCapabilities;

    // Check validity of queried display HDR properties:
    {
        window->nativeDisplayHDRMetadataValidity = 0;
        psych_uint8* bp = (psych_uint8*) &window->nativeDisplayHDRMetadata;

        for (i = 0; i < sizeof(window->nativeDisplayHDRMetadata); i++) {
            window->nativeDisplayHDRMetadataValidity += (unsigned int) *(bp++);
        }

        // Greater 355 sum over struct suggests at least some valid data was returned, so
        // mark it as valid (==1). Value 355 is what we would get with a content-free all
        // zero struct, where only the .sType is defined as VK_STRUCTURE_TYPE_HDR_METADATA_EXT:
        window->nativeDisplayHDRMetadataValidity = (window->nativeDisplayHDRMetadataValidity > 355) ? 1 : 0;

        // Missing HDR data, because Vulkan driver didn't provide any? Or buggy AMD Vulkan proprietary driver on MS-Windows?
        if (!window->nativeDisplayHDRMetadataValidity ||
            ((window->nativeDisplayHDRMetadata.maxFrameAverageLightLevel == 0) && (vulkan->driverProps.driverID == VK_DRIVER_ID_AMD_PROPRIETARY_KHR))) {
            // Try OS specific fallback to get (better - less buggy) display HDR properties:
            #if PSYCH_SYSTEM == PSYCH_WINDOWS
                PsychMSDXGIQueryOutputHDR(window, vulkan);
            #endif
        }

        // Invalid / Absurd white-point chromaticity coordinates way lower than around 0.3 for D65?
        if (window->nativeDisplayHDRMetadataValidity &&
            (window->nativeDisplayHDRMetadata.whitePoint.x < 0.15) && (window->nativeDisplayHDRMetadata.whitePoint.y < 0.15)) {
            // At least AMD written Linux drivers (ie. amdgpu-pro and amdvlk as of amdvlk 2020-Q2.6 (driverVersion 8388758 aka VK_MAKE_VERSION(2,0,150)))
            // have broken HDR color gamut reporting again. This time all reported chromaticity coordinates are too small by a factor of 5x.
            // Work around this by multiplying all coordinates by a factor of 5.0. This has been fixed in driverVersion 8388763, v2.0.155 aka 2020-Q3.4:
            if ((PSYCH_SYSTEM == PSYCH_LINUX) && (vulkan->deviceProps.driverVersion < 8388763) &&
                (vulkan->driverProps.driverID == VK_DRIVER_ID_AMD_OPEN_SOURCE_KHR || vulkan->driverProps.driverID == VK_DRIVER_ID_AMD_PROPRIETARY_KHR)) {
                window->nativeDisplayHDRMetadata.displayPrimaryRed.x *= 5.0;
                window->nativeDisplayHDRMetadata.displayPrimaryRed.y *= 5.0;

                window->nativeDisplayHDRMetadata.displayPrimaryGreen.x *= 5.0;
                window->nativeDisplayHDRMetadata.displayPrimaryGreen.y *= 5.0;

                window->nativeDisplayHDRMetadata.displayPrimaryBlue.x *= 5.0;
                window->nativeDisplayHDRMetadata.displayPrimaryBlue.y *= 5.0;

                window->nativeDisplayHDRMetadata.whitePoint.x *= 5.0;
                window->nativeDisplayHDRMetadata.whitePoint.y *= 5.0;

                if (verbosity > 1)
                    printf("PsychVulkanCore-WARNING: Buggy AMD Vulkan driver reports wrong (too small) color gamut. Hopefully fixing this by scaling up with a factor of 5.0x.\n");
            }
        }
    }

    // Initialize currentDisplayHDRMetadata, ie. what we would set when switching to HDR mode or
    // changing HDR info packets, to the displays nativeDisplayHDRMetadata, as a good startup
    // default, except for the content light levels which we set to zero, which signals to the
    // display that they are unknown. This because during dynamic stimulus rendering we really
    // do not know in advance what those maximum values over a whole session will be. Usercode
    // can always override for known content:
    window->currentDisplayHDRMetadata = window->nativeDisplayHDRMetadata;
    window->currentDisplayHDRMetadata.maxFrameAverageLightLevel = 0;
    window->currentDisplayHDRMetadata.maxContentLightLevel = 0;

    if (verbosity > 3) {
        printf("Surface minImageCount %i - maxImageCount %i\n", window->surfaceCapabilities.minImageCount, window->surfaceCapabilities.maxImageCount);
        printf("Surface currentExtent %i x %i pixels with up to %i view layers\n", window->surfaceCapabilities.currentExtent.width, window->surfaceCapabilities.currentExtent.height, window->surfaceCapabilities.maxImageArrayLayers);
        printf("Display native HDR properties as queried from monitor:\n");
        printf("Display Supports control of HDR local dimming: %s\n", nativeHDRCapabilitiesAMD.localDimmingSupport ? "Yes" : "No");
        printf("Display Gamut  R: [%f, %f]\n", window->nativeDisplayHDRMetadata.displayPrimaryRed.x, window->nativeDisplayHDRMetadata.displayPrimaryRed.y);
        printf("Display Gamut  G: [%f, %f]\n", window->nativeDisplayHDRMetadata.displayPrimaryGreen.x, window->nativeDisplayHDRMetadata.displayPrimaryGreen.y);
        printf("Display Gamut  B: [%f, %f]\n", window->nativeDisplayHDRMetadata.displayPrimaryBlue.x, window->nativeDisplayHDRMetadata.displayPrimaryBlue.y);
        printf("Display Gamut WP: [%f, %f]\n", window->nativeDisplayHDRMetadata.whitePoint.x, window->nativeDisplayHDRMetadata.whitePoint.y);
        printf("Display minLuminance: %f nits\n", window->nativeDisplayHDRMetadata.minLuminance);
        printf("Display maxLuminance: %f nits\n", window->nativeDisplayHDRMetadata.maxLuminance);
        printf("Content maxFrameAverageLightLevel: %f nits\n", window->nativeDisplayHDRMetadata.maxFrameAverageLightLevel);
        printf("Content maxContentLightLevel: %f nits\n", window->nativeDisplayHDRMetadata.maxContentLightLevel);
    }

    // Get the list of VkFormat's that are supported:
    result = fpGetPhysicalDeviceSurfaceFormats2KHR(vulkan->physicalDevice, &surfaceInfo, &window->surfaceFormatCount, NULL);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: Failed to vkGetPhysicalDeviceSurfaceFormats2KHR: %i\n", result);

        return(FALSE);
    }

    window->surfaceFormats = (VkSurfaceFormat2KHR*) malloc(window->surfaceFormatCount * sizeof(VkSurfaceFormat2KHR));
    for (i = 0; i < window->surfaceFormatCount; i++) {
        window->surfaceFormats[i].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
        window->surfaceFormats[i].pNext = NULL;
    }

    result = fpGetPhysicalDeviceSurfaceFormats2KHR(vulkan->physicalDevice, &surfaceInfo, &window->surfaceFormatCount, window->surfaceFormats);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: Failed to vkGetPhysicalDeviceSurfaceFormats2KHR II for %i formats: %i\n", window->surfaceFormatCount, result);

        free(window->surfaceFormats);
        window->surfaceFormats = NULL;
        window->surfaceFormatCount = 0;
        return(FALSE);
    }

    if (verbosity > 3)
        printf("Surface number of supported surface colorspace + pixelformat combinations: %i\n", window->surfaceFormatCount);

    // Use fallback format BGRA8 unorm if that is all that is supported:
    if (window->surfaceFormatCount == 1 && window->surfaceFormats[0].surfaceFormat.format == VK_FORMAT_UNDEFINED) {
        window->surfaceFormats[0].surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
    }
    else {
        // More colorspace + format combos available. Enumerate:

        // Try to get RGBA16F float, then RGB10A2 formats as second choice, then
        // a fallback to default RGBA8:
        for (i = 0; i < window->surfaceFormatCount; i++) {
            if (verbosity > 3) {
                if (window->surfaceFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_DOLBYVISION_EXT)
                    printf("[%i] For colorspace VK_COLOR_SPACE_DOLBYVISION_EXT          - ", i);

                if (window->surfaceFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT)
                    printf("[%i] For colorspace VK_COLOR_SPACE_HDR10_ST2084_EXT         - ", i);

                if (window->surfaceFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_HDR10_HLG_EXT)
                    printf("[%i] For colorspace VK_COLOR_SPACE_HDR10_HLG_EXT            - ", i);

                if (window->surfaceFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_BT2020_LINEAR_EXT)
                    printf("[%i] For colorspace VK_COLOR_SPACE_BT2020_LINEAR_EXT        - ", i);

                if (window->surfaceFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_DISPLAY_NATIVE_AMD)
                    printf("[%i] For colorspace VK_COLOR_SPACE_DISPLAY_NATIVE_AMD       - ", i);

                if (window->surfaceFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_PASS_THROUGH_EXT)
                    printf("[%i] For colorspace VK_COLOR_SPACE_PASS_THROUGH_EXT         - ", i);

                if (window->surfaceFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT)
                    printf("[%i] For colorspace VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT - ", i);

                if (window->surfaceFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_BT709_NONLINEAR_EXT)
                    printf("[%i] For colorspace VK_COLOR_SPACE_BT709_NONLINEAR_EXT      - ", i);

                if (window->surfaceFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    printf("[%i] For colorspace VK_COLOR_SPACE_SRGB_NONLINEAR_KHR       - ", i);

                switch (window->surfaceFormats[i].surfaceFormat.format) {
                    case VK_FORMAT_R16G16B16A16_SFLOAT:
                        printf("[%i] Swapchain format VK_FORMAT_R16G16B16A16_SFLOAT\n", i);
                        break;

                    case VK_FORMAT_R16G16B16A16_UNORM:
                        printf("[%i] Swapchain format VK_FORMAT_R16G16B16A16_UNORM\n", i);
                        break;

                    case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
                        printf("[%i] Swapchain format VK_FORMAT_A2R10G10B10_UNORM_PACK32\n", i);
                        break;

                    case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
                        printf("[%i] Swapchain format VK_FORMAT_A2B10G10R10_UNORM_PACK32\n", i);
                        break;

                    case VK_FORMAT_B8G8R8A8_SRGB:
                        printf("[%i] Swapchain format VK_FORMAT_B8G8R8A8_SRGB\n", i);
                        break;

                    case VK_FORMAT_B8G8R8A8_UNORM:
                        printf("[%i] Swapchain format VK_FORMAT_B8G8R8A8_UNORM\n", i);
                        break;

                    case VK_FORMAT_R8G8B8A8_UNORM:
                        printf("[%i] Swapchain format VK_FORMAT_R8G8B8A8_UNORM\n", i);
                        break;

                    case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
                        printf("[%i] Swapchain format VK_FORMAT_A8B8G8R8_SRGB_PACK32\n", i);
                        break;

                    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
                        printf("[%i] Swapchain format VK_FORMAT_A8B8G8R8_UNORM_PACK32\n", i);
                        break;

                    default:
                        printf("[%i] Swapchain format unknown %d\n", i, window->surfaceFormats[i].surfaceFormat.format);
                }
            }

            // Standard HDR10 BT2020 colorspace with ST2084 PQ "Perceptual Quantizer" OETF supported?
            if (window->surfaceFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT) {
                // Yes. Which pixel format?
                if (window->surfaceFormats[i].surfaceFormat.format == VK_FORMAT_R16G16B16A16_SFLOAT) {
                    window->supports_hdr10_rgba16f = TRUE;
                }

                if (window->surfaceFormats[i].surfaceFormat.format == VK_FORMAT_A2B10G10R10_UNORM_PACK32) {
                    window->supports_hdr10_rgb10a2 = TRUE;
                }

                if (window->surfaceFormats[i].surfaceFormat.format == VK_FORMAT_A2R10G10B10_UNORM_PACK32) {
                    window->supports_hdr10_bgr10a2 = TRUE;
                }
            }

            // Extended scRGB colorspace with linear (scaled) encoding supported?
            if (window->surfaceFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT) {
                // Yes. Which pixel format?
                if (window->surfaceFormats[i].surfaceFormat.format == VK_FORMAT_R16G16B16A16_SFLOAT) {
                    window->supports_scrgb_rgba16f = TRUE;
                }
            }

            // AMD FreeSync2 HDR colorspace supported?
            if (window->surfaceFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_DISPLAY_NATIVE_AMD) {
                // Yes. Which pixel format?
                if (window->surfaceFormats[i].surfaceFormat.format == VK_FORMAT_R16G16B16A16_SFLOAT ||
                    window->surfaceFormats[i].surfaceFormat.format == VK_FORMAT_A2R10G10B10_UNORM_PACK32) {
                    window->supports_fs2_hdr = TRUE;
                }
            }

            // Standard SRGB colorspace with non-linear (gamma) encoding supported?
            if (window->surfaceFormats[i].surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                // Yes. Which pixel format?
                if (window->surfaceFormats[i].surfaceFormat.format == VK_FORMAT_R16G16B16A16_SFLOAT) {
                    window->supports_srgb_rgba16f = TRUE;
                }

                if (window->surfaceFormats[i].surfaceFormat.format == VK_FORMAT_A2B10G10R10_UNORM_PACK32) {
                    window->supports_srgb_rgb10a2 = TRUE;
                }

                if (window->surfaceFormats[i].surfaceFormat.format == VK_FORMAT_A2R10G10B10_UNORM_PACK32) {
                    window->supports_srgb_bgr10a2 = TRUE;
                }
            }
        }
    }

    // Probe supported present modes for surface:
    uint32_t presentModeCount;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan->physicalDevice, window->surface, &presentModeCount, NULL);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: Failed to vkGetPhysicalDeviceSurfacePresentModesKHR: %i\n", result);

        return(FALSE);
    }

    if (presentModeCount != 0) {
        VkPresentModeKHR presentModes[10];
        presentModeCount = (presentModeCount <= 10) ? presentModeCount : 10;
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan->physicalDevice, window->surface, &presentModeCount, presentModes);
        if (result != VK_SUCCESS) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: Failed to vkGetPhysicalDeviceSurfacePresentModesKHR for %i modes: %i\n", presentModeCount, result);

            return(FALSE);
        }

        // At the moment we just check for vsynced and non-vsynced present. The
        // former should always be supported, as the spec requires it:
        for (i = 0; i < presentModeCount; i++) {
            if (presentModes[i] == VK_PRESENT_MODE_FIFO_KHR)
                window->supports_vsync_FIFO = TRUE;
            if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
                window->supports_non_vsync = TRUE;
        }
    }

    return(TRUE);
}

#if PSYCH_SYSTEM == PSYCH_WINDOWS
#if defined(VK_USE_PLATFORM_WIN32_KHR)

void PsychProcessWindowEvents(PsychVulkanWindow* window)
{
    (void) window;
    MSG msg;

    // Run our message dispatch loop until we've processed all events in the event queue for our Vulkan windows:
    // We need to do this periodically so Windows is convinced that our window/application is still alive and
    // responsive. Otherwise it might conclude trouble and replace our Vulkan window by a ghost window which
    // is effectively dead and useless for OpenGL rendering and Vulkan display:
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

psych_bool PsychCreateMSWindowsDisplaySurface(PsychVulkanWindow* window, PsychVulkanDevice* vulkan, psych_bool isFullscreen, int screenId, void* outputHandle, PsychRectType rect, double refreshHz)
{
    VkResult result;
    psych_bool rc = FALSE;

    window->surface = (VkSurfaceKHR) VK_NULL_HANDLE;
    window->display = (VkDisplayKHR) VK_NULL_HANDLE;
    window->win32PrivateWindow = NULL;

    if (!windowClassRegistered) {
        WNDCLASSEX windowClass;
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        windowClass.lpfnWndProc = DefWindowProc;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = connection;
        windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        windowClass.lpszMenuName = NULL;
        windowClass.lpszClassName = "PTB-Vulkan";
        windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

        // Register our own window class for our Vulkan windows:
        if (!RegisterClassEx(&windowClass)) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: For gpu [%s] creating private vulkan window class failed in RegisterClassEx().\n", vulkan->deviceProps.deviceName);

            goto createsurface_out;
        }

        windowClassRegistered = TRUE;
    }

    // Windows Vulkan ICD's need their own window, they don't want to share with OpenGL and
    // therefore can not present into the standard Psychtoolbox onscreen window:
    window->win32PrivateWindow = CreateWindowEx(WS_EX_TOPMOST | WS_EX_APPWINDOW,
                                                "PTB-Vulkan",   // class name
                                                "PTB-Vulkan",   // app name
                                                WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // window style
                                                (int) rect[0],  // x coord
                                                (int) rect[1],  // y coord
                                                (unsigned int) (rect[2] - rect[0]), // width
                                                (unsigned int) (rect[3] - rect[1]), // height
                                                NULL,           // handle to parent
                                                NULL,           // handle to menu
                                                connection,     // hInstance
                                                NULL);          // no extra parameters

    if (!window->win32PrivateWindow) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: For gpu [%s] creating private vulkan output window for NVidia Vulkan failed in .\n", vulkan->deviceProps.deviceName);

        goto createsurface_out;
    }

    ShowWindow(window->win32PrivateWindow, SW_SHOW);

    PsychProcessWindowEvents(window);

    // Setup fullScreenExclusive struct for use by PsychProbeSurfaceProperties()
    // and vkCreateSwapchainKHR():
    PsychInitFullScreenExlusiveStructs(window);

    VkWin32SurfaceCreateInfoKHR createInfoWin32 = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .hinstance = connection,
        .hwnd = window->win32PrivateWindow
    };

    result = vkCreateWin32SurfaceKHR(vulkanInstance, &createInfoWin32, NULL, &window->surface);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: For gpu [%s] creating vulkan output window failed in vkCreateWin32SurfaceKHR: %i\n", vulkan->deviceProps.deviceName, result);

        goto createsurface_out;
    }

    // Got a windowed window for Vulkan stimulus display.
    if (verbosity > 3)
        printf("PsychVulkanCore-INFO: For gpu [%s] created a window display surface [%p] for display window %i\n", vulkan->deviceProps.deviceName, window->surface, window->index);

    // Mark success:
    rc = TRUE;

createsurface_out:

    if (!rc && (window->win32PrivateWindow)) {
        DestroyWindow(window->win32PrivateWindow);
        window->win32PrivateWindow = NULL;
    }

    return (rc);
}
#endif
#endif

#if PSYCH_SYSTEM == PSYCH_LINUX
void PsychProcessWindowEvents(PsychVulkanWindow* window)
{
    // No op so far:
    (void) window;
}

psych_bool PsychCreateLinuxDisplaySurface(PsychVulkanWindow* window, PsychVulkanDevice* vulkan, psych_bool isFullscreen, int screenId, void* outputHandle, PsychRectType rect, double refreshHz)
{
    VkResult result;
    psych_bool rc = FALSE;
    int saved_default_screen = -1;
    // We need a per-window X-Display connection, otherwise the NVidia proprietary blob will be unhappy in direct display mode.
    // More specifically it will work on 1st invocation, but then fail on repeated open window calls, unless the display connection
    // is closed each time / a new connection created for the next open window invocation. Ergo we need per-window instance display
    // connections to work around this driver bug:
    Display* connection = XOpenDisplay(NULL);

    window->surface = (VkSurfaceKHR) VK_NULL_HANDLE;
    window->display = (VkDisplayKHR) VK_NULL_HANDLE;
    window->x11PrivateWindow = None;

    if (isFullscreen) {
        unsigned int i;
        uint32_t modeCount, planeCount, planeIndex, planeStackIndex;
        VkDisplayModePropertiesKHR* modes;
        VkDisplayModePropertiesKHR targetMode;
        VkDisplayPlanePropertiesKHR* planes;

        // On Linux we use RandR output leasing for leasing the DRM/KMS output for exclusive control, then switch it to direct display mode:
        if (vulkan->driverProps.driverID == VK_DRIVER_ID_NVIDIA_PROPRIETARY) {
            // NVidia blob needs a special workaround. It fails output leasing if the DefaultScreen() X-Screen in the X-Display connection
            // handle is not the screen to which the NVidia driver is attached. More specifically, vkGetRandROutputDisplayEXT() works fine
            // and figures out the right X-Screen, but vkAcquireXlibDisplayEXT() doesn't. As a workaround we hack the proper screenId into
            // the display connection handle just for these two Vulkan calls, then restore to original.
            saved_default_screen = ((_XPrivDisplay) connection)->default_screen;
            ((_XPrivDisplay) connection)->default_screen = screenId;
        }

        result = fpGetRandROutputDisplayEXT(vulkan->physicalDevice, connection, (RROutput) outputHandle, &window->display);
        if (result != VK_SUCCESS || window->display == VK_NULL_HANDLE) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: For gpu [%s] Mapping vulkan output to target display failed in vkGetRandROutputDisplayEXT (display=%p): result = %i\n", vulkan->deviceProps.deviceName, window->display, result);

            goto createsurface_out;
        }

        if (verbosity > 3)
            printf("PsychVulkanCore-INFO: gpu [%s] has access to target display %p. Trying to switch to direct display mode...\n", vulkan->deviceProps.deviceName, window->display);

        result = fpAcquireXlibDisplayEXT(vulkan->physicalDevice, connection, window->display);
        if (result != VK_SUCCESS) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: For gpu [%s] acquiring display for direct display mode failed (display=%p): result = %i\n", vulkan->deviceProps.deviceName, window->display, result);

            goto createsurface_out;
        }

        if (verbosity > 3)
            printf("PsychVulkanCore-INFO: gpu [%s] display %p switched to direct display mode. Setting up for direct display surface.\n", vulkan->deviceProps.deviceName, window->display);

        // We have exclusive direct display mode access now. Probe all properties needed to create a fullscreen display surface.
        // Get the video modes supported by the display
        result = vkGetDisplayModePropertiesKHR(vulkan->physicalDevice, window->display, &modeCount, NULL);
        if (result != VK_SUCCESS || modeCount == 0) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: For gpu [%s] could not find any valid display modes! count = %i, result = %i\n", vulkan->deviceProps.deviceName, modeCount, result);

            goto createsurface_out;
        }

        modes = (VkDisplayModePropertiesKHR*) malloc(modeCount * sizeof(VkDisplayModePropertiesKHR));
        result = vkGetDisplayModePropertiesKHR(vulkan->physicalDevice, window->display, &modeCount, modes);
        if (result != VK_SUCCESS) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: For gpu [%s] could not find any valid display modes! count = %i, result = %i\n", vulkan->deviceProps.deviceName, modeCount, result);

            free(modes);
            goto createsurface_out;
        }

        // Match available modes against our requirements wrt. resolution and refresh rate:
        for (i = 0; i < modeCount; i++) {
            if (verbosity > 3)
                printf("PsychVulkanCore-INFO: Target %d x %d @%f Hz versus mode[%i]: %d x %d @%f Hz\n", (unsigned int) (rect[2] - rect[0]), (unsigned int) (rect[3] - rect[1]), refreshHz,
                       i, modes[i].parameters.visibleRegion.width, modes[i].parameters.visibleRegion.height, (float) modes[i].parameters.refreshRate / 1000.0f);

            if ((modes[i].parameters.visibleRegion.width == (unsigned int) (rect[2] - rect[0])) &&
                (modes[i].parameters.visibleRegion.height == (unsigned int) (rect[3] - rect[1])) &&
                (fabs(modes[i].parameters.refreshRate - refreshHz * 1000) < 500)) {
                // This one is suitable for us, select it as targetMode:
                targetMode = modes[i];
                if (verbosity > 3)
                    printf("PsychVulkanCore-INFO: Selecting video mode[%i]: %d x %d @%f Hz\n", i, modes[i].parameters.visibleRegion.width, modes[i].parameters.visibleRegion.height, (float) modes[i].parameters.refreshRate / 1000.0f);

                break;
            }
        }

        // No need for mode list anymore, we hopefully got our targetMode:
        free(modes);

        if (i == modeCount) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: For gpu [%s] could not find any suitable display mode for our requirements!\n", vulkan->deviceProps.deviceName);

            goto createsurface_out;
        }

        // Get the list of display planes:
        result = vkGetPhysicalDeviceDisplayPlanePropertiesKHR(vulkan->physicalDevice, &planeCount, NULL);
        if (result != VK_SUCCESS || planeCount == 0) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: For gpu [%s] could not find any valid display planes! count = %i, result = %i\n", vulkan->deviceProps.deviceName, planeCount, result);

            goto createsurface_out;
        }

        planes = (VkDisplayPlanePropertiesKHR*) malloc(planeCount * sizeof(VkDisplayPlanePropertiesKHR));
        result = vkGetPhysicalDeviceDisplayPlanePropertiesKHR(vulkan->physicalDevice, &planeCount, planes);
        if (result != VK_SUCCESS) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: For gpu [%s] could not find any valid display planes! count = %i, result = %i\n", vulkan->deviceProps.deviceName, planeCount, result);

            free(planes);
            goto createsurface_out;
        }

        // Find a compatible plane for the display:
        for (planeIndex = 0; planeIndex < planeCount; planeIndex++) {
            uint32_t displayCount;
            VkDisplayKHR *displays;

            // Reject planes already used on a different display:
            if ((planes[planeIndex].currentDisplay != VK_NULL_HANDLE) &&
                (planes[planeIndex].currentDisplay != window->display))
                continue;

            result = vkGetDisplayPlaneSupportedDisplaysKHR(vulkan->physicalDevice, planeIndex, &displayCount, NULL);
            if (result != VK_SUCCESS) {
                if (verbosity > 0)
                    printf("PsychVulkanCore-ERROR: For gpu [%s] vkGetDisplayPlaneSupportedDisplaysKHR() failed for planeIndex %i, result = %i\n", vulkan->deviceProps.deviceName, planeIndex, result);

                free(planes);
                goto createsurface_out;
            }

            // Skip to next plane if this one does not have supported displays:
            if (displayCount == 0)
                continue;

            // Get set of supported displays:
            displays = (VkDisplayKHR*) malloc(displayCount * sizeof(VkDisplayKHR));
            result = vkGetDisplayPlaneSupportedDisplaysKHR(vulkan->physicalDevice, planeIndex, &displayCount, displays);
            if (result != VK_SUCCESS) {
                if (verbosity > 0)
                    printf("PsychVulkanCore-ERROR: For gpu [%s] vkGetDisplayPlaneSupportedDisplaysKHR() II failed for planeIndex %i, result = %i\n", vulkan->deviceProps.deviceName, planeIndex, result);

                free(planes);
                free(displays);

                goto createsurface_out;
            }

            // Is our display part of the set of supported displays for this planeIndex?
            for (i = 0; i < displayCount; i++) {
                if (displays[i] == window->display)
                    break;
            }

            // Matching done, free displays:
            free(displays);

            // If our display is part of the set then break out of planeIndex finding loop:
            if (i < displayCount) {
                planeStackIndex = planes[planeIndex].currentStackIndex;
                break;
            }
        }

        // Done with probing planes for supported displays:
        free(planes);

        // Found a suitable planeIndex and planeStackIndex?
        if (planeIndex == planeCount) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: For gpu [%s] Could not find any plane which supports our display in a set of %i planes!?!\n", vulkan->deviceProps.deviceName, planeCount);

            goto createsurface_out;
        }

        VkDisplaySurfaceCreateInfoKHR createInfo = {
            .sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR,
            .pNext = NULL,
            .flags = 0,
            .displayMode = targetMode.displayMode,
            .planeIndex = planeIndex,
            .planeStackIndex = planeStackIndex,
            .transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .globalAlpha = 0,
            .alphaMode = VK_DISPLAY_PLANE_ALPHA_OPAQUE_BIT_KHR,
            .imageExtent = targetMode.parameters.visibleRegion
        };

        result = vkCreateDisplayPlaneSurfaceKHR(vulkanInstance, &createInfo, NULL, &window->surface);
        if (result != VK_SUCCESS) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: For gpu [%s] vkCreateDisplayPlaneSurfaceKHR() failed for planeIndex %i, result = %i\n", vulkan->deviceProps.deviceName, planeIndex, result);

            goto createsurface_out;
        }

        // We got a raw display surface for displaying to in direct display mode!
        if (verbosity > 3)
            printf("PsychVulkanCore-INFO: For gpu [%s] created a direct display surface [%p] for display window %i\n", vulkan->deviceProps.deviceName, window->surface, window->index);
    }
    else {
        // Open a X11 windowed output window on Linux:

        // outputHandle contains the X11 Window handle of the Psychtoolbox onscreen window.
        #if defined(VK_USE_PLATFORM_XLIB_KHR)
        // The NVidia blob needs its own X-Window, it doesn't want to present into the standard Psychtoolbox onscreen window.
        // But even for the FOSS drivers there are corner cases where things go sideways without a dedicated X-Window, e.g.,
        // when using mirror mode where primary stimulus is presented by Vulkan, but a slave window presents an experimenter
        // feedback image (mirroring/cloning) via OpenGL. So the safe thing to do is to always use a dedicated X-Window:
        {
            unsigned int windowMapEventCount = 0;
            XSetWindowAttributes attr = { 0 };

            attr.background_pixel = 0;  // Background color defaults to black.
            attr.border_pixel = 0;      // Border color as well.
            attr.event_mask = StructureNotifyMask; // We're only interested in StructureNotify to wait for Windows to be mapped.
            attr.override_redirect = 1;
            window->x11PrivateWindow = XCreateWindow(connection, RootWindow(connection, screenId), (int) rect[0], (int) rect[1],
                                                     (unsigned int) (rect[2] - rect[0]), (unsigned int) (rect[3] - rect[1]), 0,
                                                     CopyFromParent, CopyFromParent, CopyFromParent,
                                                     CWOverrideRedirect | CWBackPixel | CWBorderPixel | CWEventMask,
                                                     &attr);

            if (window->x11PrivateWindow == None) {
                if (verbosity > 0)
                    printf("PsychVulkanCore-ERROR: For gpu [%s] creating private vulkan output window for NVidia Vulkan failed in XCreateSimpleWindow.\n", vulkan->deviceProps.deviceName);

                goto createsurface_out;
            }

            XMapRaised(connection, window->x11PrivateWindow);

            while (windowMapEventCount < 1) {
                XEvent ev;
                XNextEvent(connection, &ev);
                if (ev.type == MapNotify || ev.type == ConfigureNotify)
                    windowMapEventCount++;
            }
        }

        VkXlibSurfaceCreateInfoKHR createInfoX11 = {
            .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
            .pNext = NULL,
            .flags = 0,
            .dpy = connection,
            .window = (window->x11PrivateWindow != None) ? window->x11PrivateWindow : (Window) outputHandle,
        };

        result = vkCreateXlibSurfaceKHR(vulkanInstance, &createInfoX11, NULL, &window->surface);
        if (result != VK_SUCCESS) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: For gpu [%s] creating vulkan output window failed in vkCreateXlibSurfaceKHR: %i\n", vulkan->deviceProps.deviceName, result);

            goto createsurface_out;
        }
        #endif

        // Got a windowed window for Vulkan stimulus display.
        if (verbosity > 3)
            printf("PsychVulkanCore-INFO: For gpu [%s] created a windowing system window display surface [%p] for display window %i\n", vulkan->deviceProps.deviceName, window->surface, window->index);
    }

    // Mark success:
    rc = TRUE;

createsurface_out:

    #if defined(VK_USE_PLATFORM_XLIB_KHR)
        if (!rc && (window->x11PrivateWindow != None)) {
            XUnmapWindow(connection, window->x11PrivateWindow);
            XDestroyWindow(connection, window->x11PrivateWindow);
            XFlush(connection);
            window->x11PrivateWindow = None;
        }
    #endif

    if (!rc && window->display) {
        fpReleaseDisplayEXT(vulkan->physicalDevice, window->display);
        window->display = VK_NULL_HANDLE;
    }

    if (saved_default_screen != -1)
        ((_XPrivDisplay) connection)->default_screen = saved_default_screen;

    if (!rc && connection)
        XCloseDisplay(connection);

    if (rc)
        window->connection = connection;

    return (rc);
}
#endif

#if PSYCH_SYSTEM == PSYCH_OSX
void PsychProcessWindowEvents(PsychVulkanWindow* window)
{
    // No op so far:
    (void) window;
}

psych_bool PsychCreateMoltenVKDisplaySurface(PsychVulkanWindow* window, PsychVulkanDevice* vulkan, psych_bool isFullscreen, int screenId, void* outputHandle, PsychRectType rect, double refreshHz)
{
    VkResult result;

    window->surface = (VkSurfaceKHR) VK_NULL_HANDLE;
    window->display = (VkDisplayKHR) VK_NULL_HANDLE;

    VkMetalSurfaceCreateInfoEXT createInfoMetal = {
        .sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT,
        .pNext = NULL,
        .flags = 0,
        .pLayer = (const CAMetalLayer*) outputHandle
    };

    result = vkCreateMetalSurfaceEXT(vulkanInstance, &createInfoMetal, NULL, &window->surface);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: For gpu [%s] creating vulkan output window failed in vkCreateMetalSurfaceEXT: %i\n", vulkan->deviceProps.deviceName, result);

        return(FALSE);
    }

    // Got a windowed window for Vulkan stimulus display.
    if (verbosity > 3)
        printf("PsychVulkanCore-INFO: For gpu [%s] created a window Metal display surface [%p] for display window %i\n", vulkan->deviceProps.deviceName, window->surface, window->index);

    return (TRUE);
}
#endif

psych_bool PsychCreateDisplaySurface(PsychVulkanWindow* window, PsychVulkanDevice* vulkan, psych_bool isFullscreen, int screenId, void* outputHandle, PsychRectType rect, double refreshHz)
{
    #if PSYCH_SYSTEM == PSYCH_LINUX
        return(PsychCreateLinuxDisplaySurface(window, vulkan, isFullscreen, screenId, outputHandle, rect, refreshHz));
    #endif

    #if PSYCH_SYSTEM == PSYCH_WINDOWS
        return(PsychCreateMSWindowsDisplaySurface(window, vulkan, isFullscreen, screenId, outputHandle, rect, refreshHz));
    #endif

    #if PSYCH_SYSTEM == PSYCH_OSX
        return(PsychCreateMoltenVKDisplaySurface(window, vulkan, isFullscreen, screenId, outputHandle, rect, refreshHz));
    #endif
}

psych_bool PsychIsVulkanGPUSuitable(PsychVulkanWindow* window, PsychVulkanDevice* vulkan, psych_uint8* targetdeviceUUID, psych_bool isFullscreen, int screenId, void* outputHandle, PsychRectType rect, double refreshHz, int hdrMode, psych_bool needsTiming, unsigned int colorPrecision, int colorSpace, int colorFormat, int flags)
{
    VkResult result;
    VkBool32 supportsPresent;
    psych_bool rc = FALSE;

    // First test if the required deviceUUID provided by our OpenGL rendering master matches the one of the Vulkan device.
    // For OpenGL -> Vulkan interop to work, both the OpenGL renderer and Vulkan renderer must use the same physical gpu,
    // so the UUID must match.

    // AMD's Vulkan drivers on Linux (amdgpu-pro and amdvlk) older than DriverVersionRaw 8388778 aka v2.0.170, aka amdvlk v-2020.Q4.6
    // use a different UUID from what Linux Mesa radeonsi OpenGL expects. The more recent releases do fix this bug:
    // Mesa OpenGL: 4-Byte fields describing PCI bus location in format: domain:bus:device:function.
    // AMD  Vulkan: 4-Byte fields describing PCI bus location in format: bus:device:function:0
    // Therefore we first treat matching against AMD written drivers on Linux to match up the right 4-byte subfields:
    if ((PSYCH_SYSTEM == PSYCH_LINUX) && (vulkan->driverProps.driverID == VK_DRIVER_ID_AMD_OPEN_SOURCE_KHR || vulkan->driverProps.driverID == VK_DRIVER_ID_AMD_PROPRIETARY_KHR)) {
        // Skip first 4 bytes of targetdeviceUUID (PCI bus domain) when matching against AMD Vulkan:
        rc = memcmp(&targetdeviceUUID[4], &vulkan->physDeviceProps.deviceUUID[0], 12) == 0;
    }

    // Other OS + driver + gpu combos, or AMD Vulkan specific match failed. Assume the UUID from OpenGL and Vulkan implementations match,
    // also in the case of the Linux combo AMD proprietary (amdgpu-pro) OpenGL + AMD written Vulkan drivers:
    if (!rc) {
        rc = memcmp(targetdeviceUUID, &(vulkan->physDeviceProps.deviceUUID[0]), 16) == 0;
    }

    if (!rc) {
        // Mismatch: This Vulkan physical device does not represent the same physical gpu that is used
        // on the OpenGL / external side for stimulus rendering and interop - Reject as unsuitable:
        if (verbosity > 4) {
            printf("PsychVulkanCore-INFO: Vulkan gpu '%s' does not represent same physical gpu as required by external client on window %i.\n", vulkan->deviceProps.deviceName, window->index);
            printf("PsychVulkanCore-INFO: Requested OpenGL targetdeviceUUID: ");
            for (int i = 0; i < 16; i++)
                printf("%02x ", targetdeviceUUID[i]);
            printf("\n");

            printf("PsychVulkanCore-INFO: Provided Vulkan deviceUUID:        ");
            for (int i = 0; i < 16; i++)
                printf("%02x ", vulkan->physDeviceProps.deviceUUID[i]);
            printf("\n\n");
        }

        return(FALSE);
    }

    // If we need HDR, then does the device support HDR at all?
    if (hdrMode && !vulkan->hasHDR) {
        if (verbosity > 4)
            printf("PsychVulkanCore-INFO: Vulkan gpu '%s' does not support HDR as required by window %i.\n", vulkan->deviceProps.deviceName, window->index);

        return(FALSE);
    }

    // If we need precise timing, and the device does support timing, we are good.
    // Otherwise we'll need to use our hacks which require strict double-buffering.
    if (needsTiming && !vulkan->hasTiming && !vulkan->hasWait) {
        // Our hacks are needed, and therefore strict double-buffering support by
        // the driver + gpu device combo.

        // Our assumption is that all devices support strict double-buffering in fullscreen mode,
        // at least as confirmed for AMD and NVidia on Linux and Windows so far, so that is fine:
        if (!isFullscreen) {
            // Windowed non-fullscreen mode, not all devices are suitable. Blacklist ones early here
            // which we already know won't work. Atm. this list is Mesa based drivers like radv for AMD
            // and anvil for Intel. However, starting with Mesa 19.3.0, the drivers support new driconf
            // options to force a minImageCount of 2, ie. support for strict double-buffering, instead of
            // the default triple-buffering (minImageCount of 3). Psychtoolbox mesa override options file
            // contains the following config section:
            //
            // <engine engine_name_match="PsychVulkanCore">
            // <option name="vk_x11_strict_image_count" value="true" />
            // <option name="vk_x11_override_min_image_count" value="2" />
            // </engine>
            //
            // to enforce use of double-buffering for us, so our timing/timestamping hack will work for
            // radv, anvil or any other Mesa supplied Vulkan driver, and our problem is solved.
            //
            // Therefore we only reject Mesa drivers if Mesa version is < 19.3.0.
            if ((PSYCH_SYSTEM == PSYCH_LINUX) &&
                (vulkan->driverProps.driverID == VK_DRIVER_ID_MESA_RADV_KHR || vulkan->driverProps.driverID == VK_DRIVER_ID_INTEL_OPEN_SOURCE_MESA_KHR) &&
                (vulkan->deviceProps.driverVersion < VK_MAKE_VERSION(19, 3, 0))) {
                // This is a no-go:
                if (verbosity > 4) {
                    printf("PsychVulkanCore-INFO: Vulkan gpu '%s' does not support required visual onset timing precision in the windowed configuration required by window %i.\n", vulkan->deviceProps.deviceName, window->index);
                    printf("PsychVulkanCore-INFO: This limitation can be resolved by upgrading your Mesa installation to Mesa version 19.3.0 or any later versions.\n");
                }

                return(FALSE);
            }
        }
    }

    if (colorPrecision) {
        // As of June 2020, Mesa 20.0.4, none of the OSS or proprietary drivers from AMD, Intel or NVidia support > 8 bpc in windowed X11 mode.
        // As far as fullscreen mode, NVidia proprietary blob and AMD written amdvlk do support 10 bit on Linux. Ergo, if precision is needed,
        // we should reject Mesa radv early, so the probe gets more quickly to amdvlk or amdgpu-pro on AMD gpu's. Rejecting anything else early
        // is pointless, as for Intel and NVidia on Linux and all manufacturers on Windows there is only one driver and thereby VkPhysicalDevice
        // per physical gpu, iow. there is no choice to find via probing. Let the probe go fully through and fail later on in the "game-over" case:
        if (vulkan->driverProps.driverID == VK_DRIVER_ID_MESA_RADV_KHR) {
            if (verbosity > 4) {
                printf("PsychVulkanCore-INFO: Vulkan gpu '%s' does not support required visual stimulus color precision by window %i.\n", vulkan->deviceProps.deviceName, window->index);
            }

            return(FALSE);
        }
    }

    // This is as far as we can get without creating a display surface on the target display, so lets create one:
    if (!PsychCreateDisplaySurface(window, vulkan, isFullscreen, screenId, outputHandle, rect, refreshHz)) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: Creating a display surface on Vulkan gpu '%s' for further probing on window %i failed.\n", vulkan->deviceProps.deviceName, window->index);

        return(FALSE);
    }

    // Does the physical gpu support presenting to this surface?
    // Note: This is almost useless for probing if a given vulkan gpu is physically driving the display on which our windows surface is displaying .
    // Both the NVidia and AMD drivers ignore the relationship between windows location on a specific X-Screen and if the driver+gpu drive that X-Screen.
    // Code inspection for both Mesa open-source Vulkan drivers and the AMD amdvlk open-source driver show that window location is not taken into account.
    // The Mesa drivers just check (for windowed windows on X11) if DRI3/Present is supported, and the color format is supported, iow. 8 bpc 24 bit color depth.
    // The amdvlk driver just checks if the gpu has display engines at all.
    result = vkGetPhysicalDeviceSurfaceSupportKHR(vulkan->physicalDevice, vulkan->graphicsQueueFamilyIndex, window->surface, &supportsPresent);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: vkGetPhysicalDeviceSurfaceSupportKHR() failed: Surface %p presentation supported = %i (res=%i).\n", window->surface, supportsPresent, result);

        return(FALSE);
    }

    if (verbosity > 4)
        printf("PsychVulkanCore-INFO: Probing if gpu '%s' supports presenting to the target surface: %s\n", vulkan->deviceProps.deviceName, supportsPresent ? "Yes" : "No");

    if (!supportsPresent)
        return(FALSE);

    // Some time granted to GUI event dispatch:
    PsychProcessWindowEvents(window);

    // Presenting to the surface is supported. Check supported color spaces, formats, present modes against requirements:
    if (!PsychProbeSurfaceProperties(window, vulkan)) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: Failed to probe surface properties on gpu '%s' for window %i [%p]: %i\n", vulkan->deviceProps.deviceName, window->index, window, result);

        return(FALSE);
    }

    if (needsTiming) {
        // We either need the ability to have strictly double-buffered swapchains, or we need precise timing support by the
        // driver, ie. support for the VK_GOOGLE_DISPLAY_TIMING_EXTENSION, or at least wait-for-present-completion support:
        if (!vulkan->hasTiming && !vulkan->hasWait && (window->surfaceCapabilities.minImageCount > 2)) {
            // No dice.
            if (verbosity > 4) {
                printf("PsychVulkanCore-INFO: Vulkan gpu '%s' does not support required visual onset timing precision in the %s configuration required by window %i.\n", vulkan->deviceProps.deviceName, isFullscreen ? "fullscreen" : "windowed", window->index);
            }

            return(FALSE);
        }

        if (!window->supports_vsync_FIFO) {
            // Vsync'ed VK_PRESENT_MODE_FIFO_KHR not supported? This should never happen!
            if (verbosity > 4)
                printf("PsychVulkanCore-INFO: Vulkan gpu '%s' does not support VK_PRESENT_MODE_FIFO_KHR in the %s configuration required by window %i! Driver bug?\n", vulkan->deviceProps.deviceName, isFullscreen ? "fullscreen" : "windowed", window->index);

            return(FALSE);
        }
    }

    // Do we need HDR?
    if (hdrMode) {
        // Yes. Check if any HDR color space + pixel format combo is supported at a minimum:
        if (!(window->supports_hdr10_rgb10a2 || window->supports_hdr10_bgr10a2 || window->supports_hdr10_rgba16f || window->supports_scrgb_rgba16f || window->supports_fs2_hdr)) {
            // Nope:
            if (verbosity > 3) {
                printf("PsychVulkanCore-INFO: Vulkan gpu '%s' does not support HDR in the %s configuration required by window %i.\n", vulkan->deviceProps.deviceName, isFullscreen ? "fullscreen" : "windowed", window->index);
                if ((PSYCH_SYSTEM == PSYCH_WINDOWS) && !isFullscreen)
                    printf("PsychVulkanCore-INFO: You may be able to make this work on Windows-10 if you enable HDR support in the display settings.\n");
            }

            return(FALSE);
        }
    }

    // Do we need higher than 8 bpc color precision?
    if (colorPrecision > 0) {
        // Yes. Check if suitable pixel color formats are supported:
        psych_bool supportsPrecision;

        // TODO FIXME This is crude and only covers standard sRGB and HDR10 BT2020 color space. Needs better matching.
        switch (colorPrecision) {
            case 1: // 10 bpc fixed point.
            case 4: // 16 bpc fixed point -> 10 bpc fixed point.
            case 5: // 16 bpc fixed point -> 16 bpc float fp16 ->10 bpc fixed point.
            case 6: // 16 bpc float fp16  -> 10 bpc fixed point.
                // All these are satisfied by a 10 bpc fixed point fallback:
                supportsPrecision = window->supports_hdr10_rgb10a2 || window->supports_hdr10_bgr10a2 || window->supports_srgb_rgb10a2 || window->supports_srgb_bgr10a2 || window->supports_fs2_hdr;
                break;

            case 2: // 16 bpc floating point RGBA16F.
                supportsPrecision = window->supports_hdr10_rgba16f || window->supports_srgb_rgba16f || window->supports_scrgb_rgba16f || window->supports_fs2_hdr;
                break;

            case 3: // 16 bpc fixed point:
                supportsPrecision = PsychIsColorSpaceFormatComboSupported(window, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, VK_FORMAT_R16G16B16A16_UNORM) || PsychIsColorSpaceFormatComboSupported(window, VK_COLOR_SPACE_HDR10_ST2084_EXT, VK_FORMAT_R16G16B16A16_UNORM);
                break;

            default:
                supportsPrecision = FALSE;
        }

        if (!supportsPrecision) {
            // Nope:
            if (verbosity > 4) {
                printf("PsychVulkanCore-INFO: Vulkan gpu '%s' does not support required precision %i in the %s configuration required by window %i.\n", vulkan->deviceProps.deviceName, colorPrecision, isFullscreen ? "fullscreen" : "windowed", window->index);
            }

            return(FALSE);
        }
    }

    // TODO FIXME probe for specific colorSpace support.
    // TODO FIXME probe for specific colorFormat support.
    // TODO FIXME probe for specific refreshHz support.

    // Some time granted to GUI event dispatch:
    PsychProcessWindowEvents(window);

    return(TRUE);
}

static bool getMemoryTypeFromProperties(PsychVulkanDevice* vulkan, uint32_t memoryTypeBits, VkFlags requirementsMask, uint32_t *memoryTypeIndex)
{
    uint32_t i;

    // Search memory types to find first memory type index with suitable properties:
    for (i = 0; i < vulkan->memoryProperties.memoryTypeCount; i++) {
        if (verbosity > 5)
            printf("PsychVulkanCore-INFO: getMemoryTypeFromProperties(): Testing memoryType index %i (%x) against allowed %x - ", i, 1 << i, memoryTypeBits);

        if ((memoryTypeBits & (1 << i))) {
            if (verbosity > 5)
                printf(" Properties %x vs. required %x - ", vulkan->memoryProperties.memoryTypes[i].propertyFlags, requirementsMask);

            // Memory type i is consistent with needs of our VkImage, does it satisfy requested memory properties
            // wrt. cpu and gpu memory access, efficiency and memory coherence?
            if ((vulkan->memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask) {
                if (verbosity > 5)
                    printf("OK. Using memoryTypeIndex %i.\n", i);

                *memoryTypeIndex = i;
                return(TRUE);
            }
            else if (verbosity > 5)
                printf("Nope.\n");
        }
        else if (verbosity > 5)
            printf("Nope.\n");
    }

    // No go!
    return(FALSE);
}

psych_bool PsychBeginCommandBuffer(VkCommandBuffer buffer)
{
    VkResult result;

    const VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
        .pInheritanceInfo = NULL,
    };

    result = vkBeginCommandBuffer(buffer, &commandBufferBeginInfo);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychBeginCommandBuffer: Failed (res=%i).\n", result);

        return(FALSE);
    }

    return(TRUE);
}

psych_bool PsychTransitionImageLayout(VkCommandBuffer cmdBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier barrier = { 0 };
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        // Switch uninitialized interop image to optimal source for copy/blit:
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        // Switch host initialized/written to interop image to optimal source for copy/blit:
        barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_HOST_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
        // Switch host initialized/written to interop image to optimal source for copy/blit:
        barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT ;
        printf("PsychTransitionImageLayout() VK_IMAGE_LAYOUT_PREINITIALIZED -> VK_IMAGE_LAYOUT_GENERAL\n");

        sourceStage = VK_PIPELINE_STAGE_HOST_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT; // XXX: Could also use VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT.
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        // Switch host initialized/written to interop image to optimal source for copy/blit:
        barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        printf("PsychTransitionImageLayout() VK_IMAGE_LAYOUT_PREINITIALIZED -> VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL\n");

        sourceStage = VK_PIPELINE_STAGE_HOST_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        // Switch host initialized/written to interop image to optimal source for copy/blit:
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        printf("PsychTransitionImageLayout() VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL -> VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL\n");

        sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT ;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        // Switch host initialized/written to interop image to optimal source for copy/blit:
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        printf("PsychTransitionImageLayout() VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL -> VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL\n");

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT ;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        // Debug for interop image: Before vkCmdClearColorImage():
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        // Debug for interop image: After vkCmdClearColorImage():
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
        // Switch swapchain image from copy/blit destination back to presentable image for display:
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = 0;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        // Switch swapchain image from "we don't care about old content" to optimal for copy/blit destination:
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else {
        printf("PsychVulkanCore-CRITICAL:PsychTransitionImageLayout() Unsupported layout transition requested. No-Op!\n");
        return(FALSE);
    }

    vkCmdPipelineBarrier(cmdBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);

    return(TRUE);
}

VkResult PsychGetNextSwapChainTargetBuffer(PsychVulkanWindow* window)
{
    VkResult result;

    // Get the index currentSwapChainBuffer of the next available swapchain image:
    // Both imageAcquiredSemaphores[frameIndex % window->numBuffers] and the
    // flipDoneFence will signal when the display engine is done with scanning
    // out the acquired image, ergo, when it was replaced as old scanout buffer
    // aka frontbuffer by a new frontbuffer, which was our old backbuffer. Iow.,
    // the semaphore and fence will signal when the previously scheduled swap /
    // present actually completed, hopefully due to gpu pageflip completion.
    //
    // So fence and semaphore signalling is our "flip completed" signal that we
    // can use to wait for flip and for visual stimulus onset timestamping.
    //
    // Note that this only works for double-buffered swap chains with exactly 2
    // buffers, not for triple-buffered or n-buffered chains with n != 2.
    result = vkResetFences(window->vulkan->device, 1, &window->flipDoneFence);
    if ((result != VK_SUCCESS) && (verbosity > 0))
        printf("PsychVulkanCore-ERROR: PsychGetNextSwapChainTargetBuffer(%i): Fence reset failed with error code %i.\n", window->index, result);

    result = vkAcquireNextImageKHR(window->vulkan->device, window->swapChain, UINT64_MAX, window->imageAcquiredSemaphores[window->frameIndex % window->numBuffers], window->flipDoneFence, &window->currentSwapChainBuffer);
    if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR) {
        // Acquired currentSwapChainBuffer - the index of the next swapchain image
        // to copy the to-be-presented stimulus into, iow. our "backBuffer".
        if ((verbosity > 6) || (verbosity > 1 && result == VK_SUBOPTIMAL_KHR))
            printf("PsychVulkanCore-DEBUG: PsychGetNextSwapChainTargetBuffer(%i): frameIndex %i - Next swapChain backBuffer image with index %i acquired.\n", window->index,
                   window->frameIndex, window->currentSwapChainBuffer);

            if (verbosity > 1 && result == VK_SUBOPTIMAL_KHR)
                printf("PsychVulkanCore-WARNING: PsychGetNextSwapChainTargetBuffer(%i): Swapchain reports status VK_SUBOPTIMAL_KHR. Performance and timing precision may suffer!\n", window->index);

        result = VK_SUCCESS;

        return(result);
    }

    if (verbosity > 0) {
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            printf("PsychVulkanCore-ERROR: PsychGetNextSwapChainTargetBuffer(%i): Swapchain reports status VK_ERROR_OUT_OF_DATE_KHR!\n", window->index);
        }
        else {
            printf("PsychVulkanCore-ERROR: PsychGetNextSwapChainTargetBuffer(%i): Swapchain reports error code %i.\n", window->index, result);
        }
    }

    // Error return:
    return(result);
}

psych_bool PsychWaitForPresentCompletion(PsychVulkanWindow* window)
{
    VkResult result;

    // Wait for flipDoneFence to signal, iow. for confirmed flip completion aka
    // imminent visual stimulus onset. Then reset the fence and timestamp the moment:
    result = vkWaitForFences(window->vulkan->device, 1, &window->flipDoneFence, VK_TRUE, 1e9);
    if (result == VK_SUCCESS)
        result = vkResetFences(window->vulkan->device, 1, &window->flipDoneFence);

    if ((result != VK_SUCCESS) && (verbosity > 0)) {
        if (result == VK_TIMEOUT) {
            printf("PsychVulkanCore-ERROR: PsychWaitForPresentCompletion(%i): Fence wait+reset failed - Timeout!\n", window->index);
        } else {
            printf("PsychVulkanCore-ERROR: PsychWaitForPresentCompletion(%i): Fence wait+reset failed with error code %i.\n", window->index, result);
        }
    }

    return (result == VK_SUCCESS ? TRUE : FALSE);
}

psych_bool PsychPresent(PsychVulkanWindow* window, double tWhen, unsigned int timestampMode)
{
    VkResult result;
    VkPresentTimeGOOGLE targetPresentTimeG;
    PsychVulkanDevice* vulkan = window->vulkan;
    uint64_t targetPresentId;

    // Mark presentation timestamp as so far "invalid"/"unknown":
    window->tPresentComplete = -1;

    // Some time granted to GUI event dispatch:
    PsychProcessWindowEvents(window);

    // Wait for the image acquired semaphore to be signaled to ensure that the
    // swapchain image won't be overwritten with new content from the interop
    // image until the presentation engine has fully released ownership to us,
    // and it is okay to overwrite the old image content:
    VkPipelineStageFlags pipeStageFlags[] = { VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT };

    // If window->interopRenderDoneSemaphore exists (non-zero) then we not only
    // wait until the target swapChainImage is ready, but also until the external
    // client has signalled render completion on the interop image via signalling
    // of interopRenderDoneSemaphore:
    VkSemaphore waitSemaphores[2];
    waitSemaphores[0] = window->imageAcquiredSemaphores[window->frameIndex % window->numBuffers];
    waitSemaphores[1] = window->interopRenderDoneSemaphore;

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = (window->interopRenderDoneSemaphore) ? 2 : 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = &pipeStageFlags[0],
        .commandBufferCount = 1,
        .pCommandBuffers = &window->swapChainCommandBuffers[window->currentSwapChainBuffer],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &window->imageCopyDoneSemaphores[window->frameIndex % window->numBuffers],
    };

    // Submit copy command. imageCopyDoneSemaphores will signal once the copy or blit
    // is completed and we are swap-ready:
    result = vkQueueSubmit(vulkan->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychPresent(%i): Interop -> Swapchain copy failed with error code %i.\n", window->index, result);

        return(FALSE);
    }

    // Wait for imageCopyDoneSemaphores before queuing the present to the display driver:
    VkPresentInfoKHR present = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &window->imageCopyDoneSemaphores[window->frameIndex % window->numBuffers],
        .swapchainCount = 1,
        .pSwapchains = &window->swapChain,
        .pImageIndices = &window->currentSwapChainBuffer,
        .pResults = NULL, // swapchainCount separate VkResult's if presenting to multiple chains.
    };

    // VK_KHR_PRESENT_ID supported?
    #ifdef VK_KHR_present_id
    if (vulkan->hasWait && (timestampMode > 0)) {
        // Assign present id - the current frameIndex:
        VkPresentIdKHR presentIdInfo = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_ID_KHR,
            .pNext = present.pNext,
            .swapchainCount = 1,
            .pPresentIds = &targetPresentId,
        };

        // targetPresentId must start with 1 for 1st queued present, hence the frameIndex + 1:
        targetPresentId = window->frameIndex + 1;
        present.pNext = &presentIdInfo;

        if (verbosity > 7)
            printf("PsychVulkanCore-DEBUG: PsychPresent(%i): Assigning frame %i with VkPresentIdKHR presentID %lli.\n", window->index, window->frameIndex, targetPresentId);
    }
    #endif

    // VK_GOOGLE_DISPLAY_TIMING supported?
    if (vulkan->hasTiming && (timestampMode > 1)) {
        // Yes: Queue a target time for the present:
        VkPresentTimesInfoGOOGLE presentTimeInfoG = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_TIMES_INFO_GOOGLE,
            .pNext = present.pNext,
            .swapchainCount = 1,
            .pTimes = &targetPresentTimeG,
        };

        present.pNext = &presentTimeInfoG;

        #if PSYCH_SYSTEM == PSYCH_LINUX || PSYCH_SYSTEM == PSYCH_OSX
        // Linux: Map tWhen GetSecs() CLOCK_REALTIME target time into CLOCK_MONOTONIC time, convert to Nanoseconds:
        // macOS: Map tWhen GetSecs() time into Mach host time (which is a no-op btw.), convert to Nanoseconds:
        targetPresentTimeG.desiredPresentTime = PsychOSRefTimeToMonotonicTime(tWhen) * 1e9;
        #else
        targetPresentTimeG.desiredPresentTime = 0; // TODO FIXME IMPLEMENT!
        #endif

        targetPresentTimeG.presentID = window->frameIndex;

        if (verbosity > 7)
            printf("PsychVulkanCore-DEBUG: PsychPresent(%i): Queuing frame %i with VkPresentTimesInfoGOOGLE for present at time >= tWhen %f secs.\n", window->index, window->frameIndex, tWhen);
    }
    else {
        // No: Simply wait for target present time:
        PsychWaitUntilSeconds(tWhen);
    }

    // Commit new HDR metadata to the swapChain / display if requested:
    if (window->hdrMode && window->currentDisplayHDRMetadataNeedsCommit) {
        window->currentDisplayHDRMetadataNeedsCommit = FALSE;

        PsychSetHDRMetaData(window);
    }

    // Present it asap:
    result = vkQueuePresentKHR(vulkan->graphicsQueue, &present);
    if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR) {
        // Success! All perfectly good?
        if ((verbosity > 6) || (verbosity > 1 && result == VK_SUBOPTIMAL_KHR))
            printf("PsychVulkanCore-DEBUG: PsychPresent(%i): frameIndex %i - swapChain image with index %i queued for present at tWhen %f secs.\n", window->index,
                   window->frameIndex, window->currentSwapChainBuffer, tWhen);

        // Suboptimal present? This may tear or have reduced performance / increased latency, and also
        // potentially screwed up timing:
        if (verbosity > 1 && result == VK_SUBOPTIMAL_KHR)
            printf("PsychVulkanCore-WARNING: PsychPresent(%i): Swapchain reports status VK_SUBOPTIMAL_KHR. Performance and timing precision may suffer!\n", window->index);
    }
    else {
        // Failed!
        if (verbosity > 0) {
            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                printf("PsychVulkanCore-ERROR: PsychPresent(%i): Failed! Swapchain reports status VK_ERROR_OUT_OF_DATE_KHR!\n", window->index);
            }
            else {
                printf("PsychVulkanCore-ERROR: PsychPresent(%i): Failed! Swapchain reports error code %i.\n", window->index, result);
            }
        }

        return(FALSE);
    }

    // The present is queued, but possibly not finished yet, so we need to enforce a host wait
    // to  wait for present completion.
    window->frameIndex++;

    // Some time granted to GUI event dispatch:
    PsychProcessWindowEvents(window);

    // Try to acquire next target swapchain buffer. window->currentSwapChainBuffer will
    // contain the index of the proper swapChainImage. This will setup the flipDoneFence
    // to signal Present completion of the present we just sent out:
    if (PsychGetNextSwapChainTargetBuffer(window) != VK_SUCCESS)
        return(FALSE);

    // Always wait for flipDoneFence:
    // Makes fence handling more robust and we can't prevent being throttled anyway, given
    // that we can't change sync-to-vblank settings on the fly, only at swapChain creation time:
    if (!PsychWaitForPresentCompletion(window))
        return(FALSE);

    // Should we timestamp (imminent) stimulus onset?
    if (timestampMode > 0) {
        // Wait for present completion supported?
        #ifdef VK_KHR_present_id
        if (vulkan->hasWait) {
            // Blocking wait with timeout of 1 second for present completion of the just-queued present:
            result = fpWaitForPresentKHR(vulkan->device, window->swapChain, targetPresentId, 1e9);
            if ((result != VK_SUCCESS) && (verbosity > 0)) {
                if (result == VK_TIMEOUT) {
                    printf("PsychVulkanCore-ERROR: vkWaitForPresentKHR(%i): Failed due to timeout!\n", window->index);
                } else {
                    printf("PsychVulkanCore-ERROR: vkWaitForPresentKHR(%i): Failed with error code %i.\n", window->index, result);
                }
            }
            else if (verbosity > 8) {
                double tNow;
                PsychGetAdjustedPrecisionTimerSeconds(&tNow);
                printf("PsychVulkanCore-DEBUG: PsychPresent(%i): Frame %i with presentID %lli signalled by vkWaitForPresentKHR as complete at %f seconds.\n", window->index, window->frameIndex - 1, targetPresentId, tNow);
            }
        }
        #endif

        // VK_GOOGLE_DISPLAY_TIMING supported for timestamping?
        if (vulkan->hasTiming && (timestampMode > 1)) {
            // Yes. Fetch timestamp from Vulkan:
            const double tQueryTimeout = 0.5; // Time out after more than tQueryTimeout seconds of failure.
            VkPastPresentationTimingGOOGLE pastTiming[1000];
            double tNow, tStart;
            uint32_t count = 0;

            // Wait until target present time is reached - No point checking before.
            // Not needed if we used wait for present complete:
            if (!vulkan->hasWait)
                PsychWaitUntilSeconds(tWhen);

            // Poll for arrival of present completion timestamp:
            PsychGetAdjustedPrecisionTimerSeconds(&tNow);
            tStart = tNow;

            // Unused atm - no longer needed:
            // This label was only used on macOS + MoltenVK for buggy MVK versions which do not
            // actually dequeue timestamps fetched via vkGetPastPresentationTimingGOOGLE(), as they
            // should, but keep all timestamps in the queue until the queue reaches capacity. Iow.,
            // the queue can only grow (up to a defined maximum, e.g., 60 entries), but never shrink.
            // This is a spec violation and it can make the timestamp query fail for up to slightly
            // more than 1 video refresh cycle. As a workaround, we repeat the procedure in 1 msecs
            // intervals until either success, or our master timeout is hit, which suggests some other
            // unexpected macOS bug:
            macosmvkworkaroundrepeat:

            while ((count < 1) && (tNow < tStart + tQueryTimeout)) {
                result = fpGetPastPresentationTimingGOOGLE(vulkan->device, window->swapChain, &count, NULL);
                if (result != VK_SUCCESS) {
                    if (verbosity > 0)
                        printf("PsychVulkanCore-ERROR: PsychPresent(%i): Failed! vkGetPastPresentationTimingGOOGLE reports error code %i.\n", window->index, result);

                    return(FALSE);
                }

                PsychGetAdjustedPrecisionTimerSeconds(&tNow);
                if ((count < 1) && (tNow < tStart + tQueryTimeout)) {
                    PsychYieldIntervalSeconds(0.001);
                    if (verbosity > 9)
                        printf("PsychVulkanCore-DEBUG: PsychPresent(%i): Polling for vkGetPastPresentationTimingGOOGLE returning results. %f msecs elapsed, %f msecs since tWhen.\n", window->index, count, 1000 * (tNow - tStart), 1000 * (tNow - tWhen));
                }
            }

            if (verbosity > 8)
                printf("PsychVulkanCore-DEBUG: PsychPresent(%i): vkGetPastPresentationTimingGOOGLE returned %i timestamps after %f msecs wait. Fetching last one %f msecs since tWhen.\n", window->index, count, 1000 * (tNow - tStart), 1000 * (tNow - tWhen));

            // Got something?
            if (count < 1) {
                // Nope, timeout.

                // Take simple raw timestamp as fallback:
                PsychGetAdjustedPrecisionTimerSeconds(&window->tPresentComplete);

                // First frame in this windows session?
                if (window->frameIndex - 1 > 0) {
                    // No, bad:
                    if (verbosity > 0)
                        printf("PsychVulkanCore-ERROR: PsychPresent(%i): vkGetPastPresentationTimingGOOGLE failed to retrieve timestamp! Timed out.\n", window->index);

                    return(FALSE);
                }
                else {
                    // Yes, be lenient, as at least on Linux + Mesa this seems to be expected:
                    if (verbosity > 5)
                        printf("PsychVulkanCore-DEBUG: PsychPresent(%i): vkGetPastPresentationTimingGOOGLE failed to retrieve timestamp for frameIndex 0. Timed out. Carrying on with fallback.\n", window->index);
                }
            }
            else {
                int i;

                // Got at least one timestamp. We want the most recent one:
                do {
                    count = (count > 1000) ? 1000 : count;

                    result = fpGetPastPresentationTimingGOOGLE(vulkan->device, window->swapChain, &count, &pastTiming[0]);
                    if ((result != VK_SUCCESS) && (result != VK_INCOMPLETE)) {
                        if (verbosity > 0)
                            printf("PsychVulkanCore-ERROR: PsychPresent(%i): Failed to retrieve next timestamp! vkGetPastPresentationTimingGOOGLE reports error code %i.\n", window->index, result);

                        return(FALSE);
                    }
                    else if ((verbosity > 8) && (result == VK_INCOMPLETE))
                        printf("PsychVulkanCore-DEBUG: PsychPresent(%i): vkGetPastPresentationTimingGOOGLE (count %i) for presentID %i [current %i] returned old timestamp %f Fetching next one.\n",
                               window->index, count, pastTiming[0].presentID, targetPresentTimeG.presentID, (double) pastTiming[0].actualPresentTime / 1e9);

                    if (result == VK_INCOMPLETE) fpGetPastPresentationTimingGOOGLE(vulkan->device, window->swapChain, &count, NULL);
                } while (result == VK_INCOMPLETE);

                for (i = 0; i < count; i++) {
                    if (verbosity > 8)
                        printf("PsychVulkanCore-DEBUG: PsychPresent(%i):%i: presentID %i [need %i] returned timestamp %f.\n",
                               window->index, i, pastTiming[i].presentID, targetPresentTimeG.presentID, (double) pastTiming[i].actualPresentTime / 1e9);
                    if (pastTiming[i].presentID == targetPresentTimeG.presentID)
                        break;
                }

                // Found a matching record before timeout?
                if (i == count) {
                    // No. Could be timeout due to temporary malfunction, or an OS implementation bug.

                    // Yield to give the system some processing time:
                    PsychYieldIntervalSeconds(0.0001);
                    PsychGetAdjustedPrecisionTimerSeconds(&tNow);

                    // macOS and not yet timed out?
                    if ((PSYCH_SYSTEM == PSYCH_OSX) && (tNow < tStart + tQueryTimeout)) {
                        // As of macOS 10.15.7 and MoltenVK 1.0.10 May 2022, at least the very first presented frame
                        // does not result in the Windowserver calling the drawable presented callback, leaving the
                        // first wait-for-present-completion timing out after tQueryTimeout, and a stale handler left
                        // to invoke. Later cleanup actions triggered during vkAcquireNextImageKHR() -> [mtlDrawable release]
                        // will trigger handler invocation with 0 timestamp, so a presented event gets misqueued for frame 0
                        // and falsely picked up while waiting for frame 1 present completion events. This would lead to failure
                        // unless we repeat the whole query sequence here, discarding the frame 0 timestamp. This way, frame 1
                        // timestamp is picked up after true frame 1 present completion. Iow. this hack/workaround realigns
                        // expected with actual timestamps in case that a presented handler fails to get triggered by the
                        // Windowserver/CoreAnimation/Metal/Vulkan-MoltenVK at the proper time. Comparison with other purely
                        // Metal-based code suggests strongly a macOS operating system bug at work.
                        if (verbosity > 10)
                            printf("PsychVulkanCore-DEBUG: PsychPresent(%i): No match yet [%f usecs elapsed]! Retrying in a bit...\n", window->index, 1.0e6 * (tNow - tStart));

                        // Repeat the whole query timestamps + try to find proper presentation timestamp:
                        count = 0;
                        goto macosmvkworkaroundrepeat;
                    }

                    // Final timeout! This should not happen on this OS + display system! Fail.
                    if (verbosity > 0)
                        printf("PsychVulkanCore-ERROR: PsychPresent(%i): NO MATCH for target presentID %i! OS bug!?! Timed out, aborting.\n", window->index, targetPresentTimeG.presentID);

                    return(FALSE);
                }

                // On macOS MoltenVK, the driver maps an invalid Metal drawable present time of zero to targetPresentTimeG.desiredPresentTime,
                // so lets detect these Metal presentation and/or timestamping failures and report them to our caller as zero "invalid" timestamps:
                if ((PSYCH_SYSTEM == PSYCH_OSX) && (targetPresentTimeG.desiredPresentTime == pastTiming[i].actualPresentTime))
                    pastTiming[i].actualPresentTime = 0;

                // Got the final - and thereby most recent - timestamp.
                // Assign as present completion timestamp:
                window->tPresentComplete = PsychOSMonotonicToRefTime((double) pastTiming[i].actualPresentTime / 1e9);

                if (verbosity > 7)
                    printf("PsychVulkanCore-DEBUG: PsychPresent(%i): vkGetPastPresentationTimingGOOGLE for presentID %i returned flip completion timestamp %f secs vs. desired present time %f secs. Delta %f msecs.\n",
                        window->index, pastTiming[i].presentID, window->tPresentComplete, PsychOSMonotonicToRefTime((double) pastTiming[i].desiredPresentTime / 1e9), 1000 * (window->tPresentComplete - tWhen));
            }
        }
        else {
            // No. Take simple raw timestamp:
            PsychGetAdjustedPrecisionTimerSeconds(&window->tPresentComplete);
        }

        if (verbosity > 5)
            printf("PsychVulkanCore-DEBUG: PsychPresent(%i): Present for frameIndex %i completed: tComplete = %f secs.\n", window->index, window->frameIndex - 1, window->tPresentComplete);
    }

    // Some time granted to GUI event dispatch:
    PsychProcessWindowEvents(window);

    // Present successfully completed:
    return(TRUE);
}

psych_bool PsychCreateInteropTexture(PsychVulkanWindow* window)
{
    const psych_bool bringup = FALSE;
    psych_bool extmem = (PSYCH_SYSTEM != PSYCH_OSX) ? TRUE : FALSE;
    VkResult result;
    PsychVulkanDevice* vulkan = window->vulkan;
    VkImageTiling tiling = (window->interopTextureTiled && !bringup) ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | ((!bringup) ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0);
    VkFlags requiredMemoryProps = bringup ? VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkMemoryRequirements memoryRequirements;

    // Note wrt. memoryRequirements:
    // For bringup we need VK_IMAGE_TILING_LINEAR so we can map the memory for host writes.
    // We also need VK_IMAGE_TILING_LINEAR on NVidia to get host visible memory for host writes.
    // However, VK_IMAGE_TILING_LINEAR does *not* allow rendering into as VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT.

    // Image setup for external interop:
    VkExternalMemoryImageCreateInfo externalMemImageInfo = {
        .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
        .pNext = NULL,
        #if PSYCH_SYSTEM == PSYCH_WINDOWS
        .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT,
        #else
        .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT,
        #endif
    };

    // Create VkImage object:
    VkImageCreateInfo imageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = (!bringup && extmem) ? &externalMemImageInfo : NULL,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = window->interopTextureVkFormat,
        .extent = {window->width, window->height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .flags = 0,
        .initialLayout = (!bringup && extmem) ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PREINITIALIZED,
    };

    result = vkCreateImage(vulkan->device, &imageCreateInfo, NULL, &window->interopImage);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychCreateInteropTexture:vkCreateImage() failed for window %i (res=%i).\n", window->index, result);

        return(FALSE);
    }

    // Figure out memory and backing store requirements for image to be exportable to OpenGL:
    vkGetImageMemoryRequirements(vulkan->device, window->interopImage, &memoryRequirements);

    VkExportMemoryAllocateInfo exportAllocInfo = {
        .sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        #if PSYCH_SYSTEM == PSYCH_WINDOWS
        .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT,
        #else
        .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT,
        #endif
    };

    VkMemoryAllocateInfo memoryAllocInfo = {
        memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        memoryAllocInfo.pNext = (!bringup && extmem) ? &exportAllocInfo : NULL,
        memoryAllocInfo.allocationSize = memoryRequirements.size,
        memoryAllocInfo.memoryTypeIndex = 0,
    };

    if(!getMemoryTypeFromProperties(vulkan, memoryRequirements.memoryTypeBits, requiredMemoryProps, &memoryAllocInfo.memoryTypeIndex)) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychCreateInteropTexture:getMemoryTypeFromProperties() failed for window %i.\n", window->index);

        goto out_interop_image;
    }

    // Allocate the backing memory:
    result = vkAllocateMemory(vulkan->device, &memoryAllocInfo, NULL, &window->interopImageMemory);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychCreateInteropTexture:vkAllocateMemory() failed for window %i (res=%i).\n", window->index, result);

        goto out_interop_image;
    }

    window->interopMemorysize = memoryRequirements.size;

    // Bind backing memory image:
    result = vkBindImageMemory(vulkan->device, window->interopImage, window->interopImageMemory, 0);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychCreateInteropTexture:vkBindImageMemory() failed for window %i (res=%i).\n", window->index, result);

        goto out_interop_image;
    }

    // In normal operation mode, get the interop handles for exporting our interopImage to Psychtoolbox
    // for import as OpenGL textures/fbo for render-to-texture:
    if (!bringup) {
        #if PSYCH_SYSTEM == PSYCH_WINDOWS
            // Get handle for shared memory with OpenGL:
            VkMemoryGetWin32HandleInfoKHR memoryGetWinhandleInfo = {
                .sType = VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR,
                .pNext = NULL,
                .memory = window->interopImageMemory,
                .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT,
            };

            result = fpGetMemoryWin32HandleKHR(vulkan->device, &memoryGetWinhandleInfo, &window->interopHandles.memory);
            if (verbosity > 4)
                printf("PsychVulkanCore-INFO: PsychCreateInteropTexture: Got Win32 memory handle %p.\n", window->interopHandles.memory);
        #endif

        #if PSYCH_SYSTEM == PSYCH_LINUX
            // Get fd for shared memory with OpenGL:
            VkMemoryGetFdInfoKHR memoryGetFdInfo = {
                .sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR,
                .pNext = NULL,
                .memory = window->interopImageMemory,
                .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT,
            };

            result = fpGetMemoryFdKHR(vulkan->device, &memoryGetFdInfo, &window->interopHandles.memory);
            if (verbosity > 4)
                printf("PsychVulkanCore-INFO: PsychCreateInteropTexture: Got POSIX memory fd %i.\n", window->interopHandles.memory);
        #endif

        #if PSYCH_SYSTEM == PSYCH_OSX
            // Tell MoltenVK to allocate a IOSurface as backing store for the interop image:
            result = vkUseIOSurfaceMVK(window->interopImage, NULL);
            if (result == VK_SUCCESS) {
                IOSurfaceRef ioSurface;

                // Retrieve backing IOSurface for interop image:
                vkGetIOSurfaceMVK(window->interopImage, &ioSurface);

                // Create a sibling OpenGL texture for it, of matching format:
                glGenTextures(1, (GLuint*) &window->interopHandles.memory);
                glBindTexture(GL_TEXTURE_RECTANGLE_ARB, (GLuint) window->interopHandles.memory);

                // Attach IOSurface as backing store for the texture, so rendering to the texture renders to the VkImage:
                // TODO GL_RGBA16 aka window->colorPrecision == 3, but not supported by MoltenVK/Metal on macOS for display anyway.
                if (CGLTexImageIOSurface2D(CGLGetCurrentContext(), GL_TEXTURE_RECTANGLE_ARB,
                                           (window->colorPrecision == 2) ? GL_RGBA16F : (window->colorPrecision == 1) ? GL_RGB10_A2 : GL_RGBA8,
                                           window->width, window->height,
                                           (window->colorPrecision >= 2) ? GL_RGBA : GL_BGRA,
                                           (window->colorPrecision == 2) ? GL_HALF_FLOAT : (window->colorPrecision == 1) ? GL_UNSIGNED_INT_2_10_10_10_REV : GL_UNSIGNED_INT_8_8_8_8_REV,
                                           ioSurface, 0)) {
                    // Failed!
                    if (verbosity > 0)
                        printf("PsychVulkanCore-ERROR: PsychCreateInteropTexture: CGLTexImageIOSurface2D() failed! IOSurface = %p, texture = %i\n", ioSurface, window->interopHandles.memory);

                    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
                    glDeleteTextures(1, (GLuint*) &window->interopHandles.memory);
                    window->interopHandles.memory = 0;
                    result = VK_ERROR_INITIALIZATION_FAILED;
                }
                else {
                    // Disable any filtering on texture - It interferes with rendering to it:
                    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                    // Done with texture setup. Texture handle window->interopHandles.memory is ready for interop rendering:
                    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

                    if (verbosity > 4)
                        printf("PsychVulkanCore-INFO: PsychCreateInteropTexture: Got OpenGL interop texture handle %i [IOSurface %p].\n", window->interopHandles.memory, ioSurface);
                }
            }
            else if (verbosity > 0) {
                printf("PsychVulkanCore-ERROR: PsychCreateInteropTexture: Failed to enable MoltenVK IOSurface backing!\n");
            }
        #endif

        if (result != VK_SUCCESS) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: PsychCreateInteropTexture: Retrieving OpenGL interop handles failed for window %i (res=%i).\n", window->index, result);

            goto out_interop_image;
        }
    }

    // For bringup, write some diagnostic pixel pattern into the interopImage:
    if (requiredMemoryProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        VkImageSubresource subResource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .arrayLayer = 0,
        };

        VkSubresourceLayout layout;
        uint8_t *data;

        vkGetImageSubresourceLayout(vulkan->device, window->interopImage, &subResource, &layout);

        if (VK_SUCCESS == vkMapMemory(vulkan->device, window->interopImageMemory, 0, memoryAllocInfo.allocationSize, 0, (void*) &data)) {
            size_t j = memoryAllocInfo.allocationSize;
            while (j--) {
                *(data++) = j;
            }

            vkUnmapMemory(vulkan->device, window->interopImageMemory);

            if (verbosity > 3)
                printf("PsychVulkanCore-INFO: PsychCreateInteropTexture: Created test image content for window %i.\n", window->index);
        }
        else {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: PsychCreateInteropTexture:vkMapMemory() failed for window %i (res=%i).\n", window->index, result);

            goto out_interop_image;
        }
    }

    // One-time transition the image layout to something useful for rendering into by the OpenGL client:
    if (!PsychBeginCommandBuffer(window->swapChainCommandBuffers[0])) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychCreateInteropTexture:PsychBeginCommandBuffer() failed for window %i.\n", window->index);

        goto out_interop_image;
    }

    if (!PsychTransitionImageLayout(window->swapChainCommandBuffers[0], window->interopImage, imageCreateInfo.initialLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychCreateInteropTexture:PsychTransitionImageLayout() failed for window %i.\n", window->index);

        goto out_interop_image;
    }

    result = vkEndCommandBuffer(window->swapChainCommandBuffers[0]);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychCreateInteropTexture: vkEndCommandBuffer() failed for window %i (res=%i).\n", window->index, result);

        goto out_interop_image;
    }

    // Submit command for inital interopImage layout transition:
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = window->swapChainCommandBuffers,
    };

    result = vkQueueSubmit(vulkan->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychCreateInteropTexture: vkQueueSubmit() failed for window %i (res=%i).\n", window->index, result);

        goto out_interop_image;
    }

    // Wait for everything to complete:
    result = vkQueueWaitIdle(vulkan->graphicsQueue);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychCreateInteropTexture: vkQueueWaitIdle() failed for window %i (res=%i).\n", window->index, result);

        goto out_interop_image;
    }

    // We can reset this command buffer, so it can be set up for actual operation for swapchain image 0:
    vkResetCommandBuffer(window->swapChainCommandBuffers[0], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

    if (verbosity > 3)
        printf("PsychVulkanCore-INFO: PsychCreateInteropTexture: Success for window %i.\n", window->index);

    return(TRUE);

out_interop_image:

    vkResetCommandBuffer(window->swapChainCommandBuffers[0], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

    vkDestroyImage(vulkan->device, window->interopImage, NULL);
    window->interopImage = VK_NULL_HANDLE;

    vkFreeMemory(vulkan->device, window->interopImageMemory, NULL);
    window->interopImageMemory = VK_NULL_HANDLE;

    return(FALSE);
}

psych_bool PsychRecordCopyCommandBuffer(PsychVulkanWindow* window, int index, psych_bool noInterop)
{
    VkResult result;
    PsychVulkanDevice* vulkan = window->vulkan;
    VkCommandBuffer cmd = window->swapChainCommandBuffers[index];

    // Start the command buffer:
    if (!PsychBeginCommandBuffer(cmd)) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychRecordCopyCommandBuffer(): PsychBeginCommandBuffer() failed for %i'th buffer of window %i.\n", index, window->index);

        return(FALSE);
    }

    if (noInterop) {
        VkImageSubresourceRange range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        VkClearColorValue clearColor = { .float32 = { (index > 1) ? 1.0f : index, (index > 1) ? index - 2 : index, (index > 1) ? 0.0f : index, 1.0f } };
        PsychTransitionImageLayout(cmd, window->interopImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        vkCmdClearColorImage(cmd, window->interopImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &range);
        PsychTransitionImageLayout(cmd, window->interopImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    }

    // Transition target swapChainImage from undefined layout to optimal transfer destination. We do not have any need for the old
    // content of the image buffer, as we will overwrite it with a new image momentarily, so following recommendation in the Vulkan
    // spec, we transition from VK_IMAGE_LAYOUT_UNDEFINED to optimal transfer destination. Transitioning from undefined layout allows
    // the Vulkan driver to simply discard old content - or leave it undefined, and skip an actual conversion operation:
    if (!PsychTransitionImageLayout(cmd, window->swapChainImages[index], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychRecordCopyCommandBuffer(): PsychTransitionImageLayout() to copy target failed for %i'th buffer of window %i.\n", index, window->index);

        return(FALSE);
    }

    // DISABLED:
    // Do pixel format of interop texture image and target swapchain image match?
    // if (window->format != window->interopTextureVkFormat) {
    //
    // Always use vkCmdBlitImage(), because defining dstOffsets the way we do
    // allows to flip the image vertically (ie. upside-down) during the blit.
    // This takes the different origin and y-axis direction of OpenGL vs. Vulkan
    // into account and flips the OpenGL source interop image into the upright
    // orientation for Vulkan:
    if (TRUE) {
        // No: Need pixel color format conversion -> blit image:
        VkImageBlit blitRegion = {
            .srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
            .srcOffsets = {{ 0, 0, 0 }, { window->width, window->height, 1 }},
            .dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
            .dstOffsets = {{ 0, window->height, 0 }, { window->width, 0, 1 }}
        };

        vkCmdBlitImage(cmd, window->interopImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, window->swapChainImages[index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitRegion, VK_FILTER_NEAREST);

        if (verbosity > 4)
            printf("PsychVulkanCore-INFO: PsychRecordCopyCommandBuffer(): Swapchainbuffer %i: Using vkCmdBlitImage() blit for interop -> swapchain transfer.\n", index);
    }
    else {
        // Yes: Can do a memcpy() style copy image in VRAM:
        VkImageCopy copyRegion = {
            .srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
            .srcOffset = { 0, 0, 0 },
            .dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
            .dstOffset = { 0, 0, 0 },
            .extent = { window->width, window->height, 1 },
        };

        vkCmdCopyImage(cmd, window->interopImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, window->swapChainImages[index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

        if (verbosity > 4)
            printf("PsychVulkanCore-INFO: PsychRecordCopyCommandBuffer(): Swapchainbuffer %i: Using vkCmdCopyImage() copy for interop -> swapchain transfer.\n", index);
    }

    // Transition from optimal transfer destination layout to the one needed for onscreen presentation, iow. a scanout capable format:
    if (!PsychTransitionImageLayout(cmd, window->swapChainImages[index], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychRecordCopyCommandBuffer(): PsychTransitionImageLayout() to present source failed for %i'th buffer of window %i.\n", index, window->index);

        return(FALSE);
    }

    // Done recording the buffer:
    result = vkEndCommandBuffer(cmd);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: PsychRecordCopyCommandBuffer(): vkEndCommandBuffer() failed for %i'th buffer of window %i.\n", index, window->index);

        return(FALSE);
    }

    return(TRUE);
}

// Commit current window->currentDisplayHDRMetadata to display output:
psych_bool PsychSetHDRMetaData(PsychVulkanWindow* window)
{
    if (!window->vulkan->hasHDR) {
        if (verbosity > 1)
            printf("PsychVulkanCore-WARNING: Tried to call PsychSetHDRMetaData() on a window attached to a gpu without HDR support! Ignored.\n");

        return(FALSE);
    }

    if (!window->hdrMode) {
        if (verbosity > 1)
            printf("PsychVulkanCore-WARNING: Tried to call PsychSetHDRMetaData() on a window without HDR enabled! Ignored.\n");

        return(FALSE);
    }

    // Commit to swapChain. The update is supposed to happen atomically at next Present. This function
    // sadly doesn't return a result status code:
    fpSetHdrMetadataEXT(window->vulkan->device, 1, &window->swapChain, &window->currentDisplayHDRMetadata);

    if (verbosity > 4) {
        printf("\n");
        printf("PsychVulkanCore-INFO: Set HDR display properties for window %i, frameIndex %i:\n", window->index, window->frameIndex);
        printf("PsychVulkanCore-INFO: Display Gamut  R: [%f, %f]\n", window->currentDisplayHDRMetadata.displayPrimaryRed.x, window->currentDisplayHDRMetadata.displayPrimaryRed.y);
        printf("PsychVulkanCore-INFO: Display Gamut  G: [%f, %f]\n", window->currentDisplayHDRMetadata.displayPrimaryGreen.x, window->currentDisplayHDRMetadata.displayPrimaryGreen.y);
        printf("PsychVulkanCore-INFO: Display Gamut  B: [%f, %f]\n", window->currentDisplayHDRMetadata.displayPrimaryBlue.x, window->currentDisplayHDRMetadata.displayPrimaryBlue.y);
        printf("PsychVulkanCore-INFO: Display Gamut WP: [%f, %f]\n", window->currentDisplayHDRMetadata.whitePoint.x, window->currentDisplayHDRMetadata.whitePoint.y);
        printf("PsychVulkanCore-INFO: Display minLuminance: %f nits\n", window->currentDisplayHDRMetadata.minLuminance);
        printf("PsychVulkanCore-INFO: Display maxLuminance: %f nits\n", window->currentDisplayHDRMetadata.maxLuminance);
        printf("PsychVulkanCore-INFO: Content maxFrameAverageLightLevel: %f nits\n", window->currentDisplayHDRMetadata.maxFrameAverageLightLevel);
        printf("PsychVulkanCore-INFO: Content maxContentLightLevel: %f nits\n", window->currentDisplayHDRMetadata.maxContentLightLevel);
        printf("\n");
    }

    return(TRUE);
}

psych_bool PsychOpenVulkanWindow(PsychVulkanWindow* window, int gpuIndex, psych_uint8* targetdeviceUUID, psych_bool isFullscreen, int screenId, void* outputHandle, PsychRectType rect, int colorPrecision, int hdrMode, double refreshHz, int colorSpace, int colorFormat, int flags)
{
    VkResult result;
    int i;
    psych_bool needsTiming = TRUE;
    psych_bool supportsPresent;
    PsychVulkanDevice* vulkan;
    psych_bool rc = FALSE;
    psych_bool noDirectToDisplay = ((PSYCH_SYSTEM == PSYCH_OSX) && (flags & 0x2)) ? TRUE : FALSE;

    // Any HDR mode requested?
    if (hdrMode > 0) {
        // HDR requires at least 10 bpc framebuffer precision, so enforce at least
        // colorPrecision level 1:
        if (colorPrecision < 1)
            colorPrecision = 1;
    }

    // Automatic colorSpace selection, unless colorSpace already defined by caller:
    if (colorSpace == 0) {
        switch(hdrMode) {
            case 1:
                // HDR-10 BT2020 color space with ST-2084 PQ "Perceptual Quantizer" OETF:
                colorSpace = VK_COLOR_SPACE_HDR10_ST2084_EXT;

                // Unless this is MS-Windows, and fp16 float precision is requested,
                // or windowed non-fullscreen HDR is requested. In that case we need
                // to use the scRGB colorspace, because only scRGB supports fp16
                // consistently across multiple gpu vendors, and only scRGB is
                // supported for windowed HDR, ie. when the DWM compositor is responsible
                // for SDR + HDR compositing. We also use scRGB + fp16 on macOS:
                if (((PSYCH_SYSTEM == PSYCH_WINDOWS) && (!isFullscreen || (colorPrecision > 1))) || (PSYCH_SYSTEM == PSYCH_OSX)) {
                    colorSpace = VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT;

                    // The scRGB colorspace only works with exactly fp16 == precision 2:
                    colorPrecision = 2;
                }

                break;

            case 0:
                // Standard dynamic range SDR default Vulkan SRGB color space and standard
                // transfer function:
                colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
                break;

            default:
                colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        }
    }

    window->isFullscreen = isFullscreen;
    window->createFlags = flags;

    // Specific gpuIndex requested?
    if (gpuIndex > 0) {
        psych_uint8 allzeros[16] = { 0 };

        // Yes. Choose this physical gpu for the window:
        vulkan = PsychGetVulkan(gpuIndex, FALSE);

        if (verbosity > 3)
            printf("PsychVulkanCore-INFO: gpuIndex %i [%s] manually selected as gpu for presenting to the target surface.\n", gpuIndex, vulkan->deviceProps.deviceName);

        // Map a all-zero targetdeviceUUID to a matching UUID, iow. make sure it gets ignored for compatibility matching:
        if (memcmp(targetdeviceUUID, allzeros, 16) == 0)
            memcpy(targetdeviceUUID, &vulkan->physDeviceProps.deviceUUID[0], 16);

        if (!PsychIsVulkanGPUSuitable(window, vulkan, targetdeviceUUID, isFullscreen, screenId, outputHandle, rect, refreshHz, hdrMode, needsTiming, colorPrecision, colorSpace, colorFormat, flags)) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: Creating vulkan output window failed. Selected gpu %i '%s' unsuitable for requested settings.\n", gpuIndex, vulkan->deviceProps.deviceName);

            goto openwindow_out2;
        }

        supportsPresent = TRUE;
    }
    else {
        // No. Try to auto-detect the proper gpuIndex / gpu by probing:
        for (gpuIndex = 1; gpuIndex <= physicalGpuCount; gpuIndex++) {
            vulkan = PsychGetVulkan(gpuIndex, FALSE);
            supportsPresent = PsychIsVulkanGPUSuitable(window, vulkan, targetdeviceUUID, isFullscreen, screenId, outputHandle, rect, refreshHz, hdrMode, needsTiming, colorPrecision, colorSpace, colorFormat, flags);

            if (verbosity > 3)
                printf("PsychVulkanCore-INFO: Does gpu %i [%s] meet our requirements for target surface: %s\n", gpuIndex, vulkan->deviceProps.deviceName, supportsPresent ? "Yes" : "No");

            if (supportsPresent)
                break;
        }

        if (!supportsPresent) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: Could not find any suitable gpu to present to given window for requested settings!\n");

            goto openwindow_out2;
        }

        if (verbosity > 3)
            printf("PsychVulkanCore-INFO: gpuIndex %i [%s] auto-selected as optimal gpu for presenting to the target surface.\n", gpuIndex, vulkan->deviceProps.deviceName);
    }

    if (!supportsPresent)
        goto openwindow_out2;

    // Assign vulkan device to this window:
    window->vulkan = vulkan;

    // Create a swapchain for the windows surface:

    // Select size of swapchain in pixels:
    VkExtent2D swapchainExtent;

    if (window->surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
        // Swapchain pixel size undefined. Assign our window size:
        swapchainExtent.width = rect[kPsychRight] - rect[kPsychLeft];
        swapchainExtent.height = rect[kPsychBottom] - rect[kPsychTop];

        // Clamp extent to surface limits:
        if (swapchainExtent.width < window->surfaceCapabilities.minImageExtent.width)
            swapchainExtent.width = window->surfaceCapabilities.minImageExtent.width;

        if (swapchainExtent.width > window->surfaceCapabilities.maxImageExtent.width)
            swapchainExtent.width = window->surfaceCapabilities.maxImageExtent.width;

        if (swapchainExtent.height < window->surfaceCapabilities.minImageExtent.height)
            swapchainExtent.height = window->surfaceCapabilities.minImageExtent.height;

        if (swapchainExtent.height > window->surfaceCapabilities.maxImageExtent.height)
            swapchainExtent.height = window->surfaceCapabilities.maxImageExtent.height;
    }
    else {
        // Swapchain size defined. Take "as is":
        swapchainExtent = window->surfaceCapabilities.currentExtent;
    }

    // Assign size to window:
    window->width = swapchainExtent.width;
    window->height = swapchainExtent.height;

    // Select number of image buffers:
    uint32_t numBuffers = window->surfaceCapabilities.minImageCount;

    if (numBuffers != 2 && window->surfaceCapabilities.minImageCount <= 2)
        numBuffers = 2;

    if ((numBuffers > window->surfaceCapabilities.maxImageCount) && (window->surfaceCapabilities.maxImageCount != 0))
        numBuffers = window->surfaceCapabilities.maxImageCount;

    if (numBuffers > MAX_BUFFERS) {
        if (verbosity > 1)
            printf("PsychVulkanCore-WARNING: Window %i had more than max allowed buffers (numBuffers=%i vs. MAX_BUFFERS %i). Clamping to MAX_BUFFERS!\n", window->index, numBuffers, MAX_BUFFERS);
        numBuffers = MAX_BUFFERS;
    }

    if ((numBuffers != 2) && (verbosity > 1))
        printf("PsychVulkanCore-WARNING: Window %i does not support strict double-buffering (numBuffers=%i). Expect timing trouble!\n", window->index, numBuffers);

    window->numBuffers = 0;

    // Selection of proper swapchain format:
    window->format = VK_FORMAT_UNDEFINED;

    switch (colorPrecision) {
        case 0: // 8 bpc standard precision:
            // Prefer RGBA8 ordering if supported, as that matches OpenGL RGBA8 for most efficient interop:
            if (PsychIsColorSpaceFormatComboSupported(window, colorSpace, VK_FORMAT_R8G8B8A8_UNORM)) {
                window->format = VK_FORMAT_R8G8B8A8_UNORM;
                break;
            }
            else if (PsychIsColorSpaceFormatComboSupported(window, colorSpace, VK_FORMAT_B8G8R8A8_UNORM) && !noDirectToDisplay) {
                window->format = VK_FORMAT_B8G8R8A8_UNORM; // This is our slightly less efficient fallback, except on macOS Direct-To-Display.
                break;
            }

            // Note that in direct display mode on Linux, NVidia's blob supports VK_FORMAT_A8B8G8R8_UNORM_PACK32
            // as swapChain format, instead of the above formats. However, this format does not work for OpenGL
            // interop for unknown reasons, maybe a NVidia driver bug. Therefore we don't try to use it here.

            // None of the above 8 bpc formats supported! Or on macOS we want to avoid Direct-To-Display to
            // work around macOS operating system bugs.
            // Fallthrough to case 1 RGB10A2 iff none of the above 8 bpc format is supported, as 10 bpc
            // formats are a valid fallback in that case. Potentially less efficient, but compatible in
            // the way they represent color values.

        case 1: // RGB10A2 10 bpc precision:
            // Prefer RGBA ordering if supported, as that matches OpenGL RGB10A2 for most efficient interop,
            // except on macOS, where we need to choose BGR10A2 for optimal interop and if we want Direct-To-Display mode for optimum performance:
            if (PsychIsColorSpaceFormatComboSupported(window, colorSpace, VK_FORMAT_A2B10G10R10_UNORM_PACK32) && ((PSYCH_SYSTEM != PSYCH_OSX) || noDirectToDisplay))
                window->format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
            else if (PsychIsColorSpaceFormatComboSupported(window, colorSpace, VK_FORMAT_A2R10G10B10_UNORM_PACK32))
                window->format = VK_FORMAT_A2R10G10B10_UNORM_PACK32; // This is our slightly less efficient fallback.
            break;

        case 2: // RGBA16F fp 16 floating point precision. This is about 11 bpc for the unorm range 0.0 - 1.0:
            if (PsychIsColorSpaceFormatComboSupported(window, colorSpace, VK_FORMAT_R16G16B16A16_SFLOAT))
                window->format = VK_FORMAT_R16G16B16A16_SFLOAT;
            break;

        case 3: // RGBA16 16 bpc unsigned normalized unorm range 0.0 - 1.0. Up to 16 bit true precision:
            if (PsychIsColorSpaceFormatComboSupported(window, colorSpace, VK_FORMAT_R16G16B16A16_UNORM))
                window->format = VK_FORMAT_R16G16B16A16_UNORM;
            break;

        case 4: // Try all deep-color unorm precisions from highest to lowest: RGBA16 -> RGB10A2:
        case 5: // Try all deep-color precisions (unorm and fp16) from highest to lowest: RGBA16 -> RGBA16F -> RGB10A2:
            // First try RGBA16 16 bpc unsigned normalized unorm range 0.0 - 1.0. Up to 16 bit true precision:
            if (PsychIsColorSpaceFormatComboSupported(window, colorSpace, VK_FORMAT_R16G16B16A16_UNORM)) {
                window->format = VK_FORMAT_R16G16B16A16_UNORM;
                break;
            }

            // Case 5, also use fp16?
            if ((colorPrecision == 5) && (PsychIsColorSpaceFormatComboSupported(window, colorSpace, VK_FORMAT_R16G16B16A16_SFLOAT))) {
                window->format = VK_FORMAT_R16G16B16A16_SFLOAT;
                break;
            }

            // Fall back to 10 bpc formats:

            // Prefer RGBA ordering if supported, as that matches OpenGL RGB10A2 for most efficient interop:
            if (PsychIsColorSpaceFormatComboSupported(window, colorSpace, VK_FORMAT_A2B10G10R10_UNORM_PACK32)) {
                window->format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
                break;
            }

            // Fall back to BGRA 10 bpc ordering:
            if (PsychIsColorSpaceFormatComboSupported(window, colorSpace, VK_FORMAT_A2R10G10B10_UNORM_PACK32)) {
                window->format = VK_FORMAT_A2R10G10B10_UNORM_PACK32; // This is our slightly less efficient fallback.
                break;
            }

            break;

        case 6: // Try fp16, then 10 bpc unorm: RGBA16F -> RGB10A2:
            if (PsychIsColorSpaceFormatComboSupported(window, colorSpace, VK_FORMAT_R16G16B16A16_SFLOAT)) {
                window->format = VK_FORMAT_R16G16B16A16_SFLOAT;
                break;
            }

            // Fall back to 10 bpc formats:

            // Prefer RGBA ordering if supported, as that matches OpenGL RGB10A2 for most efficient interop:
            if (PsychIsColorSpaceFormatComboSupported(window, colorSpace, VK_FORMAT_A2B10G10R10_UNORM_PACK32)) {
                window->format = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
                break;
            }

            // Fall back to BGRA 10 bpc ordering:
            if (PsychIsColorSpaceFormatComboSupported(window, colorSpace, VK_FORMAT_A2R10G10B10_UNORM_PACK32)) {
                window->format = VK_FORMAT_A2R10G10B10_UNORM_PACK32; // This is our slightly less efficient fallback.
                break;
            }

            break;

        default:
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: Unsupported / Unknown color precision type %i requested!\n", colorPrecision);
            goto openwindow_out1;
    }

    // Got something usable?
    if (window->format == VK_FORMAT_UNDEFINED) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: Requested color precision type %i unsupported by the gpuIndex %i [%s]!\n", colorPrecision, gpuIndex, vulkan->deviceProps.deviceName);
        goto openwindow_out1;
    }

    // Assign colorspace to window:
    window->colorspace = colorSpace;

    VkSwapchainCreateInfoKHR swapChainCreateInfo = { 0 };
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = window->surface;
    swapChainCreateInfo.minImageCount = numBuffers;
    swapChainCreateInfo.imageFormat = window->format;
    swapChainCreateInfo.imageColorSpace = colorSpace;
    swapChainCreateInfo.imageExtent = swapchainExtent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.queueFamilyIndexCount = 0;
    swapChainCreateInfo.pQueueFamilyIndices = NULL;
    swapChainCreateInfo.preTransform = window->surfaceCapabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapChainCreateInfo.clipped = VK_TRUE;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    #if PSYCH_SYSTEM == PSYCH_WINDOWS
    swapChainCreateInfo.pNext = (window->isFullscreen) ? &fullscreenExclusiveInfo : NULL;
    #endif

    // Map swapchain format to a compatible OpenGL supported format for interop:
    switch(swapChainCreateInfo.imageFormat) {
        case VK_FORMAT_B8G8R8A8_UNORM:              // RGBA8 interop -> BGRA8 swapchain swizzled, except on macOS:
            window->interopTextureVkFormat = (PSYCH_SYSTEM == PSYCH_OSX) ? VK_FORMAT_B8G8R8A8_UNORM : VK_FORMAT_R8G8B8A8_UNORM;
            window->colorPrecision = 0;

            if (verbosity > 3)
                printf("PsychVulkanCore-INFO: Using 8 bpc unorm [0; 1] range RGBA8 framebuffer.\n");
            break;

        case VK_FORMAT_R8G8B8A8_UNORM:              // RGBA8 interop -> RGBA8 swapchain.
            window->interopTextureVkFormat = VK_FORMAT_R8G8B8A8_UNORM;
            window->colorPrecision = 0;

            if (verbosity > 3)
                printf("PsychVulkanCore-INFO: Using 8 bpc unorm [0; 1] range RGBA8 framebuffer.\n");
            break;

        case VK_FORMAT_A2R10G10B10_UNORM_PACK32: // RGB10A2 interop -> BGR10A2 swapchain swizzled, except on macOS:
            window->interopTextureVkFormat = (PSYCH_SYSTEM == PSYCH_OSX) ? VK_FORMAT_A2R10G10B10_UNORM_PACK32 : VK_FORMAT_A2B10G10R10_UNORM_PACK32;
            window->colorPrecision = 1;

            if (verbosity > 3)
                printf("PsychVulkanCore-INFO: Using 10 bpc unorm [0; 1] range RGB10A2 framebuffer.\n");
            break;

        case VK_FORMAT_A2B10G10R10_UNORM_PACK32: // RGB10A2 interop -> RGB10A2 swapchain, except on macOS where we need to swizzle:
            if ((PSYCH_SYSTEM == PSYCH_OSX) && (colorPrecision == 0) && noDirectToDisplay) {
                // Special case: macOS noDirectToDisplay hack, where we wanted a 8 bpc fb, but can't use it,
                // as it would trigger macOS Metal bugs. We allocate a 10 bpc swapchain buffer, but setup the
                // interop texture as 8 bpc, so Screen's imaging pipeline can potentially avoid an unneeded
                // drawBufferFBO->finalizedFBO unsharing, which would cause a performance hit:
                window->interopTextureVkFormat = VK_FORMAT_B8G8R8A8_UNORM;
                window->colorPrecision = 0;
            }
            else {
                // Standard 10 bpc use case:
                window->interopTextureVkFormat = (PSYCH_SYSTEM == PSYCH_OSX) ? VK_FORMAT_A2R10G10B10_UNORM_PACK32 : VK_FORMAT_A2B10G10R10_UNORM_PACK32;
                window->colorPrecision = 1;
            }

            if (verbosity > 3)
                printf("PsychVulkanCore-INFO: Using 10 bpc unorm [0; 1] range RGB10A2 framebuffer.\n");
            break;

        case VK_FORMAT_R16G16B16A16_SFLOAT: // RGBA16F interop -> RGBA16F swapchain.
            window->interopTextureVkFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
            window->colorPrecision = 2;

            if (verbosity > 3)
                printf("PsychVulkanCore-INFO: Using 16 bit (~11 bpc unorm linear precision equivalent) fp16 half-float range RGBA16F framebuffer.\n");
            break;

        case VK_FORMAT_R16G16B16A16_UNORM: // RGBA16 interop -> RGBA16 swapchain.
            window->interopTextureVkFormat = VK_FORMAT_R16G16B16A16_UNORM;
            window->colorPrecision = 3;

            if (verbosity > 3)
                printf("PsychVulkanCore-INFO: Using 16 bpc unorm [0; 1] range RGBA16 framebuffer.\n");
            break;

        default:
            printf("PsychVulkanCore-CRITICAL: Unrecognized swapChain format 0x%x for window %i! Driver bug?!?\n", swapChainCreateInfo.imageFormat, window->index);
            goto openwindow_out1;
    }

    result = vkCreateSwapchainKHR(vulkan->device, &swapChainCreateInfo, NULL, &window->swapChain);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: vkCreateSwapchainKHR() failed for window %i: res=%i.\n", window->index, result);

        goto openwindow_out1;
    }

    // Some time granted to GUI event dispatch:
    PsychProcessWindowEvents(window);

    #if defined(VK_USE_PLATFORM_WIN32_KHR)
        if (isFullscreen && !(window->createFlags & 0x2)) {
            // Switch swapChain into fullScreenExclusive mode:
            int retries;

            // Retry to go fullscreen exclusive up to 10 times:
            result = VK_ERROR_INITIALIZATION_FAILED;
            for (retries = 0; (retries < 10) && (result == VK_ERROR_INITIALIZATION_FAILED); retries++) {
                result = fpAcquireFullScreenExclusiveModeEXT(vulkan->device, window->swapChain);
                if (result != VK_SUCCESS) {
                    if (verbosity > 0) {
                        if (result == VK_ERROR_INITIALIZATION_FAILED)
                            printf("PsychVulkanCore-ERROR: gpu [%s] Could not switch to fullscreen exclusive mode [Attempt nr. %i]!\n", vulkan->deviceProps.deviceName, retries + 1);
                        else
                            printf("PsychVulkanCore-ERROR: gpu [%s] Error during switch to fullscreen exclusive mode [Attempt nr. %i]: %i\n", vulkan->deviceProps.deviceName, result + 1);
                    }
                }
                else if (verbosity > 3)
                    printf("PsychVulkanCore-INFO: For gpu [%s] switched to fullscreen exclusive display mode for swapChain [%p] of display window %i\n", vulkan->deviceProps.deviceName, window->swapChain, window->index);

                PsychProcessWindowEvents(window);
                PsychYieldIntervalSeconds(0.050);
            }

            // Still no success?
            if (result != VK_SUCCESS) {
                // This is not a fatal error, but it may impair timing and reliability:
                if (verbosity > 0)
                    printf("PsychVulkanCore-ERROR: Multiple retries failed. Fallback to non-exclusive mode. Visual timing precision, robustness, and general reliability may be degraded!\n");

                // Destroy and recreate swapChain without fullscreen exclusive access:
                vkDestroySwapchainKHR(vulkan->device, window->swapChain, NULL);
                window->swapChain = (VkSwapchainKHR) VK_NULL_HANDLE;
                swapChainCreateInfo.pNext = NULL;

                result = vkCreateSwapchainKHR(vulkan->device, &swapChainCreateInfo, NULL, &window->swapChain);
                if (result != VK_SUCCESS) {
                    if (verbosity > 0)
                        printf("PsychVulkanCore-ERROR: vkCreateSwapchainKHR() failed for window %i in non-FS-except mode: res=%i.\n", window->index, result);

                    goto openwindow_out1;
                }
            }
        }
        else if (isFullscreen && verbosity > 1) {
            printf("PsychVulkanCore-WARNING: For gpu [%s] did *not* switch to fullscreen exclusive display mode for fullscreen display window %i,\n", vulkan->deviceProps.deviceName, window->index);
            printf("PsychVulkanCore-WARNING: because of user request, or due to automatic workaround for buggy Vulkan driver.\n");
            printf("PsychVulkanCore-WARNING: Visual timing precision and robustness, as well as general stimulus reliability, may be degraded!\n");
        }
    #endif

    result = vkGetSwapchainImagesKHR(vulkan->device, window->swapChain, &window->numBuffers, NULL);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: vkGetSwapchainImagesKHR() failed for window %i: res=%i.\n", window->index, result);

        goto openwindow_out1;
    }

    if ((window->numBuffers != numBuffers) && (verbosity > 1))
        printf("PsychVulkanCore-WARNING: Window %i has different number of swapchain images %i than requested %i. Expect timing trouble!\n",
               window->index, window->numBuffers, numBuffers);

    window->swapChainImages = (VkImage*) malloc(window->numBuffers * sizeof(VkImage));

    result = vkGetSwapchainImagesKHR(vulkan->device, window->swapChain, &window->numBuffers, window->swapChainImages);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: vkGetSwapchainImagesKHR() of %i images failed for window %i: res=%i.\n", window->numBuffers, window->index, result);

        goto openwindow_out1;
    }

    const VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    // Create semaphores for each swapchain image:
    for (i = 0; i < window->numBuffers; i++) {
        result = vkCreateSemaphore(vulkan->device, &semaphoreCreateInfo, NULL, &window->imageAcquiredSemaphores[i]);
        if (result != VK_SUCCESS) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: vkCreateSemaphore() for %i'th swapchain image acquire of %i failed for window %i: res=%i.\n", i, window->numBuffers, window->index, result);

            goto openwindow_out1;
        }

        result = vkCreateSemaphore(vulkan->device, &semaphoreCreateInfo, NULL, &window->imageCopyDoneSemaphores[i]);
        if (result != VK_SUCCESS) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: vkCreateSemaphore() for %i'th swapchain image copy done of %i failed for window %i: res=%i.\n", i, window->numBuffers, window->index, result);

            goto openwindow_out1;
        }
    }

    const VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    };

    // Create flipDoneFence:
    result = vkCreateFence(vulkan->device, &fenceCreateInfo, NULL, &window->flipDoneFence);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: vkCreateFence() for failed for window %i: res=%i.\n", window->index, result);

        goto openwindow_out1;
    }

    // Create command buffers, one for each swapchain image, to store the interop -> swapchain copy/blit commands
    // and layout transitions:
    VkCommandBufferAllocateInfo commandBufferInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = vulkan->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = window->numBuffers,
    };

    result = vkAllocateCommandBuffers(vulkan->device, &commandBufferInfo, &window->swapChainCommandBuffers[0]);
    if (result != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: vkAllocateCommandBuffers() for command buffers for %i swapchain images failed for window %i: res=%i.\n", window->numBuffers, window->index, result);

        goto openwindow_out1;
    }

    // Create OpenGL -> Vulkan interop texture image:
    VkFormatProperties formatProps;
    VkFormatFeatureFlags requiredMask = VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT;
    vkGetPhysicalDeviceFormatProperties(vulkan->physicalDevice, window->interopTextureVkFormat, &formatProps);

    switch (vulkan->deviceProps.vendorID) {
        case 0x8086: // Intel gpu: Verified to work with tiled rendering on Iris as of Mesa 21.0.0-devel + OpenGL interop patchset.
        case 0x10de: // NVidia gpu: At least proprietary drivers allow tiled rendering:
        case 0x14e4: // Broadcom gpu: Verified to work with tiled rendering on VideoCore-6 / RaspberryPi 4/400.
            if ((formatProps.optimalTilingFeatures & requiredMask) == requiredMask) {
                window->interopTextureTiled = TRUE;
                break;
            }
            // If tiling is not supported for some reason, fallthrough to linear mode:

        case 0x1002: // AMD gpu: AMD OpenGL drivers currently only allow rendering into linear non-tiled interop texture.
        default:
            if ((formatProps.linearTilingFeatures & requiredMask) == requiredMask) {
                window->interopTextureTiled = FALSE;
            }

            break;
    }

    // macOS MoltenVK can only do tiled rendering:
    if ((PSYCH_SYSTEM == PSYCH_OSX) && ((formatProps.optimalTilingFeatures & requiredMask) == requiredMask))
        window->interopTextureTiled = TRUE;

    if (!PsychCreateInteropTexture(window)) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: Creating OpenGL -> Vulkan interop texture failed for window %i.\n", window->index);

        goto openwindow_out1;
    }

    // Build command buffers for interop image -> swapchain copy/blit operation:
    for (i = 0; i < window->numBuffers; i++) {
        // flags bit 0 (value 0x1) set means to disable interop and just draw hard-coded test patterns:
        if (!PsychRecordCopyCommandBuffer(window, i, flags & 0x1)) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: Building command buffer for %i'th swapchain image copy/blit out of %i failed for window %i: res=%i.\n", i, window->numBuffers, window->index, result);

            goto openwindow_out1;
        }
    }

    // Some time granted to GUI event dispatch:
    PsychProcessWindowEvents(window);

    // Prep work for first true Present - One-time warmup after creating the swapChain.
    // Try to acquire next target swapchain buffer window->currentSwapChainBuffer will
    // contain the index of the proper swapChainImage:
    if (PsychGetNextSwapChainTargetBuffer(window) != VK_SUCCESS) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: Warmup stage 1 failed for window %i.\n", window->index);

        goto openwindow_out1;
    }

    if (!PsychWaitForPresentCompletion(window)) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: Warmup stage 2 failed for window %i.\n", window->index);

        goto openwindow_out1;
    }

    // Enable HDR metadata packet transmission to display output:
    window->hdrMode = hdrMode;
    if (hdrMode && !PsychSetHDRMetaData(window)) {
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: Failed to enable HDR mode %i for window %i.\n", hdrMode, window->index);

        goto openwindow_out1;
    }

    // Report nominal refresh rate of display if this is supported:
    if ((verbosity > 3) && vulkan->hasTiming && fpGetRefreshCycleDurationGOOGLE) {
        VkRefreshCycleDurationGOOGLE refreshDur;
        if (VK_SUCCESS == fpGetRefreshCycleDurationGOOGLE(vulkan->device, window->swapChain, &refreshDur)) {
            printf("PsychVulkanCore-INFO: Vulkan reports nominal refresh rate %f Hz for display associated with window %i.\n", 1.0e9 / (double) refreshDur.refreshDuration, window->index);
        } else {
            printf("PsychVulkanCore-INFO: vkGetRefreshCycleDurationGOOGLE() for window %i failed.\n", window->index);
        }
    }

    // Success: Mark it as such:
    rc = TRUE;

    window->isValid = TRUE;

    // Some time granted to GUI event dispatch:
    PsychProcessWindowEvents(window);

    return(TRUE);

openwindow_out1:

    // Free all command buffers:
    vkFreeCommandBuffers(vulkan->device, vulkan->commandPool, window->numBuffers, &window->swapChainCommandBuffers[0]);

    vkDestroyImage(vulkan->device, window->interopImage, NULL);
    window->interopImage = VK_NULL_HANDLE;

    vkFreeMemory(vulkan->device, window->interopImageMemory, NULL);
    window->interopImageMemory = VK_NULL_HANDLE;

    vkDestroyFence(vulkan->device, window->flipDoneFence, NULL);

    for (i = 0; i < MAX_BUFFERS; i++) {
        vkDestroySemaphore(vulkan->device, window->imageAcquiredSemaphores[i], NULL);
        window->imageAcquiredSemaphores[i] = VK_NULL_HANDLE;

        vkDestroySemaphore(vulkan->device, window->imageCopyDoneSemaphores[i], NULL);
        window->imageCopyDoneSemaphores[i] = VK_NULL_HANDLE;
    }

    if (window->swapChainImages) {
        free(window->swapChainImages);
        window->swapChainImages = NULL;
        window->numBuffers = 0;
    }

    vkDestroySwapchainKHR(vulkan->device, window->swapChain, NULL);
    window->swapChain = (VkSwapchainKHR) VK_NULL_HANDLE;

openwindow_out2:

    vkDestroySurfaceKHR(vulkanInstance, window->surface, NULL);
    window->surface = (VkSurfaceKHR) VK_NULL_HANDLE;

    if (window->display != VK_NULL_HANDLE) {
        fpReleaseDisplayEXT(vulkan->physicalDevice, window->display);
        window->display = VK_NULL_HANDLE;
    }

#if defined(VK_USE_PLATFORM_XLIB_KHR)
    if (window->connection) {
        if (window->x11PrivateWindow != None) {
            XUnmapWindow(window->connection, window->x11PrivateWindow);
            XDestroyWindow(window->connection, window->x11PrivateWindow);
            XFlush(window->connection);
            window->x11PrivateWindow = None;
        }

        XCloseDisplay(window->connection);
        window->connection = NULL;
    }
#endif

#if defined(VK_USE_PLATFORM_WIN32_KHR)
    if (window->win32PrivateWindow) {
        DestroyWindow(window->win32PrivateWindow);
        window->win32PrivateWindow = NULL;
    }
#endif

    return (rc);
}

psych_bool PsychCloseVulkanWindow(PsychVulkanWindow* window)
{
    int i;

    if (!window->isValid)
        return(TRUE);

    if (verbosity > 3) {
        printf("PsychVulkanCore-INFO: Closing Vulkan window %i.\n", window->index);
        fflush(NULL);
    }

    // Clean up / Reset from previous call if needed:
    if (window->surfaceFormats) {
        free(window->surfaceFormats);
        window->surfaceFormats = NULL;
        window->surfaceFormatCount = 0;
    }

    // Some time granted to GUI event dispatch:
    PsychProcessWindowEvents(window);

    vkDeviceWaitIdle(window->vulkan->device);

    // Free all command buffers:
    vkFreeCommandBuffers(window->vulkan->device, window->vulkan->commandPool, window->numBuffers, &window->swapChainCommandBuffers[0]);

    vkDestroyImage(window->vulkan->device, window->interopImage, NULL);
    window->interopImage = VK_NULL_HANDLE;

    // Do we still own the interop handle for memory?
    if (window->interopHandles.memory != 0) {
        // Need to release it:
        if (verbosity > 4) {
            printf("PsychVulkanCore-INFO: Vulkan window %i: Releasing image memory interop handle.\n", window->index);
        }

        #if PSYCH_SYSTEM == PSYCH_LINUX
            close(window->interopHandles.memory);
        #endif

        #if PSYCH_SYSTEM == PSYCH_WINDOWS
            CloseHandle(window->interopHandles.memory);
        #endif

        #if PSYCH_SYSTEM == PSYCH_OSX
            glDeleteTextures(1, (GLuint*) &window->interopHandles.memory);
        #endif

        window->interopHandles.memory = 0;
    }

    vkFreeMemory(window->vulkan->device, window->interopImageMemory, NULL);
    window->interopImageMemory = VK_NULL_HANDLE;

    vkDestroyFence(window->vulkan->device, window->flipDoneFence, NULL);

    vkDestroySemaphore(window->vulkan->device, window->interopRenderDoneSemaphore, NULL);
    window->interopRenderDoneSemaphore = VK_NULL_HANDLE;

    for (i = 0; i < MAX_BUFFERS; i++) {
        vkDestroySemaphore(window->vulkan->device, window->imageAcquiredSemaphores[i], NULL);
        window->imageAcquiredSemaphores[i] = VK_NULL_HANDLE;

        vkDestroySemaphore(window->vulkan->device, window->imageCopyDoneSemaphores[i], NULL);
        window->imageCopyDoneSemaphores[i] = VK_NULL_HANDLE;
    }

    if (window->swapChainImages) {
        free(window->swapChainImages);
        window->swapChainImages = NULL;
        window->numBuffers = 0;
    }

    vkDestroySwapchainKHR(window->vulkan->device, window->swapChain, NULL);
    window->swapChain = (VkSwapchainKHR) VK_NULL_HANDLE;

    vkDeviceWaitIdle(window->vulkan->device);

    if (verbosity > 4) {
        printf("PsychVulkanCore-INFO: Vulkan window %i: swapChain is gone. Releasing display.\n", window->index);
        fflush(NULL);
    }

    if (window->display != VK_NULL_HANDLE) {
        fpReleaseDisplayEXT(window->vulkan->physicalDevice, window->display);
        window->display = VK_NULL_HANDLE;
    }

    #if defined(VK_USE_PLATFORM_XLIB_KHR)
    if (window->connection) {
        if (window->x11PrivateWindow != None) {
            XUnmapWindow(window->connection, window->x11PrivateWindow);
            XDestroyWindow(window->connection, window->x11PrivateWindow);
            XFlush(window->connection);
            window->x11PrivateWindow = None;
        }

        XCloseDisplay(window->connection);
        window->connection = NULL;
    }
    #endif

    #if defined(VK_USE_PLATFORM_WIN32_KHR)
    if (window->win32PrivateWindow) {
        DestroyWindow(window->win32PrivateWindow);
        window->win32PrivateWindow = NULL;
    }
    #endif

    if (verbosity > 4) {
        printf("PsychVulkanCore-INFO: Vulkan window %i: Display or Window is gone. Releasing surface.\n", window->index);
        fflush(NULL);
    }

    vkDestroySurfaceKHR(vulkanInstance, window->surface, NULL);
    window->surface = (VkSurfaceKHR) VK_NULL_HANDLE;

    vkDeviceWaitIdle(window->vulkan->device);

    if (verbosity > 4) {
        printf("PsychVulkanCore-INFO: Vulkan window %i: Window successfully closed.\n", window->index);
        fflush(NULL);
    }

    // Invalidate window -- Mark it as dead in the window array:
    window->isValid = FALSE;

    // One less:
    windowCount--;

    return(TRUE);
}

void PsychCloseAllVulkanWindows(void)
{
    int handle;
    PsychVulkanWindow* window;

    for (handle = 0; handle < MAX_PSYCH_VULKAN_WINDOWS; handle++) {
        window = PsychGetVulkanWindow(handle, TRUE);
        if (window)
            PsychCloseVulkanWindow(window);
    }
}

PsychError PSYCHVULKANVerbosity(void)
{
    static char useString[] = "oldVerbosity = PsychVulkanCore('Verbosity' [, verbosity]);";
    static char synopsisString[] =
        "Returns and optionally sets level of 'verbosity' for driver debug output.\n"
        "'verbosity' = New level of verbosity: 0 = Silent, 1 = Errors only, 2 = Warnings, 3 = Info, 4 = Debug.\n";
    static char seeAlsoString[] = "";

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Return optional old verbosity setting:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, verbosity);

    // Get optional new verbosity setting:
    PsychCopyInIntegerArg(1, kPsychArgOptional, &verbosity);

    return(PsychError_none);
}

PsychError PSYCHVULKANGetCount(void)
{
    static char useString[] = "numDevices = PsychVulkanCore('GetCount');";
    static char synopsisString[] =
        "Returns count of currently available Vulkan driver + GPU combos.\n";
    static char seeAlsoString[] = "GetDevices";

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));

    PsychVulkanCheckInit(TRUE);

    PsychCopyOutDoubleArg(1, kPsychArgOptional, physicalGpuCount);

    return(PsychError_none);
}

PsychError PSYCHVULKANGetDevices(void)
{
    static char useString[] = "devices = PsychVulkanCore('GetDevices');";
    static char synopsisString[] =
    "Returns an array of structs enumerating all currently available Vulkan driver + GPU combos.\n"
    "Each struct in the array describes one combination of Vulkan driver and device, with the "
    "following fields:\n\n"
    "'DeviceIndex' = Index of the device. Can be used in 'OpenWindow' as 'gpuIndex'.\n"
    "'GpuName' = Name of the Vulkan device / graphics card.\n"
    "'GpuDriver' = Name of the Vulkan driver associated with the device.\n"
    "'DriverInfo' = Additional info associated with the driver, vendor specific.\n"
    "'DriverVersion' = Major.minor.point version string with the driver version.\n"
    "'DriverVersionRaw' = Raw scalar Vulkan driver version.\n"
    "'DriverId' = Id of the type of Vulkan driver: 1 = AMD proprietary, 2 = AMD open\n"
    "             source amdvlk, 3 = AMD OSS Mesa radv, 4 = NVidia proprietary,\n"
    "             5 = Intel proprietary Windows, 6 = Intel OSS Mesa anvil,\n"
    "             13 = Mesa OSS llvmpipe.\n"
    "'VendorID' = Vulkan device vendor id. Typically the PCI vendor id.\n"
    "'DeviceID' = Vulkan device id. Typically the PCI device id.\n"
    "'VulkanVersion' = Major.minor.point version of supported Vulkan api.\n"
    "'GpuType' = Type of device: 0 = Unknown/Other, 1 = Integrated gpu,\n"
    "            2 = Discrete gpu, 3 = Virtual gpu, 4 = cpu, ie. software renderer.\n\n"
    "'SupportsHDR' = Does the gpu support driving HDR displays in principle?\n"
    "                0 = No, 1 = Basic HDR-10.\n"
    "'SupportsTiming' = Does the gpu support high precision/reliability timing\n"
    "                   extensions. 0 = No, 1 = Yes. If the driver does not support\n"
    "                   timing extensions, the driver will fall back to hacks.\n"
    "'SupportsWait' = Does the gpu support waiting for present completion: 0 = No, 1 = Yes.\n"
    "\n";
    static char seeAlsoString[] = "GetCount";

    int i;
    char tmp[64];
    PsychGenericScriptType *s;
    const char *fieldNames[] = { "DeviceIndex", "GpuName", "GpuDriver", "DriverInfo", "DriverVersion", "DriverVersionRaw", "DriverId", "VendorId", "DeviceId", "VulkanVersion", "GpuType",
                                 "SupportsHDR", "SupportsTiming", "SupportsWait" };
    const int fieldCount = 14;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(0));

    PsychVulkanCheckInit(FALSE);

    // Create a structure and populate it.
    PsychAllocOutStructArray(1, kPsychArgOptional, physicalGpuCount, fieldCount, fieldNames, &s);

    for (i = 0; i < physicalGpuCount; i++) {
        PsychVulkanDevice* vulkan = PsychGetVulkan(i + 1, FALSE);
        PsychSetStructArrayDoubleElement("DeviceIndex", i, i + 1, s);
        PsychSetStructArrayStringElement("GpuName", i, vulkan->deviceProps.deviceName, s);
        PsychSetStructArrayStringElement("GpuDriver", i, vulkan->driverProps.driverName, s);
        PsychSetStructArrayStringElement("DriverInfo", i, vulkan->driverProps.driverInfo, s);
        sprintf(tmp, "%i.%i.%i", VK_VERSION_MAJOR(vulkan->deviceProps.driverVersion), VK_VERSION_MINOR(vulkan->deviceProps.driverVersion), VK_VERSION_PATCH(vulkan->deviceProps.driverVersion));
        PsychSetStructArrayStringElement("DriverVersion", i, tmp, s);
        PsychSetStructArrayDoubleElement("DriverVersionRaw", i, vulkan->deviceProps.driverVersion, s);
        PsychSetStructArrayDoubleElement("DriverId", i, vulkan->driverProps.driverID, s);
        PsychSetStructArrayDoubleElement("VendorId", i, vulkan->deviceProps.vendorID, s);
        PsychSetStructArrayDoubleElement("DeviceId", i, vulkan->deviceProps.deviceID, s);
        sprintf(tmp, "%i.%i.%i", VK_VERSION_MAJOR(vulkan->deviceProps.apiVersion), VK_VERSION_MINOR(vulkan->deviceProps.apiVersion), VK_VERSION_PATCH(vulkan->deviceProps.apiVersion));
        PsychSetStructArrayStringElement("VulkanVersion", i, tmp, s);
        PsychSetStructArrayDoubleElement("GpuType", i, vulkan->deviceProps.deviceType, s);
        PsychSetStructArrayDoubleElement("SupportsHDR", i, vulkan->hasHDR, s);
        PsychSetStructArrayDoubleElement("SupportsTiming", i, vulkan->hasTiming, s);
        PsychSetStructArrayDoubleElement("SupportsWait", i, vulkan->hasWait, s);
    }

    return(PsychError_none);
}

PsychError PSYCHVULKANCloseWindow(void)
{
    static char useString[] = "PsychVulkanCore('CloseWindow' [, vulkanWindow]);";
    static char synopsisString[] =
        "Close Vulkan presentation window 'vulkanWindow'.\n"
        "If 'vulkanWindow' is omitted then all Vulkan windows are closed.\n";
    static char seeAlsoString[] = "OpenWindow";

    int handle;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure Vulkan api is initialized, fail if not:
    PsychVulkanCheckInit(FALSE);

    // Get window index, if any:
    if (PsychCopyInIntegerArg(1, kPsychArgOptional, &handle)) {
        // Get the window:
        PsychVulkanWindow* window = PsychGetVulkanWindow(handle, FALSE);

        // Close it:
        PsychCloseVulkanWindow(window);
    }
    else {
        // No handle given: Close all Vulkan windows:
        PsychCloseAllVulkanWindows();
    }

    return(PsychError_none);
}

PsychError PSYCHVULKANOpenWindow(void)
{
    static char useString[] = "vulkanWindow = PsychVulkanCore('OpenWindow', gpuIndex, targetUUID, isFullscreen, screenId, rect, outputHandle, hdrMode, colorPrecision, refreshHz, colorSpace, colorFormat, flags);";
    //                         1                                            1         2           3             4         5     6             7        8               9          10          11           12
    static char synopsisString[] =
        "Open a display window on a Vulkan device.\n\n"
        "'gpuIndex' is the index of the Vulkan gpu that should be used for displaying the window. Devices "
        "are numbered 1 to PsychVulkanCore('GetCount'). A value of zero means to auto-detect the proper "
        "gpu + driver combo, given all other passed in parameters.\n"
        "'targetUUID' A 16 byte uint8() vector with the unique device id UUID which identifies the gpu hardware. "
        "This is needed for OpenGL + Vulkan interoperation, so Screen() can pass images for presentation to this "
        "driver. The UUID of the OpenGL gpu used by Screen() for rendering and the Vulkan gpu used for presentation "
        "must match for this to work, hence Screen() needs to provide the proper 'targetUUID' to use here. "
        "The proper 'targetUUID' can be set as winfo.GLDeviceUUID with winfo queried as winfo = Screen('GetWindowInfo', window) "
        "for a given Screen onscreen 'window'. The special targetUUID = zeros(1, 16, 'uint8'); can be used to match any gpu.\n"
        "'isFullscreen' Open a fullscreen exclusive window if set to 1, a windowed window if set to 0. Proper presentation "
        "timing and timestamping, HDR display and similar is often only possible with fullscreen exclusive mode.\n"
        "'screenId' The 'screenId' of the presentation monitor or screen, as used in Screen('Openwindow', screenId) ...\n"
        "'rect' The [left, top, right, bottom] bounding rectangle of the presentation windows client area. Should match "
        "the rectangle covered of the desktop or screen of a display monitor for fullscreen exclusive mode. Defines the "
        "window location in windowed mode.\n"
        "'outputHandle' Handle defining the video output surface to use, in an operating system dependent manner: On MS-Windows "
        "the parameter is currently ignored. On Linux X11 in windowed mode, it must be the X11 window handle of the window to "
        "which we want to display (cfe. Screen('GetWindowInfo', ...);. On Linux in X11 fullscreen mode, it must be the RandR "
        "output XID for the video output to which we want to display in fullscreen mode.\n"
        "'hdrMode' Which HDR display mode, if any, to set up for: 0 = No HDR mode, SDR display. 1 = HDR-10. This will also "
        "determine presets for 'colorPrecision', 'colorSpace' and 'colorFormat' depending on selected mode, if those arguments "
        "are set to auto-select. E.g., hdrMode == 1 for HDR-10 enforces at least 'colorPrecision' 1 for at least 10 bpc output, "
        "and selects a HDR10 color space with BT2020 color gamut and SMPTE ST-2084 PQ eotf encoding.\n"
        "'colorPrecision' Required output framebuffer (swapchain) precision. For HDR display modes, will be forced to at least 1 "
        "for at least 10 bpc precision:\n"
        "0 = 8 bpc fixed point linear RGBA8, unorm\n"
        "1 = 10 bpc fixed point linear RGB10A2, unorm\n"
        "2 = fp16 RGBA16F half-float\n"
        "3 = 16 bpc fixed point linear RGBA16, unorm\n"
        "4 = Try all deep-color unorm precisions from highest to lowest: RGBA16 -> RGB10A2\n"
        "\n"
        "5 = Try all deep-color precisions (unorm and fp16) from highest to lowest: RGBA16 -> RGBA16F -> RGB10A2\n"
        "6 = Try fp16, then 10 bpc unorm: RGBA16F -> RGB10A2\n"
        "Values 0 - 3 request the exact precision, and fail window creation if the system does not support them.\n"
        "Values 4 - 6 request the highest precision, then fall back to lower precisions if the system does not support them, "
        "and fail if the lowest precision in the sequence is not supported either. E.g., A value of 6 would aim for fp16, for "
        "more than 10 bpc effective output precision, but fall back to 10 bpc if fp16 is not supported. It would abort though "
        "if 10 bpc would not be supported either. This is a good choice for HDR display modes with the highest possible "
        "precision that a given operating system + Vulkan driver + gpu + display cable + display combination supports.\n"
        "'refreshHz' The desired video refresh rate for fullscreen windows. May have no effect in windowed mode.\n"
        "'colorSpace' The output color space to assume / select as VkColorSpace id. If zero, then will be set automatically according to 'hdrMode', e.g., "
        "VK_COLOR_SPACE_SRGB_NONLINEAR_KHR in hdrMode 0, VK_COLOR_SPACE_HDR10_ST2084_EXT in hdrMode 1 (=HDR10).\n"
        "'colorFormat' NOT USED YET! The pixel output color format to use as VkFormat color format id. If empty, then will be set automatically according to 'colorPrecision' and/or 'hdrMode'.\n"
        "'flags' Special mode selection flags or'ed together: +1 = Diagnostic display only, no Screen() OpenGL interop, just show an alternating black-white test image. Useful "
        "for most basic Vulkan testing and driver bringup if the given gpu does not have graphics drivers with OpenGL+Vulkan interop capabilities yet.\n"
        "+2 = Do not switch to fullscreen-exclusive mode on MS-Windows, even for fullscreen windows. This is useful as workaround for some buggy Vulkan drivers.\n"
        "\n\n"
        "Returns: The 'vulkanWindow' handle of the Vulkan presentation window.\n";

    static char seeAlsoString[] = "CloseWindow";

    psych_uint8* targetdeviceUUID;
    int gpuIndex, screenId;
    int isFullscreen;
    int colorPrecision;
    int hdrMode;
    int m, n, p;
    double* mat;
    PsychRectType rect;
    void* outputHandle;
    double refreshHz;
    int colorSpace;
    int colorFormat;
    int flags;
    int handle;
    PsychVulkanWindow* window;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(12));
    PsychErrorExit(PsychRequireNumInputArgs(12));

    // Make sure Vulkan api is initialized, fail if not:
    PsychVulkanCheckInit(FALSE);

    // Get mandatory GPU index:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &gpuIndex);

    // Get deviceUUID 16 Bytes long:
    PsychAllocInUnsignedByteMatArg(2, kPsychArgRequired, &m, &n, &p, &targetdeviceUUID);
    if (m != 1 || n != 16 || p != 1)
        PsychErrorExitMsg(PsychError_invalidRectArg, "Invalid targetUUID given. Must be a 16 Byte row vector.");

    // Get mandatory fullscreen flag:
    PsychCopyInIntegerArg(3, kPsychArgRequired, &isFullscreen);

    // Get mandatory Psychtoolbox style screenId:
    PsychCopyInIntegerArg(4, kPsychArgRequired, &screenId);

    // Get mandatory position and size rectangle:
    PsychAllocInDoubleMatArg(5, kPsychArgRequired, &m, &n, &p, &mat);
    if (m != 1 || n != 4 || p != 1)
        PsychErrorExitMsg(PsychError_invalidRectArg, "Invalid window rect given.");
    memcpy(rect, mat, sizeof(PsychRectType));

    // Get mandatory monitor output handle:
    PsychCopyInPointerArg(6, kPsychArgRequired, &outputHandle);

    // Get mandatory hdrMode:
    PsychCopyInIntegerArg(7, kPsychArgRequired, &hdrMode);

    // Get mandatory color precision flag:
    PsychCopyInIntegerArg(8, kPsychArgRequired, &colorPrecision);

    // Get mandatory refresh rate in Hz:
    PsychCopyInDoubleArg(9, kPsychArgRequired, &refreshHz);

    // Get mandatory color space selector:
    PsychCopyInIntegerArg(10, kPsychArgRequired, &colorSpace);

    // Get mandatory color format selector:
    PsychCopyInIntegerArg(11, kPsychArgRequired, &colorFormat);

    // Get mandatory flags:
    PsychCopyInIntegerArg(12, kPsychArgRequired, &flags);

    // Find a free window slot:
    for (handle = 1; (handle <= MAX_PSYCH_VULKAN_WINDOWS) && vulkanWindows[handle - 1].isValid; handle++);
    if (handle > MAX_PSYCH_VULKAN_WINDOWS)
        PsychErrorExitMsg(PsychError_toomanyWin, "Too many Vulkan output display windows open.");

    // Get vulkanWindow record for free slot:
    window = &vulkanWindows[handle - 1];

    // Zero-init:
    memset(window, 0, sizeof(*window));

    // Assign window index already for diagnostic purpose in called routines:
    window->index = handle;

    if (!PsychOpenVulkanWindow(window, gpuIndex, targetdeviceUUID, isFullscreen, screenId, outputHandle, rect, colorPrecision, hdrMode, refreshHz, colorSpace, colorFormat, flags))
        PsychErrorExitMsg(PsychError_user, "Failed to open vulkan output window.");

    windowCount++;

    PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) handle);

    return(PsychError_none);
}

PsychError PSYCHVULKANGetHDRProperties(void)
{
    static char useString[] = "hdr = PsychVulkanCore('GetHDRProperties', vulkanWindow);";
    static char synopsisString[] =
    "Return HDR properties of Vulkan presentation window 'vulkanWindow'.\n"
    "'hdr' is a struct with information about the HDR properties and settings "
    "for the display and window represented by 'vulkanWindow'. The following "
    "fields are defined:\n"
    "'GPUIndex' = Index of the Vulkan device used for this window.\n"
    "'Valid' = Are the HDR display properties valid? 0 = No, as no data could be "
    "queried from the display, 1 = Yes, data has been queried from display and is "
    "supposed to represent actual display HDR capabilities and properties.\n"
    "'HDRMode' 0 = None (SDR), 1 = Basic HDR-10 enabled with BT-2020 color "
    "space, 10 bpc color precision, and ST-2084 PQ Perceptual Quantizer EOTF.\n"
    "'LocalDimmingControl' 0 = No, 1 = Local dimming control supported.\n"
    "'MinLuminance' Minimum supported luminance in nits.\n"
    "'MaxLuminance' Maximum supported peak / burst luminance in nits.\n"
    "'MaxFrameAverageLightLevel' Maximum sustainable supported luminance in nits.\n"
    "'MaxContentLightLevel' Maximum desired content light level in nits.\n"
    "'ColorGamut' A 2-by-4 matrix encoding the CIE-1931 2D chromaticity coordinates of the "
    "red, green, and blue color primaries in columns 1, 2 and 3, and the white-point "
    "in column 4.\n"
    "'ColorSpace' Vulkan colorspace used for display. See VkColorSpaceKHR spec for reference.\n"
    "'ColorFormat' Vulkan pixel color format used for display. See VkFormat spec for reference.\n"
    "'ColorPrecision' Effective color precision of the Vulkan interop image and Vulkan swapChain: "
    "0 = GL_RGBA8, 1 = GL_RGB10A2, 2 = GL_RGBA16F, 3 = GL_RGBA16.\n"
    "\n";
    static char seeAlsoString[] = "OpenWindow HDRMetadata";

    int handle;
    PsychVulkanWindow* window;
    PsychGenericScriptType *s;
    PsychGenericScriptType *outMat;
    double *v;
    const char *fieldNames[] = { "Valid", "HDRMode", "LocalDimmingControl", "MinLuminance", "MaxLuminance",
                                 "MaxFrameAverageLightLevel", "MaxContentLightLevel", "ColorGamut",
                                 "ColorSpace", "ColorFormat", "ColorPrecision", "GPUIndex" };
    const int fieldCount = 12;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure Vulkan api is initialized, fail if not:
    PsychVulkanCheckInit(FALSE);

    // Get window index, if any:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);

    // Get the window:
    window = PsychGetVulkanWindow(handle, FALSE);

    // Create a structure and populate it.
    PsychAllocOutStructArray(1, kPsychArgOptional, -1, fieldCount, fieldNames, &s);

    PsychSetStructArrayDoubleElement("Valid", 0, window->nativeDisplayHDRMetadataValidity, s);
    PsychSetStructArrayDoubleElement("GPUIndex", 0, window->vulkan->deviceIndex, s);
    PsychSetStructArrayDoubleElement("HDRMode", 0, window->hdrMode, s);
    PsychSetStructArrayDoubleElement("LocalDimmingControl", 0, window->local_dimming_supported, s);
    PsychSetStructArrayDoubleElement("MinLuminance", 0, window->nativeDisplayHDRMetadata.minLuminance, s);
    PsychSetStructArrayDoubleElement("MaxLuminance", 0, window->nativeDisplayHDRMetadata.maxLuminance, s);
    PsychSetStructArrayDoubleElement("MaxFrameAverageLightLevel", 0, window->nativeDisplayHDRMetadata.maxFrameAverageLightLevel, s);
    PsychSetStructArrayDoubleElement("MaxContentLightLevel", 0, window->nativeDisplayHDRMetadata.maxContentLightLevel, s);
    PsychSetStructArrayDoubleElement("ColorSpace", 0, window->colorspace, s);
    PsychSetStructArrayDoubleElement("ColorFormat", 0, window->format, s);
    PsychSetStructArrayDoubleElement("ColorPrecision", 0, window->colorPrecision, s);

    // Create color gamut and white point matrix:
    PsychAllocateNativeDoubleMat(2, 4, 1, &v, &outMat);

    *(v++) = window->nativeDisplayHDRMetadata.displayPrimaryRed.x;
    *(v++) = window->nativeDisplayHDRMetadata.displayPrimaryRed.y;

    *(v++) = window->nativeDisplayHDRMetadata.displayPrimaryGreen.x;
    *(v++) = window->nativeDisplayHDRMetadata.displayPrimaryGreen.y;

    *(v++) = window->nativeDisplayHDRMetadata.displayPrimaryBlue.x;
    *(v++) = window->nativeDisplayHDRMetadata.displayPrimaryBlue.y;

    *(v++) = window->nativeDisplayHDRMetadata.whitePoint.x;
    *(v++) = window->nativeDisplayHDRMetadata.whitePoint.y;

    PsychSetStructArrayNativeElement("ColorGamut", 0, outMat, s);

    return(PsychError_none);
}

PsychError PSYCHVULKANHDRMetadata(void)
{
    static char useString[] = "oldHdrMetadata = PsychVulkanCore('HDRMetadata', vulkanWindow, metadataType [, maxFrameAverageLightLevel][, maxContentLightLevel][, minLuminance][, maxLuminance][, colorGamut]);";
    //                         1                                               1             2               3                            4                       5               6               7
    static char synopsisString[] =
    "Return and/or set HDR metadata for Vulkan presentation window 'vulkanWindow'.\n"
    "This function returns the currently defined HDR metadata that is sent to the "
    "HDR display monitor associated with Vulkan window 'vulkanWindow', according to the "
    "format specified by the mandatory 'metadataType' parameter. It optionally "
    "allows to define new HDR metadata to send to the monitor, starting with the next "
    "presented visual stimulus image, ie. at the completion of the next Screen('Flip') or "
    "PsychVulkanCore('Present').\n"
    "Note: Setting new HDR metadata can be an expensive - and potentially visually disruptive - "
    "operation with some graphics drivers on some operating system, e.g., on MS-Windows. Therefore "
    "avoid too frequent updates. Our driver will eliminate redundant updates as a simple performance "
    "optimization.\n"
    "Return argument 'oldHdrMetadata' is a struct with information about the current metadata. "
    "Optionally you can define new metadata to be sent to the display. If you specify any new "
    "settings, but omit any values or leave them [] empty, then those values will remain at their "
    "current / old values.\n"
    "The following fields in the struct and as new settings are defined:\n"
    "'MetadataType' Mandatory: Type of metadata to send or query. Currently only a value of 0 is supported, which "
    "defines \"Static HDR metadata type 1\", as specified by the standards SMPTE 2086 for the "
    "mastering display color properties (~ color volume) and CTA-861.3 / CTA-861-G standards "
    "for content light levels. This type of metadata allows for luminance levels between "
    "0 and 65535 nits, except for 'MinLuminance' which allows for a maximum of 6.5535 nits.\n"
    "The content light level properties 'MaxFrameAverageLightLevel' and 'MaxContentLightLevel' default to "
    "0 at startup, which signals to the display device that they are unknown, a reasonable assumption for "
    "dynamically rendered content with prior unknown maximum values over a whole session.\n"
    "'MaxFrameAverageLightLevel' Maximum frame average light level of the visual content in nits [0; 65535].\n"
    "'MaxContentLightLevel' Maximum light level of the visual content in nits [0; 65535].\n"
    "The following mastering display properties (~ color volume) default to the properties of the used HDR "
    "display monitor for presentation, if they could be queried from the connected monitor. "
    "It is advisable to override them with the real properties of the mastering display, e.g., for "
    "properly mastered movie content or image files where this information may be available.\n"
    "'MinLuminance' Minimum supported luminance of the mastering display in nits [0; 6.5535].\n"
    "'MaxLuminance' Maximum supported luminance of the mastering display in nits [0; 65535].\n"
    "'ColorGamut' A 2-by-4 matrix encoding the CIE-1931 2D chromaticity coordinates of the "
    "red, green, and blue color primaries in columns 1, 2, and 3, and the location of the white-point "
    "in column 4. This defines the color space and gamut in which the visual content was produced.\n"
    "\n";
    static char seeAlsoString[] = "OpenWindow GetHDRProperties Present";

    int handle;
    PsychVulkanWindow* window;
    VkHdrMetadataEXT oldDisplayHDRMetadata;
    PsychGenericScriptType *s;
    PsychGenericScriptType *outMat;
    double *v;
    int metadataType = 0;
    double maxFrameAverageLightLevel = DBL_MAX, maxContentLightLevel = DBL_MAX;
    double minLuminance = DBL_MAX, maxLuminance = DBL_MAX;
    double *colorGamut;
    int m, n, p;
    const char *fieldNames[] = { "MetadataType", "MinLuminance", "MaxLuminance", "MaxFrameAverageLightLevel", "MaxContentLightLevel", "ColorGamut" };
    const int fieldCount = 6;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous or too little arguments:
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(7));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure Vulkan api is initialized, fail if not:
    PsychVulkanCheckInit(FALSE);

    // Get window index, if any:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);

    // Get the window:
    window = PsychGetVulkanWindow(handle, FALSE);

    // Get and validate required metadataType:
    PsychCopyInIntegerArg(2, (PsychGetNumInputArgs() > 2) ? kPsychArgRequired : kPsychArgOptional, &metadataType);
    if (metadataType != 0)
        PsychErrorExitMsg(PsychError_user, "Invalid HDR 'metadataType' specified: Valid value is 0 only at the moment.");

    // Userscript wants current / old settings?
    if (PsychIsArgPresent(PsychArgOut, 1)) {
        // Create a structure and populate it with the current settings / old settings:
        PsychAllocOutStructArray(1, kPsychArgOptional, -1, fieldCount, fieldNames, &s);

        // Currently we only support MetadataType 0, ie. "Static HDR Metadata Type 1" as known from HDR-10 standard,
        // and the only thing current graphics and display drivers generally support afaik as of September 2020:
        PsychSetStructArrayDoubleElement("MetadataType", 0, 0, s);

        // Mastering display min and max luminance:
        PsychSetStructArrayDoubleElement("MinLuminance", 0, window->currentDisplayHDRMetadata.minLuminance, s);
        PsychSetStructArrayDoubleElement("MaxLuminance", 0, window->currentDisplayHDRMetadata.maxLuminance, s);

        // Scene content average and maximum content light level:
        PsychSetStructArrayDoubleElement("MaxFrameAverageLightLevel", 0, window->currentDisplayHDRMetadata.maxFrameAverageLightLevel, s);
        PsychSetStructArrayDoubleElement("MaxContentLightLevel", 0, window->currentDisplayHDRMetadata.maxContentLightLevel, s);

        // Create color gamut and white point matrix defining the mastering display color gamut / color space:
        PsychAllocateNativeDoubleMat(2, 4, 1, &v, &outMat);

        *(v++) = window->currentDisplayHDRMetadata.displayPrimaryRed.x;
        *(v++) = window->currentDisplayHDRMetadata.displayPrimaryRed.y;

        *(v++) = window->currentDisplayHDRMetadata.displayPrimaryGreen.x;
        *(v++) = window->currentDisplayHDRMetadata.displayPrimaryGreen.y;

        *(v++) = window->currentDisplayHDRMetadata.displayPrimaryBlue.x;
        *(v++) = window->currentDisplayHDRMetadata.displayPrimaryBlue.y;

        *(v++) = window->currentDisplayHDRMetadata.whitePoint.x;
        *(v++) = window->currentDisplayHDRMetadata.whitePoint.y;

        PsychSetStructArrayNativeElement("ColorGamut", 0, outMat, s);
    }

    // Backup current settings, so we can avoid redundant updates:
    oldDisplayHDRMetadata = window->currentDisplayHDRMetadata;

    // Assign optional new settings from user script:

    // Content light levels:
    if (PsychCopyInDoubleArg(3, kPsychArgOptional, &maxFrameAverageLightLevel)) {
        if (maxFrameAverageLightLevel < 0 || maxFrameAverageLightLevel > 65535)
            PsychErrorExitMsg(PsychError_user, "Invalid HDR content maxFrameAverageLightLevel specified: Must be between 0 and 65535 nits.");

        window->currentDisplayHDRMetadata.maxFrameAverageLightLevel = (float) maxFrameAverageLightLevel;

        window->currentDisplayHDRMetadataNeedsCommit = TRUE;
    }

    if (PsychCopyInDoubleArg(4, kPsychArgOptional, &maxContentLightLevel)) {
        if (maxContentLightLevel < 0 || maxContentLightLevel > 65535)
            PsychErrorExitMsg(PsychError_user, "Invalid HDR content maxContentLightLevel specified: Must be between 0 and 65535 nits.");

        if (maxContentLightLevel < window->currentDisplayHDRMetadata.maxFrameAverageLightLevel)
            PsychErrorExitMsg(PsychError_user, "Invalid HDR content maxContentLightLevel specified: Must be equal or greater than maxFrameAverageLightLevel in nits.");

        window->currentDisplayHDRMetadata.maxContentLightLevel = (float) maxContentLightLevel;

        window->currentDisplayHDRMetadataNeedsCommit = TRUE;
    }

    // Mastering display supported light levels:
    if (PsychCopyInDoubleArg(5, kPsychArgOptional, &minLuminance)) {
        if (minLuminance < 0 || minLuminance > 6.5535)
            PsychErrorExitMsg(PsychError_user, "Invalid HDR mastering display minLuminance specified: Must be between 0 and 6.5535 nits.");

        window->currentDisplayHDRMetadata.minLuminance = (float) minLuminance;

        window->currentDisplayHDRMetadataNeedsCommit = TRUE;
    }

    if (PsychCopyInDoubleArg(6, kPsychArgOptional, &maxLuminance)) {
        if (maxLuminance < 0 || maxLuminance > 65535)
            PsychErrorExitMsg(PsychError_user, "Invalid HDR mastering display maxLuminance specified: Must be between 0 and 65535 nits.");

        if (maxLuminance <  window->currentDisplayHDRMetadata.minLuminance)
            PsychErrorExitMsg(PsychError_user, "Invalid HDR mastering display maxLuminance specified: Must be equal or greater than minLuminance in nits.");

        window->currentDisplayHDRMetadata.maxLuminance = (float) maxLuminance;

        window->currentDisplayHDRMetadataNeedsCommit = TRUE;
    }

    // Mastering display / content color space:
    if (PsychAllocInDoubleMatArg(7, kPsychArgOptional, &m, &n, &p, &colorGamut)) {
        if (m != 2 || n!= 4 || p > 1)
            PsychErrorExitMsg(PsychError_user, "Invalid HDR colorGamut matrix specified: Must be a 2-by-4 matrix with [red, green, blue, white-point] chromaticity coordinates.");

        // Note: We intentionally do not range-check these chromaticity coordinates against [0.0 ; 1.0], as
        // there are color spaces with gamut coordinates outside the "physical" range 0 - 1, so values
        // outside the conventional range are valid albeit "unconventional":
        window->currentDisplayHDRMetadata.displayPrimaryRed.x = (float) *(colorGamut++);
        window->currentDisplayHDRMetadata.displayPrimaryRed.y = (float) *(colorGamut++);

        window->currentDisplayHDRMetadata.displayPrimaryGreen.x = (float) *(colorGamut++);
        window->currentDisplayHDRMetadata.displayPrimaryGreen.y = (float) *(colorGamut++);

        window->currentDisplayHDRMetadata.displayPrimaryBlue.x = (float) *(colorGamut++);
        window->currentDisplayHDRMetadata.displayPrimaryBlue.y = (float) *(colorGamut++);

        window->currentDisplayHDRMetadata.whitePoint.x = (float) *(colorGamut++);
        window->currentDisplayHDRMetadata.whitePoint.y = (float) *(colorGamut++);

        window->currentDisplayHDRMetadataNeedsCommit = TRUE;
    }

    // We do not call the driver to update HDR metadata here, as the semantic says this should be
    // only happening at Present() time. We only marked the need for an update at proper time in
    // the assignments above.
    if (window->currentDisplayHDRMetadataNeedsCommit) {
        // Some more consistency checks:
        if ((window->currentDisplayHDRMetadata.maxLuminance <  window->currentDisplayHDRMetadata.minLuminance) && (minLuminance != DBL_MAX))
            PsychErrorExitMsg(PsychError_user, "Invalid HDR mastering display minLuminance specified: Must be equal or smaller than current maxLuminance in nits.");

        if ((window->currentDisplayHDRMetadata.maxContentLightLevel <  window->currentDisplayHDRMetadata.maxFrameAverageLightLevel) && (maxFrameAverageLightLevel != DBL_MAX))
            PsychErrorExitMsg(PsychError_user, "Invalid HDR content maxFrameAverageLightLevel specified: Must be equal or smaller than current maxContentLightLevel in nits.");

        // Check if the to-be-set metadata has really changed, ie. it isn't a redundant
        // update to the settings that were already current. Turns out this is important
        // on MS-Windows with both AMD and even more so NVidia drivers, or performance will
        // fall off a cliff! E.g., AMD takes over 100 msecs for an update, NVidia takes on the
        // order of 1 second and a modesetting sequence! Strong work :/
        if (memcmp(&oldDisplayHDRMetadata, &window->currentDisplayHDRMetadata, sizeof(oldDisplayHDRMetadata)) == 0) {
            // No effective change. This update is redundant, so skip it:
            window->currentDisplayHDRMetadataNeedsCommit = FALSE;

            if (verbosity > 5)
                printf("PsychVulkanCore-INFO: In 'HDRMetadata', redundant HDR metadata update for window %i skipped.\n", window->index);
        }
    }

    return(PsychError_none);
}

PsychError PSYCHVULKANHDRLocalDimming(void)
{
    static char useString[] = "oldlocalDimmmingEnable = PsychVulkanCore('HDRLocalDimming', vulkanWindow [, localDimmmingEnable]);";
    //                         1                                                           1               2
    static char synopsisString[] =
    "Return and/or set HDR local backlight dimming enable setting for Vulkan presentation window 'vulkanWindow'.\n"
    "This function returns the currently set HDR local backlight dimming setting for dynamic contrast control on "
    "the HDR display monitor associated with Vulkan window 'vulkanWindow'.\n"
    "Return argument 'oldlocalDimmmingEnable' is the current setting.\n"
    "The optional 'localDimmingEnable' is the new setting to apply. This will only "
    "work if the display and display driver supports the VK_AMD_display_native_hdr "
    "Vulkan extension. As of June 2020, only \"AMD FreeSync2 HDR compliant\" "
    "HDR monitors under Windows-10 with an AMD graphics card in fullscreen mode support this.\n"
    "The PsychVulkanCore('GetHDRProperties', vulkanWindow) function allows to query if the "
    "current setup supports this function. Please note that this function will always report "
    "the selected 'localDimmingEnable' setting made by your code on a nominally supported setup. "
    "There is no way for our driver to detect if the mode change on the display was accepted, as "
    "the operating system provides no feedback about this. At least one model of \"compatible\" monitor "
    "is already known to ignore this setting, unknown if this is an AMD driver bug or monitor firmware "
    "bug. Tread carefully! Manual control of this setting on the monitor itself may be the safer choice."
    "\n";
    static char seeAlsoString[] = "GetHDRProperties";

    int handle;
    PsychVulkanWindow* window;
    int localDimmmingEnable;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous or too little arguments:
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));

    // Make sure Vulkan api is initialized, fail if not:
    PsychVulkanCheckInit(FALSE);

    // Get window index, if any:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);

    // Get the window:
    window = PsychGetVulkanWindow(handle, FALSE);

    // Return current / old setting:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, window->localDimmmingEnable);

    // Assign optional new settings from user script:
    if (PsychCopyInIntegerArg(2, kPsychArgOptional, &localDimmmingEnable)) {
        if ((localDimmmingEnable != 0) && (localDimmmingEnable != 1))
            PsychErrorExitMsg(PsychError_user, "Invalid HDR localDimmmingEnable specified: Valid values are 0 (Disable) and 1 (Enable).");

        // Supported by display and driver?
        if (!window->local_dimming_supported)
            PsychErrorExitMsg(PsychError_user, "Tried to set HDR localDimmmingEnable mode for HDR display, but driver + display does not support this!");

        // vkSetLocalDimmingAMD does not provide a return code with success/failure,
        // so just assume it succeeded, the best we can do:
        window->localDimmmingEnable = localDimmmingEnable;
        fpSetLocalDimmingAMD(window->vulkan->device, window->swapChain, (VkBool32) localDimmmingEnable);
    }

    return(PsychError_none);
}

PsychError PSYCHVULKANClose(void)
{
    static char useString[] = "PsychVulkanCore('Close');";
    //
    static char synopsisString[] =
    //    MK: Hide the existence of the 'vulkanDevice' handle, as using it is non-sensical in the currrent
    //        implementation.
    //    "Close Vulkan device 'vulkanHandle'. Do nothing if no such device is open.\n"
    //    "If the optional 'vulkanHandle' is omitted, then close all open devices and shutdown the driver, "
    "Close all open Vulkan devices and shutdown the driver, "
    "ie. perform the same cleanup as if 'clear PsychVulkanCore' would be executed.\n";
    static char seeAlsoString[] = "Open";

    int handle = -1;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp();return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments:
    PsychErrorExit(PsychCapNumOutputArgs(0));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Make sure driver is initialized:
    PsychVulkanCheckInit(FALSE);

    // Get optional device handle:
    PsychCopyInIntegerArg(1, kPsychArgOptional, &handle);

    if (handle >= 1) {
        // Close device:
        PsychVulkanClose(handle);
    }
    else {
        // No handle provided: Close all devices, shutdown driver.
        PsychVulkanShutDown();
    }

    return(PsychError_none);
}

PsychError PSYCHVULKANGetInteropHandle(void)
{
    static char useString[] = "[interopObjectHandle, allocationSize, formatSpec, tilingMode, memoryOffset, width, height, interopSemaphoreHandle] = PsychVulkanCore('GetInteropHandle', vulkanWindowHandle [, wantSemaphore=0][, eye=0]);";
    //                          1                    2               3           4           5             6      7       8                                                             1                     2                  3
    static char synopsisString[] =
        "Retrieve all info needed to import the Vulkan interop image as OpenGL renderable texture.\n"
        "Returned values allow to construct an OpenGL GL_TEXTURE2D object which uses the driver "
        "internal VkImage's backing memory as backing memory for the OpenGL texture. Iow. OpenGL "
        "and Vulkan can share an image buffer into which OpenGL renders and which is later displayed "
        "by PsychVulkanCore() via Vulkan WSI.\n"
        "'vulkanWindowHandle' is a handle to the Vulkan window to use for stimulus display.\n"
        "'wantSemaphore' Optional: If a semaphore interop handle should also be returned as 8th "
        "return argument. Defaults to 0 = No. 1 = Yes will export the handle, and each 'Present' "
        "operation on the 'vulkanWindowHandle' will block until the returned semaphore is signalled.\n"
        "'eye' Optional Eye for which interop data should be returned in a stereo display setup:\n"
        "0 = Left eye view or monoscopic view, 1 = Right eye view. Defaults to 0.\n"
        "Return arguments:\n"
        "'interopObjectHandle' Operating system specific handle to the interop image backing memory. "
        "The caller is responsible for releasing the handle once it is no longer needed.\n"
        "'allocationSize' Number of bytes of backing memory for the 'interopObjectHandle' to import.\n"
        "'formatSpec' Type of texture to create: 0 = GL_RGBA8, 1 = GL_RGB10A2, 2 = GL_RGBA16F, 3 = GL_RGBA16.\n"
        "'tilingMode' Type of tiling to use/assume for rendering: 0 = Linear (non-tiled), 1 = Tiled.\n"
        "'memoryOffset' Memory offset in bytes into the imported memory object to use.\n"
        "'width' Width of texture in pixels.\n"
        "'height' Height of texture in pixels.\n"
        "'interopSemaphoreHandle' Operating system specific handle to the interop semaphore, if one was "
        "requested by setting 'wantSemaphore' to 1. "
        "The caller is responsible for releasing the handle once it is no longer needed.\n";

    static char seeAlsoString[] = "";

    int vulkanWindowHandle, eyeIndex, wantSemaphore;
    PsychVulkanWindow* window;

    // All sub functions should have these two lines
    PsychPushHelp(useString, synopsisString,seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(8));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychVulkanCheckInit(FALSE);

    // Get vulkanWindowHandle and associated Vulkan window:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &vulkanWindowHandle);
    window = PsychGetVulkanWindow(vulkanWindowHandle, FALSE);

    wantSemaphore = 0;
    PsychCopyInIntegerArg(2, kPsychArgOptional, &wantSemaphore);
    if (wantSemaphore < 0 || wantSemaphore > 1)
        PsychErrorExitMsg(PsychError_user, "Invalid 'wantSemaphore' specified. Must be 0 = no or 1 = yes.");

    // Get eye:
    eyeIndex = 0;
    PsychCopyInIntegerArg(3, kPsychArgOptional, &eyeIndex);
    if (eyeIndex < 0 || eyeIndex > 1)
        PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0 or 1 for left- or right eye.");

    if (eyeIndex > 0 && !(window->isStereo))
        PsychErrorExitMsg(PsychError_user, "Invalid 'eye' specified. Must be 0, as mono display mode is selected.");

    // Return interopObjectHandle for interop memory object which backs the interop VkImage. Make sure the caller
    // really wants this returned, as it matters for potential ownership transfer of handles, or the lack thereof:
    if ((PsychGetNumNamedOutputArgs() >= 1) && PsychCopyOutPointerArg(1, kPsychArgOptional, (void*) (size_t) window->interopHandles.memory)) {
        // interopHandles.memory handle returned to caller. This constitutes an ownership transfer,
        // so we are no longer owning this handle and are no longer responsible for destroying it
        // at the end of a session:
        window->interopHandles.memory = 0;
    }

    // Return backing memory size in bytes:
    PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) window->interopMemorysize);

    // Return internal texture format precision specifier for OpenGL texture:
    PsychCopyOutDoubleArg(3, kPsychArgOptional, (double) window->colorPrecision);

    // Return tiling mode:
    PsychCopyOutDoubleArg(4, kPsychArgOptional, (double) window->interopTextureTiled);

    // Return memory offset into exported memory object - Hard-coded zero atm.:
    PsychCopyOutDoubleArg(5, kPsychArgOptional, (double) 0);

    // Return width:
    PsychCopyOutDoubleArg(6, kPsychArgOptional, (double) window->width);

    // Return height:
    PsychCopyOutDoubleArg(7, kPsychArgOptional, (double) window->height);

    // Export of a semaphore handle requested?
    if (wantSemaphore && (PsychGetNumNamedOutputArgs() >= 8)) {
        // Yes: Create the semaphore, retrieve and export an interop handle to it:
        VkResult result;

        // Exists already? Redundant calls are not allowed - Everybody only one cross...
        if (window->interopRenderDoneSemaphore)
            PsychErrorExitMsg(PsychError_user, "Tried to create and export 'interopSemaphoreHandle' twice. Must be done only once per window - Everybody only one cross!");

        // Create semaphore for export to / use by external renderer to signal render completion to us:
        const VkExportSemaphoreCreateInfo renderCompleteSemaphoreExportInfo = {
            .sType = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO,
            .pNext = NULL,
            .handleTypes = (PSYCH_SYSTEM == PSYCH_WINDOWS) ? VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT : VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT,
        };

        const VkSemaphoreCreateInfo renderCompleteSemaphoreCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = &renderCompleteSemaphoreExportInfo,
            .flags = 0,
        };

        result = vkCreateSemaphore(window->vulkan->device, &renderCompleteSemaphoreCreateInfo, NULL, &window->interopRenderDoneSemaphore);
        if (result != VK_SUCCESS) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: vkCreateSemaphore() for external interop render-complete semaphore failed for window %i: res=%i.\n", window->index, result);

            PsychErrorExitMsg(PsychError_system, "Creation of external interop render-complete semaphore failed.");
        }

        // Create export handle to semaphore:
        #if PSYCH_SYSTEM == PSYCH_WINDOWS
        VkSemaphoreGetWin32HandleInfoKHR interopRenderDoneSemaphoreHandleInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_GET_WIN32_HANDLE_INFO_KHR,
            .pNext = NULL,
            .semaphore = window->interopRenderDoneSemaphore,
            .handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT,
        };

        result = fpGetSemaphoreWin32HandleKHR(window->vulkan->device, &interopRenderDoneSemaphoreHandleInfo, &window->interopHandles.glComplete);
        #endif

        #if PSYCH_SYSTEM == PSYCH_LINUX
        VkSemaphoreGetFdInfoKHR interopRenderDoneSemaphoreHandleInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_GET_FD_INFO_KHR,
            .pNext = NULL,
            .semaphore = window->interopRenderDoneSemaphore,
            .handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT,
        };

        result = fpGetSemaphoreFdKHR(window->vulkan->device, &interopRenderDoneSemaphoreHandleInfo, &window->interopHandles.glComplete);
        #endif

        #if PSYCH_SYSTEM == PSYCH_OSX
        result = -1; // TODO OSX
        #endif

        if (result != VK_SUCCESS) {
            if (verbosity > 0)
                printf("PsychVulkanCore-ERROR: Getting handles for external interop render-complete semaphore failed for window %i: res=%i.\n", window->index, result);

            // Delete now obsolete interopRenderDoneSemaphore:
            vkDestroySemaphore(window->vulkan->device, window->interopRenderDoneSemaphore, NULL);
            window->interopRenderDoneSemaphore = VK_NULL_HANDLE;

            PsychErrorExitMsg(PsychError_system, "Getting handles for external interop render-complete semaphore failed.");
        }

        // Return semaphore handle to caller for external use/signalling:
        PsychCopyOutPointerArg(8, kPsychArgOptional, (void*) (size_t) window->interopHandles.glComplete);

        // Now that the ownership of the handle is transferred to the caller, remove our reference to it:
        window->interopHandles.glComplete = 0;
    }
    else {
        // Copy out dummy invalid 8th arg NULL pointer:
        PsychCopyOutPointerArg(8, kPsychArgOptional, NULL);
    }

    return(PsychError_none);
}

PsychError PSYCHVULKANPresent(void)
{
    static char useString[] = "[tPredictedOnset, frameIndex] = PsychVulkanCore('Present', vulkanWindowHandle [, tWhen=0][, doTimestamp=2]);";
    //                          1                2                                        1                     2          3
    static char synopsisString[] =
        "Present last rendered frame to Vulkan window 'vulkanWindowHandle'.\n\n"
        "This will commit the current set of 2D textures with new rendered content "
        "to the swapchains, for consumption by the presentation engine. "
        "You usually won't call this function yourself, but Screen('Flip') "
        "will call it automatically for you at the appropriate moment.\n\n"
        "'doTimestamp' If set to 1 or 2, performs timestamping of stimulus onset, or at least "
        "tries to estimate such onset time. If set to 0, do nothing timestamping-wise. "
        "A value of 1 always uses a home-made method of scheduling and timestamping. "
        "A value of 2 tries to use a Vulkan-provided high-precision method if available "
        "and possible, and falls back to method 1 otherwise. Value 2 is the default.\n\n"
        "'tWhen' If provided, defines the target presentation time, as provided by "
        "Screen('Flip', win, tWhen); a value of zero, or omission, means to present as "
        "soon as possible.\n\n"
        "'tPredictedOnset' returns predicted onset time for the just presented frame.\n\n"
        "'frameIndex' Running index of just presented (or queued for presentation) frame.\n"
        "\n\n";
    static char seeAlsoString[] = "";

    int vulkanWindowHandle;
    PsychVulkanWindow *window;
    double tWhen = 0;
    int doTimestamp = 2;

    // All sub functions should have these two lines:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(3));
    PsychErrorExit(PsychRequireNumInputArgs(1));

    // Make sure driver is initialized:
    PsychVulkanCheckInit(FALSE);

    // Get window handle:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &vulkanWindowHandle);
    window = PsychGetVulkanWindow(vulkanWindowHandle, FALSE);
    if (window == NULL)
        PsychErrorExitMsg(PsychError_user, "Invalid 'vulkanWindowHandle' provided. No such window open.");

    // Get optional presentation target time:
    PsychCopyInDoubleArg(2, kPsychArgOptional, &tWhen);

    // Get optional timestamping flag:
    PsychCopyInIntegerArg(3, kPsychArgOptional, &doTimestamp);
    if (doTimestamp < 0 || doTimestamp > 2)
        PsychErrorExitMsg(PsychError_user, "Invalid 'doTimestamp' provided. Must be 0, 1 or 2.");

    // Present:
    if (!PsychPresent(window, tWhen, doTimestamp)) {
        // Make present failure non-fatal. We are often called from an imaging pipeline
        // hook callback, so erroring out with PsychErrorExitMsg() can cause complete
        // hangs of the host application - tedious to debug. Rather keep going with a
        // warning.
        if (verbosity > 0)
            printf("PsychVulkanCore-ERROR: 'Present' operation failed for some reason! Trying to keep going, brace for impact!\n");
    }

    // Return estimated onset time:
    PsychCopyOutDoubleArg(1, kPsychArgOptional, (doTimestamp > 0) ? window->tPresentComplete : -1.0);

    // Copy out frameIndex:
    PsychCopyOutDoubleArg(2, kPsychArgOptional, (double) window->frameIndex - 1);

    return(PsychError_none);
}
