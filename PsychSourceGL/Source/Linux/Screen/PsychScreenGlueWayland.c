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

    TODO: Implement the following functions:

    PsychGetCGModeFromVideoSetting
    PsychGetScreenDepth
    PsychSetNominalFramerate
    PsychOSSetOutputConfig
    PsychSetScreenSettings
    PsychRestoreScreenSettings
    PsychOSDefineX11Cursor
    PsychHideCursor
    PsychShowCursor
    PsychPositionCursor
    PsychReadNormalizedGammaTable
    PsychLoadNormalizedGammaTable

*/

/*
 * Some of the helper code for wl_output and wl_seat enumeration is copied
 * with modifications from the Weston example client "weston-info.c"
 *
 * That code carries the following copyright as of December 2014 (Weston 1.6+):
 *
 * Copyright © 2012 Philipp Brüschweiler
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The copyright holders make
 * no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
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
static psych_bool wayland_roundtrip_needed = FALSE;

// Helpers so we can easily use/include/upgrade to Weston client sample code:
static void *
xmalloc(size_t s)
{
    return(malloc(s));
}

static void *
xzalloc(size_t s)
{
    return(calloc(1, s));
}

static char *
xstrdup(const char *s)
{
    return(strdup(s));
}

struct output_mode {
    struct wl_list link;

    uint32_t flags;
    int32_t width, height;
    int32_t refresh;
};

struct output_info {
    // struct global_info global;

    struct wl_output *output;

    struct {
        int32_t x, y;
        int32_t physical_width, physical_height;
        enum wl_output_subpixel subpixel;
        enum wl_output_transform output_transform;
        char *make;
        char *model;
    } geometry;

    struct wl_list modes;
};

// Array of information about all available Wayland outputs:
static struct output_info* displayOutputs[kPsychMaxPossibleDisplays];
// Same as above, but only stores a pointer to the wl_output:
// Currently shared with PsychWindoeGlueWayland.c:
struct wl_output* displayWaylandOutputs[kPsychMaxPossibleDisplays];

static void
print_output_info(void *data)
{
    struct output_info *output = data;
    struct output_mode *mode;
    const char *subpixel_orientation;
    const char *transform;

    //print_global_info(data);

    switch (output->geometry.subpixel) {
        case WL_OUTPUT_SUBPIXEL_UNKNOWN:
            subpixel_orientation = "unknown";
            break;
        case WL_OUTPUT_SUBPIXEL_NONE:
            subpixel_orientation = "none";
            break;
        case WL_OUTPUT_SUBPIXEL_HORIZONTAL_RGB:
            subpixel_orientation = "horizontal rgb";
            break;
        case WL_OUTPUT_SUBPIXEL_HORIZONTAL_BGR:
            subpixel_orientation = "horizontal bgr";
            break;
        case WL_OUTPUT_SUBPIXEL_VERTICAL_RGB:
            subpixel_orientation = "vertical rgb";
            break;
        case WL_OUTPUT_SUBPIXEL_VERTICAL_BGR:
            subpixel_orientation = "vertical bgr";
            break;
        default:
            fprintf(stderr, "unknown subpixel orientation %u\n",
                    output->geometry.subpixel);
            subpixel_orientation = "unexpected value";
            break;
    }

    switch (output->geometry.output_transform) {
        case WL_OUTPUT_TRANSFORM_NORMAL:
            transform = "normal";
            break;
        case WL_OUTPUT_TRANSFORM_90:
            transform = "90°";
            break;
        case WL_OUTPUT_TRANSFORM_180:
            transform = "180°";
            break;
        case WL_OUTPUT_TRANSFORM_270:
            transform = "270°";
            break;
        case WL_OUTPUT_TRANSFORM_FLIPPED:
            transform = "flipped";
            break;
        case WL_OUTPUT_TRANSFORM_FLIPPED_90:
            transform = "flipped 90°";
            break;
        case WL_OUTPUT_TRANSFORM_FLIPPED_180:
            transform = "flipped 180°";
            break;
        case WL_OUTPUT_TRANSFORM_FLIPPED_270:
            transform = "flipped 270°";
            break;
        default:
            fprintf(stderr, "unknown output transform %u\n",
                    output->geometry.output_transform);
            transform = "unexpected value";
            break;
    }

    printf("\tx: %d, y: %d,\n",
           output->geometry.x, output->geometry.y);
    printf("\tphysical_width: %d mm, physical_height: %d mm,\n",
           output->geometry.physical_width,
           output->geometry.physical_height);
    printf("\tmake: '%s', model: '%s',\n",
           output->geometry.make, output->geometry.model);
    printf("\tsubpixel_orientation: %s, output_transform: %s,\n",
           subpixel_orientation, transform);

    wl_list_for_each(mode, &output->modes, link) {
        printf("\tmode:\n");

        printf("\t\twidth: %d px, height: %d px, refresh: %.f Hz,\n",
               mode->width, mode->height,
               (float) mode->refresh / 1000);

        printf("\t\tflags:");
        if (mode->flags & WL_OUTPUT_MODE_CURRENT)
            printf(" current");
        if (mode->flags & WL_OUTPUT_MODE_PREFERRED)
            printf(" preferred");
        printf("\n");
    }
}

static void
output_handle_geometry(void *data, struct wl_output *wl_output,
                       int32_t x, int32_t y,
                       int32_t physical_width, int32_t physical_height,
                       int32_t subpixel,
                       const char *make, const char *model,
                       int32_t output_transform)
{
    struct output_info *output = data;

    output->geometry.x = x;
    output->geometry.y = y;
    output->geometry.physical_width = physical_width;
    output->geometry.physical_height = physical_height;
    output->geometry.subpixel = subpixel;
    output->geometry.make = xstrdup(make);
    output->geometry.model = xstrdup(model);
    output->geometry.output_transform = output_transform;
}

static void
output_handle_mode(void *data, struct wl_output *wl_output,
                   uint32_t flags, int32_t width, int32_t height,
                   int32_t refresh)
{
    struct output_info *output = data;
    struct output_mode *mode = xmalloc(sizeof *mode);

    mode->flags = flags;
    mode->width = width;
    mode->height = height;
    mode->refresh = refresh;

    wl_list_insert(output->modes.prev, &mode->link);
}

static const struct wl_output_listener output_listener = {
    output_handle_geometry,
    output_handle_mode,
    0,
    0
};

static void
destroy_output_info(void *data)
{
    struct output_info *output = data;
    struct output_mode *mode, *tmp;

    wl_output_destroy(output->output);

    if (output->geometry.make != NULL)
        free(output->geometry.make);
    if (output->geometry.model != NULL)
        free(output->geometry.model);

    wl_list_for_each_safe(mode, tmp, &output->modes, link) {
        wl_list_remove(&mode->link);
        free(mode);
    }
}

static void add_output_info(struct output_info** outputSlot, uint32_t id, uint32_t version)
{
    struct output_info *output = xzalloc(sizeof *output);

    // init_global_info(info, &output->global, id, "wl_output", version);
    // output->global.print = print_output_info;
    // output->global.destroy = destroy_output_info;
    *outputSlot = output;

    wl_list_init(&output->modes);

    output->output = wl_registry_bind(wl_registry, id,
                                      &wl_output_interface, 1);
    wl_output_add_listener(output->output, &output_listener,
                           output);
    wayland_roundtrip_needed = TRUE;
}

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

    // Look for presentation interface v1+:
    if (!strcmp(interface, "presentation") && (version >= 1)) {
        wayland_pres = wl_registry_bind(registry, name, &presentation_interface, 1);
        if (!wayland_pres) {
            if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: wl_registry_bind for presentation_interface failed!\n");
            return;
        }

        presentation_add_listener(wayland_pres, &wayland_presentation_listener, self);
        if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-DEBUG: Wayland presentation_interface bound!\n");
    }

    // Look for Wayland outputs ~ video outputs ~ displays ~ our PTB screens:
    // Not yet sure if wl_output ~ PTB screen is the optimal abstraction/mapping,
    // but as a starter...
    if (!strcmp(interface, "wl_output") && (version >= 1)) {
        add_output_info(&displayOutputs[numDisplays], name, version);
        if (PsychPrefStateGet_Verbosity() > 2) {
            printf("PTB-DEBUG: New output display screen %i enumerated.\n", numDisplays);
        }
        numDisplays++;
    }

    return;
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

static void ProcessWaylandEvents(int screenNumber)
{
    // Block until the Wayland server has processed all pending requests and
    // has sent out pending events on all event queues. This should ensure
    // that the registry listener has received announcement of the shell and
    // compositor.
    do {
        wayland_roundtrip_needed = FALSE;
        if (wl_display_roundtrip(wl_display) == -1) {
            if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: ProcessWaylandEvents(): wl_display_roundtrip failed!\n");
        }
        // Repeat until everything is enumerated.
    } while (wayland_roundtrip_needed);
}

struct output_mode* PsychWaylandGetCurrentMode(int screenId)
{
    struct output_info* output;
    struct output_mode* mode;

    if (screenId >= numDisplays || screenId < 0) PsychErrorExitMsg(PsychError_internal, "screenNumber is out of range");
    output = displayOutputs[screenId];

    wl_list_for_each(mode, &output->modes, link) {
        if (mode->flags & WL_OUTPUT_MODE_CURRENT)
            return(mode);
    }

    return(NULL);
}

// Linux only: Retrieve modeline and crtc_info for a specific output on a specific screen:
// Caution: If crtc is non-NULL and receives a valid XRRCrtcInfo*, then this pointer must
//          be released by the caller via XRRFreeCrtcInfo(crtc), or resources will leak!
// Must be called under display lock protection!
XRRModeInfo* PsychOSGetModeLine(int screenId, int outputIdx, XRRCrtcInfo **crtc)
{
    int m;
    static XRRModeInfo rrmode; // XRRAllocModeInfo (char *name, int nameLength);
    struct output_info* output;
    struct output_mode* mode;

    if (screenId >= numDisplays || screenId < 0) PsychErrorExitMsg(PsychError_internal, "screenNumber is out of range");
    output = displayOutputs[screenId];

    memset(&rrmode, 0, sizeof(rrmode));
    XRRCrtcInfo *crtc_info = calloc(1, sizeof(XRRCrtcInfo));

    // Query info about video modeline and crtc of output 'outputIdx':
    if (PsychScreenToHead(screenId, outputIdx) >= 0) {
        // Get current video mode for output:
        mode = PsychWaylandGetCurrentMode(screenId);

        // Assign minimal needed info to satisfy our callers:
        rrmode.width = mode->width;
        rrmode.height = mode->height;

        // Only fill (x,y) start position in compositor space:
        crtc_info->x = output->geometry.x;
        crtc_info->y = output->geometry.y;

        // Not quite true, as this should be viewport size, not
        // size of video mode:
        crtc_info->width = mode->width;
        crtc_info->height = mode->height;

        // Could do rotation if we wanted...
    }

    // Optionally return crtc_info in *crtc:
    if (crtc) {
        // Return crtc_info, if any - NULL otherwise:
        *crtc = crtc_info;
    }
    else {
        // crtc_info not required by caller. We release it:
        if (crtc_info) XRRFreeCrtcInfo(crtc_info);
    }

    // Return a pointer to our static rrmode. Welcome to hackistan!
    return(&rrmode);
}

void InitCGDisplayIDList(void)
{
    int i;

    // Define waffle window system backend to use: Wayland only, obviously.
    static int32_t init_attrs[3] = {
        WAFFLE_PLATFORM,
        WAFFLE_PLATFORM_WAYLAND,
        0,
    };

    // NULL-out array of displays:
    for (i = 0; i < kPsychMaxPossibleDisplays; i++) {
        displayCGIDs[i] = NULL;
        displayWaylandOutputs[i] = NULL;
    }

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
    do {
        wayland_roundtrip_needed = FALSE;
        if (wl_display_roundtrip(wl_display) == -1) {
            if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: wl_display_roundtrip failed\n");
            PsychErrorExitMsg(PsychError_system, "FATAL ERROR: Initialisation failed! Game over!");
        }
        // Repeat until everything is enumerated.
    } while (wayland_roundtrip_needed);

    // Initialize screenId -> GPU headId mapping to identity mappings for numDisplays:
    PsychInitScreenToHeadMappings(numDisplays);

    // Setup the screenNumber --> Wayland display mappings:
    for (i = 0; i < numDisplays && i < kPsychMaxPossibleDisplays; i++) {
        // Store the wl_output* handle for retrieval by our client code:
        displayWaylandOutputs[i] = displayOutputs[i]->output;

        // Detailed info about enumerated output:
        if (PsychPrefStateGet_Verbosity() > 2) print_output_info(displayOutputs[i]);

        // Set reference crtc == our output info for primary output to always 0,
        // to create a screen == output mapping:
        PsychSetScreenToHead(i, 0, 0);

        //displayX11Screens[i] = i;
        //xinput_info[i] = xinput_info[0];
        //xinput_ndevices[i] = xinput_ndevices[0];

        // Get all relevant screen config info and cache it internally:
        // GetRandRScreenConfig(x11_dpy, i);
    }

    if (numDisplays > 1) printf("PTB-INFO: A total of %i Wayland display screens is available for use.\n", numDisplays);

    return;
}

void PsychCleanupDisplayGlue(void)
{
    int i;

    // Make sure the mmio mapping is shut down:
    PsychOSShutdownPsychtoolboxKernelDriverInterface();

    PsychLockDisplay();

    // Go trough full screen list:
    for (i = 0; i < PsychGetNumDisplays(); i++) {
        if (displayOutputs[i]) {
            destroy_output_info(displayOutputs[i]);
            displayOutputs[i] = NULL;
            displayWaylandOutputs[i] = NULL;
            displayCGIDs[i] = NULL;
        }

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

    if (screenNumber >= numDisplays || screenNumber < 0) PsychErrorExitMsg(PsychError_internal, "screenNumber is out of range");

    // Update out view of this screens configuration:
    PsychLockDisplay();
//      ProcessWaylandEvents(screenNumber);
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
    // This routine is not really needed anymore. Only theoretically called from
    // SCREENResolutions.c, where it gets skipped anyway. Just define a no-op
    // implementation to avoid linker failure or ugly workarounds for the moment:
    return(0);
}

/*   PsychGetAllSupportedScreenSettings()
 *
 *     Queries the display system for a list of all supported display modes, ie. all valid combinations
 *     of resolution, pixeldepth and refresh rate. Allocates temporary arrays for storage of this list
 *     and returns it to the calling routine. This function is basically only used by Screen('Resolutions').
 */
int PsychGetAllSupportedScreenSettings(int screenNumber, int outputId, long** widths, long** heights, long** hz, long** bpp)
{
    int i, numPossibleModes;
    struct output_info* output_info = NULL;
    struct output_mode* mode = NULL;

    if (screenNumber >= numDisplays || screenNumber < 0) PsychErrorExit(PsychError_invalidScumber);

    PsychLockDisplay();
    ProcessWaylandEvents(screenNumber);
    PsychUnlockDisplay();

    if (outputId < 0) {
        // Count number of available modes:
        numPossibleModes = 0;
        output_info = displayOutputs[screenNumber];
        wl_list_for_each(mode, &output_info->modes, link) {
            numPossibleModes++;
        }

        // Allocate output arrays: These will get auto-released at exit from Screen():
        *widths  = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
        *heights = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
        *hz      = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
        *bpp     = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));

        // Reiterate and fill all slots:
        i = 0;
        wl_list_for_each(mode, &output_info->modes, link) {
            if (i == numPossibleModes) break;

            (*widths)[i]  = (long) mode->width;
            (*heights)[i] = (long) mode->height;
            (*hz)[i]      = (long) ((float) mode->refresh / 1000.0);
            (*bpp)[i]     = (long) PsychGetScreenDepthValue(screenNumber);

            i++;
        }

        // Done:
        return(numPossibleModes);
    }

    if (outputId >= kPsychMaxPossibleCrtcs) PsychErrorExitMsg(PsychError_user, "Invalid output index provided! No such output for this screen!");

    // A bit dull: We only have one output per screen atm., as we go with the screen == output model for initial Wayland enablement:
    if (outputId >= 1 || outputId < 0) PsychErrorExitMsg(PsychError_user, "Invalid output index provided! No such output for this screen!");
    outputId = PsychScreenToHead(screenNumber, outputId);

    // Now have literally the same code as for the outputId < 0 case above,
    // Pretty dumb, but best we can do atm. given the screenid = outputid mapping.
    // TODO: Needs more thought...

    // Count number of available modes:
    numPossibleModes = 0;
    output_info = displayOutputs[screenNumber];
    wl_list_for_each(mode, &output_info->modes, link) {
        numPossibleModes++;
    }

    // Allocate output arrays: These will get auto-released at exit from Screen():
    *widths  = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
    *heights = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
    *hz      = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));
    *bpp     = (long*) PsychMallocTemp(numPossibleModes * sizeof(long));

    // Reiterate and fill all slots:
    i = 0;
    wl_list_for_each(mode, &output_info->modes, link) {
        if (i == numPossibleModes) break;

        (*widths)[i]  = (long) mode->width;
        (*heights)[i] = (long) mode->height;
        (*hz)[i]      = (long) ((float) mode->refresh / 1000.0);
        (*bpp)[i]     = (long) PsychGetScreenDepthValue(outputId);

        i++;
    }

    // Done:
    return(numPossibleModes);
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
// TODO: Make it work for real.
    PsychLockDisplay();
//     ProcessWaylandEvents(screenNumber);
//
//     if (displayCGIDs[screenNumber]) {
//         PsychAddValueToDepthStruct(DefaultDepth(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber)), depth);
//     }
//     else {
        PsychAddValueToDepthStruct(24, depth);
//     }

    PsychUnlockDisplay();
}

float PsychGetNominalFramerate(int screenNumber)
{
    struct output_mode* mode = NULL;
    float vrefresh = 0;

    if (PsychPrefStateGet_ConserveVRAM() & kPsychIgnoreNominalFramerate) return(0);

    if (screenNumber >= numDisplays || screenNumber < 0)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetNominalFramerate() is out of range");

    // Get current video mode for screenNumber:
    mode = PsychWaylandGetCurrentMode(screenNumber);
    if (mode == NULL) return(0);

    // Convert nominal refresh rate in milliHz to Hz and return it:
    return((float) mode->refresh / 1000.0);
}

float PsychSetNominalFramerate(int screenNumber, float requestedHz)
{
    // We start with a default vrefresh of zero, which means "couldn't query refresh from OS":
    float vrefresh = 0;

    if (screenNumber >= numDisplays || screenNumber < 0)
        PsychErrorExitMsg(PsychError_internal, "screenNumber is out of range");

    // Not available on non-X11:
    if (!displayOutputs[screenNumber]) return(0);

    // TODO: Could store target rate to set in a "pending new Hz" array,
    // and try to apply it via wl_shell fullscreen request...
    return(0);
}

/* Returns the physical display size in mm */
void PsychGetDisplaySize(int screenNumber, int *width, int *height)
{
    if (screenNumber >= numDisplays || screenNumber < 0)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetDisplaySize() is out of range");

    if (!displayOutputs[screenNumber]) { *width = 0; *height = 0; return; }

    // Update XLib's view of this screens configuration:
    PsychLockDisplay();
    ProcessWaylandEvents(screenNumber);

    *width = (int) displayOutputs[screenNumber]->geometry.physical_width;
    *height = (int) displayOutputs[screenNumber]->geometry.physical_height;

    PsychUnlockDisplay();
}

// Get size of a video output in pixels - active scanout area in pixels:
void PsychGetScreenPixelSize(int screenNumber, long *width, long *height)
{
    struct output_mode* mode = NULL;

    if (screenNumber >= numDisplays || screenNumber < 0) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenPixelSize() is out of range");
    if (!displayOutputs[screenNumber]) PsychErrorExitMsg(PsychError_system, "Could not query screen size in PsychGetScreenPixelSize() for wanted screen");

    // Update XLib's view of this screens configuration:
    PsychLockDisplay();
    ProcessWaylandEvents(screenNumber);

    // Get size from current mode:
    // TODO: Make sure to take output scaling info provided via wl_output listener callback into account:
    mode = PsychWaylandGetCurrentMode(screenNumber);
    if (!mode) PsychErrorExitMsg(PsychError_system, "Could not query screen size in PsychGetScreenPixelSize() for wanted screen");

    *width = (int) mode->width;
    *height = (int) mode->height;

    PsychUnlockDisplay();
}

// Width and height of output in compositor space units (points?):
void PsychGetScreenSize(int screenNumber, long *width, long *height)
{
    struct output_mode* mode = NULL;

    if (screenNumber >= numDisplays || screenNumber < 0) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenSize() is out of range");
    if (!displayOutputs[screenNumber]) PsychErrorExitMsg(PsychError_system, "Could not query screen size in PsychGetScreenSize() for wanted screen");

    // Update XLib's view of this screens configuration:
    PsychLockDisplay();
    ProcessWaylandEvents(screenNumber);

    // Get size from current mode:
    // TODO: Make sure to take output scaling info provided via wl_output listener callback into account,
    // so we don't report wrong values on HiDPI / Retina style displays:
    mode = PsychWaylandGetCurrentMode(screenNumber);
    if (!mode) PsychErrorExitMsg(PsychError_system, "Could not query screen size in PsychGetScreenSize() for wanted screen");

    *width = mode->width;
    *height = mode->height;

    PsychUnlockDisplay();
}

// Global bounding rectangle of output in compositor space coordinates:
void PsychGetGlobalScreenRect(int screenNumber, double *rect)
{
    PsychGetScreenRect(screenNumber, rect);
    rect[kPsychLeft]   += (int) displayOutputs[screenNumber]->geometry.x;
    rect[kPsychRight]  += (int) displayOutputs[screenNumber]->geometry.x;
    rect[kPsychTop]    += (int) displayOutputs[screenNumber]->geometry.y;
    rect[kPsychBottom] += (int) displayOutputs[screenNumber]->geometry.y;;
}

// Bounding rectangle of output in compositor space units (points?):
void PsychGetScreenRect(int screenNumber, double *rect)
{
    long width, height; 

    PsychGetScreenSize(screenNumber, &width, &height);
    rect[kPsychLeft] = 0;
    rect[kPsychTop] = 0;
    rect[kPsychRight] = (int) width;
    rect[kPsychBottom] = (int) height;
}

/*
    This is a place holder for a function which uncovers the number of dacbits.  To be filled in at a later date.

    There seems to be no way to uncover the dacbits programatically.
    For now we just use pessimistic 8 bits to avoid false precision.
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
//         ProcessWaylandEvents(screenNumber);
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
//         ProcessWaylandEvents(settings->screenNumber);
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
//     ProcessWaylandEvents(settings->screenNumber);
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
//     ProcessWaylandEvents(settings->screenNumber);

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
    // extension available and enabled, so return an optimistic "No" if the
    // extension is available and enabled:
    return((wayland_pres) ? 0 : 1);
}

// !PTB_USE_WAYLAND
#endif
