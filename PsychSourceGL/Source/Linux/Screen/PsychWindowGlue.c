/*
 *    PsychToolbox3/Source/Linux/Screen/PsychWindowGlue.c
 *
 *    PLATFORMS:
 *
 *        This is the Linux/X11 version only.
 *
 *    AUTHORS:
 *
 *        Mario Kleiner       mk      mario.kleiner.de@gmail.com
 *
 *    HISTORY:
 *
 *        2/20/06             mk      Created - Derived from Windows version.
 *
 *    DESCRIPTION:
 *
 *        Functions in this file comprise an abstraction layer for probing and controlling window state, except for window content.
 *
 *        Each C function which implements a particular Screen subcommand should be platform neutral.  For example, the source to SCREENPixelSizes()
 *        should be platform-neutral, despite that the calls in OS X and Linux to detect available pixel sizes are different.  The platform
 *        specificity is abstracted out in C files which end it "Glue", for example PsychScreenGlue, PsychWindowGlue, PsychWindowTextClue.
 *
 *    NOTES:
 *
 *    TO DO:
 *
 */

#include "Screen.h"

// Array with register offsets of the CRTCs used by AMD/ATI gpus.
// Defined in PsychGraphicsHardwareHALSupport.c, but accessed here:
extern unsigned int crtcoff[kPsychMaxPossibleCrtcs];

// If non-zero entries, then we are or have been running on Mesa
// and mesaversion encodes major.minor.patchlevel. Gets zero-initialized
// in X11 screen glue at Screen() load time, then assigned proper values
// during PsychOSOpenOnscreenWindow() if running under Mesa:
int mesaversion[3];

/* Following code is shared between the classic X11/GLX backend and the new Waffle backend: */

/* These are needed for realtime scheduling control: */
#include <sched.h>
#include <errno.h>

// utsname for uname() so we can find out on which kernel we're running:
#include <sys/utsname.h>

// Needed for our custom PRIME outputSink timestamping via UDP connection
// to the hacked up modesetting ddx:
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct _buf {
    uint64_t frame;
    uint64_t usec;
    int scrnIndex;
    unsigned char flags;
} _buf;

static int prime_sockfd[kPsychMaxPossibleDisplays] = { 0 };
static int prime_sockfd2[kPsychMaxPossibleDisplays] = { 0 };

// Pointer barriers, global array for all windows:
#define PSYCH_MAX_POINTER_BARRIERS 1024
static PointerBarrier barriers[PSYCH_MAX_POINTER_BARRIERS] = { 0 };
static PsychRectType barrierRects[PSYCH_MAX_POINTER_BARRIERS] = { 0 };
static PsychWindowRecordType* barrierParentWindows[PSYCH_MAX_POINTER_BARRIERS] = { 0 };
static int pointerBarrierCount = 0;

static Atom _NET_WM_FRAME_DRAWN_atom;
static Atom _NET_WM_FRAME_TIMINGS_atom;

// Backup for old screen saver settings before disable:
static int oldscreensaver[4];

static psych_int64 PsychOSGetPostSwapSBC(PsychWindowRecordType *windowRecord);

/** PsychRealtimePriority: Temporarily boost priority to highest available priority on Linux.
 *    PsychRealtimePriority(true) enables realtime-scheduling (like Priority(>0) would do in Matlab).
 *    PsychRealtimePriority(false) restores scheduling to the state before last invocation of PsychRealtimePriority(true),
 *    it undos whatever the previous switch did.
 *
 *    We switch to RT scheduling during PsychGetMonitorRefreshInterval() and a few other timing tests in
 *    PsychOpenWindow() to reduce measurement jitter caused by possible interference of other tasks.
 */
psych_bool PsychRealtimePriority(psych_bool enable_realtime)
{
    static psych_bool old_enable_realtime = FALSE;
    static int   oldPriority = SCHED_OTHER;
    const  int   realtime_class = SCHED_FIFO;
    struct sched_param param;
    static struct sched_param oldparam;

    if (old_enable_realtime == enable_realtime) {
        // No transition with respect to previous state -> Nothing to do.
        return(true);
    }

    // Transition requested:
    if (enable_realtime) {
        // Transition to realtime requested:

        // Get current scheduling policy and back it up for later restore:
        pthread_getschedparam(pthread_self(), &oldPriority, &oldparam);

        // Check if realtime scheduling isn't already active.
        // If we are already in RT mode (e.g., Priority(2) call in Matlab), we skip the switch...
        if (oldPriority != realtime_class) {
            // RT scheduling not yet active -> Switch to it.
            // We use the smallest realtime priority that's available for realtime_class.
            // This way, other processes like watchdogs can preempt us, if needed.
            param.sched_priority = sched_get_priority_min(realtime_class);
            if (pthread_setschedparam(pthread_self(), realtime_class, &param)) {
                // Failed!
                if(!PsychPrefStateGet_SuppressAllWarnings()) {
                    printf("PTB-INFO: Failed to enable realtime-scheduling [%s]!\n", strerror(errno));
                    if (errno==EPERM) {
                        printf("PTB-INFO: You need to run the script PsychLinuxConfiguration once, then logout and login again for this to work.\n");
                    }
                }
                errno=0;
            }
        }
    }
    else {
        // Transition from RT to whatever-it-was-before scheduling requested: We just reestablish the backed-up old
        // policy: If the old policy wasn't Non-RT, then we don't switch back...
        if (oldPriority != realtime_class) oldparam.sched_priority = 0;

        if (pthread_setschedparam(pthread_self(), oldPriority, &oldparam)) {
            // Failed!
            if(!PsychPrefStateGet_SuppressAllWarnings()) {
                printf("PTB-INFO: Failed to disable realtime-scheduling [%s]!\n", strerror(errno));
                if (errno==EPERM) {
                    printf("PTB-INFO: You need to run the script PsychLinuxConfiguration once, then logout and login again, for this to work.\n");
                }
            }
            errno=0;
        }
    }

    //printf("PTB-INFO: Realtime scheduling %sabled\n", enable_realtime ? "en" : "dis");

    // Success.
    old_enable_realtime = enable_realtime;
    return(TRUE);
}

/* The following code is only used for implementation of the classic X11/GLX backend: */
#ifndef PTB_USE_WAYLAND

// Perform OS specific processing of Window events:
void PsychOSProcessEvents(PsychWindowRecordType *windowRecord, int flags)
{
    Window rootRet;
    unsigned int depth_return, border_width_return, w, h;
    int x, y;

    (void) flags;

    // Trigger event queue dispatch processing for GUI windows:
    if (windowRecord == NULL) {
        // No op, so far...
        return;
    }

    // No-Op if we are not running on a X11 based display backend:
    if (!windowRecord->targetSpecific.privDpy || !windowRecord->targetSpecific.xwindowHandle) return;

    // GUI windows need to behave GUIyee:
    if ((windowRecord->specialflags & kPsychGUIWindow) && PsychIsOnscreenWindow(windowRecord) && !(windowRecord->specialflags & kPsychFbOverrideSizeActive)) {
        // Update windows rect and globalrect, based on current size and location:
        PsychLockDisplay();
        XGetGeometry(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.xwindowHandle, &rootRet, &x, &y,
                     &w, &h, &border_width_return, &depth_return);
        XTranslateCoordinates(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.xwindowHandle, rootRet,
                              0,0, &x, &y, &rootRet);
        PsychUnlockDisplay();

        PsychMakeRect(windowRecord->globalrect, x, y, x + (int) w, y + (int) h);
        PsychNormalizeRect(windowRecord->globalrect, windowRecord->rect);
        PsychSetupClientRect(windowRecord);
        PsychSetupView(windowRecord, FALSE);
    }
}

#ifndef PTB_USE_WAFFLE

/* XAtom support for setup of transparent windows: */
#include <X11/Xatom.h>

// For detection of DRI3/Present support:
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/dri3.h>
#include <xcb/present.h>

// For DPMS control:
#include <X11/extensions/dpms.h>

// For enhanced Present extension tricks involving the desktop compositor:
#include <X11/extensions/Xcomposite.h>

// For enhanced Present timing under desktop composition via _NET_WM_FRAME_DRAWN et al.:
#include <X11/extensions/sync.h>

// Use dedicated x-display handles for each onscreen window?
static psych_bool usePerWindowXConnections = FALSE;

// Use GLX version 1.3 setup code? Enabled INTEL_SWAP_EVENTS and other goodies...
static psych_bool useGLX13 = FALSE;

// Event base for GLX extension:
static int glx_error_base, glx_event_base;

static int xfixes_event_base1, xfixes_event_base2, xfixes_majorversion;
static psych_bool xfixes_available = FALSE;

static int xsync_event_base1, xsync_event_base2;
static psych_bool xsync_available = FALSE;

// Number of currently open onscreen windows:
static int x11_windowcount = 0;

#ifndef GLX_BUFFER_SWAP_COMPLETE_INTEL_MASK
#define GLX_BUFFER_SWAP_COMPLETE_INTEL_MASK	0x04000000
#endif

#ifndef GLX_BufferSwapComplete
#define GLX_BufferSwapComplete	1
#endif

typedef struct GLXBufferSwapComplete {
    int type;
    unsigned long serial;	/* # of last request processed by server */
    Bool send_event;		/* true if this came from a SendEvent request */
    Display *display;		/* Display the event was read from */
    GLXDrawable drawable;	/* drawable on which event was requested in event mask */
    int event_type;
    int64_t ust;
    int64_t msc;
    int64_t sbc;
} GLXBufferSwapComplete;

/* Detect DRI3/Present support. Must be called under PsychLockDisplay() protection. */
static psych_bool IsDRI3Supported(PsychWindowRecordType *windowRecord)
{
    const xcb_query_extension_reply_t *dri3extension;
    xcb_dri3_query_version_cookie_t cookie;
    xcb_dri3_query_version_reply_t *reply;
    xcb_dri3_open_cookie_t open_cookie;
    xcb_dri3_open_reply_t *open_reply;
    xcb_generic_error_t *error;
    int major, minor;
    Display *dpy = windowRecord->targetSpecific.deviceContext;

    // Try to connect to DRI3 extension:
    dri3extension = xcb_get_extension_data(XGetXCBConnection(dpy), &xcb_dri3_id);
    if ((dri3extension == NULL) || !dri3extension->present) {
        return(FALSE);
    }

    cookie = xcb_dri3_query_version(XGetXCBConnection(dpy), XCB_DRI3_MAJOR_VERSION, XCB_DRI3_MINOR_VERSION);
    reply = xcb_dri3_query_version_reply(XGetXCBConnection(dpy), cookie, &error);
    free(error);
    if (reply == NULL) return(FALSE);

    major = reply->major_version;
    minor = reply->minor_version;
    free(reply);
    if (major < 0) return(FALSE);

    open_cookie = xcb_dri3_open(XGetXCBConnection(dpy), RootWindow(dpy, DefaultScreen(dpy)), None);
    open_reply = xcb_dri3_open_reply(XGetXCBConnection(dpy), open_cookie, NULL);

    if (!open_reply) return(FALSE);
    if (open_reply->nfd != 1) return(FALSE);
    close(xcb_dri3_open_reply_fds(XGetXCBConnection(dpy), open_reply)[0]);
    free(open_reply);

    // Seems we are running on a DRI3 capable driver. A modern Mesa
    // version would use DRI3/Present on such a driver. Is DRI3/Present
    // disabled by env variable?
    if (getenv("LIBGL_DRI3_DISABLE")) return(FALSE);

    // No. Check if we are on Mesa version 10.0.0 or later, as earlier Mesa versions
    // don't support DRI3/Present at all:
    const char* verstring = strstr((const char*) glGetString(GL_VERSION), "Mesa");
    if (!verstring || (sscanf(verstring, "Mesa %i.%i", &major, &minor) != 2) || (major < 10)) return(FALSE);

    // Ok, this is a DRI3 enabled driver, and we are on a DRI3/Present capable Mesa,
    // so in all likelyhood DRI3/Present will be used for rendering and presentation.
    // Is the X-Server recent enough? Servers older than 1.16.3 are seriously buggy
    // wrt. DRI3/Present, so a word of warning would be due:
    if (XVendorRelease(windowRecord->targetSpecific.deviceContext) < 11603000) {
        if (PsychPrefStateGet_Verbosity() > 1) {
            printf("\nPTB-WARNING: XServer version older than 1.16.3 with defective DRI3/Present implementation detected!\n");
            printf("PTB-WARNING: Stimulus presentation will not work correctly. Either upgrade your XServer, or disable\n");
            printf("PTB-WARNING: DRI3/Present. One way to disable DRI3 is to set the environment variable LIBGL_DRI3_DISABLE\n");
            printf("PTB-WARNING: to a non-zero value, then restart Octave or Matlab. Another way to disable DRI3 is via\n");
            printf("PTB-WARNING: some xorg.conf settings. Cfe. the man pages of your display driver via 'man intel',\n");
            printf("PTB-WARNING: 'man radeon' or 'man nouveau', depending on the graphics card you use.\n");
            printf("PTB-WARNING: Will continue, but expect hangs, sync failure or visually corrupted stimuli until this is fixed.\n\n");
        }
    }

    // DRI3/Present enabled and in use:
    windowRecord->specialflags |= kPsychIsDRI3Window;

    return(TRUE);
}

psych_bool PsychIsPresentSupported(PsychWindowRecordType *windowRecord)
{
    int major_version, minor_version;
    const xcb_query_extension_reply_t *present_extension;
    xcb_present_query_version_cookie_t cookie;
    xcb_present_query_version_reply_t *reply;
    xcb_connection_t *dpy = XGetXCBConnection(windowRecord->targetSpecific.privDpy);

    // Try to connect to Present extension:
    present_extension = xcb_get_extension_data(dpy, &xcb_present_id);
    if ((present_extension == NULL) || !present_extension->present) {
        return(FALSE);
    }

    // Check if minimum required version 1.1 is supported by server:
    cookie = xcb_present_query_version(dpy, XCB_PRESENT_MAJOR_VERSION, XCB_PRESENT_MINOR_VERSION);
    reply = xcb_present_query_version_reply(dpy, cookie, NULL);
    major_version = reply->major_version;
    minor_version = reply->minor_version;
    free(reply);

    if (!(major_version > 1 || minor_version >= 1)) {
        return(FALSE);
    }

    // Make sure composite extension is supported:
    if (!XCompositeQueryExtension(windowRecord->targetSpecific.privDpy, &major_version, &minor_version))
        return(FALSE);

    // Check for composite extension support of at least version 0.3:
    if (!XCompositeQueryVersion(windowRecord->targetSpecific.privDpy, &major_version, &minor_version) ||
        (major_version <= 0 && minor_version < 3))
        return(FALSE);

    // We are set here:
    return(TRUE);
}

psych_bool PsychOSEnablePresentEventReception(PsychWindowRecordType *windowRecord, int forCompositor, psych_bool enable)
{
    xcb_void_cookie_t cookie;
    xcb_connection_t *dpy;
    Window overlay, root_return, parent_return;
    Window *children_return = NULL;
    unsigned int nchildren_return = 0;

    // Skip enable/disable if already en/disabled as requested, return previous == current enable state:
    if ((enable && windowRecord->targetSpecific.present_notify_queue[forCompositor]) ||
        (!enable && !windowRecord->targetSpecific.present_notify_queue[forCompositor]))
        return(enable);

    dpy = XGetXCBConnection(windowRecord->targetSpecific.privDpy);

    PsychLockDisplay();

    if (forCompositor) {
        // Get Window handle of composite overlay window, and potentially of children of it:
        overlay = XCompositeGetOverlayWindow(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.xwindowHandle);
        if ((overlay != None) && XQueryTree(windowRecord->targetSpecific.privDpy, overlay, &root_return, &parent_return, &children_return, &nchildren_return) &&
            (nchildren_return > 0) && (children_return)) {
            // Does have child windows. Get last (== top-most in stacking order) one and assign
            // as our window of interest for peeking into the composition process timing:
            overlay = children_return[nchildren_return-1];

            // Free list of children:
            XFree(children_return);
        }

        // We only need the window handle, not direct use of the overlay itself, so immediately release our reference to it:
        if (overlay != None)
            XCompositeReleaseOverlayWindow(windowRecord->targetSpecific.privDpy, overlay);

        if (getenv("COMPOSITORFBWIN"))
            overlay = strtol(getenv("COMPOSITORFBWIN"), NULL, 16);

        if (overlay == None) {
            if (PsychPrefStateGet_Verbosity() > 0)
                printf("PTB-ERROR: Could not find X-Composite overlay window id for desktop compositor. Expect severe timing malfunctions!\n");

            PsychUnlockDisplay();
            return(FALSE);
        }
    }
    else {
        overlay = None;
    }

    // Need to switch:
    if (enable) {
        // Enable reception of pixmap present complete notify events for our windowRecord:
        if (!windowRecord->targetSpecific.present_notify_event_id[forCompositor])
            windowRecord->targetSpecific.present_notify_event_id[forCompositor] = xcb_generate_id(dpy);

        cookie = xcb_present_select_input_checked(dpy, windowRecord->targetSpecific.present_notify_event_id[forCompositor],
                                                  (forCompositor) ? overlay : windowRecord->targetSpecific.xwindowHandle,
                                                  (!forCompositor && windowRecord->targetSpecific.present_notify_queue[1]) ?
                                                  (XCB_PRESENT_EVENT_MASK_IDLE_NOTIFY | XCB_PRESENT_EVENT_MASK_COMPLETE_NOTIFY) : XCB_PRESENT_EVENT_MASK_COMPLETE_NOTIFY);
        xcb_discard_reply(dpy, cookie.sequence);

        windowRecord->targetSpecific.present_notify_queue[forCompositor] = xcb_register_for_special_xge(dpy, &xcb_present_id,
                                                                                                        windowRecord->targetSpecific.present_notify_event_id[forCompositor], NULL);

        if (PsychPrefStateGet_Verbosity() > 4)
            if (forCompositor)
                printf("PTB-INFO: Custom X11/Present compositor feedback flip completion timestamping for window %i enabled. Will start at next flip. Compositor target %p.\n",
                       windowRecord->windowIndex, overlay);
            else
                printf("PTB-INFO: Custom X11/Present feedback flip completion timestamping for window %i enabled. Will start at next flip.\n", windowRecord->windowIndex);
    }
    else {
        // Disable reception of all present notify events for our windowRecord:
        if (PsychPrefStateGet_Verbosity() > 4)
            printf("PTB-INFO: All custom X11/Present feedback delivery for window %i disabled.\n", windowRecord->windowIndex);

        for (forCompositor = 0; forCompositor < 2; forCompositor++) {
            if (windowRecord->targetSpecific.present_notify_queue[forCompositor]) {
                xcb_unregister_for_special_event(dpy, windowRecord->targetSpecific.present_notify_queue[forCompositor]);
                windowRecord->targetSpecific.present_notify_queue[forCompositor] = NULL;

                cookie = xcb_present_select_input_checked(dpy, windowRecord->targetSpecific.present_notify_event_id[forCompositor],
                                                        (forCompositor) ? overlay : windowRecord->targetSpecific.xwindowHandle,
                                                        XCB_PRESENT_EVENT_MASK_NO_EVENT);
                xcb_discard_reply(dpy, cookie.sequence);
            }
        }
    }

    PsychUnlockDisplay();

    // Return previous enable state:
    return(!enable);
}

// Ensure that video outputs are properly configured to reproduce at least min_bpc framebuffer output precision:
psych_bool PsychOSEnsureMinimumOutputPrecision(int screenNumber, int min_bpc)
{
    CGDirectDisplayID dpy;
    Window root;
    int scrnum;
    long max_bpc, min_maxbpc, new_max_bpc, set_max_bpc, actual_max_bpc = 0;
    int gpuMaintype = kPsychUnknown;
    int gpuMinortype = 0;
    int fNumDisplayHeads = 0;
    PsychGetCGDisplayIDFromScreenNumber(&dpy, screenNumber);
    Atom max_bpc_atom = XInternAtom(dpy, "max bpc", True);
    scrnum = PsychGetXScreenIdForScreen(screenNumber);
    root = RootWindow(dpy, scrnum);
    PsychGetGPUSpecs(screenNumber, &gpuMaintype, &gpuMinortype, NULL, &fNumDisplayHeads);

    PsychLockDisplay();

    // Framebuffer color resolution of more than standard 8 bpc requested?
    if (min_bpc > 8) {
        // Yes. Make sure all video outputs on this X-Screen are set up for maximum color output precision.
        XRRScreenResources *resources = XRRGetScreenResources(dpy, root);
        RROutput output;
        unsigned char *prop = NULL;
        unsigned long nitems = 0;
        unsigned long bytes_after;
        Atom actual_type;
        int actual_format;
        int i;

        // Currently we only upgrade 'max bpc' if it is too low for 10 bpc+ output
        // and not altered by user or 3rd party software, iow. at 8 bpc default.
        // We also only upgrade to 10 bpc, even if 12 bpc or more might look
        // appropriate for > 10 bpc output. Why? Mostly for three practical reasons:
        // 1. HDMI deep color capable displays are mandated by the HDMI spec that they
        //    must advertise 12 bpc support whenever they are deep color capable, even
        //    if they can't actually handle 12 bpc meaningfully! As most HDMI displays
        //    don't actually do 12 bpc, this means they will internally truncate to
        //    10 bpc. As the gpu won't dither 12+ bpc content down to 10 bpc if it
        //    thinks it is outputting to a true 12 bpc display, this means we get only
        //    10 bpc content! Otoh. if we restrict ourselves to 10 bpc output, then
        //    such HDMI fake 12 bpc displays can't fool us - we treat them like 10 bpc
        //    displays and the gpu dithers down to 10 bpc, achieving a net precision of
        //    more than 10 bpc for typical scenarios.
        //
        // 2. Almost all commercially available displays as of the year 2020 do not
        //    support more than 10 bpc anyway, and need dithering, so restricting to
        //    10 bpc does not do harm on 99% of all setups.
        //
        // 3. Users can always manually configure (xrandr --output name --set 'max bpc' 12)
        //    their display outputs to allow for 12 bpc output or higher. So the few lucky
        //    owners of true 12 bpc displays can take full advantage of such panels.
        //
        // We may need to periodically revisit this strategy, as future generation gpu's
        // and displays become available...
        switch (min_bpc) {
            case 10:
                min_maxbpc = 8;
                new_max_bpc = 10;
                break;

            case 11:
            case 12:
                min_maxbpc = 8;
                new_max_bpc = 10;
                break;

            case 16:
                min_maxbpc = 8;
                new_max_bpc = 10;
                break;

            default:
                printf("PTB-ERROR:PsychOSEnsureMinimumOutputPrecision: min_bpc %i requested, but this is unsupported!\n", min_bpc);
                min_maxbpc = 0;
                new_max_bpc = 0;
                break;
        }

        // 'max bpc' output / connector property supported in general?
        if (max_bpc_atom) {
            // Yes. Check all outputs on this screen:
            for (i = 0; i < resources->noutput; i++) {
                output = resources->outputs[i];
                max_bpc = 0;
                set_max_bpc = 0;

                // If the given output doesn't support 'max bpc' property, then skip it:
                if ((XRRGetOutputProperty(dpy, output, max_bpc_atom, 0, 4, False, False, None, &actual_type, &actual_format, &nitems, &bytes_after, &prop) != Success) || (prop == NULL))
                    continue;

                // Does it have the proper property, and is current 'max bpc' too low for our precision needs? Or is 16 bpc selected on a AMD gpu, which would be the default for AMD + eDP?
                if ((actual_type == XA_INTEGER) && (nitems == 1) && (actual_format == 32) && (((max_bpc = *((long *) prop)) <= min_maxbpc) || ((max_bpc == 16) && (gpuMaintype == kPsychRadeon) && (gpuMinortype >= 80)) )) {
                    // max_bpc too low (or at AMD default) for needed precision. Try to upgrade to new_max_bpc:
                    set_max_bpc = max_bpc;

                    // Output has 'max bpc' but it is too low for our 'min_bpc' precision needs. Crank it up to its maximum:
                    XRRPropertyInfo *info = XRRQueryOutputProperty(dpy, output, max_bpc_atom);

                    if (info && (info->range) && (info->num_values == 2)) {
                        set_max_bpc = (new_max_bpc <= info->values[1]) ? new_max_bpc : info->values[1];

                        if (PsychPrefStateGet_Verbosity() > 2)
                            printf("PTB-INFO: Output %i of screen %i has too low max bpc %i <= %i bpc for high precision (%i bpc) mode. Requesting a new maximum bpc of %i bits.\n", i, scrnum, max_bpc, min_maxbpc, min_bpc, (int) set_max_bpc);

                        XRRChangeOutputProperty(dpy, output, max_bpc_atom, XA_INTEGER, 32, PropModeReplace, (unsigned char *) &set_max_bpc, 1);
                    }

                    if (info)
                        XFree(info);
                }
                else {
                    set_max_bpc = max_bpc;

                    if (PsychPrefStateGet_Verbosity() > 3)
                        printf("PTB-INFO: Output %i of screen %i has sufficient max bpc %i > %i bpc for high precision (%i bpc) mode. Nothing to do.\n", i, scrnum, max_bpc, min_maxbpc, min_bpc);
                }

                if (set_max_bpc > actual_max_bpc)
                    actual_max_bpc = set_max_bpc;

                // Done with this one, free up, go to next one:
                XFree(prop);
            }

            XFlush(dpy);
        }

        XRRFreeScreenResources(resources);
    }

    PsychUnlockDisplay();

    // More than 10 bpc framebuffer content output precision needed,
    // and hardware output bpc limited to maximum 10 bpc on AMD DC with DCE8+ engine?
    if (max_bpc_atom && (min_bpc > 10) && (actual_max_bpc == 10) && (gpuMaintype == kPsychRadeon) && (gpuMinortype >= 80)) {
        // Yes. For displays output with <= actual_max_bpc 10 bpc, spatial dithering
        // is needed to make something out of this min_bpc > 10 bit content.
        //
        // The special case we need to handle here is an AMD gpu with DCE display
        // engine (DCE-8/10/11/12) feeding to a native 10 bpc video sink, where
        // dithering to 10 bpc would be needed, but the Linux amdgpu DC driver
        // does not do that for unknown reasons.
        //
        // Try to fix this by detecting if dithering is enabled, as it should be,
        // and will be for 6 bpc or 8 bpc display sinks. If dithering is disabled,
        // because a 10 bpc sink is connected or the > 10 bpc sink is artificially
        // restricted to 10 bpc -- iow. we'd need to dither to 10 bpc, but driver
        // did not enable this, then we manually override the drivers programming
        // and "hack on" 10 bpc spatial dithering:
        if (PsychPrefStateGet_Verbosity() > 2)
            printf("PTB-INFO: Screen %i needs dithering for high precision %i bpc output to <= %i bpc video sink.\n", scrnum, min_bpc, actual_max_bpc);

        // Call directly into our low-level dithering control routine. 0xffffffff
        // signals to the routine that it only should manually force-enable hw
        // dithering iff it isn't already enabled (as we assume should be for
        // 6 bpc and 8 bpc video output), ie. only for 10 bpc target signal depth.
        // The routine knows a proper magic value to set up at least DCE11.2 with
        // good quality:
        PsychOSKDSetDitherMode(screenNumber, 0xffffffff);
    }

    return(TRUE);
}

/*
 *    PsychOSOpenOnscreenWindow()
 *
 *    Creates the pixel format and the context objects and then instantiates the context onto the screen.
 *
 *    -The pixel format and the context are stored in the target specific field of the window recored.  Close
 *    should clean up by destroying both the pixel format and the context.
 *
 *    -We mantain the context because it must be be made the current context by drawing functions to draw into
 *    the specified window.
 *
 *    -We maintain the pixel format object because there seems to be now way to retrieve that from the context.
 *
 *    -To tell the caller to clean up PsychOSOpenOnscreenWindow returns FALSE if we fail to open the window. It
 *    would be better to just issue an PsychErrorExit() and have that clean up everything allocated outside of
 *    PsychOpenOnscreenWindow().
 */
psych_bool PsychOSOpenOnscreenWindow(PsychScreenSettingsType *screenSettings, PsychWindowRecordType *windowRecord, int numBuffers, int stereomode, int conserveVRAM)
{
    char windowTitle[32];
    PsychRectType screenrect;
    CGDirectDisplayID dpy;
    int scrnum;
    XSetWindowAttributes attr;
    unsigned long mask;
    Window root;
    Window win;
    GLXContext ctx;
    GLXFBConfig *fbconfig = NULL;
    GLXWindow glxwindow = (XID) 0;
    XVisualInfo *visinfo = NULL;
    int i, x, y, width, height, nrconfigs, buffdepth;
    GLenum glerr;
    int attrib[41];
    int attribcount=0;
    int stereoenableattrib=0;
    int depth, bpc;
    int windowLevel;
    int major, minor;
    psych_bool newstyle_setup = FALSE;
    int gpuMaintype = 0, gpuMinortype = 0;
    const char* mesaver = NULL;
    psych_bool mesamapi_strdupbug = FALSE;
    int saved_default_screen = 0;

    // Running under Wayland, ie. not with native XOrg X-Server, but deficient XWayland server?
    if (getenv("WAYLAND_DISPLAY") && strlen(getenv("WAYLAND_DISPLAY"))) {
        // That's a no-go at least as of May 2022 with XWayland version 1.22.1.1 aka 22.1.1,
        // at it has a severly broken OpenML implementation, which will lead to a hard hang
        // during startup tests.
        printf("\nPTB-ERROR: You are trying to run a Screen() implementation meant *only* for a native XOrg X-Server");
        printf("\nPTB-ERROR: under a XWayland fake X-Server, on top of a Wayland desktop GUI session. This is not");
        printf("\nPTB-ERROR: supported, as XWayland has various bugs which would make Psychtoolbox hang/malfunction!");
        printf("\nPTB-ERROR: ");
        printf("\nPTB-ERROR: Please resolve this issue now, by logging out of your desktop session and then logging in");
        printf("\nPTB-ERROR: again while choosing a classic X11/XOrg GUI, which uses a suitable native XOrg X-Server.");
        printf("\nPTB-ERROR: On Ubuntu Linux, there is usually a drop-down session chooser on your login screen (e.g.,");
        printf("\nPTB-ERROR: reachable by clicking onto a little gear icon in the bottom-right corner of the screen),");
        printf("\nPTB-ERROR: which offers options with (X11) or (Xorg) in its name, e.g., \"Plasma (X11)\" or");
        printf("\nPTB-ERROR: \"Ubuntu on Xorg\" or \"GNOME on Xorg\". These would be the right choices.");
        printf("\nPTB-ERROR: If you really know what you are doing and absolutely want to force running under XWayland");
        printf("\nPTB-ERROR: you could execute \"clear all; setenv('WAYLAND_DISPLAY'); Screen('Preference','ConserveVRAM', 2^19);\"");
        printf("\nPTB-ERROR: and then retry. This may malfunction or hang in various ugly ways, you have been warned!\n");
        return(FALSE);
    }

    // Include onscreen window index in title:
    sprintf(windowTitle, "PTB Onscreen Window [%i]:", windowRecord->windowIndex);

    // First opened onscreen window? If so, we try to map GPU MMIO registers
    // to enable beamposition based timestamping and other special goodies:
    if (x11_windowcount == 0) PsychScreenMapRadeonCntlMemory();

    PsychGetGPUSpecs(screenSettings->screenNumber, &gpuMaintype, &gpuMinortype, NULL, NULL);

    // Retrieve windowLevel, an indicator of where non-fullscreen windows should
    // be located wrt. to other windows. 0 = Behind everything else, occluded by
    // everything else. 1 - 999 = At layer windowLevel -> Occludes stuff on layers "below" it.
    // 1000 - 1999 = At highest level, but partially translucent / alpha channel allows to make
    // regions transparent. 2000 or higher: Above everything, fully opaque, occludes everything.
    // 2000 is the default.
    windowLevel = PsychPrefStateGet_WindowShieldingLevel();

    // Set windowing system backend type to zero, aka "classic" aka X11/GLX: Not strictly needed, as this is init-default.
    windowRecord->winsysType = 0;

    // Set OpenGL api type to classic desktop OpenGL: Not strictly needed, as this is init-default.
    windowRecord->glApiType = 0;

    // Init userspace GL context to safe default:
    windowRecord->targetSpecific.glusercontextObject = NULL;
    windowRecord->targetSpecific.glswapcontextObject = NULL;

    // Which display depth is requested?
    depth = PsychGetValueFromDepthStruct(0, &(screenSettings->depth));

    // Map the logical screen number to the corresponding X11 display connection handle
    // for the corresponding X-Server connection.
    PsychGetCGDisplayIDFromScreenNumber(&dpy, screenSettings->screenNumber);
    scrnum = PsychGetXScreenIdForScreen(screenSettings->screenNumber);

    // Default to use of one shared x-display connection "dpy" for all onscreen windows
    // on a given x-display and x-screen:
    windowRecord->targetSpecific.privDpy = dpy;

    // Override with per-window x-display connection if requested by environment variable:
    usePerWindowXConnections = (getenv("PTB_USEPERWINDOWXCONNECTIONS")) ? TRUE : FALSE;
    if (usePerWindowXConnections) {
        // Open a dedicated X-Display connection for this onscreen window. This is meant to
        // avoid parallel ops on multiple onscreen windows, e.g., async swaps via flip-threads,
        // from blocking on a single shared x-display connection.
        // The dedicated handle is a clone of the x-display handle/connection for
        // the parent-screen associated with this onscreen window:
        //
        // NOTICE: As of September 2011 and X-Server 1.9.x and 1.10.x, this doesn't work
        //         well at all! It is entirely useless in its current form, just left for
        //         documentation, and in case we find some better - and actually working -
        //         use case for per-window x-display connections in the future.
        //
        // Problems with the current approach:
        //
        // * If we create each onscreen window (== GLXDrawable) and associated OpenGL
        //   contexts on a separate x-display connection, the OpenGL contexts apparently
        //   can't share resources like texture objects, FBO's, PBO's, VBO's, display lists
        //   or shaders. This is an absolute no-go for PTB's rendering architecture!
        //
        // * If we use the master dpy connection for OpenGL ops and only dedicated connections
        //   for non-OpenGL GLX ops, e.g., the OML_sync_control functions, then those functions
        //   fail with BadContext errors and i couldn't find any way or hack around it.
        //
        // * Other than those cases, there isn't any real use for dedicated x-display connections
        //   at the moment.
        //
        // It is unclear if these problems are due to bugs in the X-Server / DRI2 infrastructure,
        // really weird and subtile bugs in our code, or if we simply tried doing something that
        // is unsupported and forbidden by design of the X-Window system / X11-Protocol.
        //
        // Anyway leave the setup code here, it is disabled by default anyway, so no harm done,
        // and maybe useful for testing if it is our bug, their bug or unsupported behaviour...
        PsychLockDisplay();
        windowRecord->targetSpecific.privDpy = XOpenDisplay(DisplayString(dpy));
        PsychUnlockDisplay();
        if (NULL == windowRecord->targetSpecific.privDpy) {
            // Failed! We are sooo done :-(
            printf("\nPTB-ERROR[XOpenDisplay() failed]: Couldn't get a dedicated x-display connection for this window to X-Server.\n\n");
            return(FALSE);
        }
    }

    PsychLockDisplay();

    // XFixes extension version 2.0 or later available and initialized?
    if (XFixesQueryExtension(dpy, &xfixes_event_base1, &xfixes_event_base2) &&
        XFixesQueryVersion(dpy, &major, &minor) && (major >= 2)) {
        xfixes_available = TRUE;
        xfixes_majorversion = major;
    }

    major = minor = 0;

    // XSync extension version 3.0 or later available and initialized?
    if (XSyncQueryExtension(dpy, &xsync_event_base1, &xsync_event_base2) &&
        XSyncInitialize(dpy, &major, &minor) && (major >= 3)) {
        xsync_available = TRUE;
    }

    major = minor = 0;

    // Init GLX extension, get its version, determine if at least V1.3 supported:
    useGLX13 = (glXQueryExtension(dpy, &glx_error_base, &glx_event_base) &&
    glXQueryVersion(dpy, &major, &minor) && ((major > 1) || ((major == 1) && (minor >= 3))));

    // Initialize GLX-1.3 protocol support. Use if possible:
    glXChooseFBConfig = (PFNGLXCHOOSEFBCONFIGPROC) glXGetProcAddressARB((const GLubyte *) "glXChooseFBConfig");
    glXGetFBConfigAttrib = (PFNGLXGETFBCONFIGATTRIBPROC) glXGetProcAddressARB((const GLubyte *) "glXGetFBConfigAttrib");
    glXGetVisualFromFBConfig = (PFNGLXGETVISUALFROMFBCONFIGPROC) glXGetProcAddressARB((const GLubyte *) "glXGetVisualFromFBConfig");
    glXCreateWindow = (PFNGLXCREATEWINDOWPROC) glXGetProcAddressARB((const GLubyte *) "glXCreateWindow");
    glXCreateNewContext = (PFNGLXCREATENEWCONTEXTPROC) glXGetProcAddressARB((const GLubyte *) "glXCreateNewContext");
    glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");
    glXDestroyWindow = (PFNGLXDESTROYWINDOWPROC) glXGetProcAddressARB((const GLubyte *) "glXDestroyWindow");
    glXSelectEvent = (PFNGLXSELECTEVENTPROC) glXGetProcAddressARB((const GLubyte *) "glXSelectEvent");
    glXGetSelectedEvent = (PFNGLXGETSELECTEDEVENTPROC) glXGetProcAddressARB((const GLubyte *) "glXGetSelectedEvent");

    PsychUnlockDisplay();

    // Check if everything we need from GLX-1.3 is supported:
    if (!useGLX13 || !glXChooseFBConfig || !glXGetVisualFromFBConfig || !glXCreateWindow || !glXCreateNewContext ||
        !glXDestroyWindow || !glXSelectEvent || !glXGetSelectedEvent || !glXGetFBConfigAttrib) {
        useGLX13 = FALSE;
        printf("PTB-INFO: Not using GLX-1.3 extension. Unsupported? Some features may be disabled.\n");
    } else {
        useGLX13 = TRUE;
    }

    // Check if this should be a fullscreen window, and if not, what its dimensions
    // should be:
    PsychGetScreenRect(screenSettings->screenNumber, screenrect);
    if (PsychMatchRect(screenrect, windowRecord->rect)) {
        // This is supposed to be a fullscreen window with the dimensions of
        // the current display/desktop:
        x=0;
        y=0;
        width=PsychGetWidthFromRect(screenrect);
        height=PsychGetHeightFromRect(screenrect);

        // Mark this window as fullscreen window:
        windowRecord->specialflags |= kPsychIsFullscreenWindow;

        // Copy absolute screen location and area of window to 'globalrect',
        // so functions like Screen('GlobalRect') can still query the real
        // bounding gox of a window onscreen:
        PsychGetGlobalScreenRect(screenSettings->screenNumber, windowRecord->globalrect);
    }
    else {
        // Window size different from current screen size:
        // A regular desktop window with borders and control icons is requested, e.g., for debugging:
        // Extract settings:
        x=windowRecord->rect[kPsychLeft];
        y=windowRecord->rect[kPsychTop];
        width=PsychGetWidthFromRect(windowRecord->rect);
        height=PsychGetHeightFromRect(windowRecord->rect);

        // Copy absolute screen location and area of window to 'globalrect',
        // so functions like Screen('GlobalRect') can still query the real
        // bounding gox of a window onscreen:
        PsychCopyRect(windowRecord->globalrect, windowRecord->rect);
    }

    // Select requested depth per color component 'bpc' for each channel:
    bpc = 8; // We default to 8 bpc == RGBA8
    if (windowRecord->depth == 30)  { bpc = 10; printf("PTB-INFO: Trying to enable at least 10 bpc fixed point framebuffer.\n"); }
    if (windowRecord->depth == 33)  { bpc = 11; printf("PTB-INFO: Trying to enable at least 11 bpc fixed point framebuffer.\n"); }
    if (windowRecord->depth == 48)  { bpc = 16; printf("PTB-INFO: Trying to enable at least 16 bpc fixed point framebuffer.\n"); }
    if (windowRecord->depth == 64)  { bpc = 16; printf("PTB-INFO: Trying to enable 16 bpc fixed point framebuffer.\n"); }
    if (windowRecord->depth == 128) { bpc = 32; printf("PTB-INFO: Trying to enable 32 bpc fixed point framebuffer.\n"); }

    // Setup pixelformat descriptor for selection of GLX visual:
    if (useGLX13) {
        attrib[attribcount++]= GLX_RENDER_TYPE; // Use RGBA true-color visual.
        attrib[attribcount++]= GLX_RGBA_BIT;    // Use RGBA true-color visual.
    } else {
        attrib[attribcount++]= GLX_RGBA;        // Use RGBA true-color visual.
    }

    attrib[attribcount++]= GLX_RED_SIZE;    // Setup requested minimum depth of each color channel:
    attrib[attribcount++]= (depth > 16) ? bpc : 1;
    attrib[attribcount++]= GLX_GREEN_SIZE;
    attrib[attribcount++]= (depth > 16) ? bpc : 1;
    attrib[attribcount++]= GLX_BLUE_SIZE;
    attrib[attribcount++]= (depth > 16) ? bpc : 1;
    attrib[attribcount++]= GLX_ALPHA_SIZE;
    // Alpha channel needs special treatment:
    if ((bpc != 10) && (bpc != 11)) {
        // Non 10/11 bpc drawable: Request a 'bpc' alpha channel if the underlying framebuffer
        // is in true-color mode ( >= 24 cpp format). If framebuffer is in 16 bpp mode, we
        // don't have/request an alpha channel at all:
        attrib[attribcount++]= (depth > 16) ? bpc : 0; // In 16 bit mode, we don't request an alpha-channel.
    }
    else if (bpc == 10) {
        // 10 bpc drawable: We have a 32 bpp pixel format with R10G10B10 10 bpc per color channel.
        // There are at most 2 bits left for the alpha channel, so we request an alpha channel with
        // minimum size 1 bit --> Will likely translate into a 2 bit alpha channel:
        attrib[attribcount++]= 1;
    }
    else {
        // 11 bpc drawable - or more likely a 32 bpp drawable with R11G11B10, ie., all 32 bpp
        // used up by RGB color info and no space for alpha bits. Therefore do not request an
        // alpha channel:
        attrib[attribcount++]= 0;
    }

    // Stereo display support: If stereo display output is requested with OpenGL native stereo,
    // we request a stereo-enabled rendering context.
    if (stereomode==kPsychOpenGLStereo) {
        attrib[attribcount++]= GLX_STEREO;
        stereoenableattrib = attribcount;
        attrib[attribcount++]= True;
    }

    // Multisampling support:
    if (windowRecord->multiSample > 0) {
        // Request a multisample buffer:
        attrib[attribcount++]= GLX_SAMPLE_BUFFERS_ARB;
        attrib[attribcount++]= 1;
        // Request at least multiSample samples per pixel:
        attrib[attribcount++]= GLX_SAMPLES_ARB;
        attrib[attribcount++]= windowRecord->multiSample;
    }

    // Support for OpenGL 3D rendering requested?
    if (PsychPrefStateGet_3DGfx()) {
        // Yes. Allocate and attach a 24 bit depth buffer and 8 bit stencil buffer:
        attrib[attribcount++]= GLX_DEPTH_SIZE;
        attrib[attribcount++]= 24;
        attrib[attribcount++]= GLX_STENCIL_SIZE;
        attrib[attribcount++]= 8;

        // Alloc an accumulation buffer as well?
        if (PsychPrefStateGet_3DGfx() & 2) {
            // Yes: Alloc accum buffer, request 64 bpp, aka 16 bits integer per color component if possible:
            attrib[attribcount++] = GLX_ACCUM_RED_SIZE;
            attrib[attribcount++] = 16;
            attrib[attribcount++] = GLX_ACCUM_GREEN_SIZE;
            attrib[attribcount++] = 16;
            attrib[attribcount++] = GLX_ACCUM_BLUE_SIZE;
            attrib[attribcount++] = 16;
            attrib[attribcount++] = GLX_ACCUM_ALPHA_SIZE;
            attrib[attribcount++] = 16;
        }
    }

    // Double buffering requested?
    if(numBuffers>=2) {
        // Enable double-buffering:
        attrib[attribcount++]= GLX_DOUBLEBUFFER;
        attrib[attribcount++]= True;

        // AUX buffers for Flip-Operations needed?
        if ((conserveVRAM & kPsychDisableAUXBuffers) == 0) {
            // Allocate one or two (mono vs. stereo) AUX buffers for new "don't clear" mode of Screen('Flip'):
            // Not clearing the framebuffer after "Flip" is implemented by storing a backup-copy of
            // the backbuffer to AUXs before flip and restoring the content from AUXs after flip.
            attrib[attribcount++]= GLX_AUX_BUFFERS;
            attrib[attribcount++]=(stereomode==kPsychOpenGLStereo || stereomode==kPsychCompressedTLBRStereo || stereomode==kPsychCompressedTRBLStereo) ? 2 : 1;
        }
    }

    // It's important that GLX_AUX_BUFFERS is the last entry in the attrib array, see code for glXChooseVisual below...

    // Finalize attric array:
    attrib[attribcount++]= None;

    PsychLockDisplay();

    root = RootWindow( dpy, scrnum );

    // Select matching visual for our pixelformat:
    if (useGLX13) {
        fbconfig = glXChooseFBConfig(dpy, scrnum, attrib, &nrconfigs);
    } else {
        visinfo = glXChooseVisual(dpy, scrnum, attrib );
    }

    if (!visinfo && !fbconfig && ((conserveVRAM & kPsychDisableAUXBuffers) == 0)) {
        // Failed to find matching visual: This can happen if we requested AUX buffers on a system
        // that doesn't support AUX-buffers. In that case we retry without requesting AUX buffers.
        // Most modern setups in the year 2022 do not support AUX buffers at all anymore, so this
        // is a very likely cause of failure, and the imaging pipeline provides much better solutions
        // for use cases historically covered by AUX buffers.

        // Terminate attrib array where the GLX_AUX_BUFFERS entry used to be...
        attrib[attribcount-3] = None;

        // Retry...
        if (useGLX13) {
            fbconfig = glXChooseFBConfig(dpy, scrnum, attrib, &nrconfigs);
        } else {
            visinfo = glXChooseVisual(dpy, scrnum, attrib );
        }
    }

    if (!visinfo && !fbconfig && (stereoenableattrib > 0)) {
        // Failed to find matching visual and OpenGL native quad-buffered frame-sequential
        // stereo requested. Probably the GPU does not support it. Disable it as we have a
        // fallback implementation for this case.
        attrib[stereoenableattrib] = False;

        // Retry:
        if (useGLX13) {
            fbconfig = glXChooseFBConfig(dpy, scrnum, attrib, &nrconfigs);
        } else {
            visinfo = glXChooseVisual(dpy, scrnum, attrib );
        }
    }

    if (!visinfo && !fbconfig) {
        // Failed to find matching visual: Could it be related to request for unsupported native 10/11/16 bpc framebuffer?
        if (((windowRecord->depth == 30) && (bpc == 10)) || ((windowRecord->depth == 33) && (bpc == 11)) || ((windowRecord->depth == 48) && (bpc == 16))) {
            // 10/11/16 bpc framebuffer requested: Let's see if we can get a visual by lowering our demand to 8 bpc:
            for (i=0; i<attribcount && attrib[i]!=GLX_RED_SIZE; i++);
            attrib[i+1] = 8;
            for (i=0; i<attribcount && attrib[i]!=GLX_GREEN_SIZE; i++);
            attrib[i+1] = 8;
            for (i=0; i<attribcount && attrib[i]!=GLX_BLUE_SIZE; i++);
            attrib[i+1] = 8;
            for (i=0; i<attribcount && attrib[i]!=GLX_ALPHA_SIZE; i++);
            attrib[i+1] = 1;

            // Retry:
            if (useGLX13) {
                fbconfig = glXChooseFBConfig(dpy, scrnum, attrib, &nrconfigs);
            } else {
                visinfo = glXChooseVisual(dpy, scrnum, attrib );
            }
        }
    }

    if (!visinfo && !fbconfig) {
        // Failed to find matching visual: Could it be related to multisampling?
        if (windowRecord->multiSample > 0) {
            // Multisampling requested: Let's see if we can get a visual by
            // lowering our demand:
            for (i=0; i<attribcount && attrib[i]!=GLX_SAMPLES_ARB; i++);
            while(!visinfo && !fbconfig && windowRecord->multiSample > 0) {
                attrib[i+1]--;
                windowRecord->multiSample--;

                if (useGLX13) {
                    fbconfig = glXChooseFBConfig(dpy, scrnum, attrib, &nrconfigs);
                } else {
                    visinfo = glXChooseVisual(dpy, scrnum, attrib );
                }
            }

            // Either we have a valid visual at this point or we still fail despite
            // requesting zero samples.
            if (!visinfo && !fbconfig) {
                // We still fail. Disable multisampling by requesting zero multisample buffers:
                for (i=0; i<attribcount && attrib[i]!=GLX_SAMPLE_BUFFERS_ARB; i++);
                windowRecord->multiSample = 0;
                attrib[i+1]=0;

                if (useGLX13) {
                    fbconfig = glXChooseFBConfig(dpy, scrnum, attrib, &nrconfigs);
                } else {
                    visinfo = glXChooseVisual(dpy, scrnum, attrib );
                }
            }
        }

        // Break out of this if we finally got one...
        if (!visinfo && !fbconfig) {
            // Failed to find matching visual:
            if (!visinfo && !fbconfig && PsychPrefStateGet_3DGfx()) {
                // Ok, retry with a 16 bit depth buffer...
                for (i=0; i<attribcount && attrib[i]!=GLX_DEPTH_SIZE; i++);
                if (attrib[i]==GLX_DEPTH_SIZE && i<attribcount) attrib[i+1]=16;
                printf("PTB-WARNING: Have to use 16 bit depth buffer instead of 24 bit buffer due to limitations of your gfx-hardware or driver. Accuracy of 3D-Gfx may be limited...\n");
                fflush(NULL);

                if (useGLX13) {
                    fbconfig = glXChooseFBConfig(dpy, scrnum, attrib, &nrconfigs);
                } else {
                    visinfo = glXChooseVisual(dpy, scrnum, attrib );
                }
                if (!visinfo && !fbconfig) {
                    // Failed again. Retry with disabled stencil buffer:
                    printf("PTB-WARNING: Have to disable stencil buffer due to limitations of your gfx-hardware or driver. Some 3D Gfx algorithms may fail...\n");
                    fflush(NULL);
                    for (i=0; i<attribcount && attrib[i]!=GLX_STENCIL_SIZE; i++);
                    if (attrib[i]==GLX_STENCIL_SIZE && i<attribcount) attrib[i+1]=0;
                    if (useGLX13) {
                        fbconfig = glXChooseFBConfig(dpy, scrnum, attrib, &nrconfigs);
                    } else {
                        visinfo = glXChooseVisual(dpy, scrnum, attrib );
                    }
                }
            }
        }
    }

    if (!visinfo && !fbconfig) {
        // Another possible problem: No alpha channel available, e.g., on the XMing X-Server for MS-Windows hosts:
        printf("PTB-WARNING: Have to disable alpha channel due to limitations of your gfx-hardware or driver. Some 3D Gfx algorithms may fail, Screen('Blendfunction') will be ineffective.\n");
        for (i = 0; i < attribcount && attrib[i] != GLX_ALPHA_SIZE; i++);
        attrib[i+1] = 0;

        // Retry:
        if (useGLX13) {
            fbconfig = glXChooseFBConfig(dpy, scrnum, attrib, &nrconfigs);
        } else {
            visinfo = glXChooseVisual(dpy, scrnum, attrib );
        }
    }

    PsychUnlockDisplay();

    if (!visinfo && !fbconfig) {
        printf("\nPTB-ERROR[glXChooseVisual() failed]: Couldn't get any suitable visual from X-Server.\n\n");
        return(FALSE);
    }

    PsychLockDisplay();

    if (fbconfig && (windowLevel >=1000 && windowLevel < 2000)) {
        // Transparent window requested and fbconfig's found. Iterate over them
        // and try to find one with 32 bit color depths:
        for (i = 0; i < nrconfigs; i++) {
            buffdepth = 0;
            if ((Success == glXGetFBConfigAttrib(dpy, fbconfig[i], GLX_BUFFER_SIZE, &buffdepth)) && (buffdepth >= 32) &&
                (visinfo = glXGetVisualFromFBConfig(dpy, fbconfig[i])) && (visinfo->depth >= 30)) {
                fbconfig[0] = fbconfig[i];
                if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Choosing GLX framebuffer config %i for transparent window.\n", i);
                break;
            }
            else if (PsychPrefStateGet_Verbosity() > 4) {
                printf("PTB-INFO: Trying GLX framebuffer config %i for transparent window: Depths %i bpp.\n", i, buffdepth);
            }
        }
    }

    // If this setup is fbconfig based, get associated visual:
    if (fbconfig) visinfo = glXGetVisualFromFBConfig(dpy, fbconfig[0]);

    // Setup basic window attributes:
    attr.background_pixel = 0;  // Background color defaults to black.
    attr.border_pixel = 0;      // Border color as well.
    attr.colormap = XCreateColormap(dpy, root, visinfo->visual, AllocNone);  // Dummy colormap assignment.
    attr.event_mask = KeyPressMask | StructureNotifyMask;                    // We're only interested in keypress events for GetChar() and StructureNotify to wait for Windows to be mapped.

    // Mask of everything we define(d):
    mask = CWOverrideRedirect | CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

    // Old style of override_redirect handling requested? This was used until beginning 2013
    // and worked well for us, but it prevents the windowmanager from seeing properties on
    // our windows which allow us to control desktop composition, e.g., on KDE/KWIN and GNOME-3/Mutter,
    // as well as on other wm's compliant with latest ICCCM spec:
    // Ok, for now we only use the new-style path if we are running under KDE/KWin and user
    // doesn't explicitely override/forbid that choice. Otherwise we use the old path, as
    // that seems to perform better, at least on tested Unity/compiz, GNOME3-Shell and LXDE/OpenBox.
    //
    // UPDATE June-2014: Do not even use new-style on KDE, unless forced by setenv("PSYCH_NEW_OVERRIDEREDIRECT", "1")
    // Turns out the new-style override redirect doesn't play well with KDE multi-display setups. It causes KDE
    // to cut off all parts of the fullscreen window except for the first video output, making this unworkable on
    // anything but single display setups. We may rework this code at some later point, but for now just disable.
    //
    // UPDATE September-2014: Unless we are using KDE on an Intel gpu, where god-knows-why we need new-style
    // override redirect handling, because KDE doesn't recognize our fullscreen windows as such and wouldn't
    // unredirect them without the new override redirect setup and signalling. Strangely other desktop environments
    // do have no problem detecting our fullscreen windows on an Intel gpu, e.g., Unity, GNOME-3/GNOME-2, ...
    //
    // UPDATE November-2014: KDE doesn't unredirect fullscreen windows by default on Intel gpus deliberately,
    // to work around bugs in some versions of the Intel-ddx, that's why the Intel trouble. So we must use
    // new-style for KDE to unredirect fullscreen windows. However, this doesn't work for multi-display
    // spanning windows due to what i think is a KWin bug. Therefore we enforce old-style setup again if there
    // are at least two video outputs connected to our target x-screen. This means the user has to manually
    // disable desktop composition if he needs dual/multi-display stimulation on a Intel gpu under KDE/KWin,
    // as the manual "user says so" override is the only method that worked. (KWin window rules were proven
    // ineffective as well):
    //
    // UPDATE April-2015: Use newstyle_setup if user wants it, or if this is a KDE single display setup,
    // where it helps. On KDE multi-display we can't use it due to the KWin problems mentioned above, on
    // other desktop environments we don't need it. This is like before, just we also use this on KDE +
    // non-Intel gpu's, to save the user the extra setup step for "unredirect_fullscreen_windows" in the KDE
    // GUI, as this is a bit more convenient.
    //
    // UPDATE June-2015: Use old style setup also on KDE with multiple X-Screens, not only multiple outputs
    // on one X-Screen. Otherwise at least the future KDE 5.3 Plasma desktop will do stupid things.
    if (!getenv("PSYCH_NEW_OVERRIDEREDIRECT") &&
        ((PsychPrefStateGet_ConserveVRAM() & kPsychOldStyleOverrideRedirect) ||
        !getenv("KDE_FULL_SESSION") || (PsychScreenToHead(screenSettings->screenNumber, 1) >= 0) ||
        (PsychGetNumDisplays() > 1))) {
        // Old style: Always override_redirect to lock out window manager, except when a real "GUI-Window"
        // is requested, which needs to behave and be treated like any other desktop app window:
        attr.override_redirect = (windowRecord->specialflags & kPsychGUIWindow) ? 0 : 1;
    }
    else {
        // New style: override_redirect by default:
        newstyle_setup = TRUE;

        attr.override_redirect = 1;

        // Don't override if it is a GUI window, for some reasons as in classic path:
        if (windowRecord->specialflags & kPsychGUIWindow) attr.override_redirect = 0;

        // Don't override if it is a fullscreen window. The NETM_FULLSCREEN state should
        // take care of fullscreen windows nicely without need to override. Although we
        // could override for transparent fullscreen windows if we were extra paranoid,
        // we *must not* override for opaque (standard) fullscreen windows, because that
        // would prevent us from disabling desktop composition on our window by use of
        // special properties -- the window manager wouldn't notice those properties/requests
        // on a override_redirected window.
        // Now here's a catch: While the WM picks up our window properties this way, and at
        // least KWIN honors them by disabling composition, the WM also takes this opportunity
        // to "misplace" our onscreen window on multi-display setups. It follows various rules on
        // where a fullscreen window should be placed on a multi-display setup, but while these
        // rules are pretty sensible for regular desktop users, they are absolutely not what we
        // want. How to solve? We let the WM have its way during window creation, so it picks up
        // our window props. Later on (see below) after the Window is mapped and the WM satisified,
        // we set the override_redirect flag to lock out the WM, then move the window to its proper
        // location, no that the WM can't interfere anymore.
        if (windowRecord->specialflags & kPsychIsFullscreenWindow) attr.override_redirect = 0;
    }

    if (PsychPrefStateGet_Verbosity() > 3)
        printf("PTB-INFO: Using %s-style override-redirect (=%i) setup path for onscreen window creation.\n", (newstyle_setup) ? "new" : "old", attr.override_redirect);

    // Create our onscreen window:
    win = XCreateWindow( dpy, root, x, y, width, height,
                            0, visinfo->depth, InputOutput,
                            visinfo->visual, mask, &attr );

    if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: GLX Visual info depths is %i bits\n", visinfo->depth);

    // Set hints and properties:
    {
        XSizeHints sizehints;
        sizehints.x = x;
        sizehints.y = y;
        sizehints.width  = width;
        sizehints.height = height;
        // Let window manager control window position if kPsychGUIWindowWMPositioned is set:
        sizehints.flags = USSize | (windowRecord->specialflags & kPsychGUIWindowWMPositioned) ? 0 : USPosition;
        XSetNormalHints(dpy, win, &sizehints);
        XSetStandardProperties(dpy, win, windowTitle, "PTB Onscreen window",
                                None, (char **)NULL, 0, &sizehints);
    }

    // Setup window transparency for user input (keyboard and mouse events):
    if (xfixes_available && (windowLevel >= 1000) && (windowLevel < 1500)) {
        // Define region as an empty input region:
        XserverRegion region = XFixesCreateRegion(dpy, NULL, 0);

        // Assign as region in which window receives input events, thereby
        // setting the input region to empty, so the window is transparent
        // to any input events like key presses or mouse clicks:
        XFixesSetWindowShapeRegion(dpy, win, ShapeInput, 0, 0, region);

        // Destroy region after assignment:
        XFixesDestroyRegion(dpy, region);
    }

    // Create corresponding glx window:
    if (fbconfig) {
        glxwindow = glXCreateWindow(dpy, fbconfig[0], win, NULL);
    }

    PsychUnlockDisplay();

    // Make sure a potential slaveWindow of us resides on the same X-Screen == has same screenNumber as us,
    // otherwise trying to perform OpenGL context resource sharing would end badly:
    if ((windowRecord->slaveWindow) && (windowRecord->slaveWindow->screenNumber != screenSettings->screenNumber)) {
        // Ohoh! Let's abort with some more helpful error message than a simple hard application crash:
        printf("\nPTB-ERROR:[glXCreateContext() resource sharing] Our peer window resides on a different X-Screen, which is forbidden. Aborting.\n\n");
        return(FALSE);
    }

    PsychLockDisplay();

    // Create associated GLX OpenGL rendering context: We use ressource
    // sharing of textures, display lists, FBO's and shaders if 'slaveWindow'
    // is assigned for that purpose as master-window. We request a direct
    // rendering context (True) if possible:
    if (fbconfig) {
        // Workaround for bug introduced into Mesa 12.0, potentially backported to some 11.2 releases
        // by commit cf804b4455fac9e585b3600a8318caaced9c23de. glXCreateNewContext() does not perform
        // some validation of fbconfig correctly against the X-Screen stored in the fbconfig for our
        // target X-Screen, but instead always validates against X-Screen 0 -- the DefaultScreen(dpy).
        // This ends badly whenever we are not trying to create a context on X-Screen 0. Work around
        // this by temporarily assigning dpy->default_screen as our target screen 'scrnum' while
        // glXCreateNewContext is called. Note: A bug fix for this bug was submitted upstream to resolve
        // this properly, but that doesn't help us if we need to run on affected Mesa 12.0.x releases,
        // as shipping with Ubuntu 16.10 and presumably future Ubuntu 16.04.2-LTS:
        saved_default_screen = ((_XPrivDisplay) dpy)->default_screen;
        ((_XPrivDisplay) dpy)->default_screen = scrnum;

        ctx = glXCreateNewContext(dpy, fbconfig[0], GLX_RGBA_TYPE, ((windowRecord->slaveWindow) ? windowRecord->slaveWindow->targetSpecific.contextObject : NULL), True);
    } else {
        ctx = glXCreateContext(dpy, visinfo, ((windowRecord->slaveWindow) ? windowRecord->slaveWindow->targetSpecific.contextObject : NULL), True );
    }

    PsychUnlockDisplay();

    if (!ctx) {
        printf("\nPTB-ERROR:[glXCreateContext() failed] OpenGL context creation failed!\n\n");
        return(FALSE);
    }

    // Store the handles:

    // windowHandle is a GLXWindow. Fallback path assigns a Window. Both are typedef'd
    // to XID, so this cast and storage is safe to do:
    windowRecord->targetSpecific.windowHandle = (fbconfig) ? glxwindow : (GLXWindow) win;

    // xwindowHandle stores the underlying X-Window:
    windowRecord->targetSpecific.xwindowHandle = win;

    windowRecord->targetSpecific.deviceContext = dpy;
    windowRecord->targetSpecific.contextObject = ctx;

    // Store chosen GLXFBConfig and visualid for interop with, e.g., OpenXR:
    windowRecord->targetSpecific.pixelFormatObject = (fbconfig) ? fbconfig[0] : NULL;
    if (fbconfig)
        glXGetFBConfigAttrib(dpy, fbconfig[0], GLX_VISUAL_ID, (int*) &windowRecord->targetSpecific.visualId);

    // Set flags to mark this window as a classic X11/GLX window:
    windowRecord->specialflags |= (kPsychIsX11Window | kPsychIsGLXWindow);

    PsychLockDisplay();

    // Create rendering context for async flips with identical visual and display as main context, share all heavyweight ressources with it:
    if (fbconfig) {
        windowRecord->targetSpecific.glswapcontextObject = glXCreateNewContext(dpy, fbconfig[0], GLX_RGBA_TYPE, windowRecord->targetSpecific.contextObject, True);
    } else {
        windowRecord->targetSpecific.glswapcontextObject = glXCreateContext(dpy, visinfo, windowRecord->targetSpecific.contextObject, True);
    }

    PsychUnlockDisplay();

    if (windowRecord->targetSpecific.glswapcontextObject == NULL) {
        printf("\nPTB-ERROR[SwapContextCreation failed]: Creating a private OpenGL context for async flips failed for unknown reasons.\n\n");
        return(FALSE);
    }

    // External 3D graphics support enabled?
    if (PsychPrefStateGet_3DGfx()) {
        // Yes. We need to create an extra OpenGL rendering context for the external
        // OpenGL code to provide optimal state-isolation. The context shares all
        // heavyweight ressources likes textures, FBOs, VBOs, PBOs, display lists and
        // starts off as an identical copy of PTB's context as of here.
        PsychLockDisplay();

        // Create rendering context with identical visual and display as main context, share all heavyweight ressources with it:
        if (fbconfig) {
            // Does usercode want to use OpenGL 3.1+ core profile?
            if ((PsychPrefStateGet_3DGfx() & 4) && glXCreateContextAttribsARB) {
                // Yes: Request GL 3.1. This will give us 3.1 with or without GL_compatibility or not,
                // or any later core profile if the driver + hw supports it:
                int ctx_attrib_list[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 1, None };
                windowRecord->targetSpecific.glusercontextObject = glXCreateContextAttribsARB(dpy, fbconfig[0], windowRecord->targetSpecific.contextObject, True, ctx_attrib_list);
            }
            else
                windowRecord->targetSpecific.glusercontextObject = glXCreateNewContext(dpy, fbconfig[0], GLX_RGBA_TYPE, windowRecord->targetSpecific.contextObject, True);
        } else {
            windowRecord->targetSpecific.glusercontextObject = glXCreateContext(dpy, visinfo, windowRecord->targetSpecific.contextObject, True);
        }

        PsychUnlockDisplay();

        if (windowRecord->targetSpecific.glusercontextObject == NULL) {
            printf("\nPTB-ERROR[UserContextCreation failed]: Creating a private OpenGL context for Matlab OpenGL failed for unknown reasons.\n\n");
            return(FALSE);
        }
    }

    // Release visual info:
    XFree(visinfo);

    // Release fbconfig array, if any:
    if (fbconfig) {
        // Restore true default screen for dpy:
        ((_XPrivDisplay) dpy)->default_screen = saved_default_screen;
        XFree(fbconfig);
    }

    PsychLockDisplay();

    // Setup window transparency:
    if ((windowLevel >= 1000) && (windowLevel < 2000)) {
        // For windowLevels between 1000 and 1999, make the window background transparent, so standard GUI
        // would be visible, wherever nothing is drawn, i.e., where alpha channel is zero:

        // Levels 1000 - 1499 and 1500 to 1999 map to a master opacity level of 0.0 - 1.0:
        unsigned int opacity = (unsigned int) (0xffffffff * (((float) (windowLevel % 500)) / 499.0));

        // Get handle on opacity property of X11:
        Atom atom_window_opacity = XInternAtom(dpy, "_NET_WM_WINDOW_OPACITY", False);

        // Assign new value for property:
        XChangeProperty(dpy, win, atom_window_opacity, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &opacity, 1);
    }

    // Is this a non-transparent (fully opaque), non-GUI, fullscreen onscreen window?
    if (!((windowLevel >= 1000) && (windowLevel < 2000)) && !(windowRecord->specialflags & kPsychGUIWindow) &&
        (windowRecord->specialflags & kPsychIsFullscreenWindow)) {
        // Yes. This is a standard stimulus presentation window which should get best
        // timing precision and performance for stimulus presentation. We don't want
        // any desktop composition to interfere with it, so it is eligible for direct
        // page-flipping (unredirected). If we are running under a modern X11 desktop
        // compositor then we can explicitely ask it to disable compositing while our
        // onscreen window is open, by setting a special NETWM property on the window.
        // This approach has just become a NETWM standard that works with KWin/KDE-5
        // and Mutter/GNOME-3.
        //
        // On legacy compositors, e.g., compiz / unity et al. this problem is solved by
        // asking them to unredirect_fullscreen_windows, as done by PsychGPUControl.m during
        // installation of PTB.
        //
        // Btw. for other properties that KDE supports/understands see function create_netwm_atoms()
        // in file netwm.cpp, e.g., at http://code.woboq.org/kde/kdelibs/kdeui/windowmanagement/netwm.cpp.html
        //

        // Set the standardized NETWM property. This is supported in Mutter (== GNOME-3 and Ubuntu desktop)
        // since 18. December 2012 (see last comment/patch in https://bugzilla.gnome.org/show_bug.cgi?id=683020),
        // and supported by KDE KWin since KDE frameworks version 5.20 (committed since 29.12.2015), cfe.
        // https://invent.kde.org/frameworks/kwindowsystem/-/commit/2a5b7943a020f2154049d28a33216149b6220d53
        unsigned int dontcomposite = 1;
        Atom atom_window_dontcomposite = XInternAtom(dpy, "_NET_WM_BYPASS_COMPOSITOR", False);

        // Assign new value for property:
        XChangeProperty(dpy, win, atom_window_dontcomposite, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &dontcomposite, 1);
    }

    // Is this a non-GUI fullscreen window? If so, set the fullscreen NETWM property:
    if (!(windowRecord->specialflags & kPsychGUIWindow) && (windowRecord->specialflags & kPsychIsFullscreenWindow)) {
        // Yes. Set the fullscreen state hint. Any well behaved window manager should understand this as
        // a request to turn the window into a completely decorationless fullscreen window, something
        // similar to what you'd get with override_redirect = 1, just in a less dirty way, that still
        // allows us to communicate our wishes, e.g., wrt. desktop composition, to the window manager.
        Atom stateFullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
        XChangeProperty(dpy, win, XInternAtom(dpy, "_NET_WM_STATE", False), XA_ATOM, 32, PropModeReplace, (unsigned char *) &stateFullscreen, 1);
    }

    // For some reason we need to use unsigned long and long here instead of
    // int32_t etc., despite the fact that on a 64-Bit build, a long is 64-Bit
    // and on a 32-Bit build, a long is 32-Bit, whereas the XChangeProperty()
    // request says a single unit is 32-Bits? Anyway, it works correctly on a
    // 64-Bit build, so this seems to be magically ok.
    struct MwmHints {
        unsigned long flags;
        unsigned long functions;
        unsigned long decorations;
        long          input_mode;
        unsigned long status;
    };

    enum {
        MWM_HINTS_FUNCTIONS = (1L << 0),
        MWM_HINTS_DECORATIONS =  (1L << 1),

        MWM_FUNC_ALL = (1L << 0),
        MWM_FUNC_RESIZE = (1L << 1),
        MWM_FUNC_MOVE = (1L << 2),
        MWM_FUNC_MINIMIZE = (1L << 3),
        MWM_FUNC_MAXIMIZE = (1L << 4),
        MWM_FUNC_CLOSE = (1L << 5)
    };

    Atom mwmHintsProperty = XInternAtom(dpy, "_MOTIF_WM_HINTS", False);

    struct MwmHints hints;
    memset(&hints, 0, sizeof(hints));
    hints.flags = MWM_HINTS_DECORATIONS | MWM_HINTS_FUNCTIONS;

    // If this window is a GUI window then enable all window decorations and
    // manipulations, except for the window close button, which would wreak havoc:
    if (windowRecord->specialflags & kPsychGUIWindow) {
        hints.decorations = MWM_FUNC_ALL;
        hints.functions   = MWM_FUNC_RESIZE | MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE | MWM_FUNC_MAXIMIZE;
    }
    else {
        // No GUI window: Prevent the user from interacting with / manipulating the window by defining an empty list of allowed actions:
        XChangeProperty(dpy, win, XInternAtom(dpy, "_NET_WM_ALLOWED_ACTIONS", False), XA_ATOM, 32, PropModeReplace, (unsigned char *) NULL, 0);

        // No decorations:
        hints.decorations = 0;

        // No interaction via any controls (buttons etc.):
        hints.functions   = 0;
    }

    // Apply hints:
    XChangeProperty(dpy, win, mwmHintsProperty, mwmHintsProperty, 32, PropModeReplace, (unsigned char *) &hints, sizeof(hints) / sizeof(long));

    // For windowLevels of at least 500, tell window manager to try to keep
    // our window above most other windows, by setting the state to WM_STATE_ABOVE:
    // Note: Can't use it for level 2000. On level 2000 it will be automatically above others, so this
    // isn't needed, but setting it would *undo* some required settings for fullscreen opaque windows.
    if ((windowLevel >= 500) && (windowLevel < 2000)) {
        Atom stateAbove = XInternAtom(dpy, "_NET_WM_STATE_ABOVE", False);
        XChangeProperty(dpy, win, XInternAtom(dpy, "_NET_WM_STATE", False), XA_ATOM, 32, PropModeReplace, (unsigned char *) &stateAbove, 1);
    }

    // Show our new window: Also raise it to the top for
    // non-zero window levels:
    if (windowLevel > 0) {
        XMapRaised(dpy, win);
    } else if (windowLevel != -1) {
        XMapWindow(dpy, win);
    }

    PsychUnlockDisplay();

    // Spin-Wait for it to be really mapped:
    while (windowLevel != -1) {
        XEvent ev;
        PsychLockDisplay();
        XNextEvent(dpy, &ev);
        PsychUnlockDisplay();
        if (ev.type == MapNotify)
            break;
    }

    PsychLockDisplay();

    // If windowLevel is zero, lower it to the bottom of the stack of windows:
    if (windowLevel == 0) XLowerWindow(dpy, win);

    // Setup window transparency for user input (keyboard and mouse events):
    if ((windowLevel < 1500) && (windowLevel >= 1000)) {
        // Need to try to be transparent for keyboard events and mouse clicks:
        XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
    }

    // Is this a non-GUI fullscreen window? In the new-style path?
    if (!(windowRecord->specialflags & kPsychGUIWindow) && (windowRecord->specialflags & kPsychIsFullscreenWindow) && newstyle_setup) {
        // Yes. As we didn't override_redirect it during creation and mapping, so the
        // WM could pick up our special window properties, the WM very likely misplaced
        // it on the screen at a very ergonomic location for desktop users which doesn't
        // suit our needs. Let's fix this:

        // First we override_redirect it to lock out the WM from further manipulations:
        attr.override_redirect = 1;
        XChangeWindowAttributes(dpy, win, mask, &attr);

        // Wait for override to complete...
        XSync(dpy, False);

        // Then we move it to its proper location, now hopefully untampered by the WM:
        XMoveWindow(dpy, win, x, y);

        // Make sure it reaches its target position:
        XSync(dpy, False);
    }

    PsychUnlockDisplay();

    // Ok, the onscreen window is ready on the screen. Time for OpenGL setup...

    // Activate the associated rendering context:
    PsychOSSetGLContext(windowRecord);

    PsychLockDisplay();

    // Try to detect if we are running on top of Mesa OpenGL, and which version:
    mesaver = strstr((const char*) glGetString(GL_VERSION), "Mesa");
    if (mesaver && (3 == sscanf(mesaver, "Mesa %i.%i.%i", &mesaversion[0], &mesaversion[1], &mesaversion[2]))) {
        if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Running on Mesa version %i.%i.%i\n", mesaversion[0], mesaversion[1], mesaversion[2]);
    }
    else {
        mesaversion[0] = mesaversion[1] = mesaversion[2] = 0;
        if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Not running on Mesa graphics library.\n");
    }

    if (mesaversion[0] > 0) {
        // Running on top of Mesa. Does it have the Mesa mapi bug?
        // Versions older than 10.5.2 have this bug, later versions have a proper bug fix:
        if ((mesaversion[0] < 10) || ((mesaversion[0] == 10) && ((mesaversion[1] < 5) || ((mesaversion[1] == 5) && (mesaversion[2] < 2))))) {
            mesamapi_strdupbug = TRUE;
            if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Mesa version < 10.5.2 does not have the mapi strdup() bug fix. Needs the mex file locking workaround.\n");
        }
        else {
            // Mesa 10.5.2 or later. This means it has sane DRI3/Present support. Let's check if the X-Server
            // has sane DRI3/Present support as well, ie., is it at least XOrg 1.16.3?
            if (XVendorRelease(windowRecord->targetSpecific.deviceContext) >= 11603000) {
                // Yes. This combo is considered safe for use with DRI3/Present, so mark
                // our windowRecord as safe for this mode of display:
                windowRecord->specialflags |= kPsychSafeForDRI3;
                if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: This combo of X-Server and Mesa is considered safe for use under DRI3/Present.\n");
            }
        }
    }

    // Ok, the OpenGL rendering context is up and running.
    // Running on top of a FOSS Mesa graphics driver?
    if ((x11_windowcount == 0) && mesamapi_strdupbug && !getenv("PSYCH_DONT_LOCK_MOGLCORE")) {
        // Yes. At least as of Mesa 10.1 as shipped in Ubuntu 14.04-LTS, Mesa
        // will become seriously crashy if our Screen() mex files is flushed
        // from memory due to a clear all/mex/Screen and afterwards reloaded.
        // This because Mesa maintains pointers back into our library image,
        // which will turn into dangling pointers if we get unloaded/reloaded
        // into a new location. To prevent Mesa crashes on clear Screen -> reload,
        // prevent this mex file against clearing from Octave/Matlab address space.
        // An ugly solution which renders "clear Screen" useless, but the best i can
        // come up with at the moment :(
        if (PsychRuntimeEvaluateString("moglcore('LockModule');") > 0) {
            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: Failed to enable moglcore locking workaround for Mesa OpenGL bug!\n");
                printf("PTB-WARNING: Calling 'clear all', 'clear mex', 'clear java', 'clear moglcore', 'clear Screen' is now unsafe and may crash if you try.\n");
                printf("PTB-WARNING: Please upgrade to a Linux distribution with a Mesa library version of at least 10.5.2 to fix this.\n");
            }
            setenv("PSYCH_DONT_LOCK_MOGLCORE", "1", 0);
        }
        else {
            if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Workaround: Disabled ability to 'clear moglcore', as a workaround for a Mesa OpenGL bug. Sorry for the inconvenience.\n");
        }
    }

    // Auto-detect and bind all available OpenGL extensions via GLEW:
    glerr = glewInit();
    if (GLEW_OK != glerr) {
        /* Problem: glewInit failed, something is seriously wrong. */
        printf("\nPTB-ERROR[GLEW init failed: %s]: Please report this to the forum. Will try to continue, but may crash soon!\n\n", glewGetErrorString(glerr));
    }
    else {
        if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Using GLEW version %s for automatic detection of OpenGL extensions...\n", glewGetString(GLEW_VERSION));
    }

    // Check for DRI3/Present operation and assign proper special flag to windowRecord if so:
    if (IsDRI3Supported(windowRecord) && PsychIsPresentSupported(windowRecord)) {
        if (PsychPrefStateGet_Verbosity() > 3)
            printf("PTB-INFO: Window uses DRI3/Present for OpenGL visual stimulus rendering and presentation.\n");
    }
    else if (PsychIsPresentSupported(windowRecord)) {
        // DRI3/Present enabled and in use, at least for display in a muxless PRIME setup:
        windowRecord->specialflags |= kPsychIsDRI3Window;

        if (PsychPrefStateGet_Verbosity() > 3)
            printf("PTB-INFO: Window uses DRI3/Present for Optimus/Prime renderoffload visual stimulus presentation on display gpu.\n");
    }

    // Increase our own open window counter:
    x11_windowcount++;

    // Disable X-Windows screensavers:
    if (x11_windowcount==1) {
        int dummy;

        // First window. Disable future use of screensaver:
        XGetScreenSaver(dpy, &oldscreensaver[0], &oldscreensaver[1], &oldscreensaver[2], &oldscreensaver[3]);
        XSetScreenSaver(dpy, 0, 0, DefaultBlanking, DefaultExposures);
        // If the screensaver is currently running, forcefully shut it down:
        XForceScreenSaver(dpy, ScreenSaverReset);

        // And just for safety, do it via DPMS disable as well:
        if (DPMSQueryExtension(dpy, &dummy, &dummy)) DPMSDisable(dpy);
    }

    // Check for availability of VSYNC and swap control extensions:
    if (!glxewIsSupported("GLX_EXT_swap_control") && !glxewIsSupported("GLX_MESA_swap_control") && !glxewIsSupported("GLX_SGI_swap_control")) {
        // No swap control extension at all!
        if (PsychPrefStateGet_Verbosity() > 1) {
            printf("PTB-WARNING: Your graphics driver doesn't allow me to control syncing wrt. vertical retrace!\n");
            printf("PTB-WARNING: Please update your display graphics driver as soon as possible to fix this.\n");
            printf("PTB-WARNING: Until then, you can manually enable syncing to VBL somehow in a manner that is\n");
            printf("PTB-WARNING: dependent on the type of gfx-card and driver. Google is your friend...\n");
        }
    }
    else if (!glxewIsSupported("GLX_EXT_swap_control") && !glxewIsSupported("GLX_MESA_swap_control")) {
        // None of the good swap control extensions.
        // As GLX_MESA_swap_control is supported by Mesa since around 2003, but afaik not by proprietary drivers, we can
        // only hit this with a proprietary driver (NVidia, AMD) that is very ancient ie. before 2013:
        if (PsychPrefStateGet_Verbosity() > 2) {
            printf("PTB-INFO: Your graphics driver would not allow me to disable syncing wrt. vertical retrace if needed.\n");
            printf("PTB-INFO: Your proprietary graphics driver must be pretty old. Consider upgrading if you need\n");
            printf("PTB-INFO: vsync disable ability, e.g., for efficient display mirroring, good timing in windowed mode, etc.\n");
            printf("PTB-INFO: NVidia proprietary drivers generally support this since at least the year 2011, ATI since at least 2013.\n");
        }
    }

    PsychUnlockDisplay();

    // Ok, we should be ready for OS independent setup...
    fflush(NULL);

    // Wait for X-Server to settle...
    PsychLockDisplay();
    XSync(dpy, 1);
    PsychUnlockDisplay();

    // Wait 250 msecs extra to give desktop compositor a chance to settle:
    PsychYieldIntervalSeconds(0.25);

    PsychLockDisplay();

    // Retrieve modeline of current video mode on primary crtc for the screen to which
    // this onscreen window is assigned. Could also query useful info about crtc, but let's not
    // overdo it in the first iteration...
    XRRCrtcInfo *crtc_info = NULL;
    XRRModeInfo *mode = PsychOSGetModeLine(screenSettings->screenNumber, 0, &crtc_info);
    if (mode) {
        // Assign modes display height aka vactive or vdisplay as startline of vblank interval:
        windowRecord->VBL_Startline = mode->height;
        // Assign vbl endline as vtotal - 1:
        windowRecord->VBL_Endline   = mode->vTotal - 1;

        // Check for output display rotation enabled. Will likely impair timing/timestamping
        // because it uses copy-swaps via an intermediate shadow framebuffer to do rotation
        // during copy-swap blit, instead of via rotated crtc scanout, as most crtc's don't
        // support this in hardware:
        if ((crtc_info->rotation != RR_Rotate_0) && (PsychPrefStateGet_Verbosity() > 1)) {
            printf("PTB-WARNING: Your primary output display has hardware rotation enabled. It is not displaying in upright orientation.\n");
            printf("PTB-WARNING: On many graphics cards, this will cause unreliable stimulus presentation timing and timestamping.\n");
            printf("PTB-WARNING: If you want non-upright stimulus presentation, look at 'help PsychImaging' on how to achieve this in\n");
            printf("PTB-WARNING: a way that doesn't impair timing. The subfunctions 'FlipHorizontal' and 'FlipVertical' are what you probably need.\n");
        }

        XRRFreeCrtcInfo(crtc_info);
    }

    // VRR handling for all Linux X11/GLX drivers, ie. currently Mesa/FOSS/Linux VRR and G-Sync:
    switch (windowRecord->vrrMode) {
        case kPsychVRROff: // Disable VRR:
            windowRecord->vrrMode = kPsychVRROff;
            break;

        case kPsychVRRAuto: // Automatic selection of optimal supported method for this setup. Currently our own custom scheduler, unless Vulkan/WSI is used:
            windowRecord->vrrMode = (windowRecord->specialflags & kPsychExternalDisplayMethod) ? kPsychVRRSimple : kPsychVRROwnScheduled;
            break;

        case kPsychVRRSimple: // Classic / Legacy / Dumb VRR - Just swapbuffers when asked to:
            windowRecord->vrrMode = kPsychVRRSimple;
            break;

        case kPsychVRROwnScheduled: // Our own custom async scheduler:
            windowRecord->vrrMode = kPsychVRROwnScheduled;
            break;

        default:
            windowRecord->vrrMode = kPsychVRROff;
            if (PsychPrefStateGet_Verbosity() > 1)
                printf("PTB-WARNING: Unsupported VRR mode %i requested. Disabling VRR.\n", windowRecord->vrrMode);
    }

    // Use vrrMinDuration "as is". Either userspace provided, or reasonable default of
    // video refresh duration of current video mode, which defines the lowest duration
    // currently doable on the current VRR hardware implementations:
    windowRecord->vrrMinDuration = windowRecord->vrrMinDuration;

    // vrrMaxDuration: If provided by userspace, use it "as is". If not provided, our
    // best guess is 33.3333 msecs corresponding to a minimum refresh rate of 30 Hz,
    // which seems to be what NVidia G-Sync displays and better FreeSync displays can do:
    windowRecord->vrrMaxDuration = windowRecord->vrrMaxDuration ? windowRecord->vrrMaxDuration : 1.0 / 30.0;

    // Running on the Mesa OpenGL library? Then we can probe for and setup VRR supported by the
    // FOSS driver stack:
    if (mesaversion[0] > 0) {
        // Check if video output is vrr_capable == true. This would mean the Linux kernel + DRM/KMS
        // driver combo supports VRR and runs on a GPU that supports VRR and we are displaying on a
        // display that is VRR capable and enabled:
        RROutput output;
        const char *output_name = NULL;
        Atom vrr_supported_atom = XInternAtom(dpy, "vrr_capable", True);
        unsigned char *prop = NULL;
        unsigned long nitems = 0;
        unsigned long bytes_after;
        Atom actual_type;
        int actual_format;
        int major, minor;
        psych_bool vrr_supported = FALSE;
        psych_bool vrr_wanted = (windowRecord->vrrMode > kPsychVRROff) ? TRUE : FALSE;

        // Find primary output for check in 'output':
        PsychUnlockDisplay();
        if (vrr_wanted)
            output_name = PsychOSGetOutputProps(screenSettings->screenNumber, 0, FALSE, NULL, NULL, (unsigned long *) &output);
        PsychLockDisplay();

        if (vrr_supported_atom && vrr_wanted &&
            (XRRGetOutputProperty(dpy, output, vrr_supported_atom, 0, 4, False, False, None, &actual_type, &actual_format, &nitems, &bytes_after, &prop) == Success) &&
            (actual_type == XA_INTEGER) && (nitems == 1) && (actual_format == 32)) {
            // printf("%s : %p %ld %d \n", output_name, prop, *((long *) prop), *((long *) prop) > 0);

            vrr_supported = (*((long *) prop) > 0) ? TRUE : FALSE;
        }

        if (prop)
            XFree(prop);

        PsychUnlockDisplay();

        PsychOSGetLinuxVersion(&major, &minor, NULL);

        if (vrr_wanted && (!(windowRecord->specialflags & kPsychIsFullscreenWindow) || (PsychPrefStateGet_WindowShieldingLevel() < 2000))) {
            vrr_wanted = FALSE;
            vrr_supported = FALSE;

            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: Can not enable Variable Refresh Rate mode for this onscreen window, because\n");
                printf("PTB-WARNING: the window is not an opaque unoccluded top-level fullscreen window. Only use\n");
                printf("PTB-WARNING: such unoccluded opaque fullscreen windows for VRR mode. -> 'help VRRSupport'\n");
            }
        }

        if (vrr_wanted && !(major > 5 || (major == 5 && minor >= 2))) {
            // Game over:
            vrr_wanted = FALSE;
            vrr_supported = FALSE;

            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: Can not enable Variable Refresh Rate mode for this fullscreen window on this display.\n");
                printf("PTB-WARNING: Your Linux kernel is too old. You need at least Linux 5.2 for research-grade support\n");
                printf("PTB-WARNING: on suitable AMD gpus, possibly more modern kernels for other suitable gpus.\n");
                printf("PTB-WARNING: The more modern kernel, the better. -> 'help VRRSupport'\n");
            }
        }

        // VRR wanted, but not supported by hardware or Linux kernel?
        if (vrr_wanted && !vrr_supported) {
            // Game over:
            vrr_wanted = FALSE;

            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: Can not enable Variable Refresh Rate mode for this fullscreen window on this display [%s].\n", output_name);
                if (nitems != 1) {
                    // vrr_capable property missing - no kms driver support:
                    printf("PTB-WARNING: Your Linux kernel driver does not support Variable Refresh Rate on your graphics card.\n");
                    if (gpuMaintype == kPsychRadeon) {
                        // AMD gpu:
                        printf("PTB-WARNING: You need an AMD gpu of at least the Sea Islands gpu family (Graphics Core Next 2nd Gen)\n");
                        printf("PTB-WARNING: controlled by the amdgpu-kms driver in Linux 5.2 or later, with the AMD display core\n");
                        printf("PTB-WARNING: DC enabled. This is the case for Volcanic Islands gpus (Graphics Core Next 3rd Gen) and later,\n");
                        printf("PTB-WARNING: but some Sea Islands gpus need you to add the following kernel boot parameters:\n");
                        printf("PTB-WARNING: radeon.cik_support=0 amdgpu.cik_support=1 amdgpu.dc=1.\n");
                    }
                    else if (gpuMaintype == kPsychIntelIGP) {
                        // Intel gpu:
                        printf("PTB-WARNING: You need an Intel gpu of at least Generation 12 (TigerLake+ / Intel Xe / DG-1 / Alchemist+)\n");
                        printf("PTB-WARNING: controlled by Linux 5.12 or later, with a DisplayPort adaptive-sync / FreeSync capable display.\n");
                    }
                    else {
                        printf("PTB-WARNING: Upgrade to a sufficiently modern Linux kernel for your graphics card.\n");
                    }
                }
                else {
                    printf("PTB-WARNING: Either the display or the video cable does not support it. You need a FreeSync or DisplayPort\n");
                    printf("PTB-WARNING: adaptive sync capable display, connected via Displayport, eDP, or maybe HDMI2.1 VRR.\n");
                }
                printf("PTB-WARNING: -> 'help VRRSupport'\n");
            }
        }

        if (vrr_wanted && vrr_supported) {
            // Display + video cable + GPU + Linux DRM/KMS + libdrm is VRR capable and enabled.
            // Ditto for the X-Server and probably the X-Video DDX driver and Mesa.
            //
            // Mark window as VRR capable with native Linux FOSS VRR support:
            windowRecord->gfxcaps |= kPsychGfxCapLinuxVRR;
        }

        // Yes. Perform VRR enable or disable sequence.
        unsigned int state;
        Atom vrr_atom;

        // Running on Mesa 19.0.0 or later with DRI3/Present support for VRR?
        if (mesaversion[0] >= 19) {
            // Yes. Perform an OpenGL bufferswap, so Mesa can set up VRR state according to its opinion and
            // system settings and then we can override it with our own opinion:
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            PsychOSFlipWindowBuffers(windowRecord);
            PsychOSGetPostSwapSBC(windowRecord);
        }

        // X11 window properties are set not for en-/disable of VRR, but we do want to let user-script
        // decide if VRR should be on or off, so override accordingly. Use of VRR requested by user script?
        if (vrr_wanted && vrr_supported) {
            // Yes: Set proper X11 window atom to request VRR mode from the driver:
            state = 1;

            if (PsychPrefStateGet_Verbosity() > 3)
                printf("PTB-INFO: Enabling VRR Variable Refresh Rate mode for this fullscreen window on Mesa graphics driver.\n");
        }
        else {
            // No: Clear proper X11 window atom to disable VRR mode in the driver:
            state = 0;

            if (PsychPrefStateGet_Verbosity() > 3)
                printf("PTB-INFO: Disabling VRR Variable Refresh Rate mode for this fullscreen window on Mesa graphics driver.\n");
        }

        PsychLockDisplay();
        vrr_atom = XInternAtom(dpy, "_VARIABLE_REFRESH", False);
        XChangeProperty(dpy, win, vrr_atom, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &state, 1);
    }

    PsychUnlockDisplay();

    // Make sure RandR outputs are properly configured for native video output at
    // at least 'bpc' bits per color channel:
    PsychOSEnsureMinimumOutputPrecision(screenSettings->screenNumber, bpc);

    // Try to enable swap event delivery to us:
    if (PsychOSSwapCompletionLogging(windowRecord, 2, 0) && (PsychPrefStateGet_Verbosity() > 3)) {
        printf("PTB-INFO: INTEL_swap_event support for additional swap completion correctness checks enabled.\n");
    }

    // Well Done!
    return(TRUE);
}

psych_bool PsychOSX11ClientCompositorSyncEnabled(PsychWindowRecordType *windowRecord)
{
    return(windowRecord->targetSpecific.syncCounter[1] ? TRUE : FALSE);
}

psych_bool PsychOSEnableX11ClientCompositorSync(PsychWindowRecordType *windowRecord)
{
    psych_bool netwmtiming_supported = FALSE;

    // Skip redundant enable:
    if (windowRecord->targetSpecific.syncCounter[1])
        return(TRUE);

    // Windowed window or composited window? And XSYNC extension for XSYNC counters available? And timestamping mode 4 selected?
    if (xsync_available && (PsychPrefStateGet_VBLTimestampingMode() == 4) && getenv("PSYCH_EXPERIMENTAL_NETWMTS")) {
        // Yes, this has to go through the X11 desktop compositor. Probe for extended client-compositor sync support by compositor.
        // Note: As of early 2022, the only known X11 compositor that supports this is GNOME's Mutter, therefore GNOME and Ubuntu desktop GUI.
        Display *dpy = windowRecord->targetSpecific.privDpy;
        Window win = windowRecord->targetSpecific.xwindowHandle;
        Window root = RootWindow(dpy, PsychGetXScreenIdForScreen(windowRecord->screenNumber));
        unsigned char *prop = NULL;
        unsigned long offset = 0;
        unsigned long nitems = 0;
        unsigned long bytes_after;
        Atom actual_type;
        int actual_format;

        PsychLockDisplay();

        Atom WM_PROTOCOLS_atom = XInternAtom(dpy, "WM_PROTOCOLS", True);
        Atom _NET_WM_SYNC_REQUEST_atom = XInternAtom(dpy, "_NET_WM_SYNC_REQUEST", True);
        Atom _NET_SUPPORTED_atom = XInternAtom(dpy, "_NET_SUPPORTED", True);
        Atom _NET_WM_SYNC_REQUEST_COUNTER_atom = XInternAtom(dpy, "_NET_WM_SYNC_REQUEST_COUNTER", True);
        _NET_WM_FRAME_DRAWN_atom = XInternAtom(dpy, "_NET_WM_FRAME_DRAWN", True);
        _NET_WM_FRAME_TIMINGS_atom = XInternAtom(dpy, "_NET_WM_FRAME_TIMINGS", True);

        // Does the compositor support extended compositor-client synchronization? Query root window property _NET_SUPPORTED
        // for the _NET_WM_FRAME_DRAWN atom:
        while ((Success == XGetWindowProperty(dpy, root, _NET_SUPPORTED_atom, offset, 1, False, AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_after, &prop)) &&
               (actual_type != None) && (nitems == 1)) {
            // Got something:
            if ((actual_type == XA_ATOM) && (actual_format == 32) && (*((Atom *) prop) == _NET_WM_FRAME_DRAWN_atom))
                netwmtiming_supported = TRUE;

            if (prop)
                XFree(prop);

            if (bytes_after == 0 || netwmtiming_supported)
                break;

            offset++;
        }

        if (PsychPrefStateGet_Verbosity() > 3)
            printf("PTB-INFO: WM SUPPORTS _NET_WM_FRAME_DRAWN: %i\n", netwmtiming_supported);

        if (netwmtiming_supported) {
            // Supported! Opt-in:
            XSyncValue syncvalue;

            // Compositor supports extended sync. Announce our opt-in to the protocol:
            if (PsychPrefStateGet_Verbosity() > 2) {
                printf("PTB-INFO: X11-Compositor supports extended client-compositor sync for better timing. Enabling *highly experimental, only lightly tested, limited* support.\n");
                printf("PTB-INFO: Things that will malfunction or hang: Use of multiple simultaneous windows, async flips, frame sequential stereo, VRR, flip events.\n");
                printf("PTB-INFO: Correct presentation timing and timestamping is somewhat, but not stringently, verified.\n");
            }

            // Create two X-Sync counters for client-compositor sync, init to counter value 0:
            XSyncIntToValue(&syncvalue, 0);
            windowRecord->targetSpecific.syncCounter[0] = XSyncCreateCounter(dpy, syncvalue);
            windowRecord->targetSpecific.syncCounter[1] = XSyncCreateCounter(dpy, syncvalue);
            netwmtiming_supported &= windowRecord->targetSpecific.syncCounter[0] && windowRecord->targetSpecific.syncCounter[1];
            if (netwmtiming_supported) {
                // Attach their id's to the _NET_WM_SYNC_REQUEST_COUNTER array to opt-in to extended sync:
                netwmtiming_supported &= XChangeProperty(dpy, win, _NET_WM_SYNC_REQUEST_COUNTER_atom, XA_CARDINAL, 32, PropModeAppend,
                                                        (unsigned char *) &windowRecord->targetSpecific.syncCounter[0], 1);
                netwmtiming_supported &= XChangeProperty(dpy, win, _NET_WM_SYNC_REQUEST_COUNTER_atom, XA_CARDINAL, 32, PropModeAppend,
                                                         (unsigned char *) &windowRecord->targetSpecific.syncCounter[1], 1);

                // Signal our opt-in to general sync by adding _NET_WM_SYNC_REQUEST to WM_PROTOCOLS:
                if (netwmtiming_supported)
                    XChangeProperty(dpy, win, WM_PROTOCOLS_atom, XA_ATOM, 32, PropModeAppend, (unsigned char *) &_NET_WM_SYNC_REQUEST_atom, 1);
            }

            if (!netwmtiming_supported && (PsychPrefStateGet_Verbosity() > 0))
                printf("PTB-ERROR: Failed to enable X11-Compositor extended client-compositor sync. Timing and timestamping will be degraded!\n");
        }

        PsychUnlockDisplay();

        // Disable vsync for this window, ie. for immediate backbuffer -> redirection surface "swaps":
        if (netwmtiming_supported)
            PsychOSSetVBLSyncLevel(windowRecord, 0);
    }

    return (netwmtiming_supported);
}

/*
 *    PsychOSGetPostSwapSBC() -- Internal method for now, used in close window path.
 *    Caution: Blocks with display lock held! Do not use outside close window path!!!
 */
static psych_int64 PsychOSGetPostSwapSBC(PsychWindowRecordType *windowRecord)
{
    psych_int64 ust, msc, sbc;
    sbc = 0;

    #ifdef GLX_OML_sync_control
    // Extension unsupported or known to be defective? Return "damage neutral" 0 in that case:
    if ((NULL == glXWaitForSbcOML) || (windowRecord->specialflags & kPsychOpenMLDefective)) return(0);

    // Extension supported: Perform query and error check.
    PsychLockDisplay();
    if (!glXWaitForSbcOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, 0, &ust, &msc, &sbc)) {
        // Failed! Return a "damage neutral" result:
        sbc = 0;
    }
    PsychUnlockDisplay();
    #endif

    return(sbc);
}

void PsychOSCloseWindow(PsychWindowRecordType *windowRecord)
{
    Display* dpy = windowRecord->targetSpecific.deviceContext;

    // Disable our own X11/Present event reception if it is on. Otherwise this no-ops:
    PsychOSEnablePresentEventReception(windowRecord, 0, FALSE);

    // We have to rebind the OpenGL context for this swapbuffers call to work around some
    // mesa bug for intel drivers which would cause a crash without context:
    PsychLockDisplay();
    glXMakeCurrent(dpy, windowRecord->targetSpecific.windowHandle, windowRecord->targetSpecific.contextObject);
    PsychUnlockDisplay();

    // Perform a fully synced flip with backbuffer cleared to black, to have a defined final
    // frontbuffer color for switching back to windowing system. Avoids leaving pixel trash
    // behind on some multi-x-screen setups with some drivers:
    PsychSetupView(windowRecord, TRUE);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    PsychOSFlipWindowBuffers(windowRecord);

    // The following swap to even flip count sequence is not really needed anymore for
    // the recent X-Servers we support. Also it can cause hangs when releasing leased
    // RandR outputs in a multi-display fullscreen Vulkan setup with more than 1 leased
    // display at a time -- a hang in PsychOSGetPostSwapSBC(), so lets just skip it in
    // such cases:
    if (!(windowRecord->specialflags & kPsychExternalDisplayMethod)) {
        PsychOSGetPostSwapSBC(windowRecord);

        // Check if we are trying to close the window after it had an "odd" (== non-even)
        // number of bufferswaps. If so, we execute one last bufferswap to make the count
        // even. This means that if this window was swapped via page-flipping, the system
        // should end with the same backbuffer-frontbuffer assignment as the one prior
        // to opening the window. This may help sidestep certain bugs in compositing desktop
        // managers (e.g., Compiz).
        if (PsychOSGetPostSwapSBC(windowRecord) % 2) {
            // Uneven count. Submit a swapbuffers request and wait for it to truly finish:

            // A glClear to touch the framebuffer before flip. Why? To accomodate some quirks of
            // the Intel ddx as of 2.99.917 with DRI2+SNA and triple-buffering enabled. Makes
            // triple-buffered mode at least marginally useful for some restricted use cases:
            glClear(GL_COLOR_BUFFER_BIT);

            PsychOSFlipWindowBuffers(windowRecord);
            PsychOSGetPostSwapSBC(windowRecord);
        }

        if (PsychPrefStateGet_Verbosity() > 5) {
            printf("PTB-DEBUG:PsychOSCloseWindow: Closing with a final swapbuffers count of %i.\n", (int) PsychOSGetPostSwapSBC(windowRecord));
        }
    }

    PsychLockDisplay();

    // Detach OpenGL rendering context again - just to be safe!
    if (strcmp(glGetString(GL_VENDOR), "ATI Technologies Inc."))
        glXMakeCurrent(dpy, None, NULL);

    // Delete rendering context:
    glXDestroyContext(dpy, windowRecord->targetSpecific.contextObject);
    windowRecord->targetSpecific.contextObject=NULL;

    // Delete swap context:
    glXDestroyContext(dpy, windowRecord->targetSpecific.glswapcontextObject);
    windowRecord->targetSpecific.glswapcontextObject=NULL;

    // Delete userspace context, if any:
    if (windowRecord->targetSpecific.glusercontextObject) {
        glXDestroyContext(dpy, windowRecord->targetSpecific.glusercontextObject);
        windowRecord->targetSpecific.glusercontextObject = NULL;
    }

    // Wait for X-Server to settle...
    XSync(dpy, 0);

    if (useGLX13) glXDestroyWindow(dpy, windowRecord->targetSpecific.windowHandle);
    windowRecord->targetSpecific.windowHandle = 0;

    // Release all active X11 pointer barriers for this window, drop lock temporarily,
    // as it is aquired by PsychOSConstrainPointer() itself:
    PsychUnlockDisplay();
    PsychOSConstrainPointer(windowRecord, FALSE, NULL);
    PsychLockDisplay();

    // Close & Destroy the window:
    XUnmapWindow(dpy, windowRecord->targetSpecific.xwindowHandle);

    // Wait for X-Server to settle...
    XSync(dpy, 0);

    // Release sync counters, if any:
    if (windowRecord->targetSpecific.syncCounter[0]) {
        XSyncDestroyCounter(dpy, windowRecord->targetSpecific.syncCounter[0]);
        XSyncDestroyCounter(dpy, windowRecord->targetSpecific.syncCounter[1]);
        windowRecord->targetSpecific.syncCounter[0] = 0;
        windowRecord->targetSpecific.syncCounter[1] = 0;
    }

    XDestroyWindow(dpy, windowRecord->targetSpecific.xwindowHandle);
    windowRecord->targetSpecific.xwindowHandle=0;

    // Wait for X-Server to settle...
    XSync(dpy, 0);

    // Release device context: We just release the reference. The connection to the display is
    // closed below.
    windowRecord->targetSpecific.deviceContext=NULL;

    // Decrement global count of open onscreen windows:
    x11_windowcount--;

    // Was this the last window?
    if (x11_windowcount <= 0) {
        int dummy;

        x11_windowcount = 0;

        // (Re-)enable X-Windows screensavers if they were enabled before opening windows:
        // Set screensaver to previous settings, potentially enabling it:
        XSetScreenSaver(dpy, oldscreensaver[0], oldscreensaver[1], oldscreensaver[2], oldscreensaver[3]);

        // And just for safety, do it via DPMS enable as well:
        if (DPMSQueryExtension(dpy, &dummy, &dummy)) DPMSEnable(dpy);

        XSync(dpy, 0);

        // Unmap/release possibly mapped device memory: Defined in PsychScreenGlue.c
        PsychScreenUnmapDeviceMemory();
    }

    // Release dedicated x-display connection for the dead window:
    if (usePerWindowXConnections) {
        XCloseDisplay(windowRecord->targetSpecific.privDpy);
    }

    PsychUnlockDisplay();

    // Shut down the PRIME UDP socket if any was in use:
    if (prime_sockfd[PsychGetXScreenIdForScreen(windowRecord->screenNumber)] > 0)
        close(prime_sockfd[PsychGetXScreenIdForScreen(windowRecord->screenNumber)]);
    prime_sockfd[PsychGetXScreenIdForScreen(windowRecord->screenNumber)] = 0;

    if (prime_sockfd2[PsychGetXScreenIdForScreen(windowRecord->screenNumber)] > 0)
        close(prime_sockfd2[PsychGetXScreenIdForScreen(windowRecord->screenNumber)]);
    prime_sockfd2[PsychGetXScreenIdForScreen(windowRecord->screenNumber)] = 0;

    // Done.
    return;
}

/*
 *    PsychOSGetVBLTimeAndCount()
 *
 *    Returns absolute system time of last VBL and current total count of VBL interrupts since
 *    startup of gfx-system for the given screen. Returns a time of -1 and a count of 0 if this
 *    feature is unavailable on the given OS/Hardware configuration.
 */
double PsychOSGetVBLTimeAndCount(PsychWindowRecordType *windowRecord, psych_uint64* vblCount)
{
    psych_uint64 ust, msc, sbc;
    double tActiveStart = -1;
    unsigned int vsync_counter = 0;
    PsychLockDisplay();

    #ifdef GLX_OML_sync_control
    // Ok, this will return VBL count and last VBL time via the OML GetSyncValuesOML call
    // if that extension is supported on this setup. The Linux FOSS graphics stack (DRI2/DRI3)
    // supports this on all gpu's since at least the year 2010:
    if ((NULL != glXGetSyncValuesOML) && !(windowRecord->specialflags & kPsychOpenMLDefective) && (glXGetSyncValuesOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, (int64_t*) &ust, (int64_t*) &msc, (int64_t*) &sbc))) {
        PsychUnlockDisplay();
        *vblCount = msc;
        if ((PsychGetKernelTimebaseFrequencyHz() > 10000) && !(windowRecord->specialflags & kPsychNeedOpenMLWorkaround1)) {
            // Convert ust into regular GetSecs timestamp:
            // At least we hope this conversion is correct...
            return( PsychOSMonotonicToRefTime(((double) ust) / PsychGetKernelTimebaseFrequencyHz()) );
        }
        else {
            // Last VBL timestamp unavailable, fallback on beamposition method:
            return(PsychGetVblankTimestamps(windowRecord, NULL));
        }
    }
    #else
    #warning GLX_OML_sync_control unsupported! Compile with -std=gnu99 to enable it!
    #endif

    // Retrieve absolute system time of end of current/last vblank, or -1 on failure/unsupported:
    tActiveStart = PsychGetVblankTimestamps(windowRecord, NULL);

    // Do we have SGI video sync extensions?
    if (NULL != glXGetVideoSyncSGI) {
        // Retrieve absolute count of vblanks since startup:
        glXGetVideoSyncSGI(&vsync_counter);
        PsychUnlockDisplay();
        *vblCount = (psych_uint64) vsync_counter;

        return(tActiveStart);
    }
    else {
        // Counter query unsupported:
        PsychUnlockDisplay();
        *vblCount = 0;

        return(tActiveStart);
    }
}

static void ParseX11PresentCompleteMode(PsychWindowRecordType *windowRecord, xcb_present_complete_notify_event_t *completed)
{
    // Map complete mode equivalent to INTEL_swap_event:
    switch (completed->mode) {
        case XCB_PRESENT_COMPLETE_MODE_FLIP:
            windowRecord->swapcompletiontype = 1;
            break;

        case XCB_PRESENT_COMPLETE_MODE_SUBOPTIMAL_COPY:
        case XCB_PRESENT_COMPLETE_MODE_COPY:
            windowRecord->swapcompletiontype = 3;
            break;

        case XCB_PRESENT_COMPLETE_MODE_SKIP:
            windowRecord->swapcompletiontype = 4;
            break;

        default:
            windowRecord->swapcompletiontype = 0;
            break;
    }
}

/* PsychOSGetSwapCompletionTimestamp()
 *
 * Retrieve a very precise timestamp of doublebuffer swap completion by means
 * of OS specific facilities. This function is optional. If the underlying
 * OS/driver/GPU combo doesn't support a high-precision, high-reliability method
 * to query such timestamps, the function should return -1 as a signal that it
 * is unsupported or (temporarily) unavailable. Higher level timestamping code
 * should use/prefer timestamps returned by this function over other timestamps
 * provided by other mechanisms if possible. Calling code must be prepared to
 * use alternate timestamping methods if this method fails or returns a -1
 * unsupported error. Calling code must expect this function to block until
 * swap completion.
 *
 * Input argument targetSBC: Swapbuffers count for which to wait for. A value
 * of zero means to block until all pending bufferswaps for windowRecord have
 * completed, then return the timestamp of the most recently completed swap.
 *
 * A value of zero is recommended.
 *
 * Returns: Precise and reliable swap completion timestamp in seconds of
 * system time in variable referenced by 'tSwap', and msc value of completed swap,
 * or a negative value on error (-1 == unsupported, -2/-3 == Query failed).
 *
 */
psych_int64 PsychOSGetSwapCompletionTimestamp(PsychWindowRecordType *windowRecord, psych_int64 targetSBC, double* tSwap)
{
    psych_int64 ust, msc, sbc;
    msc = -1;

    #ifdef GLX_OML_sync_control

    // Swap already completed via our SoftSync implementation?
    if (windowRecord->target_sbc == -1) {
        // Yes. Return unsupported, so our own beamposition timestamping is used
        // to calculate actual onset time and such:
        windowRecord->specialflags |= kPsychBufferAgeWarningDone;
        return(-1);
    }

    // Do we use X11 client-compositor sync protocol for composited windows, to wait
    // for true present completion by compositor and get timestamps from it?
    if (windowRecord->targetSpecific.syncCounter[1] && windowRecord->targetSpecific.targetSyncCounter &&
        (PsychPrefStateGet_VBLTimestampingMode() == 4)) {
        // Yes.
        XEvent ev;
        double tstart, tnow;
        psych_uint64 fc = 0;
        psych_uint64 ts = 0;
        int verbosity = PsychPrefStateGet_Verbosity();

        PsychGetAdjustedPrecisionTimerSeconds(&tstart);

        // First we expect the _NET_WM_FRAME_DRAWN ClientMessage with which the compositor
        // signals completion of the compositing pass (ie. submit rendering + swapbuffers
        // to the display gpu driver) which includes the most recent updated visual content
        // submitted by us for our window in PsychOSFlipWindowBuffers():
        while (TRUE) {
            // TODO FIXME: Use polling and peeking instead of blocking dequeue, to make
            // this compatible with multi-window operation and async flips etc., where we
            // do not want to hold the display lock for long amounts of time, and not clobber
            // events for different onscreen windows on the same display connection.
            // Or implement some distribution and logging...
            PsychLockDisplay();
            XNextEvent(windowRecord->targetSpecific.privDpy, &ev);
            PsychUnlockDisplay();

            if (ev.type == ClientMessage) {
                if ((ev.xclient.window == windowRecord->targetSpecific.xwindowHandle) && (ev.xclient.format == 32)) {
                    PsychGetAdjustedPrecisionTimerSeconds(&tnow);
                    if (verbosity > 12)
                        printf("Got ClientMessage: %f secs  [%f delta] Serial %i, synthetic %i - ", tnow, tnow - tstart, ev.xclient.serial, ev.xclient.send_event);

                    if (ev.xclient.message_type == _NET_WM_FRAME_DRAWN_atom) {
                        if (verbosity > 12) printf("_NET_WM_FRAME_DRAWN:   ");
                        fc = ((psych_uint64) (unsigned int) ev.xclient.data.l[1]) << 32ULL | ((psych_uint64) (unsigned int) ev.xclient.data.l[0]);

                        if (fc == windowRecord->targetSpecific.targetSyncCounter) {
                            ts = ((psych_uint64) (unsigned int) ev.xclient.data.l[3]) << 32ULL | ((psych_uint64) (unsigned int) ev.xclient.data.l[2]);
                            if (verbosity > 12) printf("ACCEPTED - Framecount %llu, Timestamp %llu usecs.\n", fc, ts);
                            break;
                        }
                        else
                            if (verbosity > 12) printf("\n");
                    }
                    else if (ev.xclient.message_type == _NET_WM_FRAME_TIMINGS_atom) {
                        if (verbosity > 12) printf("_NET_WM_FRAME_TIMINGS.\n");
                    }
                    else {
                        if (verbosity > 12) printf("Unknown.\n");
                    }
                }
            }
        }

        // Then we expect the _NET_WM_FRAME_TIMINGS ClientMessage after pageflip completion of
        // the relevant compositor frame, ie. true stimulus onset, including some high precision
        // timestamps of stimulus onset - well at least high precision for FOSS display drivers:
        while (TRUE) {
            PsychLockDisplay();
            XNextEvent(windowRecord->targetSpecific.privDpy, &ev);
            PsychUnlockDisplay();

            if (ev.type == ClientMessage) {
                if ((ev.xclient.window == windowRecord->targetSpecific.xwindowHandle) && (ev.xclient.format == 32)) {
                    PsychGetAdjustedPrecisionTimerSeconds(&tnow);
                    if (verbosity > 12)
                        printf("Got ClientMessage: %f secs  [%f delta] Serial %i, synthetic %i - ", tnow, tnow - tstart, ev.xclient.serial, ev.xclient.send_event);

                    if (ev.xclient.message_type == _NET_WM_FRAME_DRAWN_atom) {
                        if (verbosity > 12) printf("_NET_WM_FRAME_DRAWN.\n");
                    }
                    else if (ev.xclient.message_type == _NET_WM_FRAME_TIMINGS_atom) {
                        if (verbosity > 12) printf("_NET_WM_FRAME_TIMINGS: ");
                        if (fc == ((psych_uint64) (unsigned int) ev.xclient.data.l[1]) << 32ULL | ((psych_uint64) (unsigned int) ev.xclient.data.l[0])) {
                            if (verbosity > 12) printf("ACCEPTED - Framecount %llu, Timestamp %llu usecs + %i usecs ", fc, ts, ev.xclient.data.l[2]);
                            ts += ev.xclient.data.l[2];
                            if (verbosity > 12) printf("= %llu usecs effective. Refresh %i usecs, Deadline %i usecs\n", ts, ev.xclient.data.l[3], ev.xclient.data.l[4]);
                            // Assign compositor reported nominal refresh interval - used for PsychOSScheduleFlipWindowBuffers in hybridGraphics mode 5:
                            if (ev.xclient.data.l[3])
                                windowRecord->VideoRefreshInterval = ((double) ev.xclient.data.l[3]) / 1e6;
                            break;
                        }
                        else
                            if (verbosity > 12) printf("\n");
                    }
                    else {
                        if (verbosity > 12) printf("Unknown.\n");
                    }
                }
            }
        }

        // Valid timestamp returned?
        if (ts) {
            // Yes. Decode into stimulus onset time and return it:
            if (targetSBC == 0)
                targetSBC = windowRecord->target_sbc;

            sbc = targetSBC;
            msc = 0;
            ust = ts;

            // Assume X11 compositor present was done via pageflip:
            windowRecord->swapcompletiontype = 1;

            if (verbosity > 12) {
                PsychGetAdjustedPrecisionTimerSeconds(&tnow);
                printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: %f secs  [%f delta] presented at NetWM: sbc = %llu msc = %llu  ust = %lld mode = %i.\n",
                       tnow, tnow - tstart, sbc, msc, ust, windowRecord->swapcompletiontype);
            }

            // See below...
            if (tSwap == NULL) return(msc);

            // Success at least for timestamping. Translate ust into system time in seconds:
            *tSwap = PsychOSMonotonicToRefTime(((double) ust) / PsychGetKernelTimebaseFrequencyHz());

            // Make sure msc is always positive and incrementing (needed, because msc is defined as signed integer):
            msc &= ~(1ULL << 63);

            // Update cached reference values for future swaps:
            windowRecord->reference_ust = ust;
            windowRecord->reference_msc = msc;
            windowRecord->reference_sbc = sbc;

            if (verbosity > 11)
                printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: Success -  NetWM! refust = %lld, refmsc = %llu, refsbc = %llu.\n", ust, msc, sbc);
        }

        // Is this most likely a single-gpu setup with NVidia gpu and NVidia proprietary driver? In such a config, none of the high
        // precision timestamping methods is available for the X11 compositor, which itself is running on the proprietary driver and
        // GLX module, neither DRI/Present, nor OML_sync_control nor INTEL_swap_event, and the compositor won't have our mmio tricks
        // either. Therefore its timestamps, if any, won't be accurate.
        if (!(windowRecord->gfxcaps & kPsychGfxCapSupportsOpenML) && (NULL != glXGetVideoSyncSGI) && (windowRecord->hybridGraphics != 5)) {
            // Yes. Return unsupported, so our own beamposition timestamping is used to calculate actual onset time and such:
            windowRecord->specialflags |= kPsychBufferAgeWarningDone;
            return(-1);
        }

        // We are successfully done:
        return(msc);
    }

    if ((windowRecord->hybridGraphics == 5) && PsychOSEnablePresentEventReception(windowRecord, 0, TRUE)) {
        double tstart, tnow;
        xcb_present_generic_event_t *event = NULL;
        psych_bool hasComposited = FALSE;
        psych_bool hasCommitted = FALSE;
        xcb_connection_t *dpy = XGetXCBConnection(windowRecord->targetSpecific.privDpy);
        psych_bool withCompositor = windowRecord->targetSpecific.present_notify_queue[1] ? TRUE : FALSE;

        if (targetSBC == 0)
            targetSBC = windowRecord->target_sbc;

        PsychGetAdjustedPrecisionTimerSeconds(&tstart);
        sbc = -1;

        while (TRUE) {
            event = (xcb_present_generic_event_t*) xcb_wait_for_special_event(dpy, windowRecord->targetSpecific.present_notify_queue[0]);
            if (event) {
                PsychGetAdjustedPrecisionTimerSeconds(&tnow);

                if (event->evtype == XCB_PRESENT_EVENT_IDLE_NOTIFY) {
                    xcb_present_idle_notify_event_t *idled = (void*) event;

                    if (!hasComposited && (idled->serial >= targetSBC))
                        hasComposited = TRUE;

                    if (PsychPrefStateGet_Verbosity() > 12)
                        printf("%f [%f] Window %p pixmap idle event at: sbc = %llu -- hasComposited %i\n",
                               tnow, tnow - tstart, idled->window, idled->serial, hasComposited);
                }

                if (event->evtype == XCB_PRESENT_EVENT_COMPLETE_NOTIFY) {
                    xcb_present_complete_notify_event_t *completed = (void*) event;
                    if (completed->kind == XCB_PRESENT_COMPLETE_KIND_PIXMAP) {
                        if (!hasCommitted && (completed->serial >= targetSBC)) {
                            hasCommitted = TRUE;

                            sbc = completed->serial;
                            msc = completed->msc;
                            ust = completed->ust;

                            ParseX11PresentCompleteMode(windowRecord, completed);
                        }

                        if (PsychPrefStateGet_Verbosity() > 12) {
                            printf("%f [%f] Window %p presented event at: sbc = %llu msc = %llu  ust = %llu mode = %i -- hasCommitted %i\n",
                                   tnow, tnow - tstart, completed->window, completed->serial, completed->msc, completed->ust,
                                   windowRecord->swapcompletiontype, hasCommitted);
                        }
                    }
                    else if (PsychPrefStateGet_Verbosity() > 12) {
                        printf("msc wait/query complete notify. sbc = %llu\n", completed->serial);
                    }
                }

                free(event);
            }

            if (hasCommitted && (!withCompositor || hasComposited))
                break;
        }

        // In the simple case without compositor we are done and sbc, msc, ust, swapcompletiontype
        // represent the presentation state of our last flip.
        //
        // In the compositor case, we only know that our flip's new visual content update has been
        // processed/composited by the compositor into its to-be-presented framebuffer. We have to
        // wait for the actual composition to be page-flipped into the scanout:
        if (withCompositor) {
            if (PsychPrefStateGet_Verbosity() > 12) {
                printf("%f [%f] Content composited at: sbc = %llu msc = %llu  ust = %llu mode = %i ---> Awaiting compositor flip.\n",
                       tnow, tnow - tstart, sbc, msc, ust, windowRecord->swapcompletiontype);
            }

            // Now we need to wait for flip completion on the actual compositor output window - the true framebuffer.
            // We wait for a pageflip completion event with a completion msc > recorded msc of queuing the present to
            // the compositor, as detected above.
            hasCommitted = FALSE;
            event = NULL;
            while (TRUE) {
                if (PsychPrefStateGet_Verbosity() > 12)
                    printf("%f [%f] Awaiting compositor flip with msc > target MSC %llu.\n", tnow, tnow - tstart, msc);

                if (!hasCommitted)
                    event = (xcb_present_generic_event_t*) xcb_wait_for_special_event(dpy, windowRecord->targetSpecific.present_notify_queue[1]);
                else
                    event = (xcb_present_generic_event_t*) xcb_poll_for_special_event(dpy, windowRecord->targetSpecific.present_notify_queue[1]);

                if (event) {
                    PsychGetAdjustedPrecisionTimerSeconds(&tnow);

                    if (event->evtype == XCB_PRESENT_EVENT_COMPLETE_NOTIFY) {
                        xcb_present_complete_notify_event_t *completed = (void*) event;
                        if (completed->kind == XCB_PRESENT_COMPLETE_KIND_PIXMAP) {
                            if ((completed->mode == XCB_PRESENT_COMPLETE_MODE_FLIP) && ((psych_int64) completed->msc > msc)) {
                                hasCommitted = TRUE;

                                // Record potential true msc, ust of flip completion aka stimulus onset:
                                msc = completed->msc;
                                ust = completed->ust;

                                ParseX11PresentCompleteMode(windowRecord, completed);
                            }

                            if (PsychPrefStateGet_Verbosity() > 12) {
                                printf("%f [%f] Compositor window %p presented event at: compositor-sbc = %llu msc = %llu  ust = %llu mode = %i -- hasCommitted %i\n",
                                       tnow, tnow - tstart, completed->window, completed->serial, completed->msc, completed->ust, windowRecord->swapcompletiontype, hasCommitted);
                            }
                        }
                        else if (PsychPrefStateGet_Verbosity() > 12) {
                            printf("COMPOSITOR OUTPUT WINDOW msc wait/query complete notify. sbc = %llu\n", completed->serial);
                        }
                    }

                    free(event);
                    event = NULL;
                }
                else if (hasCommitted) {
                    break;
                }
            }
        }

        if (PsychPrefStateGet_Verbosity() > 12) {
            PsychGetAdjustedPrecisionTimerSeconds(&tnow);
            printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: %f secs  [%f delta] presented at: sbc = %llu msc = %llu  ust = %llu mode = %i.\n",
                   tnow, tnow - tstart, sbc, msc, ust, windowRecord->swapcompletiontype);
        }

        // See below...
        if (tSwap == NULL) return(msc);

        // Success at least for timestamping. Translate ust into system time in seconds:
        *tSwap = PsychOSMonotonicToRefTime(((double) ust) / PsychGetKernelTimebaseFrequencyHz());

        // Make sure msc is always positive and incrementing (needed, because msc is defined as signed integer):
        msc &= ~(1ULL << 63);

        // Update cached reference values for future swaps:
        windowRecord->reference_ust = ust;
        windowRecord->reference_msc = msc;
        windowRecord->reference_sbc = sbc;

        if (PsychPrefStateGet_Verbosity() > 11)
            printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: Success! refust = %lld, refmsc = %lld, refsbc = %lld.\n", ust, msc, sbc);

        // Used copy-swaps, despite being a fullscreen window, which should be unredirected via unredirect_fullscreen_windows policy,
        // thereby bypass compositing and use page-flipping?
        if (!withCompositor && (windowRecord->swapcompletiontype == 3) && (windowRecord->specialflags & kPsychIsFullscreenWindow)) {
            // Yes: Could be compositor active, or could be no compositor active but pageflip rejected due to incompatible drivers or
            // hardware, e.g., due to use of AMD iGPU + NVidia dGPU as of Linux 5.14. Could also be old-school no-compositor at all,
            // and still pageflip rejected.
            if (PsychPrefStateGet_Verbosity() > 2)
                printf("PTB-INFO: Suboptimal non-pageflipped presentation in use. ");

            // If a compositor is active or at least enabled, so we either already do use, or could use, compositing, we may be
            // able to salvage this, timing-wise, assuming user allows it by not using a timestamping mode other than 4. We allow
            // to opt out of this with another mode, because it will cut achievable framerates down to half the video refresh rate.
            if (PsychOSIsDWMEnabled(windowRecord->screenNumber) && (PsychPrefStateGet_VBLTimestampingMode() == 4)) {
                // Compositor enabled, and either already compositing us, or at least able to do so.
                // Make sure we go through compositing and setup for use of our Present+Compositor timing trick:
                if (PsychPrefStateGet_Verbosity() > 2)
                    printf("Reconfiguring for compositing, with potentially lower performance and higher lag.\n");

                // Disable our current Present timestamping:
                PsychOSEnablePresentEventReception(windowRecord, 0, FALSE);

                // Try to request compositing explicitely:
                unsigned int dontcomposite = 2;
                PsychLockDisplay();
                XChangeProperty(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.xwindowHandle,
                                XInternAtom(windowRecord->targetSpecific.privDpy, "_NET_WM_BYPASS_COMPOSITOR", False),
                                XA_CARDINAL, 32, PropModeReplace, (unsigned char *) &dontcomposite, 1);

                // Need to flush the connection and then wait a bit for the compositor to respond by creating
                // a suitable composition output child window of the X-Composite overlay window. The event
                // reception setup code below will latch onto that newly created window to get the actual
                // pageflip completion events and timestamps for stimulus onset:
                XFlush(windowRecord->targetSpecific.privDpy);
                PsychUnlockDisplay();

                PsychYieldIntervalSeconds(1);

                // Try to use NetWM client-compositor extended sync for timestamping, fall back to
                // Present-Events if that is not supported by the running compositor:
                if (!PsychOSEnableX11ClientCompositorSync(windowRecord)) {
                    // Reenable in compositor compatible mode as fallback:
                    PsychOSEnablePresentEventReception(windowRecord, 1, TRUE);
                    PsychOSEnablePresentEventReception(windowRecord, 0, TRUE);
                    if (PsychPrefStateGet_Verbosity() > 2) {
                        printf("PTB-INFO: Using a desktop GUI like GNOME-3 or Ubuntu desktop, which uses the Mutter compositor, may give better timing if\n");
                        printf("PTB-INFO: you opt-in into our new experimental(!) support: Call setenv('PSYCH_EXPERIMENTAL_NETWMTS', '1') at start of your script.\n");
                    }
                }

                // Fake a "pageflip used" if this was one of the first few presents, so we shut up pointless warnings:
                if (sbc <= 10)
                    windowRecord->swapcompletiontype = 1;
            }
            else if (PsychPrefStateGet_Verbosity() > 2) {
                printf("Compositor unavailable or its use forbidden, can't compensate. Expect timing problems and visual tearing artifacts!\n");
            }
        }

        // Done, jump to the end of the routine, where some checking and warning about lack of pageflip is done
        // if needed:
        goto fliptypecheck;
    }

    // DRI Prime hybridGraphics setup in outputSource -> outputSink mode? Typically a NVidia Optimus
    // Laptop under the proprietary NVidia driver, which can't do the much better Prime DRI3/Present gpu renderoffload.
    // If OML_sync_control is not supported natively, then use our custom UDP protocol between the modesetting-ddx and us.
    // This only works for unredirected fullscreen windows, as only then we can be somewhat certain that
    // pageflips on the outputSink iGPU are triggered by our OpenGL bufferswaps.
    if (((NULL == glXWaitForSbcOML) || (windowRecord->specialflags & kPsychOpenMLDefective)) &&
        (windowRecord->hybridGraphics == 3 || windowRecord->hybridGraphics == 4) &&
        (windowRecord->specialflags & kPsychIsFullscreenWindow)) {
        // Type 4 setup uses wait for true flip completion, type 3 setup only waits for flip scheduled:
        int targetCompletionMode = (windowRecord->hybridGraphics == 4) ? 1 : 0;
        int xscreen = PsychGetXScreenIdForScreen(windowRecord->screenNumber);

        // Yes. Try to use our own custom UDP network protocol to get swap completion timestamps
        // from the specially hacked modesetting ddx.
        if (prime_sockfd[xscreen] == 0) {
            struct _buf buf;
            struct sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(client_addr));
            client_addr.sin_family = AF_INET;
            client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
            client_addr.sin_port = htons(10000 + xscreen);

            // Get socket:
            prime_sockfd[xscreen] = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (prime_sockfd[xscreen] < 0) {
                if (PsychPrefStateGet_Verbosity() > 0)
                    printf("PTB-ERROR:PsychOSGetSwapCompletionTimestamp: Failed to create UDP receive socket for Prime slave-output timestampig on screen %i! [%s]\n",
                           windowRecord->screenNumber, strerror(errno));
                return(-1);
            }

            // Try to bind it to localhost loopback receive port 10000 + X-Screen numbeer:
            if (bind(prime_sockfd[xscreen], (struct sockaddr *) &client_addr, sizeof(client_addr))) {
                // Failed.
                if (PsychPrefStateGet_Verbosity() > 0)
                    printf("PTB-ERROR:PsychOSGetSwapCompletionTimestamp: Failed to bind UDP socket for Prime slave-output timestampig on screen %i to port %i! [%s]\n",
                           windowRecord->screenNumber, 10000 + xscreen, strerror(errno));
                close(prime_sockfd[xscreen]);
                prime_sockfd[xscreen] = -1;
                return(-1);
            }

            prime_sockfd2[xscreen] = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (prime_sockfd2[xscreen] < 0) {
                if (PsychPrefStateGet_Verbosity() > 0)
                    printf("PTB-ERROR:PsychOSGetSwapCompletionTimestamp: Failed to create UDP send socket for Prime slave-output timestampig on screen %i! [%s]\n",
                           windowRecord->screenNumber, strerror(errno));
                close(prime_sockfd[xscreen]);
                prime_sockfd[xscreen] = -1;
                return(-1);
            }

            client_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            if(connect(prime_sockfd2[xscreen], (struct sockaddr *) &client_addr, sizeof(client_addr))) {
                // Failed.
                if (PsychPrefStateGet_Verbosity() > 0)
                    printf("PTB-ERROR:PsychOSGetSwapCompletionTimestamp: Failed to connect UDP send socket for Prime slave-output timestampig on screen %i to port %i! [%s]\n",
                           windowRecord->screenNumber, 10000 + xscreen, strerror(errno));
                close(prime_sockfd[xscreen]);
                prime_sockfd[xscreen] = -1;
                close(prime_sockfd2[xscreen]);
                prime_sockfd2[xscreen] = -1;
                return(-1);
            }

            // Ready to rock at next flip:
            windowRecord->specialflags &= ~kPsychOpenMLDefective;

            if (PsychPrefStateGet_Verbosity() > 2)
                printf("PTB-INFO: Custom PRIME Flip completion timestamping for screen %i enabled. Will start at next Flip...\n", windowRecord->screenNumber);

            // Warn about use of multiple video outputs per screen. This cannot possibly work with any level of reliability:
            if ((PsychScreenToHead(windowRecord->screenNumber, 1) >= 0) && (PsychPrefStateGet_Verbosity() > 1))
                printf("PTB-WARNING: Custom PRIME Flip completion timestamping for screen %i will not work reliably, because multiple displays are active on this screen!\n",
                       windowRecord->screenNumber);

            // We must skip this one, as the X-Server may have sent out feedback for this flip already,
            // before we managed to create and bind our UDP socket, so the info may be lost and we would
            // hang if we tried to wait for it to arrive. The next flip will be processed by us:
            windowRecord->reference_sbc = 0;

            // Wait for half a second to make sure that any potential pending flips complete
            // and any in-flight UDP packets have been received by our new socket. Then drain
            // whatever got received, until nothing gets received for at least 10 msecs.
            // We want to make sure the sockets receive queue is in a well defined (=empty)
            // state before we use the connection for actual swap timestamping:
            PsychWaitIntervalSeconds(0.5);
            while (recv(prime_sockfd[xscreen], &buf, sizeof(buf), MSG_DONTWAIT | MSG_WAITALL) >= 0)
                PsychWaitIntervalSeconds(0.010);

            return(-1);
        }

        // Prime custom timestamping ready?
        if (prime_sockfd[xscreen] > 0) {
            struct _buf buf;

            // First wait for swap fence packet via a blocking receive:
            while (TRUE) {
                recv(prime_sockfd[xscreen], &buf, sizeof(buf), MSG_WAITALL);
                if (PsychPrefStateGet_Verbosity() > 12)
                    printf("0-Screen %i: flipcomplete %i : sbc=%lld : msc=%lld : ust=%lld\n", buf.scrnIndex, buf.flags, windowRecord->reference_sbc, buf.frame, buf.usec);

                // glXSwapBuffers fence packet received from ourselves, ie. PsychOSFlipWindowBuffers?
                if (buf.flags == 100) {
                    // Exit blocking receive loop:
                    break;
                }
            }

            // Then, if waitig for flip completion, wait for the preceeding flip-scheduled packet via a blocking receive:
            while (targetCompletionMode == 1) {
                recv(prime_sockfd[xscreen], &buf, sizeof(buf), MSG_WAITALL);
                if (PsychPrefStateGet_Verbosity() > 12)
                    printf("0-Screen %i: flipcomplete %i : sbc=%lld : msc=%lld : ust=%lld\n", buf.scrnIndex, buf.flags, windowRecord->reference_sbc, buf.frame, buf.usec);

                // Proper "flip queued" packet received?
                if (buf.flags == 0) {
                    // Exit blocking receive loop:
                    break;
                }
            }

            // Then wait for desired flip scheduled/completion packet via a blocking receive:
            while (msc == -1) {
                recv(prime_sockfd[xscreen], &buf, sizeof(buf), MSG_WAITALL);
                if (PsychPrefStateGet_Verbosity() > 12)
                    printf("1-Screen %i: flipcomplete %i : sbc=%lld : msc=%lld : ust=%lld\n", buf.scrnIndex, buf.flags, windowRecord->reference_sbc, buf.frame, buf.usec);

                // Proper completion packet of type targetCompletionMode received?
                if (buf.flags == targetCompletionMode) {
                    // Yes. Assign preliminary result:
                    msc = buf.frame;
                    ust = buf.usec;

                    // Exit blocking receive loop:
                    break;
                }
            }

            // We have a useable result. But is it the most recent - and thereby right - one, or stale data?
            // Perform non-blocking receives until the sockets receive queue is drained. If a UDP packet of
            // our targetCompletionMode is received its content will overwrite our previous results:
            while (recv(prime_sockfd[xscreen], &buf, sizeof(buf), MSG_DONTWAIT | MSG_WAITALL) == sizeof(buf)) {
                if (PsychPrefStateGet_Verbosity() > 12)
                    printf("2-Screen %i: flipcomplete %i : sbc=%lld : msc=%lld : ust=%lld\n", buf.scrnIndex, buf.flags, windowRecord->reference_sbc, buf.frame, buf.usec);

                // Proper completion packet of type targetCompletionMode received?
                if (buf.flags == targetCompletionMode) {
                    // Yes. Overwrite previous result with this more recent one:
                    msc = buf.frame;
                    ust = buf.usec;
                }
            }

            // Make sure msc is always positive and incrementing (needed, because msc is defined as signed integer):
            msc &= ~(1ULL << 63);

            // If no actual timestamp / msc was requested, then we return here:
            if (tSwap == NULL) return(msc);

            // Success at least for timestamping. Translate ust into system time in seconds:
            *tSwap = PsychOSMonotonicToRefTime(((double) ust) / PsychGetKernelTimebaseFrequencyHz());

            if (targetCompletionMode == 0) {
                *tSwap += windowRecord->VideoRefreshInterval;
                ust += (psych_int64) (windowRecord->VideoRefreshInterval * 1e6);
            }

            // Update cached reference values for future swaps:
            windowRecord->reference_ust = ust;
            windowRecord->reference_msc = msc;
            windowRecord->reference_sbc++;

            if (PsychPrefStateGet_Verbosity() > 11) printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: Success! refust = %lld, refmsc = %lld, refsbc = %lld.\n", ust, msc, sbc);

            // Return msc of swap completion:
            return(msc);
        }

        // Does not work: Fall through to OML_sync_control timestamping.
    }

    // Extension unsupported or known to be defective? Return -1 "unsupported" in that case:
    if ((NULL == glXWaitForSbcOML) || (windowRecord->specialflags & kPsychOpenMLDefective)) return(-1);

    // Hack to work around bugs in Mesa 10.3.3's DRI3/Present glXWaitForSBC() implementation. Doesn't work properly
    // for targetSBC == 0, so feed it the equivalent non-zero targetSBC from the glXSwapBuffersMscOML()
    // return value to force it into the less broken code path. This bug is fixed in 10.3.4+, 10.4.1+, 10.5+ and later,
    // so mostly of historic value, as distros which enable DRI3 also ship sufficiently modern Mesa versions. Iow.,
    // this workaround is a good candidate for removal in a future cleanup:
    if (targetSBC == 0) {
        targetSBC = windowRecord->target_sbc;
        if (PsychPrefStateGet_Verbosity() > 11) printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: Supported. Calling with overriden targetSBC = %lld.\n", targetSBC);
    }
    else if (PsychPrefStateGet_Verbosity() > 11) printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: Supported. Calling with targetSBC = %lld.\n", targetSBC);

    // If this is a vsync'ed swap which potentially waits until a future point in time before completing, then
    // glXWaitForSbcOML() may block until that future point in time. Doing so, it will block the used x-display
    // connection to the X-Server. If we are not the only onscreen window in existence and use of per-window
    // x-display connections is disabled then we share this connection with all other onscreen windows. If
    // currently any asynchronous swaps are pending via async background flip threads, then us blocking
    // the shared x-display connection in glXWaitForSbcOML() would prevent those other threads from
    // communicating with the X-Server, effectively destroying all parallelism for background swap execution.
    // As a consequence all scheduled swaps on all onscreen windows would execute and finalize in lock-step,
    // rendering the requested stimulus onset presentation times for those windows dysfunctional, therefore
    // massively disrupting the wanted presentation timing!
    //
    // To prevent this, we must only call glXWaitForSbcOML() after we can be certain the swap completed. We
    // do this by waiting via polling. We poll the current sbc value and compare against the target value for
    // confirmed swap completion. Only then we continue to glXWaitForSbcOML() to collect the swap info non-blocking.
    //
    // This is the polling loop:
    PsychLockDisplay();
    while ((windowRecord->vSynced) && ((PsychGetNrAsyncFlipsActive() > 0) || (PsychGetNrFrameSeqStereoWindowsActive() > 0) || (PsychGetNrVRRSchedulerWindowsActive() > 0)) &&
        (windowRecord->targetSpecific.privDpy == windowRecord->targetSpecific.deviceContext) &&
        glXGetSyncValuesOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, &ust, &msc, &sbc) &&
        (sbc < windowRecord->target_sbc)) {
        // Wanted 'sbc' value of target_sbc not yet reached -> The bufferswap isn't confirmed to be completed yet.
        // Need to wait a bit to release the cpu for other threads and processes, then repoll for swap completion.
        PsychUnlockDisplay();

        // Make sure msc is always positive and incrementing (needed, because msc is defined as signed integer):
        msc &= ~(1ULL << 63);

        // Is the current video refresh cycle count 'msc' already at or past the expected count of swap completion?
        if (msc < windowRecord->lastSwaptarget_msc) {
            // No: At time 'ust', the 'msc' was at least one refresh cycle duration away from the earliest possible
            // count of swap completion. That means the swap won't complete earlier than at least one refresh
            // duration after 'ust'. Let's go to sleep and wait until almost until that point in time, aka
            // 'ust' + 1 video refresh duration:
            PsychWaitUntilSeconds(PsychOSMonotonicToRefTime(((double) ust) / PsychGetKernelTimebaseFrequencyHz()) + windowRecord->VideoRefreshInterval - 0.001);
        } else {
            // Yes: Swap completion can happen almost any time now. Sleep a bit, then repoll:
            PsychYieldIntervalSeconds(0.0001);
        }

        // Repoll for swap completion...
        PsychLockDisplay();
    }

    // Display lock held here...

    // Extension supported: Perform query and error check.
    if (!glXWaitForSbcOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, targetSBC, &ust, &msc, &sbc)) {
        PsychUnlockDisplay();

        // OpenML supposed to be supported and in good working order according to startup check?
        if (windowRecord->gfxcaps & kPsychGfxCapSupportsOpenML) {
            // Yes. Then this is a new failure condition and we report it as such:
            if (PsychPrefStateGet_Verbosity() > 11) {
                printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: glXWaitForSbcOML() failed! Failing with rc = -2.\n");
            }
            return(-2);
        }

        // No. Failing this call is kind'a expected, so we don't make a big fuss on each
        // failure but return "unsupported" rc, so calling code can try fallback-path without
        // making much noise:
        return(-1);
    }

    PsychUnlockDisplay();

    // Make sure msc is always positive and incrementing (needed, because msc is defined as signed integer):
    msc &= ~(1ULL << 63);

    // This disabled codepath implements a workaround for kms drivers which deliver broken/wrong kms pageflip
    // events. It would work on any kms driver, but is currently not needed for any of them (as of all kernel
    // versions starting with Linux 3.16, and all latest stable kernels since 3.13, and all kernels before 3.13).
    //
    // The workaround implemented here is used in PTB releases since April 2014 to work around bugs in kms-pageflip
    // events on nouveau-kms present in the initial releases of Linux 3.13 - 3.15. These bugs have been fixed in
    // Linux 3.16 and the bug fixes have been backported to 3.13 - 3.15, so all latest stable Linux kernels are now
    // bug-free wrt. this issue. However, 3.13-3.15 kernels which haven't been updated to the latest stable kernels
    // will carry the bug and need the workaround, as signalled by specialflags & kPsychNeedOpenMLTSWorkaround.
    // We also allow to manually force the workaround on, via kPsychForceOpenMLTSWorkaround ConserveVRAM setting,
    // in case it needs to be enabled post-release for whatever reason:
    if ((windowRecord->specialflags & kPsychNeedOpenMLTSWorkaround) || (PsychPrefStateGet_ConserveVRAM() & kPsychForceOpenMLTSWorkaround)) {
        int rc;
        double tref;

        // Try to (ab)use glXGetSyncValuesOML() to get kms vblank timestamp for the vblank of
        // swap completion. If this works we can take advantage of kms accurate timestamps.
        // However, due to the nature of the assumed kms pageflip bug there is a small chance that
        // glXWaitForSbcOML() returned after swap completion but *before* the vblank timestamps and
        // counts could get updated by the kernels vblank irq handler, so the values returned by
        // glXGetSyncValuesOML() might be outdated and therefore also wrong. We query the current
        // values and then validate them in a conservative fashion. If they are close enough to
        // current system time, ie., in the future or less than about a video refresh cycle in the
        // past then we can assume them to be correct and useful to us and we can use them. Otherwise
        // we assume we got old and stale values and just fallback to standard mmio beamposition
        // timestamping. This is a conservative approach which rather discards good values than
        // risking to accept wrong values. This way timestamps should always be correct, even though
        // we have to pay a price in terms of higher execution time and more timestamp noise in case
        // of false rejects:
        PsychLockDisplay();
        rc = glXGetSyncValuesOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, &ust, &msc, &sbc);
        PsychUnlockDisplay();

        // Make sure msc is always positive and incrementing (needed, because msc is defined as signed integer):
        msc &= ~(1ULL << 63);

        if (rc && (msc >= windowRecord->lastSwaptarget_msc)) {
            PsychGetAdjustedPrecisionTimerSeconds(&tref);
            // Threshold selection for stale timestamp reject: If VideoRefreshInterval is already available post
            // calibration, then we choose 80% video refresh duration - Should catch most timing spikes but still
            // provide enough safety margin against long vblank durations or other jitter. If measurement isn't available,
            // e.g., during initial calibration, we choose a 7 msecs threshold: That's about half a video refresh duration
            // for a 60 Hz display and 7/8th on a 120 Hz display - good enough for calibration, where we are tolerant against
            // outliers anyway.
            tref -= (windowRecord->VideoRefreshInterval > 0) ? (windowRecord->VideoRefreshInterval * 0.8) : 0.007;
            if (PsychOSMonotonicToRefTime(((double) ust) / PsychGetKernelTimebaseFrequencyHz()) < tref) {
                // Yes. Consider the returned ust invalid/outdated/stale. Return with "unsupported" rc to trigger
                // regular mmio beamposition timestamping:
                if (PsychPrefStateGet_Verbosity() > 11) {
                    printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: Workaround provided unreliable result - II. Returning rc=-1 to trigger silent fallback.\n");
                }
                return(-1);
            }
        }
        else {
            if (PsychPrefStateGet_Verbosity() > 11) {
                printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: Workaround provided unreliable result - I. Returning rc=-1 to trigger silent fallback.\n");
            }
            return(-1);
        }
    }

    // Check for valid return values: A zero ust or msc means failure, except for results from nouveau,
    // because there it is "expected" to get a constant zero return value for msc, at least when running
    // on top of a Linux kernel older than 3.13, when this shortcoming was fixed (except for old pre-nv50
    // gpu's = GeForce-7000 and earlier, where the old fallback path is triggered to work around problems
    // with this ancient hardware, and that fallback still returns msc == 0):
    if ((windowRecord->vSynced) && ((ust == 0) || ((msc == 0) && !strstr((char*) glGetString(GL_VENDOR), "nouveau")))) {
        // If this happens at a sbc of less than 20 then it is a known glitch in the intel-ddx which has no
        // practical negative effects, so we paper over it and fail silently with an "unsupported" rc. A
        // pointless warning for a non-issue would just confuse users.
        if (sbc < 20) return(-1);

        if (PsychPrefStateGet_Verbosity() > 1) {
            printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: Invalid return values ust = %lld, msc = %lld from call with success return code (sbc = %lld)! Failing with rc = -2.\n", ust, msc, sbc);
            printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: This likely means a driver bug or malfunction, or that timestamping support has been disabled by the user in the driver!\n");
        }

        // Return with "failure" rc, so calling code can provide more error handling:
        return(-2);
    }

    // Another consistency check: This one is meant to catch the totally broken glXSwapBuffersMscOML()
    // implementation of the Intel-DDX from June 2011 to December 2012. The bug has been fixed in the
    // ddx driver version 2.20.16, released at 15th December 2012.
    //
    // That driver completely ignores the provided targetMSC for fullscreen page-flips!!! It just swaps
    // at next vblank. Iow, the real msc of swap completion can be much lower than the requested targetMSC,
    // killing any kind of stimulus onset timing.
    //
    // Check for this: If the swapcompletion msc is at least 2000 counts, we assume it didn't just wrap
    // around by chance, but is a valid target for concsistency checks. 2000 vblanks ~ 10-33 secs with typical
    // refresh rates. This means a blind spot of about 10-30 seconds every ~ 8 months of system uptime, so
    // at most a dozen trials could get screwed unnoticed if somebody is really really unlucky.
    //
    // If the check is executed, the msc of swap completion should always be >= targetMSC, otherwise something
    // is deeply broken in the driver:
    if ((windowRecord->vSynced) && (msc > 2000) && (windowRecord->lastSwaptarget_msc > 2000) && (msc < windowRecord->lastSwaptarget_msc)) {
        // Utterly broken OML swap scheduling! Disable it, so we can use our old fallback path. Warn user once
        // about broken driver:

        // First detected failure? Skip it on successive failures, as the fallback path will have taken
        // care of it -- One would hope at least.
        if (windowRecord->gfxcaps & kPsychGfxCapSupportsOpenML) {
            // Disable OpenML swap scheduling, we will use the classic wait + glXSwapBuffers path, but
            // still keep glXWaitForSBC() timestamping functional:
            windowRecord->gfxcaps &= ~kPsychGfxCapSupportsOpenML;

            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("\nPTB-WARNING: The flip stimulus onset completed at vblank count %lld before the requested target vblank count %lld !!\n", msc, windowRecord->lastSwaptarget_msc);
                printf("PTB-WARNING: This likely means a serious graphics driver bug or malfunction in the drivers swap scheduling mechanism!\n");
                printf("PTB-WARNING: I will now switch to a fallback / backup method for the remainder of this session, trying to work around this bug.\n");
                printf("PTB-WARNING: All Intel graphics drivers released between June 2011 and up to 14th December 2012 are known to have this bug.\n");
                printf("PTB-WARNING: If you use such a graphics card or driver, please try to update your graphics driver as soon as possible for reliable operation.\n\n");
            }
        }
        else {
            // Failure detected again, *after* our workaround was activated! This must be another independent bug!
            // Note: So far no driver ever exposed this bug, but some of the work the Intel developers are currently
            // doing on their driver has some potential to introduce such a bug, so we are better safe than sorry.
            // Specifically: If the kms pageflip completion handler in the intel-kms driver should return a stale
            // msc and ust from previous vblank, because our special workaround and safety code was removed in Oct. 2012.
            // In theory, the driver has been fixed for all current Intel gpu's, but in practice you never know what
            // kind of hardware bugs may show up or hide in future and current gpus. Better safe than sorry...

            // Disable OpenML completely, in the hope that our old "classic" path can somehow deal with the problem,
            // or perform further diagnostics at least:
            windowRecord->specialflags |= kPsychOpenMLDefective;
            if (PsychPrefStateGet_Verbosity() > 0) {
                printf("\nPTB-ERROR: The flip stimulus onset completed at vblank count %lld before the requested target vblank count %lld !!\n", msc, windowRecord->lastSwaptarget_msc);
                printf("PTB-ERROR: This likely means a serious graphics driver bug or malfunction in the drivers swap scheduling or timestamping mechanism!\n");
                printf("PTB-ERROR: I will now switch to a fallback / backup method for the remainder of this session, trying to work around this bug.\n");
                printf("PTB-ERROR: There are no guarantees though. Your system should be considered *not trustwhorthy* for timing sensitive tasks\n");
                printf("PTB-ERROR: until the problem is properly diagnosed and fixed. Please report this failure to the Psychtoolbox user forum.\n\n");
            }
        }
    }

    // If no actual timestamp / msc was requested, then we return here. This is used by the
    // workaround code for multi-threaded XLib access. It passes NULL to just (ab)use this
    // function to wait for swap completion, before it touches the framebuffer for real.
    // See function PsychLockedTouchFramebufferIfNeeded() in PsychWindowSupport.c
    // It is also used for the startup tests in PsychOSInitializeOpenML() which is why it
    // *has* to be located after the consistency checks directly above this statement.
    if (tSwap == NULL) return(msc);

    // Success at least for timestamping. Translate ust into system time in seconds:
    *tSwap = PsychOSMonotonicToRefTime(((double) ust) / PsychGetKernelTimebaseFrequencyHz());

    // If we are running on a slightly incomplete nouveau-kms driver which always returns a zero msc,
    // we need to get good ust,msc,sbc values for later use as reference and as return value via an
    // extra roundtrip to the kernel. The most important info, the swap completion timestamp, aka ust
    // as returned from glXWaitForSbcOML() has already been converted into GetSecs() timebase and returned
    // in tSwap, so it is ok to overwrite ust here:
    if (msc == 0) {
        PsychLockDisplay();
        if (!glXGetSyncValuesOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, &ust, &msc, &sbc)) {
            PsychUnlockDisplay();

            // Ohoh:
            if (PsychPrefStateGet_Verbosity() > 11) {
                printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: Invalid return values ust = %lld, msc = %lld from glXGetSyncValuesOML() call with success return code (sbc = %lld)! Failing with rc = -1.\n", ust, msc, sbc);
            }

            // Return with "unsupported" rc, so calling code can try fallback-path:
            return(-1);
        }
        PsychUnlockDisplay();
    }

    // Make sure msc is always positive and incrementing (needed, because msc is defined as signed integer):
    msc &= ~(1ULL << 63);

    // Update cached reference values for future swaps:
    windowRecord->reference_ust = ust;
    windowRecord->reference_msc = msc;
    windowRecord->reference_sbc = sbc;

    if (PsychPrefStateGet_Verbosity() > 11) printf("PTB-DEBUG:PsychOSGetSwapCompletionTimestamp: Success! refust = %lld, refmsc = %lld, refsbc = %lld.\n", ust, msc, sbc);

    // Try to get corresponding INTEL_swap_event for cross-checking:
    if (PsychOSSwapCompletionLogging(windowRecord, 4, (int) sbc)) {
        // Label called from hybridGraphics == 5 Optimus code, with injected windowRecord->swapcompletiontype:
        fliptypecheck:

        // Got it. We are only interested in one thing: Was this a fullscreen window bufferswap with a non page-flipped swap?
        // For non-fullscreen windows, all bets are off wrt. stimulus onset timing or timestamping, and the user knows this,
        // as we've told so at window creation time.
        //
        // For fullscreen windows however, the user can expect pageflip swaps for best precision. If this doesn't work out,
        // it hints to some configuration problem on the system and we better warn the user about unreliable timing:
        if ((windowRecord->vSynced) && (windowRecord->specialflags & kPsychIsFullscreenWindow) && (PsychPrefStateGet_SkipSyncTests() < 2) && (windowRecord->swapcompletiontype > 1)) {
            // Ohoh: Non-pageflipped fullscreen window swap:
            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("\nPTB-WARNING: Flip %lld for window %i didn't use pageflipping for flip. Visual presentation timing and timestamps are likely unreliable!\n", windowRecord->target_sbc, windowRecord->windowIndex);
                printf("PTB-WARNING: Something is misconfigured on your system, otherwise pageflipping would have been used by the graphics driver for reliable timing.\n");
                printf("PTB-WARNING: However, if you see this message only sporadically, this might be caused by onscreen popup messages a la \"You have new mail!\" or\n");
                printf("PTB-WARNING: \"New updates are ready to install\" etc. Being low on free system memory can cause this as well, especially on integrated graphics chips.\n");
                printf("PTB-WARNING: Read the Linux specific section of 'help SyncTrouble' for some other common causes and fixes for this problem.\n");
            }
        }
    }

    #endif

    // Return msc of swap completion:
    return(msc);
}

/* PsychOSInitializeOpenML() - Linux specific function.
 *
 * Performs basic initialization of the OpenML OML_sync_control extension.
 * Performs basic and extended correctness tests and disables extension if it
 * is unreliable, or enables workarounds for partially broken extensions.
 *
 * Called from PsychDetectAndAssignGfxCapabilities() as part of the PsychOpenOffscreenWindow()
 * procedure for a window with OpenML support.
 *
 */
void PsychOSInitializeOpenML(PsychWindowRecordType *windowRecord)
{
    #ifdef GLX_OML_sync_control

    double tNow, tDeadline;
    psych_int64 ust, msc, sbc, oldmsc, oldust, finalmsc;
    psych_bool failed = FALSE;
    char extraversionsignature[512];
    int major = 0, minor = 0, patchlevel = 0, extralevel = 0;

    // Enable rendering context of window:
    PsychSetGLContext(windowRecord);

    // Check if we are running on a version of Linux 3.13 - 3.15 with broken nouveau-kms pageflip completion events:
    PsychOSGetLinuxVersion(&major, &minor, &patchlevel);

    // We check Linux versions 3.13 to 3.15 for broken kms-pageflip events if we are running on nouveau-kms.
    if (((major == 3) && ((minor >= 13) && (minor <= 15))) && strstr((const char *) glGetString(GL_VENDOR), "nouveau")) {
        // Potentially faulty nouveau-kms. Check against the known kernel patchlevels when the bug was fixed:
        // We know Linux stable kernels 3.13.11.5+, 3.14.12+ and 3.15.5+ are fixed.
        // As far as Ubuntu distribution kernels go, we know the ones based on 3.13.11.5+ are fine,
        // but we need some special case query code below to detect them, as their regular version
        // string identifies them as "3.13.0-something":
        if (((minor == 15) && (patchlevel < 5)) || ((minor == 14) && (patchlevel < 12))) {
            // Not yet fixed pre 3.14.12+ or pre 3.15.5+ kernel. Force workaround:
            windowRecord->specialflags |= kPsychNeedOpenMLTSWorkaround;
        }

        if ((minor == 13) && (patchlevel > 0) && (patchlevel < 12)) {
            // Not fixed 3.13.1 - 3.13.11 kernel:
            windowRecord->specialflags |= kPsychNeedOpenMLTSWorkaround;
        }

        // Finally a special case check for Ubuntu distribution kernels:
        if ((minor == 13) && (patchlevel == 0)) {
            // Is this a Ubuntu distro kernel?
            FILE* fd = fopen("/proc/version_signature", "rt");
            if (fd && fgets(extraversionsignature, sizeof(extraversionsignature), fd)) {
                // Seems so. Find signature of the stable kernel on which this one is based:
                if (!strstr(extraversionsignature, "3.13.11.")) {
                    // Nope. This is a regular 3.13.11 kernel with the bug:
                    windowRecord->specialflags |= kPsychNeedOpenMLTSWorkaround;
                }
                else {
                    // Definitely an Ubuntu distribution kernel based on the
                    // Ubuntu maintained 3.13.11.x long term stable kernel series.
                    // Kernels based on 3.13.11.5+ are good, earlier ones are bad:
                    sscanf(strstr(extraversionsignature, "3.13.11."), "%*i.%*i.%*i.%i", &extralevel);
                    if (extralevel < 5) {
                        windowRecord->specialflags |= kPsychNeedOpenMLTSWorkaround;
                        if (PsychPrefStateGet_Verbosity() >= 4) printf("PTB-INFO: Ubuntu distribution kernel with nouveau-kms pageflip event bug.\n");
                    }
                    else if (PsychPrefStateGet_Verbosity() >= 4) printf("PTB-INFO: Fixed Ubuntu distribution kernel without nouveau-kms pageflip event bug. Good :)\n");
                }
            }
            else {
                // Nope. This is a regular 3.13.0 kernel with the bug:
                windowRecord->specialflags |= kPsychNeedOpenMLTSWorkaround;
            }

            if (fd) fclose(fd);
        }

        // Yes. kms driver on these kernels delivers faulty data inside its kms-pageflip completion events, so although
        // return from glXWaitForSbcOML() can be trusted to mean swap-completion, the msc and ust timestamp are wrong.
        if ((windowRecord->specialflags & kPsychNeedOpenMLTSWorkaround) && (PsychPrefStateGet_Verbosity() >= 3)) {
            printf("PTB-INFO: Your Linux kernel has a slightly faulty nouveau graphics driver. Enabling a workaround.\n");
            printf("PTB-INFO: Please upgrade your kernel to the latest stable version as soon as possible to avoid the workaround.\n");
        }
    }

    PsychLockDisplay();

    // Perform a wait for 3 video refresh cycles to get valid (ust,msc,sbc)
    // values for initialization of windowRecord's cached values:
    if (!glXGetSyncValuesOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, &ust, &msc, &sbc) || (msc == 0) ||
        !glXWaitForMscOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, msc + 3, 0, 0, &ust, &msc, &sbc) || (ust == 0)) {
        PsychUnlockDisplay();

        // Basic OpenML functions failed?!? Not good! Disable OpenML swap scheduling:
        windowRecord->gfxcaps &= ~kPsychGfxCapSupportsOpenML;

        // OpenML timestamping in PsychOSGetSwapCompletionTimestamp() and PsychOSGetVBLTimeAndCount() disabled:
        windowRecord->specialflags |= kPsychOpenMLDefective;

        // Warn user:
        if (PsychPrefStateGet_Verbosity() > 1) {
            printf("PTB-WARNING: At least one test call for OpenML OML_sync_control extension failed! Will disable OpenML and revert to fallback implementation.\n");
        }

        return;
    }

    // Have a valid (ust, msc) baseline. Store it in windowRecord for future use:
    windowRecord->reference_ust = ust;
    windowRecord->reference_msc = msc;
    windowRecord->reference_sbc = sbc;

    // Perform correctness test for glXGetSyncValuesOML() over a time span
    // of 6 video refresh cycles. This checks for a limitation that is present
    // in all shipping Linux kernels up to at least version 2.6.36, possibly
    // also in 2.6.37 depending on MK's progress with this feature:
    finalmsc = msc + 6;
    oldmsc = msc;
    oldust = ust;
    PsychGetAdjustedPrecisionTimerSeconds(&tDeadline);
    tDeadline += 0.5;

    while ((msc < finalmsc) && !failed) {
        // Wait a quarter millisecond:
        PsychWaitIntervalSeconds(0.000250);

        // Query current (msc, ust):
        if (!glXGetSyncValuesOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, &ust, &msc, &sbc)) {
            // Query failed!
            failed = TRUE;
        }

        // Has msc changed since last query due to a regular msc increment, i.e., a new video refresh interval?
        if (msc != oldmsc) {
            // Yes. Update reference values for test:
            oldmsc = msc;
            oldust = ust;
        }

        // ust must be equal to oldust at this point, either because a msc increment has updated
        // the ust for the new vblank interval in lock-step and our code above has updated oldust
        // accordingly, or because no msc increment has happened, in which case ust should stay
        // unchanged as well, ie., ust == oldust. If ust and oldust are different then that means
        // that ust has changed its value in the middle of a refresh interval without an intervening
        // vblank. This would happen if glXGetSyncValuesOML() is defective and doesn't return ust
        // timestamps locked to vblank / msc increments, but simply system time values.
        if (ust != oldust) {
            // Failure of glXGetSyncValuesOML()! This is a broken implementation which needs
            // our workaround:
            failed = TRUE;
        }

        // Implement a 0.5 seconds timeout to deal with broken XWayland implementations:
        PsychGetAdjustedPrecisionTimerSeconds(&tNow);
        if (tNow > tDeadline) {
            failed = TRUE;
            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-INFO: OpenML OML_sync_control implementation with broken glXGetSyncValuesOML() function detected. XWayland?!? Enabling workaround for ok performance.\n");
            }
        }

        // Repeat test loop:
    }

    // Failed or succeeded?
    if (failed) {
        // Failed! Enable workaround and optionally inform user:
        windowRecord->specialflags |= kPsychNeedOpenMLWorkaround1;

        if (PsychPrefStateGet_Verbosity() > 1) {
            printf("PTB-INFO: OpenML OML_sync_control implementation with problematic glXGetSyncValuesOML() function detected. Enabling workaround for ok performance.\n");
        }
    }

    if (glXGetSyncValuesOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, &ust, &msc, &sbc)) {
        // Check swap scheduling for reliable operation. Intel ddx drivers from June 2011 to at least October 2012 are known
        // to be seriously buggy here. Schedule a swap a few vblanks in the future, wait for its completion and timestamp it.
        // This will run a consistency check inside PsychOSGetSwapCompletionTimestamp() which would trigger warnings and fallbacks
        // if it detects problems of the driver with sticking to the schedule:
        PsychUnlockDisplay();

        // A glClear to touch the framebuffer before flip. Why? To accomodate some quirks of
        // the Intel ddx as of 2.99.917 with DRI2+SNA and triple-buffering enabled. Makes
        // triple-buffered mode at least marginally useful for some restricted use cases.
        // Without rendering something to the framebuffer, swap scheduling totally falls over
        // if triple-buffering is enabled under DRI2...
        glClear(GL_COLOR_BUFFER_BIT);
        PsychOSScheduleFlipWindowBuffers(windowRecord, 0.0, msc + 5, 0, 0, 0);

        // Just a dummy call to wait for completion and to trigger consistency checks and workarounds if needed:
        PsychOSGetSwapCompletionTimestamp(windowRecord, 0, NULL);
    }
    else {
        PsychUnlockDisplay();

        if (PsychPrefStateGet_Verbosity() > 1) {
            printf("PTB-WARNING: Spurious failure of glXGetSyncValuesOML(). Could not perform some correctness tests. Something may be broken in your systems timestamping!\n");
        }
    }

    // Linux native VRR support enabled?
    if (PsychVRRActive(windowRecord) && ((windowRecord->gfxcaps & kPsychGfxCapLinuxVRR) || (windowRecord->gfxcaps & kPsychGfxCapGSync))) {
        double tFirst, tLast, tNominal, tMeasured;
        int i, good = 0;
        double tError = 0.0;

        // Yes, ostensibly. We know usercode requested it and the Linux kernel + gpu/cable/display
        // hardware supports it, and we requested it from the XOrg display driver. What we don't
        // know yet is if the XOrg ddx is of a suitable minimum version and if it is enabled in the
        // xorg.conf. The only way to find out is by looking for telltale signs of it via measurement.
        //
        // Execute a bufferswap, then another one back-to-back, but 1.25 nominal video refresh durations
        // after the 1st one. Wait for swap completion and check timestamps. If VRR is active, we should
        // see swap completion roughly 1.25 refreshes later. If VRR is inactive/disabled, then we should
        // see it complete at least 2.0 refresh durations later.
        tNominal = 1.0 / ((double) PsychGetNominalFramerate(windowRecord->screenNumber));

        if (PsychPrefStateGet_Verbosity() > 3)
            printf("\nPTB-DEBUG: VRR quick test...\n");

        PsychRealtimePriority(TRUE);

        // Run 60 trials:
        for (i = 0; i < 60; i++) {
            glClear(GL_COLOR_BUFFER_BIT);
            PsychOSFlipWindowBuffers(windowRecord);
            PsychOSGetSwapCompletionTimestamp(windowRecord, 0, &tFirst);
            glClear(GL_COLOR_BUFFER_BIT);
            glFinish();
            PsychWaitUntilSeconds(tFirst + 1.25 * tNominal);
            PsychOSFlipWindowBuffers(windowRecord);
            PsychOSGetSwapCompletionTimestamp(windowRecord, 0, &tLast);
            tMeasured = tLast - tFirst;
            if (tMeasured < 1.9 * tNominal) {
                good++;

                // Initial estimation of vrrLatencyCompensation offset:
                tError += tMeasured - 1.25 * tNominal;
            }

            if (PsychPrefStateGet_Verbosity() > 9)
                printf("PTB-DEBUG: [%i of %i] Msecs: %f vs. %f [nominal %f]\n", good, i + 1, 1000.0 * tMeasured, 1000.0 * 1.9 * tNominal, 1000.0 * tNominal);
        }

        PsychRealtimePriority(FALSE);

        if (PsychPrefStateGet_Verbosity() > 9)
            printf("\n");

        // Compute initial latency compensation as a simple mean of the measured latencies:
        windowRecord->vrrLatencyCompensation = (good > 0) ? (tError / (double) good) : 0.0;

        // At least 50 out of 60 as expected for VRR?
        if (good < 50) {
            // Nope: Mark VRR as unsupported:
            windowRecord->gfxcaps &= ~kPsychGfxCapLinuxVRR;
            windowRecord->gfxcaps &= ~kPsychGfxCapGSync;

            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: The VRR startup tests failed for this window. Seems Variable Refresh Rate doesn't work or\n");
                printf("PTB-WARNING: is disabled. This could be due to either the XOrg video driver (DDX) being too old, or due\n");
                printf("PTB-WARNING: to VRR being disabled in the XOrg config settings. In the latter case, use XOrgConfCreator +\n");
                printf("PTB-WARNING: XOrgConfSelector + logout + login to enable VRR.\n");
                printf("PTB-WARNING: In the former case, update your XOrg DDX video driver.\n");
                if (PsychOSX11ScreenUsesModesettingDDX(windowRecord->screenNumber)) {
                    printf("PTB-WARNING: You are currently using the modesetting-DDX video driver. For this to work, you either\n");
                    printf("PTB-WARNING: need X-Server 1.21 or later with modesetting-DDX 1.21 or later, or if you are on an\n");
                    printf("PTB-WARNING: older X-Server with AMD graphics, at least version 19.0 of the xf86-video-amdgpu driver.\n");
                }
                else {
                    printf("PTB-WARNING: For AMD graphics cards you need at least version 19.0 of the xf86-video-amdgpu driver.\n");
                }
                printf("PTB-WARNING: For more info and troubleshooting help, type 'help VRRSupport'.\n");
                printf("PTB-WARNING: [%i out of %i successful test trials, at least 50 successful ones needed.]\n\n", good, i);
            }
        }
        else if (PsychPrefStateGet_Verbosity() > 3)
            printf("PTB-DEBUG: VRR all good: Using initial latency compensation of %f msecs.\n", 1000 * windowRecord->vrrLatencyCompensation);
    }

    #else
    // Disable extension:
    windowRecord->gfxcaps &= ~kPsychGfxCapSupportsOpenML;
    #endif

    return;
}

static psych_int64 PsychOSScheduleSoftSyncFlip(PsychWindowRecordType *windowRecord, double tWhen, psych_int64 targetMSC)
{
#ifdef GLX_OML_sync_control
    psych_int64 msc, ust, sbc, rc = -1;
    double tNow, tVblankStart, tActiveStart, tOff, tOn, tSwapped;
    int headid, screenId;
    unsigned int ctlreg, value, status;
    int gpuMaintype, gpuMinortype, fNumDisplayHeads;

    screenId = windowRecord->screenNumber;

    // Check if the window is a fullscreen onscreen window, and no special transparency or
    // similar is enabled for the window, iow. if it is almost certainly a unredirected
    // fullscreen window. Also if this is a vsync'ed flip. We need this, because we need
    // kms-pageflipping for this to work. Also check if the low-level access support is
    // available, which will be needed:
    if (!((windowRecord->specialflags & kPsychIsFullscreenWindow) &&
        (windowRecord->vrrMode > kPsychVRROff) && windowRecord->vSynced &&
        (PsychPrefStateGet_WindowShieldingLevel() == 2000) &&
        PsychOSIsKernelDriverAvailable(screenId)))
        return(-1);

    // We only support AMD GPUs at the moment, nothing else, also only DCE-4.0 and later:
    if (!PsychGetGPUSpecs(screenId, &gpuMaintype, &gpuMinortype, NULL, &fNumDisplayHeads) ||
        (gpuMaintype != kPsychRadeon) || (gpuMinortype < 40) || (gpuMinortype >= 0xffff)) {
        return(-1);
    }

    // At the moment we only support one active output - single display stimulation, ie.
    // one display engine associated with this window. Otherwise it might get too complicated:
    if (PsychScreenToCrtcId(screenId, 1) >= 0)
        return(-1);

    // Get display headid for the single display engine associated with the onscreen windows monitor:
    headid = PsychScreenToCrtcId(screenId, 0);

    // Valid display engine id? Otherwise abort:
    if (headid < 0 || headid > fNumDisplayHeads - 1)
        return(-1);

    // We need a valid stimulus onset time, aka start time of scanout of a
    // previously flipped stimulus. If we don't have this, e.g., because user
    // requested a non-vsync'ed flip or forced high-precision timestamping off,
    // we don't have a reliable baseline for start of this video refresh cycle:
    tActiveStart = PsychGetVblankTimestamps(windowRecord, &tVblankStart);
    PsychGetAdjustedPrecisionTimerSeconds(&tNow);
    if (PsychPrefStateGet_Verbosity() > 10)
        printf("PTB-DEBUG: Offset %lf msecs.\n", 1000 * (tNow - tVblankStart));

    // Ok, minimum requirements at the hardware and configuration side fulfilled. Check if the
    // tWhen target onset time is not inside this video frame. It must be at least after the
    // start of active scanout for the next refresh cycle. For earlier tWhen we are too close, so
    // have to fall back to conventional flips, with stimulus onset at earliest - and locked to -
    // the start of the next frame:
    if (tWhen < tActiveStart + windowRecord->VideoRefreshInterval)
        return(-1);

    // Is tWhen inside the next video refresh cycle after this one?
    // If not, then we need to do a blocking wait until we are 1 refresh cycle
    // before the refresh cycle that contains tWhen:
    while (tWhen >= tVblankStart + 2 * windowRecord->VideoRefreshInterval) {
        // Yes. Sleep a refresh cycle, then recheck:
        //printf("TF %f ", (tWhen - tVblankStart) / windowRecord->VideoRefreshInterval);
        PsychWaitUntilSeconds(tVblankStart + windowRecord->VideoRefreshInterval);
        tActiveStart = PsychGetVblankTimestamps(windowRecord, &tVblankStart);
        //printf("Now %f\n", (tWhen - tVblankStart) / windowRecord->VideoRefreshInterval);
    }

    // Past safety deadline for safe soft-sync procedure?
    // The deadline is 2 msecs before start of vblank for the target frame, leaving
    // enough time to schedule the bufferswap and the display engine stop:
    PsychGetAdjustedPrecisionTimerSeconds(&tNow);
    if (tNow > tVblankStart + windowRecord->VideoRefreshInterval - 0.002) {
        // Too late! The best we can do is wait until start of the next scanout
        // cycle has happened, then go to the standard swap scheduling to trigger
        // an immediate bufferswap for the following cycle, so we swap too late
        // instead of too early wrt. tWhen -- minimum surprise:
        if (PsychPrefStateGet_Verbosity() > 10)
            printf("PTB-DEBUG: No headroom - %f msecs late. Fallback.\n",
                   tNow - (tVblankStart + windowRecord->VideoRefreshInterval - 0.002));
        PsychWaitUntilSeconds(tVblankStart + windowRecord->VideoRefreshInterval + 0.002);
        return(-1);
    }

    // The flip is supposed to happen at start of next vblank, so the next active scanout cycle would
    // show the new post-Flip stimulus. We do not want the next active scanout to start at its regular
    // time though, so program the display hw to stop the display engine / scanout at the end of this
    // refresh cycle:
    while (PsychGetDisplayBeamPosition(NULL, screenId) >= windowRecord->VBL_Startline);

    // Ok, all criteria are satisfied, the game is on!
    // We are safely inside the refresh cycle before the cycle which contains tWhen.
    // Issue an immediate bufferswap request to trigger pageflip programming for a pageflip
    // that should execute and complete at the start of the upcoming vblank:
    rc = glXSwapBuffersMscOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, 0, 0, 0);

    if (rc < 0)
        return(-4);

    // Invalidate target_sbc and lastSwaptarget_msc. target_sbc == -1 signals use of this
    // adaptive flip method, so PsychOSGetSwapCompletionTimestamp() can act accordingly:
    windowRecord->target_sbc = -1;
    windowRecord->lastSwaptarget_msc = 0;

    // Shut down this CRTC by clearing its master enable bit (bit 0):
    value = PsychOSKDReadRegister(screenId, EVERGREEN_CRTC_CONTROL + crtcoff[headid], &status);
    PsychOSKDWriteRegister(screenId, EVERGREEN_CRTC_CONTROL + crtcoff[headid], value & ~(0x1 << 0), &status);

    // Do a polling wait for confirmation of bufferswap completion:
    sbc = 0;
    tOff = 0;
    while (sbc < rc) {
        // Wait timed out? Abort, if so:
        PsychGetAdjustedPrecisionTimerSeconds(&tNow);
        if ((tOff == 0) &&
            !(PsychOSKDReadRegister(screenId, EVERGREEN_CRTC_CONTROL + crtcoff[headid], &status) & (0x1 << 16)))
            tOff = tNow;

        if (tNow > tActiveStart + windowRecord->VideoRefreshInterval) {
            rc = -4;
            if (PsychPrefStateGet_Verbosity() > 10)
                printf("PTB-DEBUG: Swap not completed before timeout! Display engine status: %i\n",
                       PsychOSKDReadRegister(screenId, EVERGREEN_CRTC_CONTROL + crtcoff[headid], &status) & (0x1 << 16) > 0);
            goto softflip_out;
        }
        glXGetSyncValuesOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, &ust, &msc, &sbc);
    }

    PsychGetAdjustedPrecisionTimerSeconds(&tSwapped);

    // Double check - Poll until crtc is offline:
    while (PsychOSKDReadRegister(screenId, EVERGREEN_CRTC_CONTROL + crtcoff[headid], &status) & (0x1 << 16)) {
        PsychGetAdjustedPrecisionTimerSeconds(&tNow);

        // Wait timed out? Abort, if so:
        if (tNow > tActiveStart + windowRecord->VideoRefreshInterval) {
            rc = -4;
            if (PsychPrefStateGet_Verbosity() > 10)
                printf("PTB-DEBUG: Display engine stop not completed before timeout!\n");
            goto softflip_out;
        }
    }

    // Get tOff if it hasn't been computed already in wait for swap completion:
    if (tOff == 0)
        PsychGetAdjustedPrecisionTimerSeconds(&tOff);

    // Pageflip completed, display engine stopped and at resting position at 1st line of vblank.
    // Ready for engine restart to get active scanout going close to the requested tWhen time.
    if (tWhen - (tActiveStart - tVblankStart) <= tOff + windowRecord->VideoRefreshInterval) {
        PsychWaitUntilSeconds(tWhen - (tActiveStart - tVblankStart));
    }
    else {
        if (PsychPrefStateGet_Verbosity() > 10)
            printf("PTB-DEBUG: Engine off clamped to safe max. 1 video refresh! Overshoot: %f msecs. Delta %f msecs.\n",
                   1000 * (tWhen - (tActiveStart - tVblankStart) - (tOff + windowRecord->VideoRefreshInterval)),
                   1000 * (tWhen - windowRecord->time_at_last_vbl));
        PsychWaitUntilSeconds(tOff + windowRecord->VideoRefreshInterval - (tActiveStart - tVblankStart));
    }

    // Record targetMSC as return code:
    rc = targetMSC;

softflip_out:
    // Start the display engine:
    value = PsychOSKDReadRegister(screenId, EVERGREEN_CRTC_CONTROL + crtcoff[headid], &status);
    PsychOSKDWriteRegister(screenId, EVERGREEN_CRTC_CONTROL + crtcoff[headid], value | (0x1 << 0), &status);

    while (!(PsychOSKDReadRegister(screenId, EVERGREEN_CRTC_CONTROL + crtcoff[headid], &status) & (0x1 << 16))) {
        PsychGetAdjustedPrecisionTimerSeconds(&tNow);
    }

    PsychGetAdjustedPrecisionTimerSeconds(&tOn);
    if (PsychPrefStateGet_Verbosity() > 10)
        printf("PTB-DEBUG: Scanout restarted at %f secs [Off %f msecs], tSwap %f msecs wrt. off. Req onset: %f secs\n",
            tOn, 1000 * (tOn - tOff), 1000 * (tSwapped - tOff), tWhen);

    // Done, one way or the other:
    return(rc);
#endif
}

static psych_bool PsychOSNetWMFlip(PsychWindowRecordType *windowRecord)
{
    XSyncValue syncIncrement;

    // First disable any latching of content from us to the compositor:
    // TODO: Investigate if a +3 increment for "urgent mode" has any advantages in any scenarios, over +1 standard mode?
    XSyncIntToValue(&syncIncrement, 1);
    XSyncChangeCounter(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.syncCounter[1], syncIncrement);

    // Then swapbuffers for an immediate non-vsynced copy-swap from our backbuffer to the offscreen composition surface:
    glXSwapBuffers(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle);

    // Make sure everything is really done and the redirection surface contains our new stimulus image:
    glXWaitGL();
    glXWaitX();

    // Then mark the frame as done to the compositor, to kick of a composition + actual swap/flip cycle:
    XSyncIntToValue(&syncIncrement, 1);
    XSyncChangeCounter(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.syncCounter[1], syncIncrement);

    // Get sync counter value for this flip cycle, so we can wait on present completion of it in PsychOSGetSwapCompletionTimestamp():
    if (XSyncQueryCounter(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.syncCounter[1], &syncIncrement))
        windowRecord->targetSpecific.targetSyncCounter = ((psych_uint64) XSyncValueHigh32(syncIncrement)) << 32ULL | ((psych_uint64) XSyncValueLow32(syncIncrement));
    else
        windowRecord->targetSpecific.targetSyncCounter = 0;

    return(windowRecord->targetSpecific.targetSyncCounter != 0);
}

/*
 *    PsychOSScheduleFlipWindowBuffers()
 *
 *    Schedules a double buffer swap operation for given window at a given
 *    specific target time or target refresh count in a specified way.
 *
 *    This uses OS specific API's and algorithms to schedule the asynchronous
 *    swap. This function is optional, target platforms are free to not implement
 *    it but simply return a "not supported" status code.
 *
 *    Arguments:
 *
 *    windowRecord          - The window to be swapped.
 *    tWhen                 - Requested target system time for swap. Swap shall happen at first VSync >= tWhen.
 *    targetMSC             - If non-zero, specifies target msc count for swap. Overrides tWhen, unless tWhen == DBL_MAX.
 *    divisor, remainder    - If set to non-zero, msc at swap must satisfy (msc % divisor) == remainder.
 *    specialFlags          - Additional options, a bit field consisting of single bits that can be or'ed together:
 *                              1 = Constrain swaps to even msc values (Used for frame-seq. stereo field selection)
 *                              2 = Constrain swaps to odd msc values  (Used for frame-seq. stereo field selection)
 *                              4 = Use our homegrown implementation/approximation of Adaptive Sync style flipping.
 *
 *    Return value:
 *
 *    Value greater than or equal to zero on success: The target msc for which swap is scheduled.
 *    Negative value: Error. Function failed. -1 == Function unsupported on current system configuration.
 *    -2 ... -x == Error condition.
 *
 */
psych_int64 PsychOSScheduleFlipWindowBuffers(PsychWindowRecordType *windowRecord, double tWhen, psych_int64 targetMSC, psych_int64 divisor, psych_int64 remainder, unsigned int specialFlags)
{
    psych_int64 ust, msc, sbc, rc;
    double tNow, tMsc;

    // Linux X11: If we opted into use of client-compositor sync for timing and
    // timestamping on this windowRecord, then we must -1 no-op here, so the
    // timed wait + PsychOSFlipWindowBuffers() fallback path is used, and it
    // triggers presentation via a glXSwapBuffers() call (to finish rendering,
    // and submit the new content to the compositors offscreen redirection surface),
    // wrapped into proper XSync syncCounter[1] increments to trigger the actual
    // composition pass and pageflip present, and emission or proper ClientMessage
    // messages on present completion:
    if (windowRecord->targetSpecific.syncCounter[1]) {
        // Compositor active and extended NetWM sync in use.

        // "NVidia Optimus" render offload with proprietary NVidia driver + GLX?
        // Then we lack support for all needed extensions for something clever,
        // no OpenML, no SGI_video_sync + MMIO beamposition queries because the
        // display gpu is not the NVidia dGPU. Ergo, do something simple and hope
        // it is good enough for many common cases. Translate requested tWhen into
        // a time delta since the end of the last known vblank interval, round it
        // suitably to a time delta to wait - and then wait - so we come back and
        // submit the request to the compositor exactly at the end of the vblank
        // before the vblank of wanted stimlus onset, minus some small fudge factor,
        // so our request hopefully happens shortly before the 2 msec composition
        // deadline and our content gets latched for the right composition + present
        // cycle:
        if (windowRecord->hybridGraphics == 5) {
            tMsc = windowRecord->time_at_last_vbl;
            if (tMsc <= 0)
                return(-1);

            if (!windowRecord->PipelineFlushDone)
                glFinish();

            windowRecord->PipelineFlushDone = TRUE;

            // Translate tWhen to targetMSC:
            if ((targetMSC == 0) && (tWhen != DBL_MAX))
                targetMSC = ((psych_int64)(floor((tWhen - tMsc) / windowRecord->VideoRefreshInterval) + 1));

            // Clamp to minimum of 1 vblank:
            if (targetMSC <= 0)
                targetMSC = 1;

            // Wait until vblank before the targer vblank ie. targetMSC - 1:
            if (PsychPrefStateGet_Verbosity() > 11)
                printf("PTB-DEBUG:PsychOSScheduleFlipWindowBuffers: NetWM PsychWaitUntilSeconds for %lld vblanks.\n", targetMSC - 1);

            // Translate targetMSC back into absolute system time when the vblank
            // preceeding the target vblank of stimulus onset ends. From there it
            // is about 2 msecs until the deadline for compositor content submission:
            tMsc += (targetMSC - 1) * windowRecord->VideoRefreshInterval;

            // Subtract a 0.5 msecs of fudge to compensate for scheduling/wakeup delay:
            tMsc -= 0.0005;

            // Wait until then (tMsc), compute wakeup delay for diagnostics:
            tMsc = PsychWaitUntilSeconds(tMsc) - tMsc;

            // Use NetWM compositor sync. Submit present to compositor:
            if (!PsychOSNetWMFlip(windowRecord))
                return(-3);

            if ((tMsc > 0.001) && (PsychPrefStateGet_Verbosity() > 11))
                printf("PTB-DEBUG:PsychOSScheduleFlipWindowBuffers: NetWM PsychWaitUntilSeconds delayed! Delay %f usecs.\n", tMsc * 1e6);

            // Assign target sbc of swap completion:
            rc = windowRecord->target_sbc + 1;

            // Done. Take common path out below:
            goto scheduleswapfinish;
        }

        // NVidia single-gpu with proprietary driver windowed/composited, without OpenML?
        if (!(windowRecord->gfxcaps & kPsychGfxCapSupportsOpenML) && (NULL != glXGetVideoSyncSGI)) {
            unsigned int vsync_counter = 0;

            // Try to use SGI_video_sync trickery, which is supposedly supported
            // on the proprietary driver in single-gpu configs.

            // Retrieve absolute system time of end of current/last vblank, or -1 on failure/unsupported:
            tMsc = PsychGetVblankTimestamps(windowRecord, NULL);

            // Enable rendering context of window:
            PsychSetGLContext(windowRecord);

            // Retrieve absolute count of vblanks since startup:
            PsychLockDisplay();
            glXGetVideoSyncSGI(&vsync_counter);
            msc = (psych_uint64) vsync_counter;

            // Failed?
            if (tMsc < 0 || msc == 0) {
                PsychUnlockDisplay();
                return(-1);
            }

            if (!windowRecord->PipelineFlushDone)
                glFinish();

            windowRecord->PipelineFlushDone = TRUE;

            if ((targetMSC == 0) && (tWhen != DBL_MAX)) {
                targetMSC = msc + ((psych_int64)(floor((tWhen - tMsc) / windowRecord->VideoRefreshInterval) + 1));
                if (windowRecord->vSynced && (targetMSC <= msc)) targetMSC = msc + 1;
            }

            if (targetMSC <= 0)
                targetMSC = 1;

            // Wait until vblank before the targer vblank ie. targetMSC - 1:
            if (PsychPrefStateGet_Verbosity() > 11) printf("PTB-DEBUG:PsychOSScheduleFlipWindowBuffers: NetWM glXWaitVideoSyncSGI until msc = %lld, now msc = %lld.\n", targetMSC - 1, msc);

            while ((vsync_counter < (unsigned int) (targetMSC - 1)) && (0 == glXWaitVideoSyncSGI(vsync_counter + 1, 0, &vsync_counter))) {
                if (PsychPrefStateGet_Verbosity() > 11) printf("PTB-DEBUG:PsychOSScheduleFlipWindowBuffers: NetWM glXWaitVideoSyncSGI: vsync_counter = %u.\n", vsync_counter);
            }

            // Use NetWM compositor sync. Submit present to compositor:
            if ((vsync_counter < (unsigned int) (targetMSC - 1)) || !PsychOSNetWMFlip(windowRecord)) {
                PsychUnlockDisplay();
                return(-3);
            }

            PsychUnlockDisplay();

            // Assign target sbc of swap completion:
            rc = windowRecord->target_sbc + 1;

            // Done. Take common path out below:
            goto scheduleswapfinish;
        }
    }

    // Linux: If this is implemented then it is implemented via the OpenML OML_sync_control extension.
    // Is the extension supported by the system and enabled by Psychtoolbox? If not, we return
    // a "not-supported" status code of -1 and turn into a no-op:
    if (!(windowRecord->gfxcaps & kPsychGfxCapSupportsOpenML))
        return(-1);

    // Extension supported and enabled. Use it.
    #ifdef GLX_OML_sync_control

    // Enable rendering context of window:
    PsychSetGLContext(windowRecord);

    // According to OpenML spec, a glFlush() isn't implicitely performed by
    // glXSwapBuffersMscOML(). Therefore need to do it ourselves, although
    // some implementations may do it anyway:
    if (!windowRecord->PipelineFlushDone) {
        // For our own "FreeSync" style flip implementation, we need to make sure
        // that the backbuffer is fully finished rendering and swap ready before
        // we go through the whole procedure:
        if ((specialFlags & 0x4) && (windowRecord->vrrMode > kPsychVRROff))
            // "FreeSync" style: Wait for render-completion aka swap ready:
            glFinish();
        else
            // Standard style: Flushing is enough, the OS does the rest:
            glFlush();
    }
    windowRecord->PipelineFlushDone = TRUE;

    // Non-Zero targetMSC provided to directy specify the msc on which swap should happen?
    // If so, then we can skip computation and directly call with that targetMSC:
    if ((targetMSC == 0) && (tWhen != DBL_MAX)) {
        // No: targetMSC shall be computed from given valid tWhen system target time.

        // Target time in the past? If so, advance it to "now", so we always lock onto
        // future vblanks. Why? Because the Intel ddx (as of v2.99.917) tries to optimize
        // bufferswaps away if their targetMSC is in the past! It turns them into buffer
        // exchanges (GLX_EXCHANGE_COMPLETE_INTEL style) that seem to have completed at
        // the most recent *past* vblank. Naturally this triggers all kinds our consistency
        // checks and makes PTB very angry. Now this only happens on DRI2 when the driver
        // is set to triple-buffered mode, which is unfortunately the default setting.
        // Switching triple-buffering off reliably solves all problems and all is good.
        // However, there are certain special usage scenarios for PTB in which we would love
        // to take advantage of Intels DRI2 triple-buffering, so we try to accomodate the driver
        // and help it to do the right thing for us even under triple-buffering. Easiest no-impact
        // way to do it is to correct and target times from the past into target times for next
        // vblank:
        PsychGetAdjustedPrecisionTimerSeconds(&tNow);
        if (windowRecord->vSynced && (tWhen < tNow)) tWhen = tNow;

        // Get current (msc,ust) reference values for computation.
        // Get current values for (msc, ust, sbc) the textbook way: Return error code -2 on failure:
        PsychLockDisplay();
        if (!glXGetSyncValuesOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, &ust, &msc, &sbc)) {
            PsychUnlockDisplay();
            return(-2);
        }

        // glXGetSyncValuesOML() known to return totally bogus ust timestamps? Or ust <= 0 returned,
        // which means a temporary (EAGAIN style) failure?
        if ((windowRecord->specialflags & kPsychNeedOpenMLWorkaround1) || (ust <= 0)) {
            // Useless ust returned. We need to recover a useable reference (msc, ust) pair via
            // trickery instead. Check if tWhen is at least 4 video refresh cycles in the future.
            if ((ust <= 0) && (PsychPrefStateGet_Verbosity() > 11)) printf("PTB-DEBUG:PsychOSScheduleFlipWindowBuffers: Invalid ust %lld returned by query. Current msc = %lld.\n", ust, msc);

            PsychGetAdjustedPrecisionTimerSeconds(&tNow);
            if (((tWhen - tNow) / windowRecord->VideoRefreshInterval) > 4.0) {
                // Yes. We have some time until deadline. Use a blocking wait for at
                // least 2 video refresh cycles. glXWaitForMscOML() doesn't have known
                // issues iff it has to wait for a msc that is in the future, ie., it has
                // to perform a blocking wait. In that case it will return a valid (msc, ust)
                // pair on return from blocking wait. Wait until msc+2 is reached and retrieve
                // updated (msc, ust):
                if (PsychPrefStateGet_Verbosity() > 11) printf("PTB-DEBUG:PsychOSScheduleFlipWindowBuffers: glXWaitForMscOML until msc = %lld, now msc = %lld.\n", msc + 2, msc);
                if (!glXWaitForMscOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, msc + 2, 0, 0, &ust, &msc, &sbc)) {
                    PsychUnlockDisplay();
                    return(-3);
                }
            }
            else {
                // No. Swap deadline is too close to current time. We have no option other than
                // simply using the last cached values in our windowRecord and hoping that they
                // will be good enough:
                ust = windowRecord->reference_ust;
                msc = windowRecord->reference_msc;
                sbc = windowRecord->reference_sbc;
            }
        }

        // Have a valid (ust, msc) baseline. Store it in windowRecord for future use:
        windowRecord->reference_ust = ust;
        windowRecord->reference_msc = msc;
        windowRecord->reference_sbc = sbc;

        // Compute targetMSC for given baseline and target time tWhen:
        tMsc = PsychOSMonotonicToRefTime(((double) ust) / PsychGetKernelTimebaseFrequencyHz());
        targetMSC = msc + ((psych_int64)(floor((tWhen - tMsc) / windowRecord->VideoRefreshInterval) + 1));
        if (windowRecord->vSynced && (targetMSC <= msc)) targetMSC = msc + 1;

        // If Linux builtin VRR is active atm. then we return the "unsupported" status code -1
        // to let conventional swap scheduling handle this in the fallback path:
        if (PsychVRRActive(windowRecord)) {
            PsychUnlockDisplay();
            return(-1);
        }

        /* DISABLED: This was a neat idea that never worked reliably on any digital display, and sadly
                     not at all on any tested analog VGA CRT monitor. We don't delete the PsychOSScheduleSoftSyncFlip()
                     implementation yet, because some of its ideas might be actually useful for VRR soon...
        // Our homegrown "FreeSync" style emulation wanted?
        if ((specialFlags & 0x4) && (windowRecord->vrrMode > kPsychVRROff) && windowRecord->vSynced) {
            // Yes. Check if emulation is possible and do it, if possible:
            rc = PsychOSScheduleSoftSyncFlip(windowRecord, tWhen, targetMSC);

            // Successfully executed? Or catastrophic failure? Then we are done.
            if (rc >= 0 || rc == -4) {
                PsychUnlockDisplay();
                return(rc);
            }

            // Unsupported or impossible. Fall back to standard swap:
            if (PsychPrefStateGet_Verbosity() > 10)
                printf("PTB-DEBUG:PsychOSScheduleFlipWindowBuffers: SoftSync swap unsupported or impossible atm. Fallback to standard swap.\n");
        }
        */
    }
    else {
        PsychLockDisplay();
    }

    // Clamp targetMSC to a positive non-zero value, unless special case
    // for glXSwapBuffers() semantic is requested, in which case we pass
    // the zero targetMSC on. A zero targetMSC, divisor and remainder will
    // trigger good old glXSwapBuffers() semantics:
    if ((targetMSC <= 0) && (tWhen != DBL_MAX)) targetMSC = 1;

    // Swap at specific even or odd frame requested? This is useful for frame-sequential stereo
    // presentation that shall start its presentation at a specific eye-view:
    if (specialFlags & (0x1 | 0x2)) {
        // Yes. Setup (divisor,remainder) constraint so that
        // 0x1 maps to even target frames, and 0x2 maps to odd
        // target frames:
        divisor = 2;
        remainder = (specialFlags & 0x1) ? 0 : 1;
        // Make sure initial targetMSC obeys (divisor,remainder) constraint:
        targetMSC += (targetMSC % divisor == remainder) ? 0 : 1;
    }

    if (PsychPrefStateGet_Verbosity() > 12) printf("PTB-DEBUG:PsychOSScheduleFlipWindowBuffers: Submitting swap request for targetMSC = %lld, divisor = %lld, remainder = %lld.\n", targetMSC, divisor, remainder);

    // Compositor active and extended NetWM sync in use?
    if (windowRecord->targetSpecific.syncCounter[1]) {
        // Wait until vblank before the targer vblank ie. targetMSC - 1:
        if (targetMSC == 0)
            targetMSC = 1;

        if (PsychPrefStateGet_Verbosity() > 11) printf("PTB-DEBUG:PsychOSScheduleFlipWindowBuffers: NetWM glXWaitForMscOML until msc = %lld, now msc = %lld.\n", targetMSC - 1, msc);

        if (!glXWaitForMscOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, targetMSC - 1, 0, 0, &ust, &msc, &sbc)) {
            PsychUnlockDisplay();
            return(-3);
        }

        // Use NetWM compositor sync. Submit present to compositor:
        if (!PsychOSNetWMFlip(windowRecord)) {
            PsychUnlockDisplay();
            return(-3);
        }

        // Assign target sbc of swap completion:
        rc = sbc + 1;
    }
    else {
        // Standard case: Ok, we have a valid final targetMSC. Schedule a bufferswap for that targetMSC, taking a potential
        // (divisor, remainder) constraint into account:
        rc = glXSwapBuffersMscOML(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle, targetMSC, divisor, remainder);
    }

    PsychUnlockDisplay();

    scheduleswapfinish:

    // Failed? Return -4 error code if so:
    if (rc == -1) return(-4);

    // Keep track of target_sbc and targetMSC, who knows for what they might be good for?
    windowRecord->target_sbc = rc;
    windowRecord->lastSwaptarget_msc = targetMSC;

    #else
    // No op branch in case OML_sync_control isn't enabled at compile time:
    return(-1);
    #endif

    // Successfully scheduled the swap request: Return targetMSC for which it was scheduled:
    return(targetMSC);
}

/*
 *    PsychOSFlipWindowBuffers()
 *
 *    Performs OS specific double buffer swap call.
 */
void PsychOSFlipWindowBuffers(PsychWindowRecordType *windowRecord)
{
    // Execute OS neutral bufferswap code first:
    PsychExecuteBufferSwapPrefix(windowRecord);

    // Prefer use of scheduled swap api (glXSwapBuffersMscOML()) on Linux, if supported,
    // to achieve the same effect as a simple glXSwapBuffers() call would have.
    // Mesa, for versions < 10.3.4, has a bug in its new DRI3/Present implementation of
    // glXWaitForSbcOML() in that it malfunctions with targetSBC == 0: It doesn't wait for
    // all pending swaps to complete, but falls through and returns stale values from previous
    // swaps! By using PsychOSScheduleFlipWindowBuffers() we get the proper targetSBC for this
    // swap request, so a corresponding workaround in PsychOSGetSwapCompletionTimestamp() can
    // use it to call glXWaitForSbcOML() with a proper non-zero targetSBC -- problem solved.
    // If the function is unsupported we fall back to good ol' glXSwapBuffers below.
    // This should not have any negative side effects compared to the old implementation, so
    // can be safely used on DRI2 and other graphics stacks as well.
    //
    // Note another fun-bug: A targetMSC of zero causes a hang of the bufferswap mechanism for
    // XOrg < 1.16.3 + Mesa < 10.3.4. We solve this in PsychOSScheduleFlipWindowBuffers() by
    // requesting a tWhen of 0.0 with targetMSC = 0, so the routine will try to compute true
    // targetMSC from tWhen and when detecting it is in the past, will push tWhen to "now", which
    // will schedule the swap for the targetMSC ( >> 0 ) of the next refresh cycle, at least if
    // vsync'ed swap is requested.
    //
    // Note: These bugs are fixed in current XOrg and Mesa, and distributions with older
    // XOrg/Mesa versions don't have DRI3 enabled by default, so this workaround is a bit
    // of historic value for odd cases. However, the current Intel ddx 2.99.917 creates some
    // new problems under DRI2 + triple-buffering. See PsychOSScheduleFlipWindowBuffers() for
    // why we want to work around these if possible for some performance gain...
    if (PsychOSScheduleFlipWindowBuffers(windowRecord, 0.0, 0, 0, 0, 0) >= 0) return;

    // For NVidia Optimus outputSink style Prime sync emit a "swap submitted" marker packet, so
    // our completion code in PsychOSGetSwapCompletionTimestamp can find the start of the most
    // recent swaps data in the UDP receive queue, filtering out spurious packets from page-flips
    // which were not triggered by ourselves, e.g., instead triggered by mouse cursor visual
    // updates (NVidia proprietary driver uses a SW cursor on Optimus setups):
    if (prime_sockfd2[PsychGetXScreenIdForScreen(windowRecord->screenNumber)] > 0) {
        struct _buf buf;
        buf.frame = 0;
        buf.usec = 0;
        buf.scrnIndex = PsychGetXScreenIdForScreen(windowRecord->screenNumber);
        buf.flags = 100;
        if (send(prime_sockfd2[buf.scrnIndex], &buf, sizeof(buf), MSG_DONTWAIT) != sizeof(buf))
            printf("PTB-ERROR:PsychOSFlipWindowBuffers: Failed to send UDP swap fence for screen %i.\n", windowRecord->screenNumber);
    }

    // Trigger the "Front <-> Back buffer swap (flip) (on next vertical retrace)":
    PsychLockDisplay();
    if (windowRecord->targetSpecific.syncCounter[1]) {
        // Use NetWM compositor sync. Submit present to compositor:
        PsychOSNetWMFlip(windowRecord);
    }
    else {
        // Standard path, a simple swapbuffers, hopefully with pageflipping for good timing:
        glXSwapBuffers(windowRecord->targetSpecific.privDpy, windowRecord->targetSpecific.windowHandle);
    }
    PsychUnlockDisplay();

    windowRecord->target_sbc = (windowRecord->hybridGraphics == 5) ? (windowRecord->target_sbc + 1): 0;
    windowRecord->lastSwaptarget_msc = 0;
}

/* Enable/disable syncing of buffer-swaps to vertical retrace. */
void PsychOSSetVBLSyncLevel(PsychWindowRecordType *windowRecord, int swapInterval)
{
    int error, myinterval = -1000;

    // Sync counter available means desktop compositor via NetWM in use, so disable actual
    // vsync for the copy-swap from onscreen window backbuffer to redirection surface Pixmap:
    if (windowRecord->targetSpecific.syncCounter[1])
        swapInterval = 0;

    // Most efficient and cross-vendor GLX_EXT_swap_control supported in Mesa since v20.3.0 from December 2020, NVidia/ATI proprietary since at least year 2013:
    if (glxewIsSupported("GLX_EXT_swap_control")) {
        PsychLockDisplay();
        glXSwapIntervalEXT(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle, swapInterval);
        glXQueryDrawable(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle, GLX_SWAP_INTERVAL_EXT, (unsigned int*) &myinterval);
        PsychUnlockDisplay();
    }
    else if (glxewIsSupported("GLX_MESA_swap_control")) {
        // Need rendering context of window:
        PsychSetGLContext(windowRecord);
        PsychLockDisplay();
        error = glXSwapIntervalMESA(swapInterval);
        myinterval = glXGetSwapIntervalMESA();
        PsychUnlockDisplay();

        if (error && (PsychPrefStateGet_Verbosity() > 1))
            printf("\nPTB-WARNING: glXSwapIntervalMESA() FAILED to %s synchronization to vertical retrace! Error=%i\n", (swapInterval > 0) ? "enable" : "disable", error);
    }
    else if (glxewIsSupported("GLX_SGI_swap_control")) {
        // Hopefully never hit this ancient GLX_SGI_swap_control thing anymore: Does not allow disable of vsync or query of actual setting.
        // Need rendering context of window:
        PsychSetGLContext(windowRecord);
        PsychLockDisplay();
        error = glXSwapIntervalSGI(swapInterval);
        PsychUnlockDisplay();

        if (error == 0)
            myinterval = swapInterval;

        if (error && (PsychPrefStateGet_Verbosity() > 1))
            printf("\nPTB-WARNING: glXSwapIntervalSGI() FAILED to %s synchronization to vertical retrace! Error=%i\n", (swapInterval > 0) ? "enable" : "disable", error);
    }

    if (myinterval != -1000) {
        // Store new setting also in internal helper variable, e.g., to allow workarounds to work:
        windowRecord->vSynced = (myinterval > 0) ? TRUE : FALSE;

        // Did the system accept our settings?
        if ((myinterval != swapInterval) && (PsychPrefStateGet_Verbosity() > 1))
            printf("\nPTB-WARNING: FAILED to %s synchronization to vertical retrace (System ignored setting [Req %i != Actual %i])!\n\n",
                  (swapInterval > 0) ? "enable" : "disable", swapInterval, myinterval);
    }

    if (PsychPrefStateGet_Verbosity() > 10)
        printf("PTB-DEBUG: PsychOSSetVBLSyncLevel(win %i, interval %i) => New interval %i => vsync %i\n", windowRecord->windowIndex, swapInterval, myinterval, windowRecord->vSynced);
}

/*
 *    PsychOSSetGLContext()
 *
 *    Set the window to which GL drawing commands are sent.
 */
void PsychOSSetGLContext(PsychWindowRecordType *windowRecord)
{
    // glXGetCurrentContext() should be safe to use unlocked, as it accesses thread
    // local state:
    if (glXGetCurrentContext() != windowRecord->targetSpecific.contextObject) {
        if (glXGetCurrentContext() != NULL) {
            // We need to glFlush the context before switching, otherwise race-conditions may occur:
            glFlush();

            // Need to unbind any FBO's in old context before switch, otherwise bad things can happen...
            if (glBindFramebufferEXT) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        }

        // Switch to new context:
        PsychLockDisplay();
        glXMakeCurrent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle, windowRecord->targetSpecific.contextObject);
        PsychUnlockDisplay();
    }
}

/*
 *    PsychOSUnsetGLContext()
 *
 *    Clear the drawing context.
 */
void PsychOSUnsetGLContext(PsychWindowRecordType* windowRecord)
{
    // Any context set on current thread?
    if (glXGetCurrentContext() != NULL) {
        // We need to glFlush the context before switching, otherwise race-conditions may occur:
        glFlush();

        // Need to unbind any FBO's in old context before switch, otherwise bad things can happen...
        if (glBindFramebufferEXT) glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

        // Skip the unbind on buggy amdgpu-pro driver. As of April 2017 (v17.10.2), we crash
        // deep inside amdgpu-pro's proprietary libGL. So skip this and hope for the best...
        if (strcmp(glGetString(GL_VENDOR), "ATI Technologies Inc.")) {
            PsychLockDisplay();
            glXMakeCurrent(windowRecord->targetSpecific.deviceContext, None, NULL);
            PsychUnlockDisplay();
        }
        else {
            if (PsychPrefStateGet_Verbosity() > 1)
                printf("PTB-WARNING: Skipping context detach in PsychOSUnsetGLContext() to work around buggy amdgpu-pro OpenGL driver!\n");
        }
    }
}

/* Same as PsychOSSetGLContext() but for selecting userspace rendering context,
 * optionally copying state from PTBs context.
 */
void PsychOSSetUserGLContext(PsychWindowRecordType *windowRecord, psych_bool copyfromPTBContext)
{
    // Child protection:
    if (windowRecord->targetSpecific.glusercontextObject == NULL) PsychErrorExitMsg(PsychError_user,"GL Userspace context unavailable! Call InitializeMatlabOpenGL *before* Screen('OpenWindow')!");

    if (copyfromPTBContext) {
        // This unbind is probably not needed on X11/GLX, but better safe than sorry...
        PsychLockDisplay();
        glXMakeCurrent(windowRecord->targetSpecific.deviceContext, None, NULL);

        // Copy render context state:
        glXCopyContext(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.contextObject, windowRecord->targetSpecific.glusercontextObject, GL_ALL_ATTRIB_BITS);
        PsychUnlockDisplay();
    }

    // Setup new context if it isn't already setup. -> Avoid redundant context switch.
    if (glXGetCurrentContext() != windowRecord->targetSpecific.glusercontextObject) {
        PsychLockDisplay();
        glXMakeCurrent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle, windowRecord->targetSpecific.glusercontextObject);
        PsychUnlockDisplay();
    }
}

/* PsychOSSetupFrameLock - Check if framelock / swaplock support is available on
 * the given graphics system implementation and try to enable it for the given
 * pair of onscreen windows.
 *
 * If possible, will try to add slaveWindow to the swap group and/or swap barrier
 * of which masterWindow is already a member, putting slaveWindow into a swap-lock
 * with the masterWindow. If masterWindow isn't yet part of a swap group, create a
 * new swap group and attach masterWindow to it, before joining slaveWindow into the
 * new group. If masterWindow is part of a swap group and slaveWindow is NULL, then
 * remove masterWindow from the swap group.
 *
 * The swap lock mechanism used is operating system and GPU dependent. Many systems
 * will not support framelock/swaplock at all.
 *
 * Returns TRUE on success, FALSE on failure.
 */
psych_bool PsychOSSetupFrameLock(PsychWindowRecordType *masterWindow, PsychWindowRecordType *slaveWindow)
{
    GLuint maxGroups, maxBarriers, targetGroup;
    psych_bool rc = FALSE;

    // GNU/Linux: Try NV_swap_group support first, then SGI swap group support.
    PsychLockDisplay();

    // NVidia swap group extension supported?
    if ((glxewIsSupported("GLX_NV_swap_group") || glewIsSupported("GLX_NV_swap_group")) && (NULL != glXQueryMaxSwapGroupsNV)) {
        // Yes. Check if given GPU really supports it:
        if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: NV_swap_group supported. Querying available groups...\n");

        if (glXQueryMaxSwapGroupsNV(masterWindow->targetSpecific.deviceContext, PsychGetXScreenIdForScreen(masterWindow->screenNumber), &maxGroups, &maxBarriers) && (maxGroups > 0)) {
            // Yes. What to do?
            if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: NV_swap_group supported. Implementation supports up to %i swap groups. Trying to join or unjoin group.\n", maxGroups);

            if (NULL == slaveWindow) {
                // Asked to remove master from swap group:
                glXJoinSwapGroupNV(masterWindow->targetSpecific.deviceContext, masterWindow->targetSpecific.windowHandle, 0);
                masterWindow->swapGroup = 0;
                PsychUnlockDisplay();
                return(TRUE);
            }
            else {
                // Non-NULL slaveWindow: Shall attach to swap group.
                // Master already part of a swap group?
                if (0 == masterWindow->swapGroup) {
                    // Nope. Try to attach it to first available one:
                    targetGroup = (GLuint) PsychFindFreeSwapGroupId(maxGroups);

                    if ((targetGroup == 0) || !glXJoinSwapGroupNV(masterWindow->targetSpecific.deviceContext, masterWindow->targetSpecific.windowHandle, targetGroup)) {
                        // Failed!
                        if (PsychPrefStateGet_Verbosity() > 1) {
                            printf("PTB-WARNING: Tried to enable framelock support for master-slave window pair, but masterWindow failed to join swapgroup %i! Skipped.\n", targetGroup);
                        }

                        goto try_sgi_swapgroup;
                    }

                    // Sucess for master!
                    masterWindow->swapGroup = targetGroup;
                }

                // Now try to join the masters swapgroup with the slave:
                if (!glXJoinSwapGroupNV(slaveWindow->targetSpecific.deviceContext, slaveWindow->targetSpecific.windowHandle,  masterWindow->swapGroup)) {
                    // Failed!
                    if (PsychPrefStateGet_Verbosity() > 1) {
                        printf("PTB-WARNING: Tried to enable framelock support for master-slave window pair, but slaveWindow failed to join swapgroup %i of master! Skipped.\n", masterWindow->swapGroup);
                    }

                    goto try_sgi_swapgroup;
                }

                // Success! Now both windows are in a common swapgroup and framelock should work!
                slaveWindow->swapGroup = masterWindow->swapGroup;

                if (PsychPrefStateGet_Verbosity() > 1) {
                    printf("PTB-INFO: Framelock support for master-slave window pair via NV_swap_group extension enabled! Joined swap group %i.\n", masterWindow->swapGroup);
                }

                PsychUnlockDisplay();
                return(TRUE);
            }
        }
    }

    // If we reach this point, then NV_swap groups are unsupported, or setup failed.
    try_sgi_swapgroup:

    // Try if we have more luck with SGIX_swap_group extension:
    if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: NV_swap_group unsupported or join operation failed. Trying GLX_SGIX_swap_group support...\n");

    // SGIX swap group extension supported?
    if((glxewIsSupported("GLX_SGIX_swap_group") || glewIsSupported("GLX_SGIX_swap_group")) && (NULL != glXJoinSwapGroupSGIX)) {
        // Yes. What to do?
        if (NULL == slaveWindow) {
            // Asked to remove master from swap group:
            glXJoinSwapGroupSGIX(masterWindow->targetSpecific.deviceContext, masterWindow->targetSpecific.windowHandle, None);
            masterWindow->swapGroup = 0;
            PsychUnlockDisplay();
            return(TRUE);
        }
        else {
            // Non-NULL slaveWindow: Shall attach to swap group.

            // Sucess for master by definition. Master is member of its own swapgroup, obviously...
            masterWindow->swapGroup = 1;

            // Now try to join the masters swapgroup with the slave. This can't fail in a non-fatal way.
            // Either it succeeds, or the whole runtime will abort with some GLX command stream error :-I
            glXJoinSwapGroupSGIX(slaveWindow->targetSpecific.deviceContext, slaveWindow->targetSpecific.windowHandle,  masterWindow->targetSpecific.windowHandle);

            // Success! Now both windows are in a common swapgroup and framelock should work!
            slaveWindow->swapGroup = masterWindow->swapGroup;

            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-INFO: Framelock support for master-slave window pair via GLX_SGIX_swap_group extension enabled!\n");
            }

            PsychUnlockDisplay();
            return(TRUE);
        }
    }

    PsychUnlockDisplay();

    if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: NV_swap_group and GLX_SGIX_swap_group unsupported or join operations failed.\n");

    return(rc);
}

psych_bool PsychOSSwapCompletionLogging(PsychWindowRecordType *windowRecord, int cmd, int aux1)
{
    const char *FieldNames[] = { "OnsetTime", "OnsetVBLCount", "SwapbuffersCount", "SwapType", "BackendFeedbackString" };
    const int  fieldCount = 5;
    PsychGenericScriptType	*s;
    unsigned long glxmask = 0;
    XEvent evt;
    int scrnum;
    int event_type;

    // Invalidate stored swap completion type for this window:
    windowRecord->swapcompletiontype = 0;

    if (cmd == 0 || cmd == 1 || cmd == 2) {
        // Check if GLX_INTEL_swap_event extension is supported. Enable/Disable swap completion event
        // delivery for our window, if so:
        // We enable if override env var "PSYCH_FORCE_INTEL_swap_event" is set, or if the extension is
        // in the glXQueryExtensionsString() or it is in both the server- and client-extension string.
        // Update November 2014: Under DRI3/Present + GLAMOR acceleration, e.g., with nouveau-ddx, it
        // is possible that the extension is only advertised in the client string, not the server or
        // unified string, but the extension still works. Therefore we are now more lenient and enable
        // the extension if it is advertised in any of the query strings. Code will still detect missing
        // extension support and fallback to the right thing if this doesn't work out, so all should be good.
        scrnum = PsychGetXScreenIdForScreen(windowRecord->screenNumber);
        PsychLockDisplay();
        if (useGLX13 && (strstr(glXQueryExtensionsString(windowRecord->targetSpecific.deviceContext, scrnum), "GLX_INTEL_swap_event") || getenv("PSYCH_FORCE_INTEL_swap_event") ||
            strstr(glXGetClientString(windowRecord->targetSpecific.deviceContext, GLX_EXTENSIONS), "GLX_INTEL_swap_event") ||
            strstr(glXQueryServerString(windowRecord->targetSpecific.deviceContext, scrnum, GLX_EXTENSIONS), "GLX_INTEL_swap_event"))) {

            // Always enable the swap event delivery, either to us or to user code:
            glXSelectEvent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle, (unsigned long) GLX_BUFFER_SWAP_COMPLETE_INTEL_MASK);
            glXGetSelectedEvent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle, &glxmask);
            PsychUnlockDisplay();

            if (glxmask & GLX_BUFFER_SWAP_COMPLETE_INTEL_MASK) {
                // Logical enable state: Usercode has precedence. If it enables it goes to it. If it disabled,
                // it gets directed to us:
                if (cmd == 0 || cmd == 1) windowRecord->swapevents_enabled = (cmd == 1) ? 1 : 2;

                // If we want the data and usercode doesn't have exclusive access to it already, then redirect to us:
                if (cmd == 2 && (windowRecord->swapevents_enabled != 1)) windowRecord->swapevents_enabled = 2;

                return(TRUE);
            }
            else {
                windowRecord->swapevents_enabled = 0;

                if (PsychPrefStateGet_Verbosity() > 1)
                    printf("PTB-WARNING: Enabling INTEL_swap_event support failed!\n");

                return(FALSE);
            }
        } else {
            // Failed to enable swap events, possibly because they're unsupported:
            windowRecord->swapevents_enabled = 0;
            PsychUnlockDisplay();
            return(FALSE);
        }
    }

    if (cmd == 3 || 4) {
        // Support for INTEL_swap_event extension enabled? Process swap events if so:
        if (useGLX13) {
            PsychLockDisplay();
            glXGetSelectedEvent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.windowHandle, &glxmask);
            PsychUnlockDisplay();

            if (glxmask & GLX_BUFFER_SWAP_COMPLETE_INTEL_MASK) {
                // INTEL_swap_event delivery enabled and requested.

                // Delivery to user-code?
                if (cmd == 3 && windowRecord->swapevents_enabled == 1) {
                    double *dummy;

                    // Try to fetch oldest pending one for this window:
                    PsychLockDisplay();
                    if (XCheckTypedWindowEvent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.xwindowHandle,
                        glx_event_base + GLX_BufferSwapComplete, &evt)) {
                        PsychUnlockDisplay();
                        // Cast to proper event type:
                        GLXBufferSwapComplete *sce = (GLXBufferSwapComplete*) &evt;
                        if (PsychPrefStateGet_Verbosity() > 5) {
                            printf("SWAPEVENT: OurWin=%i ust = %lld, msc = %lld, sbc = %lld, type %s.\n", (int) (sce->drawable == windowRecord->targetSpecific.xwindowHandle),
                                   sce->ust, sce->msc, sce->sbc, (sce->event_type == GLX_FLIP_COMPLETE_INTEL) ? "PAGEFLIP" : (sce->event_type == GLX_COPY_COMPLETE_INTEL) ? "BLIT" : "EXCHANGE");
                        }

                        PsychAllocOutStructArray(aux1, FALSE, -1, fieldCount, FieldNames, &s);
                        PsychSetStructArrayDoubleElement("OnsetTime", 0, PsychOSMonotonicToRefTime(((double) sce->ust) / PsychGetKernelTimebaseFrequencyHz()), s);
                        PsychSetStructArrayDoubleElement("OnsetVBLCount", 0, (double) sce->msc, s);
                        PsychSetStructArrayDoubleElement("SwapbuffersCount", 0, (double) sce->sbc, s);
                        switch (sce->event_type) {
                            case GLX_FLIP_COMPLETE_INTEL:
                                PsychSetStructArrayStringElement("SwapType", 0, "Pageflip", s);
                                PsychSetStructArrayStringElement("BackendFeedbackString", 0, "?cez", s);
                                break;

                            case GLX_EXCHANGE_COMPLETE_INTEL:
                                PsychSetStructArrayStringElement("SwapType", 0, "Exchange", s);
                                PsychSetStructArrayStringElement("BackendFeedbackString", 0, "___z", s);
                                break;

                            case GLX_COPY_COMPLETE_INTEL:
                                PsychSetStructArrayStringElement("SwapType", 0, "Copy", s);
                                PsychSetStructArrayStringElement("BackendFeedbackString", 0, "?c__", s);
                                break;

                            default:
                                PsychSetStructArrayStringElement("SwapType", 0, "Unknown", s);
                                PsychSetStructArrayStringElement("BackendFeedbackString", 0, "", s);
                        }

                        return(TRUE);
                    }
                    PsychUnlockDisplay();

                    // Return empty [] argument, so client code can detect "no new result":
                    PsychAllocOutDoubleMatArg(aux1, FALSE, 0, 0, 0, &dummy);
                }

                // Delivery to internal code "us"?
                if (cmd == 4 && windowRecord->swapevents_enabled == 2) {
                    // Get the most recent event in the queue, old ones are not interesting to us atm.:
                    event_type = 0; // Init to "undefined"

                    // Fetch until exhausted:
                    PsychLockDisplay();
                    while (XCheckTypedWindowEvent(windowRecord->targetSpecific.deviceContext, windowRecord->targetSpecific.xwindowHandle, glx_event_base + GLX_BufferSwapComplete, &evt)) {
                        // Cast to proper event type:
                        GLXBufferSwapComplete *sce = (GLXBufferSwapComplete*) &evt;
                        if (PsychPrefStateGet_Verbosity() > 10) {
                            printf("SWAPEVENT: OurWin=%i ust = %lld, msc = %lld, sbc = %lld, type %s.\n", (int) (sce->drawable == windowRecord->targetSpecific.xwindowHandle),
                                   sce->ust, sce->msc, sce->sbc, (sce->event_type == GLX_FLIP_COMPLETE_INTEL) ? "PAGEFLIP" : (sce->event_type == GLX_COPY_COMPLETE_INTEL) ? "BLIT" : "EXCHANGE");
                        }

                        // Assign the one that matches our last 'sbc' for swap completion on our windowRecord:
                        if ((sce->drawable == windowRecord->targetSpecific.xwindowHandle) && (((int) sce->sbc) == aux1)) event_type = sce->event_type;
                    }
                    PsychUnlockDisplay();

                    // event_type is either zero if nothing fetched, or the swap type of the most
                    // recent bufferswap:
                    switch (event_type) {
                        case GLX_FLIP_COMPLETE_INTEL:
                            windowRecord->swapcompletiontype = 1;
                            break;

                        case GLX_EXCHANGE_COMPLETE_INTEL:
                            windowRecord->swapcompletiontype = 2;
                            break;

                        case GLX_COPY_COMPLETE_INTEL:
                            windowRecord->swapcompletiontype = 3;
                            break;

                        default:
                            windowRecord->swapcompletiontype = 0;
                            return(FALSE);
                    }

                    return(TRUE);
                }
            }
        }
    }

    // Invalid cmd or failed cmd:
    return(FALSE);
}

/* PsychOSAdjustForCompositorDelay()
 *
 * Compute OS and desktop compositor specific delay that needs to be subtracted from the
 * target time for a OpenGL doublebuffer swap when conventional swap scheduling is used.
 * Subtract the delay, if any, from the given targetTime and return the corrected targetTime.
 *
 */
double PsychOSAdjustForCompositorDelay(PsychWindowRecordType *windowRecord, double targetTime, psych_bool onlyForCalibration)
{
    // Nothing to do for classic X11/GLX. Just return identity. However, if this
    // is a NVidia Optimus Laptop with Intel iGPU + NVidia dGPU and the proprietary
    // NVidia binary graphics driver is used for output source -> output sink PRIME
    // mode, then we have to work around a NVidia oddity: There is always 1 frame extra
    // delay after a bufferswap request until flip at minimum. Ergo, subtract one video
    // refresh duration from the target time to Increase our chance of hitting the proper
    // target frame. This as of the design of driver version 370.23 with XOrg 1.19-rc1.
    //
    // For the new Optimus PRIME renderoffload implementation, we also get 1 frame delay if
    // we must go through the desktop compositor, so try to compensate for that. Going through
    // the compositor is needed if the iGPU can't pageflip/scanout pixmap buffers directly from
    // the dGPU, ie. scanout from GTT memory RAM aperture. This is the case for AMD dGPU's, old
    // AMD iGPU's pre-Stoney, AMD RavenRidge initial revisions, and whenever the renderoffload
    // gpu is not an AMD gpu driven by amdgpu-kms. The latter limitation is a software/driver
    // limitation at least as of Linux 5.14, so it may get lifted at some time. I do have kernel
    // patches that do that in a hacky way, which may not be upstreamable...
    if (!onlyForCalibration && ((windowRecord->hybridGraphics == 2) || (windowRecord->hybridGraphics == 3) ||
        ((windowRecord->hybridGraphics == 5) && windowRecord->targetSpecific.present_notify_queue[1]))) {
        if (PsychPrefStateGet_Verbosity() > 14)
            printf("PTB-DEBUG: PsychOSAdjustForCompositorDelay: Optimus Pre-targetTime: %f secs. VideoRefreshInterval %f secs.\n",
                   targetTime, windowRecord->VideoRefreshInterval);
        targetTime -= windowRecord->VideoRefreshInterval;
    }

    return(targetTime);
}

static int getFreeBarrierSlot(PsychWindowRecordType *windowRecord, PsychRectType rect)
{
    int i;
    for (i = 0; (i < PSYCH_MAX_POINTER_BARRIERS) && (barrierParentWindows[i] != NULL); i++);
    if (i == PSYCH_MAX_POINTER_BARRIERS)
        return(-1);

    // One more barrier used up:
    barrierParentWindows[i] = windowRecord;
    PsychCopyRect(barrierRects[i], rect);
    pointerBarrierCount++;
    return(i);
}

/* PsychOSConstrainPointer()
 *
 * Establish or release pointer confinement to a rectangle, a mouse trap if you want.
 * As a Linux X11 only feature, it is also possible to define random additional horizontal
 * or vertical line segments which the pointer shall not pass.
 *
 * Returns TRUE on success, FALSE on failure, e.g., maximum number of barriers exceeded.
 */
psych_bool PsychOSConstrainPointer(PsychWindowRecordType *windowRecord, psych_bool constrain, PsychRectType rect)
{
    int i;
    CGDirectDisplayID dpy;
    PsychGetCGDisplayIDFromScreenNumber(&dpy, windowRecord->screenNumber);

    // Need XFixes version 5+, available since X-Server 1.11 (released Aug 2011):
    if (xfixes_majorversion < 5) {
        if ((PsychPrefStateGet_Verbosity() > 0) && constrain)
            printf("PTB-ERROR:PsychOSConstrainPointer: Tried to add mouse pointer barrier, but your X-Server is too old to support this - Failed! Upgrade to X-Server 1.11 or later.\n");

        return(FALSE);
    }

    if (rect) {
        // Translate from window local coordinates to X-Screen global coordinates:
        rect[kPsychLeft] += windowRecord->globalrect[kPsychLeft];
        rect[kPsychRight] += windowRecord->globalrect[kPsychLeft];
        rect[kPsychTop] += windowRecord->globalrect[kPsychTop];
        rect[kPsychBottom] += windowRecord->globalrect[kPsychTop];
    }

    // Should we create new pointer barriers?
    if (constrain) {
        // Single barrier line? An empty rect would define a line:
        if (IsPsychRectEmpty(rect)) {
            if (pointerBarrierCount + 1 > PSYCH_MAX_POINTER_BARRIERS) {
                if (PsychPrefStateGet_Verbosity() > 0)
                    printf("PTB-ERROR:PsychOSConstrainPointer: Tried to add more mouse pointer barriers than supported - Failed! The current limit is a maximum of %i barriers.\n",
                           PSYCH_MAX_POINTER_BARRIERS);
                    return(FALSE);
            }

            PsychLockDisplay();

            // Add a single line barrier:
            if ((rect[kPsychLeft] == rect[kPsychRight]) && (rect[kPsychTop] != rect[kPsychBottom])) {
                // Vertical barrier line:
                barriers[getFreeBarrierSlot(windowRecord, rect)] = XFixesCreatePointerBarrier(dpy, windowRecord->targetSpecific.xwindowHandle,
                                                                                              rect[kPsychLeft], rect[kPsychTop],
                                                                                              rect[kPsychLeft], rect[kPsychBottom],
                                                                                              0, /* block in all directions */
                                                                                              0, NULL); /* no per-device barriers */
            }
            else if ((rect[kPsychLeft] != rect[kPsychRight]) && (rect[kPsychTop] == rect[kPsychBottom])) {
                // Horizontal barrier line:
                barriers[getFreeBarrierSlot(windowRecord, rect)] = XFixesCreatePointerBarrier(dpy, windowRecord->targetSpecific.xwindowHandle,
                                                                                              rect[kPsychLeft], rect[kPsychTop],
                                                                                              rect[kPsychRight], rect[kPsychTop],
                                                                                              0, /* block in all directions */
                                                                                              0, NULL); /* no per-device barriers */
            }
            else {
                // A point: This is a special case. Create some tight barrier double-bounding-rectangles:
                PsychUnlockDisplay();

                if (pointerBarrierCount + 8 > PSYCH_MAX_POINTER_BARRIERS) {
                    if (PsychPrefStateGet_Verbosity() > 0)
                        printf("PTB-ERROR:PsychOSConstrainPointer: Tried to add more mouse pointer barriers than supported - Failed! The current limit is a maximum of %i barriers.\n",
                               PSYCH_MAX_POINTER_BARRIERS);
                        return(FALSE);
                }

                rect[kPsychLeft]--;
                rect[kPsychTop]--;
                rect[kPsychRight]--;
                rect[kPsychBottom]--;

                rect[kPsychLeft]--;
                rect[kPsychTop]--;
                rect[kPsychRight]++;
                rect[kPsychBottom]++;
                PsychOSConstrainPointer(windowRecord, TRUE, rect);
                rect[kPsychLeft]--;
                rect[kPsychTop]--;
                rect[kPsychRight]++;
                rect[kPsychBottom]++;
                PsychOSConstrainPointer(windowRecord, TRUE, rect);
                PsychLockDisplay();
            }

            XFlush(dpy);
            PsychUnlockDisplay();
        }
        else {
            // Want a barrier rectangle to constrain pointer to the inside of a box:
            if (pointerBarrierCount + 4 > PSYCH_MAX_POINTER_BARRIERS) {
                if (PsychPrefStateGet_Verbosity() > 0)
                    printf("PTB-ERROR:PsychOSConstrainPointer: Tried to add more mouse pointer barriers than supported - Failed! The current global limit is a maximum of %i barriers.\n",
                           PSYCH_MAX_POINTER_BARRIERS);
                return(FALSE);
            }

            PsychLockDisplay();

            // Barrier at left edge of rect:
            barriers[getFreeBarrierSlot(windowRecord, rect)] = XFixesCreatePointerBarrier(dpy, windowRecord->targetSpecific.xwindowHandle,
                                                                                          rect[kPsychLeft], rect[kPsychTop],
                                                                                          rect[kPsychLeft], rect[kPsychBottom],
                                                                                          0, /* block in all directions */
                                                                                          0, NULL); /* no per-device barriers */

            // Barrier at right edge of rect:
            barriers[getFreeBarrierSlot(windowRecord, rect)] = XFixesCreatePointerBarrier(dpy, windowRecord->targetSpecific.xwindowHandle,
                                                                                          rect[kPsychRight], rect[kPsychTop],
                                                                                          rect[kPsychRight], rect[kPsychBottom],
                                                                                          0, /* block in all directions */
                                                                                          0, NULL); /* no per-device barriers */

            // Barrier at top edge of rect:
            barriers[getFreeBarrierSlot(windowRecord, rect)] = XFixesCreatePointerBarrier(dpy, windowRecord->targetSpecific.xwindowHandle,
                                                                                          rect[kPsychLeft], rect[kPsychTop],
                                                                                          rect[kPsychRight], rect[kPsychTop],
                                                                                          0, /* block in all directions */
                                                                                          0, NULL); /* no per-device barriers */

            // Barrier at bottom edge of rect:
            barriers[getFreeBarrierSlot(windowRecord, rect)] = XFixesCreatePointerBarrier(dpy, windowRecord->targetSpecific.xwindowHandle,
                                                                                          rect[kPsychLeft], rect[kPsychBottom],
                                                                                          rect[kPsychRight], rect[kPsychBottom],
                                                                                          0, /* block in all directions */
                                                                                          0, NULL); /* no per-device barriers */
            XFlush(dpy);
            PsychUnlockDisplay();
        }

        if (PsychPrefStateGet_Verbosity() > 5)
            printf("PTB-DEBUG: Adding pointer barrier for window %i with defining rect [%i, %i, %i, %i]\n",
                   windowRecord->windowIndex, (int) rect[kPsychLeft], (int) rect[kPsychTop], (int) rect[kPsychRight], (int) rect[kPsychBottom]);
    }
    else {
        // Release all pointer barriers for the window if rect == NULL,
        // otherwise release the barrier(s) for the window which are matching the given rect.
        if (PsychPrefStateGet_Verbosity() > 5) {
            if (rect == NULL)
                printf("PTB-DEBUG: Releasing pointer barriers for window %i.\n", windowRecord->windowIndex);
            else
                printf("PTB-DEBUG: Releasing pointer barriers for window %i which match rect [%i, %i, %i, %i].\n",
                       windowRecord->windowIndex, (int) rect[kPsychLeft], (int) rect[kPsychTop], (int) rect[kPsychRight], (int) rect[kPsychBottom]);
        }

        // rect which defines a single point a la [x,y,x,y]?
        if (rect && (rect[kPsychLeft] == rect[kPsychRight]) && (rect[kPsychTop] == rect[kPsychBottom])) {
            // This is a special case, as it is confined by a double rect (see above for point case).
            // Need to unconfine it equally special:
            rect[kPsychLeft]--;
            rect[kPsychTop]--;
            rect[kPsychRight]--;
            rect[kPsychBottom]--;
            rect[kPsychLeft]--;
            rect[kPsychTop]--;
            rect[kPsychRight]++;
            rect[kPsychBottom]++;
            PsychOSConstrainPointer(windowRecord, FALSE, rect);
            rect[kPsychLeft]--;
            rect[kPsychTop]--;
            rect[kPsychRight]++;
            rect[kPsychBottom]++;
            PsychOSConstrainPointer(windowRecord, FALSE, rect);

            return(TRUE);
        }

        PsychLockDisplay();

        for (i = 0; i < PSYCH_MAX_POINTER_BARRIERS; i++)
            if ((barrierParentWindows[i] == windowRecord) && ((rect == NULL) || PsychMatchRect(barrierRects[i], rect))) {
                XFixesDestroyPointerBarrier(dpy, barriers[i]);
                barriers[i] = 0;
                barrierParentWindows[i] = NULL;
                pointerBarrierCount--;
            }

        XFlush(dpy);
        PsychUnlockDisplay();
    }

    return(TRUE);
}

/* PsychVRRActive()
 *
 * Report back, if the current windowRecord is corresponding to a onscreen window that
 * is presented using Variable Refresh Rate technology for stimulus presentation with fine-grained
 * timing, e.g., AMD FreeSync, NVidia G-Sync, Displayport adaptive sync, or HDMI VRR.
 *
 * Returns TRUE if VRR likely active, FALSE otherwise.
 */
psych_bool PsychVRRActive(PsychWindowRecordType *windowRecord)
{
    // Not in VRR mode if VRR mode not enabled by user-script via windowRecord->vrrMode > 0 or if
    // this isn't a non-transparent, opaque, decorationless, top-level, unoccluded, unredirected fullscreen window:
    if (!(windowRecord->vrrMode) || !(windowRecord->specialflags & kPsychIsFullscreenWindow) ||
        (PsychPrefStateGet_WindowShieldingLevel() < 2000))
        return(FALSE);

    // Basics for VRR are covered.

    // Is this a G-Sync capable and configured NVidia gpu? We assume that if the setup
    // is capable of G-Sync in principle, then GSync is enabled, given user-script requested
    // that. We can not actually detect if G-Sync is active or disabled by user settings in
    // xorg.conf or nvidia control center GUI, so go with most reasonable assumption:
    if (windowRecord->gfxcaps & kPsychGfxCapGSync)
        return(TRUE);

    // Is this a VRR capable (HDMI VRR / DP adaptive sync / FreeSync) gpu, e.g., from AMD or Intel?
    if (windowRecord->gfxcaps & kPsychGfxCapLinuxVRR)
        return(TRUE);

    // Nope, VRR inactive or unsupported:
    return(FALSE);
}

/* End of classic X11/GLX backend: */
#endif
#endif

