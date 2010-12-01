/*
	PsychSourceGL/Source/Common/Screen/PsychMovieSupportGStreamer.c
	
	PLATFORMS:	All with PTB_USE_GSTREAMER defined.

	AUTHORS:

	mario.kleiner@tuebingen.mpg.de		mk	Mario Kleiner

	HISTORY:

        28.11.2010    mk      Wrote it.

	DESCRIPTION:
	
	Psychtoolbox functions for dealing with movies. This is the operating system independent
	version which uses the GStreamer media framework.

	These PsychGSxxx functions are called from the dispatcher in
	Common/Screen/PsychMovieSupport.[hc].

	TODO:

		- Fix frame-based seeking: Time base seeking works well, frame based not so much.
		- Check if the 'drop' property + max_queue property of the appsink could be used
		  in a creative way to synchronize 1st played frame/sound with 1st texture fetch.
		- dto. other uses settings for queue length.
		- Make auto-rewind and reverse playback more robust.
		- Preload into RAM - implement if possible, although preroll seems to be sufficient.
		- Avoid spin-wait polling from calling high-level code when waiting for new frames.
		- Check check check exact timing, precision, robustness, performance...

*/

#ifdef PTB_USE_GSTREAMER

#include "Screen.h"
#include <glib.h>
#include "PsychMovieSupportGStreamer.h"
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

static const psych_bool oldstyle = FALSE;

#define PSYCH_MAX_MOVIES 100

typedef struct {
    unsigned char asyncstate;
    char* moviename;
    PsychWindowRecordType windowRecord;
    int moviehandle;
    double preloadSecs;	
#if PSYCH_SYSTEM == PSYCH_OSX
    pthread_t pid;
#endif
} asyncmovieinfo;

typedef struct {
    psych_mutex		mutex;
    psych_condition     condition;
    double		pts;
    GstElement		*theMovie;
    GMainLoop		*MovieContext;
    GstElement          *videosink;
    unsigned char	*imageBuffer;
    int			frameAvail;
    int                 preRollAvail;
    double		rate;
    int			loopflag;
    double		movieduration;
    int			nrframes;
    double		fps;
    int			width;
    int			height;
    double		last_pts;
    int			nr_droppedframes;
    int                 nrAudioTracks;
    int                 nrVideoTracks;
    char                movieLocation[FILENAME_MAX];
} PsychMovieRecordType;

static PsychMovieRecordType movieRecordBANK[PSYCH_MAX_MOVIES];
static int numMovieRecords = 0;
static psych_bool firsttime = TRUE;

/*
 *     PsychGSMovieInit() -- Initialize movie subsystem.
 *     This routine is called by Screen's RegisterProject.c PsychModuleInit()
 *     routine at Screen load-time. It clears out the movieRecordBANK to
 *     bring the subsystem into a clean initial state.
 */
void PsychGSMovieInit(void)
{
    // Initialize movieRecordBANK with NULL-entries:
    int i;
    for (i=0; i < PSYCH_MAX_MOVIES; i++) {
	memset(&movieRecordBANK[i], 0, sizeof(PsychMovieRecordType));
    }    
    numMovieRecords = 0;

    // Initialize GLib's threading system early:
    g_thread_init(NULL);

    return;
}

/*
 *      PsychGSAsyncCreateMovie() -- Open a movie file in the background.
 *
 *      This function is called by SCREENOpenMovie as main-function of
 *      a new Posix-Thread for background movie loading. It simply calls
 *      PsychGSCreateMovie(), waiting for it to return a new moviehandle.
 *      Then it returns those info and terminates.
 *      -> By calling PsychGSCreateMovie from the run-function of a dedicated
 *      Posix-Thread which runs independent of the main Matlab/PTB Thread with
 *      non-realtime priority, we can do the work of opening a movie
 *      in the background, hopefully not affecting the timing of the main PTB
 *      thread too much.
 */
void* PsychGSAsyncCreateMovie(void* mi)
{
    struct sched_param sp;
    int rc;
    
    // Get a pointer to the info-struct:
    asyncmovieinfo* movieinfo = (asyncmovieinfo*) mi;

    // The special value -1000 tells PsychGSCreateMovie to not output any error-
    // messages as this could easily crash Matlab.
    int mymoviehandle=-1000;

    // Reduce our scheduling priority to the minimum value of zero, so
    // we do not interfere too much with the PTB main thread:
    sp.sched_priority = 0;
    if ((rc=pthread_setschedparam(pthread_self(), SCHED_OTHER, &sp))!=0) {
        printf("PTB-ERROR: In PsychGSAsyncCreateMovie(): PTHREAD ERROR %i ...", rc);
    }
    
    // Execute our normal OpenMovie function: This does the hard work:
    PsychGSCreateMovie(&(movieinfo->windowRecord), movieinfo->moviename, movieinfo->preloadSecs, &mymoviehandle);
	
    // Ok, either we have a moviehandle to a valid movie, or we failed, which would
    // be signalled to the calling function via some negative moviehandle:
    movieinfo->moviehandle = mymoviehandle; // Return moviehandle.
    movieinfo->asyncstate = 2; // Set state to "Completed"
    
    // Exit from the routine. This will automatically terminate our Posix-Thread.
    return(NULL);
}

int PsychGSGetMovieCount(void) {
	return(numMovieRecords);
}

/* Perform one context loop iteration (for bus message handling) if doWait == false,
 * or two seconds worth of iterations if doWait == true. This drives the message-bus
 * callback, so needs to be performed to get any error reporting etc.
 */
int PsychGSProcessMovieContext(GMainLoop *loop, psych_bool doWait)
{
	double tdeadline, tnow;
	PsychGetAdjustedPrecisionTimerSeconds(&tdeadline);
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
static psych_bool PsychMoviePipelineSetState(GstElement* theMovie, GstState state, double timeoutSecs)
{
    GstState			state_pending;
    GstStateChangeReturn	rcstate;

    gst_element_set_state(theMovie, state);

    // Non-Blocking, async?
    if (timeoutSecs < 0) return(TRUE);
 
    // Wait for up to timeoutSecs for state change to complete or fail:
    rcstate = gst_element_get_state(theMovie, &state, &state_pending, timeoutSecs * 1e9);
    switch(rcstate) {
	case GST_STATE_CHANGE_SUCCESS:
		//printf("PTB-DEBUG: Preroll completed with GST_STATE_CHANGE_SUCCESS.\n");
	break;
	case GST_STATE_CHANGE_ASYNC:
		printf("PTB-INFO: Preroll in progress with GST_STATE_CHANGE_ASYNC.\n");
	break;
	case GST_STATE_CHANGE_FAILURE:
		printf("PTB-ERROR: Preroll failed with GST_STATE_CHANGE_FAILURE!\n");
		return(FALSE);
	break;

	default:
		printf("PTB-ERROR: Unknown state-change result in preroll.\n");
		return(FALSE);
    }

    return(TRUE);
}

/* Receive messages from the playback pipeline message bus and handle them: */
static gboolean PsychMovieBusCallback(GstBus *bus, GstMessage *msg, gpointer dataptr)
{
  PsychMovieRecordType* movie = (PsychMovieRecordType*) dataptr;

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
	//printf("PTB-DEBUG: Message EOS received.\n");
    break;

    case GST_MESSAGE_WARNING: {
      gchar  *debug;
      GError *error;

      gst_message_parse_warning(msg, &error, &debug);
      printf("PTB-WARNING: GStreamer movie playback engine reports this warning:\n"
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
      printf("PTB-ERROR: GStreamer movie playback engine reports this error:\n"
	     "           Error from element %s: %s\n", GST_OBJECT_NAME(msg->src), error->message);
      printf("           Additional debug info: %s.\n\n", (debug) ? debug : "None");

      if ((error->domain == GST_RESOURCE_ERROR) && (error->code != GST_RESOURCE_ERROR_NOT_FOUND)) {
	      printf("           This means that there was some problem with reading the movie file (permissions etc.).\n\n");
      }

      if ((error->domain == GST_RESOURCE_ERROR) && (error->code == GST_RESOURCE_ERROR_NOT_FOUND)) {
	      printf("           This means that no such moviefile with the given name could be found.\n\n");
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

/* Video data arrived callback: Purely for documentation, because only used if oldstyle == true, that is *never*. */
static gboolean PsychHaveVideoDataCallback(GstPad *pad, GstBuffer *buffer, gpointer dataptr)
{
	unsigned int alloc_size;
	PsychMovieRecordType* movie = (PsychMovieRecordType*) dataptr;
	
	PsychLockMutex(&movie->mutex);

	if (movie->rate == 0) {
		PsychUnlockMutex(&movie->mutex);
		return(TRUE);
	}

	/* Perform onetime-init for the buffer */
	if (NULL == movie->imageBuffer) {
		// Allocate the buffer:
		alloc_size = buffer->size;
		if (buffer->size < movie->width * movie->height * 4) {
			alloc_size = movie->width * movie->height * 4;
			printf("PTB-DEBUG: Overriding unsafe buffer size of %d bytes with %d bytes.\n", buffer->size, alloc_size);
		} 
		// printf("PTB-DEBUG: Allocating image buffer of %d bytes.\n", alloc_size);
		movie->imageBuffer = calloc(1, alloc_size);
	}

	// Copy new image data to our buffer:
	memcpy(movie->imageBuffer, buffer->data, buffer->size);
	movie->frameAvail++;
        // printf("PTB-DEBUG: New frame %d [size %d] %lf.\n", movie->frameAvail, buffer->size, (double) buffer->timestamp / (double) 1e9);
	
	// Fetch presentation timestamp and convert to seconds:
	movie->pts = (double) buffer->timestamp / (double) 1e9;

	PsychUnlockMutex(&movie->mutex);
	PsychSignalCondition(&movie->condition);

	return(TRUE);
}

/* Called at each end-of-stream event at end of playback: */
static void PsychEOSCallback(GstAppSink *sink, gpointer user_data)
{
	PsychMovieRecordType* movie = (PsychMovieRecordType*) user_data;

	PsychLockMutex(&movie->mutex);
	//printf("PTB-DEBUG: Videosink reached EOS.\n");
	PsychUnlockMutex(&movie->mutex);
	PsychSignalCondition(&movie->condition);

	return;
}

/* Called whenever an active seek has completed or pipeline goes into pause.
 * Signals/handles arrival of preroll buffers. Used to detect/signal when
 * new videobuffers are available in non-playback mode.
 */
static GstFlowReturn PsychNewPrerollCallback(GstAppSink *sink, gpointer user_data)
{
	PsychMovieRecordType* movie = (PsychMovieRecordType*) user_data;

	PsychLockMutex(&movie->mutex);
	//printf("PTB-DEBUG: New PrerollBuffer received.\n");
	movie->preRollAvail++;
	PsychUnlockMutex(&movie->mutex);
	PsychSignalCondition(&movie->condition);

	return(GST_FLOW_OK);
}

/* Called whenever pipeline is in active playback and a new video frame arrives.
 * Used to detect/signal when new videobuffers are available in playback mode.
 */
static GstFlowReturn PsychNewBufferCallback(GstAppSink *sink, gpointer user_data)
{
	PsychMovieRecordType* movie = (PsychMovieRecordType*) user_data;

	PsychLockMutex(&movie->mutex);
	//printf("PTB-DEBUG: New Buffer received.\n");
	movie->frameAvail++;
	PsychUnlockMutex(&movie->mutex);
	PsychSignalCondition(&movie->condition);

	return(GST_FLOW_OK);
}

/* Not used by us, but needs to be defined as no-op anyway: */
static GstFlowReturn PsychNewBufferListCallback(GstAppSink *sink, gpointer user_data)
{
	PsychMovieRecordType* movie = (PsychMovieRecordType*) user_data;

	PsychLockMutex(&movie->mutex);
	//printf("PTB-DEBUG: New Bufferlist received.\n");
	PsychUnlockMutex(&movie->mutex);
	PsychSignalCondition(&movie->condition);

	return(GST_FLOW_OK);
}

/* Not used by us, but needs to be defined as no-op anyway: */
static void PsychDestroyNotifyCallback(gpointer user_data)
{
	return;
}

/* This callback is called when the pipeline is about to finish playback
 * of the current movie stream. If looped playback is enabled, this needs
 * to trigger a repetition by rescheduling the movie URI for playback.
 *
 * Allows gapless playback, but doesn't work reliable on all media types.
 *
 */
static void PsychMovieAboutToFinishCB(GstElement *theMovie, gpointer user_data)
{
	PsychMovieRecordType* movie = (PsychMovieRecordType*) user_data;
	if ((movie->loopflag > 0) && (movie->rate != 0)) {
		g_object_set(G_OBJECT(theMovie), "uri", movie->movieLocation, NULL);
		if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: About-to-finish received: Rewinding...\n");
	}

	return;
}

/* Not used, didn't work, but left here in case we find a use for it in the future. */
static void PsychMessageErrorCB(GstBus *bus, GstMessage *msg)
{
      gchar  *debug;
      GError *error;

      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);

      printf("PTB-BUSERROR: %s\n", error->message);
      g_error_free (error);
	return;
}

/*
 *      PsychGSCreateMovie() -- Create a movie object.
 *
 *      This function tries to open a moviefile (with or without audio/video tracks)
 *      and create an associated movie object for it.
 *
 *      win = Pointer to window record of associated onscreen window.
 *      moviename = char* with the name of the moviefile.
 *      preloadSecs = How many seconds of the movie should be preloaded/prefetched into RAM at movie open time?
 *      moviehandle = handle to the new movie.
 */
void PsychGSCreateMovie(PsychWindowRecordType *win, const char* moviename, double preloadSecs, int* moviehandle)
{
    GstCaps                     *colorcaps;
    GstElement			*theMovie = NULL;
    GMainLoop			*MovieContext = NULL;
    GstBus			*bus = NULL;
    GstFormat			fmt;
    gint64			length_format;
    GstPad			*pad, *peerpad;
    const GstCaps		*caps;
    GstStructure		*str;
    gint			width,height;
    gint			rate1, rate2;
    int				i, slotid;
    GError			*error = NULL;
    char			movieLocation[FILENAME_MAX];
    psych_bool			trueValue = TRUE;
    char			msgerr[10000];
    char			errdesc[1000];
    psych_bool			printErrors;

    // Suppress output of error-messages if moviehandle == 1000. That means we
    // run in our own Posix-Thread, not in the Matlab-Thread. Printing via Matlabs
    // printing facilities would likely cause a terrible crash.
    printErrors = (*moviehandle == -1000) ? FALSE : TRUE;
    
    // Set movie handle to "failed" initially:
    *moviehandle = -1;

    // We start GStreamer only on first invocation.
    if (firsttime) {
        // Initialize GStreamer:
        if(!gst_init_check(NULL, NULL, &error)) {
		if (printErrors && error) {
			printf("PTB-ERROR: GStreamer initialization failed with error: %s\n", (char*) error->message);
			g_error_free(error);
			PsychErrorExitMsg(PsychError_system, "GStreamer initialization failed! Movie playback functions out of order.");
		} else return;
        }
        firsttime = FALSE;
    }

    if (win && !PsychIsOnscreenWindow(win)) {
        if (printErrors) PsychErrorExitMsg(PsychError_user, "Provided windowPtr is not an onscreen window."); else return;
    }

    if (NULL == moviename) {
        if (printErrors) PsychErrorExitMsg(PsychError_internal, "NULL-Ptr instead of moviename passed!"); else return;
    }

    if (numMovieRecords >= PSYCH_MAX_MOVIES) {
        *moviehandle = -2;
        if (printErrors) PsychErrorExitMsg(PsychError_user, "Allowed maximum number of simultaneously open movies exceeded!"); else return;
    }

    // Search first free slot in movieRecordBANK:
    for (i=0; (i < PSYCH_MAX_MOVIES) && (movieRecordBANK[i].theMovie); i++);
    if (i>=PSYCH_MAX_MOVIES) {
        *moviehandle = -2;
        if (printErrors) PsychErrorExitMsg(PsychError_user, "Allowed maximum number of simultaneously open movies exceeded!"); else return;
    }

    // Slot slotid will contain the movie record for our new movie object:
    slotid=i;

    // Zero-out new record in moviebank:
    memset(&movieRecordBANK[slotid], 0, sizeof(PsychMovieRecordType));
    
    // Create name-string for moviename: If an URI qualifier is at the beginning,
    // we're fine and just pass the URI as-is. Otherwise we add the file:// URI prefix.
    if (strstr(moviename, "://")) {
	snprintf(movieLocation, sizeof(movieLocation)-1, "%s", moviename);
    } else {
	snprintf(movieLocation, sizeof(movieLocation)-1, "file:////%s", moviename);
    }
    strncpy(movieRecordBANK[slotid].movieLocation, movieLocation, FILENAME_MAX);

    // Create movie playback pipeline:
    theMovie = gst_element_factory_make ("playbin2", "ptbmovieplaybackpipeline");

    // Assign name of movie to play:
    g_object_set(G_OBJECT(theMovie), "uri", movieLocation, NULL);

    // Connect callback to about-to-finish signal: Signal is emitted as soon as
    // end of current playback iteration is approaching. The callback checks if
    // looped playback is requested. If so, it schedules a new playback iteration.
    g_signal_connect(G_OBJECT(theMovie), "about-to-finish", G_CALLBACK(PsychMovieAboutToFinishCB), &(movieRecordBANK[slotid]));

    // Assign message context, message bus and message callback for
    // the pipeline to report events and state changes, errors etc.:    
    MovieContext = g_main_loop_new (NULL, FALSE);
    movieRecordBANK[slotid].MovieContext = MovieContext;
    bus = gst_pipeline_get_bus(GST_PIPELINE(theMovie));
    // Didn't work: g_signal_connect (G_OBJECT(bus), "message::error", G_CALLBACK(PsychMessageErrorCB), NULL);
    //              g_signal_connect (G_OBJECT(bus), "message::warning", G_CALLBACK(PsychMessageErrorCB), NULL);
    gst_bus_add_watch(bus, PsychMovieBusCallback, &(movieRecordBANK[slotid]));
    gst_object_unref(bus);

    // Assign a fakesink named "ptbsink0" as destination video-sink for
    // all video content. This allows us to get hold of the video frame buffers for
    // converting them into PTB OpenGL textures:
    GstElement *videosink = gst_element_factory_make ("appsink", "ptbsink0");
    if (!videosink) {
	printf("PTB-ERROR: Failed to create video-sink appsink ptbsink!\n");
	PsychGSProcessMovieContext(movieRecordBANK[slotid].MovieContext, TRUE);
	PsychErrorExitMsg(PsychError_system, "Opening the movie failed. Reason hopefully given above.");
    };

    movieRecordBANK[slotid].videosink = videosink;

    // Our OpenGL texture creation routine needs GL_BGRA8 data in G_UNSIGNED_8_8_8_8_REV
    // format, but the pipeline usually delivers YUV data in planar format. Therefore
    // need to perform colorspace/colorformat conversion. We build a little videobin
    // which consists of a ffmpegcolorspace converter plugin connected to our appsink
    // plugin which will deliver video data to us for conversion into textures.
    // The "sink" pad of the converter plugin is connected as the "sink" pad of our
    // videobin, and the videobin is connected to the video-sink output of the pipeline,
    // thereby receiving decoded video data. We place a videocaps filter inbetween the
    // converter and the appsink to enforce a color format conversion to the "colorcaps"
    // we need. colorcaps define the needed data format for efficient conversion into
    // a RGBA8 texture:
    colorcaps = gst_caps_new_simple (   "video/x-raw-rgb",
					"bpp", G_TYPE_INT, 32,
					"depth", G_TYPE_INT, 32,
					"alpha_mask", G_TYPE_INT, 0x000000FF,
					"red_mask", G_TYPE_INT,   0x0000FF00,
					"green_mask", G_TYPE_INT, 0x00FF0000,
					"blue_mask", G_TYPE_INT,  0xFF000000,
					NULL);

    /*
    // Old style method: Only left here for documentation to show how one can create
    // video sub-pipelines via bin's and connect them to each other via ghostpads: 

    GstElement *videobin = gst_bin_new ("video_output_bin");
    GstElement *videocon = gst_element_factory_make ("ffmpegcolorspace", "color_converter");
    gst_bin_add_many(GST_BIN(videobin), videocon, videosink, NULL);

    GstPad *ghostpad = gst_ghost_pad_new("Video_Ghostsink", gst_element_get_pad(videocon, "sink"));
    gst_element_add_pad(videobin, ghostpad);

    gst_element_link_filtered(videocon, videosink, colorcaps);

    // Assign our special videobin as video-sink of the pipeline:
    g_object_set(G_OBJECT(theMovie), "video-sink", videobin, NULL);
    */

    // New style method: Leaves the freedom of choice of color converter (if any)
    // to the auto-plugger.

    // Assign 'colorcaps' as caps to our videosink. This marks the videosink so
    // that it can only receive video image data in the format defined by colorcaps,
    // i.e., a format that is easy to consume for OpenGL's texture creation on std.
    // gpu's. It is the job of the video pipeline's autoplugger to plug in proper
    // color & format conversion plugins to satisfy videosink's needs.
    gst_app_sink_set_caps(GST_APP_SINK(videosink), colorcaps);

    // Assign our special appsink 'videosink' as video-sink of the pipeline:
    g_object_set(G_OBJECT(theMovie), "video-sink", videosink, NULL);
    gst_caps_unref(colorcaps);

    // Get the pad from the final sink for probing width x height of movie frames and nominal framerate of movie:	
    pad = gst_element_get_pad(videosink, "sink");

    PsychGSProcessMovieContext(movieRecordBANK[slotid].MovieContext, FALSE);

    // Should we preroll / preload?	
    if (preloadSecs > 0) {
	// Preload / Preroll the pipeline:
	if (!PsychMoviePipelineSetState(theMovie, GST_STATE_PAUSED, 30.0)) {
		PsychGSProcessMovieContext(movieRecordBANK[slotid].MovieContext, TRUE);
		PsychErrorExitMsg(PsychError_user, "In OpenMovie: Opening the movie failed. Reason given above.");
	}
    } else {
	// Ready the pipeline:
	if (!PsychMoviePipelineSetState(theMovie, GST_STATE_READY, 30.0)) {
		PsychGSProcessMovieContext(movieRecordBANK[slotid].MovieContext, TRUE);
		PsychErrorExitMsg(PsychError_user, "In OpenMovie: Opening the movie failed. Reason given above.");
	}    
    }

    // Query number of available video and audio tracks in movie:
    g_object_get (G_OBJECT(theMovie),
               "n-video", &movieRecordBANK[slotid].nrVideoTracks,
               "n-audio", &movieRecordBANK[slotid].nrAudioTracks,
                NULL);

    // We need a valid onscreen window handle for real video playback:
    if ((NULL == win) && (movieRecordBANK[slotid].nrVideoTracks > 0)) {
        if (printErrors) PsychErrorExitMsg(PsychError_user, "No windowPtr to an onscreen window provided. Must do so for movies with videotrack!"); else return;
    }
 
    PsychGSProcessMovieContext(movieRecordBANK[slotid].MovieContext, FALSE);

    PsychInitMutex(&movieRecordBANK[slotid].mutex);
    PsychInitCondition(&movieRecordBANK[slotid].condition, NULL);

    if (oldstyle) {
	// Install the probe callback for reception of video frames from engine at the sink-pad itself:
	gst_pad_add_buffer_probe(pad, G_CALLBACK(PsychHaveVideoDataCallback), &(movieRecordBANK[slotid]));
    } else {
	// Install callbacks used by the videosink (appsink) to announce various events:
	GstAppSinkCallbacks videosinkCallbacks = {
		.eos = PsychEOSCallback,
		.new_preroll = PsychNewPrerollCallback,
		.new_buffer = PsychNewBufferCallback,
		.new_buffer_list = PsychNewBufferListCallback
	};

	gst_app_sink_set_callbacks(GST_APP_SINK(videosink), &videosinkCallbacks, &(movieRecordBANK[slotid]), PsychDestroyNotifyCallback);
    }

    // Drop frames if callback can't pull buffers fast enough:
    // This together with the max queue lengths of 1 allows to
    // maintain audio-video sync by framedropping if needed.
    gst_app_sink_set_drop(GST_APP_SINK(videosink), TRUE);

    // Only allow one queued buffer before dropping:
    gst_app_sink_set_max_buffers(GST_APP_SINK(videosink), 1);

    // Videotrack available?
    if (movieRecordBANK[slotid].nrVideoTracks > 0) {
	// Yes: Query size and framerate of movie:
	peerpad = gst_pad_get_peer(pad);
	caps=gst_pad_get_negotiated_caps(peerpad);
	if (caps) {
		str=gst_caps_get_structure(caps,0);
		/* Get some data about the frame */
		gst_structure_get_int(str,"width",&width);
		gst_structure_get_int(str,"height",&height);
		rate1 = 0;
		rate2 = 1;
		gst_structure_get_fraction(str, "framerate", &rate1, &rate2);
	 } else {
		printf("PTB-DEBUG: No frame info available after preroll.\n");	
	 }
    } else {
	// No: Audio only. Assign harmless settings for fps and frame size:
	rate1 = 0;
	rate2 = 1;
	width = height = 0;
    }

    // Release the pad:
    gst_object_unref(pad);

    // Assign new record in moviebank:
    movieRecordBANK[slotid].theMovie = theMovie;
    movieRecordBANK[slotid].loopflag = 0;
    movieRecordBANK[slotid].frameAvail = 0;
    movieRecordBANK[slotid].imageBuffer = NULL;

    *moviehandle = slotid;

    // Increase counter:
    numMovieRecords++;

    // Compute basic movie properties - Duration and fps as well as image size:
    
    // Retrieve duration in seconds:
    fmt = GST_FORMAT_TIME;
    if (gst_element_query_duration(theMovie, &fmt, &length_format)) {
	// This returns nsecs, so convert to seconds:
    	movieRecordBANK[slotid].movieduration = (double) length_format / (double) 1e9;
	//printf("PTB-DEBUG: Duration of movie %i [%s] is %lf seconds.\n", slotid, moviename, movieRecordBANK[slotid].movieduration);
    } else {
	movieRecordBANK[slotid].movieduration = DBL_MAX;
	printf("PTB-WARNING: Could not query duration of movie %i [%s] in seconds. Returning infinity.\n", slotid, moviename);
    }

    // Assign expected framerate, assuming a linear spacing between frames:
    movieRecordBANK[slotid].fps = (double) rate1 / (double) rate2;
    //printf("PTB-DEBUG: Framerate fps of movie %i [%s] is %lf fps.\n", slotid, moviename, movieRecordBANK[slotid].fps);

    // Compute framecount from fps and duration:
    movieRecordBANK[slotid].nrframes = (int)(movieRecordBANK[slotid].fps * movieRecordBANK[slotid].movieduration + 0.5);
    //printf("PTB-DEBUG: Number of frames in movie %i [%s] is %i.\n", slotid, moviename, movieRecordBANK[slotid].nrframes);

    // Define size of images in movie:
    movieRecordBANK[slotid].width = width;
    movieRecordBANK[slotid].height = height;

    // Ready to rock!
    return;
}

/*
 *  PsychGSGetMovieInfos() - Return basic information about a movie.
 *
 *  framecount = Total number of video frames in the movie, determined by counting.
 *  durationsecs = Total playback duration of the movie, in seconds.
 *  framerate = Estimated video playback framerate in frames per second (fps).
 *  width = Width of movie images in pixels.
 *  height = Height of movie images in pixels.
 *  nrdroppedframes = Total count of videoframes that had to be dropped during last movie playback,
 *                    in order to keep the movie synced with the realtime clock.
 */
void PsychGSGetMovieInfos(int moviehandle, int* width, int* height, int* framecount, double* durationsecs, double* framerate, int* nrdroppedframes)
{
    if (moviehandle < 0 || moviehandle >= PSYCH_MAX_MOVIES) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided!");
    }
    
    if (movieRecordBANK[moviehandle].theMovie == NULL) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. No movie associated with this handle !!!");
    }

    if (framecount) *framecount = movieRecordBANK[moviehandle].nrframes;
    if (durationsecs) *durationsecs = movieRecordBANK[moviehandle].movieduration;
    if (framerate) *framerate = movieRecordBANK[moviehandle].fps;
    if (nrdroppedframes) *nrdroppedframes = movieRecordBANK[moviehandle].nr_droppedframes;
    if (width) *width = movieRecordBANK[moviehandle].width; 
    if (height) *height = movieRecordBANK[moviehandle].height; 

    return;
}

/*
 *  PsychGSDeleteMovie() -- Delete a movie object and release all associated ressources.
 */
void PsychGSDeleteMovie(int moviehandle)
{
    if (moviehandle < 0 || moviehandle >= PSYCH_MAX_MOVIES) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided!");
    }
    
    if (movieRecordBANK[moviehandle].theMovie == NULL) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. No movie associated with this handle !!!");
    }
        
    // Stop movie playback immediately:
    PsychMoviePipelineSetState(movieRecordBANK[moviehandle].theMovie, GST_STATE_NULL, 20.0);

    // Delete movieobject for this handle:
    gst_object_unref(GST_OBJECT(movieRecordBANK[moviehandle].theMovie));
    movieRecordBANK[moviehandle].theMovie=NULL;

    // Delete visual context for this movie:
    movieRecordBANK[moviehandle].MovieContext = NULL;

    PsychDestroyMutex(&movieRecordBANK[moviehandle].mutex);
    PsychDestroyCondition(&movieRecordBANK[moviehandle].condition);

    free(movieRecordBANK[moviehandle].imageBuffer);
    movieRecordBANK[moviehandle].imageBuffer = NULL;
    movieRecordBANK[moviehandle].videosink = NULL;

    // Decrease counter:
    if (numMovieRecords>0) numMovieRecords--;
        
    return;
}

/*
 *  PsychGSDeleteAllMovies() -- Delete all movie objects and release all associated ressources.
 */
void PsychGSDeleteAllMovies(void)
{
    int i;
    for (i=0; i<PSYCH_MAX_MOVIES; i++) {
        if (movieRecordBANK[i].theMovie) PsychGSDeleteMovie(i);
    }
    return;
}

/*
 *  PsychGSGetTextureFromMovie() -- Create an OpenGL texture map from a specific videoframe from given movie object.
 *
 *  win = Window pointer of onscreen window for which a OpenGL texture should be created.
 *  moviehandle = Handle to the movie object.
 *  checkForImage = true == Just check if new image available, false == really retrieve the image, blocking if necessary.
 *  timeindex = When not in playback mode, this allows specification of a requested frame by presentation time.
 *              If set to -1, or if in realtime playback mode, this parameter is ignored and the next video frame is returned.
 *  out_texture = Pointer to the Psychtoolbox texture-record where the new texture should be stored.
 *  presentation_timestamp = A ptr to a double variable, where the presentation timestamp of the returned frame should be stored.
 *
 *  Returns true (1) on success, false (0) if no new image available, -1 if no new image available and there won't be any in future.
 */
int PsychGSGetTextureFromMovie(PsychWindowRecordType *win, int moviehandle, int checkForImage, double timeindex,
			     PsychWindowRecordType *out_texture, double *presentation_timestamp)
{
    GstElement			*theMovie;
    unsigned int		failcount=0;
    GLuint			texid;
    float			rate;
    double			targetdelta, realdelta, frames;
    PsychRectType		outRect;
    GstBuffer                   *videoBuffer = NULL;
    GstFormat		        fmt;
    gint64		        pos_index, bufferIndex;
    double                      deltaT = 0;
    GstEvent                    *event;

    if (!PsychIsOnscreenWindow(win)) {
        PsychErrorExitMsg(PsychError_user, "Need onscreen window ptr!!!");
    }
    
    if (moviehandle < 0 || moviehandle >= PSYCH_MAX_MOVIES) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided.");
    }
    
    if ((timeindex!=-1) && (timeindex < 0 || timeindex >= 10000.0)) {
        PsychErrorExitMsg(PsychError_user, "Invalid timeindex provided.");
    }
    
    if (NULL == out_texture && !checkForImage) {
        PsychErrorExitMsg(PsychError_internal, "NULL-Ptr instead of out_texture ptr passed!!!");
    }
    
    // Fetch references to objects we need:
    theMovie = movieRecordBANK[moviehandle].theMovie;
    if (theMovie == NULL) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. No movie associated with this handle.");
    }

    // Allow context task to do its internal bookkeeping and cleanup work:
    PsychGSProcessMovieContext(movieRecordBANK[moviehandle].MovieContext, FALSE);

    // If this is a pure audio "movie" with no video tracks, we always return failed,
    // as those certainly don't have movie frames associated.
    if (movieRecordBANK[moviehandle].nrVideoTracks == 0) return((checkForImage) ? -1 : FALSE);

    // Get current playback rate:
    rate = movieRecordBANK[moviehandle].rate;

    // Is movie actively playing (automatic async playback, possibly with synced sound)?
    // If so, then we ignore the 'timeindex' parameter, because the automatic playback
    // process determines which frames should be delivered to PTB when. This function will
    // simply wait or poll for arrival/presence of a new frame that hasn't been fetched
    // in previous calls.
    if (0 == rate) {
        // Movie playback inactive. We are in "manual" mode: No automatic async playback,
        // no synced audio output. The user just wants to manually fetch movie frames into
        // textures for manual playback in a standard Matlab-loop.

	// First pass - checking for new image?
	if (checkForImage) {
		// Image for specific point in time requested?
		if (timeindex >= 0) {
			// Yes. We try to retrieve the next possible image for requested timeindex.
			// Seek to target timeindex:
			PsychGSSetMovieTimeIndex(moviehandle, timeindex, FALSE);
		}
		else {
			// No. We just retrieve the next frame, given the current position.
			// Nothing to do so far...
		}

		// Check for frame availability happens down there in the shared check code...
	}
    }

    // Should we just check for new image? If so, just return availability status:
    if (checkForImage) {
	PsychLockMutex(&movieRecordBANK[moviehandle].mutex);
	if ((((0 != rate) && movieRecordBANK[moviehandle].frameAvail) || ((0 == rate) && movieRecordBANK[moviehandle].preRollAvail)) &&
	    !gst_app_sink_is_eos(GST_APP_SINK(movieRecordBANK[moviehandle].videosink))) {
		// New frame available. Unlock and report success:
		//printf("PTB-DEBUG: NEW FRAME %d\n", movieRecordBANK[moviehandle].frameAvail);
		PsychUnlockMutex(&movieRecordBANK[moviehandle].mutex);
		return(true);
	}

	// Is this the special case of a movie without video, but only sound? In that case
	// we always return a 'false' because there ain't no image to return. We check this
	// indirectly - If the imageBuffer is NULL then the video callback hasn't been called.
	if (oldstyle && (NULL == movieRecordBANK[moviehandle].imageBuffer)) {
		PsychUnlockMutex(&movieRecordBANK[moviehandle].mutex);
		return(false);
	}

	// None available. Any chance there will be one in the future?
        if (gst_app_sink_is_eos(GST_APP_SINK(movieRecordBANK[moviehandle].videosink)) && movieRecordBANK[moviehandle].loopflag == 0) {
		// No new frame available and there won't be any in the future, because this is a non-looping
		// movie that has reached its end.
		PsychUnlockMutex(&movieRecordBANK[moviehandle].mutex);
		return(-1);
        }
        else {
		// No new frame available yet:
		PsychUnlockMutex(&movieRecordBANK[moviehandle].mutex);
		//printf("PTB-DEBUG: NO NEW FRAME\n");
		return(false);
        }
    }

    // If we reach this point, then an image fetch is requested. If no new data
    // is available we shall block:

    PsychLockMutex(&movieRecordBANK[moviehandle].mutex);
    // printf("PTB-DEBUG: Blocking fetch start %d\n", movieRecordBANK[moviehandle].frameAvail);

    if (((0 != rate) && !movieRecordBANK[moviehandle].frameAvail) ||
	((0 == rate) && !movieRecordBANK[moviehandle].preRollAvail)) {
	// No new frame available. Perform a blocking wait:
	PsychTimedWaitCondition(&movieRecordBANK[moviehandle].condition, &movieRecordBANK[moviehandle].mutex, 10.0);

	// Recheck:
	if (((0 != rate) && !movieRecordBANK[moviehandle].frameAvail) ||
	    ((0 == rate) && !movieRecordBANK[moviehandle].preRollAvail)) {
		// Game over! Wait timed out after 10 secs.
		PsychUnlockMutex(&movieRecordBANK[moviehandle].mutex);
		printf("PTB-ERROR: No new video frame received after timeout of 10 seconds! Something's wrong. Aborting fetch.\n");
		return(FALSE);
	}

	// At this point we should have at least one frame available.
        // printf("PTB-DEBUG: After blocking fetch start %d\n", movieRecordBANK[moviehandle].frameAvail);
    }

    // We're here with at least one frame available and the mutex lock held.

    // Preroll case is simple:
    movieRecordBANK[moviehandle].preRollAvail = 0;

    // Perform texture fetch & creation:
    if (oldstyle) {
	// Reset frame available flag:
	movieRecordBANK[moviehandle].frameAvail = 0;

	// This will retrieve an OpenGL compatible pointer to the pixel data and assign it to our texmemptr:
	out_texture->textureMemory = (GLuint*) movieRecordBANK[moviehandle].imageBuffer;
    } else {
	// Active playback mode?
	if (0 != rate) {
		// Active playback mode: One less frame available after our fetch:
		movieRecordBANK[moviehandle].frameAvail--;
		if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: Pulling from videosink, %d buffers avail...\n", movieRecordBANK[moviehandle].frameAvail);

		// Clamp frameAvail to queue lengths:
		if (gst_app_sink_get_max_buffers(GST_APP_SINK(movieRecordBANK[moviehandle].videosink)) < movieRecordBANK[moviehandle].frameAvail) {
			movieRecordBANK[moviehandle].frameAvail = gst_app_sink_get_max_buffers(GST_APP_SINK(movieRecordBANK[moviehandle].videosink));
		}

		// This will pull the oldest video buffer from the videosink. It would block if none were available,
		// but that won't happen as we wouldn't reach this statement if none were available. It would return
		// NULL if the stream would be EOS or the pipeline off, but that shouldn't ever happen:
		videoBuffer = gst_app_sink_pull_buffer(GST_APP_SINK(movieRecordBANK[moviehandle].videosink));
	} else {
		// Passive fetch mode: Use prerolled buffers after seek:
		// These are available even after eos...
		videoBuffer = gst_app_sink_pull_preroll(GST_APP_SINK(movieRecordBANK[moviehandle].videosink));
	}

	// We can unlock early, thanks to videosink's internal buffering:
	PsychUnlockMutex(&movieRecordBANK[moviehandle].mutex);

	if (videoBuffer) {
		// Assign pointer to videoBuffer's data directly: Avoids one full data copy compared to oldstyle method.
		out_texture->textureMemory = (GLuint*) GST_BUFFER_DATA(videoBuffer);

		// Assign pts presentation timestamp in pipeline stream time and convert to seconds:
		movieRecordBANK[moviehandle].pts = (double) GST_BUFFER_TIMESTAMP(videoBuffer) / (double) 1e9;
		if (GST_CLOCK_TIME_IS_VALID(GST_BUFFER_DURATION(videoBuffer)))
			deltaT = (double) GST_BUFFER_DURATION(videoBuffer) / (double) 1e9;
		bufferIndex = GST_BUFFER_OFFSET(videoBuffer);
	} else {
		printf("PTB-ERROR: No new video frame received in gst_app_sink_pull_buffer! Something's wrong. Aborting fetch.\n");
		return(FALSE);
	}
	if (PsychPrefStateGet_Verbosity()>4) printf("PTB-DEBUG: ...done.\n");
    }

    // Assign presentation_timestamp:
    if (presentation_timestamp) *presentation_timestamp = movieRecordBANK[moviehandle].pts;

    // Activate OpenGL context of target window:
    PsychSetGLContext(win);

    #if PSYCH_SYSTEM == PSYCH_OSX
    // Explicitely disable Apple's Client storage extensions. For now they are not really useful to us.
    glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_FALSE);
    #endif

    // Build a standard PTB texture record:
    PsychMakeRect(out_texture->rect, 0, 0, movieRecordBANK[moviehandle].width, movieRecordBANK[moviehandle].height);    
        
    // Set NULL - special texture object as part of the PTB texture record:
    out_texture->targetSpecific.QuickTimeGLTexture = NULL;

    // Set texture orientation as if it were an inverted Offscreen window: Upside-down.
    out_texture->textureOrientation = 3;
        
    // We use zero client storage memory bytes:
    out_texture->textureMemorySizeBytes = 0;

    // Textures are aligned on 4 Byte boundaries because texels are RGBA8:
    out_texture->textureByteAligned = 4;

    // Let PsychCreateTexture() do the rest of the job of creating, setting up and
    // filling an OpenGL texture with content:
    PsychCreateTexture(out_texture);

    // Detection of dropped frames: This is a heuristic. We'll see how well it works out...
    // TODO: GstBuffer videoBuffer provides special flags that should allow to do a more
    // robust job, although nothing's wrong with the current approach per se...
    if (rate && presentation_timestamp) {
        // Try to check for dropped frames in playback mode:

        // Expected delta between successive presentation timestamps:
        targetdelta = 1.0f / (movieRecordBANK[moviehandle].fps * rate);

        // Compute real delta, given rate and playback direction:
        if (rate > 0) {
            realdelta = *presentation_timestamp - movieRecordBANK[moviehandle].last_pts;
            if (realdelta < 0) realdelta = 0;
        }
        else {
            realdelta = -1.0 * (*presentation_timestamp - movieRecordBANK[moviehandle].last_pts);
            if (realdelta < 0) realdelta = 0;
        }
        
        frames = realdelta / targetdelta;
        // Dropped frames?
        if (frames > 1 && movieRecordBANK[moviehandle].last_pts >= 0) {
            movieRecordBANK[moviehandle].nr_droppedframes += (int) (frames - 1 + 0.5);
        }

        movieRecordBANK[moviehandle].last_pts = *presentation_timestamp;
    }

    // Unlock.
    if (oldstyle) {
	PsychUnlockMutex(&movieRecordBANK[moviehandle].mutex);
    } else {
	gst_buffer_unref(videoBuffer);
	videoBuffer = NULL;
    }
    
    // Manually advance movie time, if in fetch mode:
    if (0 == rate) {
        // We are in manual fetch mode: Need to manually advance movie to next
        // media sample:
	event = gst_event_new_step(GST_FORMAT_BUFFERS, 1, 1.0, TRUE, FALSE);
	gst_element_send_event(theMovie, event);

	// Block until seek completed, failed, or timeout of 30 seconds reached:
        gst_element_get_state(theMovie, NULL, NULL, 30 * 1e9);
    }

    return(TRUE);
}

/*
 *  PsychGSFreeMovieTexture() - Release texture memory for a Quicktime texture.
 *
 *  This routine is called by PsychDeleteTexture() in PsychTextureSupport.c
 *  It performs the special cleanup necessary for Quicktime created textures.
 *  As this ain't Quicktime but GStreamer there ain't nothing to do for us.
 */
void PsychGSFreeMovieTexture(PsychWindowRecordType *win)
{
    return;
}

/*
 *  PsychGSPlaybackRate() - Start- and stop movieplayback, set playback parameters.
 *
 *  moviehandle = Movie to start-/stop.
 *  playbackrate = zero == Stop playback, non-zero == Play movie with spec. rate,
 *                 e.g., 1 = forward, 2 = double speed forward, -1 = backward, ...
 *  loop = 0 = Play once. 1 = Loop, aka rewind at end of movie and restart.
 *  soundvolume = 0 == Mute sound playback, between 0.0 and 1.0 == Set volume to 0 - 100 %.
 *  Returns Number of dropped frames to keep playback in sync.
 */
int PsychGSPlaybackRate(int moviehandle, double playbackrate, int loop, double soundvolume)
{
    int			dropped = 0;
    GstElement		*theMovie = NULL;
    
    if (moviehandle < 0 || moviehandle >= PSYCH_MAX_MOVIES) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided!");
    }
        
    // Fetch references to objects we need:
    theMovie = movieRecordBANK[moviehandle].theMovie;    
    if (theMovie == NULL) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. No movie associated with this handle !!!");
    }
    
    if (playbackrate != 0) {
        // Start playback of movie:

	// Set volume and mute state for audio:
	g_object_set(G_OBJECT(theMovie), "mute", (soundvolume <= 0) ? TRUE : FALSE, NULL);
	g_object_set(G_OBJECT(theMovie), "volume", soundvolume, NULL);

	// Set playback rate:
	gst_element_seek(theMovie, playbackrate, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE, GST_SEEK_TYPE_NONE, 0, GST_SEEK_TYPE_NONE, 0);
        movieRecordBANK[moviehandle].loopflag = loop;
        movieRecordBANK[moviehandle].last_pts = -1.0;
        movieRecordBANK[moviehandle].nr_droppedframes = 0;
	movieRecordBANK[moviehandle].rate = playbackrate;
	movieRecordBANK[moviehandle].frameAvail = 0;
	movieRecordBANK[moviehandle].preRollAvail = 0;

	// Start it:
	PsychMoviePipelineSetState(theMovie, GST_STATE_PLAYING, 10.0);
	PsychGSProcessMovieContext(movieRecordBANK[moviehandle].MovieContext, FALSE);
    }
    else {
	// Stop playback of movie:
	movieRecordBANK[moviehandle].rate = 0;
	PsychMoviePipelineSetState(theMovie, GST_STATE_PAUSED, 10.0);
	PsychGSProcessMovieContext(movieRecordBANK[moviehandle].MovieContext, FALSE);

        // Output count of dropped frames:
        if ((dropped=movieRecordBANK[moviehandle].nr_droppedframes) > 0) {
            if (PsychPrefStateGet_Verbosity()>2) {
		printf("PTB-INFO: Movie playback had to drop %i frames of movie %i to keep playback in sync.\n", movieRecordBANK[moviehandle].nr_droppedframes, moviehandle);
	    }
        }
    }

    return(dropped);
}

/*
 *  void PsychGSExitMovies() - Shutdown handler.
 *
 *  This routine is called by Screen('CloseAll') and on clear Screen time to
 *  do final cleanup. It deletes all textures and releases all movie objects.
 *
 */
void PsychGSExitMovies(void)
{
    PsychWindowRecordType	**windowRecordArray;
    int				i, numWindows; 
    
    // Release all Quicktime related OpenGL textures:
    PsychCreateVolatileWindowRecordPointerList(&numWindows, &windowRecordArray);
    for(i=0; i<numWindows; i++) {
        // Delete all Quicktime textures:
        if ((windowRecordArray[i]->windowType == kPsychTexture) && (windowRecordArray[i]->targetSpecific.QuickTimeGLTexture !=NULL)) { 
            PsychCloseWindow(windowRecordArray[i]);
        }
    }
    PsychDestroyVolatileWindowRecordPointerList(windowRecordArray);
    
    // Release all movies:
    PsychGSDeleteAllMovies();
    
    return;
}

/*
 *  PsychGSGetMovieTimeIndex()  -- Return current playback time of movie.
 */
double PsychGSGetMovieTimeIndex(int moviehandle)
{
    GstElement		*theMovie = NULL;
    GstFormat		fmt;
    gint64		pos_nsecs;

    if (moviehandle < 0 || moviehandle >= PSYCH_MAX_MOVIES) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided!");
    }
    
    // Fetch references to objects we need:
    theMovie = movieRecordBANK[moviehandle].theMovie;    
    if (theMovie == NULL) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. No movie associated with this handle !!!");
    }

    fmt = GST_FORMAT_TIME;
    if (!gst_element_query_position(theMovie, &fmt, &pos_nsecs)) {
	printf("PTB-WARNING: Could not query position in movie %i in seconds. Returning zero.\n", moviehandle);
	pos_nsecs = 0;
    }

    // Retrieve timeindex:
    return((double) pos_nsecs / (double) 1e9);
}

/*
 *  PsychGSSetMovieTimeIndex()  -- Set current playback time of movie, perform active seek if needed.
 */
double PsychGSSetMovieTimeIndex(int moviehandle, double timeindex, psych_bool indexIsFrames)
{
    GstElement		*theMovie;
    double		oldtime;
    long		targetIndex;
    GstEvent            *event;
    
    if (moviehandle < 0 || moviehandle >= PSYCH_MAX_MOVIES) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided!");
    }
    
    // Fetch references to objects we need:
    theMovie = movieRecordBANK[moviehandle].theMovie;    
    if (theMovie == NULL) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. No movie associated with this handle !!!");
    }
    
    // Retrieve current timeindex:
    oldtime = PsychGSGetMovieTimeIndex(moviehandle);

    // TODO NOTE: We could use GST_SEEK_FLAG_SKIP to allow framedropping on fast forward/reverse playback...

    // Index based or target time based seeking?
    if (indexIsFrames) {
	// Index based seeking:		
	// TODO FIXME: This doesn't work (well) at all! Something's wrong here...
	// Seek to given targetIndex:
	targetIndex = (long) (timeindex + 0.5);

	// Simple seek, frame buffer (index) oriented, with pipeline flush and accurate seek,
	// i.e., not locked to keyframes, but frame-accurate: GST_FORMAT_DEFAULT?
	// gst_element_seek_simple(theMovie, GST_FORMAT_BUFFERS, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE, targetIndex);
	event = gst_event_new_seek(1.0, GST_FORMAT_BUFFERS, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
				   GST_SEEK_TYPE_SET, targetIndex, GST_SEEK_TYPE_END, 0);
	gst_element_send_event(theMovie, event);
    }
    else {
	// Time based seeking:
	// Set new timeindex as time in seconds:

	// Simple seek, time-oriented, with pipeline flush and accurate seek,
	// i.e., not locked to keyframes, but frame-accurate:
	gst_element_seek_simple(theMovie, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE, timeindex * (double) 1e9);
    }

    // Block until seek completed, failed or timeout of 30 seconds reached:
    gst_element_get_state(theMovie, NULL, NULL, 30 * 1e9);

    // Return old time value of previous position:
    return(oldtime);
}

// #ifdef PTB_USE_GSTREAMER
#endif
