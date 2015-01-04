/*
    PsychToolbox3/Source/Linux/Screen/PsychScreenGlueWayland.c

    PLATFORMS:

        This is the Linux version with the Wayland specific backend
        code. PsychScreenGlue.c contains shared code which is shared/
        used from this file as well to avoid code duplication.

    AUTHORS:

        Mario Kleiner       mk      mario.kleiner.de@gmail.com

    HISTORY:

        4-Jan-2015          mk      Written.

    DESCRIPTION:

        Functions in this file comprise an abstraction layer for probing and controlling screen state on Wayland.
*/

#ifdef PTB_USE_WAYLAND

/* At the moment we still depend on Waffle even for "native" Wayland support.
 * Waffle is mostly used for initial Wayland connection setup, window creation,
 * and interacting with EGL to get the rendering api's setup and attached,
 * OpenGL contexts created and managed, GL extensions bound etc.
 *
 * Plan is to mix Wayland specific code with "minimal" Waffle here, and then incrementally
 * replace Waffle bits with Wayland bits as far as this is neccessary or useful.
 */
#ifndef PTB_USE_WAFFLE
#error PTB_USE_WAFFLE not defined for PTB_USE_WAYLAND build! We depend on Waffle at the moment, so this is a no-go!
#endif

#include "Screen.h"

#include <waffle.h>
#include <waffle_wayland.h>

#include <wayland-client.h>

// This header file and corresponding implementation file currently included in our
// source tree, as permitted by license. There's probably a better way to do this?
#include "presentation_timing-client-protocol.h"

/* These are needed for our GPU specific beamposition query implementation: */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

// Maximum number of slots in a gamma table to set/query: This should be plenty.
#define MAX_GAMMALUT_SIZE 16384

// These are defined in/shared with PsychScreenGlue.c:
extern PsychScreenSettingsType     displayOriginalCGSettings[kPsychMaxPossibleDisplays];            //these track the original video state before the Psychtoolbox changed it.
extern psych_bool                  displayOriginalCGSettingsValid[kPsychMaxPossibleDisplays];
extern psych_bool                  displayCursorHidden[kPsychMaxPossibleDisplays];
extern CGDisplayCount              numDisplays;

// displayCGIDs stores the wl_display handles to the display connections of each PTB logical screen.
extern CGDirectDisplayID           displayCGIDs[kPsychMaxPossibleDisplays];
extern psych_mutex displayLock;

// Functions defined in PsychScreenGlue.c:
void PsychLockScreenSettings(int screenNumber);
void PsychUnlockScreenSettings(int screenNumber);
psych_bool PsychCheckScreenSettingsLock(int screenNumber);

// Hack:
extern psych_bool has_xrandr_1_2;

// Shared waffle display connection handle for whole session:
struct waffle_display *waffle_display = NULL;

// Shared corresponding wl_display handle for session:
struct wl_display* wl_display = NULL;

struct wl_compositor* wl_compositor = NULL;

// Also share native underlying EGL display:
EGLDisplay egl_display = NULL;

// Handle to the presentation extension:
struct presentation *wayland_pres = NULL;

// And our presentation reference clock:
uint32_t wayland_presentation_clock_id;

static struct wl_registry *wl_registry = NULL;

static void
wayland_set_presentation_clock_id(void *data, struct presentation *presentation,
                                  uint32_t clk_id)
{
    struct wl_display *self = data;
    wayland_presentation_clock_id = clk_id;
    if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-DEBUG: Wayland presentation clock set to id %i.\n", (int) clk_id);
}

static const struct presentation_listener wayland_presentation_listener = {
    wayland_set_presentation_clock_id
};

static void
wayland_registry_listener_global(void *data,
                                 struct wl_registry *registry,
                                 uint32_t name,
                                 const char *interface,
                                 uint32_t version)
{
    struct wl_display *self = data;

    if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-DEBUG: Wayland registry extension candidate: %s\n", interface);

    // Only look for presentation interface v1:
    if (strcmp(interface, "presentation") || (version != 1)) return;

    // Got it:
    wayland_pres = wl_registry_bind(registry, name, &presentation_interface, 1);
    if (!wayland_pres) {
        if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: wl_registry_bind for presentation_interface failed!\n");
        return;
    }

    presentation_add_listener(wayland_pres, &wayland_presentation_listener, self);
    if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-DEBUG: Wayland presentation_interface bound!\n");
}

static void
wayland_registry_listener_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
    return;
}

static const struct wl_registry_listener wayland_registry_listener = {
    .global = wayland_registry_listener_global,
    .global_remove = wayland_registry_listener_global_remove
};

struct presentation *get_wayland_presentation_extension(PsychWindowRecordType* windowRecord)
{
    // Already have a cached presentation_interface? If so, just return it:
    return(wayland_pres);
}

// Init XInput extension: Called under display lock protection:
static void InitXInputExtensionForDisplay(CGDirectDisplayID dpy, int idx)
{
//     int major, minor;
//     int rc, i;
//
//     // XInputExtension supported? If so do basic init:
//     if (!XQueryExtension(dpy, "XInputExtension", &xi_opcode, &xi_event, &xi_error)) {
//         printf("PTB-WARNING: XINPUT1/2 extension unsupported. Will only be able to handle one mouse and mouse cursor.\n");
//         goto out;
//     }
//
//     // XInput V 2.0 or later supported?
//     major = 2;
//     minor = 0;
//     rc = XIQueryVersion(dpy, &major, &minor);
//     if (rc == BadRequest) {
//         printf("PTB-WARNING: No XInput-2 support. Server supports version %d.%d only.\n", major, minor);
//         printf("PTB-WARNING: XINPUT1/2 extension unsupported. Will only be able to handle one mouse and mouse cursor.\n");
//         goto out;
//     } else if (rc != Success) {
//         printf("PTB-ERROR: Internal error during XInput-2 extension init sequence! This is a bug in Xlib!\n");
//         printf("PTB-WARNING: XINPUT1/2 extension unsupported. Will only be able to handle one mouse and mouse cursor.\n");
//         goto out;
//     }
//
//     // printf("PsychHID: INFO: XI2 supported. Server provides version %d.%d.\n", major, minor);
//
//     // Enumerate all XI2 input devices for this x-display:
//     xinput_info[idx] = XIQueryDevice(dpy, XIAllDevices, &xinput_ndevices[idx]);
//
//     out:
    return;
}

// ProcessRandREvents: Must be called called under display lock protection!
static void ProcessRandREvents(int screenNumber)
{
//     XEvent evt;
//
//     if (!has_xrandr_1_2) return;
//
//     // Check for screen config change events and dispatch them:
//     while (XCheckTypedWindowEvent(displayCGIDs[screenNumber], RootWindow(displayCGIDs[screenNumber], displayX11Screens[screenNumber]), xr_event + RRScreenChangeNotify, &evt)) {
//         // Screen changed: Dispatch new configuration to X-Lib:
//         XRRUpdateConfiguration(&evt);
//     }
}

// GetRandRScreenConfig: Must be called called under display lock protection!
static void GetRandRScreenConfig(CGDirectDisplayID dpy, int idx)
{
//     int major, minor;
//     int o, m, num_crtcs, isPrimary, crtcid, crtccount;
//     int primaryOutput = -1, primaryCRTC = -1, primaryCRTCIdx = -1;
//     int crtcs[100];
//
//     // Preinit to "undefined":
//     displayX11ScreenResources[idx] = NULL;
//
//     // XRandR extension supported? If so do basic init:
//     if (!XRRQueryExtension(dpy, &xr_event, &xr_error) ||
//         !XRRQueryVersion(dpy, &major, &minor)) {
//         printf("PTB-WARNING: XRandR extension unsupported. Display infos and configuration functions will be very limited!\n");
//         return;
//     }
//
//     // Detect version of XRandR:
//     if (major > 1 || (major == 1 && minor >= 2)) has_xrandr_1_2 = TRUE;
//     if (major > 1 || (major == 1 && minor >= 3)) has_xrandr_1_3 = TRUE;
//
//     // Select screen configuration notify events to get delivered to us:
//     Window root = RootWindow(dpy, displayX11Screens[idx]);
//     XRRSelectInput(dpy, root, RRScreenChangeNotifyMask);
//
//     // Fetch current screen configuration info for this screen and display:
//     XRRScreenResources* res = XRRGetScreenResourcesCurrent(dpy, root);
//     displayX11ScreenResources[idx] = res;
//     if (NULL == res) {
//         printf("PTB-WARNING: Could not query configuration of x-screen %i on display %s. Display infos and configuration will be very limited.\n",
//                 displayX11Screens[idx], DisplayString(dpy));
//         return;
//     }
//
//     if (!has_xrandr_1_2) {
//         printf("PTB-WARNING: XRandR version 1.2 unsupported! Could not query useful info for x-screen %i on display %s. Infos and configuration will be very limited.\n",
//                 displayX11Screens[idx], DisplayString(dpy));
//         return;
//     }
//
//     // Total number of assigned crtc's for this screen:
//     crtccount = 0;
//
//     // Iterate over all outputs for this screen:
//     for (o = 0; o < res->noutput; o++) {
//         XRROutputInfo *output_info = XRRGetOutputInfo(dpy, res, res->outputs[o]);
//         if (!output_info) {
//             printf("PTB-WARNING: Could not get output info for %i'th output of screen %i [display %s]!\n", o, displayX11Screens[idx], DisplayString(dpy));
//             continue;
//         }
//
//         // Get info about this output:
//         if (has_xrandr_1_3 && (XRRGetOutputPrimary(dpy, root) > 0)) {
//             isPrimary = (XRRGetOutputPrimary(dpy, root) == res->outputs[o]) ? 1 : 0;
//         }
//         else {
//             isPrimary = -1;
//         }
//
//         for (crtcid = 0; crtcid < res->ncrtc; crtcid++) {
//             if (res->crtcs[crtcid] == output_info->crtc) break;
//         }
//         if (crtcid == res->ncrtc) crtcid = -1;
//
//         // Store crtc for this output:
//         crtcs[o] = crtcid;
//
//         if (PsychPrefStateGet_Verbosity() > 3) {
//             printf("PTB-INFO: Display '%s' : X-Screen %i : Output %i [%s]: %s : ", DisplayString(dpy), displayX11Screens[idx], o, (const char*) output_info->name, (isPrimary > -1) ? ((isPrimary == 1) ? "Primary output" : "Secondary output") : "Unknown output priority");
//             printf("%s : CRTC %i [XID %i]\n", (output_info->connection == RR_Connected) ? "Connected" : "Offline", crtcid, (int) output_info->crtc);
//         }
//
//         if ((isPrimary > 0) && (crtcid >= 0)) {
//             primaryOutput = o;
//             primaryCRTC = crtcid;
//         }
//
//         // Is this output - and its crtc - really enabled for this screen?
//         if (crtcid >=0) {
//             // Yes: Add its crtcid to the list of crtc's for this screen:
//             PsychSetScreenToHead(idx, crtcid, crtccount);
//             PsychSetScreenToCrtcId(idx, minimum_crtcid, crtccount);
//
//             // Increment id of next allocated crtc scanout engine on GPU:
//             // We assume they are allocated in the order of activated outputs,
//             // e.g., first output of first X-Screen -> crtc 0, 2nd output of first
//             // X-Screen -> crtc 1, first output of 2nd X-Screen -> crtc 2 etc.
//             //
//             // This is as heuristic as previous approach, but it should continue
//             // work as well or as bad as previous one, except it should fix the
//             // problem reported in forum message #14200 for AMD Catalyst driver.
//             // It should work for bog-standard ZaphodHead setups. It will work in
//             // any case on single-display setups or multi-display setups where a single
//             // X-Screen spans multiple display outputs aka multiple crtcs.
//             //
//             // The working assumption is that the user of a ZaphodHead config assigned the different
//             // GPU outputs, and thereby their associated physical crtc's, in an ascending order to
//             // X-Screens. This is a reasonable assumption, but in no way guaranteed by the system.
//             // Therefore this heuristic can go wrong on non-standard ZaphodHead Multi-X-Screen setups.
//             // In such cases the user can always use the Screen('Preference', 'ScreenToHead', ...);
//             // command or the PSYCHTOOLBOX_PIPEMAPPINGS environment variable to override the wrong
//             // mapping, although it would be a pita for complex setups.
//             minimum_crtcid++;
//
//             // Increment running count of active outputs (displays) attached to
//             // the currently processed X-Screend idx:
//             crtccount++;
//         }
//
//         // Release info for this output:
//         XRRFreeOutputInfo(output_info);
//     }
//
//     // Found a defined primary output?
//     if (primaryOutput == -1) {
//         // Could not find primary output -- none defined. Use first connected
//         // output as primary output:
//         for (o = 0; o < res->noutput; o++) {
//             XRROutputInfo *output_info = XRRGetOutputInfo(dpy, res, res->outputs[o]);
//             if (output_info && (output_info->connection == RR_Connected) && (crtcs[o] >= 0)) {
//                 primaryOutput = o;
//                 primaryCRTC = crtcs[o];
//                 XRRFreeOutputInfo(output_info);
//                 break;
//             }
//
//             if (output_info) XRRFreeOutputInfo(output_info);
//         }
//
//         // Still undefined? Use first output as primary output:
//         if (primaryOutput == -1) {
//             primaryOutput = 0;
//             primaryCRTC = (crtcs[0] >= 0) ? crtcs[0] : 0;
//         }
//     }
//
//     // Assign primary crtc of primary output - index 0 - as default display head for this screen:
//     // We swap the contents of slot 0 - the primary crtc slot - and whatever slot currently
//     // contains the crtcid of our primaryCRTC. This way we shuffle the primary crtc into the
//     // 1st slot (zero):
//     for (o = 0; o < crtccount; o++) {
//         if (PsychScreenToHead(idx, o) == primaryCRTC) {
//             PsychSetScreenToHead(idx, PsychScreenToHead(idx, 0), o);
//             primaryCRTCIdx = PsychScreenToCrtcId(idx, o);
//             PsychSetScreenToCrtcId(idx, PsychScreenToCrtcId(idx, 0), o);
//         }
//     }
//
//     PsychSetScreenToHead(idx, primaryCRTC, 0);
//     PsychSetScreenToCrtcId(idx, primaryCRTCIdx, 0);
//
//     if (PsychPrefStateGet_Verbosity() > 2) {
//         printf("PTB-INFO: Display '%s' : X-Screen %i : Assigning primary output as %i with RandR-CRTC %i and GPU-CRTC %i.\n", DisplayString(dpy), displayX11Screens[idx], primaryOutput, primaryCRTC, primaryCRTCIdx);
//     }
//
    return;
}

// Linux only: Retrieve modeline and crtc_info for a specific output on a specific screen:
// Caution: If crtc is non-NULL and receives a valid XRRCrtcInfo*, then this pointer must
//          be released by the caller via XRRFreeCrtcInfo(crtc), or resources will leak!
// Must be called under display lock protection!
XRRModeInfo* PsychOSGetModeLine(int screenId, int outputIdx, XRRCrtcInfo **crtc)
{
    int m;
    XRRModeInfo *mode = NULL;
//     XRRCrtcInfo *crtc_info = NULL;
//
//     // Query info about video modeline and crtc of output 'outputIdx':
//     XRRScreenResources *res = displayX11ScreenResources[screenId];
//     if (has_xrandr_1_2 && (PsychScreenToHead(screenId, outputIdx) >= 0)) {
//         crtc_info = XRRGetCrtcInfo(displayCGIDs[screenId], res, res->crtcs[PsychScreenToHead(screenId, outputIdx)]);
//
//         for (m = 0; (m < res->nmode) && crtc_info; m++) {
//             if (res->modes[m].id == crtc_info->mode) {
//                 mode = &res->modes[m];
//                 break;
//             }
//         }
//     }
//
//     // Optionally return crtc_info in *crtc:
//     if (crtc) {
//         // Return crtc_info, if any - NULL otherwise:
//         *crtc = crtc_info;
//     }
//     else {
//         // crtc_info not required by caller. We release it:
//         if (crtc_info) XRRFreeCrtcInfo(crtc_info);
//     }
//
    return(mode);
}

void InitCGDisplayIDList(void)
{
    int rc, i, j, k, count, scrnid;

    // Define waffle window system backend to use: Wayland only, obviously.
    static int32_t init_attrs[3] = {
        WAFFLE_PLATFORM,
        WAFFLE_PLATFORM_WAYLAND,
        0,
    };

    // NULL-out array of displays:
    for (i = 0; i < kPsychMaxPossibleDisplays; i++) displayCGIDs[i] = NULL;

    // Preinit screen to head mappings to identity default:
    PsychInitScreenToHeadMappings(0);

    // Initial count of screens is zero:
    numDisplays = 0;

    // Initial minimum allowed crtc id is zero:
    // minimum_crtcid = 0;

    // Initialize Waffle for Wayland display system backend:
    if (PsychPrefStateGet_Verbosity() > 2) {
        printf("PTB-INFO: Using FOSS Waffle display backend library, written by Chad Versace, Copyright 2012 - 2015 Intel.\n");
    }

    if (!waffle_init(init_attrs) && (waffle_error_get_code() != WAFFLE_ERROR_ALREADY_INITIALIZED)) {
        // Game over:
        if (PsychPrefStateGet_Verbosity() > 0) {
            printf("PTB-ERROR: Could not initialize Waffle Wayland/EGL backend - Error: %s.\n", waffle_error_to_string(waffle_error_get_code()));
            printf("PTB-ERROR: Try to fix the reason for the error, then restart Octave/Matlab and retry.\n");
        }
        PsychErrorExitMsg(PsychError_system, "FATAL ERROR: Couldn't initialize Waffle's Wayland backend! Game over!");
    }

    if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: Trying to connect Wayland Waffle to display '%s'.\n", getenv("PSYCH_WAFFLE_DISPLAY"));
    waffle_display = waffle_display_connect(getenv("PSYCH_WAFFLE_DISPLAY"));

    if (!waffle_display) {
        if (PsychPrefStateGet_Verbosity() > 0) {
            printf("PTB-ERROR: Could not connect Waffle to display: %s.\n", waffle_error_to_string(waffle_error_get_code()));
            if (!getenv("WAYLAND_DISPLAY")) printf("PTB-ERROR: Seems Screen() is not running on a Wayland display server? That's a no-go for this Wayland-only Screen() mex file!\n");
        }
        PsychErrorExitMsg(PsychError_system, "FATAL ERROR: Couldn't open display connection to Wayland server! Game over!");
    }

    // Extract EGL_Display for backends which use EGL:
    union waffle_native_display* wafflenatdis = waffle_display_get_native(waffle_display);
    egl_display = wafflenatdis->wayland->egl_display;
    wl_display = wafflenatdis->wayland->wl_display;
    wl_compositor = wafflenatdis->wayland->wl_compositor;

    // Release the waffle_native_display:
    free(wafflenatdis);
    wafflenatdis = NULL;

    // Get our own wl_registry, do the enumeration and binding:
    wl_registry = wl_display_get_registry(wl_display);
    if (!wl_registry) {
        if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: wl_display_get_registry failed\n");
        PsychErrorExitMsg(PsychError_system, "FATAL ERROR: Initialisation failed! Game over!");
    }

    if (wl_registry_add_listener(wl_registry, &wayland_registry_listener, wl_display) < 0) {
        if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: wl_registry_add_listener failed\n");
        PsychErrorExitMsg(PsychError_system, "FATAL ERROR: Initialisation failed! Game over!");
    }

    // Block until the Wayland server has processed all pending requests and
    // has sent out pending events on all event queues. This should ensure
    // that the registry listener has received announcement of the shell and
    // compositor.
    if (wl_display_roundtrip(wl_display) == -1) {
        if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: wl_display_roundtrip failed\n");
        PsychErrorExitMsg(PsychError_system, "FATAL ERROR: Initialisation failed! Game over!");
    }

    // Query number of available screens on this X11 display:
    count = 1; // ScreenCount(x11_dpy);

    // Setup the screenNumber --> Wayland display mappings:
    for (i = 0; i < count && i < kPsychMaxPossibleDisplays; i++) {
        displayCGIDs[i] = NULL;
        //displayX11Screens[i] = i;
        //xinput_info[i] = xinput_info[0];
        //xinput_ndevices[i] = xinput_ndevices[0];

        // Get all relevant screen config info and cache it internally:
        // GetRandRScreenConfig(x11_dpy, i);
    }

    numDisplays = i;

    if (numDisplays > 1) printf("PTB-INFO: A total of %i Wayland display screens is available for use.\n", numDisplays);

    // Initialize screenId -> GPU headId mapping to identity mappings:
    PsychInitScreenToHeadMappings(numDisplays);

    return;
}

void PsychCleanupDisplayGlue(void)
{
    CGDirectDisplayID dpy, last_dpy;
    int i;

    // Make sure the mmio mapping is shut down:
    PsychOSShutdownPsychtoolboxKernelDriverInterface();

    PsychLockDisplay();

    // Go trough full screen list:
    last_dpy = NULL;
    for (i = 0; i < PsychGetNumDisplays(); i++) {
        // Get display-ptr for this screen:
        PsychGetCGDisplayIDFromScreenNumber(&dpy, i);

        // No Wayland display connection associated with this screen? Skip it.
        if (!dpy) continue;

//         // Did we close this connection already (dpy==last_dpy)?
//         if (dpy != last_dpy) {
//             // Nope. Keep track of it...
//             last_dpy = dpy;
//
//             // ...and close display connection to X-Server:
//             XCloseDisplay(dpy);
//
//             // Release actual input info list for this display server connection:
//             if (xinput_info[i]) {
//                 // TODO: XIFreeDeviceInfo(xinput_info[i]);
//             }
//         }

        // NULL-Out xinput extension data:
        //xinput_info[i] = NULL;
        //xinput_ndevices[i] = 0;

        // Release per-screen info structures:
        // TODO: if (displayX11ScreenResources[i]) XRRFreeScreenResources(displayX11ScreenResources[i]);
        //displayX11ScreenResources[i] = NULL;
    }

    // Reset our binding to presentation_feedback extension if this is our last onscreen window to close:
    presentation_destroy(wayland_pres);
    wayland_pres = NULL;

    // Destroy our reference to the registry:
    wl_registry_destroy(wl_registry);
    wl_registry = NULL;

    // Release our shared waffle display connection:
    waffle_display_disconnect(waffle_display);
    waffle_display = NULL;

    PsychUnlockDisplay();

    // Destroy the display lock mutex, now that we're done with it for this Screen() session instance:
    PsychDestroyMutex(&displayLock);

    // All connections should be closed now. We can't NULL-out the display list, but
    // our scripting host environment will flush the Screen - Mexfile anyway...
    return;
}

// XIDeviceInfo* PsychGetInputDevicesForScreen(int screenNumber, int* nDevices)
// {
//     if(screenNumber >= numDisplays) PsychErrorExit(PsychError_invalidScumber);
//     if (nDevices) *nDevices = xinput_ndevices[screenNumber];
//     return(xinput_info[screenNumber]);
// }

void PsychGetScreenDepths(int screenNumber, PsychDepthType *depths)
{
    int* x11_depths = NULL;
    int  i, count = 0;

    if(screenNumber>=numDisplays || screenNumber < 0) PsychErrorExitMsg(PsychError_internal, "screenNumber is out of range"); //also checked within SCREENPixelSizes

    // Update XLib's view of this screens configuration:
    PsychLockDisplay();
//      ProcessRandREvents(screenNumber);
//
//     if (displayCGIDs[screenNumber]) {
//         x11_depths = XListDepths(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber), &count);
//     }

    PsychUnlockDisplay();

    if (x11_depths && depths && count > 0) {
        // Query successful: Add all values to depth struct:
//         for(i=0; i<count; i++) PsychAddValueToDepthStruct(x11_depths[i], depths);
//         XFree(x11_depths);
    }
    else {
        // Query failed: Assume at least 32 bits is available.
        printf("PTB-WARNING: Couldn't query available display depths values! Returning a made up list...\n");
        PsychAddValueToDepthStruct(32, depths);
        PsychAddValueToDepthStruct(24, depths);
        PsychAddValueToDepthStruct(16, depths);
    }
}

double PsychOSVRefreshFromMode(XRRModeInfo *mode)
{
//    double dot_clock = (double) mode->dotClock / 1000.0;
    double vrefresh = 0;

//     // Sanity check:
//     if ((dot_clock <= 0) || (mode->hTotal < 1) || (mode->vTotal < 1)) return(0);
//
//     vrefresh = (((dot_clock * 1000.0) / mode->hTotal) * 1000.0) / mode->vTotal;
//
//     // Divide vrefresh by 1000 to get real Hz - value:
//     vrefresh = vrefresh / 1000.0;
//
//     // Doublescan mode? If so, divide vrefresh by 2:
//     if (mode->modeFlags & RR_DoubleScan) vrefresh /= 2.0;
//
//     // Interlaced mode? If so, multiply vrefresh by 2:
//     if (mode->modeFlags & RR_Interlace) vrefresh *= 2.0;
//
    return(vrefresh);
}

/*   PsychGetAllSupportedScreenSettings()
 *
 *     Queries the display system for a list of all supported display modes, ie. all valid combinations
 *     of resolution, pixeldepth and refresh rate. Allocates temporary arrays for storage of this list
 *     and returns it to the calling routine. This function is basically only used by Screen('Resolutions').
 */
int PsychGetAllSupportedScreenSettings(int screenNumber, int outputId, long** widths, long** heights, long** hz, long** bpp)
{
//     int i, j, o, nsizes, nrates, numPossibleModes;
//     XRRModeInfo *mode = NULL;
//     XRROutputInfo *output_info = NULL;

    if(screenNumber >= numDisplays || screenNumber < 0) PsychErrorExit(PsychError_invalidScumber);

    // Only supported with RandR 1.2 or later:
    if (!has_xrandr_1_2) return(0);

//     if (outputId < 0) {
//         PsychLockDisplay();
//
//         // Iterate over all screen sizes and count number of size x refresh rate combos:
//         numPossibleModes = 0;
//         XRRScreenSize *scs = XRRSizes(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber), &nsizes);
//         for (i = 0; i < nsizes; i++) {
//             XRRRates(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber), i, &nrates);
//             numPossibleModes += nrates;
//         }
//
//         PsychUnlockDisplay();
//
//         // Allocate output arrays: These will get auto-released at exit from Screen():
//         *widths  = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
//         *heights = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
//         *hz      = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
//         *bpp     = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
//
//         // Reiterate and fill all slots:
//         numPossibleModes = 0;
//         for (i = 0; i < nsizes; i++) {
//             PsychLockDisplay();
//             short* rates = XRRRates(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber), i, &nrates);
//             PsychUnlockDisplay();
//             for (j = 0; j < nrates; j++) {
//                 (*widths)[numPossibleModes]  = (long) scs[i].width;
//                 (*heights)[numPossibleModes] = (long) scs[i].height;
//                 (*hz)[numPossibleModes]      = (long) rates[j];
//                 (*bpp)[numPossibleModes]     = (long) PsychGetScreenDepthValue(screenNumber);
//                 numPossibleModes++;
//             }
//         }
//
//         // Done:
//         return(numPossibleModes);
//     }
//
//     // Find crtc for given outputid and screen:
//     XRRScreenResources *res = displayX11ScreenResources[screenNumber];
//     if (outputId >= kPsychMaxPossibleCrtcs) PsychErrorExitMsg(PsychError_user, "Invalid output index provided! No such output for this screen!");
//
//     outputId = PsychScreenToHead(screenNumber, outputId);
//     if (outputId >= res->ncrtc || outputId < 0) PsychErrorExitMsg(PsychError_user, "Invalid output index provided! No such output for this screen!");
//
//     RRCrtc crtc = res->crtcs[outputId];
//
//     // Find output associated with the crtc for this outputId:
//     PsychLockDisplay();
//     for (o = 0; o < res->noutput; o++) {
//         output_info = XRRGetOutputInfo(displayCGIDs[screenNumber], res, res->outputs[o]);
//         if (output_info->crtc == crtc) break;
//         XRRFreeOutputInfo(output_info);
//     }
//     PsychUnlockDisplay();
//
//     // Got it?
//     if (o == res->noutput) PsychErrorExitMsg(PsychError_user, "Invalid output index provided! No such output for this screen!");
//
//     // Got it: output_info contains a list of all modes (XID's) supported by this
//     // display output / crtc combo: Iterate over all of them and return them.
//     numPossibleModes = output_info->nmode;
//
//     // Allocate output arrays: These will get auto-released at exit from Screen():
//     *widths  = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
//     *heights = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
//     *hz      = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
//     *bpp     = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
//
//     for (i = 0; i < numPossibleModes; i++) {
//         // Fetch modeline for i'th mode:
//         for (j = 0; j < res->nmode; j++) {
//             if (res->modes[j].id == output_info->modes[i]) break;
//         }
//
//         (*widths)[i] = (long) res->modes[j].width;
//         (*heights)[i] = (long) res->modes[j].height;
//         (*hz)[i] = (long) (PsychOSVRefreshFromMode(&res->modes[j]) + 0.5);
//         (*bpp)[i] = (long) 32;
//     }
//
//     // Free output info:
//     XRRFreeOutputInfo(output_info);
//
//     // Done:
//     return(numPossibleModes);
}

/*
 * PsychGetCGModeFromVideoSettings()
 */
psych_bool PsychGetCGModeFromVideoSetting(CFDictionaryRef *cgMode, PsychScreenSettingsType *setting)
{
    int i, j, nsizes = 0, nrates = 0;

    // No op on system without RandR:
    if (!has_xrandr_1_2) {
        // Dummy assignment:
        *cgMode = -1;

        // Also cannot restore display settings at Window / Screen / Runtime close time, so disable it:
        displayOriginalCGSettingsValid[setting->screenNumber] = FALSE;

        // Some info for the reader:
        if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Getting or setting display video modes unsupported on this graphics driver due to lack of RandR v1.2 support.\n");

        // Return success in good faith that its ok.
        return(TRUE);
    }

    // Extract parameters from setting struct:
    CGDirectDisplayID dpy = displayCGIDs[setting->screenNumber];
    int width  = (int) PsychGetWidthFromRect(setting->rect);
    int height = (int) PsychGetHeightFromRect(setting->rect);
    int fps    = (int) (setting->nominalFrameRate + 0.5);

    if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Trying to validate/find closest video mode for requested spec: width = %i x height = %i, rate %i Hz.\n", width, height, fps);

    // Find matching mode:
    int size_index = -1;

    PsychLockDisplay();

//     XRRScreenSize *scs = XRRSizes(dpy, PsychGetXScreenIdForScreen(setting->screenNumber), &nsizes);
//     for (i = 0; i < nsizes; i++) {
//         if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Testing against mode of resolution w x h = %i x %i with refresh rates: ", scs[i].width, scs[i].height);
//         if ((width == scs[i].width) && (height == scs[i].height)) {
//             short *rates = XRRRates(dpy, PsychGetXScreenIdForScreen(setting->screenNumber), i, &nrates);
//             for (j = 0; j < nrates; j++) {
//                 if (PsychPrefStateGet_Verbosity() > 3) printf("%i ", (int) rates[j]);
//                 if (rates[j] == (short) fps) {
//                     // Our requested size x fps combo is supported:
//                     size_index = i;
//                     if (PsychPrefStateGet_Verbosity() > 3) printf("--> Got it! Mode id %i. ", size_index);
//                 }
//             }
//         }
//         if (PsychPrefStateGet_Verbosity() > 3) printf("\n");
//     }

    PsychUnlockDisplay();

    if ((nsizes == 0 || nrates == 0) && (PsychPrefStateGet_Verbosity() > 1)) printf("PTB-WARNING: Getting or setting display video modes unsupported on this graphics driver despite advertised RandR v1.2 support.\n");

    // Found valid settings?
    if (size_index == -1) return(FALSE);

    *cgMode = size_index;
    return(TRUE);
}

/*
    PsychGetScreenDepth()

    The caller must allocate and initialize the depth struct. 
*/
void PsychGetScreenDepth(int screenNumber, PsychDepthType *depth)
{
    if (screenNumber>=numDisplays || screenNumber < 0) PsychErrorExitMsg(PsychError_internal, "screenNumber is out of range"); //also checked within SCREENPixelSizes

    // Update XLib's view of this screens configuration:
    PsychLockDisplay();
//     ProcessRandREvents(screenNumber);
//
//     if (displayCGIDs[screenNumber]) {
//         PsychAddValueToDepthStruct(DefaultDepth(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber)), depth);
//     }
//     else {
        PsychAddValueToDepthStruct(32, depth);
//     }

    PsychUnlockDisplay();
}

float PsychGetNominalFramerate(int screenNumber)
{
    if (PsychPrefStateGet_ConserveVRAM() & kPsychIgnoreNominalFramerate) return(0);

    if(screenNumber >= numDisplays || screenNumber < 0)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetNominalFramerate() is out of range");

    // No-Op on non-X11:
    if (!displayCGIDs[screenNumber]) return(0);

    // TODO No-Op for now:
    return(0);

#ifdef USE_VIDMODEEXTS

    // Information returned by the XF86VidModeExtension:
    XF86VidModeModeLine mode_line;  // The mode line of the current video mode.
    int dot_clock;                  // The RAMDAC / TDMS pixel clock frequency.

    // We start with a default vrefresh of zero, which means "couldn't query refresh from OS":
    float vrefresh = 0;

    // First we try to get modeline of primary crtc from RandR:
    PsychLockDisplay();
//     XRRModeInfo *mode = PsychOSGetModeLine(screenNumber, 0, NULL);
    PsychUnlockDisplay();

    // Modeline with plausible values returned by RandR?
    if (mode && (mode->hTotal > mode->width) && (mode->vTotal > mode->height)) {
        if (PsychPrefStateGet_Verbosity() > 4) {
            printf ("RandR: %s (0x%x) %6.1fMHz\n",
                    mode->name, (int)mode->id,
                    (double)mode->dotClock / 1000000.0);
            printf ("        h: width  %4d start %4d end %4d total %4d skew %4d\n",
                    mode->width, mode->hSyncStart, mode->hSyncEnd,
                    mode->hTotal, mode->hSkew);
            printf ("        v: height %4d start %4d end %4d total %4d\n",
                    mode->height, mode->vSyncStart, mode->vSyncEnd, mode->vTotal);
        }

        dot_clock = (int) ((double) mode->dotClock / 1000.0);
        mode_line.htotal = mode->hTotal;
        mode_line.vtotal = mode->vTotal;
        mode_line.flags = 0;
        mode_line.flags |= (mode->modeFlags & RR_DoubleScan) ? 0x0020 : 0x0;
        mode_line.flags |= (mode->modeFlags & RR_Interlace) ? 0x0010 : 0x0;
    }

    // More child-protection: (utterly needed!)
    if ((dot_clock <= 0) || (mode_line.htotal < 1) || (mode_line.vtotal < 1)) {
        if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: PsychGetNominalFramerate: Invalid modeline retrieved from RandR/VidModeExt. Giving up!\n");
        return(0);
    }

    // Query vertical refresh rate. If it fails we default to the last known good value...
    // Vertical refresh rate is: RAMDAC pixel clock / width of a scanline in clockcylces /
    // number of scanlines per videoframe.
    vrefresh = (((dot_clock * 1000) / mode_line.htotal) * 1000) / mode_line.vtotal;

    // Divide vrefresh by 1000 to get real Hz - value:
    vrefresh = vrefresh / 1000.0f;

    // Definitions from xserver's hw/xfree86/common/xf86str.h
    // V_INTERLACE    = 0x0010,
    // V_DBLSCAN    = 0x0020,

    // Doublescan mode? If so, divide vrefresh by 2:
    if (mode_line.flags & 0x0020) vrefresh /= 2;

    // Interlaced mode? If so, multiply vrefresh by 2:
    if (mode_line.flags & 0x0010) vrefresh *= 2;

    // Done.
    return(vrefresh);
#else
    return(0);
#endif
}

float PsychSetNominalFramerate(int screenNumber, float requestedHz)
{
    // Information returned by/sent to the XF86VidModeExtension:
//     XF86VidModeModeLine mode_line;  // The mode line of the current video mode.
    int dot_clock;                  // The RAMDAC / TDMS pixel clock frequency.
    int rc;
    int event_base;

    // We start with a default vrefresh of zero, which means "couldn't query refresh from OS":
    float vrefresh = 0;

    if (screenNumber >= numDisplays || screenNumber < 0)
        PsychErrorExitMsg(PsychError_internal, "screenNumber is out of range");

    // Not available on non-X11:
    if (!displayCGIDs[screenNumber]) return(0);

//     PsychLockDisplay();
//
//     if (!XF86VidModeSetClientVersion(displayCGIDs[screenNumber])) {
//         // Failed to use VidMode-Extension. We just return a vrefresh of zero.
//         PsychUnlockDisplay();
//         return(0);
//     }
//
//     if (!XF86VidModeQueryExtension(displayCGIDs[screenNumber], &event_base, &x11_errorbase)) {
//         // Failed to use VidMode-Extension. We just return a vrefresh of zero.
//         PsychUnlockDisplay();
//         return(0);
//     }
//
//     // Attach our error callback handler and reset error-state:
//     x11_errorval = 0;
//     x11_olderrorhandler = XSetErrorHandler(x11VidModeErrorHandler);
//
//     // Step 1: Query current dotclock and modeline:
//     if (!XF86VidModeGetModeLine(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber), &dot_clock, &mode_line)) {
//         // Restore default error handler:
//         XSetErrorHandler(x11_olderrorhandler);
//         PsychUnlockDisplay();
//
//         PsychErrorExitMsg(PsychError_internal, "Failed to query video dotclock and modeline!");
//     }
//
//     // Step 2: Calculate updated modeline:
//     if (requestedHz > 10) {
//         // Step 2-a: Given current dot-clock and modeline and requested vrefresh, compute
//         // modeline for closest possible match:
//         requestedHz*=1000.0f;
//         vrefresh = (((dot_clock * 1000) / mode_line.htotal) * 1000) / requestedHz;
//
//         // Assign it to closest modeline setting:
//         mode_line.vtotal = (int)(vrefresh + 0.5f);
//     }
//     else {
//         // Step 2-b: Delta mode. requestedHz represents a direct integral offset
//         // to add or subtract from current modeline setting:
//         mode_line.vtotal+=(int) requestedHz;
//     }
//
//     // Step 3: Try to set new modeline:
//     if (!XF86VidModeModModeLine(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber), &mode_line)) {
//         // Restore default error handler:
//         XSetErrorHandler(x11_olderrorhandler);
//         PsychUnlockDisplay();
//
//         // Invalid modeline? Signal this:
//         return(-1);
//     }
//
//     // We synchronize and wait for X-Request completion. If the modeline was invalid,
//     // this will trigger an invocation of our errorhandler, which in turn will
//     // set the x11_errorval to a non-zero value:
//     XSync(displayCGIDs[screenNumber], FALSE);
//
//     // Restore default error handler:
//     XSetErrorHandler(x11_olderrorhandler);
//
//     PsychUnlockDisplay();
//
//     // Check for error:
//     if (x11_errorval) {
//         // Failed to set new mode! Must be invalid. We return -1 to signal this:
//         return(-1);
//     }
//
//     // No error...

    // Step 4: Query new settings and return them:
    vrefresh = PsychGetNominalFramerate(screenNumber);

    // Done.
    return(vrefresh);
}

/* Returns the physical display size as reported by X11: */
void PsychGetDisplaySize(int screenNumber, int *width, int *height)
{
    if (screenNumber >= numDisplays || screenNumber < 0)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetDisplaySize() is out of range");

    // Not available on non-X11:
    if (TRUE || !displayCGIDs[screenNumber]) { *width = 0; *height = 0; return; }

//     // Update XLib's view of this screens configuration:
//     PsychLockDisplay();
//     ProcessRandREvents(screenNumber);
//
//     *width = (int) XDisplayWidthMM(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber));
//     *height = (int) XDisplayHeightMM(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber));
//
//     PsychUnlockDisplay();
}

void PsychGetScreenPixelSize(int screenNumber, long *width, long *height)
{
    // For now points == pixels, so just a dumb wrapper, as long as we
    // don't have special "Retina Display" / HiDPI handling in place on X11:
    PsychGetScreenSize(screenNumber, width, height);
}

void PsychGetScreenSize(int screenNumber, long *width, long *height)
{
    if(screenNumber >= numDisplays || screenNumber < 0) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range");

    // Not available on non-X11: MK TODO FIXME - How to get real values?
    if (TRUE || !displayCGIDs[screenNumber]) { *width = 1680; *height = 1050; return; }

//     // Update XLib's view of this screens configuration:
//     PsychLockDisplay();
//     ProcessRandREvents(screenNumber);
//
//     *width=XDisplayWidth(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber));
//     *height=XDisplayHeight(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber));
//
//     PsychUnlockDisplay();
}

void PsychGetGlobalScreenRect(int screenNumber, double *rect)
{
    // Create an empty rect:
    PsychMakeRect(rect, 0, 0, 1, 1);
    // Fill it with meaning by PsychGetScreenRect():
    PsychGetScreenRect(screenNumber, rect);
}

void PsychGetScreenRect(int screenNumber, double *rect)
{
    long width, height; 

    PsychGetScreenSize(screenNumber, &width, &height);
    rect[kPsychLeft]=0;
    rect[kPsychTop]=0;
    rect[kPsychRight]=(int)width;
    rect[kPsychBottom]=(int)height; 
} 

/*
    This is a place holder for a function which uncovers the number of dacbits.  To be filled in at a later date.
    If you know that your card supports >8 then you can fill that in the PsychtPreferences and the psychtoolbox
    will act accordingly.

    There seems to be no way to uncover the dacbits programatically.  According to apple CoreGraphics
    sends a 16-bit word and the driver throws out whatever it chooses not to use.

    For now we just use 8 to avoid false precision.

    If we can uncover the video card model then  we can implement a table lookup of video card model to number of dacbits.
*/
int PsychGetDacBitsFromDisplay(int screenNumber)
{
    return(8);
}

//Set display parameters

/* Linux only: PsychOSSetOutputConfig()
 * Set a video mode and other settings for a specific crtc of a specific output 'outputId'
 * for a specific screen 'screenNumber'.
 *
 * Returns true on success, false on failure.
 */
int PsychOSSetOutputConfig(int screenNumber, int outputId, int newWidth, int newHeight, int newHz, int newX, int newY)
{
    int modeid, maxw, maxh, output, widthMM, heightMM;
    XRRCrtcInfo *crtc_info = NULL, *crtc_info2;
    CGDirectDisplayID dpy = displayCGIDs[screenNumber];
//     XRRScreenResources *res = displayX11ScreenResources[screenNumber];

    // Need this later:
    PsychGetDisplaySize(screenNumber, &widthMM, &heightMM);

    if (has_xrandr_1_2 && (PsychScreenToHead(screenNumber, outputId) >= 0)) {
        PsychLockDisplay();
//         crtc_info = XRRGetCrtcInfo(dpy, res, res->crtcs[PsychScreenToHead(screenNumber, outputId)]);
        PsychUnlockDisplay();
    }
    else {
        // Failed!
        return(FALSE);
    }

//     // Disable auto-restore of screen settings - It would end badly...
//     displayOriginalCGSettingsValid[screenNumber] = FALSE;
//
//     // Find matching mode:
//     for (modeid = 0; modeid < res->nmode; modeid++) {
//         if (((int) res->modes[modeid].width == newWidth) && ((int) res->modes[modeid].height == newHeight) &&
//             (newHz == (int)(PsychOSVRefreshFromMode(&res->modes[modeid]) + 0.5))) {
//             break;
//         }
//     }
//
//     // Matching mode found for modesetting?
//     if (modeid < res->nmode) {
//         PsychLockDisplay();
//
//         // Assign default panning:
//         if (newX < 0) newX = crtc_info->x;
//         if (newY < 0) newY = crtc_info->y;
//
//         // Iterate over all outputs and compute the new screen bounding box:
//         maxw = maxh = 0;
//         for (output = 0; (PsychScreenToHead(screenNumber, output) >= 0) && (output < kPsychMaxPossibleCrtcs); output++) {
//             if (output == outputId) continue;
//
//             crtc_info2 = XRRGetCrtcInfo(dpy, res, res->crtcs[PsychScreenToHead(screenNumber, output)]);
//             if (crtc_info2->x + (int) crtc_info2->width > maxw) maxw = crtc_info2->x + (int) crtc_info2->width;
//             if (crtc_info2->y + (int) crtc_info2->height > maxh) maxh = crtc_info2->y + (int) crtc_info2->height;
//             XRRFreeCrtcInfo(crtc_info2);
//         }
//
//         // Incorporate our soon reconfigured crtc:
//         if (newX + newWidth  > maxw) maxw = newX + newWidth;
//         if (newY + newHeight > maxh) maxh = newY + newHeight;
//
//         // [0, 0, maxw, maxh] is the new bounding rectangle of the scanned out framebuffer. Set screen size accordingly:
//
//         // Prevent clients from getting confused by our config sequence:
//         // XGrabServer(dpy);
//
//         // Disable target crtc:
//         if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Disabling crtc %i.\n", outputId);
//         Status rc = XRRSetCrtcConfig(dpy, res, res->crtcs[PsychScreenToHead(screenNumber, outputId)], crtc_info->timestamp,
//                                     0, 0, None, RR_Rotate_0, NULL, 0);
//
//         // Resize screen: MK Don't! Skip this for now, use PsychSetScreenSettings() aka Screen('Resolution') to resize
//         // the screen without changing the crtc / output settings. More flexible...
//         // if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Resizing screen %i to %i x %i pixels.\n", screenNumber, maxw, maxh);
//         // XRRSetScreenSize(dpy, RootWindow(dpy, PsychGetXScreenIdForScreen(screenNumber)), maxw, maxh, widthMM, heightMM);
//
//         // Switch mode of target crtc and reenable it:
//         if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Enabling crtc %i.\n", outputId);
//
//         crtc_info2 = XRRGetCrtcInfo(dpy, res, res->crtcs[PsychScreenToHead(screenNumber, outputId)]);
//         rc = XRRSetCrtcConfig(dpy, res, res->crtcs[PsychScreenToHead(screenNumber, outputId)], crtc_info2->timestamp,
//                                 newX, newY, res->modes[modeid].id, crtc_info->rotation,
//                                 crtc_info->outputs, crtc_info->noutput);
//         XRRFreeCrtcInfo(crtc_info);
//         XRRFreeCrtcInfo(crtc_info2);
//
//         // XUngrabServer(dpy);
//
//         // Make sure the screen change gets noticed by XLib:
//         ProcessRandREvents(screenNumber);
//
//         PsychUnlockDisplay();
//
//         return(TRUE);
//     } else {
//         XRRFreeCrtcInfo(crtc_info);
//         return(FALSE);
//     }
}

/*
    PsychSetScreenSettings()

    Accept a PsychScreenSettingsType structure holding a video mode and set the display mode accordingly.

    If we can not change the display settings because of a lock (set by open window or close window) then return false.

    SCREENOpenWindow should capture the display before it sets the video mode.  If it doesn't, then PsychSetVideoSettings will
    detect that and exit with an error.  SCREENClose should uncapture the display. 

    The duties of SCREENOpenWindow are:
    -Lock the screen which serves the purpose of preventing changes in video setting with open Windows.
    -Capture the display which gives the application synchronous control of display parameters.

    TO DO: for 8-bit palletized mode there is probably more work to do.

*/

psych_bool PsychSetScreenSettings(psych_bool cacheSettings, PsychScreenSettingsType *settings)
{
    CFDictionaryRef cgMode;
    psych_bool      isValid, isCaptured;
    Rotation        rotation;
    short           rate;
    Time            cfg_timestamp;
    CGDirectDisplayID dpy;

    if (settings->screenNumber >= numDisplays || settings->screenNumber < 0) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychSetScreenSettings() is out of range");
    dpy = displayCGIDs[settings->screenNumber];

    // Not available on non-X11:
    if (!dpy) return(false);

    return(false);

//     //Check for a lock which means onscreen or offscreen windows tied to this screen are currently open.
//     // MK Disabled: if(PsychCheckScreenSettingsLock(settings->screenNumber)) return(false);  //calling function should issue an error for attempt to change display settings while windows were open.
//
//     //Check to make sure that this display is captured, which OpenWindow should have done.  If it has not been done, then exit with an error.
//     isCaptured=PsychIsScreenCaptured(settings->screenNumber);
//     if(!isCaptured) PsychErrorExitMsg(PsychError_internal, "Attempt to change video settings without capturing the display");
//
//     // Store the original display mode if this is the first time we have called this function.  The psychtoolbox will disregard changes in
//     // the screen state made through the control panel after the Psychtoolbox was launched. That is, OpenWindow will by default continue to
//     // open windows with finder settings which were in place at the first call of OpenWindow.  That's not intuitive, but not much of a problem
//     // either.
//     if(!displayOriginalCGSettingsValid[settings->screenNumber]) {
//         PsychGetScreenSettings(settings->screenNumber, &displayOriginalCGSettings[settings->screenNumber]);
//         displayOriginalCGSettingsValid[settings->screenNumber] = TRUE;
//     }
//
//     // Multi-Display configuration?
//     if (PsychScreenToHead(settings->screenNumber, 1) != -1) {
//         // Yes: At least two display heads attached. We can't use the XRRSetScreenConfigAndRate() method,
//         // it is only suitable for single-display setups. In this case, we only set the screen size, aka
//         // framebuffer size. User scripts can use the 'ConfigureDisplay' function to setup the crtc's:
//
//         // Also cannot restore display settings at Window / Screen / Runtime close time, so disable it:
//         displayOriginalCGSettingsValid[settings->screenNumber] = FALSE;
//
//         // Resize screen:
//         int widthMM, heightMM;
//         PsychGetDisplaySize(settings->screenNumber, &widthMM, &heightMM);
//         int width  = (int) PsychGetWidthFromRect(settings->rect);
//         int height = (int) PsychGetHeightFromRect(settings->rect);
//
//         if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Resizing screen %i to %i x %i pixels.\n", settings->screenNumber, width, height);
//
//         PsychLockDisplay();
//
//         XRRSetScreenSize(dpy, RootWindow(dpy, PsychGetXScreenIdForScreen(settings->screenNumber)), width, height, widthMM, heightMM);
//
//         // Make sure the screen change gets noticed by XLib:
//         ProcessRandREvents(settings->screenNumber);
//
//         PsychUnlockDisplay();
//
//         // Done.
//         return(true);
//     }
//
//     // Single display configuration, go ahead:
//
//     //Find core graphics video settings which correspond to settings as specified withing by an abstracted psychsettings structure.
//     isValid = PsychGetCGModeFromVideoSetting(&cgMode, settings);
//     if (!isValid || (int) cgMode < 0) {
//         // This is an internal error because the caller is expected to check first.
//         PsychErrorExitMsg(PsychError_user, "Attempt to set invalid video settings or function unsupported with this graphics-driver.");
//     }
//
//     // Change the display mode.
//     PsychLockDisplay();
//     XRRScreenConfiguration *sc = XRRGetScreenInfo(dpy, RootWindow(dpy, PsychGetXScreenIdForScreen(settings->screenNumber)));
//
//     // Extract parameters from settings struct:
//     rate = (short) (settings->nominalFrameRate + 0.5);
//
//     // Fetch current rotation, so we can (re)apply it -- We don't support changing rotation yet:
//     XRRConfigCurrentConfiguration(sc, &rotation);
//
//     // Fetch config timestamp so we can prove to the server we're trustworthy:
//     Time timestamp = XRRConfigTimes(sc, &cfg_timestamp);
//
//     // Apply new configuration - combo of old rotation with new size (encoded in cgMode) and refresh rate:
//     Status rc = XRRSetScreenConfigAndRate(dpy, sc, RootWindow(dpy, PsychGetXScreenIdForScreen(settings->screenNumber)), cgMode, rotation, rate, timestamp);
//
//     // Cleanup:
//     XRRFreeScreenConfigInfo(sc);
//
//     // Make sure the screen change gets noticed by XLib:
//     ProcessRandREvents(settings->screenNumber);
//
//     PsychUnlockDisplay();
//
//     // Done:
//     return((rc != BadValue) ? true : false);
}

/*
    PsychRestoreVideoSettings()

    Restores video settings to the state set by the finder.  Returns TRUE if the settings can be restored or false if they 
    can not be restored because a lock is in effect, which would mean that there are still open windows.    

*/
psych_bool PsychRestoreScreenSettings(int screenNumber)
{
    CFDictionaryRef             cgMode;
    psych_bool                  isValid, isCaptured;
    Rotation                    rotation;
    short                       rate;
    Time                        cfg_timestamp;
    CGDirectDisplayID           dpy;
    PsychScreenSettingsType     *settings;

    if (screenNumber >= numDisplays || screenNumber < 0)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); //also checked within SCREENPixelSizes

    //Check to make sure that the original graphics settings were cached.  If not, it means that the settings were never changed, so we can just
    //return true. 
    if (!displayOriginalCGSettingsValid[screenNumber]) return(true);

    //Check to make sure that this display is captured, which OpenWindow should have done.  If it has not been done, then exit with an error.  
    isCaptured = PsychIsScreenCaptured(screenNumber);
    if (!isCaptured) PsychErrorExitMsg(PsychError_internal, "Attempt to change video settings without capturing the display");

    // Retrieve original screen settings which we should restore for this screen:
    settings = &displayOriginalCGSettings[screenNumber];

    // Invalidate settings - we want a fresh game after restoring the resolution:
    displayOriginalCGSettingsValid[screenNumber] = FALSE;

    //Find core graphics video settings which correspond to settings as specified withing by an abstracted psychsettings structure.  
    isValid = PsychGetCGModeFromVideoSetting(&cgMode, settings);
    if (!isValid || (int) cgMode < 0){
        // This is an internal error because the caller is expected to check first. 
        PsychErrorExitMsg(PsychError_user, "Attempt to restore invalid video settings or function unsupported with this graphics-driver.");
    }

    //Change the display mode.
    dpy = displayCGIDs[settings->screenNumber];

//     PsychLockDisplay();
//
//     XRRScreenConfiguration *sc = XRRGetScreenInfo(dpy, RootWindow(dpy, PsychGetXScreenIdForScreen(settings->screenNumber)));
//
//     // Extract parameters from settings struct:
//     rate = (short) (settings->nominalFrameRate + 0.5);
//
//     // Fetch current rotation, so we can (re)apply it -- We don't support changing rotation yet:
//     XRRConfigCurrentConfiguration (sc, &rotation);
//
//     // Fetch config timestamp so we can prove to the server we're trustworthy:
//     Time timestamp = XRRConfigTimes(sc, &cfg_timestamp);
//
//     // Apply new configuration - combo of old rotation with new size (encoded in cgMode) and refresh rate:
//     Status rc = XRRSetScreenConfigAndRate(dpy, sc, RootWindow(dpy, PsychGetXScreenIdForScreen(settings->screenNumber)), cgMode, rotation, rate, timestamp);
//
//     // Cleanup:
//     XRRFreeScreenConfigInfo(sc);
//
//     // Make sure the screen change gets noticed by XLib:
//     ProcessRandREvents(settings->screenNumber);

//     PsychUnlockDisplay();
//
//     // Done:
//     return ((rc != BadValue) ? true : false);
}

void PsychOSDefineX11Cursor(int screenNumber, int deviceId, Cursor cursor)
{
    PsychWindowRecordType **windowRecordArray;
    int i, numWindows;

    // Not available on non-X11:
    if (!displayCGIDs[screenNumber]) return;

    // Iterate over all open onscreen windows associated with this screenNumber and
    // apply new X11 cursor definition to each of them:
    PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);

    PsychLockDisplay();
    for(i = 0; i < numWindows; i++) {
        if (PsychIsOnscreenWindow(windowRecordArray[i]) && (windowRecordArray[i]->screenNumber == screenNumber) &&
            windowRecordArray[i]->targetSpecific.xwindowHandle) {
            // Candidate.
            if (deviceId >= 0) {
                // XInput extension for per-device settings:
//                 XIDefineCursor(displayCGIDs[screenNumber], deviceId, windowRecordArray[i]->targetSpecific.xwindowHandle, cursor);
            }
            else {
                // Old-School global settings:
//                 XDefineCursor(displayCGIDs[screenNumber], windowRecordArray[i]->targetSpecific.xwindowHandle, cursor);
            }
        }
    }
    PsychUnlockDisplay();
    PsychDestroyVolatileWindowRecordPointerList(windowRecordArray);

    return;
}

void PsychHideCursor(int screenNumber, int deviceIdx)
{
    // Static "Cursor" object which defines a completely transparent - and therefore invisible
    // X11 cursor for the mouse-pointer.
    static Cursor nullCursor = -1;

    // Check for valid screenNumber:
    if ((screenNumber >= numDisplays) || (screenNumber < 0)) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychHideCursor() is out of range"); //also checked within SCREENPixelSizes

    // Not available on non-X11:
    if (!displayCGIDs[screenNumber]) return;

    // Cursor already hidden on screen? If so, nothing to do:
    if ((deviceIdx < 0) && displayCursorHidden[screenNumber]) return;

    // nullCursor already ready?
    if( nullCursor == (Cursor) -1 ) {
        // Create one:
//         Pixmap cursormask;
//         XGCValues xgc;
//         GC gc;
//         XColor dummycolour;
//
//         PsychLockDisplay();
//
//         cursormask = XCreatePixmap(displayCGIDs[screenNumber], RootWindow(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber)), 1, 1, 1/*depth*/);
//         xgc.function = GXclear;
//         gc = XCreateGC(displayCGIDs[screenNumber], cursormask, GCFunction, &xgc );
//         XFillRectangle(displayCGIDs[screenNumber], cursormask, gc, 0, 0, 1, 1 );
//         dummycolour.pixel = 0;
//         dummycolour.red   = 0;
//         dummycolour.flags = 04;
//         nullCursor = XCreatePixmapCursor(displayCGIDs[screenNumber], cursormask, cursormask, &dummycolour, &dummycolour, 0, 0 );
//         XFreePixmap(displayCGIDs[screenNumber], cursormask );
//         XFreeGC(displayCGIDs[screenNumber], gc );
//
//         PsychUnlockDisplay();
    }

    if (deviceIdx < 0) {
        // Attach nullCursor to our onscreen window:
//         PsychOSDefineX11Cursor(screenNumber, deviceIdx, nullCursor);
//         PsychLockDisplay();
//         XFlush(displayCGIDs[screenNumber]);
//         PsychUnlockDisplay();
        displayCursorHidden[screenNumber]=TRUE;
    } else {
//         // XInput cursor: Master pointers only.
//         int nDevices;
//         XIDeviceInfo* indevs = PsychGetInputDevicesForScreen(screenNumber, &nDevices);
//
//         // Sanity check:
//         if (NULL == indevs) PsychErrorExitMsg(PsychError_user, "Sorry, your system does not support individual mouse pointers.");
//         if (deviceIdx >= nDevices) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such cursor pointer.");
//         if (indevs[deviceIdx].use != XIMasterPointer) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such master cursor pointer.");
//
//         // Attach nullCursor to our onscreen window:
//         PsychOSDefineX11Cursor(screenNumber, indevs[deviceIdx].deviceid, nullCursor);
//         PsychLockDisplay();
//         XFlush(displayCGIDs[screenNumber]);
//         PsychUnlockDisplay();
    }

    return;
}

void PsychShowCursor(int screenNumber, int deviceIdx)
{
    Cursor arrowCursor;

    // Check for valid screenNumber:
    if (screenNumber >= numDisplays || screenNumber < 0) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychHideCursor() is out of range"); //also checked within SCREENPixelSizes

    // Not available on non-X11:
    if (!displayCGIDs[screenNumber]) return;

    if (deviceIdx < 0) {
        // Cursor not hidden on screen? If so, nothing to do:
        if(!displayCursorHidden[screenNumber]) return;

//         // Reset to standard Arrow-Type cursor, which is a visible one.
//         PsychLockDisplay();
//         arrowCursor = XCreateFontCursor(displayCGIDs[screenNumber], 2);
//         PsychUnlockDisplay();
//
//         PsychOSDefineX11Cursor(screenNumber, deviceIdx, arrowCursor);
//
//         PsychLockDisplay();
//         XFlush(displayCGIDs[screenNumber]);
//         PsychUnlockDisplay();

        displayCursorHidden[screenNumber]=FALSE;
    } else {
//         // XInput cursor: Master pointers only.
//         int nDevices;
//         XIDeviceInfo* indevs = PsychGetInputDevicesForScreen(screenNumber, &nDevices);
//
//         // Sanity check:
//         if (NULL == indevs) PsychErrorExitMsg(PsychError_user, "Sorry, your system does not support individual mouse pointers.");
//         if (deviceIdx >= nDevices) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such cursor pointer.");
//         if (indevs[deviceIdx].use != XIMasterPointer) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such master cursor pointer.");
//
//         // Reset to standard Arrow-Type cursor, which is a visible one.
//         PsychLockDisplay();
//         arrowCursor = XCreateFontCursor(displayCGIDs[screenNumber], 2);
//         PsychUnlockDisplay();
//
//         PsychOSDefineX11Cursor(screenNumber, indevs[deviceIdx].deviceid, arrowCursor);
//
//         PsychLockDisplay();
//         XFlush(displayCGIDs[screenNumber]);
//         PsychUnlockDisplay();
    }
}

void PsychPositionCursor(int screenNumber, int x, int y, int deviceIdx)
{
    // Not available on non-X11:
    if (!displayCGIDs[screenNumber]) return;

    // Reposition the mouse cursor:
    if (deviceIdx < 0) {
        // Core protocol cursor:
//         PsychLockDisplay();
//         if (XWarpPointer(displayCGIDs[screenNumber], None, RootWindow(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber)), 0, 0, 0, 0, x, y)==BadWindow) {
//             PsychUnlockDisplay();
//             PsychErrorExitMsg(PsychError_internal, "Couldn't position the mouse cursor! (XWarpPointer() failed).");
//         }
    } else {
        // XInput cursor: Master pointers only.
//         int nDevices;
//         XIDeviceInfo* indevs = PsychGetInputDevicesForScreen(screenNumber, &nDevices);
//
//         // Sanity check:
//         if (NULL == indevs) PsychErrorExitMsg(PsychError_user, "Sorry, your system does not support individual mouse pointers.");
//         if (deviceIdx >= nDevices) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such cursor pointer.");
//         if (indevs[deviceIdx].use != XIMasterPointer) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such master cursor pointer.");
//
//         PsychLockDisplay();
//         if (XIWarpPointer(displayCGIDs[screenNumber], indevs[deviceIdx].deviceid, None, RootWindow(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber)), 0, 0, 0, 0, x, y)) {
//             PsychUnlockDisplay();
//             PsychErrorExitMsg(PsychError_internal, "Couldn't position the mouse cursor! (XIWarpPointer() failed).");
//         }
    }

//     XFlush(displayCGIDs[screenNumber]);
//     PsychUnlockDisplay();
}

/*
    PsychReadNormalizedGammaTable()
*/
void PsychReadNormalizedGammaTable(int screenNumber, int outputId, int *numEntries, float **redTable, float **greenTable, float **blueTable)
{
    CGDirectDisplayID cgDisplayID;
    static float localRed[MAX_GAMMALUT_SIZE], localGreen[MAX_GAMMALUT_SIZE], localBlue[MAX_GAMMALUT_SIZE];

    // The X-Windows hardware LUT has 3 tables for R,G,B.
    // Each entry is a 16-bit word with the n most significant bits used for an n-bit DAC or display encoder:
    psych_uint16    RTable[MAX_GAMMALUT_SIZE];
    psych_uint16    GTable[MAX_GAMMALUT_SIZE];
    psych_uint16    BTable[MAX_GAMMALUT_SIZE];
    int i, n;

    // Initial assumption: Failed.
    n = 0;

    // Not available on non-X11:
    if (!displayCGIDs[screenNumber]) { *numEntries = 0; return; }

    // Query OS for gamma table:
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);

    if (has_xrandr_1_2) {
//         // Use RandR V 1.2 for per-crtc query:
//         XRRScreenResources *res = displayX11ScreenResources[screenNumber];
//
//         if (outputId >= kPsychMaxPossibleCrtcs) PsychErrorExitMsg(PsychError_user, "Invalid output index provided! No such output for this screen!");
//         outputId = PsychScreenToHead(screenNumber, ((outputId < 0) ? 0 : outputId));
//         if (outputId >= res->ncrtc || outputId < 0) PsychErrorExitMsg(PsychError_user, "Invalid output index provided! No such output for this screen!");
//
//         RRCrtc crtc = res->crtcs[outputId];
//         PsychLockDisplay();
//         XRRCrtcGamma *lut = XRRGetCrtcGamma(cgDisplayID, crtc);
//         PsychUnlockDisplay();
//
//         n = (lut) ? lut->size : 0;
//
//         if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: PsychReadNormalizedGammaTable: Provided RandR HW-LUT size is n=%i.\n", n);
//
//         // Gamma lut query successfull?
//         if (n > 0) {
//             if ((n > MAX_GAMMALUT_SIZE) && (PsychPrefStateGet_Verbosity() > 1)) {
//                 printf("PTB-WARNING: ReadNormalizedGammatable: Hardware gamma table size of %i slots exceeds our maximum of %i slots. Clamping returned size to %i slots. Returned LUT's may be wrong!\n", n, MAX_GAMMALUT_SIZE, MAX_GAMMALUT_SIZE);
//             }
//
//             // Clamp for safety:
//             n = (n <= MAX_GAMMALUT_SIZE) ? n : MAX_GAMMALUT_SIZE;
//
//             // Convert tables: Map 16-bit values into 0-1 normalized floats:
//             for (i = 0; i < n; i++) localRed[i]   = ((float) lut->red[i]) / 65535.0f;
//             for (i = 0; i < n; i++) localGreen[i] = ((float) lut->green[i]) / 65535.0f;
//             for (i = 0; i < n; i++) localBlue[i]  = ((float) lut->blue[i]) / 65535.0f;
//         }
//
//         if (lut) XRRFreeGamma(lut);
    }

    // Assign output lut's:
    *redTable=localRed; *greenTable=localGreen; *blueTable=localBlue;

    // Assign size of LUT's::
    *numEntries = n;

    return;
}

/* Copy provided input LUT into target output LUT. If input is smaller than target LUT, but
 * fits nicely because output size is an integral multiple of input, then oversample input
 * to create proper output. If size doesn't match or output is smaller than input, abort
 * with error.
 *
 * Rationale: LUT's of standard GPUs are 256 slots, LUT's of high-end GPU's can be bigger
 *            powers-of-two sizes, e.g., 512, 1024, 2048, 4096 for some NVidia QuadroFX
 *            parts. For reasons of backwards compatibility we always want to be able to
 *            accept a good'ol 256 slots input LUT, even if the GPU expects something bigger.
 *            -> This simple oversampling via replication allows us to do that without obvious
 *               bad consequences for precision.
 *
 */
static void ConvertLUTToHwLUT(int nOut, psych_uint16* Rout, psych_uint16* Gout, psych_uint16* Bout, int nIn, float *redTable, float *greenTable, float *blueTable)
{
    int i, s;

    // Can't handle too big input tables for GPU:
    if (nOut < nIn) {
        printf("PTB-ERROR: Provided gamma table has %i slots, but graphics card accepts at most %i slots!\n", nIn, nOut);
        PsychErrorExitMsg(PsychError_user, "Provided gamma table has too many slots!");
    }

    // Can't handle tables which don't fit:
    if ((nOut % nIn) != 0) {
        printf("PTB-ERROR: Provided gamma table has %i slots, but graphics card expects %i slots.\n", nIn, nOut);
        printf("PTB-ERROR: Unfortunately, graphics card LUT size is not a integral multiple of provided gamma table size.\n");
        printf("PTB-ERROR: I can not handle this case, as it could cause ugly distortions in the displayed color range.\n");
        PsychErrorExitMsg(PsychError_user, "Provided gamma table has wrong number of slots!");
    }

    // Compute oversampling factor:
    s = nOut / nIn;
    if (PsychPrefStateGet_Verbosity() > 5) {
        printf("PTB-DEBUG: PsychLoadNormalizedGammaTable: LUT size nIn %i <= nOut %i --> Oversample %i times.\n", nIn, nOut, s);
    }

    // Copy and oversample: Each slot in red/green/blueTable is replicated
    // into s consecutive slots of R/G/Bout:
    for (i = 0; i < nOut; i++) {
        Rout[i] = (psych_uint16) (redTable[i/s]   * 65535.0f + 0.5f);
        Gout[i] = (psych_uint16) (greenTable[i/s] * 65535.0f + 0.5f);
        Bout[i] = (psych_uint16) (blueTable[i/s]  * 65535.0f + 0.5f);
    }
}

unsigned int PsychLoadNormalizedGammaTable(int screenNumber, int outputId, int numEntries, float *redTable, float *greenTable, float *blueTable)
{
    CGDirectDisplayID cgDisplayID;
    int i, j, n;
    RRCrtc crtc;

    static psych_uint16    RTable[MAX_GAMMALUT_SIZE];
    static psych_uint16    GTable[MAX_GAMMALUT_SIZE];
    static psych_uint16    BTable[MAX_GAMMALUT_SIZE];

    // The X-Windows hardware LUT has 3 tables for R,G,B.
    // Each entry is a 16-bit word with the n most significant bits used for an n-bit DAC or display encoder.

    // Set new gammaTable:
    PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);

    // Initial assumption: Failure.
    n = 0;

    // Not available on non-X11:
    if (!displayCGIDs[screenNumber]) return(0);

    if (has_xrandr_1_2) {
//         // Use RandR V 1.2 for per-crtc setup:
//
//         // Setup of all crtc's with this gamma table requested?
//         if (outputId < 0) {
//             // Yes: Iterate over all outputs, set via recursive call:
//             j = 1;
//             for (i = 0; (j > 0) && (i < kPsychMaxPossibleCrtcs) && (PsychScreenToHead(screenNumber, i) > -1); i++) {
//                 j = PsychLoadNormalizedGammaTable(screenNumber, i, numEntries, redTable, greenTable, blueTable);
//             }
//
//             // Done trying to set all crtc's. Return status:
//             return((unsigned int) j);
//         }
//
//         // No, or recursive self-call: Load a specific crtc for output 'outputId':
//         XRRScreenResources *res = displayX11ScreenResources[screenNumber];
//
//         if (outputId >= kPsychMaxPossibleCrtcs) PsychErrorExitMsg(PsychError_user, "Invalid output index provided! No such output for this screen!");
//         outputId = PsychScreenToHead(screenNumber, outputId);
//         if (outputId >= res->ncrtc || outputId < 0) PsychErrorExitMsg(PsychError_user, "Invalid output index provided! No such output for this screen!");
//
//         crtc = res->crtcs[outputId];
//
//         // Get required size of gamma table:
//         PsychLockDisplay();
//         n = XRRGetCrtcGammaSize(cgDisplayID, crtc);
//         PsychUnlockDisplay();
//         if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: PsychLoadNormalizedGammaTable: Required RandR HW-LUT size is n=%i.\n", n);
    }

    // RandR 1.2 supported and has ability to set LUT's?
    if (has_xrandr_1_2 && (n > 0)) {
//         // Allocate table of appropriate size:
//         XRRCrtcGamma *lut = XRRAllocGamma(n);
//         n = lut->size;
//
//         // Convert tables: Map 16-bit values into 0-1 normalized floats:
//         ConvertLUTToHwLUT(n, lut->red, lut->green, lut->blue, numEntries, redTable, greenTable, blueTable);
//
//         // Assign to crtc:
//         PsychLockDisplay();
//         XRRSetCrtcGamma(cgDisplayID, crtc, lut);
//         PsychUnlockDisplay();
//
//         // Release lut:
//         XRRFreeGamma(lut);
    }

//     PsychLockDisplay();
//     XFlush(cgDisplayID);
//     PsychUnlockDisplay();

    // Return "success":
    return(1);
}

// Return true (non-zero) if a desktop compositor is likely active on screen 'screenNumber':
int PsychOSIsDWMEnabled(int screenNumber)
{
    // This is Wayland. There is always a compositor active, but what this
    // function actually returns is if a compositor is active which is
    // incompatible with good visual presentation timing and timestamping.
    // Wayland should not have this problem, at least with the presentation
    // extension available and enabled, so return an optimistic "No":
    #if PTB_USE_WAYLAND_PRESENT
    // TODO: Actually check for runtime support for extension.
    return(0);
    #else
    return(1);
    #endif
}

// !PTB_USE_WAYLAND
#endif
