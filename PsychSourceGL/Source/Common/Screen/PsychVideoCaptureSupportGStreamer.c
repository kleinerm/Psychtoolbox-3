/*
    PsychSourceGL/Source/Common/Screen/PsychVideoCaptureSupportGStreamer.c

    PLATFORMS:

    All.

    AUTHORS:

    Mario Kleiner           mk      mario.kleiner.de@gmail.com

    HISTORY:

    9.01.2011               Created initial version.
    8.04.2011               Make video/audio recording work ok.
    5.06.2011               Make video/audio recording good enough
                            for initial release on Linux.
    20.08.2014              Ported to GStreamer-1.4 and later.

    DESCRIPTION:

    This is the videocapture engine based on the free software (LGPL'ed)
    GStreamer multimedia framework. It supports video capture, sound capture and
    recording of video and/or sound. It requires GStreamer-1.4 or later.

    NOTES:

    The following works well so far (tested on Linux with Webcams):

        * Video device enumeration and selection (tested with iSight, USB webcams, DV Firewire cams, ...)

        * Simultaneous capture from 2 video cams.

        * Videocapture -> Live texture display. All formats L, LA, RGB, RGBA, YUYV.

        * Video-only recording, also optionally combined with live texture display.

        * Video + audio recording (also with optional live display).

        * Timestamping, GetSecs or pipeline running time timebase.

        * Low latency (dropframes) capture, and non-dropframes capture.

        * Selection of resolution and framerate.

        * Definition of ROI's and application to video feed and/or video recording.

        * Video codec selection.

        * Provides interface to change camera settings like exposure time, gain, contrast, etc.

    => Most functionality for typical everyday tasks works perfect or reasonably well.
    => Some issues for special case apps persist, as written below.

    TODO:

*/

#ifdef PTB_USE_GSTREAMER

#include "Screen.h"
#include <float.h>
#include <locale.h>
#include "PsychVideoCaptureSupport.h"

// Include for dynamic loading of external plugin, for now only on Unix:
#if PSYCH_SYSTEM != PSYCH_WINDOWS
#include <dlfcn.h>
#endif

// These are the includes for GStreamer:
#include <glib.h>
#include <gst/gst.h>

#if GST_CHECK_VERSION(1,0,0)

#include <gst/app/gstappsink.h>
#include <gst/video/colorbalance.h>
#include <gst/pbutils/encoding-profile.h>

// Name of the presets for camerabin2 video recording with encoding profiles:
const char video_preset_name[] = "ptb3_videorecording_presets_video";
const char audio_preset_name[] = "ptb3_videorecording_presets_audio";
const char muxer_preset_name[] = "ptb3_videorecording_presets_muxer";

// Global gstlaunchbinsrc gst-launch style spec of a videocapture bin for use
// with deviceIndex -9 in Screen('OpenVideoCapture'):
char gstlaunchbinsrc[8192] = { 0 };

#define PSYCH_MAX_VIDSRC    256
PsychVideosourceRecordType *devices = NULL;
int ntotal = 0;

// Record which defines all state for a capture device:
typedef struct {
    int valid;                        // Is this a valid device record? zero == Invalid.
    psych_mutex mutex;
    psych_condition condition;
    int capturehandle;
    int frameAvail;                   // Number of frames in videosink.
    int preRollAvail;
    GstElement *camera;               // Ptr to a GStreamer camera object that holds the internal state for such cams.
    GMainLoop *VideoContext;          // Message bus context for delivery of status/error/warning messages by GStreamer.
    GstElement *videosink;            // Our appsink for retrieval of live video feeds --> PTB texture conversion.
    GstElement *videosource;          // The videosource, encapsulating the actual video capture device.
    GstElement *videowrappersrc;      // The wrappercambinsrc used for encapsulating videosource for camerabin2 capture.
    GstElement *videoenc;             // Video encoder for video recording.
    GstElement *videorate_filter;     // Video framerate converter to guarantee constant framerate and a/v sync for recording.
    GstClockTime lastSavedBaseTime;   // Last time the pipeline was put into PLAYING state. Saved at StopCapture time before stop.
    int nrAudioTracks;
    int nrVideoTracks;
    int dropframes;                   // 1 == Always deliver most recent frame in FIFO, even if dropping of frames is neccessary.
    unsigned char* scratchbuffer;     // Scratch buffer for YUV->RGB conversion.
    int reqpixeldepth;                // Requested number of channels of single pixel in output texture.
    int pixeldepth;                   // Depth of single pixel from grabber in bits aka bpp.
    int bitdepth;                     // Requested or actual bpc - bits per color/luminance channel.
    int num_dmabuffers;               // Number of DMA ringbuffers to use in DMA capture.
    int nrframes;                     // Total count of decompressed images.
    double fps;                       // Acquisition framerate of capture device.
    int width;                        // Width x height of captured images from video source (prior to cropping etc.)
    int height;                       // This is effectively the configured resolution (width x height pixels) of the video source.
    int frame_width;                  // Width x height of frames returned from videosink for OpenGL texture conversion or
    int frame_height;                 // image return in raw image buffer.
    double last_pts;                  // Capture timestamp of previous frame.
    double current_pts;               // Capture timestamp of current frame.
    int current_dropped;              // Dropped count for this fetch cycle...
    int nr_droppedframes;             // Counter for dropped frames.
    int grabber_active;               // Grabber running?
    int recording_active;             // Movie file recording requested?
    unsigned int recordingflags;      // recordingflags, as passed to 'OpenCaptureDevice'.
    PsychRectType roirect;            // Region of interest rectangle - denotes subarea of full video capture area.
    double avg_decompresstime;        // Average time spent in decompressor.
    double avg_gfxtime;               // Average time spent in buffer --> OpenGL texture conversion and statistics.
    int nrgfxframes;                  // Count of fetched textures.
    char* targetmoviefilename;        // Filename of a movie file to record.
    char* cameraFriendlyName;         // Camera friendly device name.
    char videosourcename[100];        // Plugin name of the videosource plugin.
    void* markerTrackerPlugin;        // Opaque pointer to instance handle of a markerTrackerPlugin.
} PsychVidcapRecordType;

static PsychVidcapRecordType vidcapRecordBANK[PSYCH_MAX_CAPTUREDEVICES];
static int numCaptureRecords = 0;
static psych_bool gs_firsttime = TRUE;
double gs_startupTime = 0.0;

// Global state and functions related to special markerTrackerPlugin's:

// Global handle to shared library:
static void* markerTrackerPlugin_libraryhandle = NULL;

// Function prototypes of functions we want to use in the plugin library:
static void* (*TrackerPlugin_initialize)(void);
static bool (*TrackerPlugin_shutdown)(void* handle);
static bool (*TrackerPlugin_processFrame)(void* handle, unsigned long* source_ptr, int imgwidth, int imgheight, int xmin, int ymin, unsigned int timeidx, double capturetimestamp, unsigned int absolute_frameindex);
static bool (*TrackerPlugin_processPluginDataBuffer)(void* handle, unsigned long* buffer, int size);

// Forward declaration of internal helper function:
void PsychGSDeleteAllCaptureDevices(void);
int PsychGSDrainBufferQueue(PsychVidcapRecordType* capdev, int numFramesToDrain, unsigned int flags);


/*    PsychGetGSVidcapRecord() -- Given a handle, return ptr to video capture record.
 *    --> Internal helper function of PsychVideoCaptureSupport.
 */
PsychVidcapRecordType* PsychGetGSVidcapRecord(int deviceIndex)
{
    // Sanity checks:
    if (deviceIndex < 0) {
        PsychErrorExitMsg(PsychError_user, "Invalid (negative) deviceIndex for video capture device passed!");
    }

    if (deviceIndex >= PSYCH_MAX_CAPTUREDEVICES) {
        PsychErrorExitMsg(PsychError_user, "Invalid deviceIndex for video capture device passed. Index exceeds number of registered devices!");
    }

    if (!vidcapRecordBANK[deviceIndex].valid) {
        PsychErrorExitMsg(PsychError_user, "Invalid deviceIndex for video capture device passed. No such device open!");
    }

    // Ok, we have a valid device record, return a ptr to it:
    return(&vidcapRecordBANK[deviceIndex]);
}

#if PSYCH_SYSTEM == PSYCH_OSX
extern gboolean	gst_init_check(int *argc, char **argv[], GError **err) __attribute__((weak_import));
#endif

/* Internal: Check if GStreamer is already initialized. Initialize it,
 * if neccessary.
 */
void PsychGSCheckInit(const char* engineName)
{
    GError      *error = NULL;
    GstClock    *system_clock = NULL;

    if (gs_firsttime) {
        // First time invocation:

        // Check if GStreamer is properly installed and (can be) dynamically loaded and linked:
        #if PSYCH_SYSTEM != PSYCH_LINUX
            // On Windows and OSX, we need to delay-load the GStreamer libraries. This loading
            // and linking will automatically happen downstream. However, if delay loading
            // would fail, we would end up with a crash! For that reason, on MS-Windows, we
            // try to load the DLL, just to probe if the real load/link/bind op later on will
            // likely succeed. If the following LoadLibrary() call fails and returns NULL,
            // then we know we would end up crashing. We check for two versions of the dll's, as
            // different GStreamer runtime distributions use different filenames for the dll's.
            //
            // On OSX we check if the gst_init_check() function is defined, aka non-NULL. The
            // OSX linker sets the symbol to NULL if dynamic weak linking during runtime failed.
            // On failure we'll output some helpful error-message instead:
            #if PSYCH_SYSTEM == PSYCH_WINDOWS
                #ifdef PTBOCTAVE3MEX
                // Octave-4 on Windows specific code. Delay loading of the main dependencies
                // does not work, because Octave-4 always resolves dependencies of mex files
                // immediately and fails if it can not do that.
                //
                // However we need a Octave-4.0.0 specific hack here to prevent failure of runtime
                // loading of some GStreamer plugins, e.g., for movie playback. Octave-4 comes
                // with its own version of libbz2, needed by GraphicsMagick for some image format.
                // This version is incompatible with the libbz2 provided by GStreamer and needed
                // by some of its plugins. Our hacky solution is to runtime load libbz2 from the
                // GStreamer installation directory now, thereby overriding the default dll search
                // order which would try to load from the application installation directory instead,
                // which would mean to load the "wrong" libbz2 from Octave-4's bin directory.
                char gst_libbz2_path[FILENAME_MAX];
                gst_libbz2_path[0] = 0;
                if (NULL == getenv("PSYCH_GSTREAMER_SDK_ROOT")) {
                    if (PsychPrefStateGet_Verbosity() > 1) {
                        printf("PTB-WARNING: Environment variable PSYCH_GSTREAMER_SDK_ROOT undefined. Apparently PsychStartup.m\n");
                        printf("PTB-WARNING: didn't set it? This can cause failure to load required GStreamer plugins, at least\n");
                        printf("PTB-WARNING: on official 32-Bit Octave-4.0.0, unless special setup steps have been performed to\n");
                        printf("PTB-WARNING: replace some unsuitable Octave DLL's with suitable corresponding DLL's from GStreamer.\n");
                        printf("PTB-WARNING: If you experience failure of multi-media functions, this might be the reason for it.\n");
                    }
                }
                else {
                    // GStreamer runtime path defined by PsychStartup.m run. Try to load override dll(s) from GStreamer
                    // runtime directory:
                    sprintf(gst_libbz2_path, "%s\\libbz2.dll", getenv("PSYCH_GSTREAMER_SDK_ROOT"));
                    if (NULL == LoadLibraryEx(gst_libbz2_path, NULL, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR)) {
                        if (PsychPrefStateGet_Verbosity() > 0) {
                            printf("PTB-ERROR: Oh oh! libbz2.dll loading from GStreamer runtime directory as '%s' failed!\n",
                                   gst_libbz2_path);
                            printf("PTB-ERROR: Error code %d. This can cause failure to load required GStreamer plugins, at least\n",
                                   GetLastError());
                            printf("PTB-ERROR: on official 32-Bit Octave-4.0.0, unless special setup steps have been performed to\n");
                            printf("PTB-ERROR: replace some unsuitable Octave DLL's with suitable corresponding DLL's from GStreamer.\n");
                            printf("PTB-ERROR: If you experience failure of multi-media functions, this might be the reason for it.\n");
                        }
                    }
                    else if (PsychPrefStateGet_Verbosity() > 3) {
                        printf("PTB-DEBUG: Loaded '%s' from GStreamer runtime directory to override Octave's incompatible DLL.\n", gst_libbz2_path);
                    }
                }

                // We don't fail in the Octave specific startup path:
                if (FALSE) {
                #else
                // Non-Octave (Matlab) specific code, where delay loading works:
                if ((NULL == LoadLibrary("libgstreamer-1.0-0.dll")) || (NULL == LoadLibrary("libgstapp-1.0-0.dll"))) {
                #endif
            #endif
            #if PSYCH_SYSTEM == PSYCH_OSX
                if (NULL == gst_init_check) {
            #endif
                // Failed:
                printf("\n\n\n");
                printf("PTB-ERROR: Tried to startup GStreamer multi-media framework. This didn't work, because one\n");
                printf("PTB-ERROR: of the required GStreamer runtime libraries failed to load, probably because it\n");
                printf("PTB-ERROR: could not be found, could not be accessed (e.g., due to permission problems),\n");
                printf("PTB-ERROR: or most likely because GStreamer isn't installed on this machine at all.\n");
                printf("PTB-ERROR: Another reason could be that you have GStreamer version 0.10.x instead of the required\n");
                printf("PTB-ERROR: version >= 1.4.0 installed. The version 0.10 series is no longer supported.\n\n");
                #if PSYCH_SYSTEM == PSYCH_WINDOWS
                    printf("PTB-ERROR: The system returned the error code %d.\n", GetLastError());
                #endif
                printf("PTB-ERROR: Please read the help by typing 'help GStreamer' for installation and troubleshooting\n");
                printf("PTB-ERROR: instructions.\n\n");
                printf("PTB-ERROR: Due to failed GStreamer initialization, the %s engine is disabled for this session.\n\n", engineName);

                PsychErrorExitMsg(PsychError_user, "GStreamer initialization failed due to library loading problems. Aborted.");
            }
        #endif

        // Can't pass environment vars from Matlab to Screen+GStreamer on a MS-Windows
        // build against MSVCRT.dll, so we need a hack to pass a debug level env var to
        // GStreamer. Set GST_DEBUG env var from within Screen's DLL boundaries if verbosity > 20,
        // as this setting should transfer to the GStreamer dll's, which are also linked against msvcrt.dll:
        #if (PSYCH_SYSTEM == PSYCH_WINDOWS) && defined(__MSVCRT__)
        if (PsychPrefStateGet_Verbosity() > 20) {
            char dbglvl[10];
            sprintf(dbglvl, "%i", PsychPrefStateGet_Verbosity() - 20);
            _putenv_s("GST_DEBUG", dbglvl);
            printf("PTB-DEBUG: Set GStreamer GST_DEBUG=%s from within Screen() dll boundaries.\n", dbglvl);
        }
        #endif

        // Initialize GStreamer:
        if(!gst_init_check(NULL, NULL, &error)) {
            if (error) {
                printf("PTB-ERROR: GStreamer initialization failed with error: %s\n", (char*) error->message);
                g_error_free(error);
            }
            else {
                printf("PTB-ERROR: GStreamer initialization failed for unknown reason\n");
            }

            printf("PTB-ERROR: Due to failed GStreamer initialization, the %s engine is out of order.\n", engineName);
            PsychErrorExitMsg(PsychError_system, "GStreamer initialization failed! Aborted.");
        }

        // gs_startupTime is added to all timestamps from GStreamer to compensate for
        // clock zero offset wrt. to our GetSecs() time:
        if (PSYCH_SYSTEM == PSYCH_WINDOWS) {
            // TODO PORTING CHECK STILL TRUE?
            // Windows: Zero-Point is time of GStreamer startup aka sometime
            // during execution of gst_init_check() above. Current system time
            // is our best approximation so far:
            PsychGetAdjustedPrecisionTimerSeconds(&gs_startupTime);
        }
        else {
            // Other OS: Zero-Point of GStreamer clock is identical to Zero-Point
            // of our GetSecs() clock, so apply zero-correction:
            gs_startupTime = 0.0;
        }

        // Select opmode of GStreamers master clock:
        // We use monotonic clock on Windows and OS/X, as these correspond to the
        // clocks we use for GetSecs(), but realtime clock on Linux:
        system_clock = gst_system_clock_obtain();
        if (system_clock) {
            g_object_set(G_OBJECT(system_clock), "clock-type", ((PSYCH_SYSTEM == PSYCH_LINUX) ? GST_CLOCK_TYPE_REALTIME : GST_CLOCK_TYPE_MONOTONIC), NULL);
        }
        if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Using GStreamer version '%s'.\n", (char*) gst_version_string());

        markerTrackerPlugin_libraryhandle = NULL;

        // Reset firsttime flag:
        gs_firsttime = FALSE;
    }
}

/*
 *     PsychGSVideoCaptureInit() -- Initialize video capture subsystem.
 *     This routine is called by Screen's RegisterProject.c PsychModuleInit()
 *     routine at Screen load-time. It clears out the vidcapRecordBANK to
 *     bring the subsystem into a clean initial state.
 */
void PsychGSVideoCaptureInit(void)
{
    // Initialize vidcapRecordBANK with NULL-entries:
    int i;
    for (i=0; i < PSYCH_MAX_CAPTUREDEVICES; i++) {
        vidcapRecordBANK[i].valid = 0;
    }
    numCaptureRecords = 0;

    return;
}

/*
 *  void PsychGSExitVideoCapture() - Shutdown handler.
 *
 *  This routine is called by Screen('CloseAll') and on clear Screen time to
 *  do final cleanup. It deletes all capture objects
 *
 */
void PsychGSExitVideoCapture(void)
{
    // Release all capture devices
    PsychGSDeleteAllCaptureDevices();

    // Reset global library handle for markertracker plugin:
    markerTrackerPlugin_libraryhandle = NULL;

    // Shutdown GStreamer framework, release all resources.
    // This is usually not needed/used. For memory leak
    // checking only!
    if (FALSE) {
        gs_firsttime = TRUE;
        gst_deinit();
    }

    return;
}

/*
 *  PsychGSDeleteAllCaptureDevices() -- Delete all capture objects and release all associated ressources.
 */
void PsychGSDeleteAllCaptureDevices(void)
{
    int i;
    for (i=0; i<PSYCH_MAX_CAPTUREDEVICES; i++) {
        if (vidcapRecordBANK[i].valid) PsychGSCloseVideoCaptureDevice(i);
    }
    return;
}

// Forward declaration:
static gboolean PsychVideoBusCallback(GstBus *bus, GstMessage *msg, gpointer dataptr);

/* Perform one context loop iteration (for bus message handling) if doWait == false,
 * or two seconds worth of iterations if doWait == true. This drives the message-bus
 * callback, so needs to be performed to get any error reporting etc.
 */
int PsychGSProcessVideoContext(PsychVidcapRecordType* capdev, psych_bool doWait)
{
    GstBus* bus;
    GstMessage *msg;
    psych_bool workdone = FALSE;
    double tdeadline, tnow;
    PsychGetAdjustedPrecisionTimerSeconds(&tdeadline);
    tnow = tdeadline;
    tdeadline+=2.0;

    // New style:
    bus = gst_pipeline_get_bus(GST_PIPELINE(capdev->camera));
    msg = NULL;

    // If doWait, try to perform iterations until 2 seconds elapsed or at least one event handled:
    while (doWait && (tnow < tdeadline) && !gst_bus_have_pending(bus)) {
        // Update time:
        PsychYieldIntervalSeconds(0.010);
        PsychGetAdjustedPrecisionTimerSeconds(&tnow);
    }

    msg = gst_bus_pop(bus);
    while (msg) {
        workdone = TRUE;
        PsychVideoBusCallback(bus, msg, capdev);
        gst_message_unref(msg);
        msg = gst_bus_pop(bus);
    }

    gst_object_unref(bus);

    return(workdone);
}

/* Initiate pipeline state changes: Startup, Preroll, Playback, Pause, Standby, Shutdown. */
static psych_bool PsychVideoPipelineSetState(GstElement* camera, GstState state, double timeoutSecs)
{
    GstState                state_pending;
    GstStateChangeReturn    rcstate;

    gst_element_set_state(camera, state);

    // Non-Blocking, async?
    if (timeoutSecs < 0) return(TRUE);

    // Wait for up to timeoutSecs for state change to complete or fail:
    rcstate = gst_element_get_state(camera, &state, &state_pending, (GstClockTime) (timeoutSecs * 1e9));
    switch(rcstate) {
        case GST_STATE_CHANGE_SUCCESS:
            if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: Statechange completed with GST_STATE_CHANGE_SUCCESS.\n");
        break;

        case GST_STATE_CHANGE_ASYNC:
            if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: Statechange in progress with GST_STATE_CHANGE_ASYNC.\n");
        break;

        case GST_STATE_CHANGE_NO_PREROLL:
            if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-INFO: Statechange completed with GST_STATE_CHANGE_NO_PREROLL.\n");
        break;

        case GST_STATE_CHANGE_FAILURE:
            if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Statechange failed with GST_STATE_CHANGE_FAILURE!\n");
            return(FALSE);
        break;

        default:
            if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Unknown state-change result in preroll.\n");
            return(FALSE);
    }

    return(TRUE);
}

/* Receive messages from the playback pipeline message bus and handle them: */
static gboolean PsychVideoBusCallback(GstBus *bus, GstMessage *msg, gpointer dataptr)
{
    PsychVidcapRecordType* capdev = (PsychVidcapRecordType*) dataptr;
    if (PsychPrefStateGet_Verbosity() > 11) printf("PTB-DEBUG: PsychVideoBusCallback: Msg source name and type: %s : %s\n", GST_MESSAGE_SRC_NAME(msg), GST_MESSAGE_TYPE_NAME(msg));

    switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_EOS:
            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Videobus: Message EOS received.\n"); fflush(NULL);
        break;

        case GST_MESSAGE_WARNING: {
            gchar  *debug;
            GError *error;

            gst_message_parse_warning(msg, &error, &debug);
            if (PsychPrefStateGet_Verbosity() > 3) {
                    printf("PTB-WARNING: GStreamer videocapture engine reports this warning:\n"
                           "             Warning from element %s: %s\n", GST_OBJECT_NAME(msg->src), error->message);
                    printf("             Additional debug info: %s.\n", (debug) ? debug : "None");
            }

            g_free(debug);
            g_error_free(error);
            break;
        }

        case GST_MESSAGE_ERROR: {
            gchar  *debug;
            GError *error;

            gst_message_parse_error(msg, &error, &debug);
            if (PsychPrefStateGet_Verbosity() > 0) {
                    printf("PTB-ERROR: GStreamer videocapture engine reports this error:\n"
                           "           Error from element %s: %s\n", GST_OBJECT_NAME(msg->src), error->message);
                    printf("           Additional debug info: %s.\n\n", (debug) ? debug : "None");

                    if ((error->domain == GST_RESOURCE_ERROR) && (error->code != GST_RESOURCE_ERROR_NOT_FOUND)) {
                        printf("           This could mean that there was some problem with opening the video device (permissions etc.).\n\n");
                    }

                    if ((error->domain == GST_RESOURCE_ERROR) && (error->code == GST_RESOURCE_ERROR_NOT_FOUND)) {
                        printf("           This means that no such video device with the given name could be found.\n\n");
                    }
            }

            g_free(debug);
            g_error_free(error);
            break;
        }

        default:
        break;
    }

    return TRUE;
}


/* Called at each end-of-stream event at end of playback: */
static void PsychEOSCallback(GstAppSink *sink, gpointer user_data)
{
    PsychVidcapRecordType* capdev = (PsychVidcapRecordType*) user_data;

    PsychLockMutex(&capdev->mutex);
    printf("PTB-DEBUG: Videosink reached EOS.\n");
    // Signal EOS to trigger an abort of device open sequence:
    capdev->grabber_active = true;
    PsychUnlockMutex(&capdev->mutex);

    return;
}

static void PsychProbeSampleProps(GstSample *videoSample, int *w, int *h, double *fps)
{
    GstCaps             *caps;
    GstStructure        *str;
    gint                rate1, rate2;
    rate1 = rate2 = 0;
    str = NULL;

    if (videoSample) {
        caps = gst_sample_get_caps(videoSample);
        if (caps) str = gst_caps_get_structure(caps, 0);
        if (fps) {
            if (str) gst_structure_get_fraction(str, "framerate", &rate1, &rate2);
            if (rate2 > 0) {
                *fps = ((double) rate1) / ((double) rate2);
            } else {
                *fps = 0.0;
            }
        }
        if (w && str) gst_structure_get_int(str,"width", w);
        if (h && str) gst_structure_get_int(str,"height", h);
    }

    return;
}

/* Called whenever pipeline goes into pause mode.
 * Signals/handles arrival of preroll buffers. Used to detect/signal when
 * new videobuffers are available in non-playback mode, when device becomes ready.
 */
static GstFlowReturn PsychNewPrerollCallback(GstAppSink *sink, gpointer user_data)
{
    GstSample *videoSample;
    int w = 0, h = 0;

    PsychVidcapRecordType* capdev = (PsychVidcapRecordType*) user_data;

    PsychLockMutex(&capdev->mutex);
    videoSample = gst_app_sink_pull_preroll(GST_APP_SINK(capdev->videosink));
    if (videoSample) {
        PsychProbeSampleProps(videoSample, &w, &h, &capdev->fps);
        gst_sample_unref(videoSample);

        // If we still don't know the size of video input images, e.g., because
        // preroll was skipped during 'OpenVideoCapture' or probing failed for
        // some other reason, then assign values now:
        if (capdev->width == 0) capdev->width = w;
        if (capdev->height == 0) capdev->height = h;
        if (capdev->frame_width == 0) capdev->frame_width = w;
        if (capdev->frame_height == 0) capdev->frame_height = h;
    }

    if (PsychPrefStateGet_Verbosity() > 5) {
        printf("PTB-DEBUG: New PrerollBuffer received. fps = %f [w x h = %i x %i]\n", capdev->fps, w, h);
    }

    capdev->preRollAvail++;

    PsychUnlockMutex(&capdev->mutex);

    return(GST_FLOW_OK);
}

/* Called whenever pipeline is in active playback and a new video frame arrives.
 * Used to detect/signal when new videobuffers are available in playback mode.
 */
static GstFlowReturn PsychNewBufferCallback(GstAppSink *sink, gpointer user_data)
{
    PsychVidcapRecordType* capdev = (PsychVidcapRecordType*) user_data;

    PsychLockMutex(&capdev->mutex);
    capdev->frameAvail++;
    if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: New Buffer received. %i\n", capdev->frameAvail);
    PsychSignalCondition(&capdev->condition);
    PsychUnlockMutex(&capdev->mutex);

    return(GST_FLOW_OK);
}

/* Not used by us, but needs to be defined as no-op anyway: */
static GstFlowReturn PsychNewBufferListCallback(GstAppSink *sink, gpointer user_data)
{
    PsychVidcapRecordType* capdev = (PsychVidcapRecordType*) user_data;

    PsychLockMutex(&capdev->mutex);
    //printf("PTB-DEBUG: New Bufferlist received.\n");
    PsychUnlockMutex(&capdev->mutex);

    return(GST_FLOW_OK);
}

/* Not used by us, but needs to be defined as no-op anyway: */
static void PsychDestroyNotifyCallback(gpointer user_data)
{
    return;
}

static GstAppSinkCallbacks videosinkCallbacks = {
    PsychEOSCallback,
    PsychNewPrerollCallback,
    PsychNewBufferCallback,
    PsychNewBufferListCallback,
    NULL
};

/*
 *  PsychGSCloseVideoCaptureDevice() -- Close a capture device and release all associated ressources.
 */
void PsychGSCloseVideoCaptureDevice(int capturehandle)
{
    // Retrieve device record for handle:
    PsychVidcapRecordType* capdev = PsychGetGSVidcapRecord(capturehandle);

    // Make sure GStreamer is ready:
    PsychGSCheckInit("videocapture");

    if (capdev->camera) {
        // Stop capture immediately if it is still running:
        PsychGSVideoCaptureRate(capturehandle, 0, 1, NULL);

        // Drain again, just to be safe:
        PsychGSDrainBufferQueue(capdev, INT_MAX, 0);

        // Close & Shutdown camera, release ressources:
        // Stop video capture immediately:
        PsychVideoPipelineSetState(capdev->camera, GST_STATE_NULL, 20.0);

        // Delete camera for this handle:
        gst_object_unref(GST_OBJECT(capdev->camera));
        capdev->camera=NULL;
    }

    // Delete video context:
    if (capdev->VideoContext) g_main_loop_unref(capdev->VideoContext);
    capdev->VideoContext = NULL;

    PsychDestroyMutex(&capdev->mutex);
    PsychDestroyCondition(&capdev->condition);

    if (capdev->videosink) {
        gst_object_unref(GST_OBJECT(capdev->videosink));
        capdev->videosink = NULL;
    }

    // This has been auto-destructed (hopefully) by camerabin:
    capdev->videosource = NULL;

    if (capdev->targetmoviefilename) free(capdev->targetmoviefilename);
    capdev->targetmoviefilename = NULL;

    if (capdev->cameraFriendlyName) free(capdev->cameraFriendlyName);
    capdev->cameraFriendlyName = NULL;

#if PSYCH_SYSTEM != PSYCH_WINDOWS

    // Shutdown and release an assigned markerTrackerPlugin:
    if (capdev->markerTrackerPlugin) {
        // Try to shutdown this instance of the plugin:
        if (!(*TrackerPlugin_shutdown)(capdev->markerTrackerPlugin)) {
            printf("PTB-WARNING: Failed to shutdown markertracker plugin for device %i.\n", capturehandle);
        }

        capdev->markerTrackerPlugin = NULL;

        // Release one reference to this instance of the plugin:
        // There may be more instances. If the last reference is released, the plugin
        // will get truly unloaded by the dynamic linker and the markerTrackerPlugin_libraryhandle
        // will become invalid / a stale pointer which we will clean up in PsychGSExitVideoCapture():
        dlclose(markerTrackerPlugin_libraryhandle);
    }

#endif

    // Invalidate device record to free up this slot in the array:
    capdev->valid = 0;

    // Decrease counter of open capture devices:
    if (numCaptureRecords>0) numCaptureRecords--;

    // Done.
    return;
}

// GstDeviceMonitor, GstDeviceProvider and GstDevice require GStreamer-1.4.0 or later.
#if GST_CHECK_VERSION(1,4,0)

static void PsychGSProbeGstDevice(GstDevice* device, int inputIndex, const char* srcname,
                                int classIndex, const char* className, const char* devHandlePropName, unsigned int flags)
{
    GValue              val = G_VALUE_INIT;
    GParamSpec          *paramSpec;
    GstElement          *videosource;
    char                port_str[64];
    char                *device_name = NULL;
    gchar               *pstring = NULL;
    gchar               *devString = NULL;
    psych_uint64        deviceURI = 0;

    if (PsychPrefStateGet_Verbosity() > 5) {
        devString = gst_device_get_device_class(device);
        printf("DEVICECLASS %s\n", (char*) devString);
        g_free(devString);

        devString = gst_device_get_display_name(device);
        printf("DISPLAYNAME %s\n", (char*) devString);
        g_free(devString);
    }

    // Get dedicated instance of the video plugin, configured to use the
    // specific capture device being probed here:
    videosource = gst_device_create_element(device, "ptb_deviceprovidervideosource");
    if (videosource == NULL) {
        if (PsychPrefStateGet_Verbosity() > 0)
            printf("PTB-ERROR: PsychGSProbeGstDevice(): Could not create videosource for probe target device!!!\n");
        return;
    }

    // Store the GstDevice* device as unique and well defined handle for this capture device:
    devices[ntotal].gstdevice = device;

    // Assign port_str equivalent to the value which would have been retrieved
    // from the probed device property of our original property-proble implementation
    // from the GStreamer-0.10.x backend. This for backwards compatibility.
    //
    // Check if our key property exists in the selected videosource and provide a type spec for it:
    if ((paramSpec = g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), devHandlePropName))) {
        // Yes. Init a GValue object for the property of given type ...
        g_value_init(&val, G_PARAM_SPEC_VALUE_TYPE(paramSpec));
        // ... and then query the property and assign it to GValue 'val':
        g_object_get_property(G_OBJECT(videosource), devHandlePropName, &val);

        // String value property?
        if (G_VALUE_HOLDS_STRING(&val)) {
            // Assign as port_str:
            sprintf(port_str, "%s", (const char*) g_value_get_string(&val));
        }

        // Numeric GUID property?
        if (G_VALUE_HOLDS_UINT64(&val)) {
            // Assign as numeric string and as numeric deviceURI:
            deviceURI = g_value_get_uint64(&val);
            sprintf(port_str, "%llu", deviceURI);
        }

        // Done with val:
        g_value_unset(&val);
    }
    else {
        // Our videosource capture device instance doesn't expose the key selection
        // property 'devHandlePropName'. What now? Mark this instance so the device
        // open code will use the stored GstDevice* for capture device instantiation
        // instead:
        sprintf(port_str, "USEGSTDEVICE* %p", device);
    }

    devices[ntotal].deviceIndex = classIndex * 10000 + inputIndex;
    devices[ntotal].classIndex = classIndex;
    devices[ntotal].inputIndex = inputIndex;
    sprintf(devices[ntotal].deviceClassName, "%s", className);
    sprintf(devices[ntotal].deviceHandle, "%s", port_str);
    sprintf(devices[ntotal].deviceSelectorProperty, "%s", devHandlePropName);
    sprintf(devices[ntotal].deviceVideoPlugin, "%s", srcname);

    // Query and assign device specific parameters:
    pstring = NULL;
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "device")) {
        g_object_get(G_OBJECT(videosource), "device", &pstring, NULL);
    }

    if (pstring) {
        sprintf(devices[ntotal].device, "%s", pstring);
        g_free(pstring);
    }
    else {
        sprintf(devices[ntotal].device, "%s", port_str);
    }

    pstring = NULL;
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "device-path")) {
        g_object_get(G_OBJECT(videosource), "device-path", &pstring, NULL);
    }

    if (pstring) {
        sprintf(devices[ntotal].devicePath, "%s", pstring);
        g_free(pstring);
    }
    else {
        sprintf(devices[ntotal].devicePath, "%s", port_str);
    }

    pstring = NULL;
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "device-name")) {
        g_object_get(G_OBJECT(videosource), "device-name", &pstring, NULL);
    }

    if (pstring) {
        sprintf(devices[ntotal].deviceName, "%s", pstring);
        g_free(pstring);
    }
    else {
        sprintf(devices[ntotal].deviceName, "%s", port_str);
    }

    pstring = NULL;
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "guid")) {
        g_object_get(G_OBJECT(videosource), "guid", &deviceURI, NULL);
        devices[ntotal].deviceURI = deviceURI;
    } else {
        devices[ntotal].deviceURI = 0;
    }

    // Done with our own dedicated device instance:
    gst_object_unref(GST_OBJECT(videosource));

    return;
}

/*
 * Use the new GstDeviceMonitor functionality of GStreamer 1.4.0
 * and later to enumerate and handle video sources which PTB is not
 * explicitely aware of, e.g., video plugins / source types whose
 * existence was unknown at the time of the PTB release and therefore
 * don't have explicit query/handling code. Or plugins for which no
 * dedicated query/handling code is needed - so this is a "catch all"
 * for leftovers.
 *
 * At this time (GStreamer 1.4.0, September 2014) the only known video
 * plugin to actually support device monitors or device providers is
 * the v4l2src plugin for Video4Linux-2 on Linux.
 */
static void PsychGSEnumerateVideoSourcesViaDeviceMonitor(void)
{
    GstDeviceMonitor    *monitor;
    GstDevice           *device;
    GstCaps             *caps;
    GList               *devlist = NULL, *devIter;
    gchar               *devString;
    int                 n = 1; // Start input index is 1 for class 0.

    monitor = gst_device_monitor_new();
    caps = gst_caps_new_empty_simple("video/x-raw");
    // Note: caps are not set at the moment (Passing NULL for "any"). Not sure if it
    // would make sense to restrict ourselves to video/x-raw, as some sources can also
    // provide things like video/x-dv or video/x-h264. Be inclusive for the moment...
    gst_device_monitor_add_filter(monitor, "Video/Source", NULL);
    gst_caps_unref(caps);

    if (!gst_device_monitor_start(monitor)) {
        if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: GstDeviceMonitor unsupported. May not be able to enumerate all video devices.\n");
    }
    else {
        devlist = gst_device_monitor_get_devices(monitor);

        for (devIter = g_list_first(devlist); devIter != NULL; devIter = g_list_next(devIter)) {
            device = (GstDevice*) devIter->data;
            if (device == NULL) continue;

            // Probe all device properties and store them in internal global videocapture device array:
            devString = gst_device_get_device_class(device);
            PsychGSProbeGstDevice(device, n, "DeviceMonitor", 0, (char*) devString, "", 0);
            g_free(devString);

            // Increment count of detected devices for this plugin:
            n++;

            // Increment count of total detected devices for all plugins so far:
            ntotal++;

            if (ntotal >= PSYCH_MAX_VIDSRC - 2) {
                if (PsychPrefStateGet_Verbosity() > 1)
                    printf("PTB-WARNING: Maximum number of allowable video sources during enumeration %i reached! Aborting enumeration.\n", PSYCH_MAX_VIDSRC);
                break;
            }
        }

        g_list_free(devlist);
        gst_device_monitor_stop(monitor);
    }

    gst_object_unref(GST_OBJECT(monitor));

    return;
}

#else
    // Dummy typedef, so we don't need to sprinkle ifdefs everywhere:
    typedef GstElement GstDevice;
    typedef GstElement GstDeviceProvider;
    static void PsychGSEnumerateVideoSourcesViaDeviceMonitor(void) {};

//#warning Building against GStreamer version older than 1.4.0 - No device monitor support! Consider upgrading!
#endif

/* Helper routine for PsychGSEnumerateVideoSources()
 *
 * Probes video source plugin 'srcname' [with class index 'classIndex' and
 * name 'className', using the property 'devHandlePropName' and probe strategy
 * as given by 'flags'. Adds all detected video input devices for that plugin
 * to the global 'devices' array and increases ntotal count accordingly.
 */
static void PsychGSEnumerateVideoSourceType(const char* srcname, int classIndex, const char* className, const char* devHandlePropName, const char* providername, unsigned int flags)
{
    GstDevice           *device;
    GstDeviceProvider   *provider;
    GList               *devlist = NULL, *devIter;
    int                 i, n, nmaxp, dopoke;
    char                class_str[64];
    int                 inputIndex;
    GstElement          *videosource = NULL;
    GValueArray         *viddevs = NULL;
    GValue              *dev = NULL;
    char                *device_name = NULL;
    gchar               *pstring = NULL;
    psych_uint64        deviceURI = 0;
    int                 oldverbose;

    // Info to the user:
    if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to probe %s [Providername '%s'] as video source...\n", srcname, providername);

    // Define class name:
    sprintf(class_str, "%s", className);

    // Does this source support device enumeration of supported capture devices?
    #if GST_CHECK_VERSION(1,4,0)
    if ((provider = gst_device_provider_factory_get_by_name((const gchar*) providername)) && GST_IS_DEVICE_PROVIDER(provider)) {
        if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: Has a GStreamer device provider - Good, using it.\n");
        devlist = gst_device_provider_get_devices(GST_DEVICE_PROVIDER(provider));
        if (devlist && (PsychPrefStateGet_Verbosity() > 4)) printf("PTB-INFO: Enumerating %i attached video devices...\n", g_list_length(devlist));

        n = 0;
        for (devIter = g_list_first(devlist); devIter != NULL; devIter = g_list_next(devIter)) {
            device = (GstDevice*) devIter->data;
            if (device == NULL) continue;

            // Probe all device properties and store them in internal global videocapture device array:
            PsychGSProbeGstDevice(device, n, srcname, classIndex, className, devHandlePropName, flags);

            // Increment count of detected devices for this plugin:
            n++;

            // Increment count of total detected devices for all plugins so far:
            ntotal++;

            if (ntotal >= PSYCH_MAX_VIDSRC - 2) {
                if (PsychPrefStateGet_Verbosity() > 1)
                    printf("PTB-WARNING: Maximum number of allowable video sources during enumeration %i reached! Aborting enumeration.\n", PSYCH_MAX_VIDSRC);
                break;
            }

            // Enumerate next device, if any...
        }

        if ((n == 0) && (PsychPrefStateGet_Verbosity() > 4)) {
            printf("PTB-INFO: Video plugin for class '%s' doesn't provide any video devices to enumerate.\n", class_str);
        }

        // Free list, but afaik does not unref/free the contained GstDevice* elements.
        // Which is good, because we want to keep them (referenced) in our own device
        // list, and free them at Screen() exit time:
        g_list_free(devlist);

        // Done with this plugin - Release provider:
        gst_object_unref(GST_OBJECT(provider));

        return;
    }
    #endif

    // If we reach this point, then the videosource plugin doesn't support straightforward enumeration,
    // so we need to be a bit more hacky here. Try enumeration by trying to access different devices,
    // poking then and see if they give signs of life. For some devices this could go wrong, so we don't
    // probe them but just expose a hard-coded number of devices, which may or may not actually be linked
    // to a real physical device:

    // Create video source plugin:
    videosource = gst_element_factory_make(srcname, "ptb_probevideosource");

    // Nothing to do if no such video plugin available:
    if (!videosource) return;

    n = 0;

    // By default, just add 5 sources per class without actual hw-detection:
    dopoke = 0;
    nmaxp = 5;

    // dc1394src - Probe up to 100 Firewire cameras, with a NULL->PAUSED->NULL transition, as it is fast:
    if (!strcmp(srcname, "dc1394src")) {
        nmaxp = 100;
        dopoke = 2;
    }

    // dv1394src or hdv1394src - Define fixed count of 1 (H)DV camera, as active probing doesn't work well:
    if (strstr(srcname, "dv1394src"))  {
        nmaxp = 1;
        dopoke = 0;
    }

    // Video4Linux2 source - Probe up to 20 cameras, with a NULL->READY->NULL transition:
    if (strstr(srcname, "v4l2")) {
        nmaxp = 20;
        dopoke = 1;
    }

    // Iterate:
    for (i = 0; i < nmaxp; i++) {
        // Only really probe if dopoke:
        if (dopoke) {
            // Set suspected camera id (select i'th camera on bus):
            if (strstr(srcname, "v4l2")) {
                // Video4Linux2 specific path spec:
                sprintf(devices[ntotal].devicePath, "/dev/video%i", i);
                g_object_set(G_OBJECT(videosource), devHandlePropName, devices[ntotal].devicePath, NULL);
            }
            else {
                // Some selection by numeric index:
                if (!strcmp(devHandlePropName, "guid")) {
                    g_object_set(G_OBJECT(videosource), devHandlePropName, (psych_uint64) i, NULL);
                }
                else {
                    g_object_set(G_OBJECT(videosource), devHandlePropName, i, NULL);
                }
            }

            // Try to set it to "paused" state, which should fail if no such
            // camera is connected:
            oldverbose = PsychPrefStateGet_Verbosity();
            PsychPrefStateSet_Verbosity(0);
            if (PsychVideoPipelineSetState(videosource, (dopoke == 2) ? GST_STATE_PAUSED : GST_STATE_READY, 0.1) == GST_STATE_CHANGE_FAILURE) {
                // No such camera connected. Game over, no need to probe further non-existent cams:
                PsychPrefStateSet_Verbosity(oldverbose);
                if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: No camera %i connected to %s video source. Probe finished.\n", i, srcname);
                break;
            }
            PsychPrefStateSet_Verbosity(oldverbose);
        }

        // i'th camera exists. Probe and assign:
        inputIndex = i;
        devices[ntotal].deviceIndex = classIndex * 10000 + inputIndex;
        devices[ntotal].classIndex = classIndex;
        devices[ntotal].inputIndex = inputIndex;
        sprintf(devices[ntotal].deviceClassName, "%s", className);
        sprintf(devices[ntotal].deviceSelectorProperty, "%s", devHandlePropName);
        sprintf(devices[ntotal].deviceVideoPlugin, "%s", srcname);
        if (strstr(srcname, "v4l2")) {
            // Video4Linux2 source: Set the already validated /dev/videoX device path:
            sprintf(devices[ntotal].deviceHandle, "%s", devices[ntotal].devicePath);
            sprintf(devices[ntotal].device, "%s", devices[ntotal].devicePath);
            devices[ntotal].deviceURI = 0;
            pstring = NULL;
            if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "device-name")) {
                //printf("PTB-INFO: PASS II\n");
                g_object_get(G_OBJECT(videosource), "device-name", &pstring, NULL);
            }

            if (pstring) {
                //printf("PTB-INFO: PASS III --> %s\n", pstring);
                sprintf(devices[ntotal].deviceName, "%s", pstring);
                g_free(pstring);
            }
            else {
                sprintf(devices[ntotal].deviceName, "%s", "NODEV");
            }
            pstring = NULL;
        }
        else {
            // Other source: Set numeric handle:
            sprintf(devices[ntotal].devicePath, "%i", inputIndex);
            sprintf(devices[ntotal].deviceHandle, "%i", inputIndex);
            sprintf(devices[ntotal].device, "%i", inputIndex);
            sprintf(devices[ntotal].deviceName, "%i", inputIndex);
            devices[ntotal].deviceURI = inputIndex;
        }

        ntotal++;
        n++;

        // Reset this cam:
        if (dopoke) gst_element_set_state(videosource, GST_STATE_NULL);
        if (dopoke && (PsychPrefStateGet_Verbosity() > 4)) printf("PTB-INFO: %i'th %s camera enumerated.\n", inputIndex, srcname);

        if (ntotal >= PSYCH_MAX_VIDSRC - 2) {
            if (PsychPrefStateGet_Verbosity() > 1)
                printf("PTB-WARNING: Maximum number of allowable video sources during enumeration %i reached! Aborting enumeration.\n", PSYCH_MAX_VIDSRC);
            break;
        }
    }

    // Release videosource:
    gst_element_set_state(videosource, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(videosource));

    // Any success?
    if (n == 0) {
        if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: No video devices to enumerate for plugin '%s'.\n", class_str);
    }

    // Done.
    return;
}

/* PsychGSEnumerateVideoSources(int outPos, int deviceIndex, GstElement **videocaptureplugin);  -- Internal.
 *
 * Enumerates all connected and supported video sources into an internal
 * array "devices".
 *
 * If deviceIndex >= 0 : Returns pointer to PsychVideosourceRecordType struct
 *                       with info about the detected device with index 'deviceIndex'
 *                       or NULL if no such device exists. The pointer is valid until
 *                       the Screen module returns control to the runtime - then it
 *                       will get deallocated and must not be accessed anymore!
 *
 *                       If videocaptureplugin is a non-NULL pointer, caller asks us
 *                       to create associated videocapture GStreamer plugin. If we
 *                       can do that, we return a pointer to it in *videocaptureplugin,
 *                       otherwise we set *videocaptureplugin = NULL.
 *                       We can do this device 'deviceIndex' supports enumeration via
 *                       GstDeviceProvider or GstDeviceMonitor on GStreamer 1.4+
 *
 *
 * If deviceIndex < 0 : Returns NULL to caller, returns a struct array to runtime
 *                      environment return argument position 'outPos' with all info
 *                      about the detected sources.
 */
PsychVideosourceRecordType* PsychGSEnumerateVideoSources(int outPos, int deviceIndex, GstElement **videocaptureplugin)
{
    PsychGenericScriptType 	*devs;
    const char *FieldNames[]={"DeviceIndex", "ClassIndex", "InputIndex", "ClassName", "InputHandle", "Device", "DevicePath", "DeviceName", "GUID", "DevicePlugin", "DeviceSelectorProperty" };

    int                 i;
    GstElement          *videosource = NULL;
    GValueArray         *viddevs = NULL;
    GValue              *dev = NULL;
    char                *device_name = NULL;
    gchar               *pstring = NULL;
    PsychVideosourceRecordType *mydevice = NULL;

    // Make sure GStreamer is ready:
    PsychGSCheckInit("videocapture");

    // Allocate temporary space for enumerated devices:
    devices = (PsychVideosourceRecordType*) PsychCallocTemp(PSYCH_MAX_VIDSRC, sizeof(PsychVideosourceRecordType));
    ntotal  = 0;

    // First use GstDeviceMonitor enumeration as a catch-all for all video sources we don't know
    // how to specifically handle:
    PsychGSEnumerateVideoSourcesViaDeviceMonitor();

    // Linux specific setup path:
    if (PSYCH_SYSTEM == PSYCH_LINUX) {
        // Try Video4Linux-II camera source: This is mostly a Maemo (maybe Meego et al.?) thing.
        PsychGSEnumerateVideoSourceType("v4l2camsrc", 1, "Video4Linux2-CameraSource", "device", "", 0);

        // Try standard Video4Linux-II source:
        PsychGSEnumerateVideoSourceType("v4l2src", 2, "Video4Linux2", "device", "v4l2deviceprovider", 0);
    }

    if (PSYCH_SYSTEM == PSYCH_WINDOWS) {
        // Try Windows kernel streaming source:
        PsychGSEnumerateVideoSourceType("ksvideosrc", 1, "Windows WDM kernel streaming", "device-index", "", 0);

        // Use DirectShow to probe:
        PsychGSEnumerateVideoSourceType("dshowvideosrc", 2, "DirectShow", "device-name", "", 0);
    }

    if (PSYCH_SYSTEM == PSYCH_OSX) {
        // Try OSX Quicktime-7 SequenceGrabber video source: Kind'a pointless as only on 32-Bit and we don't
        // do that anymore. But leave it here for sentimental reasons - fond memories of actually useable
        // videocapture on OSX...
        PsychGSEnumerateVideoSourceType("osxvideosrc", 1, "OSXQuicktimeSequenceGrabber", "device", "", 0);

        // Try OSX AVFoundation video source: The <sarcasm>latest and greatest</sarcasm> for OSX 10.8+ or so.
        // We enumerate this one before qtkitvideosrc, as the latter aka QTKit is deprecated since OSX 10.9.
        // Indeed a first test shows avfvideosrc performing better on OSX 10.9, so i guess Apple does its
        // "break old functionality to shove new api's down the throat of developers" thing again...
        PsychGSEnumerateVideoSourceType("avfvideosrc", 4, "OSXAVFoundationVideoSource", "device-index", "", 0);

        // Try the crappy OSX QTKit video source for 64-Bit systems with Quicktime-X aka QTKit:
        PsychGSEnumerateVideoSourceType("qtkitvideosrc", 2, "OSXQuicktimeKitVideoSource", "device-index", "", 1);

        // Try OSX MIO video source: Unless we're under Octave, where some weird bug/interaction
        // would cause a crash in the miovideosrc plugin if we tried, so we don't try on Octave.
        // Note that this one is not included as of GStreamer 1.4.0, likely because it uses non-public
        // Apple api's, so is probably unsafe to use long-term...
        #ifndef PTBOCTAVE3MEX
        PsychGSEnumerateVideoSourceType("miovideosrc", 3, "OSXMIOVideoSource", "device-name", "", 0);
        #endif
    }

    // Try IIDC-1394 Cameras:
    PsychGSEnumerateVideoSourceType("dc1394src", 7, "1394-IIDC", "camera-number", "", 1);

    // Try GeniCam-Cameras via aravis plugin:
    PsychGSEnumerateVideoSourceType("aravissrc", 8, "GeniCam-Aravis", "camera-name", "", 0);

    // Try DV-Cameras:
    PsychGSEnumerateVideoSourceType("dv1394src", 5, "DV1394", "guid", "", 0);

    // Try HDV-Cameras:
    PsychGSEnumerateVideoSourceType("hdv1394src", 6, "HDV1394", "guid", "", 0);

    // ClassIndex 9 is blocked out for videotestsrc and other weirdo sources.

    // Nothing enumerated?
    if (ntotal <= 0) {
        if (PsychPrefStateGet_Verbosity() > 4) {
            printf("PTB-INFO: Could not detect any supported video devices on this system.\n");
            printf("PTB-INFO: Trying to fake an auto-detected default device and a test video source...\n");
        }

        ntotal = 0;

        // Create a fake entry for the autovideosrc:
        devices[ntotal].deviceIndex = 90000;
        devices[ntotal].classIndex = 9;
        devices[ntotal].inputIndex = 0;
        sprintf(devices[ntotal].deviceVideoPlugin, "%s", "autovideosrc");
        sprintf(devices[ntotal].deviceSelectorProperty, "%s", "");
        sprintf(devices[ntotal].deviceHandle, "%s", "");
        ntotal++;

        // Create fake entries for the videotestsrc:
        for (i = 0; i <= 22; i++) {
            devices[ntotal].deviceIndex = 90001 + i;
            devices[ntotal].classIndex = 9;
            devices[ntotal].inputIndex = 1 + i;
            sprintf(devices[ntotal].deviceVideoPlugin, "%s", "videotestsrc");
            sprintf(devices[ntotal].deviceSelectorProperty, "%s", "");
            sprintf(devices[ntotal].deviceHandle, "%s", "");
            ntotal++;
        }
    }
    else {
        // Found some real ones. However, in any case, always add a fake entry for videotestsrc,
        // so if everything else fails on a setup we can use the synthetic videotestsrc for easy
        // testing:
        devices[ntotal].deviceIndex = 90001;
        devices[ntotal].classIndex = 9;
        devices[ntotal].inputIndex = 1;
        sprintf(devices[ntotal].deviceVideoPlugin, "%s", "videotestsrc");
        sprintf(devices[ntotal].deviceSelectorProperty, "%s", "");
        sprintf(devices[ntotal].deviceHandle, "%s", "");
        ntotal++;
    }

    // Add fake entry for deviceIndex zero, as a copy of the first real entry:
    memcpy(&devices[ntotal], &devices[0], sizeof(PsychVideosourceRecordType));
    devices[ntotal].deviceIndex = 0;
    // For deviceIndex 0: Bump refcount on GstDevice* of associated capture device, if any:
    if (devices[ntotal].gstdevice) gst_object_ref((GstDevice*) devices[ntotal].gstdevice);
    ntotal++;

    // Have enumerated devices:
    if (deviceIndex >= 0) {
        // Yes: Return device name for that index:
        for (i = 0; i < ntotal; i++) {
            if (devices[i].deviceIndex == deviceIndex) {
                // Got it: Return pointer to device struct:
                mydevice = &devices[i];

                // If this one was enumerated via GstDeviceProvider or GstDeviceMonitor,
                // then immediately create the corresponding GstElement* video capture
                // plugin which instantiates the capture device associated with GstDevice*
                // Return pointer to the plugin. Only skip this step if no such plugin
                // requested by caller (unlikely):
                if (videocaptureplugin) {
                    // Default to - "no create support":
                    *videocaptureplugin = NULL;

                    #if GST_CHECK_VERSION(1,4,0)
                    // Caller wants us to create associated video capture plugin if possible:
                    if (devices[i].gstdevice) {
                        // Have one. Create and assign associated plugin:
                        *videocaptureplugin = gst_device_create_element((GstDevice*) devices[i].gstdevice, "ptb_videosource");
                    }
                    #endif
                }
                break;
            }
        }
    }
    else {
        // No: Code wants us to return struct array with all enumerated
        // devices to userspace:

        // Create output struct array with n output slots:
        PsychAllocOutStructArray(outPos, TRUE, ntotal, 11, FieldNames, &devs);

        // Iterate all available devices:
        for(i = 0; i < ntotal; i++) {
            PsychSetStructArrayDoubleElement("DeviceIndex", i, devices[i].deviceIndex, devs);
            PsychSetStructArrayDoubleElement("ClassIndex", i, devices[i].classIndex, devs);
            PsychSetStructArrayDoubleElement("InputIndex", i, devices[i].inputIndex, devs);
            PsychSetStructArrayStringElement("ClassName", i, devices[i].deviceClassName , devs);
            PsychSetStructArrayStringElement("InputHandle", i, devices[i].deviceHandle, devs);
            PsychSetStructArrayStringElement("Device", i, devices[i].device, devs);
            PsychSetStructArrayStringElement("DevicePath", i, devices[i].devicePath, devs);
            PsychSetStructArrayStringElement("DeviceName", i, devices[i].deviceName, devs);
            PsychSetStructArrayStringElement("GUID", i,  (devices[i].deviceURI > 0) ? devices[i].deviceHandle : "0", devs);
            PsychSetStructArrayStringElement("DevicePlugin", i, devices[i].deviceVideoPlugin, devs);
            PsychSetStructArrayStringElement("DeviceSelectorProperty", i, devices[i].deviceSelectorProperty, devs);
        }
    }

    // Need to unref GstDevice* objects on exit, if they are created by GstDeviceMonitor/Provider:
    for (i = 0; i < ntotal; i++) {
        if (devices[i].gstdevice) gst_object_unref((GstDevice*) devices[i].gstdevice);
    }

    // Done. Return device struct if assigned:
    return(mydevice);
}

static gboolean print_field (GQuark field, const GValue * value, gpointer pfx) {
    gchar *str = gst_value_serialize (value);

    printf("%s  %15s: %s\n", (gchar *) pfx, g_quark_to_string (field), str);
    g_free (str);
    return TRUE;
}

psych_bool PsychGSGetResolutionAndFPSForSpec(PsychVidcapRecordType *capdev, int* width, int* height, double* fps, int reqdepth, int reqbitdepth)
{
    GstCaps         *caps = NULL;
    GstStructure    *str;
    gint            qwidth, qheight;
    gint            qbpp;
    gint            twidth = -1, theight = -1;
    gint            maxpixelarea = 0;
    double          tfps = 0.0;
    int             i, nrcandidates = 0;
    float           fpsmin, fpsmax, curfps;
    gint            idx1, fps_n, fps_d, minwidth, minheight;

    // Query caps of videosource and extract supported video capture modes:
    g_object_get(G_OBJECT(capdev->camera), "viewfinder-supported-caps", &caps, NULL);

    if (caps) {
        if (PsychPrefStateGet_Verbosity() > 4)
            printf("PTB-DEBUG: Videosource caps are: %" GST_PTR_FORMAT "\n\n", caps);

        // Iterate through all supported video capture modes:
        for (i = 0; i < (int) gst_caps_get_size(caps); i++) {
            curfps = 0;
            str = gst_caps_get_structure(caps, i);

            // Print all properties of i'th cap in human readable form if wanted:
            if (PsychPrefStateGet_Verbosity() > 5) {
                printf("PTB-DEBUG: %s\n", gst_structure_get_name(str));
                gst_structure_foreach (str, print_field, (gpointer) "PTB-DEBUG: ");
            }

            // Extract maximum supported framerate for given cap. Try if fps is encoded as fraction,
            // list of fractions or a min-max range of fractions. Choose the highest available fps
            // for later use:
            {
                const GValue* framerates = gst_structure_get_value(str, "framerate");

                // framerates can be in the format of a single fraction, a list of fractions, or
                // an allowable range of fractions:
                if (G_VALUE_HOLDS(framerates, gst_fraction_get_type())) {
                    if (gst_structure_get_fraction(str, "framerate", &fps_n, &fps_d)) {
                        if (curfps < (float) fps_n / (float) fps_d) curfps = (float) fps_n / (float) fps_d;
                        if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: Caps %i : FPS %f Hz.\n", i, (float) fps_n / (float) fps_d);
                    }
                }
                else if (G_VALUE_HOLDS(framerates, gst_value_list_get_type())) {
                    for (idx1 = 0; idx1 < (int) gst_value_list_get_size(framerates); idx1++) {
                        const GValue* value = gst_value_list_get_value (framerates, idx1);
                        fps_n = gst_value_get_fraction_numerator(value);
                        fps_d = gst_value_get_fraction_denominator(value);
                        if (curfps < (float) fps_n / (float) fps_d) curfps = (float) fps_n / (float) fps_d;
                        if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: %i-%i : FPS %f Hz.\n", i, idx1, (float) fps_n / (float) fps_d);
                    }
                }
                else if (G_VALUE_HOLDS(framerates, gst_fraction_range_get_type())) {
                    const GValue* frmin = gst_value_get_fraction_range_min(framerates);
                    const GValue* frmax = gst_value_get_fraction_range_max(framerates);
                    fps_n = gst_value_get_fraction_numerator(frmin);
                    fps_d = gst_value_get_fraction_denominator(frmin);
                    fpsmin = (float) fps_n / (float) fps_d;
                    if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: %i: FPS min %f - ", i, fpsmin);

                    fps_n = gst_value_get_fraction_numerator(frmax);
                    fps_d = gst_value_get_fraction_denominator(frmax);
                    fpsmax = (float) fps_n / (float) fps_d;

                    if (curfps < fpsmax) curfps = fpsmax;

                    if (PsychPrefStateGet_Verbosity() > 5) printf("max %f\n", fpsmax);
                }
            }

            // Query of width x height: Set a default of 0 pixels, in case query doesn't return anything.
            // This will fail if width or height are expressed as a valid range of values, but that's fine,
            // because we wouldn't know how to use a range of valid values anyway. It's not useful for auto-
            // detection, as in my experience the minimum is too small (1 x 1 pixel anyone?) and the maximum
            // is too big (defaults to 32k x 32k aka 1 GigaPixels). For pure validation it could have some
            // value if the limits were reasonably tight around what the hardware supports, but this seems
            // to be not worth the trouble.
            qwidth = minwidth = 0;
            gst_structure_get_int(str, "width", &qwidth);
            qheight = minheight = 0;
            gst_structure_get_int(str, "height", &qheight);

            // Valid "scalar' width found? If not, try if width is encoded as a
            // list of possible values:
            if (qwidth == 0) {
                const GValue* value = gst_structure_get_value(str, "width");
                if (G_VALUE_HOLDS(value, gst_int_range_get_type())) {
                    qwidth = gst_value_get_int_range_max(value);
                    minwidth = gst_value_get_int_range_min(value);
                    if (qwidth >= 32767) { qwidth = minwidth = 0; }
                    if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: Caps %i : width [%i - %i].\n", i, minwidth, qwidth);
                }
            }
            else {
                minwidth = qwidth;
            }

            // Valid "scalar' width found? If not, try if height is encoded as a
            // list of possible values:
            if (qheight == 0) {
                const GValue* value = gst_structure_get_value(str, "height");
                if (G_VALUE_HOLDS(value, gst_int_range_get_type())) {
                    qheight = gst_value_get_int_range_max(value);
                    minheight = gst_value_get_int_range_min(value);
                    if (qheight >= 32767) { qheight = minheight = 0; }
                    if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: Caps %i : height [%i - %i].\n", i, minheight, qheight);
                }
            }
            else {
                minheight = qheight;
            }

            // qbpp queried bits per pixel - Usually ends up as -1 "undefined", especially
            // on GStreamer 1.x, so usually useless:
            qbpp = -1;
            gst_structure_get_int(str, "bpp", &qbpp);
            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Videosource cap %i: w = %i h = %i bpp = %i maxfps = %f\n", i, qwidth, qheight, qbpp, curfps);

            // Anything meaningful enumerated? Skip otherwise.
            if ((qwidth == 0) && (qheight == 0)) continue;

            // This (qwidth, qheight) pair is a valid candidate for validation/detection:
            nrcandidates++;

            // Is this detection of default resolution, or validation of a
            // given resolution?
            if ((*width == -1) && (*height == -1)) {
                // Auto-Detection of optimal default resolution.
                // Need to find the one with highest resolution (== pixel area):
                // An additional constraint is that the bits per pixel bpp value of the video mode should be at least as high as the
                // requested bpp = reqbitdepth * reqdepth. However, this check is skipped for YUV formats (reqdepth == 2), requested
                // bit depths reqbitdepth of 8 bpc or lower (consumer class stuff) or if the mode doesn't have a defined bpp, aka qbpp <= 0:
                if ((qwidth * qheight > maxpixelarea) && ((qbpp <= 0) || (reqbitdepth <= 8) || (reqdepth == 2) || (qbpp >= reqbitdepth * reqdepth))) {
                    // A new favorite with max pixel area:
                    maxpixelarea = qwidth * qheight;
                    twidth = qwidth;
                    theight = qheight;
                    tfps = (double) curfps;
                }
            }
            else {
                // Validation: Reject/Skip modes which don't support requested range of resolution:
                if ((((*width < (int) minwidth) || (*width > (int) qwidth)) && (qwidth > 0)) ||
                    (((*height < (int) minheight) || (*height > (int) qheight)) && (qheight > 0))) continue;

                // Check for bitdepths bpc requirements and reject unsatisfying ones - See above for logic:
                if (!((qbpp <= 0) || (reqbitdepth <= 8) || (reqdepth == 2) || (qbpp >= reqbitdepth * reqdepth))) continue;

                // Acceptable mode for requested resolution and framerate. Set it:
                maxpixelarea = (*width) * (*height);
                twidth = *width;
                theight = *height;
                tfps = (double) curfps;
            }
        }

        gst_caps_unref(caps);

        // Any matching mode found?
        if (twidth == -1) {
            // No. Did we have any valid candidates from enumeration and all failed to match?
            if (nrcandidates > 0) {
                // No candidate matched: Requested resolution + fps + pixelformat combo is not supported:
                if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Could not validate video source resolution %i x %i. Returning failure.\n", *width, *height);
                return(FALSE);
            }
            else {
                // Could not enumerate any candidates. Was auto-detection requested?
                if ((*width == -1) && (*height == -1)) {
                    // Yes. This means we failed to detect anything. Return failure, so caller can try to
                    // handle this via fallback methods:
                    if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Could not find any valid modes on video source. Triggering fallback.\n");
                    return(FALSE);
                }
                else {
                    // No. This was supposed to be validation and we couldn't do proper validation.
                    // Be optimistic, return validation success and just hope for the best:
                    if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Could not find any valid modes on video source for validation. Assuming settings are fine.\n");
                    twidth = *width;
                    theight = *height;
                    tfps = *fps;
                }
            }
        }

        // Special case DV video source, which has defined horizontal resolution of 720 pixels,
        // but no defined vertical resolution? Two possible values: 576 (PAL) or 480 (NTSC).
        if ((twidth == 720) && (theight == 0)) {
            // If auto-detection requested, then assume PAL is the correct choice,
            // ie., 576 pixels height. If just validation is requested, just pass-through
            // whatever was passed in if it was 480 or 576 pixels:
            if (*height == -1) {
                // Auto-Detect: Assume 576 pixels PAL:
                if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Special case auto-detect DV video source resolution. Defaulting to 720 x 576 PAL.\n");
                theight = 576;
            }
            else if (*height == 576 || *height == 480) {
                // Potentially valid height value: Accept.
                if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Special case DV video source resolution 720 x %i. Assuming ok.\n", *height);
                theight = *height;
            }
            else {
                // Impossible value: Reject.
                if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Could not validate DV video source resolution %i x %i. Returning failure.\n", *width, *height);
                return(FALSE);
            }
        }

        // Yes. Return settings:
        *fps = tfps;
        *width = twidth;
        *height = theight;

        if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Will use auto-detected or validated video source resolution %i x %i. maxfps = %f\n", *width, *height, tfps);

        return(TRUE);
    } else {
        if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: PsychGSGetResolutionAndFPSForSpec(): Capability query to video source failed!\n");

        // If this was supposed to be auto-detection, just fail and hope for triggered fallback:
        if ((*width == -1) && (*height == -1)) return(FALSE);

        // This was meant to be a validation of given parameters. Just be optimistic and return success,
        // then hope for the best:
        return(TRUE);
    }
}

// Parse codecSpec string, check for certain element specs. If found, parse them
// and create corresponding GstElement*, otherwise return NULL.
GstElement* CreateGStreamerElementFromString(const char* codecSpec, const char* typeSpec, char* codecParams)
{
    char *codecPipelineSpec, *codecPipelineEnd;
    GstElement* element = NULL;

    if (strstr(codecSpec, typeSpec)) {
        // Find start of codec spec string:
        codecPipelineSpec = strstr(codecSpec, typeSpec);

        // Cut off the prefix:
        codecPipelineSpec += strlen(typeSpec);

        // Copy the remaining string, so we can mess with it:
        codecPipelineSpec = strdup(codecPipelineSpec);

        // Search for end-of-codecspec marker:
        codecPipelineEnd = strstr(codecPipelineSpec, ":::");

        // If any, null-terminate at start of marker:
        if (codecPipelineEnd) *codecPipelineEnd = 0;

        // codecPipelineSpec is now a null-terminated string which only
        // contains the GStreamer syntax description of the codec and
        // its parameters.

        // Parse it and create a corresponding bin for use as encoder element:
        // Set GError* to NULL: Real men don't do error handling.
        element = gst_parse_bin_from_description_full((const gchar *) codecPipelineSpec, TRUE, NULL, GST_PARSE_FLAG_FATAL_ERRORS, NULL);
        if (element == NULL) {
            // Oopsie: Failed!
            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: Failed to create an encoder element of type '%s' from the following passed parameter string:\n", typeSpec);
                printf("PTB-WARNING: %s\n", codecPipelineSpec);
                printf("PTB-WARNING: Will revert to default settings for this element. This will likely fail soon...\n");
                printf("PTB-WARNING: Full parameter string was:\n");
                printf("PTB-WARNING: %s\n", codecSpec);
            }
        }
        else {
            // Success!
            if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Element '%s' created from spec '%s'.\n", typeSpec, codecPipelineSpec);
        }
        sprintf(codecParams, "%s", codecPipelineSpec);
        free(codecPipelineSpec);
    }

    return(element);
}

// If encoding profiles are supported, use this function to create one from given specs:
GstEncodingProfile* PsychCreateEncodingProfile(const char* muxer, const char* videocodec, const char* audiocodec)
{
    GstEncodingContainerProfile *prof = NULL;
    GstCaps *caps;

    // Create media container from 'muxer' spec:
    caps = gst_caps_from_string(muxer);
    prof = gst_encoding_container_profile_new("Psychtoolbox audio/video",
                                            "Psychtoolbox recorded audio and video",
                                            caps, muxer_preset_name);
    gst_caps_unref(caps);

    // Create video encoding profile from 'videocodec' spec:
    caps = gst_caps_from_string(videocodec);
    gst_encoding_container_profile_add_profile(prof, (GstEncodingProfile*) gst_encoding_video_profile_new(caps, video_preset_name, NULL, 0));
    gst_caps_unref(caps);

    // Create audio encoding profile from 'audiocodec' spec, if any. Otherwise omit audio encoding:
    if (audiocodec) {
        caps = gst_caps_from_string(audiocodec);
        gst_encoding_container_profile_add_profile(prof, (GstEncodingProfile*) gst_encoding_audio_profile_new(caps, audio_preset_name, NULL, 0));
        gst_caps_unref(caps);
    }

    return((GstEncodingProfile*) prof);
}

// Save all preset values of all plugins (encoders, muxers) contained in 'element' bin under
// the preset name 'preset_name':
void PsychSaveEncodingPresetsForElement(GstElement* element, const char* preset_name)
{
    GValue item = G_VALUE_INIT;
    GstElement* plugin;
    psych_bool done = FALSE;
    GstIterator* it = NULL;

    // Bin or single plugin?
    if (!GST_IS_BIN(element)) {
        // Handling of single elements:
        plugin = element;
        if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: In pipeline: %s presets for encoder plugin: %s\n", GST_IS_PRESET(plugin) ? "Saving" : "No", (const char*) gst_object_get_name(GST_OBJECT(plugin)));
        if (GST_IS_PRESET(plugin)) gst_preset_save_preset(GST_PRESET(plugin), preset_name);
        return;
    }

    // Handling of elements which are bins, ie., containers for other elements:
    it = gst_bin_iterate_all_by_interface(GST_BIN(element), GST_TYPE_PRESET); // Retrieve all plugins in element with GstPreset interface:

    // Iterate over eligible plugins:
    while (it && !done) {
        plugin = NULL;
        switch (gst_iterator_next(it, &item)) {
            case GST_ITERATOR_OK:
                plugin = g_value_peek_pointer(&item);
                if (plugin) {
                    if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: In pipeline: %s presets for encoder plugin: %s\n", GST_IS_PRESET(plugin) ? "Saving" : "No", (const char*) gst_object_get_name(GST_OBJECT(plugin)));

                    // Double check the 'plugin' supports the GstPreset interface. Save its presets if so:
                    if (GST_IS_PRESET(plugin)) gst_preset_save_preset(GST_PRESET(plugin), preset_name);
                    g_value_reset(&item);
                }
                break;

            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(it);
                break;

            case GST_ITERATOR_DONE:
                done = TRUE;
                break;

            default:
                if (plugin) gst_object_unref(plugin);
        }
    }

    g_value_unset(&item);
    if (it) gst_iterator_free(it);
    it = NULL;

    return;
}

psych_bool PsychSetupRecordingPipeFromString(PsychVidcapRecordType* capdev, char* codecSpec, char* outCodecName, psych_bool launchline, psych_bool forCamerabin, psych_bool soundForVideoRecording)
{
    GstElement *camera = NULL;
    GstElement *some_element = NULL;
    GstElement *audio_enc = NULL;
    GstElement *audio_src = NULL;
    GstElement *muxer_elt = NULL;

    char *poption = NULL;
    char *codecName = NULL;
    char *codecSep  = NULL;
    char muxerProfile[1000];
    char audioProfile[1000];
    char muxer[1000];
    char audiocodec[1000];
    char videocodec[1000];
    char codecoption[1000];
    char audiosrc[1000];
    char aactype[20];

    int nrAudioChannels;
    int audioFreq;

    int interlaced = -1;
    int keyFrameInterval = -1;
    float videoQuality = -1;
    float audioQuality = -1;
    int audioBitrate = -1;
    int videoBitrate = -1;
    int bigFiles = -1;
    int fastStart = -1;
    int indexItemsSec = -1;
    int profile = -1;

    // Use GStreamer encoding profiles instead of low-level do-it-yourself setup?
    psych_bool use_profiles = forCamerabin;

    // Require setup of an audio-codec for audio recording or audio track writing?
    psych_bool use_audio = (soundForVideoRecording || strstr(codecSpec, "AddAudioTrack")) ? TRUE : FALSE;

    // Use of audio encoder?
    if (use_audio) {
        // Yes. Try if voaacenc AAC encoder is available, choose it as default choice for
        // AAC encoding. If unavailable, choose ffenc_aac or faac or avenc_aac instead as fallback:
        // Note: As of August 2014 and GStreamer-1.4 for OSX, only vooaacenc and avenc_aac are available,
        // but avenc_aac has rank 0 as isn't ever selected by encodebins auto-plugger, so wouldn't work for
        // camerabin "profile" encoding.
        audio_enc = gst_parse_bin_from_description_full("voaacenc", TRUE, NULL, GST_PARSE_FLAG_FATAL_ERRORS, NULL);
        if (audio_enc) {
            gst_object_unref(G_OBJECT(audio_enc));
            audio_enc = NULL;
            sprintf(aactype, "voaacenc");
        }
        else {
            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: voaacenc audio encoder not available. Using ffenc_aac instead.\n");
            audio_enc = gst_parse_bin_from_description_full("ffenc_aac", TRUE, NULL, GST_PARSE_FLAG_FATAL_ERRORS, NULL);
            if (audio_enc) {
                gst_object_unref(G_OBJECT(audio_enc));
                audio_enc = NULL;
                sprintf(aactype, "ffenc_aac");
            }
            else {
                if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: ffenc_aac audio encoder not available. Using faac instead.\n");
                audio_enc = gst_parse_bin_from_description_full("faac", TRUE, NULL, GST_PARSE_FLAG_FATAL_ERRORS, NULL);
                if (audio_enc) {
                    gst_object_unref(G_OBJECT(audio_enc));
                    audio_enc = NULL;
                    sprintf(aactype, "faac");
                }
                else {
                    if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: faac audio encoder not available. Using avenc_aac instead.\n");
                    sprintf(aactype, "avenc_aac");
                }
            }
        }
    }

    memset(muxerProfile, 0, sizeof(muxerProfile));
    memset(muxer, 0, sizeof(muxer));
    memset(audiosrc, 0, sizeof(audiosrc));
    memset(audiocodec, 0, sizeof(audiocodec));
    memset(audioProfile, 0, sizeof(audioProfile));
    memset(videocodec, 0, sizeof(videocodec));

    // Get camera object - the camerabin1/2:
    camera = capdev->camera;

    // Parse and assign high-level properties if any:
    // Must switch numeric input format to C-Language style, so decimal point
    // gets correctly recognized!
    setlocale(LC_NUMERIC, "C");

    codecSep = strstr(codecSpec, "Profile=");
    if (codecSep) {
        sscanf(codecSep, "Profile=%i", &profile);
    }

    codecSep = strstr(codecSpec, "Interlaced=");
    if (codecSep) {
        sscanf(codecSep, "Interlaced=%i", &interlaced);
    }

    codecSep = strstr(codecSpec, "Keyframe=");
    if (codecSep) {
        sscanf(codecSep, "Keyframe=%i", &keyFrameInterval);
    }

    // Audio bit rate in kBit/Sec
    codecSep = strstr(codecSpec, "Audiobitrate=");
    if (codecSep) {
        sscanf(codecSep, "Audiobitrate=%i", &audioBitrate);
    }

    // Video bit rate in kBit/Sec
    codecSep = strstr(codecSpec, "Videobitrate=");
    if (codecSep) {
        sscanf(codecSep, "Videobitrate=%i", &videoBitrate);
    }

    codecSep = strstr(codecSpec, "Bigfiles=");
    if (codecSep) {
        sscanf(codecSep, "Bigfiles=%i", &bigFiles);
    }

    codecSep = strstr(codecSpec, "Faststart=");
    if (codecSep) {
        sscanf(codecSep, "Faststart=%i", &fastStart);
    }

    codecSep = strstr(codecSpec, "Timeresolution=");
    if (codecSep) {
        sscanf(codecSep, "Timeresolution=%i", &indexItemsSec);
    }

    codecSep = strstr(codecSpec, "Videoquality=");
    if (codecSep) {
        sscanf(codecSep, "Videoquality=%f", &videoQuality);
    }

    codecSep = strstr(codecSpec, "Audioquality=");
    if (codecSep) {
        sscanf(codecSep, "Audioquality=%f", &audioQuality);
    }

    // Create matching video encoder for codecSpec:
    // Codecs are sorted by suitability for high quality realtime video recording, so if usercode
    // doesn't specify a codec, and thereby requests use of the recommended default codec 'DEFAULTenc',
    // we will try to choose the best codec, then on failure fallback to the 2nd best, 3rd best, etc.

    // Video encoder from parameter string?
    some_element = NULL;
    if ((use_profiles && strstr(codecSpec, "VideoCodec=")) || ((some_element = CreateGStreamerElementFromString(codecSpec, "VideoCodec=", videocodec)) != NULL)) {
        // Yes. Assign it as our encoder:
        capdev->videoenc = some_element;
        if (!capdev->videoenc && use_profiles) capdev->videoenc = (GstElement*) 0x1;

        // Need to extract the actual name of the codec from the codecSpec string:
        codecName = strstr(codecSpec, "VideoCodec=");
        codecName+= strlen("VideoCodec=");
        codecName = strdup(codecName);
        codecSep = codecName;
        while ((*codecSep > 0) && (*codecSep != ' ')) codecSep++;
        *codecSep = 0;

        sprintf(outCodecName, "%s", codecName);
        free(codecName);
        codecName = NULL;
    }

    // Start with H264 encoder as default:
    if (strstr(codecSpec, "x264enc") || strstr(codecSpec, "1635148593") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
        // Define recommended (compatible) audioencoder/muxer and their default options:
        sprintf(audioProfile, "audio/mpeg,mpegversion=4");
        sprintf(muxerProfile, "video/x-msvideo");
        sprintf(outCodecName, "video/x-h264");
        sprintf(audiocodec, "AudioCodec=%s ", aactype); // Need to use faac MPEG-4 audio encoder.
        sprintf(muxer, "qtmux");                        // Need to use Quicktime-Multiplexer.

        // Videoencoder not yet created? If so, we have to do it now:
        if (!capdev->videoenc) {
            // Not yet created. Create full codec & option string from high level properties,
            // if any, then create based on string:
            sprintf(videocodec, "VideoCodec=x264enc ");

            // Interlace flag specified?
            if (interlaced >= 0) {
                // Enable or disable handling of interlaced input video stream:
                sprintf(codecoption, "interlaced=%i ", (interlaced) ? 1 : 0);
                strcat(videocodec, codecoption);
            }

            // Keyframe interval specified?
            if (keyFrameInterval >= 0) {
                // Assign maximum distance between key frames:
                sprintf(codecoption, "key-int-max=%i ", keyFrameInterval);
                strcat(videocodec, codecoption);
            } else {
                // Default to a keyframe at least every 30 frames:
                strcat(videocodec, "key-int-max=30 ");
            }

            // Quality vs. Speed tradeoff specified?
            if (videoQuality >= 0) {
                // Yes: Map quality vs. speed scalar to 0-10 number for speed preset:
                if (videoQuality > 1) videoQuality = 1;

                // Speed-Quality profile: 0 = "None"
                // 1 = "Ultra fast", 2 = "Super fast", 3 = "Very fast",  4 = "Faster", 5 = "Fast",
                // 6 = "Medium" - the default,7 = "Slow", 8 = "Slower", 9 = "Very slow", 10 = "Placebo"
                sprintf(codecoption, "speed-preset=%i ", (int) (videoQuality * 10.0 + 0.5));
                strcat(videocodec, codecoption);
            }
            else {
                // No: Use the fastest speed at lowest quality:
                strcat(videocodec, "speed-preset=1 ");
            }

            // Bitrate specified?
            if (videoBitrate >= 0) {
                sprintf(codecoption, "bitrate=%i ", (int) videoBitrate);
                strcat(videocodec, codecoption);
            }

            // Create videocodec from options string:
            capdev->videoenc = CreateGStreamerElementFromString(videocodec, "VideoCodec=", videocodec);

            // This must go last, after CreateGStreamerElementFromString()! Encoding profile specified?
            if (profile >= 0) {
                // Assign profile:
                switch (profile) {
                    case 0:
                        sprintf(codecoption, " ! video/x-h264, profile=constrained-baseline");
                        break;

                    case 1:
                        sprintf(codecoption, " ! video/x-h264, profile=baseline");
                        break;

                    case 2:
                        sprintf(codecoption, " ! video/x-h264, profile=main");
                        break;

                    case 3:
                        sprintf(codecoption, " ! video/x-h264, profile=high");
                        break;

                    case 4:
                        sprintf(codecoption, " ! video/x-h264, profile=high-10");
                        break;

                    case 5:
                        sprintf(codecoption, " ! video/x-h264, profile=high-4:2:2");
                        break;

                    case 6:
                        sprintf(codecoption, " ! video/x-h264, profile=high-4:4:4");
                        break;

                    case 7:
                        sprintf(codecoption, " ! video/x-h264, profile=high-10-intra");
                        break;

                    case 8:
                        sprintf(codecoption, " ! video/x-h264, profile=high-4:2:2-intra");
                        break;

                    case 9:
                        sprintf(codecoption, " ! video/x-h264, profile=high-4:4:4-intra");
                        break;

                    default:
                        printf("PTB-WARNING: Invalid 'Profile=' option value for H264 encoding! Valid values must be between 0 and 9. Ignoring option and using default.\n");
                        codecoption[0] = 0;
                }
                strcat(videocodec, codecoption);
            }

            // If specific codec couldn't be created but encoding profiles are in use then we "fake" a
            // 0x1 codec to suppress warning messages and error/fallback handling. The encoding profile
            // will take care of this, we just can't set specific encoding options:
            if (!capdev->videoenc && use_profiles) capdev->videoenc = (GstElement*) 0x1;
        }

        if (!capdev->videoenc) {
            printf("PTB-WARNING: Failed to create 'x264enc' H.264 video encoder! Does not seem to be installed on your system?\n");
        }
        else {
            if (!use_profiles) sprintf(outCodecName, "x264enc");
        }
    }

    // Then xvidenc - MPEG4 in a AVI container: High quality, handles 640 x 480 @ 30 fps on
    // a 4 year old MacBookPro Core2Duo 2.2 Ghz with about 70% - 100% cpu load, depending on settings.
    if (strstr(codecSpec, "xvidenc") || strstr(codecSpec, "1836070006") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
        // Define recommended (compatible) audioencoder/muxer and their default options:
        sprintf(audioProfile, "audio/mpeg,mpegversion=4");
        sprintf(muxerProfile, "video/x-msvideo");
        sprintf(outCodecName, "video/x-xvid");
        sprintf(audiocodec, "AudioCodec=%s ", aactype); // Need to use faac MPEG-4 audio encoder.
        sprintf(muxer, "avimux");                // Need to use avi-multiplexer.

        // Videoencoder not yet created? If so, we have to do it now:
        if (!capdev->videoenc) {
            // Not yet created. Create full codec & option string from high level properties,
            // if any, then create based on string:
            sprintf(videocodec, "VideoCodec=xvidenc ");

            // Interlace flag specified?
            if (interlaced >= 0) {
                // Enable or disable handling of interlaced input video stream:
                sprintf(codecoption, "interlaced=%i ", (interlaced) ? 1 : 0);
                strcat(videocodec, codecoption);
            }

            // Keyframe interval specified?
            if (keyFrameInterval >= 0) {
                // Assign maximum distance between key frames:
                sprintf(codecoption, "max-key-interval=%i ", keyFrameInterval);
                strcat(videocodec, codecoption);
            }

            // Quality vs. Speed tradeoff specified?
            if (videoBitrate >= 0) {
                sprintf(codecoption, "bitrate=%i ", (int) videoBitrate * 1024);
                strcat(videocodec, codecoption);
            }

            // Create videocodec from options string:
            capdev->videoenc = CreateGStreamerElementFromString(videocodec, "VideoCodec=", videocodec);
            if (!capdev->videoenc && use_profiles) capdev->videoenc = (GstElement*) 0x1;
        }

        if (!capdev->videoenc) {
            printf("PTB-WARNING: Failed to create 'xvidenc' xvid/mpeg-4 video encoder! Does not seem to be installed on your system?\n");
        }
        else {
            if (!use_profiles) sprintf(outCodecName, "xvidenc");
        }
    }

    // avenc_mpeg4 also creates MPEG4, but at lower quality - much more blocky etc.
    if (strstr(codecSpec, "avenc_mpeg4") || ((strstr(codecSpec, "DEFAULTenc") || strstr(codecSpec, "1836070006")) && !capdev->videoenc)) {
        // Define recommended (compatible) audioencoder/muxer and their default options:
        sprintf(audioProfile, "audio/mpeg,mpegversion=4");
        sprintf(muxerProfile, "video/x-msvideo");
        sprintf(outCodecName, "video/mpeg,mpegversion=4");
        sprintf(audiocodec, "AudioCodec=%s ", aactype); // Need to use faac MPEG-4 audio encoder.
        sprintf(muxer, "avimux");                // Need to use avi-multiplexer.

        // Videoencoder not yet created? If so, we have to do it now:
        if (!capdev->videoenc) {
            // Not yet created. Create full codec & option string from high level properties,
            // if any, then create based on string:
            sprintf(videocodec, "VideoCodec=avenc_mpeg4 ");

            // Interlace flag specified?
            if (interlaced >= 0) {
                // Enable or disable handling of interlaced input video stream:
                sprintf(codecoption, "interlaced=%i ", (interlaced) ? 1 : 0);
                strcat(videocodec, codecoption);
            }

            // Keyframe interval specified?
            if (keyFrameInterval >= 0) {
                // Assign maximum distance between key frames:
                sprintf(codecoption, "max-key-interval=%i ", keyFrameInterval);
                strcat(videocodec, codecoption);
            }

            // Quality vs. Speed tradeoff specified?
            if (videoBitrate >= 0) {
                sprintf(codecoption, "bitrate=%i ", (int) videoBitrate * 1024);
                strcat(videocodec, codecoption);
            }

            // Create videocodec from options string:
            capdev->videoenc = CreateGStreamerElementFromString(videocodec, "VideoCodec=", videocodec);
            if (!capdev->videoenc && use_profiles) capdev->videoenc = (GstElement*) 0x1;
        }

        if (!capdev->videoenc) {
            printf("PTB-WARNING: Failed to create 'avenc_mpeg4' mpeg-4 video encoder! Does not seem to be installed on your system?\n");
        }
        else {
            if (!use_profiles) sprintf(outCodecName, "avenc_mpeg4");
        }
    }

    // Theora + Ogg vorbis audio in .ogv container:
    if (strstr(codecSpec, "theoraenc") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
        // Define recommended (compatible) audioencoder/muxer and their default options:
        sprintf(audioProfile, "audio/x-vorbis");
        sprintf(muxerProfile, "application/ogg");
        sprintf(outCodecName, "video/x-theora");
        sprintf(audiocodec, "AudioCodec=vorbisenc "); // Need to use Ogg Vorbis audio encoder.
        sprintf(muxer, "oggmux");                // Need to use Ogg-multiplexer.

        // Videoencoder not yet created? If so, we have to do it now:
        if (!capdev->videoenc) {
            // Not yet created. Create full codec & option string from high level properties,
            // if any, then create based on string:
            sprintf(videocodec, "VideoCodec=theoraenc drop-frames=0 speed-level=2 ");

            // Keyframe interval specified?
            if (keyFrameInterval >= 0) {
                // Assign maximum distance between key frames:
                sprintf(codecoption, "keyframe-auto=0 keyframe-force=%i ", keyFrameInterval);
                strcat(videocodec, codecoption);
            }

            // Quality vs. Speed tradeoff specified?
            if (videoQuality >= 0) {
                // Yes: Map quality vs. speed scalar to 0-63 number:
                if (videoQuality > 1) videoQuality = 1;

                sprintf(codecoption, "quality=%i ", (int) (videoQuality * 63.0 + 0.5));
                strcat(videocodec, codecoption);
            }

            // Quality vs. Speed tradeoff specified?
            if (videoBitrate >= 0) {
                sprintf(codecoption, "bitrate=%i ", (int) videoBitrate);
                strcat(videocodec, codecoption);
            }

            // Create videocodec from options string:
            capdev->videoenc = CreateGStreamerElementFromString(videocodec, "VideoCodec=", videocodec);
            if (!capdev->videoenc && use_profiles) capdev->videoenc = (GstElement*) 0x1;
        }

        if (!capdev->videoenc) {
            printf("PTB-WARNING: Failed to create 'theoraenc' video encoder! Does not seem to be installed on your system?\n");
        }
        else {
            if (!use_profiles) sprintf(outCodecName, "theoraenc");
        }
    }

    // VP-8 in WebM or Matroska container:
    if (strstr(codecSpec, "vp8enc") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
        // Define recommended (compatible) audioencoder/muxer and their default options:
        sprintf(audioProfile, "audio/x-vorbis");
        sprintf(muxerProfile, "video/webm");
        sprintf(outCodecName, "video/x-vp8");
        sprintf(audiocodec, "AudioCodec=vorbisenc "); // Need to use Ogg Vorbis audio encoder.
        sprintf(muxer, "webmmux");                    // Default to WebM multiplexer.

        // Need to use matroska/webm-multiplexer:
        if (strstr(codecSpec, "_matroska")) {
            sprintf(muxer, "matroskamux");
            sprintf(muxerProfile, "video/x-matroska");
        }

        if (strstr(codecSpec, "_webm")) {
            sprintf(muxer, "webmmux");
            sprintf(muxerProfile, "video/webm");
        }

        // Videoencoder not yet created? If so, we have to do it now:
        if (!capdev->videoenc) {
            // Not yet created. Create full codec & option string from high level properties,
            // if any, then create based on string:
            sprintf(videocodec, "VideoCodec=vp8enc ");

            // Keyframe interval specified?
            if (keyFrameInterval >= 0) {
                // Assign maximum distance between key frames:
                sprintf(codecoption, "max-keyframe-distance=%i ", keyFrameInterval);
                strcat(videocodec, codecoption);
            }

            // Quality vs. Speed tradeoff specified?
            if (videoQuality >= 0) {
                // Yes: Map quality vs. speed scalar to 0-10 number:
                if (videoQuality > 1) videoQuality = 1;

                sprintf(codecoption, "quality=%i ", (int) (videoQuality * 10.0 + 0.5));
                strcat(videocodec, codecoption);
            }

            // Quality vs. Speed tradeoff specified?
            if (videoBitrate >= 0) {
                sprintf(codecoption, "bitrate=%i ", (int) videoBitrate * 1024);
                strcat(videocodec, codecoption);
            }

            // Create videocodec from options string:
            capdev->videoenc = CreateGStreamerElementFromString(videocodec, "VideoCodec=", videocodec);
            if (!capdev->videoenc && use_profiles) capdev->videoenc = (GstElement*) 0x1;
        }

        if (!capdev->videoenc) {
            printf("PTB-WARNING: Failed to create 'vp8enc' VP-8 video encoder! Does not seem to be installed on your system?\n");
        }
        else {
            if (!use_profiles) sprintf(outCodecName, "vp8enc");
        }
    }

    // Raw YUV:
    if (strstr(codecSpec, "yuvraw") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
        // Define recommended (compatible) audioencoder/muxer and their default options:
        sprintf(audioProfile, "audio/mpeg,mpegversion=4");
        sprintf(muxerProfile, "video/x-msvideo");
        sprintf(outCodecName, "video/x-raw-yuv");
        sprintf(audiocodec, "AudioCodec=%s ", aactype); // Need to use faac MPEG-4 audio encoder.
        sprintf(muxer, "avimux");                // Need to use AVI-Multiplexer.

        // Videoencoder not yet created? If so, we have to do it now:
        if (!capdev->videoenc) {
            // Not yet created. Create full codec & option string from high level properties,
            // if any, then create based on string:
            sprintf(videocodec, "VideoCodec=capsfilter caps=\"video/x-raw-yuv, format=I420, width=(int)640, height=(int)480\" ");

            // Create videocodec from options string:
            capdev->videoenc = CreateGStreamerElementFromString(videocodec, "VideoCodec=", videocodec);
            if (!capdev->videoenc && use_profiles) capdev->videoenc = (GstElement*) 0x1;
        }

        if (!capdev->videoenc) {
            printf("PTB-WARNING: Failed to create 'capsfilter' YUV pass-through video encoder! Does not seem to be installed on your system?\n");
        }
        else {
            if (!use_profiles) sprintf(outCodecName, "yuvraw");
        }
    }

    // H263 -- Does not work well yet.
    if (strstr(codecSpec, "avenc_h263p") || strstr(codecSpec, "1748121139") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
        // Define recommended (compatible) audioencoder/muxer and their default options:
        sprintf(audioProfile, "audio/mpeg,mpegversion=4");
        sprintf(muxerProfile, "video/x-msvideo");
        sprintf(outCodecName, "video/x-h263");
        sprintf(audiocodec, "AudioCodec=%s ", aactype); // Need to use faac MPEG-4 audio encoder.
        sprintf(muxer, "qtmux");                 // Need to use Quicktime-Multiplexer.

        // Videoencoder not yet created? If so, we have to do it now:
        if (!capdev->videoenc) {
            // Not yet created. Create full codec & option string from high level properties,
            // if any, then create based on string:
            sprintf(videocodec, "VideoCodec=avenc_h263p ");

            // Interlace flag specified?
            if (interlaced >= 0) {
                // Enable or disable handling of interlaced input video stream:
                sprintf(codecoption, "interlaced=%i ", (interlaced) ? 1 : 0);
                strcat(videocodec, codecoption);
            }

            // Keyframe interval specified?
            if (keyFrameInterval >= 0) {
                // Assign maximum distance between key frames:
                sprintf(codecoption, "max-key-interval=%i ", keyFrameInterval);
                strcat(videocodec, codecoption);
            }

            // Quality vs. Speed tradeoff specified?
            if (videoBitrate >= 0) {
                sprintf(codecoption, "bitrate=%i ", (int) videoBitrate * 1024);
                strcat(videocodec, codecoption);
            }

            // Create videocodec from options string:
            capdev->videoenc = CreateGStreamerElementFromString(videocodec, "VideoCodec=", videocodec);
            if (!capdev->videoenc && use_profiles) capdev->videoenc = (GstElement*) 0x1;
        }

        if (!capdev->videoenc) {
            printf("PTB-WARNING: Failed to create 'avenc_h263p' H.263 video encoder! Does not seem to be installed on your system?\n");
        }
        else {
            if (!use_profiles) sprintf(outCodecName, "avenc_h263p");
        }
    }

    // Raw Huffman encoded YUV: Works with camerabin2, and with libdc1394 video recording and Screen movie writing:
    // This is lossless for 1-channel gray and 3-channel RGB, but only encodes 8 bpc content so far.
    if (strstr(codecSpec, "huffyuv") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
        // Define recommended (compatible) audioencoder/muxer and their default options:
        sprintf(audioProfile, "audio/mpeg,mpegversion=4");
        sprintf(muxerProfile, "video/x-msvideo");
        sprintf(outCodecName, "video/x-huffyuv");
        sprintf(audiocodec, "AudioCodec=%s ", aactype); // Need to use faac MPEG-4 audio encoder.
        sprintf(muxer, "avimux");           // Need to use AVI-Multiplexer.

        // Videoencoder not yet created? If so, we have to do it now:
        if (!capdev->videoenc) {
            // Not yet created. Create full codec & option string from high level properties,
            // if any, then create based on string:
            sprintf(videocodec, "VideoCodec=avenc_huffyuv ");

            // Keyframe interval specified?
            if (keyFrameInterval >= 0) {
                // Assign maximum distance between key frames:
                sprintf(codecoption, "gop-size=%i ", keyFrameInterval);
                strcat(videocodec, codecoption);
            }

            // Quality vs. Speed tradeoff specified?
            if (videoBitrate >= 0) {
                sprintf(codecoption, "bitrate=%i ", (int) videoBitrate * 1024);
                strcat(videocodec, codecoption);
            }

            // Create videocodec from options string:
            capdev->videoenc = CreateGStreamerElementFromString(videocodec, "VideoCodec=", videocodec);
            if (!capdev->videoenc && use_profiles) capdev->videoenc = (GstElement*) 0x1;
        }

        if (!capdev->videoenc) {
            printf("PTB-WARNING: Failed to create 'avenc_huffyuv' compressed YUV lossless video encoder! Does not seem to be installed on your system?\n");
        }
        else {
            if (!use_profiles) sprintf(outCodecName, "avenc_huffyuv");
        }
    }

    // LJPEG image encoding: This is lossless but can only store 8 bpc content, and there's no decoder in GStreamer-0.10!
    if (strstr(codecSpec, "avenc_ljpeg") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
        // Define recommended (compatible) audioencoder/muxer and their default options:
        sprintf(audioProfile, "audio/mpeg,mpegversion=4");
        sprintf(muxerProfile, "video/x-msvideo");
        sprintf(outCodecName, "image/jpeg");
        sprintf(audiocodec, "AudioCodec=%s ", aactype); // Need to use faac MPEG-4 audio encoder.
        sprintf(muxer, "avimux"); // Use AVI-Multiplexer. Quicktime would also work.

        // Videoencoder not yet created? If so, we have to do it now:
        if (!capdev->videoenc) {
            // Not yet created. Create full codec & option string from high level properties,
            // if any, then create based on string:
            sprintf(videocodec, "VideoCodec=avenc_ljpeg ");

            // Keyframe interval specified?
            if (keyFrameInterval >= 0) {
                // Assign maximum distance between key frames:
                sprintf(codecoption, "gop-size=%i ", keyFrameInterval);
                strcat(videocodec, codecoption);
            }

            // Quality vs. Speed tradeoff specified?
            if (videoBitrate >= 0) {
                sprintf(codecoption, "bitrate=%i ", (int) videoBitrate * 1024);
                strcat(videocodec, codecoption);
            }

            // Create videocodec from options string:
            capdev->videoenc = CreateGStreamerElementFromString(videocodec, "VideoCodec=", videocodec);
            if (!capdev->videoenc && use_profiles) capdev->videoenc = (GstElement*) 0x1;
        }

        if (!capdev->videoenc) {
            printf("PTB-WARNING: Failed to create 'avenc_ljpeg' compressed JPEG lossless video encoder! Does not seem to be installed on your system?\n");
        }
        else {
            if (!use_profiles) sprintf(outCodecName, "avenc_ljpeg");
        }
    }

    // SGI-RLE image encoding: This is lossless and RLE compressed but can only store 8 bpc content, and we'll lose movie info like duration or framerate!
    // Essentially the sequency of images is simply stored in a container for separate image files. Accessible by GStreamer or ffmpeg, but not a movie per se.
    // Audio may not work - untested.
    if (strstr(codecSpec, "avenc_sgi") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
        // Define recommended (compatible) audioencoder/muxer and their default options:
        sprintf(audioProfile, "audio/mpeg,mpegversion=4");
        sprintf(muxerProfile, "multipart/x-mixed-replace");
        sprintf(outCodecName, "image/x-sgi");
        sprintf(audiocodec, "AudioCodec=%s ", aactype); // Need to use faac MPEG-4 audio encoder.
        sprintf(muxer, "multipartmux"); // Use Multipart-Multiplexer. Will lose fps and movie duration!

        // Videoencoder not yet created? If so, we have to do it now:
        if (!capdev->videoenc) {
            // Not yet created. Create full codec & option string from high level properties,
            // if any, then create based on string:
            sprintf(videocodec, "VideoCodec=avenc_sgi ");

            // Keyframe interval specified?
            if (keyFrameInterval >= 0) {
                // Assign maximum distance between key frames:
                sprintf(codecoption, "gop-size=%i ", keyFrameInterval);
                strcat(videocodec, codecoption);
            }

            // Quality vs. Speed tradeoff specified?
            if (videoBitrate >= 0) {
                sprintf(codecoption, "bitrate=%i ", (int) videoBitrate * 1024);
                strcat(videocodec, codecoption);
            }

            // Create videocodec from options string:
            capdev->videoenc = CreateGStreamerElementFromString(videocodec, "VideoCodec=", videocodec);
            if (!capdev->videoenc && use_profiles) capdev->videoenc = (GstElement*) 0x1;
        }

        if (!capdev->videoenc) {
            printf("PTB-WARNING: Failed to create 'avenc_sgi' compressed SGI-RLE lossless video encoder! Does not seem to be installed on your system?\n");
        }
        else {
            if (!use_profiles) sprintf(outCodecName, "avenc_sgi");
        }
    }

    // y4menc raw image encoding: This is near-lossless for luminance only, somewhat lossy for color, but can only store 8 bit YUV content.
    // Audio may not work - untested.
    if (strstr(codecSpec, "y4menc") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
        // Define recommended (compatible) audioencoder/muxer and their default options:
        sprintf(audioProfile, "audio/mpeg,mpegversion=4");
        sprintf(muxerProfile, "application/x-yuv4mpeg");
        sprintf(outCodecName, "video/x-raw-yuv");
        sprintf(audiocodec, "AudioCodec=%s ", aactype); // Need to use faac MPEG-4 audio encoder.
        sprintf(muxer, "y4menc"); // Use y4menc-Multiplexer. Encodes to MJPEG style YUV.

        // Videoencoder not yet created? If so, we have to do it now:
        if (!capdev->videoenc) {
            // Not yet created. Create full codec & option string from high level properties,
            // if any, then create based on string:
            sprintf(videocodec, "VideoCodec=identity ");

            // Create videocodec from options string:
            capdev->videoenc = CreateGStreamerElementFromString(videocodec, "VideoCodec=", videocodec);
            if (!capdev->videoenc && use_profiles) capdev->videoenc = (GstElement*) 0x1;
        }

        if (!capdev->videoenc) {
            printf("PTB-WARNING: Failed to create 'y4menc' YUV4MPEG near-lossless video encoder! Does not seem to be installed on your system?\n");
        }
        else {
            if (!use_profiles) sprintf(outCodecName, "identity");
        }
    }

    // Try then Apple OSX specific H264 encoder: Rank 0 as of GStreamer-1.4, so only suitable for
    // non-camerabin / non-encoding-profile recording:
    if (strstr(codecSpec, "vtenc_h264") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
        // Define recommended (compatible) audioencoder/muxer and their default options:
        sprintf(audioProfile, "audio/mpeg,mpegversion=4");
        sprintf(muxerProfile, "video/x-msvideo");
        sprintf(outCodecName, "video/x-h264");
        sprintf(audiocodec, "AudioCodec=%s ", aactype); // Need to use faac MPEG-4 audio encoder.
        sprintf(muxer, "qtmux");                 // Need to use Quicktime-Multiplexer.

        // Videoencoder not yet created? If so, we have to do it now:
        if (!capdev->videoenc) {
            // Not yet created. Create full codec & option string from high level properties,
            // if any, then create based on string:
            sprintf(videocodec, "VideoCodec=vtenc_h264 ");

            // Bitrate specified?
            if (videoBitrate >= 0) {
                sprintf(codecoption, "bitrate=%i ", (int) videoBitrate);
                strcat(videocodec, codecoption);
            }

            // Create videocodec from options string:
            capdev->videoenc = CreateGStreamerElementFromString(videocodec, "VideoCodec=", videocodec);
            if (!capdev->videoenc && use_profiles) capdev->videoenc = (GstElement*) 0x1;
        }

        if (!capdev->videoenc) {
            printf("PTB-WARNING: Failed to create 'vtenc_h264' H.264 MacOSX specific video encoder! Does not seem to be installed on your system?\n");
        }
        else {
            if (!use_profiles) sprintf(outCodecName, "vtenc_h264");
        }
    }

    // Use of audio encoding for any means requested? If so, perform codec and audio source setup:
    if (use_audio) {
        // Audio encoder from parameter string? Or new-style encoding profiles in use?

        // Encoding profiles in use? Then we need to parse optional AudioProfile= string ourselves for the MIME spec,
        // unless none specified. In that case audioProfile will already contain the auto-setup MIME string from
        // video codec setup above.
        if (use_profiles && strstr(codecSpec, "AudioProfile=")) {
            poption = strstr(codecSpec, "AudioProfile=");
            poption+= strlen("AudioProfile=");

            // Store audiocodec name in 'audiocodec':
            sprintf(audioProfile, "%s", poption);

            // Terminate audiocodec string if a ::: marker is encountered:
            poption = strstr(audioProfile, ":::");
            if (poption) *poption = 0;

            // audioProfile now contains audio profile MIME spec.
            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Selecting audio encoder according to: %s\n", audioProfile);
        }

        if ((audio_enc = CreateGStreamerElementFromString(codecSpec, "AudioCodec=", audiocodec)) != NULL) {
            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Created audio encoder (I) according to: %s\n", audiocodec);
        } else {
            // No: Build from preset as defined by video codec and some high level settings:
            // This path is hit if encoding-profiles aren't used, so we need to do our own
            // codec setup (ie., camerabin1 or classic movie writing) and usercode didn't
            // specify a codec and its settings manually via "AudioCodec=" parameter.

            // Audio quality flag specified?
            if (audioQuality >= 0) {
                if (audioQuality > 1) audioQuality = 1;

                if (strstr(audiocodec, "faac") || strstr(audiocodec, "enc_aac") || strstr(audiocodec, "voaacenc")) {
                    // Map quality range 0.0 - 1.0 to bitrate range 0 - 320000 bits/sec:
                    sprintf(codecoption, "bitrate=%i ", (int) (audioQuality * 320000));
                    strcat(audiocodec, codecoption);
                }

                if (strstr(audiocodec, "vorbisenc")) {
                    // Assign quality range 0.0 - 1.0 directly:
                    sprintf(codecoption, "quality=%f ", audioQuality);
                    strcat(audiocodec, codecoption);
                }
            }

            // Audio bitrate specified?
            if (audioBitrate >= 0) {
                if (strstr(audiocodec, "faac") || strstr(audiocodec, "enc_aac") || strstr(audiocodec, "voaacenc")) {
                    sprintf(codecoption, "bitrate=%i ", audioBitrate * 1000);
                    strcat(audiocodec, codecoption);
                }

                if (strstr(audiocodec, "vorbisenc")) {
                    sprintf(codecoption, "managed=1 bitrate=%i ", audioBitrate * 1000);
                    strcat(audiocodec, codecoption);
                }
            }

            // Create audio encoder:
            if ((audio_enc = CreateGStreamerElementFromString(audiocodec, "AudioCodec=", audiocodec)) != NULL) {
                // Yes: Assign it.
                if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Created audio encoder (II) according to: %s\n", audiocodec);
            } else {
                if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Failed to create requested audio encoder [%s]! Falling back to default encoder%s.\n", audiocodec, (use_profiles) ? " for profile" : "");
            }
        }

        // Audio source from parameter string? Most of the time usercode would not want
        // to change type/assignment/source parameters of a source. Also, the available
        // settings are common enough across sources that it doesn't make sense for us to
        // introduce our own high-level parameters. Therefore we only provide the low-level
        // interface here.
        // Interesting sources would be: alsasrc, osssrc, oss4src, pulsesrc, jackaudiosrc,
        // autoaudiosrc, gconfaudiosrc.
        //
        // Interesting settings would be:
        // device = The audio device name.
        // server = Audio server name for Jack and PulseAudio.
        // slave-method = Type of syncing to master clock.
        if (soundForVideoRecording) audio_src = CreateGStreamerElementFromString(codecSpec, "AudioSource=", audiosrc);
    }
    else {
        // No audio encoding/writing: Disable all audio related stuff.
        audio_src = NULL;
        audio_enc = NULL;
        audiocodec[0] = 0;
        audioProfile[0] = 0;
    }

    // Multiplexer from parameter string?
    if (strstr(codecSpec, "Muxer=")) {
        // Must create it without help of CreateGStreamerElementFromString() as the muxer has
        // 2 sink pads, but our method can only handle 1 sink pad. Therefore just create a
        // muxer by name, setting it to its default settings:
        poption = strstr(codecSpec, "Muxer=");
        poption+= strlen("Muxer=");

        // Store muxer name in 'muxer':
        sprintf(muxer, "%s", poption);

        // Terminate muxer string if a ::: marker is encountered:
        poption = strstr(muxer, ":::");
        if (poption) *poption = 0;

        // muxer now contains muxer spec.
    }

    // Muxer MIME profile specified?
    if (strstr(codecSpec, "MuxerProfile=")) {
        poption = strstr(codecSpec, "MuxerProfile=");
        poption+= strlen("MuxerProfile=");

        // Store muxer name in 'muxer':
        sprintf(muxerProfile, "%s", poption);

        // Terminate muxer string if a ::: marker is encountered:
        poption = strstr(muxerProfile, ":::");
        if (poption) *poption = 0;

        // muxerProfile now contains container spec.
    }

    // Build multiplexer from 'muxer' spec:
    muxer_elt = gst_element_factory_make(muxer, "ptbvideomuxer0");

    // Special muxer-specific setup of mux parameters:
    if (strstr(muxer, "avimux")) {
        // Big file writing support (> 2GB) is on by default. This allows to change it:
        if (bigFiles >= 0) {
            g_object_set(muxer_elt, "bigfile", (bigFiles > 0) ? 1 : 0, NULL);
            sprintf(codecoption, " bigfile=%i", (bigFiles > 0) ? 1 : 0);
            strcat(muxer, codecoption);
        }
    }

    if (strstr(muxer, "qtmux")) {
        // Big file writing support (> 2GB) is on by default. This allows to change it:
        if (bigFiles >= 0) {
            g_object_set(muxer_elt, "large-file", (bigFiles > 0) ? 1 : 0, NULL);
            sprintf(codecoption, " large-file=%i", (bigFiles > 0) ? 1 : 0);
            strcat(muxer, codecoption);
        }

        if (fastStart >= 0) {
            // Enable/Disable fast start support for low-latency start of movie playback:
            g_object_set(muxer_elt, "faststart", (fastStart > 0) ? 1 : 0, NULL);
            sprintf(codecoption, " faststart=%i", (fastStart > 0) ? 1 : 0);
            strcat(muxer, codecoption);
        }
        else {
            // Enable fast start support for low-latency start of movie playback:
            // FIXME: Unless we're on Windows where this would cause a crash in qtmux!
            // TODO: Investigate & Fix root cause. For now we just disable faststart
            // on Windows.
            fastStart = (PSYCH_SYSTEM == PSYCH_WINDOWS) ? 0 : 1;
            g_object_set(muxer_elt, "faststart", fastStart, NULL);
            sprintf(codecoption, " faststart=%i", fastStart);
            strcat(muxer, codecoption);
        }

        if (indexItemsSec >= 0) {
            // Write seek index entry for each 1/indexItemsSec of movie time:
            g_object_set(muxer_elt, "movie-timescale", indexItemsSec, NULL);
            sprintf(codecoption, " movie-timescale=%i", indexItemsSec);
            strcat(muxer, codecoption);
        } else {
            // Write seek index entry for each millisecond of movie time:
            g_object_set(muxer_elt, "movie-timescale", 1000, NULL);
            sprintf(codecoption, " movie-timescale=%i", 1000);
            strcat(muxer, codecoption);
        }
    }

    if (strstr(muxer, "matroskamux") || strstr(muxer, "webmmux")) {
        if (indexItemsSec >= 0) {
            // Write seek index entry for each 1/indexItemsSec of movie time:
            g_object_set(muxer_elt, "min-index-interval", (int)  (1e9 / indexItemsSec), NULL);
            sprintf(codecoption, " min-index-interval=%i", (int) (1e9 / indexItemsSec));
            strcat(muxer, codecoption);
        } else {
            // Write seek index entry for each millisecond (1e6 nanoseconds) of movie time:
            g_object_set(muxer_elt, "min-index-interval", (int)  1e6, NULL);
            sprintf(codecoption, " min-index-interval=%i", (int) 1e6);
            strcat(muxer, codecoption);
        }
    }

    // Still no video codec? Then this is game over: FIXME Small memory leak here on error exit.
    if (capdev->videoenc == NULL) PsychErrorExitMsg(PsychError_user, "Could not find or setup requested video codec or any fallback codec for video recording. Aborted.");
    if (use_audio && (!audio_enc && !use_profiles)) PsychErrorExitMsg(PsychError_user, "Could not find or setup requested audio codec for recording. Aborted.");
    if (!muxer_elt && !use_profiles) PsychErrorExitMsg(PsychError_user, "Could not find or setup requested audio-video multiplexer for recording. Aborted.");

    // Invalidate our no longer needed fake videoenc entry if profiles are in use:
    if (use_profiles && (capdev->videoenc == (GstElement*) 0x1)) capdev->videoenc = NULL;

    if (PsychPrefStateGet_Verbosity() > 3) {
        if (use_audio) printf("PTB-INFO: Audiosource: %s\n", audiosrc);
        if (use_audio) printf("PTB-INFO: Audiocodec : %s\n", audiocodec);
        if (use_audio && use_profiles) printf("PTB-INFO: Audio profile: %s\n", audioProfile);
        printf("PTB-INFO: Videocodec : %s\n", videocodec);
        if (use_profiles) printf("PTB-INFO: Video profile: %s\n", outCodecName);
        printf("PTB-INFO: Multiplexer: %s\n", muxer);
        if (use_profiles) printf("PTB-INFO: Container profile: %s\n", muxerProfile);
    }

    // Setup for video recording via generated codecs or encoding profiles for use with camerabin?
    if (!launchline || forCamerabin) {
        // Camerabin2 in use:
        // Release our objects, if any. But first store their parameter settings
        // as presets for use by our video/audio/mux profiles.
        if (muxer_elt) {
            PsychSaveEncodingPresetsForElement(muxer_elt, muxer_preset_name);
            gst_object_unref(G_OBJECT(muxer_elt));
            muxer_elt = NULL;
        }

        if (audio_enc) {
            PsychSaveEncodingPresetsForElement(audio_enc, audio_preset_name);
            gst_object_unref(G_OBJECT(audio_enc));
            audio_enc = NULL;
        }

        if (capdev->videoenc) {
            // Need to black-list certain encoders, for which preset saving is buggy and would crash:
            // Update Aug-2014: No known buggy codecs as of GStreamer-1.4.0 so no black-listing atm.
            if (TRUE) {
                // Not black-listed - Save presets:
                PsychSaveEncodingPresetsForElement(capdev->videoenc, video_preset_name);
            }
            else {
                // Black listed:
                if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Saving of encoder presets for black-listed video encoder '%s' skipped.\n", videocodec);
            }
            gst_object_unref(G_OBJECT(capdev->videoenc));
            capdev->videoenc = NULL;
        }

        // Assign special audio source, if any:
        if (audio_src) g_object_set(camera, "audio-source", audio_src, NULL);

        // Assign video (and audio) encoding and muxing profile:
        g_object_set(camera, "video-profile", PsychCreateEncodingProfile(muxerProfile, outCodecName, (soundForVideoRecording) ? audioProfile : NULL), NULL);
    } else {
        // Setup for launch-line based movie writing : Release our objects, we have our launch line:
        if (capdev->videoenc) { gst_object_unref(G_OBJECT(capdev->videoenc)); capdev->videoenc = NULL; }
        if (audio_enc) gst_object_unref(G_OBJECT(audio_enc));
        if (audio_src) gst_object_unref(G_OBJECT(audio_src));
        if (muxer_elt) gst_object_unref(G_OBJECT(muxer_elt));

        // Build gst-launch style GStreamer pipeline spec string:

        // Special keyword for audio tracks in written movies?
        if ((poption=strstr(codecSpec, "AddAudioTrack"))) {
            // Audio and Video:
            poption+=strlen("AddAudioTrack");
            if (sscanf(poption, "=%i@%i", &nrAudioChannels, &audioFreq) != 2) {
                // Assign default 1 mono-channel count and 48 kHz frequency:
                nrAudioChannels = 1;
                audioFreq = 48000;
            }

            if (nrAudioChannels < 1 || nrAudioChannels > 256 || audioFreq < 1 || audioFreq > 200000) {
                printf("PTB-ERROR: Invalid parameters in 'AddAudioTrack='! Either %i audio channels outside valid range 1-256, or %i audioFreq outside allowable range 1 - 200000 Hz.\n",
                    nrAudioChannels, audioFreq);
                PsychErrorExitMsg(PsychError_user, "Invalid audio recording parameters provided");
            }

            // If no "AudioSource=" was provided then configure for appsrc feeding
            // from Screen('AddAudioBufferToMovie') into movie:
            if (strlen(audiosrc) == 0) sprintf(audiosrc, "appsrc name=ptbaudioappsrc format=3 do-timestamp=0 stream-type=0 max-bytes=0 block=1 is-live=0 emit-signals=0 ! capsfilter caps=\"audio/x-raw, format=F32LE, channels=(int)%i, rate=(int)%i\" ! audioresample ! audioconvert ! queue", nrAudioChannels, audioFreq);

            // We add bits to feed from 'audiosrc' into 'audiocodec' into the common muxer of video and audio stream:
            sprintf(outCodecName, " %s ! ptbvideomuxer0. %s ! %s ! ptbvideomuxer0. %s name=ptbvideomuxer0 ", videocodec, audiosrc, audiocodec, muxer);
        } else {
            // Video only:

            // We feed the output of 'videocodec' directly into the muxer:
            sprintf(outCodecName, " %s ! %s ", videocodec, muxer);
        }
    }

    return(TRUE);
}

/* PsychGetCodecLaunchLineFromString() - Helper function for GStreamer based movie writing.
 *
 * Take a 'codecSpec' string with user provided codec settings string, get
 * a proper gst-launch style segment for the codecs and muxers back.
 * launchString must be preallocated and of sufficient size (approx. 1Kb).
 * Returns TRUE on success, FALSE on failure.
 */
psych_bool PsychGetCodecLaunchLineFromString(char* codecSpec, char* launchString)
{
    // Need a fake capdev to pass to PsychSetupRecordingPipeFromString():
    PsychVidcapRecordType dummydev;
    memset(&dummydev, 0, sizeof(dummydev));

    // Pass off the job and just return its results. This is shared with encoder/muxer
    // creation, setup, configuration and testing for the video recording engine:
    return(PsychSetupRecordingPipeFromString(&dummydev, codecSpec, launchString, TRUE, FALSE, FALSE));
}

/* PsychHaveVideoDataCallback: This is used if an external C plugin, e.g., video LoadMarkerTrackingPlugin
 * is loaded to execute that plugin on the most recently arrived video input buffer. The callback is attached
 * to the sink-pad of our videosink appsink, so the callback gets executed for each incoming buffer on the
 * relevant streaming thread. This makes sure it automatically works even if we are not actively fetching
 * video data, e.g, our appsink just drops all frames, as we piggyback our custom processing on one of the
 * existing GStreamer threads - no need for our own thread and plumbing as for the dc1394 engine.
 */
static GstPadProbeReturn PsychHaveVideoDataCallback(GstPad *pad, GstPadProbeInfo *info, gpointer dataptr)
{
    unsigned char *input_image;
    PsychVidcapRecordType *capdev = (PsychVidcapRecordType *) dataptr;
    GstBuffer *videoBuffer = info->data;
    #pragma warning( disable : 4068 )
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    GstMapInfo mapinfo = GST_MAP_INFO_INIT;
    #pragma GCC diagnostic pop

    // Is a special markertracker plugin loaded for this camera? If so, execute it on this frame:
    if (capdev->markerTrackerPlugin) {
        // Map the buffers memory for read+write. Tracking only needs read-access, but if visualization
        // of tracking is enabled, the plugin will also write color info overlay data into the buffer:
        if (!gst_buffer_map(videoBuffer, &mapinfo, GST_MAP_READ | GST_MAP_WRITE)) {
            printf("PTB-ERROR: Failed to map video data of captured video frame! Something's wrong. Skipping tracking for this frame.\n");
            return(GST_PAD_PROBE_OK);
        }

        input_image = (unsigned char*) (GLuint*) mapinfo.data;

        // Yes! Execute: Plugin reads from (unsigned long*) input_image:
        if (!(*TrackerPlugin_processFrame) (capdev->markerTrackerPlugin, (unsigned long*) input_image, capdev->width, capdev->height,
                                            (int) capdev->roirect[kPsychLeft], (int) capdev->roirect[kPsychTop],
                                            (unsigned int) GST_BUFFER_OFFSET(videoBuffer),
                                            (double) GST_BUFFER_PTS(videoBuffer) / 1e9, (unsigned int) GST_BUFFER_OFFSET(videoBuffer))) {
            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: Failed to process video frame with framecount %i by markertracker plugin for capture device %i.\n",
                       GST_BUFFER_OFFSET(videoBuffer), capdev->capturehandle);
            }
        }

        gst_buffer_unmap(videoBuffer, &mapinfo);
    }

    return(GST_PAD_PROBE_OK);
}

/*
 *      PsychGSOpenVideoCaptureDevice() -- Create a video capture object.
 *
 *      This function tries to open and initialize a connection to a camera
 *      and returns the associated captureHandle for it.
 *
 *      slotid = Number of slot in vidcapRecordBANK[] array to use for this camera.
 *      win = Pointer to window record of associated onscreen window.
 *      deviceIndex = Index of the grabber device.
 *      capturehandle = handle to the new capture object.
 *      capturerectangle = If non-NULL a ptr to a PsychRectangle which contains the ROI for capture. Special roi [0 0 w h] selects resolution w x h on device.
 *      reqdepth = Number of layers for captured output textures. (0=Don't care, 1=LUMINANCE8, 2=LUMINANCE8_ALPHA8, 3=RGB8, 4=RGBA8, 5=YCBCR)
 *      num_dmabuffers = Number of buffers to queue internally before dropping buffers. Zero = Don't drop buffers, fill up whole memory if neccessary.
 *      allow_lowperf_fallback = Ignored - No such fallback exists anymore.
 *      targetmoviefilename = Filename of movie file to record (if any) if deviceIndex >=0,
 *                            special spec string for a video capture device if deviceIndex < 0.
 *      recordingflags = Special flags to control internal operation (harddisc recording yes/no, live video during recording?, audio recording? workarounds?)
 *
 *      bitdepth = Number of bits per color component / channel, aka bpc. We support 8 bpc and 16 bpc capture on GStreamer at the moment.
 *
 */
psych_bool PsychGSOpenVideoCaptureDevice(int slotid, PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle,
                                        int reqdepth, int num_dmabuffers, int allow_lowperf_fallback, char* targetmoviefilename, unsigned int recordingflags, int bitdepth)
{
    GError          *error;
    GstCaps         *colorcaps;
    GstCaps         *vfcaps, *reccaps;
    GstElement      *camera = NULL;
    GMainLoop       *VideoContext = NULL;
    GstBus          *bus = NULL;
    GstElement      *videosink = NULL;
    GstElement      *videosource = NULL;
    GstElement      *videosource_filter = NULL;
    GstElement      *videocrop_filter = NULL;
    GstElement      *videowrappersrc = NULL;
    GstPad          *pad, *peerpad;
    GstPad          *dvpad;
    GstCaps         *caps;
    GstStructure    *str;
    gint            width, height;
    gint            rate1, rate2;
    gint            twidth, theight;
    char            *codecSpec;
    char            codecName[10000];
    PsychVidcapRecordType   *capdev = NULL;
    char            config[1000];
    char            tmpstr[1000];
    char            device_name[1000];
    char            plugin_name[1000];
    char            prop_name[1000];
    gchar           *pstring = NULL;
    PsychVideosourceRecordType  *theDevice = NULL;
    psych_bool      overrideFrameSize = FALSE;

    config[0] = 0;
    tmpstr[0] = 0;
    device_name[0] = 0;
    plugin_name[0] = 0;
    prop_name[0] = 0;

    // Init capturehandle to none:
    *capturehandle = -1;

    // Make sure GStreamer is ready:
    PsychGSCheckInit("videocapture");

    // As a side effect of some PsychGSCheckInit() some broken GStreamer runtimes can change
    // the OpenGL context binding behind our back to some GStreamer internal context.
    // Make sure our own context is bound after return from PsychGSCheckInit() to protect
    // against the state bleeding this would cause:
    if (win) PsychSetGLContext(win);

    // Map deviceIndex of requested video source to device name:
    if (deviceIndex >= 0) {
        // Get device name for given deviceIndex from video device
        // enumeration (or NULL if no such device):
        theDevice = PsychGSEnumerateVideoSources(-1, deviceIndex, &videosource);
        if (NULL == theDevice) {
            printf("PTB-ERROR: There isn't any video capture device available for provided deviceIndex %i.\n", deviceIndex);
            PsychErrorExitMsg(PsychError_user, "Invalid deviceIndex provided. No such video source. Aborted.");
        }

        // Assign name:
        sprintf(device_name, "%s", theDevice->deviceHandle);
        sprintf(plugin_name, "%s", theDevice->deviceVideoPlugin);
        sprintf(prop_name, "%s", theDevice->deviceSelectorProperty);
        if (theDevice->deviceURI > 0) {
            sprintf(device_name, "%llu", theDevice->deviceURI);
        }

        if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Trying to open video capture device with deviceIndex %i [%s].\n", deviceIndex, device_name);
    }

    // Slot 'slotid' will contain the record for our new capture object:

    // Zero-out new record in video bank:
    memset(&vidcapRecordBANK[slotid], 0, sizeof(PsychVidcapRecordType));

    // Initialize new record:
    vidcapRecordBANK[slotid].valid = 1;

    // Retrieve device record for slotid:
    capdev = PsychGetGSVidcapRecord(slotid);

    capdev->camera = NULL;
    capdev->grabber_active = 0;
    capdev->scratchbuffer = NULL;

    // Make sure bitdepth is either 8 bpc or 16 bpc, nothing else:
    bitdepth = (bitdepth > 8) ? 16 : 8;

    // Selection of pixel depths:
    if (reqdepth == 4 || reqdepth == 0) {
        // Default is RGB, aka RGBA8 or RGBA16:
        reqdepth = 4;
    }
    else {
        switch (reqdepth) {
            case 2:
                // A no-go: Instead we use 1 channel luminance:
                if (PsychPrefStateGet_Verbosity()>1)
                    printf("PTB-WARNING: Video capture engine doesn't support requested Luminance+Alpha format. Will revert to pure luminance instead...\n");
                reqdepth = 1;
            break;

            case 1: // Accept as is: Luminance.
            case 3: // Accept as is: RGB.
            break;

            case 5: // Accept as YVYU. This only at bitdepth 8, though.
                if (!(win->gfxcaps & kPsychGfxCapUYVYTexture) || (bitdepth > 8)) {
                    // Usercode requested type 5 - UYVY textures, but GPU does not support them.
                    // Fallback to type 4 - RGBA8 textures:
                    if (PsychPrefStateGet_Verbosity() > 1) {
                        printf("PTB-WARNING: Requested YUYV-422 texture format for video capture unsupported by GPU%s. Falling back to RGBA8 format.\n", (bitdepth > 8) ? " for requested > 8 bpc bitdepth" : "");
                    }
                    bitdepth = 8;
                    reqdepth = 4;
                }
            break;

            case 6: // Accept as YUV-I420. This only at bitdepth 8, though.
                if ((bitdepth > 8) || !(win->gfxcaps & kPsychGfxCapFBO) || !PsychAssignPlanarI420TextureShader(NULL, win)) {
                    // Usercode requested type 6 - I420 textures, but GPU does not support them.
                    // Fallback to type 4 - RGBA8 textures:
                    if (PsychPrefStateGet_Verbosity() > 1) {
                        printf("PTB-WARNING: Requested YUV-I420 texture format for video capture unsupported by GPU%s. Falling back to RGBA8 format.\n", (bitdepth > 8) ? " for requested > 8 bpc bitdepth" : "");
                    }
                    bitdepth = 8;
                    reqdepth = 4;
                }
            break;

            default:
            // Unknown format:
            PsychErrorExitMsg(PsychError_user, "You requested an invalid image depths (not one of 0, 1, 2, 3, 4, 5 or 6). Aborted.");
        }
    }

    // Requested output texture pixel depth in layers:
    capdev->reqpixeldepth = reqdepth;
    capdev->pixeldepth = reqdepth * ((bitdepth > 8) ? 16 : 8);
    capdev->bitdepth = bitdepth;
    if ((capdev->bitdepth > 8) && (PsychPrefStateGet_Verbosity() > 2)) printf("PTB-INFO: Requesting %i bpc data from camera %i.\n", bitdepth, deviceIndex);

    // Assign number of dma buffers to use:
    capdev->num_dmabuffers = num_dmabuffers;

    PsychInitMutex(&vidcapRecordBANK[slotid].mutex);
    PsychInitCondition(&vidcapRecordBANK[slotid].condition, NULL);

    // Try to open and initialize camera according to given settings:
    camera = gst_element_factory_make ("camerabin", "ptbvideocapturepipeline");

    // Pipeline creation failed?
    if (NULL == camera) PsychErrorExitMsg(PsychError_user, "Failed to create video capture pipeline! Aborted.");

    // Set basic opmode to 2 for video capture/recording, instead of default 1
    // for still image capture:
    g_object_set(G_OBJECT(camera), "mode", 2, NULL);

    sprintf(config, "%s", device_name);

    // Assign new record in videobank:
    vidcapRecordBANK[slotid].camera = camera;
    vidcapRecordBANK[slotid].frameAvail = 0;

    // Build an appropriate video source and set up its input:

    // Setup for enumerated device aka deviceIndex >= 0 ?
    if (deviceIndex >= 0) {
        // Create proper videosource plugin if possible:
        if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach '%s' as video source...\n", plugin_name);

        if (strstr(plugin_name, "dv1394src")) {
            // dv1394src and hdv1394src need special treatment:

            // videosource plugin already created by GstDeviceMonitor/GstDeviceProvider code (unlikely as of GStreamer 1.4.0)?
            if (videosource) {
                // Yep. We can't use it as is and would need to incorporate it into a self-made bin and
                // change its parameters, so for now take the lame route: Destroy it and then try to
                // regen it with our special purpose code here from GStreamer-0.10 days, using the
                // hopefully provided GUID, otherwise trouble!
                gst_object_unref(GST_ELEMENT(videosource));
                videosource = NULL;
                if (PsychPrefStateGet_Verbosity() > 1) {
                    printf("PTB-WARNING: Got a premade videosource for plugin '%s'. Not useable for us. Destroying and regenerating.\n", plugin_name);
                    printf("PTB-WARNING: Please report this incident to the Psychtoolbox forum - More work is needed here for robustness.\n");
                }
            }

            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device with guid '%llu' as video input [Property %s].\n", theDevice->deviceURI, prop_name);
            sprintf(tmpstr, "%s typefind=true do-timestamp=true guid=%llu ! dvdemux ! dvdec name=ptbdvsource", plugin_name, theDevice->deviceURI);
            videosource = gst_parse_bin_from_description_full((const gchar *) tmpstr, FALSE, NULL, GST_PARSE_FLAG_FATAL_ERRORS, NULL);

            if (videosource) {
                // Attach proper dvpad for this special snowflake:
                dvpad = gst_element_get_static_pad(gst_bin_get_by_name(GST_BIN(videosource), "ptbdvsource"), "src");
                gst_element_add_pad(videosource, gst_ghost_pad_new("src", dvpad));
                gst_object_unref(GST_OBJECT(dvpad));
                dvpad = NULL;
            }
        }
        else {
            // Standard path: Already got a ready-made plugin from enumeration routine?
            // If so, skip creation and setup / device attachment from device properties:
            if (videosource == NULL) {
                // Nope: Classic setup path. Create a virgin one from scratch:
                videosource = gst_element_factory_make(plugin_name, "ptb_videosource");

                if (videosource) {
                    // Attach correct video input device to it:avfvideosrc
                    if ((!strcmp(plugin_name, "dc1394src") || !strcmp(plugin_name, "qtkitvideosrc") || !strcmp(plugin_name, "avfvideosrc")) && (prop_name[0] != 0)) {
                        // DC1394 source or QTKITVideosource or AVFoundation based videosource:
                        if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device with guid '%llu' as video input [Property %s].\n", theDevice->deviceURI, prop_name);
                        g_object_set(G_OBJECT(videosource), prop_name, (int) theDevice->deviceURI, NULL);
                    } else {
                        if ((config[0] != 0) && (prop_name[0] != 0) && (theDevice->deviceURI == 0)) {
                            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device '%s' as video input [Property %s].\n", config, prop_name);
                            g_object_set(G_OBJECT(videosource), prop_name, config, NULL);
                        }

                        if ((theDevice->deviceURI > 0) && (prop_name[0] != 0)) {
                            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device with guid '%llu' as video input [Property %s].\n", theDevice->deviceURI, prop_name);
                            g_object_set(G_OBJECT(videosource), prop_name, theDevice->deviceURI, NULL);
                        }
                    }
                }
            }
            else if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Using ready-made video source for deviceIndex %i, as provided by enumeration routines.\n", deviceIndex);
        }

        // Success after all?
        if (!videosource) {
            if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed videocapture plugin creation for deviceIndex %i. We are out of options and will fail.\n", deviceIndex);
            PsychErrorExitMsg(PsychError_system, "GStreamer failed to find a suitable video source! Game over.");
        }

        // End of setup path for deviceIndex >= 0 with enumerated and selected videosources.
    }

    // MS-Windows specific manual override setup path:
    if ((PSYCH_SYSTEM == PSYCH_WINDOWS) && (deviceIndex > -8) && (deviceIndex < 0)) {
        // Non-Firewire video source selected:
        if ((deviceIndex == -1) || (deviceIndex == -2)) {
            // First try Kernel-Streaming based video source for low-latency capture:
            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach ksvideosrc as video source...\n");
            videosource = gst_element_factory_make("ksvideosrc", "ptb_videosource");
        }

        if (videosource) {
            // Kernel streaming video source:

            // Fetch mandatory targetmoviename parameter as name spec string:
            if (targetmoviefilename == NULL) PsychErrorExitMsg(PsychError_user, "You set 'deviceIndex' to a negative value, but didn't provide the required device name string in the 'moviename' argument! Aborted.");

            // Assign:
            strcat(config, targetmoviefilename);

            switch(deviceIndex) {
                case -1:
                    // Human friendly device name provided:
                    if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device '%s' as video input.\n", config);
                    g_object_set(G_OBJECT(videosource), "device-name", config, NULL);
                    break;

                case -2:
                    // DirectShow device path provided:
                    if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device at path '%s' as video input.\n", config);
                    g_object_set(G_OBJECT(videosource), "device-path", config, NULL);
                    break;

                default:
                    PsychErrorExitMsg(PsychError_user, "Invalid 'deviceIndex' provided! Not a value of -1 or -2. Aborted.");
            }
        }

        // No kernel streaming video source available?
        if (!videosource) {
            // No. Try a Directshow video source instead:
            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach dshowvideosrc as video source...\n");
            videosource = gst_element_factory_make("dshowvideosrc", "ptb_videosource");

            if (videosource) {
                if (deviceIndex != -5) {
                    // Fetch optional targetmoviename parameter as name spec string:
                    if (targetmoviefilename == NULL) PsychErrorExitMsg(PsychError_user, "You set 'deviceIndex' to a negative value, but didn't provide the required device name string in the 'moviename' argument! Aborted.");

                    // Assign:
                    strcat(config, targetmoviefilename);
                }

                switch(deviceIndex) {
                    case -1:
                    case -3:
                        // Human friendly device name provided:
                        if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device '%s' as video input.\n", config);
                        g_object_set(G_OBJECT(videosource), "device-name", config, NULL);
                        break;

                    case -2:
                    case -4:
                        // DirectShow device path provided:
                        if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device at path '%s' as video input.\n", config);
                        g_object_set(G_OBJECT(videosource), "device", config, NULL);
                        break;
                    case -5:
                        if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach default DirectShow video device as video input.\n");
                        break;
                    default:
                        PsychErrorExitMsg(PsychError_user, "Invalid 'deviceIndex' provided! Aborted.");
                }
            }
        }
        // Have a standard windows video source ready.

        // Still no video source available?
        if (!videosource) {
            if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed! We are out of options and will fail now.\n");
            PsychErrorExitMsg(PsychError_system, "GStreamer failed to find a suitable video source! Game over.");
        }
    } // End of MS-Windows Video source creation.

    // MacOS/X specific setup path:
    if ((PSYCH_SYSTEM == PSYCH_OSX) && (deviceIndex > -8) && (deviceIndex < 0)) {
        if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video source via MacOSX specific special case setup code.\n");
        if (deviceIndex == -1) videosource = gst_element_factory_make("autovideosrc", "ptb_videosource");
        if (deviceIndex == -2) videosource = gst_element_factory_make("qtkitvideosrc", "ptb_videosource");
        if (deviceIndex == -3) videosource = gst_element_factory_make("avfvideosrc", "ptb_videosource");
        if (deviceIndex == -4) videosource = gst_element_factory_make("videotestsrc", "ptb_videosource");

        if (!videosource) {
            if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed! We are out of options and will fail now.\n");
            PsychErrorExitMsg(PsychError_system, "GStreamer failed to find a suitable video source! Game over.");
        }
    } // End of OS/X Video source creation.

    // OS independent special sources with no support for enumeration:

    // aravissrc for GeniCam cams: Doesn't support enumeration, just opening of cams by name:
    if (deviceIndex == -8) {
        if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach aravissrc as GeniCam video source...\n");

        sprintf(plugin_name, "aravissrc");
        videosource = gst_element_factory_make("aravissrc", "ptb_videosource");

        if (!videosource) {
            if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed to create aravissrc GeniCam video source! We are out of options and will fail now.\n");
            PsychErrorExitMsg(PsychError_system, "GStreamer failed to find a suitable video source! Game over.");
        }

        // Fetch optional targetmoviename parameter as name spec string:
        if (targetmoviefilename) {
            // Assign:
            strcat(config, targetmoviefilename);

            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach GeniCam video device '%s' as video input.\n", config);
            g_object_set(G_OBJECT(videosource), "camera-name", config, NULL);
        }
        else {
            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach default Aravis GeniCam video device as video input.\n");
        }
    }

    // Videosource provided as a bin which is constructed from a gst-launch line:
    if (deviceIndex == -9) {
        // Fetch mandatory gstlaunchbinsrc parameter as bin spec string:
        if (gstlaunchbinsrc[0] == 0) {
            PsychErrorExitMsg(PsychError_user, "You set 'deviceIndex' to -9 to request creation of a user-defined video source, but didn't set the required gst-launch style string beforehand! Aborted.");
        }

        // Feedback to user:
        if (PsychPrefStateGet_Verbosity() > 2) {
            printf("PTB-INFO: Trying to attach generic user provided bin as video source. The gst-launch style pipeline description is:\n");
            printf("PTB-INFO: %s\n", gstlaunchbinsrc);
        }

        // Create a bin from the provided gst-launch style string and assign it as videosource plugin:
        sprintf(plugin_name, "gstlaunchbinsrc");
        error = NULL;
        videosource = gst_parse_bin_from_description_full((const gchar *) gstlaunchbinsrc, FALSE, NULL, GST_PARSE_FLAG_FATAL_ERRORS, &error);

        if (!videosource) {
            printf("PTB-ERROR: Failed to create generic bin video source!\n");
            if (error) {
                printf("PTB-ERROR: GStreamer returned the following error: %s\n", (char*) error->message);
                g_error_free(error);
            }
            PsychErrorExitMsg(PsychError_user, "GStreamer failed to build a suitable video source from your pipeline spec! Game over.");
        }

        // DV video sources need special treatment due to their use of special dvdec decoders and dvdemux'ers:
        if (strstr(gstlaunchbinsrc, "ptbdvsource")) {
            dvpad = gst_element_get_static_pad(gst_bin_get_by_name(GST_BIN(videosource), "ptbdvsource"), "src");
            gst_element_add_pad(videosource, gst_ghost_pad_new("src", dvpad));
            gst_object_unref(GST_OBJECT(dvpad));
            dvpad = NULL;
        }
    }

    // Some plugins need typefind'ing dhowvideosrc for sure, but we also set it for aravissrc to be safe:
    if (strstr(plugin_name, "dshowvideosrc") || strstr(plugin_name, "aravissrc")) g_object_set(G_OBJECT(videosource), "typefind", 1, NULL);

    // Enable timestamping by videosource, unless its been done already for a dv1394src:
    if (!strstr(plugin_name, "dv1394src") && !strstr(plugin_name, "gstlaunchbinsrc")) g_object_set(G_OBJECT(videosource), "do-timestamp", 1, NULL);

    // videotestsrc needs special setup - Must be marked as live-source:
    if (strstr(plugin_name, "videotestsrc")) {
        g_object_set(G_OBJECT(videosource), "is-live", 1, NULL);

        // Also assign different test patterns, depending on deviceIndex (90001 - 90023):
        g_object_set(G_OBJECT(videosource), "pattern", deviceIndex - 90001, NULL);
        g_object_set(G_OBJECT(videosource), "kyt", (deviceIndex > 90001) ? 1 : 0, NULL);
    }

    // Assign video source to pipeline: Attach indirectly to camerabin2 via a camerawrappersrc.
    videowrappersrc = gst_element_factory_make ("wrappercamerabinsrc", "ptbwrappervideosrc0");
    g_object_set(videowrappersrc, "video-source", videosource, NULL);
    g_object_set(camera, "camera-source", videowrappersrc, NULL);

    // Name of target movie file for video and audio recording specified?
    if (((deviceIndex >= 0) || (deviceIndex <= -9)) && targetmoviefilename) {
        // Video recording: Assign it to camerabin to perform video recording:

        // Codec settings or type specified?
        if ((codecSpec = strstr(targetmoviefilename, ":CodecSettings="))) {
            // Replace ':' with a zero in targetmoviefilename, so it gets null-terminated
            // and only points to the actual movie filename:
            *codecSpec = 0;

            // Move after null-terminator:
            codecSpec++;

            // Replace the ':CodecSettings=' with the special keyword 'DEFAULTenc', so
            // so the default video codec is chosen, but the given settings override its
            // default parameters.
            strncpy(codecSpec, "DEFAULTenc    ", strlen("DEFAULTenc    "));

            if (strlen(codecSpec) == 0) PsychErrorExitMsg(PsychError_user, "Invalid (empty) :CodecSettings= parameter specified. Aborted.");
        } else if ((codecSpec = strstr(targetmoviefilename, ":CodecType="))) {
            // Replace ':' with a zero in targetmoviefilename, so it gets null-terminated
            // and only points to the actual movie filename:
            *codecSpec = 0;
            // Advance codecSpec to point to the actual codec spec string:
            codecSpec+= 11;

            if (strlen(codecSpec) == 0) PsychErrorExitMsg(PsychError_user, "Invalid (empty) :CodecType= parameter specified. Aborted.");
        } else {
            // No codec specified: Use our default encoder, the one that's been shown to
            // produce good results:
            codecSpec = strdup("DEFAULTenc");
        }

        // Create matching video encoder for codecSpec:
        if (PsychSetupRecordingPipeFromString(capdev, codecSpec, codecName, FALSE, TRUE, (recordingflags & 2) ? TRUE : FALSE)) {
            if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: Video%s recording into file [%s] enabled for device %i. Codec is [%s].\n",
                                    ((recordingflags & 2) ? " and audio" : ""), targetmoviefilename, deviceIndex, codecName);
            if (strcmp(codecSpec, "DEFAULTenc") == 0) free(codecSpec);
            codecSpec = NULL;
        } else {
            if (strcmp(codecSpec, "DEFAULTenc") == 0) free(codecSpec);
            PsychErrorExitMsg(PsychError_system, "Setup of video recording failed. Reason hopefully given above.");
        }

        capdev->targetmoviefilename = strdup(targetmoviefilename);
        capdev->recording_active = TRUE;
    } else {
        // No video recording, only live capture:
        capdev->recording_active = FALSE;
    }

    // Create a appsink named "ptbsink0" for attachment as the destination video-sink for
    // all video content. This allows us to get hold of the videoframe buffers for
    // converting them into PTB OpenGL textures:
    videosink = gst_element_factory_make ("appsink", "ptbsink0");
    if (!videosink) {
        printf("PTB-ERROR: Failed to create video-sink appsink ptbsink!\n");
        PsychGSProcessVideoContext(&(vidcapRecordBANK[slotid]), TRUE);
        PsychErrorExitMsg(PsychError_system, "Opening the videocapture device failed. Reason hopefully given above.");
    }

    vidcapRecordBANK[slotid].videosink = videosink;

    // Our OpenGL texture creation routine needs GL_BGRA8 data in GL_UNSIGNED_8_8_8_8_REV
    // format, but the pipeline usually delivers YUV data in planar format. Therefore
    // need to perform colorspace/colorformat conversion. These colorcaps, as assigned to
    // the videosink will try to nudge the video source to deliver data in our requested
    // format. If this ain't possible, they will enforce creation of a colorspace converter
    // inbetween the video source and our videosink:
    switch (reqdepth) {
        case 4:
            if (bitdepth <= 8) {
                // Classic proven 8 bpc path:
                colorcaps = gst_caps_new_simple("video/x-raw",
                                                "format", G_TYPE_STRING, "BGRA",
                                                "bpp", G_TYPE_INT, capdev->pixeldepth,
                                                "depth", G_TYPE_INT, capdev->pixeldepth,
                                                "alpha_mask", G_TYPE_INT, 0x000000FF,
                                                "red_mask", G_TYPE_INT,   0x0000FF00,
                                                "green_mask", G_TYPE_INT, 0x00FF0000,
                                                "blue_mask", G_TYPE_INT,  0xFF000000,
                                                NULL);
            }
            else {
                // 16 bpc high precision path: Component ordering is ARGB, not BGRA, as
                // in the special BGRA8 case for 4 layer 8 bpc:  Doesn't work at least on OSX qtkitvideosrc
                colorcaps = gst_caps_new_simple("video/x-raw",
                                                "format", G_TYPE_STRING, "ARGB64",
                                                "bpp", G_TYPE_INT, capdev->pixeldepth,
                                                "depth", G_TYPE_INT, capdev->pixeldepth,
                                                "blue_mask", G_TYPE_INT64,  0x000000000000FFFF,
                                                "green_mask", G_TYPE_INT64, 0x00000000FFFF0000,
                                                "red_mask", G_TYPE_INT64,   0x0000FFFF00000000,
                                                "alpha_mask", G_TYPE_INT64, 0xFFFF000000000000,
                                                NULL);
            }
        break;

        case 3:
            if (bitdepth <= 8) {
                // Classic proven 8 bpc path:
                colorcaps = gst_caps_new_simple("video/x-raw",
                                                "format", G_TYPE_STRING, "RGB",
                                                "bpp", G_TYPE_INT, capdev->pixeldepth,
                                                "depth", G_TYPE_INT, capdev->pixeldepth,
                                                "red_mask", G_TYPE_INT,   0x00FF0000,
                                                "green_mask", G_TYPE_INT, 0x0000FF00,
                                                "blue_mask", G_TYPE_INT,  0x000000FF,
                                                NULL);
            }
            else {
                // 16 bpc high precision path: Doesn't work at least on OSX qtkitvideosrc
                colorcaps = gst_caps_new_simple("video/x-raw",
                                                "format", G_TYPE_STRING, "RGB",
                                                "bpp", G_TYPE_INT, capdev->pixeldepth,
                                                "depth", G_TYPE_INT, capdev->pixeldepth,
                                                "red_mask", G_TYPE_INT64,   0x0000FFFF00000000,
                                                "green_mask", G_TYPE_INT64, 0x00000000FFFF0000,
                                                "blue_mask", G_TYPE_INT64,  0x000000000000FFFF,
                                                NULL);
            }
        break;

        case 2:
        case 1:
            // This works for both 8 bpc and 16 bpc:
            colorcaps = gst_caps_new_simple("video/x-raw",
                                            "format", G_TYPE_STRING, (bitdepth > 8) ? "GRAY16_LE" : "GRAY8",
                                            "bpp", G_TYPE_INT, capdev->pixeldepth,
                                            "depth", G_TYPE_INT, capdev->pixeldepth,
                                            "endianess", G_TYPE_INT, 1234,
                                            "endianness", G_TYPE_INT, 1234,
                                            NULL);
        break;

        case 5: // YUYV-422 packed pixel encoding:
            colorcaps = gst_caps_new_simple("video/x-raw",
                                            "format", G_TYPE_STRING, "UYVY",
                                            NULL);
            reqdepth = 2;
            capdev->reqpixeldepth = 2;
            capdev->pixeldepth = 16;
        break;

        case 6: // YUV-I420 planar pixel encoding:
            colorcaps = gst_caps_new_simple("video/x-raw",
                                            "format", G_TYPE_STRING, "I420",
                                            NULL);
            reqdepth = 2;
            capdev->reqpixeldepth = 2;
            capdev->pixeldepth = 12;
        break;

        default:
            colorcaps = NULL;
            PsychErrorExitMsg(PsychError_internal, "Unknown reqdepth parameter received!");
    }

    // Assign 'colorcaps' as caps to our videosink. This marks the videosink so
    // that it can only receive video image data in the format defined by colorcaps,
    // i.e., a format that is easy to consume for OpenGL's texture creation on std.
    // gpu's. It is the job of the video pipeline's autoplugger to plug in proper
    // color & format conversion plugins to satisfy videosink's needs.
    gst_app_sink_set_caps(GST_APP_SINK(videosink), colorcaps);

    // Install callbacks used by the videosink (appsink) to announce various events:
    gst_app_sink_set_callbacks(GST_APP_SINK(videosink), &videosinkCallbacks, &(vidcapRecordBANK[slotid]), PsychDestroyNotifyCallback);

    // ROI rectangle specified?
    if (capturerectangle) {
        if ((capturerectangle[kPsychLeft] == 0) && (capturerectangle[kPsychTop] == 0)) {
            // roi = [0 0 w h] --> Specs a target capture resolution.
            // Extract wanted width and height and use it as target capture resolution:
            twidth  = (int) PsychGetWidthFromRect(capturerectangle);
            theight = (int) PsychGetHeightFromRect(capturerectangle);
        } else {
            // roi = [l t r b] --> Specs a ROI to crop out of full res capture.

            // Don't change videocapture resolution -- Leave it at auto-detected settings:
            twidth  = -1;
            theight = -1;

            // Create videocrop filter to crop away everything outside the defined ROI:
            videocrop_filter = gst_element_factory_make ("videocrop", "ptbvideocropfilter");
            if (!videocrop_filter) {
                // Disable capturerectangle, so we revert to full device default resolution:
                capturerectangle = NULL;
                if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Selection of specified video ROI not supported by setup. Using full resolution.\n");
            }
        }
    }
    else {
        // No user specified resolution. Rely on auto-detection:
        twidth  = -1;
        theight = -1;
    }

    // Should we disable application of colorcaps as filter-caps to video source? By default we disable,
    // but an optional recordingflag of 4096 would opt-in to use of filter-caps for higher performance.
    // We want to disable this by default because we want maximum acceptance of different color formats
    // from the video source, e.g., to accomodate exotic video sources, at the expense of some performance
    // loss due to need of an extra colorspace / color format conversion step down the pipeline:
    if (!(recordingflags & 4096)) {
        // Yes:
        if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: OpenVideoCapture: Disabling use of colorcaps as filter-caps. Lower performance, but higher compatibility.\n");

        // Disable: We do this by deleting colorcaps and creating them as empty
        // caps object, so they don't impose restriction on color encoding if they get
        // applied downstream, e.g., as viewfinder-caps or capture-caps:
        gst_caps_unref(colorcaps);

        // No size/resolution specified?
        if (twidth == -1 && theight == -1) {
            // Totally unconstrained format:
            colorcaps = gst_caps_new_any();
        }
        else {
            // Non-Default resolution requested. Need to set a bit more explicit caps
            // for this to work -- at least basic color format:
            if (reqdepth == 3 || reqdepth == 4) {
                // RGB8 or RGBA8:
                colorcaps = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, (reqdepth == 4) ? "BGRA" : "RGB", NULL);
            }
            else if (reqdepth == 1 || (reqdepth == 2 && capdev->pixeldepth != 12 && capdev->pixeldepth != 16)) {
                // GRAY8:
                colorcaps = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, (bitdepth > 8) ? "GRAY16_LE" : "GRAY8", NULL);
            }
            else {
                // UYVY or I420:
                colorcaps = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, (capdev->pixeldepth == 16) ? "UYVY" : "I420", NULL);
            }
        }
    }

    // Assign our special appsink 'videosink' as video-sink of the pipeline:
    g_object_set(G_OBJECT(camera), "viewfinder-sink", videosink, NULL);

    // Setup pipeline for video recording or pure capture:
    if (!capdev->recording_active) {
        // Pure video capture, no recording: Optimize pipeline for this case:
        // We distinguish 8 bpc mode and > 8 bpc mode. In 8 bpc mode, we leave all builtin
        // video converters active, by setting flags to 0, so we can accomodate the widest
        // possible range of consumer class, off-the-shelf cameras in classic 8 bpc mode.
        // In > 8 bpc precision mode with RGB or RGBA layers (3 or 4), we must use flags
        // 2+4+8 to disable video filters (scaling, cropping, extra colorspace conversions),
        //  as they would not allow passthrough of > 8 bpc x-raw-rgb data, only 8 and 16 bpp
        // x-raw-gray. Note that the mandatory camerawrapperbinsrc is another limiting factor
        // for bitdepth, because it contains a videocrop element as of August 2014, which will
        // limit bitdepth to 8 bpc, so as of 2014, camerabin based video capture or recording
        // won't work with bitdepth > 8 bpc, except for grayscale :-(
        //
        // The tweaks here for camerabin are more of an investment into a brighter future with a better camerawrapperbinsrc:
        if ((bitdepth > 8) && (reqdepth == 3 || reqdepth == 4)) {
            // 16 bpc troublemakers: RGB and RGBA:
            g_object_set(G_OBJECT(camera), "flags", 2+4+8, NULL);
        }
        else {
            // 8 bpc any or 16 bpc grayscale only:
            g_object_set(G_OBJECT(camera), "flags", 0, NULL);
        }
    } else {
        // Video recording (with optional capture). Setup pipeline:
        g_object_set(G_OBJECT(camera),
                    "flags", ((bitdepth > 8) && (reqdepth == 3 || reqdepth == 4)) ? 2+4+8 : 0,
                    NULL);

        // Audio recording requested?
        if (recordingflags & 2) {
            // Yes:
            g_object_set(G_OBJECT(camera),
                        "mute", FALSE,
                        NULL);
        }
        else {
            // No:
            g_object_set(G_OBJECT(camera),
                        "mute", TRUE,
                        NULL);
        }
    }

    // Create and use a videorate converter always when video recording is active (because without it
    // we can get choppy or truncated video recordings and/or audio-video sync problems), and
    // optionally in pure video feedback mode if explicitely requested by usercode via
    // recordingflags & 128, as this is usually not what one wants for live video processing or
    // feedback.
    if (capdev->recording_active || (recordingflags & 128)) {
        // Attach a videorate converter as source filter. It should take care of adjusting
        // for fluctuating framerates from the videosource device by dropping or duplicating
        // videoframes and adjusting timestamps as needed to generate a video stream with the
        // selected target output framerate:
        capdev->videorate_filter = gst_element_factory_make ("videorate", "ptbvideoratefilter");
        if (capdev->videorate_filter == NULL) {
            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: Could not create video rate conversion filter. Therefore can't compensate\n");
                printf("PTB-WARNING: for possible fluctuations in videocapture framerate. This can cause\n");
                printf("PTB-WARNING: audio-video sync problems or wrong timing/speed in recorded videos!\n");
            }
        }
        else {
            // By default we apply the videorate converter upstream at the videosource,
            // so it affects both video recording and live video feedback. Usercode can
            // restrict conversion to the recorded video stream only by setting
            // recordingflags & 256:
            if (recordingflags & 256) {
                // Apply to video recording only -- attach to video-filter:
                g_object_set(G_OBJECT(camera), "video-filter", capdev->videorate_filter, NULL);
                if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Restricting video framerate conversion to recorded video.\n");
            }
            else {
                // Attach as video-source-filter -- upstream right at the source:
                videosource_filter = capdev->videorate_filter;
                if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Video framerate conversion applies to recorded video and live video.\n");
            }
        }
    }

    // We need to READY the pipeline, otherwise the queries and validations below will fail
    // due to lack of available video capture caps:
    if (!PsychVideoPipelineSetState(camera, GST_STATE_READY, 30.0)) {
        PsychGSProcessVideoContext(&(vidcapRecordBANK[slotid]), TRUE);
        PsychErrorExitMsg(PsychError_user, "In OpenVideoCapture: Opening the video capture device failed during camerabin pipeline zero -> ready. Reason given above.");
    }

    // Specific capture resolution requested?
    if ((twidth != -1) && (theight != -1)) {
        // Yes. Validate and request it.

        // The usual crap. Enumeration of supported resolutions doesn't work with various video sources, so
        // we skip validation and trust blindly that the usercode is right if this is one of the non-enumerating
        // video sources:
        if (!strstr(plugin_name, "dshowvideosrc") && !strstr(plugin_name, "autovideosrc") && !strstr(plugin_name, "videotestsrc") &&
            !strstr(plugin_name, "aravissrc") && !strstr(plugin_name, "gstlaunchbinsrc")) {
            // Query camera if it supports the requested resolution:
            capdev->fps = -1;
            if (!PsychGSGetResolutionAndFPSForSpec(capdev, &twidth, &theight, &capdev->fps, reqdepth, bitdepth)) {
                // Unsupported resolution. Game over!
                printf("PTB-ERROR: Video capture device %i doesn't support requested video capture resolution of %i x %i pixels! Aborted.\n", slotid, twidth, theight);
                PsychErrorExitMsg(PsychError_user, "Failed to open video device at requested video resolution.");
            }
        }

        // Resolution supported. Request it by setting up colorcaps for camerabin:
        gst_caps_set_simple(colorcaps, "width", G_TYPE_INT, twidth, "height", G_TYPE_INT, theight, NULL);
        g_object_set(G_OBJECT(camera), "viewfinder-caps", colorcaps, NULL);
        if (capdev->recording_active) {
            g_object_set(G_OBJECT(camera), "video-capture-caps", colorcaps, NULL);
        }

        // Assign requested and validated resolution as capture resolution of video source:
        capdev->width = twidth;
        capdev->height = theight;
        capdev->fps = 0;
    }
    else {
        // No, usercode wants auto-detected default resolution. Query
        // highest possible resolution (maximum pixel area) and choose that:
        capdev->width = -1;
        capdev->height = -1;
        capdev->fps = -1;

        // Auto-Detection doesn't work with various video source plugins. Skip it and hope that later probing code will do the job:
        if ((!strstr(plugin_name, "dshowvideosrc") && !strstr(plugin_name, "autovideosrc") && !strstr(plugin_name, "videotestsrc") &&
            !strstr(plugin_name, "aravissrc") && !strstr(plugin_name, "gstlaunchbinsrc")) &&
            PsychGSGetResolutionAndFPSForSpec(capdev, &capdev->width, &capdev->height, &capdev->fps, reqdepth, bitdepth)) {
            // Resolution properly auto-detected. Request it by setting colorcaps for camerabin:
            gst_caps_set_simple(colorcaps, "width", G_TYPE_INT, capdev->width, "height", G_TYPE_INT, capdev->height, NULL);
            g_object_set(G_OBJECT(camera), "viewfinder-caps", colorcaps, NULL);
            if (capdev->recording_active) {
                g_object_set(G_OBJECT(camera), "video-capture-caps", colorcaps, NULL);
            }
        }
        else {
            // Source without enumeration capability. Set "don't know" values and hope the fallback code below
            // does a better job at guessing the true source resolution:
            capdev->width  = 0;
            capdev->height = 0;

            // ROI defined? We can't handle this, because we don't know the true video capture resolution
            // which would be needed at this point in the setup path due to the broken enumeration.
            if (capturerectangle) {
                capturerectangle = NULL;
                overrideFrameSize = TRUE;

                // Delete useless videocrop element if any:
                if (videocrop_filter) gst_object_unref(G_OBJECT(videocrop_filter));
                videocrop_filter = NULL;

                if (PsychPrefStateGet_Verbosity() > 1) {
                    printf("PTB-WARNING: Usercode specified a 'roirectangle' with a ROI for video cropping, but this system setup\n");
                    printf("PTB-WARNING: doesn't support this. Ignoring 'roirectangle' and reverting to full video capture resolution.\n");
                }
            }
        }

        // Reset capdev->fps to neutral zero: capdev->width and capdev->height are already auto-assigned.
        capdev->fps = 0;
    }

    // We can only assign videosource_filters (for videorate plugin) or videocrop_filter
    // to camerabin while the pipeline is in NULL state. Therefore make it so:
    if (videosource_filter || videocrop_filter) {
        if (!PsychVideoPipelineSetState(camera, GST_STATE_NULL, 30.0)) {
            PsychGSProcessVideoContext(&(vidcapRecordBANK[slotid]), TRUE);
            PsychErrorExitMsg(PsychError_user, "In OpenVideoCapture: Opening the video capture device failed during intermediate camerabin pipeline ready -> zero transition. Reason given above.");
        }
    }

    // videocrop_filter for ROI processing available?
    if (videocrop_filter) {
        // Setup final cropping region:
        g_object_set(G_OBJECT(videocrop_filter),
                    "left",   (int) capturerectangle[kPsychLeft],
                    "top",    (int) capturerectangle[kPsychTop],
                    "right",  capdev->width - (int) capturerectangle[kPsychRight],
                    "bottom", capdev->height - (int) capturerectangle[kPsychBottom],
                    NULL);

        // Check if live feed is enabled and ROI cropping for it is not disabled:
        if (!(recordingflags & 1024) && !(recordingflags & 4)) {
            g_object_set(G_OBJECT(camera), "viewfinder-filter", videocrop_filter, NULL);

            // We need to adapt the caps of the viewfinder to expect image data of the
            // expected size after cropping takes place:
            vfcaps = gst_caps_copy(colorcaps);
            gst_caps_set_simple(vfcaps, "width", G_TYPE_INT, (int) PsychGetWidthFromRect(capturerectangle),
                                "height", G_TYPE_INT, (int) PsychGetHeightFromRect(capturerectangle), NULL);
            g_object_set(G_OBJECT(camera), "viewfinder-caps", vfcaps, NULL);
            gst_caps_unref(vfcaps);

            // No stupid hack needed in this case:
            overrideFrameSize = FALSE;
        }
        else {
            // Disable application of ROI to video buffers:
            overrideFrameSize = TRUE;
        }

        // Is videorecording active and cropping for it requested?
        if ((recordingflags & 512) && capdev->recording_active) {
            // Need to attach to "video-filter", which affects video encoding and recording.
            // We need to setup a 2nd cropping filter with identical settings for this, unless
            // the 1st one isn't used as the viewfinder-filter.
            if ((recordingflags & 1024) || (recordingflags & 4)) {
                // Live feed disabled or no cropping for live feed wanted. The videocrop_filter
                // is unused and we can use it here for cropping the videorecording by
                // attaching to video-filter:
                g_object_set(G_OBJECT(camera), "video-filter", videocrop_filter, NULL);
            }
            else {
                // videocrop_filter already used for cropping the live feed aka viewfinder.
                // Generate a new videocrop element, set it up identically, attach it to
                // video-filter:
                videocrop_filter = gst_element_factory_make ("videocrop", "ptbvideoreccropfilter");
                if (!videocrop_filter) {
                    if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Failed to apply video ROI to videorecording! Using full resolution.\n");
                }
                else {
                    g_object_set(G_OBJECT(videocrop_filter),
                                "left",   (int) capturerectangle[kPsychLeft],
                                "top",    (int) capturerectangle[kPsychTop],
                                "right",  capdev->width - (int) capturerectangle[kPsychRight],
                                "bottom", capdev->height - (int) capturerectangle[kPsychBottom],
                                NULL);
                    g_object_set(G_OBJECT(camera), "video-filter", videocrop_filter, NULL);
                }
            }

            // Cropping applies to video frames which go to the encoder. Adapt video encoder bin caps
            // to expect image data of the size after cropping takes place:
            reccaps = gst_caps_copy(colorcaps);
            gst_caps_set_simple(reccaps, "width", G_TYPE_INT, (int) PsychGetWidthFromRect(capturerectangle),
                                "height", G_TYPE_INT, (int) PsychGetHeightFromRect(capturerectangle), NULL);
            g_object_set(G_OBJECT(camera), "video-capture-caps", reccaps, NULL);
            gst_caps_unref(reccaps);
        }
        // End of video cropping setup for ROI implementation.
    }

    // Attach videosource filter upstream at video source, if any. This way it affects
    // both video recording and video live feed:
    if (videosource_filter) {
        // Can't attach to camerabin directly, but to wrappercamerasrc:
        g_object_set(G_OBJECT(videowrappersrc), "video-source-filter", videosource_filter, NULL);
    }

    gst_caps_unref(colorcaps);

    // Disable asynchronous transition to GST_PAUSED state for video sink when simultaneous
    // video recording is active: TODO FIXME: This seems to have no effect whatsoever....
    if (capdev->recording_active) g_object_set(G_OBJECT(videosink), "async", FALSE, NULL);

    // Disable internal queuing of the last sample, as we don't ever use the "last-sample" property:
    g_object_set(G_OBJECT(videosink), "enable-last-sample", FALSE, NULL);

    // Get the pad from the final sink for probing width x height of video frames and nominal framerate of video source:
    pad = gst_element_get_static_pad(videosink, "sink");

    if ((capdev->recording_active) && (recordingflags & 4)) {
        // No live feedback (flags 4). Implement this by setting the limit of
        // queued buffers in appsink to 1. This will only store at most one
        // buffer in the sink and then drop all future buffers. The buffer drain
        // routine in StopVideoCapture will dispose of that one useless
        // buffer at capture stop time:
        capdev->num_dmabuffers = 1;

        if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Only harddisc recording, no live video display (recordingflags & 4).\n");
    }

    // Specific maximum number of buffers to use?
    if (capdev->num_dmabuffers) {
        // Yes: Drop frames if callback can't pull buffers fast enough:
        gst_app_sink_set_drop(GST_APP_SINK(videosink), TRUE);

        // Allownum_dmabuffers to be queued before dropping buffers:
        gst_app_sink_set_max_buffers(GST_APP_SINK(videosink), capdev->num_dmabuffers);
    }
    else {
        // No: Leave queue length at default and disable buffer dropping:
        gst_app_sink_set_drop(GST_APP_SINK(videosink), FALSE);
    }

    PsychGSProcessVideoContext(&(vidcapRecordBANK[slotid]), FALSE);

    if (strstr(plugin_name, "dv1394src") || strstr(gstlaunchbinsrc, "ptbdvsource")) {
        recordingflags |= 8;
        if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Pipeline preroll skipped because DV capture via dv1394src or hdv1394src is active.\n");
    }

    // Only preroll if prerolling not disabled by recordingflag 8,
    // or if we use the fallback path, which utterly needs this:
    if (!(recordingflags & 8)) {
        // Ready the pipeline:
        if (!PsychVideoPipelineSetState(camera, GST_STATE_READY, 30.0)) {
            PsychGSProcessVideoContext(&(vidcapRecordBANK[slotid]), TRUE);
            PsychErrorExitMsg(PsychError_user, "In OpenVideoCapture: Opening the video capture device failed during pipeline zero -> ready. Reason given above.");
        }

        // Should we dump the whole encoding pipeline graph to a file for visualization
        // with GraphViz? This can be controlled via PsychTweak('GStreamerDumpFilterGraph' dirname);
        if (getenv("GST_DEBUG_DUMP_DOT_DIR")) {
            // Dump complete capture/recording filter graph to a .dot file for later visualization with GraphViz:
            printf("PTB-DEBUG: Dumping potential pre-preroll video capture/recording graph to directory %s.\n", getenv("GST_DEBUG_DUMP_DOT_DIR"));
            GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(camera), GST_DEBUG_GRAPH_SHOW_ALL, "PsychVideoCaptureGraphPostReadyPrePlaying");
        }

        // Preroll the pipeline:
        if (!PsychVideoPipelineSetState(camera, GST_STATE_PLAYING, 30.0)) {
            PsychGSProcessVideoContext(&(vidcapRecordBANK[slotid]), TRUE);
            PsychErrorExitMsg(PsychError_user, "In OpenVideoCapture: Opening the video capture device failed during pipeline preroll ready->playing. Reason given above.");
        }
    }

    g_object_get (G_OBJECT(camera), "camera-source", &videosource,NULL);

    // There's always at least a video channel:
    vidcapRecordBANK[slotid].nrVideoTracks = 1;

    // We need a valid onscreen window handle for real video playback:
    if ((NULL == win) && !(recordingflags & 4)) {
        PsychErrorExitMsg(PsychError_user, "No windowPtr to an onscreen window provided. Must do so for sources with video channels!");
    }

    PsychGSProcessVideoContext(&(vidcapRecordBANK[slotid]), FALSE);

    // Assign harmless initial settings for fps and frame size:
    rate1 = 0;
    rate2 = 1;
    width = height = 0;
    caps = NULL;

    // Query video frame size and framerate of device:
    peerpad = gst_pad_get_peer(pad);
    if (GST_IS_PAD(peerpad)) caps = gst_pad_get_current_caps(peerpad);

    if (caps) {
        str=gst_caps_get_structure(caps,0);

        /* Get some data about the frame */
        gst_structure_get_int(str,"width",&width);
        gst_structure_get_int(str,"height",&height);
        gst_structure_get_fraction(str, "framerate", &rate1, &rate2);
        gst_caps_unref(caps);
        if (PsychPrefStateGet_Verbosity() > 4) printf("Negotiated videosink res: %i x %i\n", width, height);
    } else {
        if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: No frame info available after preroll.\n");
    }

    // Release the pad:
    gst_object_unref(pad);

    // Our camera should be ready: Assign final handle.
    *capturehandle = slotid;
    capdev->capturehandle = slotid;

    // Increase counter of open capture devices:
    numCaptureRecords++;

    // Assign our first guess on video device resolution, only already
    // setup by detection code above:
    if ((capdev->width == 0) && (capdev->height == 0)) {
        capdev->width = width;
        capdev->height = height;
    }

    // Sanity check: grabber_active should be false, because we initialized it so.
    // It will be true if the videosink went EOS during the open and preroll sequence
    // due to something going wrong (see PsychEOSCallback()). We try to catch this here
    // and abort the open sequence instead of later hanging the whole app...
    PsychLockMutex(&capdev->mutex);
    if (capdev->grabber_active) {
        capdev->grabber_active = false;
        PsychUnlockMutex(&capdev->mutex);
        PsychErrorExitMsg(PsychError_user, "In OpenVideoCapture: Opening the video capture device failed during preroll due to premature EOS.");
    }
    PsychUnlockMutex(&capdev->mutex);

    // Reset framecounter:
    capdev->nrframes = 0;
    capdev->grabber_active = 0;

    rate1 = 0;
    rate2 = 1;
    width = height = 0;

    // Query true properties of attached video source:
    videosource = NULL;
    g_object_get (G_OBJECT(videowrappersrc), "video-source", &videosource, NULL);

    // Our camerabin2 doesn't use explicit video encoder, but video encoding profiles...
    capdev->videoenc = NULL;

    if (videosource) {
        pstring = NULL;
        if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "device")) {
            g_object_get(G_OBJECT(videosource), "device", &pstring, NULL);
            if (pstring) {
                if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: Camera device name is '%s'.\n", pstring);
                g_free(pstring); pstring = NULL;
            }
        }

        if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "device-path")) {
            g_object_get(G_OBJECT(videosource), "device-path", &pstring, NULL);
            if (pstring) {
                if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: Camera device-path is '%s'.\n", pstring);
                g_free(pstring); pstring = NULL;
            }
        }

        if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "device-name")) {
            g_object_get(G_OBJECT(videosource), "device-name", &pstring, NULL);
            if (pstring) {
                if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: Camera friendly device-name is '%s'.\n", pstring);
                capdev->cameraFriendlyName = strdup(pstring);
                g_free(pstring); pstring = NULL;
            }
            else {
                capdev->cameraFriendlyName = strdup("Unknown");
            }
        }

        if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "camera-name")) {
            g_object_get(G_OBJECT(videosource), "camera-name", &pstring, NULL);
            if (pstring) {
                if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: Camera friendly device-name is '%s'.\n", pstring);
                capdev->cameraFriendlyName = strdup(pstring);
                g_free(pstring); pstring = NULL;
            }
            else {
                capdev->cameraFriendlyName = strdup("Unknown");
            }
        }

        // Get the pad from the src pad of the source for probing width x height
        // of video frames and nominal framerate of video source:
        pad = gst_element_get_static_pad(videosource, "src");

        // Yes: Query video frame size and framerate of device:
        peerpad = gst_pad_get_peer(pad);
        caps = NULL;

        if (GST_IS_PAD(peerpad)) caps = gst_pad_get_current_caps(peerpad);

        if (caps) {
            str=gst_caps_get_structure(caps,0);

            /* Get some data about the frame */
            gst_structure_get_int(str,"width",&width);
            gst_structure_get_int(str,"height",&height);
            gst_structure_get_fraction(str, "framerate", &rate1, &rate2);
            if (PsychPrefStateGet_Verbosity() > 4) printf("Negotiated videosource w = %i h = %i fps = %f\n", width, height, rate1/rate2);
            gst_caps_unref(caps);
        } else {
            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: No frame info for video source available after preroll.\n");
        }

        // Release the pad:
        gst_object_unref(pad);
    }

    // If correct width x height not yet assigned, retry assignment from
    // results of videosource query:
    if (capdev->width == 0) capdev->width = width;
    if (capdev->height == 0) capdev->height = height;

    // If we totally failed to query width and height, set what usercode
    // requested - if it requested anything:
    if (capdev->width == 0) capdev->width = (twidth > 0) ? twidth : 0;
    if (capdev->height == 0) capdev->height = (theight > 0) ? theight : 0;

    // Create final user-visible effective ROI:
    if (capturerectangle && ((twidth == -1) && (theight == -1))) {
        // Usercode specified a ROI, videosource is left at its full default
        // resolution, ROI is applied via videocrop_filter. Specified ROI is
        // effective ROI, so just copy it:
        PsychCopyRect(capdev->roirect, capturerectangle);
    } else {
        // Usercode specified no ROI, but a target device resolution or no
        // resolution at all. Videosource is configured for a certain resolution,
        // as stored in capdev->width x height. Our effective ROI is simply
        // the full videosource resolution:
        PsychMakeRect(capdev->roirect, 0, 0, capdev->width, capdev->height);
    }

    // The size of returned video frames for texture creation or image buffer return
    // etc., is equal to the width x height of the ROI:
    capdev->frame_width  = (int) PsychGetWidthFromRect(capdev->roirect);
    capdev->frame_height = (int) PsychGetHeightFromRect(capdev->roirect);

    // Do we need to force the frame_width x frame_height to full capture
    // resolution?
    if (overrideFrameSize) {
        // Yes.
        capdev->frame_width  = capdev->width;
        capdev->frame_height = capdev->height;
    }

    PsychGSDrainBufferQueue(capdev, INT_MAX, 0);

    // If we prerolled before, we need to undo its effects:
    if (!(recordingflags & 8)) {
        // Pause the pipeline again:
        if (!PsychVideoPipelineSetState(camera, GST_STATE_READY, 30.0)) {
            PsychGSProcessVideoContext(&(vidcapRecordBANK[slotid]), TRUE);
            PsychErrorExitMsg(PsychError_user, "In OpenVideoCapture: Opening the video capture device failed during preroll playing -> pause. Reason given above.");
        }
    }

    // Obsolete on Linux, hopefully also on Windows, but needed on MacOS/X:
    // Make sure our capture timestamps are in system time (GetSecs() reference time)
    // unless recordingflags 64 are set, in which case timestamps are in pipeline
    // running time, or whatever GStreamer finds convenient.
    if ((PSYCH_SYSTEM == PSYCH_OSX) && !(recordingflags & 64)) {
        // Set defined start time for pipeline - Used for cts timestamps:
        gst_element_set_base_time(camera, GST_CLOCK_TIME_NONE);
        gst_element_set_start_time(camera, GST_CLOCK_TIME_NONE);
    }

    // Assign final recordingflags:
    vidcapRecordBANK[slotid].recordingflags = recordingflags;

    // Assign plugin name:
    sprintf(&capdev->videosourcename[0], "%s", plugin_name);

    // Store a pointer to the videosource plugin:
    capdev->videosource = videosource;
    capdev->videowrappersrc = videowrappersrc;

    if (PsychPrefStateGet_Verbosity() > 2) {
        printf("PTB-INFO: Camera %i opened [Source resolution width x height = %i x %i, video image size %i x %i]\n",
        slotid, capdev->width, capdev->height, capdev->frame_width, capdev->frame_height);
    }

    // Sanity check: grabber_active should be false, because we initialized it so.
    // It will be true if the videosink went EOS during the open and preroll sequence
    // due to something going wrong (see PsychEOSCallback()). We try to catch this here
    // and abort the open sequence instead of later hanging the whole app...
    PsychLockMutex(&capdev->mutex);
    if (capdev->grabber_active) {
        capdev->grabber_active = false;
        PsychUnlockMutex(&capdev->mutex);
        PsychErrorExitMsg(PsychError_user, "In OpenVideoCapture: Opening the video capture device failed during preroll due to premature EOS.");
    }
    PsychUnlockMutex(&capdev->mutex);

    PsychGSDrainBufferQueue(capdev, INT_MAX, 0);

    return(TRUE);
}

/* Internal helper:
 * Drain up to 'numFramesToDrain' videobuffers from the videosink of 'capdev'.
 * 'flags' modify drain behaviour. Unused so far.
 * Return number of drained buffers.
 */
int PsychGSDrainBufferQueue(PsychVidcapRecordType* capdev, int numFramesToDrain, unsigned int flags)
{
    GstSample *videoSample = NULL;
    int drainedCount = 0;

    if ((capdev->frameAvail > (int) gst_app_sink_get_max_buffers(GST_APP_SINK(capdev->videosink))) &&
        (gst_app_sink_get_max_buffers(GST_APP_SINK(capdev->videosink)) > 0))
        capdev->frameAvail = gst_app_sink_get_max_buffers(GST_APP_SINK(capdev->videosink));

    // Drain while anything available, but at most numFramesToDrain frames.
    while (GST_IS_APP_SINK(capdev->videosink) && !gst_app_sink_is_eos(GST_APP_SINK(capdev->videosink))
        && ((capdev->frameAvail > 0) || (flags & 0x1)) && (numFramesToDrain > drainedCount)) {
        if (capdev->frameAvail > 0) capdev->frameAvail--;
        videoSample = gst_app_sink_pull_sample(GST_APP_SINK(capdev->videosink));
        gst_sample_unref(videoSample);
        videoSample = NULL;
        drainedCount++;
    }

    return(drainedCount);
}

/*
 *  PsychGSVideoCaptureRate() - Start- and stop video capture.
 *
 *  capturehandle = Grabber to start-/stop.
 *  capturerate = zero == Stop capture, non-zero == Capture at given rate if possible.
 *  dropframes = 0 - Always deliver oldest frame in DMA ringbuffer. 1 - Always deliver newest frame.
 *               --> 1 == drop frames in ringbuffer if behind -- low-latency capture.
 *
 *               If dropframes is 1 at 'stop' time, then discard all pending buffers in queue,
 *               otherwise retain them so they can be fetched after stop.
 *
 *  startattime = Deadline (in system time) for which to wait before real start of capture.
 *  Returns Number of dropped frames during capture.
 */
int PsychGSVideoCaptureRate(int capturehandle, double capturerate, int dropframes, double* startattime)
{
    GstElement              *camera = NULL;
    GstBuffer               *videoBuffer = NULL;
    guint64                 nrInFrames, nrOutFrames, nrDroppedFrames, nrDuplicatedFrames;
    GstCaps                 *caps = NULL;
    GstCaps                 *capsi = NULL;
    GstCaps                 *capss = NULL;
    GstCaps                 *capsr = NULL;
    psych_bool              fps_matched = FALSE;
    int                     idx, idx2, fps_n, fps_d;
    double                  fpsmin, fpsmax;
    int                     dropped = 0;
    int                     drainedCount;
    float                   framerate = 0;

    // Retrieve device record for handle:
    PsychVidcapRecordType* capdev = PsychGetGSVidcapRecord(capturehandle);
    camera = capdev->camera;

    // Make sure GStreamer is ready:
    PsychGSCheckInit("videocapture");

    PsychGSProcessVideoContext(capdev, FALSE);

    // Start- or stop capture?
    if (capturerate > 0) {
        // Start capture:
        if (capdev->grabber_active) PsychErrorExitMsg(PsychError_user, "You tried to start video capture, but capture is already started!");

        // Reset statistics:
        capdev->last_pts = -1.0;
        capdev->nr_droppedframes = 0;
        capdev->lastSavedBaseTime = 0;

        // Framedropping in the sense we define it is not supported by libGStreamer, so we implement it ourselves.
        // Store the 'dropframes' flag in our capdev struct, so the PsychGSGetTextureFromCapture()
        // knows how to handle this:
        capdev->dropframes = (dropframes > 0) ? 1 : 0;

        if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: StartVideoCapture: Draining any queued stale video buffers before start of capture.\n");
        drainedCount = PsychGSDrainBufferQueue(capdev, INT_MAX, 0);
        if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: StartVideoCapture: Drain of %i buffers completed.\n", drainedCount);

        // Start the video capture for this camera.
        if (!PsychVideoPipelineSetState(camera, GST_STATE_READY, 10.0)) {
            // Failed!
            PsychGSProcessVideoContext(capdev, FALSE);
            PsychErrorExitMsg(PsychError_user, "Failure in pipeline transition null -> ready - Start of video capture failed!");
        }

        // Requested capturerate is not DBL_MAX aka "auto-selected default"?
        if (capturerate < DBL_MAX) {
            // Specific capturerate requested.
            // Modify caps for camerabin2 - Add a "framerate" property matching our requirements, but first
            // validate the requested capturerate against supported capture rates of this device at its
            // current settings for resolution, color format and color depths.

            // Get current capture caps (so far without a framerate) - Store a writable copy of them in caps:
            g_object_get(G_OBJECT(camera), "viewfinder-caps", &capsi, NULL);
            caps = gst_caps_copy(capsi);
            capsr = gst_caps_copy(capsi);
            gst_caps_unref(capsi);

            // Get list of supported capture caps for this device in capss:
            g_object_get(G_OBJECT(camera), "viewfinder-supported-caps", &capss, NULL);

            // Intersect with current caps to find the subset of possible caps, given the
            // already set/fixed resolution and color format/depth. This essentially leaves
            // us with caps representing the available framerates in capsi:
            gst_caps_set_simple(capsr, "width", G_TYPE_INT, capdev->width, "height", G_TYPE_INT, capdev->height, NULL);
            capsi = gst_caps_intersect(capsr, capss);
            gst_caps_unref(capsr);
            gst_caps_unref(capss);

            // Print 'em:
            if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Videosource intersected caps are: %" GST_PTR_FORMAT "\n\n", capsi);

            // Match requested framerate against supported framerates:
            for (idx = 0; idx < (int) gst_caps_get_size(capsi); idx++) {
                // For idx'th cap, get framerate value with all framerates of that cap:
                GstStructure *capsstruct = gst_caps_get_structure (capsi, idx);
                const GValue* framerates = gst_structure_get_value(capsstruct, "framerate");

                // framerates can be in the format of a single fraction, a list of fractions, or
                // an allowable range of fractions:
                if (G_VALUE_HOLDS(framerates, gst_fraction_get_type())) {
                    if (gst_structure_get_fraction (capsstruct, "framerate", &fps_n, &fps_d)) {
                        if (fabs((int)(capturerate + 0.5) - ((double) fps_n / (double) fps_d)) < 1.0) fps_matched = TRUE;
                        if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Caps %i : FPS %f Hz.\n", idx, (float) fps_n / (float) fps_d);
                    }
                }
                else if (G_VALUE_HOLDS(framerates, gst_value_list_get_type())) {
                    for (idx2 = 0; idx2 < (int) gst_value_list_get_size(framerates); idx2++) {
                        const GValue* value = gst_value_list_get_value (framerates, idx2);
                        fps_n = gst_value_get_fraction_numerator(value);
                        fps_d = gst_value_get_fraction_denominator(value);
                        if (fabs((int)(capturerate + 0.5) - ((double) fps_n / (double) fps_d)) < 1.0) fps_matched = TRUE;
                        if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: %i-%i : FPS %f Hz.\n", idx, idx2, (float) fps_n / (float) fps_d);
                    }
                }
                else if (G_VALUE_HOLDS(framerates, gst_fraction_range_get_type())) {
                    const GValue* frmin = gst_value_get_fraction_range_min(framerates);
                    const GValue* frmax = gst_value_get_fraction_range_max(framerates);
                    fps_n = gst_value_get_fraction_numerator(frmin);
                    fps_d = gst_value_get_fraction_denominator(frmin);
                    fpsmin = (double) fps_n / (double) fps_d;
                    if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: %i: FPS min %f - ", idx, fpsmin);

                    fps_n = gst_value_get_fraction_numerator(frmax);
                    fps_d = gst_value_get_fraction_denominator(frmax);
                    fpsmax = (double) fps_n / (double) fps_d;
                    if (PsychPrefStateGet_Verbosity() > 4) printf("max %f\n", fpsmax);

                    if (((int)(capturerate + 0.5) >= fpsmin) && ((int)(capturerate + 0.5) <= fpsmax)) fps_matched = TRUE;
                }
            }
            gst_caps_unref(capsi);

            // Requested capturerate supported?
            if (fps_matched) {
                // Yes! Add it to our caps object:
                // CAUTION: Do use gst_fraction_get_type() and NOT the Macro GST_TYPE_FRACTION, otherwise delay-loading
                // on MS-Windows won't work!!!
                gst_caps_set_simple(caps, "framerate", gst_fraction_get_type(), (int)(capturerate + 0.5), 1, NULL);

                // Set caps object and thereby capture/recording framerate:
                g_object_set(G_OBJECT(camera), "viewfinder-caps", caps, NULL);

                // Free "used up" caps object with writable viewfinder caps:
                gst_caps_unref(caps);

                // If recording is active we want to add the "framerate" property to the recording
                // caps as well:
                if (capdev->recording_active) {
                    // Need to create a fresh copy of the current recording caps, so it is mutable
                    // and we can add the new "framerate" constraint/property.
                    g_object_get(G_OBJECT(camera), "video-capture-caps", &capsi, NULL);
                    caps = gst_caps_copy(capsi);
                    gst_caps_unref(capsi);

                    // Assign our validated recording framerate to recording caps:
                    gst_caps_set_simple(caps, "framerate", gst_fraction_get_type(), (int)(capturerate + 0.5), 1, NULL);

                    // Assign new caps as video recording caps:
                    g_object_set(G_OBJECT(camera), "video-capture-caps", caps, NULL);

                    // Free "used up" caps object with writable video recording caps:
                    gst_caps_unref(caps);
                }
            }
            else {
                // No. Play it safe and just refrain from setting a capture framerate. This will
                // run the capture and recording at whatever the default framerate of the device is.
                // However, warn user about unsupported setting and our fallback solution:
                if (PsychPrefStateGet_Verbosity() > 1) {
                    printf("PTB-WARNING: Video device %i does not support requested framerate %i fps at current settings for video resolution\n", capturehandle, (int)(capturerate + 0.5));
                    printf("PTB-WARNING: and color depth. Will workaround this by trying to capture at the default framerate of the device for current settings.\n");
                }

                // Free unneeded caps object with writable viewfinder caps:
                gst_caps_unref(caps);
            }
        }

        if (!PsychVideoPipelineSetState(camera, GST_STATE_PAUSED, 10.0)) {
            // Failed!
            PsychGSProcessVideoContext(capdev, FALSE);
            PsychErrorExitMsg(PsychError_user, "Failure in pipeline transition ready -> paused - Start of video capture failed!");
        }

        // Init parameters:
        capdev->last_pts = -1.0;
        capdev->nr_droppedframes = 0;
        capdev->frameAvail = 0;
        capdev->preRollAvail = 0;

        // Wait until start deadline reached:
        if (*startattime != 0) PsychWaitUntilSeconds(*startattime);

        // Start actual video capture and/or recording:
        if (PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Starting capture...\n");

        if (!PsychVideoPipelineSetState(camera, GST_STATE_PLAYING, 10.0)) {
            // Failed!
            PsychGSProcessVideoContext(capdev, FALSE);

            // Some hint for a trick that worked in the past: Skipping the Ready -> Paused -> Playing -> Ready
            // sequence for probing that is done by default in 'OpenVideoCapture'. From the fact that we reached
            // this point in execution, we know that 'OpenVideoCapture' succeeded, so that sequence worked at
            // least once, but the similar sequence here failed. Maybe we have only one shot at this, so better
            // use it here, where it really matters, by skipping the sequence at the beginning, ergo recordingflags = 8
            // is a good idea:
            if (!(capdev->recordingflags & 8)) {
                printf("PTB-HINT: When this error happened in past similar situations, sometimes it helped to set the\n");
                printf("PTB-HINT: optional 'recordingflags' parameter of the Screen('OpenVideoCapture', ..) command to\n");
                printf("PTB-HINT: a value of 8, so i'd suggest trying that.\n\n");
            }

            PsychErrorExitMsg(PsychError_user, "Failure in pipeline transition paused -> playing - Start of video capture failed!");
        }

        // Start video recording if requested:
        if (capdev->recording_active) {
            if (PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Starting recording...\n");
            g_object_set(G_OBJECT(camera), "mode", 2, NULL);
            if (PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Recording 1 started...\n");
            g_object_set(G_OBJECT(camera), "location", capdev->targetmoviefilename, NULL);
            if (PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Recording 2 started...\n");
            g_signal_emit_by_name (camera, "start-capture", 0, 0);
            if (PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Recording started...\n");
        }

        // Should we dump the whole encoding pipeline graph to a file for visualization
        // with GraphViz? This can be controlled via PsychTweak('GStreamerDumpFilterGraph' dirname);
        if (getenv("GST_DEBUG_DUMP_DOT_DIR")) {
            // Dump complete capture/recording filter graph to a .dot file for later visualization with GraphViz:
            printf("PTB-DEBUG: Dumping actual video capture/recording graph to directory %s.\n", getenv("GST_DEBUG_DUMP_DOT_DIR"));
            GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(camera), GST_DEBUG_GRAPH_SHOW_ALL, "PsychVideoCaptureGraphActual");
        }

        // Wait for real start of capture, i.e., arrival of 1st captured
        // video buffer:
        PsychLockMutex(&capdev->mutex);

        // This wait is just here to clear potentially pending conditions on
        // MS-Windows. Therefore only wait for 1.1 msecs at most:
        PsychTimedWaitCondition(&capdev->condition, &capdev->mutex, 0.0011);

        // Wait for first frame to become available:
        while (capdev->frameAvail == 0) {
            if (PsychPrefStateGet_Verbosity() > 5) {
                printf("PTB-DEBUG: Waiting for real start: fA = %i pA = %i fps=%f\n", capdev->frameAvail, capdev->preRollAvail, capdev->fps);
                fflush(NULL);
            }
            PsychTimedWaitCondition(&capdev->condition, &capdev->mutex, 10.0);
        }

        // Assign a plausible framerate if none assigned properly:
        if (capdev->fps <= 0) capdev->fps = capturerate;

        // Ok, capture is now started:
        capdev->grabber_active = 1;

        PsychUnlockMutex(&capdev->mutex);

        // Record real start time:
        PsychGetAdjustedPrecisionTimerSeconds(startattime);

        // Some processing time for message dispatch:
        PsychGSProcessVideoContext(capdev, FALSE);

        if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: Capture engine fully running...\n");

        if(PsychPrefStateGet_Verbosity() > 3) {
            printf("PTB-INFO: Capture started on device %i - Input video resolution %i x %i - Framerate: %f fps.\n",
                capturehandle, capdev->width, capdev->height, capdev->fps);
        }
    }
    else {
        // Stop capture:
        if (capdev->grabber_active) {
            // Store a backup copy of pipeline basetime for use in offline frame fetch:
            capdev->lastSavedBaseTime = gst_element_get_base_time(camera);

            // Video framerate converter attached?
            if (capdev->videorate_filter) {
                // Query its stats in dropped and duplicated frames:
                g_object_get(G_OBJECT(capdev->videorate_filter),
                        "in", &nrInFrames,
                        "out", &nrOutFrames,
                        "drop", &nrDroppedFrames,
                        "duplicate", &nrDuplicatedFrames,
                        NULL);
            }

            // Stop video recording if requested:
            if (capdev->recording_active) {
                PsychGSProcessVideoContext(capdev, FALSE);

                if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: StopVideoCapture: Stopping video recording...\n");
                g_signal_emit_by_name (camera, "stop-capture", 0);

                if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: StopVideoCapture: Stopping pipeline [playing -> paused]\n");
                if (!PsychVideoPipelineSetState(camera, GST_STATE_PAUSED, 10.0)) {
                    if (PsychPrefStateGet_Verbosity() > 0) {
                        PsychGSProcessVideoContext(capdev, FALSE);
                        printf("PTB-ERROR: StopVideoCapture: Unable to pause recording pipeline! Prepare for trouble!\n");
                    }
                }

                if(PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: StopVideoCapture: Videorecording stopped.\n");
            }

            // Stop pipeline, bring it back into READY state:
            if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: StopVideoCapture: Fully stopping and shutting down capture pipeline.\n");

            if (!PsychVideoPipelineSetState(camera, GST_STATE_READY, 10.0)) {
                if (PsychPrefStateGet_Verbosity() > 0) {
                    PsychGSProcessVideoContext(capdev, FALSE);
                    printf("PTB-ERROR: StopVideoCapture: Unable to stop capture pipeline [Transition 1 to ready state failed]! Prepare for trouble!\n");
                }
            }
            else {
                // Capture stopped. More cleanup work:
                PsychGSProcessVideoContext(capdev, FALSE);
                if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: StopVideoCapture: Stopped.\n");

                // Drain any queued buffers, if requested:
                if (dropframes) {
                    drainedCount = PsychGSDrainBufferQueue(capdev, INT_MAX, 0);
                    if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: StopVideoCapture: Drain of %i buffers completed. GStreamer idle, cleaning up...\n", drainedCount);
                }
            }

            // Ok, capture is now stopped.
            capdev->grabber_active = 0;

            if (capdev->scratchbuffer) {
                // Release scratch-buffer:
                free(capdev->scratchbuffer);
                capdev->scratchbuffer = NULL;
            }

            if (PsychPrefStateGet_Verbosity() > 3) {
                // Output count of dropped frames:
                if ((dropped=capdev->nr_droppedframes) > 0) {
                    printf("PTB-INFO: Video live capture dropped at least %i frames on device %i to keep capture running in sync with realtime.\n", dropped, capturehandle);
                }

                if (capdev->videorate_filter) {
                    printf("PTB-INFO: Framerate stabilization received %i frames, delivered %i frames, had to drop %i frames and duplicate %i frames\n",
                        (int) nrInFrames, (int) nrOutFrames, (int) nrDroppedFrames, (int) nrDuplicatedFrames);
                    printf("PTB-INFO: to compensate for framerate fluctuations or mismatch between expected vs. real framerate on device %i.\n", capturehandle);
                }

                if (capdev->nrframes>0) capdev->avg_decompresstime/= (double) capdev->nrframes;
                printf("PTB-INFO: Average time spent in video decompressor (waiting/polling for new frames) was %f milliseconds.\n", (float) capdev->avg_decompresstime * 1000.0f);

                if (capdev->nrgfxframes>0) capdev->avg_gfxtime/= (double) capdev->nrgfxframes;
                printf("PTB-INFO: Average time spent in GetCapturedImage (intensity calculation Video->OpenGL texture conversion) was %f milliseconds.\n",  (float) capdev->avg_gfxtime * 1000.0f);
            }
        }
    }

    // Reset framecounters and statistics:
    capdev->nrframes = 0;
    capdev->avg_decompresstime = 0;
    capdev->nrgfxframes = 0;
    capdev->avg_gfxtime = 0;

    // Return either the real capture framerate (at start of capture) or count of dropped frames - at end of capture.
    return((capturerate!=0) ? (int) (capdev->fps + 0.5) : dropped);
}


/*
 *  PsychGSGetTextureFromCapture() -- Create an OpenGL texturemap from a specific videoframe from given capture object.
 *
 *  win = Window pointer of onscreen window for which a OpenGL texture should be created.
 *  capturehandle = Handle to the capture object.
 *  checkForImage = >0 == Just check if new image available, 0 == really retrieve the image, blocking if necessary.
 *                   2 == Check for new image, block inside this function (if possible) if no image available.
 *
 *  timeindex = This parameter is currently ignored and reserved for future use.
 *  out_texture = Pointer to the Psychtoolbox texture-record where the new texture should be stored.
 *  presentation_timestamp = A ptr to a double variable, where the presentation timestamp of the returned frame should be stored.
 *  summed_intensity = An optional ptr to a double variable. If non-NULL, then average of intensities over all channels and pixels is calculated and returned.
 *  outrawbuffer = An optional ptr to a memory buffer of sufficient size. If non-NULL, the buffer will be filled with the captured raw image data, e.g., for use inside Matlab or whatever...
 *  Returns Number of pending or dropped frames after fetch on success (>=0), -1 if no new image available yet, -2 if no new image available and there won't be any in future.
 */
int PsychGSGetTextureFromCapture(PsychWindowRecordType *win, int capturehandle, int checkForImage, double timeindex,
                                PsychWindowRecordType *out_texture, double *presentation_timestamp, double* summed_intensity, rawcapimgdata* outrawbuffer)
{
    PsychVidcapRecordType *capdev;
    GstBuffer *videoBuffer = NULL;
    GstSample *videoSample = NULL;
    gint64 bufferIndex;
    double deltaT = 0;
    GstClockTime baseTime;

    int waitforframe;
    int w, h;
    psych_uint64 intensity = 0;
    unsigned int count, i;
    unsigned char* pixptr;
    psych_uint16* pixptrs;
    psych_bool newframe = FALSE;
    double tstart, tend;
    int nrdropped = 0;
    unsigned char* input_image = NULL;

    // Disable warning about missing field initializer in calls
    // like GstMapInfo mapinfo = = GST_MAP_INFO_INIT;
    // Not our bug, but GStreamer's, so suppress for now.
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    GstMapInfo mapinfo = GST_MAP_INFO_INIT;
    #pragma GCC diagnostic pop

    // Make sure GStreamer is ready:
    PsychGSCheckInit("videocapture");

    // Take start timestamp for timing stats:
    PsychGetAdjustedPrecisionTimerSeconds(&tstart);

    // Retrieve device record for handle:
    capdev = PsychGetGSVidcapRecord(capturehandle);

    // Allow context task to do its internal bookkeeping and cleanup work:
    PsychGSProcessVideoContext(capdev, FALSE);

    // If this is a pure audio capture with no video channels, we always return failed,
    // as those certainly don't have movie frames associated.
    if ((capdev->recordingflags & 4) || (capdev->nrVideoTracks == 0)) return((checkForImage) ? -1 : FALSE);

    if (!PsychIsOnscreenWindow(win)) {
        PsychErrorExitMsg(PsychError_user, "You need to pass a handle to an onscreen window, not to something else!");
    }

    // Compute width and height for later creation of textures etc. Need to do this here,
    // so we can return the values for raw data retrieval:
    w = capdev->frame_width;
    h = capdev->frame_height;

    // If a outrawbuffer struct is provided, we fill it with info needed to allocate a
    // sufficient memory buffer for returned raw image data later on:
    if (outrawbuffer) {
        outrawbuffer->w = w;
        outrawbuffer->h = h;
        outrawbuffer->depth = (capdev->reqpixeldepth !=2) ? capdev->reqpixeldepth : 1;
        outrawbuffer->bitdepth = (capdev->bitdepth > 8) ? 16 : 8;
    }

    // Blocking wait for new image requested?
    waitforframe = (checkForImage > 1) ? 1 : 0;

    // A checkForImage 4 means "no op" with the GStreamer capture engine: This is meant to drive
    // a single threaded movie recording engine, ie., grant processing time to it.
    // Our GStreamer engine doesn't need this as it is highly multi-threaded and does all
    // relevant work in the background.
    if (checkForImage == 4) return(0);

    // Should we just check for new image?
    if (checkForImage) {
        // Reset current dropped count to zero:
        capdev->current_dropped = 0;

        PsychLockMutex(&capdev->mutex);
        if (!capdev->frameAvail) {
            // No new frame available yet:

            if (capdev->grabber_active == 0) {
                // Grabber stopped. We'll never get a new image:
                PsychUnlockMutex(&capdev->mutex);
                return(-2);
            }

            // Blocking wait requested?
            if (!waitforframe) {
                // No blocking wait, just return "no new frame yet":
                PsychUnlockMutex(&capdev->mutex);
                //printf("PTB-DEBUG: NO NEW FRAME\n");
                return(-1);
            }

            // No new frame available. Perform a blocking wait:
            PsychTimedWaitCondition(&capdev->condition, &capdev->mutex, 10.0);

            // Recheck:
            if (!capdev->frameAvail) {
                // Game over! Wait timed out after 10 secs.
                PsychUnlockMutex(&capdev->mutex);
                if (PsychPrefStateGet_Verbosity()>4) printf("PTB-ERROR: In blocking wait: No new video frame received after timeout of 10 seconds! Aborting fetch.\n");
                return(-1);
            }

            // Have at least one new frame after wait :-)
        }

        // New frame available:

        // Store count of currently queued frames (in addition to the one just fetched).
        // This is an indication of how well the users script is keeping up with the video stream,
        // technically the number of frames that would need to be dropped to keep in sync with the
        // stream.
        capdev->current_dropped = capdev->frameAvail - 1;

        //printf("PTB-DEBUG: NEW FRAME %d\n", capdev->frameAvail);

        // Ok, at least one new frame ready. If more than one frame has queued up and
        // we are in 'dropframes' mode, ie. we should always deliver the most recent available
        // frame, then we quickly fetch & discard all queued frames except the last one.
        while((capdev->dropframes) && ((int) capdev->frameAvail > 1)) {
            // On frame to dequeue:
            capdev->frameAvail--;
            PsychUnlockMutex(&capdev->mutex);

            // Pull next buffer which is to be dropped:
            videoSample = gst_app_sink_pull_sample(GST_APP_SINK(capdev->videosink));

            // Release the capture buffer. Return it to the DMA ringbuffer pool:
            gst_sample_unref(videoSample);
            videoSample = NULL;

            // Retry if there are more pending frames to drop:
            PsychLockMutex(&capdev->mutex);
        }

        // Release lock:
        PsychUnlockMutex(&capdev->mutex);

        // Update stats for decompression:
        PsychGetAdjustedPrecisionTimerSeconds(&tend);

        // Increase counter of decompressed frames:
        capdev->nrframes++;

        // Update avg. decompress time:
        capdev->avg_decompresstime+= (tend - tstart);

        // Return availability status: At least one new frame available :-)
        return(0);
    }

    // This point is only reached if checkForImage == FALSE, which only happens
    // if a new frame is available in our buffer:
    PsychLockMutex(&capdev->mutex);

    //printf("PTB-DEBUG: Blocking fetch start %d\n", capdev->frameAvail);
    if (!capdev->frameAvail) {
        // No new frame available but grabber active. Perform a blocking wait:
        if (capdev->grabber_active) PsychTimedWaitCondition(&capdev->condition, &capdev->mutex, 10.0);

        // Recheck:
        if (!capdev->frameAvail) {
            // Game over! Wait timed out after 10 secs.
            PsychUnlockMutex(&capdev->mutex);
            if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: No new video frame received after timeout of 10 seconds! Something's wrong. Aborting fetch.\n");
            return(-1);
        }

        // At this point we should have at least one frame available.
    }

    // We're here with at least one frame available and the mutex lock held.
    if (PsychPrefStateGet_Verbosity()>6) printf("PTB-DEBUG: Pulling from videosink, %d buffers avail...\n", capdev->frameAvail);

    // One less frame available after our fetch:
    capdev->frameAvail--;

    // Clamp frameAvail to queue lengths, unless queue length is set to zero, which means "unlimited":
    if ((int) gst_app_sink_get_max_buffers(GST_APP_SINK(capdev->videosink)) < capdev->frameAvail) {
        if (gst_app_sink_get_max_buffers(GST_APP_SINK(capdev->videosink)) > 0) {
            capdev->frameAvail = gst_app_sink_get_max_buffers(GST_APP_SINK(capdev->videosink));
        }
    }
    if (PsychPrefStateGet_Verbosity()>6) printf("PTB-DEBUG: Post-Pulling from videosink, %d buffers avail...\n", capdev->frameAvail);

    // This will pull the oldest video buffer from the videosink. It would block if none were available,
    // but that won't happen as we wouldn't reach this statement if none were available. It would return
    // NULL if the stream would be EOS or the pipeline off, but that shouldn't ever happen:
    videoSample = gst_app_sink_pull_sample(GST_APP_SINK(capdev->videosink));

    // We can unlock early, thanks to videosink's internal buffering:
    PsychUnlockMutex(&capdev->mutex);

    if (videoSample) {
        videoBuffer = gst_sample_get_buffer(videoSample);

        // Map the buffers memory for reading:
        if (!gst_buffer_map(videoBuffer, &mapinfo, GST_MAP_READ)) {
            printf("PTB-ERROR: Failed to map video data of captured video frame! Something's wrong. Aborting fetch.\n");
            gst_buffer_unref(videoBuffer);
            videoBuffer = NULL;
            return(-1);
        }

        // Assign pointer to videoBuffer's data directly: Avoids one full data copy compared to oldstyle method.
        // input_image points to the image buffer in our cam:
        input_image = (unsigned char*) (GLuint*) mapinfo.data;

        // Assign pts presentation timestamp in pipeline stream time and convert to seconds:
        if (capdev->recordingflags & 64) {
            // Retrieve raw buffer timestamp - pipeline running time.
            capdev->current_pts = (double) GST_BUFFER_PTS(videoBuffer) / (double) 1e9;
        } else {
            // Add base time to convert running time buffer timestamp into absolute time:
            baseTime = gst_element_get_base_time(capdev->camera);
            if (baseTime == 0) baseTime = capdev->lastSavedBaseTime;

            capdev->current_pts = (double) (GST_BUFFER_PTS(videoBuffer) + baseTime) / (double) 1e9;

            // Apply corrective offset for GStreamer clock base zero point:
            capdev->current_pts+= gs_startupTime;
        }

        deltaT = 0.0;
        if (GST_CLOCK_TIME_IS_VALID(GST_BUFFER_DURATION(videoBuffer)))
            deltaT = (double) GST_BUFFER_DURATION(videoBuffer) / (double) 1e9;

        if (PsychPrefStateGet_Verbosity() > 6) {
            PsychProbeSampleProps(videoSample, NULL, NULL, &capdev->fps);
            printf("Bufferprobe: newfps = %f altfps = %f\n", capdev->fps, (float) ((deltaT > 0) ? 1.0 / deltaT : 0.0));
        }

        bufferIndex = GST_BUFFER_OFFSET(videoBuffer);
    } else {
        if (PsychPrefStateGet_Verbosity()>0) printf("PTB-ERROR: No new video frame received in gst_app_sink_pull_sample! Something's wrong. Aborting fetch.\n");
        return(-1);
    }
    if (PsychPrefStateGet_Verbosity()>6) printf("PTB-DEBUG: ...done.\n");

    // Presentation timestamp requested?
    if (presentation_timestamp) {
        // Return it:
        *presentation_timestamp = capdev->current_pts;
    }

    // Synchronous texture fetch: Copy content of capture buffer into a texture:
    // =========================================================================

    // Do we want to do something with the image data and have a
    // scratch buffer for color conversion alloc'ed? NOTE: This is currently a no-op, as scratchbuffer always == NULL
    if ((capdev->scratchbuffer) && ((out_texture) || (summed_intensity) || (outrawbuffer))) {
        // Yes. Perform color-conversion YUV->RGB from cameras DMA buffer
        // into the scratch buffer and set scratch buffer as source for
        // all further operations:
        memcpy(capdev->scratchbuffer, input_image, w * h * capdev->reqpixeldepth);

        // Ok, at this point we should have a RGB8 texture image ready in scratch_buffer.
        // Set scratch buffer as our new image source for all further processing:
        input_image = (unsigned char*) capdev->scratchbuffer;
    }

    // Only setup if really a texture is requested (non-benchmarking mode):
    if (out_texture) {
        // Activate OpenGL context of target window:
        PsychSetGLContext(win);

        #if PSYCH_SYSTEM == PSYCH_OSX
            // Explicitely disable Apple's Client storage extensions. For now they are not really useful to us.
            glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_FALSE);
        #endif

        // Set default rect of texture:
        PsychMakeRect(out_texture->rect, 0, 0, w, h);
        PsychCopyRect(out_texture->clientrect, out_texture->rect);

        // Set texture orientation as if it were an inverted Offscreen window: Upside-down.
        out_texture->textureOrientation = 3;

        // Setup a pointer to our buffer as texture data pointer: Setting memsize to zero
        // prevents unwanted free() operation in PsychDeleteTexture...
        out_texture->textureMemorySizeBytes = 0;

        // Set default texture depth: Could be 8, 16, 24 or 32 bpp.
        out_texture->depth = capdev->reqpixeldepth * 8;

        // 4-channel textures are aligned on 4 Byte boundaries because texels are RGBA8. If they
        // have an even number of pixels (width) per row, they are even 8 Byte aligned. Otherwise
        // we play safe and assume no alignment, ie., 1 Byte alignment:
        out_texture->textureByteAligned = (capdev->reqpixeldepth == 4) ? ((w % 2) ? 4 : 8) : 1;

        // This will retrieve an OpenGL compatible pointer to the pixel data and assign it to our texmemptr:
        out_texture->textureMemory = (GLuint*) input_image;

        // Special case depths == 2, aka YCBCR texture?
        if ((capdev->bitdepth <= 8) && (capdev->reqpixeldepth == 2) && (capdev->pixeldepth == 16) && (win->gfxcaps & kPsychGfxCapUYVYTexture)) {
            // GPU supports UYVY textures and we get data in that YCbCr format. Tell
            // texture creation routine to use this optimized format:
            if (!glewIsSupported("GL_APPLE_ycbcr_422")) {
                // No support for more powerful Apple extension. Use Linux MESA extension:
                out_texture->textureinternalformat = GL_YCBCR_MESA;
                out_texture->textureexternalformat = GL_YCBCR_MESA;
            } else {
                // Apple extension supported:
                out_texture->textureinternalformat = GL_RGB8;
                out_texture->textureexternalformat = GL_YCBCR_422_APPLE;
            }
            // Same enumerant for Apple and Mesa:
            out_texture->textureexternaltype   = GL_UNSIGNED_SHORT_8_8_MESA;

            // Number of effective channels is 3 for RGB8:
            out_texture->nrchannels = 3;

            // And 24 bpp depth:
            out_texture->depth = 24;

            // Byte alignment: For even number of pixels, assume at least 4 Byte alignment due to packing of 2 effective
            // pixels into one 32-Bit packet, maybe even 8 Byte alignment if divideable by 4. For other width's, assume
            // no alignment ie., 1 Byte:
            out_texture->textureByteAligned = (w % 2) ? 1 : ((w % 4) ? 4 : 8);
        }

        // YUV I420 planar pixel upload requested?
        if ((capdev->bitdepth <= 8) && (capdev->reqpixeldepth == 2) && (capdev->pixeldepth == 12)) {
            // We encode I420 planar data inside a 8 bit per pixel luminance texture of
            // 1.5x times the height of the video frame. First the "Y" luminance plane
            // is stored at full 1 sample per pixel resolution with 8 bits. Then a 0.25x
            // height slice with "U" Cr chrominance data at half the horizontal and vertical
            // resolution aka 1 sample per 2x2 pixel quad. Then a 0.25x height slice with "V"
            // Cb chrominance data at 1 sample per 2x2 pixel quad resolution. As such the texture
            // appears to OpenGL as a normal LUMINANCE8 texture. Conversion of the planar format
            // into useable RGBA8 pixel fragments will happen during rendering via a suitable fragment
            // shader. The net gain of this is that we effectively only need 1.5 Bytes per pixel instead
            // of 3 Bytes for RGB8 or 4 Bytes for RGBA8:
            out_texture->textureexternaltype   = GL_UNSIGNED_BYTE;
            out_texture->textureexternalformat = GL_LUMINANCE;
            out_texture->textureinternalformat = GL_LUMINANCE8;

            // Define a rect of 1.5 times the video frame height, so PsychCreateTexture() will source
            // the whole input data buffer:
            PsychMakeRect(out_texture->rect, 0, 0, w, h * 1.5);

            // Check if 1.5x height texture fits within hardware limits of this GPU:
            if (h * 1.5 > win->maxTextureSize) PsychErrorExitMsg(PsychError_user, "Videoframe size too big for this graphics card and pixelFormat! Please retry with a pixeldepth of 4 in 'OpenVideoCapture'.");

            // Byte alignment: Assume no alignment for now:
            out_texture->textureByteAligned = 1;

            // Create planar "I420 inside L8" texture:
            PsychCreateTexture(out_texture);

            // Restore rect and clientrect of texture to effective size of video frame:
            PsychMakeRect(out_texture->rect, 0, 0, w, h);
            PsychCopyRect(out_texture->clientrect, out_texture->rect);

            // Mark texture as planar encoded, so proper conversion shader gets applied during
            // call to PsychNormalizeTextureOrientation(), prior to any render-to-texture operation, e.g.,
            // if used as an offscreen window, or as a participant of a Screen('TransformTexture') call:
            out_texture->specialflags |= kPsychPlanarTexture;

            // Assign special filter shader for sampling and color-space conversion of the
            // planar texture during drawing or PsychNormalizeTextureOrientation():
            if (!PsychAssignPlanarI420TextureShader(out_texture, win)) PsychErrorExitMsg(PsychError_user, "Assignment of I420 video decoding shader failed during video texture creation!");

            // Number of effective channels is 3 for RGB8:
            out_texture->nrchannels = 3;

            // And 24 bpp depth:
            out_texture->depth = 24;
        }
        else if (capdev->bitdepth > 8) {
            // Is this a > 8 bpc image format? If not, we ain't nothing more to prepare.
            // If yes, we need to use a high precision floating point texture to represent
            // the > 8 bpc image payload without loss of image information:

            // highbitthreshold: If the net bpc value is greater than this, then use 32bpc floats
            // instead of 16 bpc half-floats, because 16 bpc would not be sufficient to represent
            // more than highbitthreshold bits faithfully:
            const int highbitthreshold = 11;

            // 9 - 16 bpc color/luminance resolution:
            out_texture->depth = capdev->reqpixeldepth * ((capdev->bitdepth > highbitthreshold) ? 32 : 16);
            out_texture->nrchannels = capdev->reqpixeldepth;

            // Byte alignment: Assume at least 2 Byte alignment due to 16 bit per component aka 2 Byte input:
            out_texture->textureByteAligned = 2;

            if (capdev->reqpixeldepth == 1) {
                // 1 layer Luminance:
                out_texture->textureinternalformat = (capdev->bitdepth > highbitthreshold) ? GL_LUMINANCE_FLOAT32_APPLE : GL_LUMINANCE_FLOAT16_APPLE;
                out_texture->textureexternalformat = GL_LUMINANCE;
                // Override for missing floating point texture support: Try to use 16 bit fixed point signed normalized textures [-1.0 ; 1.0] resolved at 15 bits:
                if (!(win->gfxcaps & kPsychGfxCapFPTex16)) out_texture->textureinternalformat = GL_LUMINANCE16_SNORM;
                out_texture->textureByteAligned = (w % 2) ? 2 : ((w % 4) ? 4 : 8);
            }
            else if (capdev->reqpixeldepth == 3) {
                // 3 layer RGB:
                out_texture->textureinternalformat = (capdev->bitdepth > highbitthreshold) ? GL_RGB_FLOAT32_APPLE : GL_RGB_FLOAT16_APPLE;
                out_texture->textureexternalformat = GL_RGB;
                // Override for missing floating point texture support: Try to use 16 bit fixed point signed normalized textures [-1.0 ; 1.0] resolved at 15 bits:
                if (!(win->gfxcaps & kPsychGfxCapFPTex16)) out_texture->textureinternalformat = GL_RGB16_SNORM;
                out_texture->textureByteAligned = (w % 2) ? 2 : ((w % 4) ? 4 : 8);
            }
            else {
                // 4 layer RGBA:
                out_texture->textureinternalformat = (capdev->bitdepth > highbitthreshold) ? GL_RGBA_FLOAT32_APPLE : GL_RGBA_FLOAT16_APPLE;
                out_texture->textureexternalformat = GL_RGBA;
                // Override for missing floating point texture support: Try to use 16 bit fixed point signed normalized textures [-1.0 ; 1.0] resolved at 15 bits:
                if (!(win->gfxcaps & kPsychGfxCapFPTex16)) out_texture->textureinternalformat = GL_RGBA16_SNORM;
                // Always 8 Byte aligned:
                out_texture->textureByteAligned = 8;
            }

            // External datatype is 16 bit unsigned integer, each color component encoded in a 16 bit value:
            out_texture->textureexternaltype = GL_UNSIGNED_SHORT;

            // Scale input data, so highest significant bit of payload is in bit 16:
            glPixelTransferi(GL_RED_SCALE,   1 << (16 - capdev->bitdepth));
            glPixelTransferi(GL_GREEN_SCALE, 1 << (16 - capdev->bitdepth));
            glPixelTransferi(GL_BLUE_SCALE,  1 << (16 - capdev->bitdepth));

            // Let PsychCreateTexture() do the rest of the job of creating, setting up and
            // filling an OpenGL texture with content:
            PsychCreateTexture(out_texture);

            // Undo scaling:
            glPixelTransferi(GL_RED_SCALE, 1);
            glPixelTransferi(GL_GREEN_SCALE, 1);
            glPixelTransferi(GL_BLUE_SCALE, 1);
        }
        else {
            // Simple case: Let PsychCreateTexture() do the rest of the job of creating, setting up and
            // filling an OpenGL texture with content:
            PsychCreateTexture(out_texture);
        }

        // This NULL-out is not strictly needed (done already in PsychCreateTexture()), just for simpler code review:
        out_texture->textureMemory = NULL;

        // Immediate conversion of texture into normalized orientation and format requested
        // by usercode?
        if (capdev->recordingflags & 2048) {
            // Transform out_texture video texture into a normalized, upright texture if it isn't already in
            // that format. We require this standard orientation for simplified shader design.
            PsychSetShader(win, 0);
            PsychNormalizeTextureOrientation(out_texture);
        }

        // Ready to use the texture...
    }

    // Sum of pixel intensities requested? 8 bpc?
    if (summed_intensity && (capdev->bitdepth <= 8)) {
        pixptr = (unsigned char*) input_image;
        count  = w * h * ((capdev->reqpixeldepth !=2) ? capdev->reqpixeldepth : 1);
        for (i=0; i<count; i++) intensity+=(psych_uint64) pixptr[i];
        *summed_intensity = ((double) intensity) / w / h / capdev->reqpixeldepth / 255;
    }

    // Sum of pixel intensities requested? 16 bpc?
    if (summed_intensity && (capdev->bitdepth > 8)) {
        pixptrs = (psych_uint16*) input_image;
        count = w * h * ((capdev->reqpixeldepth !=2) ? capdev->reqpixeldepth : 1);
        for (i=0; i<count; i++) intensity+=(psych_uint64) pixptrs[i];
        *summed_intensity = ((double) intensity) / w / h / capdev->reqpixeldepth / ((1 << (capdev->bitdepth)) - 1);
    }

    // Raw data requested?
    if (outrawbuffer) {
        // Copy it out:
        outrawbuffer->w = w;
        outrawbuffer->h = h;
        outrawbuffer->depth = (capdev->reqpixeldepth !=2) ? capdev->reqpixeldepth : 1;
        outrawbuffer->bitdepth = (capdev->bitdepth > 8) ? 16 : 8;
        count = (w * h * outrawbuffer->depth * (outrawbuffer->bitdepth / 8));
        // Either 8 bpc or 16 bpc data - A simple memcpy does the job efficiently:
        memcpy(outrawbuffer->data, (const void *) input_image, count);
    }

    // Release the capture buffer. Return it to the DMA ringbuffer pool:
    gst_buffer_unmap(videoBuffer, &mapinfo);
    gst_sample_unref(videoSample);
    videoBuffer = NULL;

    // Update total count of dropped (or pending) frames:
    capdev->nr_droppedframes += capdev->current_dropped;
    nrdropped = capdev->current_dropped;
    capdev->current_dropped = 0;

    // Timestamping:
    PsychGetAdjustedPrecisionTimerSeconds(&tend);

    // Increase counter of retrieved textures:
    capdev->nrgfxframes++;

    // Update average time spent in texture conversion:
    capdev->avg_gfxtime+= (tend - tstart);

    // We're successfully done! Return number of dropped (or pending in DMA ringbuffer) frames:
    return(nrdropped);
}

/* Set capture device specific parameters:
 * Currently, the named parameters are a subset of the parameters supported by the
 * IIDC specification, mapped to more convenient names.
 *
 * Input: pname = Name string to specify the parameter.
 *        value = Either DBL_MAX to not set but only query the parameter, or some other
 *                value, that we try to set in the Firewire camera.
 *
 * Returns: Old value of the setting
 */
double PsychGSVideoCaptureSetParameter(int capturehandle, const char* pname, double value)
{
    int intval, oldintval;
    float oldfvalue = FLT_MAX;
    double oldvalue = DBL_MAX; // Initialize return value to the "unknown/unsupported" default.
    psych_bool assigned = false;
    psych_bool present  = false;
    GstColorBalance* cb = NULL;
    GList* cl = NULL;
    GList* iter = NULL;
    GstColorBalanceChannel* cc = NULL;

    // Retrieve device record for handle, or NULL for special "all devices" handle -1:
    PsychVidcapRecordType* capdev = (capturehandle != -1) ? PsychGetGSVidcapRecord(capturehandle) : NULL;

    // Make sure GStreamer is ready:
    PsychGSCheckInit("videocapture");

    oldintval = 0xFFFFFFFF;

    // Round value to integer:
    intval = (int) (value + 0.5);

    if (strstr(pname, "SetNextCaptureBinSpec=")) {
        // Assign string with gst-launch style video capture bin description for use
        // with a deviceIndex of -9 in next Screen('OpenVideoCapture', -9, ...) call.
        // Instead of connecting to one of the special devices -8 to -1 or an enumerated
        // video source like the default source zero, or others, we parse the string assigned
        // here and create a GStreamer bin which acts as video source:
        // Find start of movie namestring and assign to pname:
        pname = strstr(pname, "=");
        pname++;

        // Assign to our gstlaunchbinsrc string:
        memset(gstlaunchbinsrc, 0, sizeof(gstlaunchbinsrc));
        snprintf(gstlaunchbinsrc, sizeof(gstlaunchbinsrc), "%s", pname);

        if (PsychPrefStateGet_Verbosity() > 2) {
            printf("PTB-INFO: Changed gst-launch style videosource bin string for use with deviceIndex -9 to '%s'.\n", gstlaunchbinsrc);
        }

        return(0);
    }

    // All other parameters require a valid capturehandle >= 0.
    if (capdev == NULL) return(DBL_MAX);

    // Set a new target movie name for video recordings:
    if (strstr(pname, "SetNewMoviename=")) {
        // Find start of movie namestring and assign to pname:
        pname = strstr(pname, "=");
        pname++;

        // Child protection:
        if (!capdev->recording_active) {
            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: Tried to change name of target movie file on device %i, but recording not enabled on that device! Ignored.\n", capturehandle);
            }
            return(-2);
        }

        // Can't reassign new codec without reopening the device:
        if (strstr(pname, ":CodecType")) {
            *(strstr(pname, ":CodecType")) = 0;
            if (PsychPrefStateGet_Verbosity() > 1) {
                printf("PTB-WARNING: Tried to change recording codec on device %i, but this isn't possible without reopening the device. Ignored.\n", capturehandle);
            }
        }

        // Release old movie name:
        if (capdev->targetmoviefilename) free(capdev->targetmoviefilename);
        capdev->targetmoviefilename = NULL;

        // Assign new movie name:
        capdev->targetmoviefilename = strdup(pname);

        if (PsychPrefStateGet_Verbosity() > 2) {
            printf("PTB-INFO: Changed name of movie file for recording on device %i to '%s'.\n", capturehandle, pname);
        }

        return(0);
    }

    // Check parameter name pname and call the appropriate subroutine:
    if (strcmp(pname, "TriggerCount")==0 || strcmp(pname, "WaitTriggerCount")==0) {
        // Query of cameras internal trigger counter or waiting for a specific
        // value in the counter requested. Trigger counters are special features,
        // (so called "Smart Features" or "Advanced Features" in the IIDC spec)
        // which are only available on selected cameras. They are not currently
        // available on GStreamer.
        return(-2);
    }

#if PSYCH_SYSTEM != PSYCH_WINDOWS

    // Load a 2D marker tracking plugin and initialize it:
    if (strstr(pname, "LoadMarkerTrackingPlugin=")) {
        // Find start of string and assign to pname:
        pname = strstr(pname, "=");
        pname++;

        // Load shared library which implements the marker tracker plugin from given filename:
        if ((markerTrackerPlugin_libraryhandle = dlopen(pname, RTLD_NOW | RTLD_LOCAL)) == NULL) {
            printf("PTB-ERROR: Failed to load markertracker plugin for device %i under name '%s'. OS reports: '%s'\n", capturehandle, pname, dlerror());
            PsychErrorExitMsg(PsychError_user, "Loading markertracker plugin failed!");
        }

        // Library loaded and linked. Get function handles for functions we do care about:
        TrackerPlugin_initialize = (void* (*)()) dlsym(markerTrackerPlugin_libraryhandle, "TrackerPlugin_initialize");

        TrackerPlugin_shutdown = (bool (*)(void*)) dlsym(markerTrackerPlugin_libraryhandle, "TrackerPlugin_shutdown");

        TrackerPlugin_processFrame = (bool (*)(void*, unsigned long* source_ptr, int imgwidth, int imgheight, int xmin, int ymin, unsigned int timeidx, double capturetimestamp, unsigned int absolute_frameindex)) dlsym(markerTrackerPlugin_libraryhandle, "TrackerPlugin_processFrame");

        TrackerPlugin_processPluginDataBuffer = (bool (*)(void*, unsigned long* buffer, int size)) dlsym(markerTrackerPlugin_libraryhandle, "TrackerPlugin_processPluginDataBuffer");

        // Binding successfull?
        if (!TrackerPlugin_initialize || !TrackerPlugin_shutdown || !TrackerPlugin_processFrame || !TrackerPlugin_processPluginDataBuffer) {
            dlclose(markerTrackerPlugin_libraryhandle);
            printf("PTB-ERROR: Failed to link/bind markertracker plugin for device %i under name '%s'. Could not resolve at least one entry point.\n", capturehandle, pname);
            PsychErrorExitMsg(PsychError_user, "Linking/Binding markertracker plugin failed!");
        }

        // Call init method. This will return a unique handle for this instance of the plugin for this capture device:
        if (NULL == (capdev->markerTrackerPlugin = (*TrackerPlugin_initialize)())) {
            dlclose(markerTrackerPlugin_libraryhandle);
            printf("PTB-ERROR: Failed to initialize markertracker plugin for device %i under name '%s'.\n", capturehandle, pname);
            PsychErrorExitMsg(PsychError_user, "Initializing markertracker plugin failed!");
        }

        // Get the sink pad from the videosink, where our to-be-processed video frames arrive on the streaming thread:
        GstPad *pad = gst_element_get_static_pad(capdev->videosink, "sink");

        // Add a pad probe callback PsychHaveVideoDataCallback(). This gets called on each received buffer
        // from the streaming thread. If a markertracker/data processing plugin is loaded, the PsychHaveVideoDataCallback()
        // will map the received video buffer and execute the plugin on it, otherwise the callback no-ops:
        gst_pad_add_probe(pad, GST_PAD_PROBE_TYPE_BUFFER, PsychHaveVideoDataCallback, capdev, NULL);
        gst_object_unref(pad);

        if (PsychPrefStateGet_Verbosity() > 2) {
            printf("PTB-INFO: Markertracker plugin loaded and initialized for device %i as '%s'.\n", capturehandle, pname);
        }

        return(0);
    }

#endif

    // Send command to  a 2D marker tracking plugin:
    if (strstr(pname, "SendCommandToMarkerTrackingPlugin=")) {
        unsigned long buffer[1024];

        // Find start of string and assign to pname:
        pname = strstr(pname, "=");
        pname++;

        if (capdev->markerTrackerPlugin) {
            // Yes! Execute: Plugin reads from (unsigned long*) input_image and could theoretically write
            // back into (unsigned char*) input_image to modify its content. In practice, it doesn't do that,
            // so this argument is provided as NULL-Ptr:
            //
            // Need funky data wrangling here, as plugin expects buffer of unsigned long's, and a size spec
            // in units of unsigned long's:
            if (strlen(pname) + 1 > sizeof(buffer)) PsychErrorExitMsg(PsychError_user, "Tried to send too much data to a markertracker plugin for this capture device!");
            memcpy(&(buffer[0]), pname, strlen(pname) + 1);

            if (!(*TrackerPlugin_processPluginDataBuffer)(capdev->markerTrackerPlugin, &(buffer[0]), (int) (strlen(pname) / sizeof(unsigned long)) + 1 )) {
                printf("PTB-ERROR: SendCommandToMarkerTrackingPlugin: Failed to send command to markertracker plugin for device %i! Command was '%s'.\n", capturehandle, pname);
                PsychErrorExitMsg(PsychError_user, "Failed to send data to markertracker plugin for this capture device!");
            }
        }
        else {
            // No plugin loaded?!?
            printf("PTB-ERROR: SendCommandToMarkerTrackingPlugin: Tried to send command to non-existent markertracker plugin for device %i! Command was '%s'.\n", capturehandle, pname);
            PsychErrorExitMsg(PsychError_user, "Tried to send data to a markertracker plugin for this capture device, but there isn't any plugin loaded!");
        }

        return(0);
    }

    // Check if GstColorBalanceInterface is supported and assign it for use downstream. Probe
    // different providers: camerabin1 (should support it), camerabin2 (doesn't at this point in time),
    // the wrappercamerabinsrc of camerabin2 (doesn't at this point in time), the video source attached
    // to the wrappercamerabinsrc (currently the video4linux2 source does support it):
    cb = NULL;
    // Probe camerabin1 / camerabin2:
    if (!cb && GST_IS_COLOR_BALANCE(capdev->camera)) cb = GST_COLOR_BALANCE(capdev->camera);

    // If fail, probe wrappercamerabinsrc:
    if (!cb && GST_IS_COLOR_BALANCE(capdev->videowrappersrc)) cb = GST_COLOR_BALANCE(capdev->videowrappersrc);

    // If fail, probe videosource itself:
    if (!cb && GST_IS_COLOR_BALANCE(capdev->videosource)) cb = GST_COLOR_BALANCE(capdev->videosource);

    // If fail, game over:
    if (!cb && (PsychPrefStateGet_Verbosity() > 3)) printf("PTB-WARNING: Video device %i does not suppport GstColorBalance interface as expected.\n", capturehandle);

    if (strcmp(pname, "PrintParameters")==0) {
        // Special command: List and print all features...
        printf("PTB-INFO: The video source provides the following controllable parameters:\n");
        printf("PTB-INFO: ----------------------------------------------------------------\n\n");
        printf("PTB-INFO: Optional parameters - may or may not be supported:\n");
        printf("PTB-INFO: Shutter, Aperture, EVCompensation, Flickermode, Whitebalancemode,\n");
        printf("PTB-INFO: Flashmode, Scenemode, Focusmode\n\n");
        printf("PTB-INFO: These are definitely supported by the connected camera:\n");


        if (cb) {
            // Enumerate all color balance channels:
            cl = (GList*) gst_color_balance_list_channels(cb);
            for (iter = g_list_first(cl); iter != NULL ; iter = g_list_next(iter)) {
                cc = (GstColorBalanceChannel*) iter->data;
                printf("PTB-INFO: '%s'\t\t min=%i\t : max=%i\t : current=%i\n", (char*) cc->label,
                (int) cc->min_value, (int) cc->max_value, (int) gst_color_balance_get_value(cb, cc));
            }
        }
        printf("PTB-INFO: ----------------------------------------------------------------\n\n");
        return(0);
    }

    // Return current framerate:
    if (strcmp(pname, "GetFramerate")==0) {
        PsychCopyOutDoubleArg(1, FALSE, capdev->fps);
        return(0);
    }

    // Return current ROI of camera, as requested (and potentially modified during
    // PsychOpenCaptureDevice(). This is a read-only parameter, as the ROI can
    // only be set during Screen('OpenVideoCapture').
    if (strcmp(pname, "GetROI")==0) {
        PsychCopyOutRectArg(1, FALSE, capdev->roirect);
        return(0);
    }

    // Return vendor name string:
    if (strcmp(pname, "GetVendorname")==0) {
        PsychCopyOutCharArg(1, FALSE, "Unknown Vendor");
        return(0);
    }

    // Return model name string:
    if (strcmp(pname, "GetModelname")==0) {
        PsychCopyOutCharArg(1, FALSE, (capdev->cameraFriendlyName) ? capdev->cameraFriendlyName : "Unknown Model");
        return(0);
    }

    if (strstr(pname, "Gain") && strstr(&capdev->videosourcename[0], "aravissrc")) {
        // Special case aravissrc plugin?

        // Query old "gain" setting, which is integer and in units of dB Dezibel:
        g_object_get(capdev->videosource, "gain", &oldintval, NULL);
        oldvalue = (double) oldintval;

        // Reset to auto-mode, if requested:
        if (strstr(pname, "Auto")) {
            value = DBL_MAX;
            g_object_set(capdev->videosource, "gain-auto", TRUE, NULL);
        }

        // Optionally set new setting and switch to manual mode:
        if (value != DBL_MAX) {
            g_object_set(capdev->videosource, "gain-auto", FALSE, NULL);
            g_object_set(capdev->videosource, "gain", (int) (value + 0.5), NULL);
        }
        return(oldvalue);
    }

    if (strstr(&capdev->videosourcename[0], "aravissrc")) {
        // aravissrc plugin supports the following additional parameters:
        if (strstr(pname, "offset-x") || strstr(pname, "offset-y") || strstr(pname, "h-binning") || strstr(pname, "v-binning")) {
            // Query old setting, which is an integer:
            g_object_get(capdev->videosource, pname, &oldintval, NULL);
            oldvalue = (double) oldintval;

            // Optionally set new setting:
            if (value != DBL_MAX) {
                g_object_set(capdev->videosource, pname, (int) (value + 0.5), NULL);
            }
            return(oldvalue);
        }
    }

    if (strstr(pname, "Shutter")!=0) {
        // Special case aravissrc plugin?
        if (strstr(&capdev->videosourcename[0], "aravissrc")) {
            // exposure is a double value, expressing exposure time in microseconds (1e6):
            g_object_get(capdev->videosource, "exposure", &oldvalue, NULL);
            oldvalue = oldvalue / 1e6;

            // Reset to auto-mode, if requested:
            if (strstr(pname, "Auto")) {
                value = DBL_MAX;
                g_object_set(capdev->videosource, "exposure-auto", TRUE, NULL);
            }

            // Optionally set new setting:
            if (value != DBL_MAX) {
                g_object_set(capdev->videosource, "exposure-auto", FALSE, NULL);
                g_object_set(capdev->videosource, "exposure", (double) (value * 1e6), NULL);
            }
            return(oldvalue);
        }

        // Standard case: Exposure is an integer in nanoseconds (1e9):

        // Query old "exposure" setting, which is duration of shutter open:
        g_object_get(capdev->camera, "exposure", &oldintval, NULL);
        oldvalue = (double) oldintval / 1e9;

        // Reset to auto-mode, if requested:
        if (strstr(pname, "Auto")) value = 0;

        // Optionally set new setting:
        if (value != DBL_MAX) g_object_set(capdev->camera, "exposure", (int) (value * 1e9), NULL);
        return(oldvalue);
    }

    if (strstr(pname, "Aperture")) {
        // Query old "aperture" setting, which is the amount of lens opening:
        g_object_get(capdev->camera, "aperture", &oldintval, NULL);
        oldvalue = (double) oldintval;

        // Optionally set new setting:
        if (intval < 0) intval = 0;
        if (intval > 255) intval = 255;

        // Reset to auto-mode, if requested:
        if (strstr(pname, "Auto")) { intval = 0; value = 0; }

        if (value != DBL_MAX) g_object_set(capdev->camera, "aperture", intval, NULL);
        return(oldvalue);
    }

    if (strstr(pname, "EVCompensation")!=0) {
        // Query old "ev-compensation" setting:
        g_object_get(capdev->camera, "ev-compensation", &oldfvalue, NULL);
        oldvalue = (double) oldfvalue;

        // Reset to auto-mode, if requested:
        if (strstr(pname, "Auto")) value = 0;

        // Optionally set new setting:
        if (value != DBL_MAX) g_object_set(capdev->camera, "ev-compensation", (float) value, NULL);
        return(oldvalue);
    }

    if (strstr(pname, "Flickermode")) {
        // Query old "flicker-mode" setting:
        g_object_get(capdev->camera, "flicker-mode", &oldintval, NULL);
        oldvalue = (double) oldintval;

        // Optionally set new setting:
        if (intval < 0) intval = 0;
        if (intval > 3) intval = 3;

        // Reset to auto-mode, if requested:
        if (strstr(pname, "Auto")) { intval = 3; value = 0; }

        if (value != DBL_MAX) g_object_set(capdev->camera, "flicker-mode", intval, NULL);
        return(oldvalue);
    }

    if (strstr(pname, "Whitebalancemode")) {
        // Query old "white-balance-mode" setting:
        g_object_get(capdev->camera, "white-balance-mode", &oldintval, NULL);
        oldvalue = (double) oldintval;

        // Optionally set new setting:
        if (intval < 0) intval = 0;
        if (intval > 5) intval = 5;

        // Reset to auto-mode, if requested:
        if (strstr(pname, "Auto")) { intval = 0; value = 0; }

        if (value != DBL_MAX) g_object_set(capdev->camera, "white-balance-mode", intval, NULL);
        return(oldvalue);
    }

    if (strstr(pname, "Focusmode")) {
        // Query old "focus-mode" setting:
        g_object_get(capdev->camera, "focus-mode", &oldintval, NULL);
        oldvalue = (double) oldintval;

        // Optionally set new setting:
        if (intval < 0) intval = 0;
        if (intval > 7) intval = 7;

        // Reset to auto-mode, if requested:
        if (strstr(pname, "Auto")) { intval = 0; value = 0; }

        if (value != DBL_MAX) g_object_set(capdev->camera, "focus-mode", intval, NULL);
        return(oldvalue);
    }

    if (strstr(pname, "Flashmode")) {
        // Query old "flash-mode" setting:
        g_object_get(capdev->camera, "flash-mode", &oldintval, NULL);
        oldvalue = (double) oldintval;


        // Optionally set new setting:
        if (intval < 0) intval = 0;
        if (intval > 4) intval = 4;

        // Reset to auto-mode, if requested:
        if (strstr(pname, "Auto")) { intval = 0; value = 0; }

        if (value != DBL_MAX) g_object_set(capdev->camera, "flash-mode", intval, NULL);
        return(oldvalue);
    }

    if (strstr(pname, "Scenemode")) {
        // Query old "scene-mode" setting:
        g_object_get(capdev->camera, "scene-mode", &oldintval, NULL);
        oldvalue = (double) oldintval;

        // Optionally set new setting:
        if (intval < 0) intval = 0;
        if (intval > 6) intval = 6;

        // Reset to auto-mode, if requested:
        if (strstr(pname, "Auto")) { intval = 6; value = 0; }

        if (value != DBL_MAX) g_object_set(capdev->camera, "scene-mode", intval, NULL);
        return(oldvalue);
    }

    // Not yet matched? Try if it matches one of the color channel properties
    // from the color balance interface.
    if (cb) {
        // Search all color balance channels:
        cl = (GList*) gst_color_balance_list_channels(cb);
        for (iter = g_list_first(cl); iter != NULL ; iter = g_list_next(iter)) {
            cc = (GstColorBalanceChannel*) iter->data;

            // Match?
            if (strcmp((const char*) cc->label, (const char*) pname) == 0) {
                assigned = TRUE;

                // Query and return old setting:
                oldvalue = (double) gst_color_balance_get_value(cb, cc);

                // Optionally assign new setting:
                if (intval < cc->min_value) intval = cc->min_value;
                if (intval > cc->max_value) intval = cc->max_value;
                if (value != DBL_MAX) gst_color_balance_set_value(cb, cc, intval);
            }
        }

        if (!assigned) {
            if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Screen('SetVideoCaptureParameter', ...) called with unknown parameter %s. Ignored...\n", pname);
        }

        return(oldvalue);
    }

    return(DBL_MAX);
}

#endif
#endif
