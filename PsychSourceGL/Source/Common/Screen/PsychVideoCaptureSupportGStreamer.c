/*
	PsychSourceGL/Source/Common/Screen/PsychVideoCaptureSupportGStreamer.c
 
	PLATFORMS:	
	
	GNU/Linux, Apple MacOS/X and MS-Windows.
 
	AUTHORS:
	
	Mario Kleiner           mk              mario.kleiner@tuebingen.mpg.de
 
	HISTORY:
	
	9.01.2010				Created initial version.
	
	DESCRIPTION:
	
	This is the videocapture engine based on the free software (LGPL'ed)
	GStreamer multimedia framework. It supports video capture, sound capture and
	recording of video and/or sound.
	 
	NOTES:
 
	TODO:
 
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

// Record which defines all state for a capture device:
typedef struct {
	int valid;                        // Is this a valid device record? zero == Invalid.
	psych_mutex mutex;
	psych_condition condition;

	int frameAvail;
	int preRollAvail;

	GstElement *camera;               // Ptr to a GStreamer camera object that holds the internal state for such cams.
	GMainLoop *VideoContext;
	GstElement *videosink;
	GstElement *videosource;
	int nrAudioTracks;
	int nrVideoTracks;
	psych_uint8	*frame;		  // Ptr to a psych_uint8 matrix which contains the most recently captured/dequeued frame.
	int dropframes;			  // 1 == Always deliver most recent frame in FIFO, even if dropping of frames is neccessary.
	unsigned char* scratchbuffer;     // Scratch buffer for YUV->RGB conversion.
	int reqpixeldepth;                // Requested depth of single pixel in output texture.
	int pixeldepth;                   // Depth of single pixel from grabber in bits.
	int num_dmabuffers;               // Number of DMA ringbuffers to use in DMA capture.
	int nrframes;                     // Total count of decompressed images.
	double fps;                       // Acquisition framerate of capture device.
	int width;                        // Width x height of captured images.
	int height;
	double last_pts;                  // Capture timestamp of previous frame.
	double current_pts;               // Capture timestamp of current frame.
	int current_dropped;              // Dropped count for this fetch cycle...
	int nr_droppedframes;             // Counter for dropped frames.
	int frame_ready;                  // Signals availability of new frames for conversion into GL-Texture.
	int grabber_active;               // Grabber running?
	int recording_active;             // Movie file recording requested?
	PsychRectType roirect;            // Region of interest rectangle - denotes subarea of full video capture area.
	double avg_decompresstime;        // Average time spent in Quicktime/Sequence Grabber decompressor.
	double avg_gfxtime;               // Average time spent in GWorld --> OpenGL texture conversion and statistics.
	int nrgfxframes;                  // Count of fetched textures.
} PsychVidcapRecordType;

static PsychVidcapRecordType vidcapRecordBANK[PSYCH_MAX_CAPTUREDEVICES];
static int numCaptureRecords = 0;
static psych_bool gs_firsttime = TRUE;
double gs_startupTime = 0.0;

// Forward declaration of internal helper function:
void PsychGSDeleteAllCaptureDevices(void);

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
	//printf("PTB-DEBUG: Message EOS received.\n");
    break;

    case GST_MESSAGE_WARNING: {
      gchar  *debug;
      GError *error;

      gst_message_parse_warning(msg, &error, &debug);
      printf("PTB-WARNING: GStreamer videocapture engine reports this warning:\n"
	     "             Warning from element %s: %s\n", GST_OBJECT_NAME(msg->src), error->message);
      printf("             Additional debug info: %s.\n", (debug) ? debug : "None");

      g_free(debug);
      g_error_free(error);
      break;
    }

    case GST_MESSAGE_ERROR: {
      gchar  *debug;
      GError *error;

      gst_message_parse_error(msg, &error, &debug);
      printf("PTB-ERROR: GStreamer videocapture engine reports this error:\n"
	     "           Error from element %s: %s\n", GST_OBJECT_NAME(msg->src), error->message);
      printf("           Additional debug info: %s.\n\n", (debug) ? debug : "None");

      if ((error->domain == GST_RESOURCE_ERROR) && (error->code != GST_RESOURCE_ERROR_NOT_FOUND)) {
	      printf("           This means that there was some problem with opening the video device (permissions etc.).\n\n");
      }

      if ((error->domain == GST_RESOURCE_ERROR) && (error->code == GST_RESOURCE_ERROR_NOT_FOUND)) {
	      printf("           This means that no such video device with the given name could be found.\n\n");
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
	//printf("PTB-DEBUG: Videosink reached EOS.\n");
	PsychUnlockMutex(&capdev->mutex);

	return;
}

static void PsychProbeBufferProps(GstBuffer *videoBuffer, int *w, int *h, double *fps)
{
	const GstCaps          *caps;
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

	// Stop capture immediately if it is still running:
	PsychGSVideoCaptureRate(capturehandle, 0, 0, NULL);
	
	// Close & Shutdown camera, release ressources:
	// Stop video capture immediately:
	PsychVideoPipelineSetState(capdev->camera, GST_STATE_NULL, 20.0);
	
	// Delete camera for this handle:
	gst_object_unref(GST_OBJECT(capdev->camera));
	capdev->camera=NULL;
	
	// Delete video context:
	capdev->VideoContext = NULL;
	
	PsychDestroyMutex(&capdev->mutex);
	PsychDestroyCondition(&capdev->condition);
	
	capdev->videosink = NULL;
	capdev->camera = NULL;
	
	// Invalidate device record to free up this slot in the array:
	capdev->valid = 0;
    
	// Decrease counter of open capture devices:
	if (numCaptureRecords>0) numCaptureRecords--;
	
	// Done.
	return;
}

char* PsychGSEnumerateVideoSources(int outPos, int deviceIndex)
{
	PsychGenericScriptType 	*devs;
	const char *FieldNames[]={"DeviceIndex", "ClassIndex", "InputIndex", "ClassName", "InputHandle", "Device", "DevicePath", "DeviceName" };

	int				i, n;
	char				port_str[64];
	char				class_str[64];
	char				msgerr[10000];
	int				deviceClass, deviceInput;
	GstElement			*videosource = NULL;
	GstPropertyProbe		*probe = NULL;
	GValueArray			*viddevs = NULL;
	GValue				*dev = NULL;
	char				*device_name = NULL;
        gchar                           *pstring = NULL;

	// Make sure GStreamer is ready:
	PsychGSCheckInit("videocapture");
			
	// Linux specific setup path:
	if (PSYCH_SYSTEM == PSYCH_LINUX) {
		deviceClass = 0;

		// Try Video4Linux-II camera source first:
		if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to probe v4l2camsrc as video source...\n");
		videosource = gst_element_factory_make("v4l2camsrc", "ptb_videosource");
		sprintf(class_str, "Video4Linux2-CameraSource");
		
		// Fallback to standard Video4Linux-II source if neccessary:
		if (!videosource) {
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Failed. Trying to probe v4l2src as video source...\n");
			videosource = gst_element_factory_make("v4l2src", "ptb_videosource");
			sprintf(class_str, "Video4Linux2");
		}

		// Fallback to autovideosrc if everything else fails:
		if (!videosource) {
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Failed. Trying to probe autovideosrc as video source with auto-detected video input device...\n");
			videosource = gst_element_factory_make("autovideosrc", "ptb_videosource");
			sprintf(class_str, "AutoVideoSource");
		}
		
		if (!videosource) {
			if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-INFO: Failed! We are out of probe options. No video devices to enumerate.\n");
			return(NULL);
		}
	}

	if (PSYCH_SYSTEM == PSYCH_WINDOWS) {
		if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to probe ksvideosrc as video source...\n");
		videosource = gst_element_factory_make("ksvideosrc", "ptb_videosource");
		sprintf(class_str, "Windows WDM kernel streaming");
		deviceClass = 1;
		
		// Kernel streaming video source available and implements probe interface?
		if ((videosource) && (gst_element_implements_interface(videosource, GST_TYPE_PROPERTY_PROBE))) {
			// Yes: Need to pre-probe if it has any available sources:
			
			// Generate property probe for videosource:
			probe = GST_PROPERTY_PROBE(videosource);

			// Retrieve array of available video input device names, aka
			// "device-name" property:
			viddevs = gst_property_probe_probe_and_get_values_name(probe, "device-name");
			
			if (viddevs) {
				// Assign count of available devices:
				n = viddevs->n_values;
				g_value_array_free(viddevs);
			}
			else {
				n = 0;
				if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: ksvideosrc does not provide any probe results.\n");
			}
			
			if (n <= 0) {
				// No video sources for kernel streaming: Destroy the source,
				// so code can retry with DirectShow source below:
				gst_element_set_state(videosource, GST_STATE_NULL);
				gst_object_unref(GST_OBJECT(videosource));
				videosource = NULL;
				n = 0;
			}

			// Reset n:
			n = 0;
		}
		else {
			if (videosource) {
				gst_element_set_state(videosource, GST_STATE_NULL);
				gst_object_unref(GST_OBJECT(videosource));
				videosource = NULL;
			}
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: ksvideosrc unavailable or does not support probing.\n");
		}
		
		if (!videosource) {
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to probe dshowvideosrc as video source...\n");
			videosource = gst_element_factory_make("dshowvideosrc", "ptb_videosource");
			sprintf(class_str, "DirectShow");
			deviceClass = 0;
		}
		
		if (!videosource) {
			if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-INFO: Failed! We are out of probe options. No video devices to enumerate.\n");
			return(NULL);
		}		
	}
	
	if (PSYCH_SYSTEM == PSYCH_OSX) {
		if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to probe osxvideosrc as video source...\n");
		videosource = gst_element_factory_make("osxvideosrc", "ptb_videosource");
		sprintf(class_str, "OSXQuicktimeSequenceGrabber");
		deviceClass = 0;
		
		if (!videosource) {
			if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-INFO: Failed! We are out of probe options. No video devices to enumerate.\n");
			return(NULL);
		}
	}
	
	// Generate property probe for videosource:
	// Make sure videosource implements property probe interface to avoid useless warning clutter,
	// unless on Linux, where this query actually causes an assertion on some GStreamer versions...
	if ((PSYCH_SYSTEM == PSYCH_LINUX) || gst_element_implements_interface(videosource, GST_TYPE_PROPERTY_PROBE)) {
		probe = GST_PROPERTY_PROBE(videosource);
	}
	else {
		probe = NULL;
	}
	
	if (probe) {
		// Retrieve array of available video input device names, aka
		// "device-name" property:
		if ((PSYCH_SYSTEM != PSYCH_LINUX) && g_object_class_find_property(G_OBJECT_GET_CLASS(probe), "device-name")) {
			viddevs = gst_property_probe_probe_and_get_values_name(probe, "device-name");
		}

		if (!viddevs) {
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Failed to probe 'device-name' property. Retrying with 'device' ...\n");
			viddevs = gst_property_probe_probe_and_get_values_name(probe, "device");
		}

		if (viddevs) {
			// Assign count of available devices:
			n = viddevs->n_values;
			
			// Does calling code just want to query device name for a given
			// deviceIndex?
			if (deviceIndex >= 0) {
				// Yes: Return device name for that index:
				deviceIndex = deviceIndex % 10000;
				
				if (deviceIndex < n) {				
					// Get deviceIndex'th entry:
					dev = g_value_array_get_nth(viddevs, deviceIndex);
					device_name = strdup((const char*) g_value_get_string(dev));
				}
			}
			else {
				// No: Code wants us to return struct array with all enumerated
				// devices to userspace:
				
				// Create output struct array with n output slots:
				PsychAllocOutStructArray(outPos, TRUE, n, 8, FieldNames, &devs);
				
				// Iterate all available devices:
				for(i = 0; i < n; i++) {
					dev = g_value_array_get_nth(viddevs, i);
					sprintf(port_str, "%s", (const char*) g_value_get_string(dev));

					if (PSYCH_SYSTEM == PSYCH_LINUX) {
						g_object_set(G_OBJECT(videosource), "device", port_str, NULL);
					} else {
						g_object_set(G_OBJECT(videosource), "device-name", port_str, NULL);
					}
					
					deviceInput = i;
					PsychSetStructArrayDoubleElement("DeviceIndex", i, deviceClass * 10000 + deviceInput, devs);
					PsychSetStructArrayDoubleElement("ClassIndex", i, deviceClass, devs);
					PsychSetStructArrayDoubleElement("InputIndex", i, deviceInput, devs);
					PsychSetStructArrayStringElement("ClassName", i, class_str, devs);
					PsychSetStructArrayStringElement("InputHandle", i, port_str, devs);

					// Query and assign device specific parameters:
					pstring = NULL;
					if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "device")) {
						g_object_get(G_OBJECT(videosource), "device", &pstring, NULL);
					}

					if (pstring) {
						PsychSetStructArrayStringElement("Device", i, pstring, devs);
						g_free(pstring);
					}
					else {
						PsychSetStructArrayStringElement("Device", i, port_str, devs);
					}
					
					pstring = NULL;
					if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "device-path")) {
						g_object_get(G_OBJECT(videosource), "device-path", &pstring, NULL);
					}

					if (pstring) {
						PsychSetStructArrayStringElement("DevicePath", i, pstring, devs);
						g_free(pstring);
					}
					else {
						PsychSetStructArrayStringElement("DevicePath", i, port_str, devs);
					}
					
					pstring = NULL;
					if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "device-name")) {
						g_object_get(G_OBJECT(videosource), "device-name", &pstring, NULL);
					}

					if (pstring) {
						PsychSetStructArrayStringElement("DeviceName", i, pstring, devs);
						g_free(pstring);
					}
					else {
						PsychSetStructArrayStringElement("DeviceName", i, port_str, devs);
					}
					
					pstring = NULL;

					// Probe next device...
				}
			}
			
			g_value_array_free(viddevs);
		}
		else {
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Video plugin '%s' doesn't provide any video devices to enumerate.\n", class_str);
		}
	}
	else {
		if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Video plugin '%s' doesn't support probing. No video devices to enumerate.\n", class_str);
	}

	// Release videosource:
	gst_element_set_state(videosource, GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(videosource));
	
	// Done. Return device name if assigned:
	return(device_name);
}

/* CHECKED TODO
*      PsychGSOpenVideoCaptureDevice() -- Create a video capture object.
*
*      This function tries to open and initialize a connection to a camera
*      and returns the associated captureHandle for it.
*
*      slotid = Number of slot in vidcapRecordBANK[] array to use for this camera.
*      win = Pointer to window record of associated onscreen window.
*      deviceIndex = Index of the grabber device. (Currently ignored)
*      capturehandle = handle to the new capture object.
*      capturerectangle = If non-NULL a ptr to a PsychRectangle which contains the ROI for capture. [0 0 w h] selects resolution w x h on device.
*      reqdepth = Number of layers for captured output textures. (0=Don't care, 1=LUMINANCE8, 2=LUMINANCE8_ALPHA8, 3=RGB8, 4=RGBA8, 5=YCBCR)
*      num_dmabuffers = Number of buffers in the ringbuffer queue (e.g., DMA buffers) - This is OS specific. Zero = Don't care.
*      allow_lowperf_fallback = If set to 1 then PTB can use a slower, low-performance fallback path to get nasty devices working.
*      targetmoviefilename and recordingflags are currently ignored, they would refer to video harddics recording capabilities.
*/
psych_bool PsychGSOpenVideoCaptureDevice(int slotid, PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle,
								   int reqdepth, int num_dmabuffers, int allow_lowperf_fallback, char* targetmoviefilename, unsigned int recordingflags)
{
	GstCaps         *colorcaps, *filter_caps;
	GstElement		*camera = NULL;
	GMainLoop		*VideoContext = NULL;
	GstBus			*bus = NULL;
	GstFormat		fmt;
	GstElement              *videosink;
	GstElement              *videosource;
	gint64			length_format;
	GstPad			*pad, *peerpad;
	const GstCaps		*caps;
	GstStructure		*str;
	gint			width, height;
	gint			rate1, rate2;
	gint			twidth, theight;
	int			i;
	psych_bool		trueValue = TRUE;
	psych_bool		printErrors = TRUE;

	PsychVidcapRecordType	*capdev = NULL;
	char			config[1000];
	char			tmpstr[1000];
	char			msgerr[10000];
	char			errdesc[1000];
	char			*tmpdevname = NULL;
	char			device_name[1000];
	gchar                   *pstring = NULL; 

	config[0] = 0;
	tmpstr[0] = 0;
	device_name[0] = 0;
	
	// Init capturehandle to none:
	*capturehandle = -1;
	
	// Make sure GStreamer is ready:
	PsychGSCheckInit("videocapture");
	
	// Map deviceIndex of requested video source to device name:
	if (deviceIndex >= 0) {
		// Get device name for given deviceIndex from video device
		// enumeration (or NULL if no such device):
		tmpdevname = PsychGSEnumerateVideoSources(-1, deviceIndex);
		if (NULL == tmpdevname) {
			printf("PTB-ERROR: There isn't any video capture device available for provided deviceIndex %i.\n", deviceIndex);
			PsychErrorExitMsg(PsychError_user, "Invalid deviceIndex provided. No such video source. Aborted.");
		}
		
		// Assign name:
		sprintf(device_name, "%s", tmpdevname);
		free(tmpdevname);
		tmpdevname = NULL;
		
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

    // ROI rectangle specified?
    if (capturerectangle) {
	if ((capturerectangle[kPsychLeft] == 0) && (capturerectangle[kPsychTop] == 0)) {
		// roi = [0 0 w h] --> Specs a target capture resolution.
		// Extract wanted width and height and use it as target capture resolution:
		twidth  = (int) PsychGetWidthFromRect(capturerectangle);
		theight = (int) PsychGetHeightFromRect(capturerectangle);
	} else {
		// roi = [l t r b] --> Specs a ROI to crop out of full res capture.
		twidth  = -1;
		theight = -1;
	}
    } else {
		twidth  = -1;
		theight = -1;
    }
    
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

    // Prepare error message in case its needed below:
    sprintf(msgerr, "PTB-ERROR: Opening the %i. camera (deviceIndex=%i) failed!\n", deviceIndex + 1, deviceIndex);

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
		
		// Assign a specific input video source name on Linux. On other operating systems,
		// the user has no choice but to use the auto-assigned default capture device:
		if (PSYCH_SYSTEM == PSYCH_LINUX) sprintf(config, "v4l2://%s", device_name);
		
		if (PsychPrefStateGet_Verbosity() > 1) {
			printf("PTB-WARNING: Could not use GStreamer 'camerabin' plugin for videocapture. Will use less powerful fallback path.\n");
		}
    }
	
    // Pipeline creation failed?
    if (NULL == camera) PsychErrorExitMsg(PsychError_user, "Failed to create video capture pipeline! Aborted.");

    // Enforce use of the system clock for this pipeline instead of leaving it to the pipeline
    // to choose a proper clock automatically:
    // TODO FIXME: To use or not to use? gst_pipeline_use_clock(camera, gst_system_clock_obtain());

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
		
		// Linux specific setup path:
		if (PSYCH_SYSTEM == PSYCH_LINUX) {

			// Try Video4Linux-II camera source first:
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach v4l2camsrc as video source...\n");
			videosource = gst_element_factory_make("v4l2camsrc", "ptb_videosource");

			// Fallback to standard Video4Linux-II source if neccessary:
			if (!videosource) {
				if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Failed. Trying to attach v4l2src as video source...\n");
				videosource = gst_element_factory_make("v4l2src", "ptb_videosource");
			}
			
			if (videosource) {
				// Ok, we have a suitable video source object. Assign video input device to it, if any specified:
				if (config[0] != 0) {
					if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device '%s' as video input.\n", config);
					g_object_set(G_OBJECT(videosource), "device", config, NULL);
				}
				else {
					if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach default Linux video device as video input.\n");
				}
			}

			// Fallback to autovideosrc if everything else fails:
			if (!videosource) {
				if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Failed. Trying to attach autovideosrc as video source with auto-detected video input device...\n");
				videosource = gst_element_factory_make("autovideosrc", "ptb_videosource");
			}
			
			if (!videosource) {
				if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed! We are out of options and will probably fail soon.\n");
				PsychErrorExitMsg(PsychError_system, "GStreamer failed to find a suitable video source! Game over.");
			}
		} // End of Linux Video source creation.

		// MS-Windows specific setup path:
		if (PSYCH_SYSTEM == PSYCH_WINDOWS) {
			if (deviceIndex !=-10) {
				// Non-Firewire video source selected:
				
				if ((deviceIndex >= 10000) || (deviceIndex == -1) || (deviceIndex == -2)) {
					// First try Kernel-Streaming based video source for low-latency capture:
					if (deviceIndex >= 10000) deviceIndex = deviceIndex - 10000;
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
					else {
						// Device index >= 0 selected: Select video source with corresponding index:
						if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device with index %i as video input.\n", deviceIndex);
						g_object_set(G_OBJECT(videosource), "device-index", deviceIndex, NULL);
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
			} else {
				// TODO FIXME: Firewire video source on Windows:
				if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach firewire as video source...\n");
				PsychErrorExitMsg(PsychError_unimplemented, "Firewire video capture not yet implemented.");
				videosource = gst_element_factory_make("dshowvideosrc", "ptb_videosource");
			}
			
			// Still no video source available?
			if (!videosource) {
				if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed! We are out of options and will probably fail soon.\n");
				PsychErrorExitMsg(PsychError_system, "GStreamer failed to find a suitable video source! Game over.");
			}
		} // End of MS-Windows Video source creation.
		
		// MacOS/X specific setup path:
		if (PSYCH_SYSTEM == PSYCH_OSX) {
			if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach osxvideosrc as video source...\n");
			videosource = gst_element_factory_make("osxvideosrc", "ptb_videosource");

			if (!videosource) {
				if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed! We are out of options and will probably fail soon.\n");
				PsychErrorExitMsg(PsychError_system, "GStreamer failed to find a suitable video source! Game over.");
			}
			
			if (deviceIndex < 0) {
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
						// DirectShow device path provided:
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
			else {
				// Human friendly device name provided:
				if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-INFO: Trying to attach video device '%s' as video input.\n", config);
				g_object_set(G_OBJECT(videosource), "device-name", config, NULL);				
			}
		} // End of OS/X Video source creation.
		
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

	    printf("PTB-INFO: Video and audio recording into file [%s] enabled.\n", targetmoviefilename);
	    g_object_set(G_OBJECT(camera), "filename", targetmoviefilename, NULL);
	    g_object_set(G_OBJECT(camera), "mode", 1, NULL);
	    capdev->recording_active = TRUE;
    } else {
	    capdev->recording_active = FALSE;
    }

    // Assign a fakesink named "ptbsink0" as destination video-sink for
    // all video content. This allows us to get hold of the video frame buffers for
    // converting them into PTB OpenGL textures:
    videosink = gst_element_factory_make ("appsink", "ptbsink0");
    if (!videosink) {
	printf("PTB-ERROR: Failed to create video-sink appsink ptbsink!\n");
	PsychGSProcessVideoContext(vidcapRecordBANK[slotid].VideoContext, TRUE);
	PsychErrorExitMsg(PsychError_system, "Opening the videocapture device failed. Reason hopefully given above.");
    };

    vidcapRecordBANK[slotid].videosink = videosink;

    // Our OpenGL texture creation routine needs GL_BGRA8 data in G_UNSIGNED_8_8_8_8_REV
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

    // Assign our special appsink 'videosink' as video-sink of the pipeline:
    if (!usecamerabin) {
	g_object_set(G_OBJECT(camera), "video-sink", videosink, NULL);
    } else {
	    g_object_set(G_OBJECT(camera), "viewfinder-sink", videosink, NULL);

	    // Start video recording if requested:
	    if (!capdev->recording_active) {
		    g_object_set(G_OBJECT(camera),
				 "flags", 1+2+4,
				 "filter-caps", colorcaps,
				 NULL);
	    } else {
		    filter_caps = gst_caps_new_simple("video/x-raw-yuv",
						      "format", GST_TYPE_FOURCC,
						      GST_MAKE_FOURCC('Y', '4', '2', 'B'), NULL);
		    
		    g_object_set(G_OBJECT(camera),
				 "flags", 1+2+4,
				 "filter-caps", filter_caps,
				 NULL);

		    gst_caps_unref(filter_caps);
	    }

	    // Specific capture resolution requested?
	    if ((twidth > 0) && (theight > 0)) {
		// Yes: Request it:
		g_object_set(G_OBJECT(camera),
			     "video-capture-width", twidth,
			     "video-capture-height", theight,
			     NULL);
	    }
	    // No: Non-Default ROI specified?
	    else if (capturerectangle && (twidth == -1) && (theight == - 1)) {
		// Yes: Operate at default video source resolution,
		// i.e., leave it at its defaults. Use cropping to
		// get our ROI.
		printf("PTB-INFO: Selection of arbitrary video ROI's not yet implemented. Using full resolution.\n");
	    }
    }

    gst_caps_unref(colorcaps);

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

    // Pause the pipeline:
    if (!PsychVideoPipelineSetState(camera, GST_STATE_READY, 30.0)) {
	    PsychGSProcessVideoContext(vidcapRecordBANK[slotid].VideoContext, TRUE);
	    PsychErrorExitMsg(PsychError_user, "In OpenVideoCapture: Opening the video capture device failed during pipeline zero -> ready. Reason given above.");
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

    // Preload / Preroll the pipeline:
    if (!PsychVideoPipelineSetState(camera, GST_STATE_PLAYING, 30.0)) {
	    PsychGSProcessVideoContext(vidcapRecordBANK[slotid].VideoContext, TRUE);
	    PsychErrorExitMsg(PsychError_user, "In OpenVideoCapture: Opening the video capture device failed during pipeline preroll ready->playing. Reason given above.");
    }
    
    // Pause the pipeline again:
    if (!PsychVideoPipelineSetState(camera, GST_STATE_PAUSED, 30.0)) {
	    PsychGSProcessVideoContext(vidcapRecordBANK[slotid].VideoContext, TRUE);
	    PsychErrorExitMsg(PsychError_user, "In OpenVideoCapture: Opening the video capture device failed during preroll playing -> pause. Reason given above.");
    }

    gst_element_set_base_time(camera, GST_CLOCK_TIME_NONE);

    // Query number of available video and audio channels on capture device:
    if (!usecamerabin) {
	    g_object_get(G_OBJECT(camera),
		         "n-video", &vidcapRecordBANK[slotid].nrVideoTracks,
		         "n-audio", &vidcapRecordBANK[slotid].nrAudioTracks,
			 NULL);
    } else {
	vidcapRecordBANK[slotid].nrVideoTracks = 1;
    }

    // We need a valid onscreen window handle for real video playback:
    if ((NULL == win) && (vidcapRecordBANK[slotid].nrVideoTracks > 0)) {
	    if (printErrors) {
		    PsychErrorExitMsg(PsychError_user, "No windowPtr to an onscreen window provided. Must do so for sources with video channels!");
	    } else return(FALSE);
    }
 
    PsychGSProcessVideoContext(vidcapRecordBANK[slotid].VideoContext, FALSE);

    // Assign harmless initial settings for fps and frame size:
    rate1 = 0;
    rate2 = 1;
    width = height = 0;

    // Videotrack available?
    if (vidcapRecordBANK[slotid].nrVideoTracks > 0) {
	// Yes: Query video frame size and framerate of device:
	peerpad = gst_pad_get_peer(pad);
	caps = gst_pad_get_negotiated_caps(peerpad);
	if (caps) {
		str=gst_caps_get_structure(caps,0);

		/* Get some data about the frame */
		gst_structure_get_int(str,"width",&width);
		gst_structure_get_int(str,"height",&height);
		gst_structure_get_fraction(str, "framerate", &rate1, &rate2);
		// printf("vt = %i w = %i h = %i fps = %f\n", vidcapRecordBANK[slotid].nrVideoTracks, width, height, rate1/rate2);
		gst_caps_unref(caps);
	} else {
		printf("PTB-DEBUG: No frame info available after preroll! Expect trouble!!\n");	
	}
    }

    // Release the pad:
    gst_object_unref(pad);

    // Assign new record in videobank:
    vidcapRecordBANK[slotid].camera = camera;
    vidcapRecordBANK[slotid].frameAvail = 0;

    // Our camera should be ready: Assign final handle.
    *capturehandle = slotid;
	
    // Increase counter of open capture devices:
    numCaptureRecords++;
    
    // Set image size:
    capdev->width = width;
    capdev->height = height;

    // Create capture ROI corresponding to width and height of video image:
    PsychMakeRect(capdev->roirect, 0, 0, capdev->width, capdev->height);

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
    } else {
	    g_object_get (G_OBJECT(camera),
			  "video-source", &videosource,
			  NULL);
    }

	if (videosource) {
		pstring = NULL; 
		if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "device")) {
			g_object_get(G_OBJECT(videosource), "device", &pstring, NULL);
			if (pstring) {
				printf("PTB-INFO: Camera device name is %s.\n", pstring);
				g_free(pstring); pstring = NULL;
			}
		}
		
		if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "device-path")) {
			g_object_get(G_OBJECT(videosource), "device-path", &pstring, NULL);
			if (pstring) {
				printf("PTB-INFO: Camera device-path is %s.\n", pstring);
				g_free(pstring); pstring = NULL;
			}
		}
		
		if (g_object_class_find_property(G_OBJECT_GET_CLASS(videosource), "device-name")) {
			g_object_get(G_OBJECT(videosource), "device-name", &pstring, NULL);
			if (pstring) {
				printf("PTB-INFO: Camera friendly device-name is %s.\n", pstring);
				g_free(pstring); pstring = NULL;
			}
		}

		// Get the pad from the src pad of the source for probing width x height
		// of video frames and nominal framerate of video source:	
		pad = gst_element_get_pad(videosource, "src");
		
		// Videotrack available?
		if (vidcapRecordBANK[slotid].nrVideoTracks > 0) {
			// Yes: Query video frame size and framerate of device:
			peerpad = gst_pad_get_peer(pad);
			caps = gst_pad_get_negotiated_caps(peerpad);
			if (caps) {
				str=gst_caps_get_structure(caps,0);
				
				/* Get some data about the frame */
				gst_structure_get_int(str,"width",&width);
				gst_structure_get_int(str,"height",&height);
				gst_structure_get_fraction(str, "framerate", &rate1, &rate2);
				// printf("vt = %i w = %i h = %i fps = %f\n", vidcapRecordBANK[slotid].nrVideoTracks, width, height, rate1/rate2);
				gst_caps_unref(caps);
			} else {
				printf("PTB-DEBUG: No frame info for video source available after preroll! Expect trouble!!\n");	
			}
		}
		
		// g_object_get(G_OBJECT(camera), "video-source-caps", &caps, NULL);
		
		// Release the pad:
		gst_object_unref(pad);		
	}

    printf("PTB-INFO: Camera successfully opened... [Native width x height = %i x %i]\n", width, height);

    return(TRUE);
}

/* CHECKED
*  PsychGSVideoCaptureRate() - Start- and stop video capture.
*
*  capturehandle = Grabber to start-/stop.
*  playbackrate = zero == Stop capture, non-zero == Capture
*  dropframes = 0 - Always deliver oldest frame in DMA ringbuffer. 1 - Always deliver newest frame.
*               --> 1 == drop frames in ringbuffer if behind -- low-latency capture.
*  startattime = Deadline (in system time) for which to wait before real start of capture.
*  Returns Number of dropped frames during capture.
*/
int PsychGSVideoCaptureRate(int capturehandle, double capturerate, int dropframes, double* startattime)
{
	GstElement		*camera = NULL;
	GstBuffer               *videoBuffer = NULL;
	GValue                  fRate = { 0, };
	int dropped = 0;
	float framerate = 0;

	// Retrieve device record for handle:
	PsychVidcapRecordType* capdev = PsychGetGSVidcapRecord(capturehandle);
	camera = capdev->camera;

	// Make sure GStreamer is ready:
	PsychGSCheckInit("videocapture");

	// Start- or stop capture?
	if (capturerate > 0) {
		// Start capture:
		if (capdev->grabber_active) PsychErrorExitMsg(PsychError_user, "You tried to start video capture, but capture is already started!");
		
		// Reset statistics:
		capdev->last_pts = -1.0;
		capdev->nr_droppedframes = 0;
		capdev->frame_ready = 0;
		
		// Framedropping is not supported by libGStreamer, so we implement it ourselves.
		// Store the 'dropframes' flag in our capdev struct, so the PsychGSGetTextureFromCapture()
		// knows how to handle this:
		capdev->dropframes = (dropframes > 0) ? 1 : 0;

		// Set volume and mute state for audio:
		//g_object_set(G_OBJECT(camera), "mute", (soundvolume <= 0) ? TRUE : FALSE, NULL);
		//g_object_set(G_OBJECT(camera), "volume", soundvolume, NULL);
		
		// Set playback rate:
		gst_element_seek(camera, capturerate, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE, GST_SEEK_TYPE_NONE, 0, GST_SEEK_TYPE_NONE, 0);
		capdev->last_pts = -1.0;
		capdev->nr_droppedframes = 0;
		capdev->frameAvail = 0;
		capdev->preRollAvail = 0;

		if (usecamerabin) {
			g_value_init(&fRate, GST_TYPE_FRACTION);
			gst_value_set_fraction(&fRate, (int)(capturerate + 0.5), 1);
			g_object_set(G_OBJECT(camera),
				     "video-capture-framerate", fRate,
				     NULL);
		}

		// Wait until start deadline reached:
		if (*startattime != 0) PsychWaitUntilSeconds(*startattime);
		
		// Start DMA driven isochronous data transfer:
		if (PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Starting capture...\n"); fflush(NULL);

		// Start the video capture for this camera.
		gst_element_set_start_time(camera, GST_CLOCK_TIME_NONE);
		if (!PsychVideoPipelineSetState(camera, GST_STATE_PLAYING, 10.0)) {
			// Failed!
			PsychErrorExitMsg(PsychError_user, "Failure in pipeline transition paused -> playing - Start of video capture failed!");
		}
		
		// Start video recording if requested:
		if (usecamerabin && capdev->recording_active) g_signal_emit_by_name (camera, "capture-start", 0);

		// Wait for real start of capture, i.e., arrival of 1st captured
		// video buffer:
		PsychLockMutex(&capdev->mutex);

		// This wait is just here to clear potentially pending conditions on
		// MS-Windows. Therefore only wait for 1.1 msecs at most:
		PsychTimedWaitCondition(&capdev->condition, &capdev->mutex, 0.0011);

		while (capdev->frameAvail == 0) {
			if (PsychPrefStateGet_Verbosity() > 5) {
				printf("PTB-DEBUG: Waiting for real start: fA = %i pA = %i fps=%f\n", capdev->frameAvail, capdev->preRollAvail, capdev->fps);
				fflush(NULL);
			}
			PsychTimedWaitCondition(&capdev->condition, &capdev->mutex, 10.0);
		}

		// Ok, capture is now started:
		capdev->grabber_active = 1;

		PsychUnlockMutex(&capdev->mutex);

		// Record real start time:
		PsychGetAdjustedPrecisionTimerSeconds(startattime);

		// Some processing time for message dispatch:
		PsychGSProcessVideoContext(capdev->VideoContext, FALSE);

		if(PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Capture engine fully running...\n"); fflush(NULL);
		
		
		/* // Allocate conversion buffer if needed for YUV->RGB conversions.
		if (capdev->pixeldepth == -1) {
			// Not used at the moment!!
			// Software conversion of YUV -> RGB needed. Allocate a proper scratch-buffer:
			capdev->scratchbuffer = malloc(capdev->width * capdev->height * 3);
		}
		*/

		if(PsychPrefStateGet_Verbosity()>1) {
			printf("PTB-INFO: Capture started on device %i - Width x Height = %i x %i - Framerate: %f fps.\n",
			       capturehandle, capdev->width, capdev->height, capdev->fps);
		}
	}
	else {
		// Stop capture:
		if (capdev->grabber_active) {
			// Stop video recording if requested:
			if (usecamerabin && capdev->recording_active) g_signal_emit_by_name (camera, "capture-stop", 0);

			// Stop isochronous data transfer from camera:
			if (!PsychVideoPipelineSetState(camera, GST_STATE_PAUSED, 10.0)) {
				PsychErrorExitMsg(PsychError_user, "Unable to stop video transfer on camera!");
			}

			PsychGSProcessVideoContext(capdev->VideoContext, FALSE);

			// Drain any queued buffers:
			while (!gst_app_sink_is_eos(GST_APP_SINK(capdev->videosink)) && (capdev->frameAvail > 0)) {
				capdev->frameAvail--;
				videoBuffer = gst_app_sink_pull_buffer(GST_APP_SINK(capdev->videosink));
				gst_buffer_unref(videoBuffer);
				videoBuffer = NULL;
			};

			// Ok, capture is now stopped.
			capdev->frame_ready = 0;
			capdev->grabber_active = 0;
			
			if (capdev->scratchbuffer) {
				// Release scratch-buffer:
				free(capdev->scratchbuffer);
				capdev->scratchbuffer = NULL;
			}

			if(PsychPrefStateGet_Verbosity()>1){
				// Output count of dropped frames:
				if ((dropped=capdev->nr_droppedframes) > 0) {
					printf("PTB-INFO: Video capture dropped %i frames on device %i to keep capture running in sync with realtime.\n", dropped, capturehandle); 
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
    GstElement			*camera;
    GstBuffer                   *videoBuffer = NULL;
    gint64		        bufferIndex;
    double                      deltaT = 0;
    GstEvent                    *event;

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

    if (!PsychIsOnscreenWindow(win)) {
        PsychErrorExitMsg(PsychError_user, "Need onscreen window ptr!!!");
    }
	
    // Retrieve device record for handle:
    capdev = PsychGetGSVidcapRecord(capturehandle);

    // Allow context task to do its internal bookkeeping and cleanup work:
    PsychGSProcessVideoContext(capdev->VideoContext, FALSE);

    // If this is a pure audio capture with no video channels, we always return failed,
    // as those certainly don't have movie frames associated.
    if (capdev->nrVideoTracks == 0) return((checkForImage) ? -1 : FALSE);

    // Compute width and height for later creation of textures etc. Need to do this here,
    // so we can return the values for raw data retrieval:
    w=capdev->width;
    h=capdev->height;

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
    // a movie recording engine, ie., grant processing time to it. Our GStreamer engine doesn't
    // support movie recording, so this is a no-op:
    if (checkForImage == 4) return(0);
	
    // Should we just check for new image?
    if (checkForImage) {
	    // Reset current dropped count to zero:
	    capdev->current_dropped = 0;
	    
	    if (capdev->grabber_active == 0) {
		    // Grabber stopped. We'll never get a new image:
		    return(-2);
	    }

	    PsychLockMutex(&capdev->mutex);
	    if (!capdev->frameAvail) {
		    // No new frame available yet:

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
		    if ((capdev->grabber_active) && !capdev->frameAvail) {
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

    if ((capdev->grabber_active) && !capdev->frameAvail) {
		// No new frame available. Perform a blocking wait:
		PsychTimedWaitCondition(&capdev->condition, &capdev->mutex, 10.0);
		
		// Recheck:
		if ((capdev->grabber_active) && !capdev->frameAvail) {
			// Game over! Wait timed out after 10 secs.
			PsychUnlockMutex(&capdev->mutex);
			if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: No new video frame received after timeout of 10 seconds! Something's wrong. Aborting fetch.\n");
			return(-1);
		}
		
		// At this point we should have at least one frame available.
        //printf("PTB-DEBUG: After blocking fetch start %d\n", capdev->frameAvail);
    }
	
    // We're here with at least one frame available and the mutex lock held.

    // One less frame available after our fetch:
    capdev->frameAvail--;
    if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: Pulling from videosink, %d buffers avail...\n", capdev->frameAvail);
    
    // Clamp frameAvail to queue lengths:
    if ((int) gst_app_sink_get_max_buffers(GST_APP_SINK(capdev->videosink)) < capdev->frameAvail) {
	    capdev->frameAvail = gst_app_sink_get_max_buffers(GST_APP_SINK(capdev->videosink));
    }
    if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: Post-Pulling from videosink, %d buffers avail...\n", capdev->frameAvail);
    
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
	    capdev->current_pts = (double) GST_BUFFER_TIMESTAMP(videoBuffer) / (double) 1e9;

        // Apply corrective offset for GStreamer clock base zero point:
        capdev->current_pts+= gs_startupTime;

	    deltaT = 0.0;
	    if (GST_CLOCK_TIME_IS_VALID(GST_BUFFER_DURATION(videoBuffer)))
		    deltaT = (double) GST_BUFFER_DURATION(videoBuffer) / (double) 1e9;

	    if (PsychPrefStateGet_Verbosity() > 5) {
		    PsychProbeBufferProps(videoBuffer, NULL, NULL, &capdev->fps);
		    printf("Bufferprobe: newfps = %f altfps = %f\n", capdev->fps, (float) ((deltaT > 0) ? 1.0 / deltaT : 0.0));
	    }

	    bufferIndex = GST_BUFFER_OFFSET(videoBuffer);
    } else {
	    if (PsychPrefStateGet_Verbosity()>0) printf("PTB-ERROR: No new video frame received in gst_app_sink_pull_buffer! Something's wrong. Aborting fetch.\n");
	    return(-1);
    }
    if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: ...done.\n");

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

		memcpy(capdev->scratchbuffer, input_image, capdev->width * capdev->height * bpp);
		
		// Ok, at this point we should have a RGB8 texture image ready in scratch_buffer.
		// Set scratch buffer as our new image source for all further processing:
		input_image = (unsigned char*) capdev->scratchbuffer;
    }
	
    // Only setup if really a texture is requested (non-benchmarking mode):
    if (out_texture) {
	    // Activate OpenGL co	ntext of target window:
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
		//		ar2VideoDispOption();
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
		PsychCopyOutCharArg(1, FALSE, "Unknown Model");
		return(0);
	}
	
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
