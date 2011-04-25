/*
	PsychSourceGL/Source/Common/Screen/PsychVideoCaptureSupportGStreamer.c
 
	PLATFORMS:	
	
	GNU/Linux, Apple MacOS/X and MS-Windows.
 
	AUTHORS:
	
	Mario Kleiner           mk              mario.kleiner@tuebingen.mpg.de
 
	HISTORY:
	
	9.01.2010				Created initial version.
	8.04.2010                               Make video/audio recording work ok.

	DESCRIPTION:
	
	This is the videocapture engine based on the free software (LGPL'ed)
	GStreamer multimedia framework. It supports video capture, sound capture and
	recording of video and/or sound.
	 
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

	=> Most functionality for typical everyday tasks works perfect or reasonably well.
        => Some issues for special case apps persist, as written below.

	TODO:

	The following problems/limitations exist, which need to be fixed asap:

        * Some codecs (e.g., huffyuv) don't work yet. Some others show low quality or
          performance. Need to optimize parameters. Need to provide way to change codec
          settings. Only the MPEG-4 video codecs show decent performance at 640x480@60fps.

        * Provide interface to change camera settings like exposure time etc.

 */

#ifdef PTB_USE_GSTREAMER

#include "Screen.h"
#include <float.h>
#include "PsychVideoCaptureSupport.h"

// These are the includes for GStreamer:
#include <glib.h>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/interfaces/propertyprobe.h>

static psych_bool usecamerabin = TRUE;

#define PSYCH_MAX_VIDSRC    256
PsychVideosourceRecordType *devices = NULL;
int ntotal = 0;

// Record which defines all state for a capture device:
typedef struct {
	int valid;                        // Is this a valid device record? zero == Invalid.
	psych_mutex mutex;
	psych_condition condition;
	int frameAvail;                   // Number of frames in videosink.
	int preRollAvail;
	GstElement *camera;               // Ptr to a GStreamer camera object that holds the internal state for such cams.
	GMainLoop *VideoContext;          // Message bus context for delivery of status/error/warning messages by GStreamer.
	GstElement *videosink;            // Our appsink for retrieval of live video feeds --> PTB texture conversion.
	GstElement *videosource;          // The videosource, encapsulating the actual video capture device.
	GstElement *videoenc;             // Video encoder for video recording.
	GstElement *videorate_filter;     // Video framerate converter to guarantee constant framerate and a/v sync for recording.
	GstClockTime lastSavedBaseTime;   // Last time the pipeline was put into PLAYING state. Saved at StopCapture time before stop.
	int nrAudioTracks;
	int nrVideoTracks;
	int dropframes;			  // 1 == Always deliver most recent frame in FIFO, even if dropping of frames is neccessary.
	unsigned char* scratchbuffer;     // Scratch buffer for YUV->RGB conversion.
	int reqpixeldepth;                // Requested depth of single pixel in output texture.
	int pixeldepth;                   // Depth of single pixel from grabber in bits.
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
	double avg_decompresstime;        // Average time spent in Quicktime/Sequence Grabber decompressor.
	double avg_gfxtime;               // Average time spent in GWorld --> OpenGL texture conversion and statistics.
	int nrgfxframes;                  // Count of fetched textures.
	char* targetmoviefilename;        // Filename of a movie file to record.
	char* cameraFriendlyName;         // Camera friendly device name.
} PsychVidcapRecordType;

static PsychVidcapRecordType vidcapRecordBANK[PSYCH_MAX_CAPTUREDEVICES];
static int numCaptureRecords = 0;
static psych_bool gs_firsttime = TRUE;
double gs_startupTime = 0.0;

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
	
	if (numCaptureRecords >= PSYCH_MAX_CAPTUREDEVICES) {
		PsychErrorExitMsg(PsychError_user, "Invalid deviceIndex for video capture device passed. Index exceeds number of registered devices!");
	}
	
	if (!vidcapRecordBANK[deviceIndex].valid) {
		PsychErrorExitMsg(PsychError_user, "Invalid deviceIndex for video capture device passed. No such device open!");
	}
	
	// Ok, we have a valid device record, return a ptr to it:
	return(&vidcapRecordBANK[deviceIndex]);
}

/* Internal: Check if GStreamer is already initialized. Initialize it,
 * if neccessary.
 */
void PsychGSCheckInit(const char* engineName)
{
	GError			*error = NULL;

	if (gs_firsttime) {
		// First time invocation:
        #if PSYCH_SYSTEM == PSYCH_WINDOWS
        // On Windows, we need to delay-load the GStreamer DLL's. This loading
        // and linking will automatically happen downstream. However, if delay loading
        // would fail, we would end up with a crash! For that reason, we try here to
        // load the DLL, just to probe if the real load/link/bind op later on will
        // likely succeed. If the following LoadLibrary() call fails and returns NULL,
        // then we know we would end up crashing. Therefore we'll output some helpful
        // error-message instead:
        if ((NULL == LoadLibrary("libgstreamer-0.10.dll")) || (NULL == LoadLibrary("libgstapp-0.10.dll"))) {
            // Failed:
            printf("\n\nPTB-ERROR: Tried to startup GStreamer. This didn't work,\n");
            printf("PTB-ERROR: because one of the required GStreamer DLL libraries failed to load. Probably because they\n");
            printf("PTB-ERROR: could not be found, could not be accessed (e.g., due to permission problems),\n");
            printf("PTB-ERROR: or they aren't installed on this machine at all.\n\n");
            printf("PTB-ERROR: Please read the online help by typing 'help GStreamer' for troubleshooting\nand installation instructions.\n\n");
			printf("PTB-ERROR: Due to failed GStreamer initialization, the %s engine is out of order.\n", engineName);
			PsychErrorExitMsg(PsychError_user, "GStreamer initialization failed due to DLL loading problems. Aborted.");
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
		g_object_set(G_OBJECT(gst_system_clock_obtain()), "clock-type", ((PSYCH_SYSTEM == PSYCH_LINUX) ? GST_CLOCK_TYPE_REALTIME : GST_CLOCK_TYPE_MONOTONIC), NULL);

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


/* Perform one context loop iteration (for bus message handling) if doWait == false,
 * or two seconds worth of iterations if doWait == true. This drives the message-bus
 * callback, so needs to be performed to get any error reporting etc.
 */
int PsychGSProcessVideoContext(GMainLoop *loop, psych_bool doWait)
{
	double tdeadline, tnow;
	PsychGetAdjustedPrecisionTimerSeconds(&tdeadline);
	tnow = tdeadline;
	tdeadline+=2.0;

	if (NULL == loop) return(0);

	while (doWait && (tnow < tdeadline)) {
		// Perform non-blocking work iteration:
		if (!g_main_context_iteration(g_main_loop_get_context(loop), false)) PsychYieldIntervalSeconds(0.010);

		// Update time:
		PsychGetAdjustedPrecisionTimerSeconds(&tnow);
	}

	// Perform one more work iteration of the event context, but don't block:
	return(g_main_context_iteration(g_main_loop_get_context(loop), false));
}

/* Initiate pipeline state changes: Startup, Preroll, Playback, Pause, Standby, Shutdown. */
static psych_bool PsychVideoPipelineSetState(GstElement* camera, GstState state, double timeoutSecs)
{
    GstState			state_pending;
    GstStateChangeReturn	rcstate;

    gst_element_set_state(camera, state);

    // Non-Blocking, async?
    if (timeoutSecs < 0) return(TRUE);
 
    // Wait for up to timeoutSecs for state change to complete or fail:
    rcstate = gst_element_get_state(camera, &state, &state_pending, (GstClockTime) (timeoutSecs * 1e9));
    switch(rcstate) {
	case GST_STATE_CHANGE_SUCCESS:
		//printf("PTB-DEBUG: Statechange completed with GST_STATE_CHANGE_SUCCESS.\n");
	break;

	case GST_STATE_CHANGE_ASYNC:
		printf("PTB-INFO: Statechange in progress with GST_STATE_CHANGE_ASYNC.\n");
	break;

	case GST_STATE_CHANGE_NO_PREROLL:
		//printf("PTB-INFO: Statechange completed with GST_STATE_CHANGE_NO_PREROLL.\n");
	break;

	case GST_STATE_CHANGE_FAILURE:
		printf("PTB-ERROR: Statechange failed with GST_STATE_CHANGE_FAILURE!\n");
		return(FALSE);
	break;

	default:
		printf("PTB-ERROR: Unknown state-change result in preroll.\n");
		return(FALSE);
    }

    return(TRUE);
}

/* Receive messages from the playback pipeline message bus and handle them: */
static gboolean PsychVideoBusCallback(GstBus *bus, GstMessage *msg, gpointer dataptr)
{
  PsychVidcapRecordType* capdev = (PsychVidcapRecordType*) dataptr;

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
	if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Videobus: Message EOS received.\n"); fflush(NULL);
    break;

    case GST_MESSAGE_WARNING: {
      gchar  *debug;
      GError *error;

      gst_message_parse_warning(msg, &error, &debug);
      if (PsychPrefStateGet_Verbosity() > 1) { 
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
		      printf("           This means that there was some problem with opening the video device (permissions etc.).\n\n");
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
	PsychUnlockMutex(&capdev->mutex);

	return;
}

static void PsychProbeBufferProps(GstBuffer *videoBuffer, int *w, int *h, double *fps)
{
	GstCaps                *caps;
	GstStructure	       *str;
	gint		       rate1, rate2;
	rate1 = rate2 = 0;
	str = NULL;
    
	if (videoBuffer) {
		caps = gst_buffer_get_caps(videoBuffer);
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
		if (caps) gst_caps_unref(caps);
	}

	return;
}

/* Called whenever pipeline goes into pause mode.
 * Signals/handles arrival of preroll buffers. Used to detect/signal when
 * new videobuffers are available in non-playback mode, when device becomes ready.
 */
static GstFlowReturn PsychNewPrerollCallback(GstAppSink *sink, gpointer user_data)
{
	GstBuffer              *videoBuffer;

	PsychVidcapRecordType* capdev = (PsychVidcapRecordType*) user_data;

	PsychLockMutex(&capdev->mutex);
	videoBuffer = gst_app_sink_pull_preroll(GST_APP_SINK(capdev->videosink));
	if (videoBuffer) {
		PsychProbeBufferProps(videoBuffer, NULL, NULL, &capdev->fps);
	}

	if (PsychPrefStateGet_Verbosity() > 5) {
		printf("PTB-DEBUG: New PrerollBuffer received. fps = %f\n", capdev->fps);
		fflush(NULL);
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
	PsychUnlockMutex(&capdev->mutex);
	PsychSignalCondition(&capdev->condition);

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
    PsychNewBufferListCallback
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
		PsychGSVideoCaptureRate(capturehandle, 0, 0, NULL);
	
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
	
	// Was our videosink (aka appsink) detached during device operation,
	// because videorecording was active but live feedback disabled, i.e.,
	// a fakesink was attached to camerabin? If so then camerabin didn't
	// own our videosink at destruction time and didn't auto-delete it.
	// We need to manually delete our orphaned videosink:
	if ((capdev->recording_active) && (capdev->recordingflags & 4) && (capdev->videosink)) {
		gst_object_unref(GST_OBJECT(capdev->videosink));
	}
	capdev->videosink = NULL;

	if (capdev->targetmoviefilename) free(capdev->targetmoviefilename);
	capdev->targetmoviefilename = NULL;

	if (capdev->cameraFriendlyName) free(capdev->cameraFriendlyName);
	capdev->cameraFriendlyName = NULL;

	// Invalidate device record to free up this slot in the array:
	capdev->valid = 0;
    
	// Decrease counter of open capture devices:
	if (numCaptureRecords>0) numCaptureRecords--;
	
	// Done.
	return;
}

/* Helper routine for PsychGSEnumerateVideoSources()
 *
 * Probes video source plugin 'srcname' [with class index 'classIndex' and
 * name 'className', using the property 'devHandlePropName' and probe strategy
 * as given by 'flags'. Adds all detected video input devices for that plugin
 * to the global 'devices' array and increases ntotal count accordingly.
 */
void PsychGSEnumerateVideoSourceType(const char* srcname, int classIndex, const char* className, const char* devHandlePropName, unsigned int flags)
{
	int					i, n;
	char				port_str[64];
	char				class_str[64];
	int					inputIndex;
	GstElement			*videosource = NULL;
	GstPropertyProbe	*probe = NULL;
	GValueArray			*viddevs = NULL;
	GValue				*dev = NULL;
	char				*device_name = NULL;
	gchar               *pstring = NULL;
	psych_uint64		deviceURI = 0;
	
	// Info to the user:
	if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to probe %s as video source...\n", srcname);
	
	// Create video source plugin, define class name:
	videosource = gst_element_factory_make(srcname, "ptb_videosource");
	sprintf(class_str, "%s", className);

	// Nothing to do if no such video plugin available:
	if (!videosource) return;
	
	// Available: Generate property probe for videosource:
	// Make sure videosource implements property probe interface to avoid useless warning clutter,
	// unless on Linux, where this query actually causes an assertion on some GStreamer versions...
	if ((PSYCH_SYSTEM == PSYCH_LINUX) || gst_element_implements_interface(videosource, GST_TYPE_PROPERTY_PROBE)) {
		probe = GST_PROPERTY_PROBE(videosource);
	}
	else {
		probe = NULL;
	}
	
	if (probe) {
		if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Probing '%s' property...\n", devHandlePropName);
		viddevs = gst_property_probe_probe_and_get_values_name(probe, devHandlePropName);

		if (viddevs) {
			// Assign count of available devices:
			n = viddevs->n_values;
			
			// Iterate all available devices:
			for(i = 0; i < n; i++) {
				// Get device handle:
				dev = g_value_array_get_nth(viddevs, i);
				if (G_VALUE_HOLDS_STRING(dev)) {
					sprintf(port_str, "%s", (const char*) g_value_get_string(dev));

					// Select it for further probing:
					g_object_set(G_OBJECT(videosource), devHandlePropName, port_str, NULL);
				}

				if (G_VALUE_HOLDS_UINT64(dev)) {
					deviceURI = g_value_get_uint64(dev);

					// Select it for further probing:
					g_object_set(G_OBJECT(videosource), devHandlePropName, deviceURI, NULL);
					sprintf(port_str, "%llu", deviceURI);
				}

				inputIndex = i;
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

				pstring = NULL;
				
				// Probe next device...
				ntotal++;
				
				if (ntotal >= PSYCH_MAX_VIDSRC) {
					printf("PTB-WARNING: Maximum number of allowable video sourced during enumeration %i exceeded! Aborting enumeration.\n", PSYCH_MAX_VIDSRC);
					break;
				}
			}

			g_value_array_free(viddevs);
		}
		else {
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Video plugin '%s' doesn't provide any video devices to enumerate.\n", class_str);
		}
	}
	else {
		if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Video plugin '%s' doesn't support probing. No video devices to enumerate for this plugin.\n", class_str);
	}

	// Release videosource:
	gst_element_set_state(videosource, GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(videosource));
	
	return;
}

/* PsychGSEnumerateVideoSources(int outPos, int deviceIndex);  -- Internal.
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
 * If deviceIndex < 0 : Returns NULL to caller, returns a struct array to runtime
 *                      environment return argument position 'outPos' with all info
 *                      about the detected sources.
 */
PsychVideosourceRecordType* PsychGSEnumerateVideoSources(int outPos, int deviceIndex)
{
	PsychGenericScriptType 	*devs;
	const char *FieldNames[]={"DeviceIndex", "ClassIndex", "InputIndex", "ClassName", "InputHandle", "Device", "DevicePath", "DeviceName", "GUID", "DevicePlugin", "DeviceSelectorProperty" };

	int					i, n;
	char				port_str[64];
	char				class_str[64];
	int					inputIndex;
	GstElement			*videosource = NULL;
	GstPropertyProbe	*probe = NULL;
	GValueArray			*viddevs = NULL;
	GValue				*dev = NULL;
	char				*device_name = NULL;
	gchar               *pstring = NULL;
	PsychVideosourceRecordType *mydevice = NULL;
	
	// Make sure GStreamer is ready:
	PsychGSCheckInit("videocapture");

	// Allocate temporary space for enumerated devices:
	devices = (PsychVideosourceRecordType*) PsychCallocTemp(PSYCH_MAX_VIDSRC, sizeof(PsychVideosourceRecordType));
	ntotal  = 0;

	// Linux specific setup path:
	if (PSYCH_SYSTEM == PSYCH_LINUX) {
		// Try Video4Linux-II camera source:
		PsychGSEnumerateVideoSourceType("v4l2camsrc", 1, "Video4Linux2-CameraSource", "device", 0);

		// Try standard Video4Linux-II source:
		PsychGSEnumerateVideoSourceType("v4l2src", 2, "Video4Linux2", "device", 0);
	}

	if (PSYCH_SYSTEM == PSYCH_WINDOWS) {
		// Try Windows kernel streaming source:
		PsychGSEnumerateVideoSourceType("ksvideosrc", 1, "Windows WDM kernel streaming", "device-name", 0);

		// Use DirectShow to probe:
		PsychGSEnumerateVideoSourceType("dshowvideosrc", 2, "DirectShow", "device-name", 0);
	}
	
	if (PSYCH_SYSTEM == PSYCH_OSX) {
		// Try OSX Quicktime sequence grabber video source:
		PsychGSEnumerateVideoSourceType("osxvideosrc", 1, "OSXQuicktimeSequenceGrabber", "device", 0);
	}
	
	// Try DV-Cameras:
	PsychGSEnumerateVideoSourceType("dv1394src", 5, "DV1394", "guid", 1);
	
	// Try HDV-Cameras:
	PsychGSEnumerateVideoSourceType("hdv1394src", 6, "HDV1394", "guid", 1);
	
	// Try IIDC-1394 Cameras:
	// Does not work, no property probe interface: PsychGSEnumerateVideoSourceType("dc1394src", 7, "1394-IIDC", "camera-number", 1);

	if (ntotal <= 0) {
		if (PsychPrefStateGet_Verbosity() > 4) {
            printf("PTB-INFO: Could not detect any supported video devices on this system.\n");
            printf("PTB-INFO: Trying to fake an auto-detected default device...\n");
        }
        
        // Create a fake entry for the autovideosrc:
        devices[0].deviceIndex = 0;
        sprintf(devices[0].deviceVideoPlugin, "autovideosrc");
        sprintf(devices[0].deviceSelectorProperty, "");
        sprintf(devices[0].deviceHandle, "");
        ntotal= 1;
        return(&devices[0]);
	}

    // Add fake entry for deviceIndex zero, as a copy of the first real entry:
    memcpy(&devices[ntotal], &devices[0], sizeof(PsychVideosourceRecordType));
    devices[ntotal].deviceIndex = 0;
    ntotal++;
    
	// Have enumerated devices:
	if (deviceIndex >= 0) {
		// Yes: Return device name for that index:
		for (i=0; i < ntotal; i++) {
			if (devices[i].deviceIndex == deviceIndex) {
				mydevice = &devices[i];
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

	// Done. Return device struct if assigned:
	return(mydevice);
}

psych_bool PsychGSGetResolutionAndFPSForSpec(PsychVidcapRecordType *capdev, int* width, int* height, double* fps, int reqdepth)
{
	GstCaps                 *caps = NULL;
	GstStructure		*str;
	gint			qwidth, qheight;
	gint                    qbpp;
	gint			rate1 = 0, rate2 = 1;
	gint			twidth = -1, theight = -1;
	gint                    maxpixelarea = -1;
	double                  tfps = 0.0;
	int			i;

	if (!usecamerabin) {
		// No camerabin, no way to query this stuff. Just fail
		printf("PTB-BUG: PsychGSGetResolutionAndFPSForSpec() called while !usecamerabin !?! Returning failure.\n");
		return(FALSE);
	}

	// Camerabin, we can actually enumerate and match.

	// Query caps of videosource and extract supported video capture modes:
	g_object_get(G_OBJECT(capdev->camera), "video-source-caps", &caps, NULL);

	if (caps) {
		if (PsychPrefStateGet_Verbosity() > 4)
			printf("PTB-DEBUG: Videosource caps are: %" GST_PTR_FORMAT "\n\n", caps);

		// Iterate through all supported video capture modes:
		for (i = 0; i < gst_caps_get_size(caps); i++) {
			str = gst_caps_get_structure(caps, i);

			gst_structure_get_int(str, "width", &qwidth);
			gst_structure_get_int(str, "height", &qheight);
			gst_structure_get_int(str, "bpp", &qbpp);
			gst_structure_get_fraction(str, "framerate", &rate1, &rate2);
			// if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Videosource cap %i: w = %i h = %i fps = %f\n", i, qwidth, qheight, (float) rate1 / (float) rate2);
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Videosource cap %i: w = %i h = %i.\n", i, qwidth, qheight);

			// Check for matching pixel color resolution, reject too low modes:
			// TODO: Not yet implemented, don't know if it makes sense at all, as the
			// colorspace converters can always take care of this.

			// Is this detection of default resolution, or validation of a
			// given resolution?
			if ((*width == -1) && (*height == -1)) {
				// Auto-Detection of optimal default resolution.
				// Need to find the one with highest resolution (== pixel area):
				if (qwidth * qheight > maxpixelarea) {
					// A new favorite with max pixel area:
					maxpixelarea = qwidth * qheight;
					twidth = qwidth;
					theight = qheight;
					tfps = (double) rate1 / (double) rate2;
				}
			}
			else {
				// Validation: Reject/Skip modes which don't support requested resolution.
				if ((*width != (int) qwidth) || (*height != (int) qheight)) continue;

				// Videomode which supports this resolution. Framerate validation?
				if (*fps != -1) {
					// TODO FIXME.
				}

				// Acceptable mode for requested resolution and framerate. Set it:
				maxpixelarea = qwidth * qheight;
			        twidth = qwidth;
				theight = qheight;
				tfps = (double) rate1 / (double) rate2;
			}
		}

		gst_caps_unref(caps);

		// Any matching mode found?
		if (twidth == -1) {
			// No! The requested resolution + fps + pixelformat combo
			// is not supported:
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Could not validate video source resolution %i x %i. Returning failure.\n", *width, *height);
			return(FALSE);
		}

		// Yes. Return settings:
		*fps = tfps;
		*width = twidth;
		*height = theight;

		if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Will use auto-detected or validated video source resolution %i x %i.\n", *width, *height);

		return(TRUE);
	} else {
		if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: PsychGSGetResolutionAndFPSForSpec(): Capability query to video source failed! Returning failure.\n");
		return(FALSE);
	}
}

// Parse codecSpec string, check for certain element specs. If found, parse them
// and create corresponding GstElement*, otherwise return NULL.
GstElement* CreateGStreamerElementFromString(const char* codecSpec, const char* typeSpec)
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
		// contains the GStreamer syntax description of the video codec and
		// its parameters.

		// Parse it and create a corresponding bin for use as encoder element:
		// Set GError* to NULL: Real men don't do error handling.
		element = gst_parse_bin_from_description((const gchar *) codecPipelineSpec, TRUE, NULL);
		if (element == NULL) {
			// Oopsie:
			printf("PTB-WARNING: Failed to create a encoder element of type '%s' from the following passed parameter string:\n", typeSpec);
			printf("PTB-WARNING: %s\n", codecPipelineSpec);
			printf("PTB-WARNING: Will revert to default settings for this element. This will likely fail soon...\n");
			printf("PTB-WARNING: Full parameter string was:\n");
			printf("PTB-WARNING: %s\n", codecSpec);
		}
		else {
			// Success!
			if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Element '%s' created from spec '%s'.\n", typeSpec, codecPipelineSpec);
		}
		free(codecPipelineSpec);
	}

	return(element);
}

/* CHECKED TODO
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
*      allow_lowperf_fallback = If set to 1 then PTB can use less capable fallback path on setups which don't support the 'camerabin' plugin.
*      targetmoviefilename = Filename of movie file to record (if any) if deviceIndex >=0,
*                            special spec string for a video capture device if deviceIndex < 0.
*      recordingflags = Special flags to control internal operation (harddisc recording yes/no, live video during recording?, audio recording? workarounds?)
*/
psych_bool PsychGSOpenVideoCaptureDevice(int slotid, PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle,
								   int reqdepth, int num_dmabuffers, int allow_lowperf_fallback, char* targetmoviefilename, unsigned int recordingflags)
{
	GstCaps                 *colorcaps, *filter_caps;
	GstElement		*camera = NULL;
	GMainLoop		*VideoContext = NULL;
	GstBus			*bus = NULL;
	GstElement              *videosink = NULL;
	GstElement              *videosource = NULL;
	GstElement              *videosource_filter = NULL;
	GstElement              *videocrop_filter = NULL;
	GstElement              *some_element = NULL;
	GstPad			*pad, *peerpad;
	GstCaps                 *caps;
	GstStructure		*str;
	gint			width, height;
	gint			rate1, rate2;
	gint			twidth, theight;
	int			i;
	char                    *codecSpec, *codecName;

	PsychVidcapRecordType	*capdev = NULL;
	char			config[1000];
	char			tmpstr[1000];
	char			device_name[1000];
	char			plugin_name[1000];
	char			prop_name[1000];
	gchar                   *pstring = NULL; 
	PsychVideosourceRecordType *theDevice = NULL;
	psych_bool              overrideFrameSize = FALSE;
	
	config[0] = 0;
	tmpstr[0] = 0;
	device_name[0] = 0;
	plugin_name[0] = 0;
	prop_name[0] = 0;

	// Init capturehandle to none:
	*capturehandle = -1;
	
	// Make sure GStreamer is ready:
	PsychGSCheckInit("videocapture");
	
	// Map deviceIndex of requested video source to device name:
	if (deviceIndex >= 0) {
		// Get device name for given deviceIndex from video device
		// enumeration (or NULL if no such device):
		theDevice = PsychGSEnumerateVideoSources(-1, deviceIndex);
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
    
    // Selection of pixel depths:
    if (reqdepth == 4 || reqdepth == 0) {
	    // Default is RGB 32 bit, aka RGBA8:
	    reqdepth = 4;
    }
    else {
	    // Only other supported format is RGB24 bit:
	    switch (reqdepth) {
	    case 2:
		    // A no-go: Instead we use 1 channel luminance8:
		    if (PsychPrefStateGet_Verbosity()>1)
			printf("PTB-WARNING: Video capture engine doesn't support requested Luminance+Alpha format. Will revert to pure luminance instead...\n");
		    reqdepth = 1;
	    break;
		    
	    case 1: // Accept as is: L8   aka Luminance 8 bit.
	    case 3: // Accept as is: RGB8 aka RGB 24 bit.
	    case 5: // Accept as YVYU.
	    break;
	    default:
		    // Unknown format:
		    PsychErrorExitMsg(PsychError_user, "You requested an invalid image depths (not one of 0, 1, 2, 3 or 4). Aborted.");
	    }	    
    }

    // Requested output texture pixel depth in layers:
    capdev->reqpixeldepth = reqdepth;
    capdev->pixeldepth = reqdepth * 8;

    // Assign number of dma buffers to use:
    capdev->num_dmabuffers = num_dmabuffers;

    PsychInitMutex(&vidcapRecordBANK[slotid].mutex);
    PsychInitCondition(&vidcapRecordBANK[slotid].condition, NULL);
    
    // Try to open and initialize camera according to given settings:
    // Create video capture pipeline with camerabin plugin:
    usecamerabin = TRUE;
    camera = gst_element_factory_make ("camerabin", "ptbvideocapturepipeline");
    sprintf(config, "%s", device_name);

    // Camerabin disabled or creation failed?
    if (NULL == camera) {
		// Failed or disabled: Use fallback playbin2 implementation.
		usecamerabin = FALSE;
		
		if (!allow_lowperf_fallback)
			PsychErrorExitMsg(PsychError_user, "Failed to create high-performance video capture pipeline and script doesn't allow fallback! Aborted.");
		
		camera = gst_element_factory_make ("playbin2", "ptbvideocapturepipeline");
		
		// Assign a specific input video source URI name if possible:
		if (theDevice && (theDevice->classIndex  < 5)) sprintf(config, "v4l2://%s", device_name);
		// DV1394 and HDV1394 only encode the port property in their URI, not the GUID. Therefore
		// we can not select a camera by GUID if we have to use playbin2 instead of camerabin.
		// Do the best we can do: Select the generic "default camera" URI for now:
		if (theDevice && (theDevice->classIndex == 5)) sprintf(config, "dv://");
		if (theDevice && (theDevice->classIndex == 6)) sprintf(config, "hdv://");
		
		if (PsychPrefStateGet_Verbosity() > 1) {
			printf("PTB-WARNING: Could not use GStreamer 'camerabin' plugin for videocapture. Will use 'playbin2' as fallback. Most\n");
			printf("PTB-WARNING: features, e.g., video recording, ROI and video resolution selection, are not supported in fallback mode.\n");
		}
    }
	
    // Pipeline creation failed?
    if (NULL == camera) PsychErrorExitMsg(PsychError_user, "Failed to create video capture pipeline! Fallback capture engine is also unsupported. Aborted.");

    // Assign new record in videobank:
    vidcapRecordBANK[slotid].camera = camera;
    vidcapRecordBANK[slotid].frameAvail = 0;

    // Enforce use of the system clock for this pipeline instead of leaving it to the pipeline
    // to choose a proper clock automatically:
    // TODO FIXME: To use or not to use? Preliminare results on Linux indicate it
    // doesn't make a difference, so skip it for now: gst_pipeline_use_clock(camera, gst_system_clock_obtain());

    // Need to select a video source for this capture pipeline:
    if (!usecamerabin) {
		// Fallback path with playbin2: All video source parameters are encoded in "URI"
		// property as a string:
		
		// Assign name and configuration parameters of video capture device to open:
		g_object_set(G_OBJECT(camera), "uri", config, NULL);
    }
    else {
		// High performance path with camerabin. Build an appropriate video source and set up its input:
		videosource = NULL;
		
		if (deviceIndex >= 0) {
			// Create proper videosource plugin if possible:
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach '%s' as video source...\n", plugin_name);
			videosource = gst_element_factory_make(plugin_name, "ptb_videosource");

			if (!videosource) {
				if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed! We are out of options and will probably fail soon.\n");
				PsychErrorExitMsg(PsychError_system, "GStreamer failed to find a suitable video source! Game over.");
			}

			// Attach correct video input device to it:
			if ((config[0] != 0) && (prop_name[0] != 0) && (theDevice->deviceURI == 0)) {
				if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device '%s' as video input [Property %s].\n", config, prop_name);
				g_object_set(G_OBJECT(videosource), prop_name, config, NULL);
			}

			if ((theDevice->deviceURI > 0) && (prop_name[0] != 0)) {
				if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device with guid '%llu' as video input [Property %s].\n", theDevice->deviceURI, prop_name);
				g_object_set(G_OBJECT(videosource), prop_name, theDevice->deviceURI, NULL);
			}
		}
		
		// MS-Windows specific setup path:
		if (PSYCH_SYSTEM == PSYCH_WINDOWS) {
			if (deviceIndex < 0) {
				// Non-Firewire video source selected:
				
				//if ((deviceIndex >= 10000) || (deviceIndex == -1) || (deviceIndex == -2)) {
				if ((deviceIndex == -1) || (deviceIndex == -2)) {
					// First try Kernel-Streaming based video source for low-latency capture:
					// if (deviceIndex >= 10000) deviceIndex = deviceIndex - 10000;
					if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach ksvideosrc as video source...\n");
					videosource = gst_element_factory_make("ksvideosrc", "ptb_videosource");
				}

				if (videosource) {
					// Kernel streaming video source:
					
					// Specific deviceIndex requested, instead of auto-select?
					if (deviceIndex < 0) {
						// Fetch optional targetmoviename parameter as name spec string:
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
				}

				// No kernel streaming video source available?
				if (!videosource) {
					// No. Try a Directshow video source instead:
					if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach dshowvideosrc as video source...\n");
					videosource = gst_element_factory_make("dshowvideosrc", "ptb_videosource");

					if (videosource) {
						// Specific deviceIndex requested, instead of auto-select?
						if (deviceIndex < 0) {
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
						else {
							// Human friendly device name provided:
							if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device '%s' as video input.\n", config);
							g_object_set(G_OBJECT(videosource), "device-name", config, NULL);							
						}
					}
				}
				// Have a standard windows video source ready.
			}
			
			// Still no video source available?
			if (!videosource) {
				if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed! We are out of options and will probably fail soon.\n");
				PsychErrorExitMsg(PsychError_system, "GStreamer failed to find a suitable video source! Game over.");
			}
		} // End of MS-Windows Video source creation.
		
		// MacOS/X specific setup path:
		if (PSYCH_SYSTEM == PSYCH_OSX) {
			if (deviceIndex < 0) {
				if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach osxvideosrc as video source...\n");
				videosource = gst_element_factory_make("osxvideosrc", "ptb_videosource");
				
				if (!videosource) {
					if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed! We are out of options and will probably fail soon.\n");
					PsychErrorExitMsg(PsychError_system, "GStreamer failed to find a suitable video source! Game over.");
				}
				
				// Fetch optional targetmoviename parameter as name spec string:
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
						// System device path provided:
						if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device '%s' as video input.\n", config);
						g_object_set(G_OBJECT(videosource), "device", config, NULL);
						break;

					case -5:
					if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach default Quicktime OS/X video device as video input.\n");
						break;
						
					default:
						PsychErrorExitMsg(PsychError_user, "Invalid 'deviceIndex' provided! Not a value of -1 or -2. Aborted.");
				}
			}
		} // End of OS/X Video source creation.

		// The usual crap for MS-Windows:
		if (strstr(plugin_name, "dshowvideosrc")) g_object_set(G_OBJECT(videosource), "typefind", 1, NULL);
		
		// Enable timestamping by videosource:
		g_object_set(G_OBJECT(videosource), "do-timestamp", 1, NULL);

		// Assign video source to pipeline:
		g_object_set(camera, "video-source", videosource, NULL);
	}

    // Assign message context, message bus and message callback for
    // the pipeline to report events and state changes, errors etc.:    
    VideoContext = g_main_loop_new (NULL, FALSE);
    vidcapRecordBANK[slotid].VideoContext = VideoContext;
    bus = gst_pipeline_get_bus(GST_PIPELINE(camera));
    gst_bus_add_watch(bus, PsychVideoBusCallback, &(vidcapRecordBANK[slotid]));
    gst_object_unref(bus);

    // Name of target movie file for video and audio recording specified?
    if ((deviceIndex >= 0) && targetmoviefilename) {
	    // Assign it to camerabin to perform video recording:
	    if (!usecamerabin)
		    PsychErrorExitMsg(PsychError_user, "You requested video recording, but current fallback video engine doesn't support this. Aborted.");

	    // Codec specified?
	    if (codecSpec = strstr(targetmoviefilename, ":CodecType=")) {
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
	    // Codecs are sorted by suitability for high quality realtime video recording, so if usercode
	    // doesn't specify a codec, and thereby requests use of the recommended default codec 'DEFAULTenc',
	    // we will try to choose the best codec, then on failure fallback to the 2nd best, 3rd best, etc.

	    // Video encoder from parameter string?
	    if ((some_element = CreateGStreamerElementFromString(codecSpec, "VideoCodec=")) != NULL) {
		    // Yes. Assign it as our encoder:
		    capdev->videoenc = some_element;

		    // Need to erase the actual name of the codec from the codecSpec string,
		    // otherwise our code below would get confused.
		    codecName = strstr(codecSpec, "VideoCodec=");
		    codecName+= strlen("VideoCodec=");
		    while ((*codecName > 0) && (*codecName != ' ')) *(codecName++) = 'X';

		    codecName = strdup("Config Defined");
	    }

	    // Start with H264 encoder as default:
	    if (strstr(codecSpec, "x264enc") || strstr(codecSpec, "1635148593") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
		    capdev->videoenc = gst_element_factory_make ("x264enc", "ptbvideocodec0");
		    if (!capdev->videoenc) {
			    printf("PTB-WARNING: Failed to create 'x264enc' H.264 video encoder! Does not seem to be installed on your system?\n");
		    }
		    else {
			    codecName = strdup("x264enc");

			    // Need to use avi-multiplexer:
			    g_object_set(camera, "video-muxer", gst_element_factory_make ("avimux", "ptbvideomuxer0"), NULL);

			    // Need to use faac MPEG-4 audio encoder:
			    g_object_set(camera, "audio-encoder", gst_element_factory_make ("faac", "ptbaudioenc0"), NULL);

			    // Set some reasonable default parameters for the codec, which provide a better
			    // speed-quality tradeoff for live video recording: (Thanks to Tobias Wolf!)

			    // Speed-Quality profile set to 1 "Ultra fast".
			    // (1 = "Ultra fast", 2 = "Super fast", 3 = "Very fast",  4 = "Faster", 5 = "Fast",
			    //  6 = "Medium" - the default,7 = "Slow", 8 = "Slower", 9 = "Very slow")
			    g_object_set(capdev->videoenc, "speed-preset", 1, NULL);
			    // g_object_set(capdev->videoenc, "", , NULL);
			    // g_object_set(capdev->videoenc, "", , NULL);
		    }
	    }

	    // Then xvidenc - MPEG4 in a AVI container: High quality, handles 640 x 480 @ 30 fps on
	    // a 4 year old MacBookPro Core2Duo 2.2 Ghz with about 70% - 100% cpu load, depending on settings.
	    if (strstr(codecSpec, "xvidenc") || strstr(codecSpec, "1836070006") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
		    capdev->videoenc = gst_element_factory_make ("xvidenc", "ptbvideocodec0");
		    if (!capdev->videoenc) {
			    printf("PTB-WARNING: Failed to create 'xvidenc' xvid/mpeg-4 video encoder! Does not seem to be installed on your system?\n");
		    }
		    else {
			    codecName = strdup("xvidenc");

			    // Need to use avi-multiplexer:
			    g_object_set(camera, "video-muxer", gst_element_factory_make ("avimux", "ptbvideomuxer0"), NULL);

			    // Need to use faac MPEG-4 audio encoder:
			    g_object_set(camera, "audio-encoder", gst_element_factory_make ("faac", "ptbaudioenc0"), NULL);
		    }
	    }

	    // ffenc_mpeg4 also creates MPEG4, but at lower quality - much more blocky etc.
	    if (strstr(codecSpec, "ffenc_mpeg4") || ((strstr(codecSpec, "DEFAULTenc") || strstr(codecSpec, "1836070006")) && !capdev->videoenc)) {
		    capdev->videoenc = gst_element_factory_make ("ffenc_mpeg4", "ptbvideocodec0");
		    if (!capdev->videoenc) {
			    printf("PTB-WARNING: Failed to create 'ffenc_mpeg4' mpeg-4 video encoder! Does not seem to be installed on your system?\n");
		    }
		    else {
			    codecName = strdup("ffenc_mpeg4");

			    // Need to use avi-multiplexer:
			    g_object_set(camera, "video-muxer", gst_element_factory_make ("avimux", "ptbvideomuxer0"), NULL);

			    // Need to use faac MPEG-4 audio encoder:
			    g_object_set(camera, "audio-encoder", gst_element_factory_make ("faac", "ptbaudioenc0"), NULL);
		    }
	    }

	    // Theora + Ogg vorbis audio in .ogv container:
	    if (strstr(codecSpec, "theoraenc") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
		    capdev->videoenc = gst_element_factory_make ("theoraenc", "ptbvideocodec0");
		    if (!capdev->videoenc) {
			    printf("PTB-WARNING: Failed to create 'theora' video encoder! Does not seem to be installed on your system?\n");
		    }
		    else {
			    codecName = strdup("theoraenc");

			    // Need to use ogg-multiplexer:
			    g_object_set(camera, "video-muxer", gst_element_factory_make ("oggmux", "ptbvideomuxer0"), NULL);

			    // Need to use vorbis audio encoder:
			    g_object_set(camera, "audio-encoder", gst_element_factory_make ("vorbisenc", "ptbaudioenc0"), NULL);

			    // Some defaults for the theora encoder:
			    g_object_set(G_OBJECT(capdev->videoenc), "drop-frames", FALSE, NULL);
			    g_object_set(G_OBJECT(capdev->videoenc), "speed-level", 2, NULL);
			    g_object_set(G_OBJECT(capdev->videoenc), "quality", 30, NULL);
			    g_object_set(G_OBJECT(capdev->videoenc), "keyframe-auto", FALSE, NULL);
			    g_object_set(G_OBJECT(capdev->videoenc), "keyframe-force", 15, NULL);
		    }
	    }

	    // VP-8 in WebM container:
	    if (strstr(codecSpec, "vp8enc") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
		    capdev->videoenc = gst_element_factory_make ("vp8enc", "ptbvideocodec0");
		    if (!capdev->videoenc) {
			    printf("PTB-WARNING: Failed to create 'vp8enc' VP-8 video encoder! Does not seem to be installed on your system?\n");
		    }
		    else {
			    if (strstr(codecSpec, "DEFAULTenc")) codecName = strdup("vp8enc_webm");

			    // Need to use matroska/webm-multiplexer:
			    if (strstr(codecName, "_matroska")) g_object_set(camera, "video-muxer", gst_element_factory_make ("matroska", "ptbvideomuxer0"), NULL);
			    if (strstr(codecName, "_webm")) g_object_set(camera, "video-muxer", gst_element_factory_make ("webmmuxer", "ptbvideomuxer0"), NULL);

			    // Need to use vorbis audio encoder:
			    g_object_set(camera, "audio-encoder", gst_element_factory_make ("vorbisenc", "ptbaudioenc0"), NULL);
		    }
	    }

	    // H263 -- Does not work well yet.
	    if (strstr(codecSpec, "ffenc_h263p") || strstr(codecSpec, "1748121139") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
		    capdev->videoenc = gst_element_factory_make ("ffenc_h263p", "ptbvideocodec0");
		    if (!capdev->videoenc) {
			    printf("PTB-WARNING: Failed to create 'ffenc_h263p' H.263 video encoder! Does not seem to be installed on your system?\n");
		    }
		    else {
			    codecName = strdup("ffenc_h263p");

			    // Need to use Quicktime-Multiplexer:
			    g_object_set(camera, "video-muxer", gst_element_factory_make ("qtmux", "ptbvideomuxer0"), NULL);

			    // Need to use faac MPEG-4 audio encoder:
			    g_object_set(camera, "audio-encoder", gst_element_factory_make ("faac", "ptbaudioenc0"), NULL);
		    }
	    }

	    // Raw Huffman encoded YUV -- Does not work yet.
	    if (strstr(codecSpec, "huffyuv") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
		    capdev->videoenc = gst_element_factory_make ("ffenc_huffyuv", "ptbvideocodec0");
		    if (!capdev->videoenc) {
			    printf("PTB-WARNING: Failed to create 'ffenc_huffyuv' YUV video encoder! Does not seem to be installed on your system?\n");
		    }
		    else {
			    codecName = strdup("huffyuv");

			    // Need to use matroska-multiplexer:
			    g_object_set(camera, "video-muxer", gst_element_factory_make ("matroskamux", "ptbvideomuxer0"), NULL);

			    // Need to use faac MPEG-4 audio encoder:
			    g_object_set(camera, "audio-encoder", gst_element_factory_make ("faac", "ptbaudioenc0"), NULL);
		    }
	    }

	    // Raw YUV: -- Does not work yet.
	    if (strstr(codecSpec, "yuvraw") || (strstr(codecSpec, "DEFAULTenc") && !capdev->videoenc)) {
		    capdev->videoenc = gst_element_factory_make ("identity", "ptbvideocodec0");
		    if (!capdev->videoenc) {
			    printf("PTB-WARNING: Failed to create 'identity' YUV pass-through video encoder! Does not seem to be installed on your system?\n");
		    }
		    else {
			    codecName = strdup("yuvraw");

			    // Need to use avi-multiplexer:
			    g_object_set(camera, "video-muxer", gst_element_factory_make ("avimux", "ptbvideomuxer0"), NULL);

			    // Need to use faac MPEG-4 audio encoder:
			    g_object_set(camera, "audio-encoder", gst_element_factory_make ("faac", "ptbaudioenc0"), NULL);
		    }
	    }

	    // Audio encoder from parameter string?
	    if ((some_element = CreateGStreamerElementFromString(codecSpec, "AudioCodec=")) != NULL) {
		    g_object_set(camera, "audio-encoder", some_element, NULL);
	    }

	    // Audio source from parameter string?
	    if ((some_element = CreateGStreamerElementFromString(codecSpec, "AudioSource=")) != NULL) {
		    g_object_set(camera, "audio-source", some_element, NULL);
	    }

	    // Multiplexer from parameter string?
	    if (strstr(codecSpec, "Muxer=")) {
		    // Must create it without help of CreateGStreamerElementFromString() as the muxer has
		    // 2 sink pads, but our method can only handle 1 sink pad. Therefore just create a
		    // muxer by name, setting it to its default settings:
		    codecSpec = strstr(codecSpec, "Muxer=");
		    codecSpec+= strlen("Muxer=");
		    g_object_set(camera, "video-muxer", gst_element_factory_make(codecSpec, "ptbvideomuxer0"), NULL);
	    }

	    // Still no video codec? Then this is game over:
	    if (capdev->videoenc == NULL) PsychErrorExitMsg(PsychError_user, "Could not find or setup requested video codec or any fallback codec for video recording. Aborted.");

	    // Attach our video encoder:
	    g_object_set(camera, "video-encoder", capdev->videoenc, NULL);
	    
	    if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: Video%s recording into file [%s] enabled for device %i. Codec is [%s].\n",
							 ((recordingflags & 2) ? " and audio" : ""), targetmoviefilename, deviceIndex, codecName);
	    capdev->targetmoviefilename = strdup(targetmoviefilename); 
	    capdev->recording_active = TRUE;
    } else {
	    capdev->recording_active = FALSE;
    }

    // Create a appsink named "ptbsink0" for attachment as the destination video-sink for
    // all video content. This allows us to get hold of the videoframe buffers for
    // converting them into PTB OpenGL textures:
    videosink = gst_element_factory_make ("appsink", "ptbsink0");
    if (!videosink) {
	printf("PTB-ERROR: Failed to create video-sink appsink ptbsink!\n");
	PsychGSProcessVideoContext(vidcapRecordBANK[slotid].VideoContext, TRUE);
	PsychErrorExitMsg(PsychError_system, "Opening the videocapture device failed. Reason hopefully given above.");
    };

    vidcapRecordBANK[slotid].videosink = videosink;

    // Our OpenGL texture creation routine needs GL_BGRA8 data in GL_UNSIGNED_8_8_8_8_REV
    // format, but the pipeline usually delivers YUV data in planar format. Therefore
    // need to perform colorspace/colorformat conversion. These colorcaps, as assigned to
    // the videosink will try to nudge the video source to deliver data in our requested
    // format. If this ain't possible, they will enforce creation of a colorspace converter
    // inbetween the video source and our videosink:
    switch (reqdepth) {
    case 4:
	    colorcaps = gst_caps_new_simple (   "video/x-raw-rgb",
						"bpp", G_TYPE_INT, capdev->pixeldepth,
						"depth", G_TYPE_INT, capdev->pixeldepth,
						"alpha_mask", G_TYPE_INT, 0x000000FF,
						"red_mask", G_TYPE_INT,   0x0000FF00,
						"green_mask", G_TYPE_INT, 0x00FF0000,
						"blue_mask", G_TYPE_INT,  0xFF000000,
						NULL);
	    break;
    case 3:
	    colorcaps = gst_caps_new_simple (   "video/x-raw-rgb",
						"bpp", G_TYPE_INT, capdev->pixeldepth,
						"depth", G_TYPE_INT, capdev->pixeldepth,
						"red_mask", G_TYPE_INT,   0x00FF0000,
						"green_mask", G_TYPE_INT, 0x0000FF00,
						"blue_mask", G_TYPE_INT,  0x000000FF,
						NULL);
	    break;
    case 2:
    case 1:
	    colorcaps = gst_caps_new_simple (   "video/x-raw-gray",
						"bpp", G_TYPE_INT, capdev->pixeldepth,
						NULL);
	    break;
    case 5:
	    colorcaps = gst_caps_new_simple (   "video/x-raw-yuv",
						"format", GST_TYPE_FOURCC, GST_MAKE_FOURCC('U', 'Y', 'V', 'Y'),
						NULL);
	    reqdepth = 2;
	    capdev->reqpixeldepth = 2;
	    capdev->pixeldepth = 16;

	    break;
    default:
	PsychErrorExitMsg(PsychError_internal, "Unknown reqdepth parameter received!");            
    }

    // Assign 'colorcaps' as caps to our videosink. This marks the videosink so
    // that it can only receive video image data in the format defined by colorcaps,
    // i.e., a format that is easy to consume for OpenGL's texture creation on std.
    // gpu's. It is the job of the video pipeline's autoplugger to plug in proper
    // color & format conversion plugins to satisfy videosink's needs.
    gst_app_sink_set_caps(GST_APP_SINK(videosink), colorcaps);

    // ROI rectangle specified and use of it supported by camerabin?
    if (usecamerabin && capturerectangle) {
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
	if (capturerectangle) {
		// ROI spec'd but camerabin not supported. This is a no-go.
		capturerectangle = NULL;
		if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Selection of video ROI's or capture resolution not supported by your setup. Using full resolution.\n");
	}

	// No user specified resolution. Rely on auto-detection:
	twidth  = -1;
	theight = -1;
    }

    // Assign our special appsink 'videosink' as video-sink of the pipeline:
    if (!usecamerabin) {
	if (capdev->recording_active) PsychErrorExitMsg(PsychError_user, "Video recording requested, but this isn't supported on this setup, sorry.");
	g_object_set(G_OBJECT(camera), "video-sink", videosink, NULL);

	// Invalidate capdev->width,height/fps etc. so it gets auto-detected
	// by preroll op:
	capdev->width = capdev->height = 0;
	capdev->fps = 0;
    } else {
	    // Attach our appsink as videosink:
	    g_object_set(G_OBJECT(camera), "viewfinder-sink", videosink, NULL);

	    // Setup pipeline for video recording or pure capture:
	    if (!capdev->recording_active) {
		    // Pure video capture, no recording: Optimize pipeline for this case:
		    g_object_set(G_OBJECT(camera),
				 "flags", 1+2+4,
				 "filter-caps", colorcaps,
				 NULL);
	    } else {
		    // Video recording (with optional capture). Setup pipeline:
		    filter_caps = gst_caps_from_string ("video/x-raw-yuv,format=(fourcc)I420");
		    
		    g_object_set(G_OBJECT(camera),
				 // Only enable sound encoding if "audio recording" flag 2 is set in
				 // recordingflags. Otherwise add flags 0x20 to disable audio encoding:
				 "flags", 0x08 + 0x04 + 0x02 + 0x01 + ((recordingflags & 2) ? 0x10 : 0x20),
				 // filter caps not needed for V4L2 webcam sources. TODO FIXME: Needed for non-Video4Linux2 sources?
				 // "filter-caps", filter_caps,
				 NULL);

		    gst_caps_unref(filter_caps);
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
				    printf("PTB-WARNING: audio-video sync problems or wrong timing/speed in videostream!\n");
			    }
		    }
		    else {
			    // By default we apply the videorate converter upstream at the videosource,
			    // so it affects both video recording and live video feedback. Usercode can
			    // restrict conversion to the recorded video stream only by setting
			    // recordingflags & 256:
			    if (recordingflags & 256) {
				    // Attach to video-post-processing -- immediately before video encoder:
				    g_object_set(G_OBJECT(camera), "video-post-processing", capdev->videorate_filter, NULL);
				    if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Restricting video framerate conversion to recorded video.\n"); 
			    }
			    else {
				    // Attach as video-source-filter -- upstream right at the source:
				    videosource_filter = capdev->videorate_filter;
				    if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Video framerate conversion applies to recorded video and live video.\n"); 
			    }
		    }
	    }

	    // Specific capture resolution requested?
	    if ((twidth != -1) && (theight != -1)) {
            // Yes. Validate and request it.
            
            // The usual Windows crap. Enumeration of supported resolutions doesn't work, so
            // we skip validation and trust blindly that the usercode is right if this is the
            // DirectShow video source:
            if (!strstr(plugin_name, "dshowvideosrc")) {
                // Query camera if it supports the requested resolution:
                capdev->fps = -1;
                if (!PsychGSGetResolutionAndFPSForSpec(capdev, &twidth, &theight, &capdev->fps, reqdepth)) {
                    // Unsupported resolution. Game over!
                    printf("PTB-ERROR: Video capture device %i doesn't support requested video capture resolution of %i x %i pixels! Aborted.\n", slotid, twidth, theight);
                    PsychErrorExitMsg(PsychError_user, "Failed to open video device at requested video resolution.");
                }
            }
            
            // Resolution supported. Request it:
            g_object_set(G_OBJECT(camera),
                         "video-capture-width", twidth,
                         "video-capture-height", theight,
                         NULL);
            
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

            // Auto-Detection doesn't work with Windows DirectShow video plugin :-( Skip it.
            if (!strstr(plugin_name, "dshowvideosrc")) {
                // Ask camera to provide auto-detected parameters:
                if (!PsychGSGetResolutionAndFPSForSpec(capdev, &capdev->width, &capdev->height, &capdev->fps, reqdepth)) {
                    // Unsupported resolution. Game over!
                    printf("PTB-ERROR: Auto-Detection of optimal video resolution on video capture device %i failed! Aborted.\n", slotid);
                    PsychErrorExitMsg(PsychError_user, "Failed to open video device with auto-detected video resolution.");
                }
                
                // Resolution supported. Request it:
                g_object_set(G_OBJECT(camera),
                             "video-capture-width", capdev->width,
                             "video-capture-height", capdev->height,
                             NULL);
            }
            else {
                // Directshow source. Set "don't know" values and hope the fallback code below
                // does a better job at guessing the true source resolution:
                capdev->width  = 0;
                capdev->height = 0;

                // ROI defined? We can't handle this, because we don't know the true video capture resolution
                // which would be needed at this point in the setup path due to the broken enumeration of dshowvideosrc.
                if (capturerectangle) {
                    capturerectangle = NULL;
                    overrideFrameSize = TRUE;

                    // Delete useless videocrop element if any:
                    if (videocrop_filter) g_object_unref(G_OBJECT(videocrop_filter));
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
        
	    // videocrop_filter for ROI processing available?
	    if (videocrop_filter) {
		    // Setup final cropping region:
		    g_object_set(G_OBJECT(videocrop_filter),
				 "left",   (int) capturerectangle[kPsychLeft],
				 "top",    (int) capturerectangle[kPsychTop],
				 "right",  capdev->width - (int) capturerectangle[kPsychRight],
				 "bottom", capdev->height - (int) capturerectangle[kPsychBottom],
				 NULL);

		    // HACK HACK HACK: This video ROI cropping implementation is not what we want,
		    // but it is the best we can do to workaround what i think are GStreamer bugs.
		    // Will work correctly for live capture cropping only. As soon as videorecording
		    // gets cropped as well, it affects the live feed in ugly ways...

		    // Is video recording active and ROI cropping for recording explicitely enabled
		    // via recordingflags setting 512?
		    if (capdev->recording_active && (recordingflags & 512)) {
			    // Yes. Attach filter to video-post-processing. This way it affects
			    // recorded video, but unfortunately also *always* the viewfinder video feed
			    // - which i think is a bug - and it affects the viewfinder feed in a weird way -
			    // which i think is a 2nd bug: The viewfinder/appsink gets the cropped
			    // ROI, but upscaled to full video resolution! We need to hack the
			    // capdev->frame_width and capdev->frame_height fields to take this
			    // into account, so we at least get a distorted image in the video
			    // textures. Usercode can Screen('DrawTexture') such distorted textures
                // with a properly scaled 'dstrect' to undistort, although this is a
			    // messy endeveaour -- but not impossible.

			    // So we attach to video-post-processing and set a special flag to
			    // signal the capdev->frame_width and capdev->frame_height needs to
			    // get hacked into shape :-(
			    g_object_set(G_OBJECT(camera), "video-post-processing", videocrop_filter, NULL);
			    overrideFrameSize = TRUE;

			    if (PsychPrefStateGet_Verbosity() > 1) {
				    printf("PTB-WARNING: Application of ROI's to recorded video is buggy, due to\n");
				    printf("PTB-WARNING: GStreamer bugs. The ROI gets correctly applied to the \n");
				    printf("PTB-WARNING: recorded video. It also gets applied to the live video feed,\n");
				    printf("PTB-WARNING: regardless if you want it or not. The live ROI texture images are\n");
				    printf("PTB-WARNING: distorted. They have the full video resolution size, but only\n");
				    printf("PTB-WARNING: show the ROI, zoomed and scaled to fit the full video resolution!\n");
				    printf("PTB-WARNING: You can either manually undistort the images during texture drawing,\n");
				    printf("PTB-WARNING: by proper choice of the 'dstRect' parameter in Screen('DrawTexture',...),\n");
				    printf("PTB-WARNING: or you can avoid application of ROI's to recorded video. In that case\n");
				    printf("PTB-WARNING: the video will be recorded at full video resolution, but application of\n");
				    printf("PTB-WARNING: ROI's to the live video textures will work correctly.\n\n");
			    }
		    }
		    else {
			    // No. Either no videorecording active -- pure live feedback, or it is
			    // active, but cropping of recorded video is not enabled.
			    // This means only the viewfinder, aka appsink aka our live video textures
			    // shall be cropped to ROI, if at all. This works as expected and is
			    // thankfully the common case for most applications of ROI's.

			    // Check if live feed is enabled and ROI cropping for it is not disabled:
			    if (!(recordingflags & 4) && !(recordingflags & 1024)) {
				    // Yes. Viewfinder feed shall be cropped: Attach to viewfinder-filter:
				    g_object_set(G_OBJECT(camera), "viewfinder-filter", videocrop_filter, NULL);

				    // No stupid hack needed in this case:
				    overrideFrameSize = FALSE;
			    }
			    else {
				    // Disable application of ROI to video buffers:
				    overrideFrameSize = TRUE;
			    }
		    }

#if 0
/* This should be the correct implementation, but it doesn't work due to what i think are bugs
   in the GStreamer videocrop or camerabin plugin. Therefore this codepath is disabled:
*/
		    // Attach video cropping to viewfinder-filter --> Apply to our live video feed,
		    // unless the "only apply to videorecording" recordingflag 512 is set or the
		    // live feed is disabled via flag 4
		    if (!(recordingflags & 1024) && !(recordingflags & 4)) {
			    g_object_set(G_OBJECT(camera), "viewfinder-filter", videocrop_filter, NULL);
		    }
		    else {
			    // Disable application of ROI to video buffers:
			    overrideFrameSize = TRUE;
		    }

		    // Is videorecording active and cropping for it requested?
		    if ((recordingflags & 512) && capdev->recording_active) {
			    // Need to attach to video-post-processing as well. We need to setup a 2nd
			    // cropping filter with identical settings for this, unless the 1st one isn't
			    // used as the viewfinder-filter.
			    if ((recordingflags & 1024) || (recordingflags & 4)) {
				    // Live feed disabled or no cropping for live feed wanted. The videocrop_filter
				    // is unused and we can use it here for cropping the videorecording by
				    // attaching to video-post-processing:
				    g_object_set(G_OBJECT(camera), "video-post-processing", videocrop_filter, NULL);
			    }
			    else {
				    // videocrop_filter already used for cropping the live feed aka viewfinder.
				    // Generate a new videocrop element, set it up identically, attach it to
				    // video-post-processing:
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
					    g_object_set(G_OBJECT(camera), "video-post-processing", videocrop_filter, NULL);
				    }
			    }
		    }
#endif
	    }


	    // Attach videosource filter, if any:
	    if (videosource_filter) {
		    g_object_set(G_OBJECT(camera), "video-source-filter", videosource_filter, NULL);
	    }
    }

    gst_caps_unref(colorcaps);

    // Disable asynchronous transition to GST_PAUSED state for video sink when simultaneous
    // video recording is active: TODO FIXME: This seems to have no effect whatsoever....
    if (capdev->recording_active) g_object_set(G_OBJECT(videosink), "async", FALSE, NULL);

    // Disable internal queuing of the last buffer, as we don't ever use the "last-buffer" property:
    g_object_set(G_OBJECT(videosink), "enable-last-buffer", FALSE, NULL);

    // Get the pad from the final sink for probing width x height of video frames and nominal framerate of video source:	
    pad = gst_element_get_pad(videosink, "sink");

    // Install callbacks used by the videosink (appsink) to announce various events:
    gst_app_sink_set_callbacks(GST_APP_SINK(videosink), &videosinkCallbacks, &(vidcapRecordBANK[slotid]), PsychDestroyNotifyCallback);

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

    PsychGSProcessVideoContext(vidcapRecordBANK[slotid].VideoContext, FALSE);

    if ((capdev->recording_active) && (recordingflags & 4)) {
	    // No live feedback (flags 4) implies no preroll (flags 8):
	    // This because a preroll would bring the pipeline from null state to
	    // at least ready state and thereby prevent us from detaching our
	    // appsink and replacing it with a fakesink. This would cause our
	    // appsink to collect video buffers which wouldn't get pulled/drained,
	    // effectively filling up all memory with "leaked" buffers until
	    // out of memory abort happens after a minute or so.
	    //
	    // Not prerolling -> pipeline in null state at fakesink attach time
	    // -> problem solved.
	    recordingflags |= 8;

	    if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Pipeline preroll skipped because only harddisc recording (recordingflags & 4).\n");
    }

    // Only preroll if prerolling not disabled by recordingflag 8,
    // or if we use the fallback path, which utterly needs this:
    if (!(recordingflags & 8) || !usecamerabin) {
	    // Ready the pipeline:
	    if (!PsychVideoPipelineSetState(camera, GST_STATE_READY, 30.0)) {
		    PsychGSProcessVideoContext(vidcapRecordBANK[slotid].VideoContext, TRUE);
		    PsychErrorExitMsg(PsychError_user, "In OpenVideoCapture: Opening the video capture device failed during pipeline zero -> ready. Reason given above.");
	    }

	    // Preroll the pipeline:
	    if (!PsychVideoPipelineSetState(camera, GST_STATE_PLAYING, 30.0)) {
		    PsychGSProcessVideoContext(vidcapRecordBANK[slotid].VideoContext, TRUE);
		    PsychErrorExitMsg(PsychError_user, "In OpenVideoCapture: Opening the video capture device failed during pipeline preroll ready->playing. Reason given above.");
	    }
    }

    if (!usecamerabin) {
	    g_object_get (G_OBJECT(camera),
			  "source", &videosource,
			  NULL);
    } else {
	    g_object_get (G_OBJECT(camera),
			  "video-source", &videosource,
			  NULL);
    }

    // Query number of available video and audio channels on capture device:
    if (!usecamerabin) {
	    g_object_get(G_OBJECT(camera),
		         "n-video", &vidcapRecordBANK[slotid].nrVideoTracks,
		         "n-audio", &vidcapRecordBANK[slotid].nrAudioTracks,
			 NULL);
    } else {
	// There's always at least a video channel:
	vidcapRecordBANK[slotid].nrVideoTracks = 1;
    }

    // We need a valid onscreen window handle for real video playback:
    if ((NULL == win) && (vidcapRecordBANK[slotid].nrVideoTracks > 0) && !(recordingflags & 4)) {
	    PsychErrorExitMsg(PsychError_user, "No windowPtr to an onscreen window provided. Must do so for sources with video channels!");
    }
 
    PsychGSProcessVideoContext(vidcapRecordBANK[slotid].VideoContext, FALSE);

    // Assign harmless initial settings for fps and frame size:
    rate1 = 0;
    rate2 = 1;
    width = height = 0;

    // Videotrack available and return of video data enabled?
    if (vidcapRecordBANK[slotid].nrVideoTracks > 0) {
	// Yes: Query video frame size and framerate of device:
	peerpad = gst_pad_get_peer(pad);
	caps = NULL;
	if (GST_IS_PAD(peerpad)) caps = gst_pad_get_negotiated_caps(peerpad);
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
    }

    // Release the pad:
    gst_object_unref(pad);

    // Our camera should be ready: Assign final handle.
    *capturehandle = slotid;

    // Increase counter of open capture devices:
    numCaptureRecords++;

    // Assign our first guess on video device resolution, only already
    // setup by detection code above:
    if ((capdev->width == 0) && (capdev->height)) {
        capdev->width = width;
        capdev->height = height;
    }
    
    // Reset framecounter:
    capdev->nrframes = 0;
    capdev->grabber_active = 0;

    rate1 = 0;
    rate2 = 1;
    width = height = 0;

    // Query true properties of attached video source:
    videosource = NULL;
	
    if (!usecamerabin) {
	    g_object_get (G_OBJECT(camera),
			  "source", &videosource,
			  NULL);
	    capdev->videoenc = NULL;
    } else {
	    g_object_get (G_OBJECT(camera),
			  "video-source", &videosource,
			  NULL);
	    g_object_get (G_OBJECT(camera),
			  "video-encoder", &capdev->videoenc,
			  NULL);
    }

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

	// Get the pad from the src pad of the source for probing width x height
	// of video frames and nominal framerate of video source:	
	pad = gst_element_get_pad(videosource, "src");

	// Videotrack available?
	if (vidcapRecordBANK[slotid].nrVideoTracks > 0) {
		// Yes: Query video frame size and framerate of device:
		peerpad = gst_pad_get_peer(pad);
		caps = NULL;
		if (GST_IS_PAD(peerpad)) caps = gst_pad_get_negotiated_caps(peerpad);
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

    // If we prerolled before, we need to undo its effects:
    if (!(recordingflags & 8) || !usecamerabin) {
	    // Pause the pipeline again:
	    if (!PsychVideoPipelineSetState(camera, (usecamerabin) ? GST_STATE_READY : GST_STATE_PAUSED, 30.0)) {
		    PsychGSProcessVideoContext(vidcapRecordBANK[slotid].VideoContext, TRUE);
		    PsychErrorExitMsg(PsychError_user, "In OpenVideoCapture: Opening the video capture device failed during preroll playing -> pause. Reason given above.");
	    }
    }

    if ((capdev->recording_active) && (recordingflags & 4)) {
	    // Only disc recording, no data return: Attach a fakesink to effectively
	    // disable most of viewfinder processing:
	    videosink = gst_element_factory_make ("fakesink", "ptbfakesink0");
	    g_object_set(G_OBJECT(camera), "viewfinder-sink", videosink, NULL);
    }

    /* Obsolete on Linux, hopefully also on Windows:
    // Make sure our capture timestamps are in system time (GetSecs() reference time)
    // unless recordingflags 64 are set, in which case timestamps are in pipeline
    // running time, or whatever GStreamer finds convenient.
    if (!(recordingflags & 64)) {
	    // Set defined start time for pipeline - Used for cts timestamps:
	    gst_element_set_base_time(camera, GST_CLOCK_TIME_NONE);
	    gst_element_set_start_time(camera, GST_CLOCK_TIME_NONE);
    }
    */

    // Assign final recordingflags:
    vidcapRecordBANK[slotid].recordingflags = recordingflags;

    if (PsychPrefStateGet_Verbosity() > 2) {
	    printf("PTB-INFO: Camera %i opened [Source resolution width x height = %i x %i, video image size %i x %i]\n",
		   slotid, capdev->width, capdev->height, capdev->frame_width, capdev->frame_height);
    }

    return(TRUE);
}

/* Internal helper:
 * Drain up to 'numFramesToDrain' videobuffers from the videosink of 'capdev'.
 * 'flags' modify drain behaviour. Unused so far.
 * Return number of drained buffers.
 */
int PsychGSDrainBufferQueue(PsychVidcapRecordType* capdev, int numFramesToDrain, unsigned int flags)
{
	GstBuffer *videoBuffer = NULL;
	int drainedCount = 0;

	// Drain while anything available, but at most numFramesToDrain frames.
	while (GST_IS_APP_SINK(capdev->videosink) && !gst_app_sink_is_eos(GST_APP_SINK(capdev->videosink))
		&& (capdev->frameAvail > 0) && (numFramesToDrain > drainedCount)) {
		capdev->frameAvail--;
		videoBuffer = gst_app_sink_pull_buffer(GST_APP_SINK(capdev->videosink));
		gst_buffer_unref(videoBuffer);
		videoBuffer = NULL;
		drainedCount++;
	}

	return(drainedCount);
}

/* CHECKED
*  PsychGSVideoCaptureRate() - Start- and stop video capture.
*
*  capturehandle = Grabber to start-/stop.
*  playbackrate = zero == Stop capture, non-zero == Capture
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
	GstElement		*camera = NULL;
	GstBuffer               *videoBuffer = NULL;
	GValue                  fRate = { 0, };
	guint64                 nrInFrames, nrOutFrames, nrDroppedFrames, nrDuplicatedFrames;
	int dropped = 0;
	int drainedCount;
	float framerate = 0;

	// Retrieve device record for handle:
	PsychVidcapRecordType* capdev = PsychGetGSVidcapRecord(capturehandle);
	camera = capdev->camera;

	// Make sure GStreamer is ready:
	PsychGSCheckInit("videocapture");

	PsychGSProcessVideoContext(capdev->VideoContext, FALSE);

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

		// Only do state transitions for camerabin:
		if (usecamerabin) {
			// Start the video capture for this camera.
			if (!PsychVideoPipelineSetState(camera, GST_STATE_READY, 10.0)) {
				// Failed!
				PsychGSProcessVideoContext(capdev->VideoContext, FALSE);
				PsychErrorExitMsg(PsychError_user, "Failure in pipeline transition null -> ready - Start of video capture failed!");
			}

			if (!PsychVideoPipelineSetState(camera, GST_STATE_PAUSED, 10.0)) {
				// Failed!
				PsychGSProcessVideoContext(capdev->VideoContext, FALSE);
				PsychErrorExitMsg(PsychError_user, "Failure in pipeline transition ready -> paused - Start of video capture failed!");
			}
		}

		// Setup of capture framerate & resolution:
		if (usecamerabin) {
			// Set requested capture/recording video resolution and framerate:
			// Map special capturerate value DBL_MAX to a fps nominator of zero. This
			// asks the engine to capture at the maximum supported framerate for given
			// format and resolution:
			g_signal_emit_by_name (G_OBJECT(camera),
					       "set-video-resolution-fps", capdev->width, capdev->height,
					       ((capturerate < DBL_MAX) ? (int)(capturerate + 0.5) : 0), 1);
		}
		else {
			// Set playback rate in non camerabin configuration:
			// We only set it if the special value DBL_MAX isn't provided. DBL_MAX means
			// to run at maximum supported fps. As we can't pass this request to playbin2,
			// we do the 2nd best and don't ask for a specific fps at all, hoping that playbin2
			// will then configure to a save high speed default value -- or at least to something
			// supported and safe:
			if (capturerate < DBL_MAX) {
				gst_element_seek(camera, capturerate, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE, GST_SEEK_TYPE_NONE, 0, GST_SEEK_TYPE_NONE, 0);
			}
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
			PsychGSProcessVideoContext(capdev->VideoContext, FALSE);
			PsychErrorExitMsg(PsychError_user, "Failure in pipeline transition paused -> playing - Start of video capture failed!");
		}
		
		// Start video recording if requested:
		if (usecamerabin && capdev->recording_active) {
			if (PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Starting recording...\n");
			g_object_set(G_OBJECT(camera), "mode", 1, NULL);
			if (PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Recording 1 started...\n");
			g_object_set(G_OBJECT(camera), "filename", capdev->targetmoviefilename, NULL);
			if (PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Recording 2 started...\n");
			g_signal_emit_by_name (camera, "capture-start", 0, 0);
			if (PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Recording started...\n");
		}

		// Wait for real start of capture, i.e., arrival of 1st captured
		// video buffer:
		PsychLockMutex(&capdev->mutex);

		// This wait is just here to clear potentially pending conditions on
		// MS-Windows. Therefore only wait for 1.1 msecs at most:
		PsychTimedWaitCondition(&capdev->condition, &capdev->mutex, 0.0011);

		// Wait for first frame to become available, but only if return of videodata is
		// actually enabled:
		if (!(capdev->recordingflags & 4)) {
			while (capdev->frameAvail == 0) {
				if (PsychPrefStateGet_Verbosity() > 5) {
					printf("PTB-DEBUG: Waiting for real start: fA = %i pA = %i fps=%f\n", capdev->frameAvail, capdev->preRollAvail, capdev->fps);
					fflush(NULL);
				}
				PsychTimedWaitCondition(&capdev->condition, &capdev->mutex, 10.0);
			}
		}

		// Assign a plausible framerate if none assigned properly:
		if (capdev->fps <= 0) capdev->fps = capturerate;

		// Ok, capture is now started:
		capdev->grabber_active = 1;

		PsychUnlockMutex(&capdev->mutex);

		// Record real start time:
		PsychGetAdjustedPrecisionTimerSeconds(startattime);

		// Some processing time for message dispatch:
		PsychGSProcessVideoContext(capdev->VideoContext, FALSE);

		if (PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Capture engine fully running...\n");
		
		/* // Allocate conversion buffer if needed for YUV->RGB conversions.
		if (capdev->pixeldepth == -1) {
			// Not used at the moment!!
			// Software conversion of YUV -> RGB needed. Allocate a proper scratch-buffer:
			capdev->scratchbuffer = malloc(capdev->frame_width * capdev->frame_height * 3);
		}
		*/

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
			if (usecamerabin && capdev->recording_active) {
				PsychGSProcessVideoContext(capdev->VideoContext, FALSE);

				if(PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: StopVideoCapture: Stopping video recording...\n");
				g_signal_emit_by_name (camera, "capture-stop", 0);

				if(PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: StopVideoCapture: Stopping pipeline [playing -> paused]\n");
				if (!PsychVideoPipelineSetState(camera, GST_STATE_PAUSED, 10.0)) {
					if(PsychPrefStateGet_Verbosity() > 0) {
						PsychGSProcessVideoContext(capdev->VideoContext, FALSE);
						printf("PTB-ERROR: StopVideoCapture: Unable to pause recording pipeline! Prepare for trouble!\n");
					}
				}

				if(PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: StopVideoCapture: Videorecording stopped.\n");
			}

			// Stop pipeline, bring it back into READY state:
			if(PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG: StopVideoCapture: Fully stopping and shutting down capture pipeline.\n");
			if (!PsychVideoPipelineSetState(camera, (usecamerabin) ? GST_STATE_READY : GST_STATE_PAUSED, 10.0)) {
				if(PsychPrefStateGet_Verbosity() > 0) {
					PsychGSProcessVideoContext(capdev->VideoContext, FALSE);
					printf("PTB-ERROR: StopVideoCapture: Unable to stop capture pipeline [Transition 1 to ready state failed]! Prepare for trouble!\n");
				}
			}
			else {
				// Capture stopped. More cleanup work:
				PsychGSProcessVideoContext(capdev->VideoContext, FALSE);
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
					printf("PTB-INFO: Framerate compensation received %i frames, delivered %i frames, had to drop %i frames and duplicate %i frames\n",
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
	
	fflush(NULL);

	// Reset framecounters and statistics:
	capdev->nrframes = 0;
	capdev->avg_decompresstime = 0;
	capdev->nrgfxframes = 0;
	capdev->avg_gfxtime = 0;
	
	// Return either the real capture framerate (at start of capture) or count of dropped frames - at end of capture.
	return((capturerate!=0) ? (int) (capdev->fps + 0.5) : dropped);
}


/* CHECKED TODO
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
*  summed_intensity = An optional ptr to a double variable. If non-NULL, then sum of intensities over all channels is calculated and returned.
*  outrawbuffer = An optional ptr to a memory buffer of sufficient size. If non-NULL, the buffer will be filled with the captured raw image data, e.g., for use inside Matlab or whatever...
*  Returns Number of pending or dropped frames after fetch on success (>=0), -1 if no new image available yet, -2 if no new image available and there won't be any in future.
*/
int PsychGSGetTextureFromCapture(PsychWindowRecordType *win, int capturehandle, int checkForImage, double timeindex,
								 PsychWindowRecordType *out_texture, double *presentation_timestamp, double* summed_intensity, rawcapimgdata* outrawbuffer)
{
    PsychVidcapRecordType *capdev;
    GstElement *camera;
    GstBuffer *videoBuffer = NULL;
    gint64 bufferIndex;
    double deltaT = 0;
    GstEvent *event;
    GstClockTime baseTime;

    int waitforframe;
    GLuint texid;
    int w, h;
    double targetdelta, realdelta, frames;
    unsigned int intensity = 0;
    unsigned int count, i, bpp;
    unsigned char* pixptr;
    psych_bool newframe = FALSE;
    double tstart, tend;
    unsigned int pixval, alphacount;
    int error;
    int nrdropped = 0;
    unsigned char* input_image = NULL;


    // Make sure GStreamer is ready:
    PsychGSCheckInit("videocapture");

    // Take start timestamp for timing stats:
    PsychGetAdjustedPrecisionTimerSeconds(&tstart);
	
    // Retrieve device record for handle:
    capdev = PsychGetGSVidcapRecord(capturehandle);

    // Allow context task to do its internal bookkeeping and cleanup work:
    PsychGSProcessVideoContext(capdev->VideoContext, FALSE);

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

    // Size of a single pixel in bytes:
    bpp = capdev->reqpixeldepth;
	
    // If a outrawbuffer struct is provided, we fill it with info needed to allocate a
    // sufficient memory buffer for returned raw image data later on:
    if (outrawbuffer) {
	    outrawbuffer->w = w;
	    outrawbuffer->h = h;
	    outrawbuffer->depth = bpp;
    }
	
    waitforframe = (checkForImage > 1) ? 1:0; // Blocking wait for new image requested?
	
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
		    videoBuffer = gst_app_sink_pull_buffer(GST_APP_SINK(capdev->videosink));

		    // Release the capture buffer. Return it to the DMA ringbuffer pool:
		    gst_buffer_unref(videoBuffer);
		    videoBuffer = NULL;

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
    videoBuffer = gst_app_sink_pull_buffer(GST_APP_SINK(capdev->videosink));

    // We can unlock early, thanks to videosink's internal buffering:
    PsychUnlockMutex(&capdev->mutex);
    
    if (videoBuffer) {
	    // Assign pointer to videoBuffer's data directly: Avoids one full data copy compared to oldstyle method.
	    // input_image points to the image buffer in our cam:
	    input_image = (unsigned char*) (GLuint*) GST_BUFFER_DATA(videoBuffer);
	    
	    // Assign pts presentation timestamp in pipeline stream time and convert to seconds:
	    if (capdev->recordingflags & 64) {
		    // Retrieve raw buffer timestamp - pipeline running time.
		    capdev->current_pts = (double) GST_BUFFER_TIMESTAMP(videoBuffer) / (double) 1e9;
	    } else {
		    // Add base time to convert running time buffer timestamp into absolute time:
		    baseTime = gst_element_get_base_time(capdev->camera);
		    if (baseTime == 0) baseTime = capdev->lastSavedBaseTime;

		    capdev->current_pts = (double) (GST_BUFFER_TIMESTAMP(videoBuffer) + baseTime) / (double) 1e9;

            // Apply corrective offset for GStreamer clock base zero point:
            capdev->current_pts+= gs_startupTime;     
        }
        
	    deltaT = 0.0;
	    if (GST_CLOCK_TIME_IS_VALID(GST_BUFFER_DURATION(videoBuffer)))
		    deltaT = (double) GST_BUFFER_DURATION(videoBuffer) / (double) 1e9;

	    if (PsychPrefStateGet_Verbosity() > 6) {
		    PsychProbeBufferProps(videoBuffer, NULL, NULL, &capdev->fps);
		    printf("Bufferprobe: newfps = %f altfps = %f\n", capdev->fps, (float) ((deltaT > 0) ? 1.0 / deltaT : 0.0));
	    }

	    bufferIndex = GST_BUFFER_OFFSET(videoBuffer);
    } else {
	    if (PsychPrefStateGet_Verbosity()>0) printf("PTB-ERROR: No new video frame received in gst_app_sink_pull_buffer! Something's wrong. Aborting fetch.\n");
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
    // scratch buffer for color conversion alloc'ed?
    if ((capdev->scratchbuffer) && ((out_texture) || (summed_intensity) || (outrawbuffer))) {
		// Yes. Perform color-conversion YUV->RGB from cameras DMA buffer
		// into the scratch buffer and set scratch buffer as source for
		// all further operations:
		memcpy(capdev->scratchbuffer, input_image, w * h * bpp);
		
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

	    PsychMakeRect(out_texture->rect, 0, 0, w, h);    
	    
	    // Set NULL - special texture object as part of the PTB texture record:
	    out_texture->targetSpecific.QuickTimeGLTexture = NULL;
	    
	    // Set texture orientation as if it were an inverted Offscreen window: Upside-down.
	    out_texture->textureOrientation = 3;
	    
	    // Setup a pointer to our buffer as texture data pointer: Setting memsize to zero
	    // prevents unwanted free() operation in PsychDeleteTexture...
	    out_texture->textureMemorySizeBytes = 0;
	    
	    // Set texture depth: Could be 8, 16, 24 or 32 bpp.
	    out_texture->depth = capdev->reqpixeldepth * 8;
	    
	    // 4-channel textures are aligned on 4 Byte boundaries because texels are RGBA8:
	    out_texture->textureByteAligned = (capdev->reqpixeldepth == 4) ? 4 : 1;

	    // This will retrieve an OpenGL compatible pointer to the pixel data and assign it to our texmemptr:
	    out_texture->textureMemory = (GLuint*) input_image;
	    
	    // Special case depths == 2, aka YCBCR texture?
	    if (capdev->reqpixeldepth == 2) {
		    #if PSYCH_SYSTEM == PSYCH_LINUX
		    out_texture->textureinternalformat = GL_YCBCR_MESA;
		    out_texture->textureexternalformat = GL_YCBCR_MESA;
		    #else
		    out_texture->textureinternalformat = GL_YCBCR_422_APPLE;
		    out_texture->textureexternalformat = GL_YCBCR_422_APPLE;
		    #endif
		    out_texture->textureexternaltype   = GL_UNSIGNED_SHORT_8_8_MESA;
	    }

	    // Let PsychCreateTexture() do the rest of the job of creating, setting up and
	    // filling an OpenGL texture with content:
	    PsychCreateTexture(out_texture);
	    
	    // Ready to use the texture...
    }
    
    // Sum of pixel intensities requested?
    if(summed_intensity) {
	    pixptr = (unsigned char*) input_image;
	    count  = w * h * bpp;
	    for (i=0; i<count; i++) intensity+=(unsigned int) pixptr[i];
	    *summed_intensity = ((double) intensity) / w / h / bpp;
    }
    
    // Raw data requested?
    if (outrawbuffer) {
	    // Copy it out:
	    outrawbuffer->w = w;
	    outrawbuffer->h = h;
	    outrawbuffer->depth = bpp;
	    count = (w * h * outrawbuffer->depth);
	    memcpy(outrawbuffer->data, (const void *) input_image, count);
    }
	
    // Release the capture buffer. Return it to the DMA ringbuffer pool:
    gst_buffer_unref(videoBuffer);
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

// CHECKED
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
	unsigned int minval, maxval, intval, oldintval;
	int triggercount;

	double oldvalue = DBL_MAX; // Initialize return value to the "unknown/unsupported" default.
	psych_bool assigned = false;
	psych_bool present  = false;
	
	// Retrieve device record for handle:
	PsychVidcapRecordType* capdev = PsychGetGSVidcapRecord(capturehandle);

	// Make sure GStreamer is ready:
	PsychGSCheckInit("videocapture");
	
	oldintval = 0xFFFFFFFF;
	
	// Round value to integer:
	intval = (int) (value + 0.5);
	
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
		// which are only available on selected cameras.
		// We currently only know how to do this on Basler cameras.
		return(-2);
	}
	
	if (strcmp(pname, "PrintParameters")==0) {
		// Special command: List and print all features...
		printf("PTB-INFO: The camera provides the following information and featureset:\n");
		printf("PTB-INFO: Sorry, 'PrintParameters' not yet implemented.\n");
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
		PsychCopyOutCharArg(1, FALSE, capdev->cameraFriendlyName);
		return(0);
	}

/*		if (usecamerabin && capdev->recording_active) {
		}
*/
	
//	if (strstr(pname, "Brightness")!=0) {
//		assigned = true;
//		feature = DC1394_FEATURE_BRIGHTNESS;    
//	}
//	
//	if (strstr(pname, "Gain")!=0) {
//		assigned = true;
//		feature = DC1394_FEATURE_GAIN;    
//	}
//	
//	if (strstr(pname, "Exposure")!=0) {
//		assigned = true;
//		feature = DC1394_FEATURE_EXPOSURE;    
//	}
//	
//	if (strstr(pname, "Shutter")!=0) {
//		assigned = true;
//		feature = DC1394_FEATURE_SHUTTER;    
//	}
//	
//	if (strstr(pname, "Sharpness")!=0) {
//		assigned = true;
//		feature = DC1394_FEATURE_SHARPNESS;    
//	}
//	
//	if (strstr(pname, "Saturation")!=0) {
//		assigned = true;
//		feature = DC1394_FEATURE_SATURATION;    
//	}
//	
//	if (strstr(pname, "Gamma")!=0) {
//		assigned = true;
//		feature = DC1394_FEATURE_GAMMA;    
//	}
	
	// Check if feature is present on this camera:
	// Not supported yet:
	present = FALSE;
	
//	if (dc1394_feature_is_present(capdev->camera, feature, &present)!=DC1394_SUCCESS) {
//		if(PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Failed to query presence of feature %s on camera %i! Ignored.\n", pname, capturehandle);
//		fflush(NULL);
//	}
//	else
	
	if (present) {
		// Feature is available:
/*		
		// Retrieve current value:
		if (dc1394_feature_get_value(capdev->camera, feature, &oldintval)!=DC1394_SUCCESS) {
			if(PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Failed to query value of feature %s on camera %i! Ignored.\n", pname, capturehandle);
			fflush(NULL);
		}
		else {      
			// Do we want to set the value?
			if (value != DBL_MAX) {
				// Query allowed bounds for its value:
				if (dc1394_feature_get_boundaries(capdev->camera, feature, &minval, &maxval)!=DC1394_SUCCESS) {
					if(PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Failed to query valid value range for feature %s on camera %i! Ignored.\n", pname, capturehandle);
					fflush(NULL);
				}
				else {
					// Sanity check against range:
					if (intval < minval || intval > maxval) {
						if(PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Requested setting %i for parameter %s not in allowed range (%i - %i) for camera %i. Ignored.\n",
																   intval, pname, minval, maxval, capturehandle);
						fflush(NULL);      
					}
					else {
						// Ok intval is valid for this feature: Can we manually set this feature?
						// Switch feature to manual control mode:
						if (dc1394_feature_set_mode(capdev->camera, feature, DC1394_FEATURE_MODE_MANUAL)!=DC1394_SUCCESS) {
							if(PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Failed to set feature %s on camera %i to manual control! Ignored.\n", pname, capturehandle);
							fflush(NULL);
						}
						else {
							// Ok, try to set the features new value:
							if (dc1394_feature_set_value(capdev->camera, feature, intval)!=DC1394_SUCCESS) {
								if(PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Failed to set value of feature %s on camera %i to %i! Ignored.\n", pname, capturehandle,
																		   intval);
								fflush(NULL);
							}
						}
					}
				}
			}
			else {
				// Don't want to set new value. Do we want to reset feature into auto-mode?
				// Prefixing a parameter name with "Auto"
				// does not switch the parameter into manual
				// control mode + set its value, as normal,
				// but it switches the parameter into automatic
				// mode, if automatic mode is supported by the
				// device.
				if (strstr(pname, "Auto")!=0) {
					// Switch to automatic control requested - Try it:
					if (dc1394_feature_set_mode(capdev->camera, feature, DC1394_FEATURE_MODE_AUTO)!=DC1394_SUCCESS) {
						if(PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Failed to set feature %s on camera %i to automatic control! Ignored.\n", pname, capturehandle);
						fflush(NULL);
					}
				}
			}
		}
*/
	}
	else {
		if(PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Requested capture device setting %s not available on cam %i. Ignored.\n", pname, capturehandle);
		fflush(NULL);
	}
	
	// Output a warning on unknown parameters:
	if (!assigned) {
		if(PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Screen('SetVideoCaptureParameter', ...) called with unknown parameter %s. Ignored...\n",
												   pname);
		fflush(NULL);
	}
	
	if (assigned && oldintval!=0xFFFFFFFF) oldvalue = (double) oldintval;
	
	// Return the old value. Could be DBL_MAX if parameter was unknown or not accepted for some reason.
	return(oldvalue);
}

#endif
