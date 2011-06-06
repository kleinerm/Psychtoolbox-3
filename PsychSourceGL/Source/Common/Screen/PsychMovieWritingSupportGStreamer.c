/*
	Common/Screen/PsychMovieWritingSupportGStreamer.c

	PLATFORMS:

		GNU/Linux for now, but would already support MS-Windows and
		will be supported on OS/X as well in the future.

		This is the movie editing and writing/creation engine based on the
		GStreamer multimedia framework.

	AUTHORS:

		Mario Kleiner           mk              mario.kleiner@tuebingen.mpg.de

	HISTORY:

		06/06/11		mk		Wrote it.

	DESCRIPTION:

		Psychtoolbox functions for dealing with GStreamer movie editing.

	NOTES:

*/

#include "Screen.h"

// No Quicktime support for GNU/Linux, but probably GStreamer support:
#if PSYCH_SYSTEM == PSYCH_LINUX

// GStreamer support enabled?
#ifdef PTB_USE_GSTREAMER

// GStreamer includes:
#include <gst/gst.h>

// PsychGetCodecLaunchLineFromString() - Helper function for GStreamer based movie writing.
// Defined in PsychVideoCaptureSupport.h: psych_bool PsychGetCodecLaunchLineFromString(char* codecSpec, char* launchString);

// GStreamer implementation of movie writing support:

// Record which defines all state for a capture device:
typedef struct {
	volatile psych_bool                             eos;
	GMainLoop *                                     Context;
	GstElement*                                     Movie;
        GstElement*                                     appsrc;
	GstBus*                                         bus;
	GstBuffer*                                      PixMap;
	char						File[FILENAME_MAX];
	int						height;
	int						width;
	int						padding;
} PsychMovieWriterRecordType;

static PsychMovieWriterRecordType moviewriterRecordBANK[PSYCH_MAX_MOVIEWRITERDEVICES];
static int moviewritercount = 0;
static psych_bool firsttime = TRUE;

/* Perform one context loop iteration (for bus message handling) if doWait == false,
 * or two seconds worth of iterations if doWait == true. This drives the message-bus
 * callback, so needs to be performed to get any error reporting etc.
 */
static int PsychGSProcessMovieContext(GMainLoop *loop, psych_bool doWait)
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

void PsychMovieWritingInit(void)
{
	int i;
	
	for (i = 0; i < PSYCH_MAX_MOVIEWRITERDEVICES; i++) {
		memset(&(moviewriterRecordBANK[i]), 0, sizeof(PsychMovieWriterRecordType));
	}
	
	moviewritercount = 0;
	firsttime = TRUE;
	return;
}

void PsychDeleteAllMovieWriters(void)
{
	int i;
		
	for (i = 0; i < PSYCH_MAX_MOVIEWRITERDEVICES; i++) {
		if (moviewriterRecordBANK[i].Movie) PsychFinalizeNewMovieFile(i);
	}
}

void PsychExitMovieWriting(void)
{
	if (firsttime) return;

	PsychDeleteAllMovieWriters();
	firsttime = TRUE;
	return;
}

PsychMovieWriterRecordType* PsychGetMovieWriter(int moviehandle, psych_bool unsafe)
{
	if (moviehandle < 0 || moviehandle >= PSYCH_MAX_MOVIEWRITERDEVICES) PsychErrorExitMsg(PsychError_user, "Invalid handle for moviewriter provided!");
	if (!unsafe && (NULL == moviewriterRecordBANK[moviehandle].Movie)) PsychErrorExitMsg(PsychError_user, "Invalid handle for moviewriter provided! No such writer open.");
	return(&(moviewriterRecordBANK[moviehandle]));
}

unsigned char*	PsychGetVideoFrameForMoviePtr(int moviehandle, unsigned int* twidth, unsigned int* theight)
{
	PsychMovieWriterRecordType* pwriterRec = PsychGetMovieWriter(moviehandle, FALSE);

	// Buffer already created?
	if (NULL == pwriterRec->PixMap) {
		// No. Let's create a suitable one:
		pwriterRec->PixMap = gst_buffer_try_new_and_alloc(pwriterRec->width * pwriterRec->height * 4 * 1);

		// Out of memory condition!
		if (NULL == pwriterRec->PixMap) return(NULL);
	}

	// Double-check:
	if (NULL == GST_BUFFER_DATA(pwriterRec->PixMap)) return(NULL);

	*twidth  = pwriterRec->width;
	*theight = pwriterRec->height - pwriterRec->padding; // Hack hack pwriterRec->padding !
	return((unsigned char*) GST_BUFFER_DATA(pwriterRec->PixMap));
}

int PsychAddVideoFrameToMovie(int moviehandle, int frameDurationUnits, psych_bool isUpsideDown)
{
	PsychMovieWriterRecordType* pwriterRec = PsychGetMovieWriter(moviehandle, FALSE);

	GstFlowReturn           ret;
	int			x, y, w, h;
	unsigned char*		pixptr   = (unsigned char*) GST_BUFFER_DATA(pwriterRec->PixMap);
	unsigned int*		wordptr  = (unsigned int*)  GST_BUFFER_DATA(pwriterRec->PixMap);
	unsigned int		*wordptr2, *wordptr1;
	unsigned int		dummy;

	if (NULL == pwriterRec->appsrc) return(0);
	if (NULL == pwriterRec->PixMap) return(0);

	if ((frameDurationUnits < 1) && (PsychPrefStateGet_Verbosity() > 1)) printf("PTB-WARNING:In AddFrameToMovie: Negative or zero 'frameduration' %i units for moviehandle %i provided! Sounds like trouble ahead.\n", frameDurationUnits, moviehandle);

	// Draw testpattern: Disabled at compile-time by default:
	if (FALSE) {
		for (y = 0; y < pwriterRec->height; y++) {
			for (x = 0; x < pwriterRec->width; x++) {
				*(pixptr++) = (unsigned char) 255; // alpha
				*(pixptr++) = (unsigned char) y; // Red
				*(pixptr++) = (unsigned char) x; // Green
				*(pixptr++) = (unsigned char) 0; // Blue
			}
		}
	}
	
	// Imagebuffer is upside-down: Need to flip it vertically:
	if (isUpsideDown) {
		h = pwriterRec->height - pwriterRec->padding; // Hack pwriterRec->padding !
		w = pwriterRec->width;
		wordptr1 = wordptr;
		for (y = 0; y < h/2; y++) {
			wordptr2 = wordptr;
			wordptr2 += ((h - 1 - y) * w);
			for (x = 0; x < w; x++) {
				dummy = *wordptr1;
				*(wordptr1++) = *wordptr2;
				*(wordptr2++) = dummy;				 
			}
		}
	}
	
	// Add encoded buffer to movie:
	g_signal_emit_by_name(pwriterRec->appsrc, "push-buffer", pwriterRec->PixMap, &ret);

	// Unref it - it is now owned and memory managed by the pipeline:
	gst_buffer_unref(pwriterRec->PixMap);

	// Drop our handle to it, so we can allocate a new one on demand:
	pwriterRec->PixMap = NULL;

	if (ret != GST_FLOW_OK) {
		// Oopsie! Error encountered - Abort.
		if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR:In AddFrameToMovie: Adding current frame to moviehandle %i failed [push-buffer returned error code %i]!\n", moviehandle, (int) ret);
		return((int) ret);
	}

	PsychGSProcessMovieContext(pwriterRec->Context, FALSE);

	if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG:In AddFrameToMovie: Added new videoframe with %i units duration and upsidedown = %i to moviehandle %i.\n", frameDurationUnits, (int) isUpsideDown, moviehandle);

	// Return success:
	return((int) ret);
}

/* Initiate pipeline state changes: Startup, Preroll, Playback, Pause, Standby, Shutdown. */
static psych_bool PsychMoviePipelineSetState(GstElement* camera, GstState state, double timeoutSecs)
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
		printf("PTB-INFO: Statechange completed with GST_STATE_CHANGE_NO_PREROLL.\n");
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

/* Receive messages from the pipeline message bus and handle them: */
static gboolean PsychMovieBusCallback(GstBus *bus, GstMessage *msg, gpointer dataptr)
{
  PsychMovieWriterRecordType* dev = (PsychMovieWriterRecordType*) dataptr;

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
	if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Videobus: Message EOS received.\n");
	dev->eos = TRUE;
    break;

    case GST_MESSAGE_WARNING: {
      gchar  *debug;
      GError *error;

      gst_message_parse_warning(msg, &error, &debug);
      if (PsychPrefStateGet_Verbosity() > 3) { 
	      printf("PTB-WARNING: GStreamer movie writing engine reports this warning:\n"
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
	      printf("PTB-ERROR: GStreamer movie writing engine reports this error:\n"
		     "           Error from element %s: %s\n", GST_OBJECT_NAME(msg->src), error->message);
	      printf("           Additional debug info: %s.\n\n", (debug) ? debug : "None");
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

int PsychCreateNewMovieFile(char* moviefile, int width, int height, double framerate, char* movieoptions)
{
	PsychMovieWriterRecordType*             pwriterRec = NULL;
	int					moviehandle = 0;
	GError                                  *myErr = NULL;
	char*					poption;
	char                                    codecString[1000];
	char                                    launchString[10000];
	int					dummyInt;
	float					dummyFloat;
	char					myfourcc[4];

	// Still capacity left?
	if (moviewritercount >= PSYCH_MAX_MOVIEWRITERDEVICES) PsychErrorExitMsg(PsychError_user, "Maximum number of movie writers exceeded. Please close some first!");

	// Find first free (i.e., NULL) slot and assign moviehandle:
	while ((pwriterRec = PsychGetMovieWriter(moviehandle, TRUE)) && pwriterRec->Movie) moviehandle++;

	if (firsttime) {
		// Make sure GStreamer is ready:
		PsychGSCheckInit("moviewriting");
		firsttime = FALSE;
	}

	// Store movie filename:
	strcpy(pwriterRec->File, moviefile);

	// Store width, height and padding:
	pwriterRec->height  = height;
	pwriterRec->width   = width;
	pwriterRec->padding = 0;
	pwriterRec->eos     = FALSE;

	// If no movieoptions specified, create default string for default
	// codec selection and configuration:
	if (strlen(movieoptions) == 0) movieoptions = strdup("DEFAULTenc");

/*
	// Assign numeric 32-bit FOURCC equivalent code to select codec:
	// This is optional. We default to kH264CodecType:
	if ((poption = strstr(movieoptions, "CodecFOURCCId="))) {
		if (sscanf(poption, "CodecFOURCCId=%i", &dummyInt) == 1) {
			pwriterRec->CodecType = (CodecType) dummyInt;
			if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Codec with FOURCC numeric id %i selected for encoding of movie %i [%s].\n", dummyInt, moviehandle, moviefile);
		}
		else PsychErrorExitMsg(PsychError_user, "Invalid CodecFOURCCId= parameter provided in movieoptions parameter. Parse error!");
	}

	// Assign 4 character string FOURCC code to select codec:
	if ((poption = strstr(movieoptions, "CodecFOURCC="))) {
		if (sscanf(poption, "CodecFOURCC=%c%c%c%c", &myfourcc[3], &myfourcc[2], &myfourcc[1], &myfourcc[0]) == 4) {
			pwriterRec->CodecType = (CodecType) (*((unsigned int*) (&myfourcc[0])));
			if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Codec with FOURCC '%c%c%c%c' = numeric id %i selected for encoding of movie %i [%s].\n", myfourcc[3], myfourcc[2], myfourcc[1], myfourcc[0], (int) pwriterRec->CodecType, moviehandle, moviefile);
		}
		else PsychErrorExitMsg(PsychError_user, "Invalid CodecFOURCC= parameter provided in movieoptions parameter. Must be exactly 4 characters! Parse error!");
	}
*/

/*
	// Assign numeric encoding quality level:
	// This is optional. We default to "normal quality":
	if ((poption = strstr(movieoptions, "EncodingQuality="))) {
		if (sscanf(poption, "EncodingQuality=%f", &dummyFloat) == 1) {
			// Map floating point quality level between 0.0 and 1.0 to 10 discrete levels:
			dummyInt = (int)(10.0 * dummyFloat + 0.5);
			dummyInt = (dummyInt < 0)  ?  0 : dummyInt;
			dummyInt = (dummyInt > 10) ? 10 : dummyInt;
			
			// Assign one of Quicktime's 6 code quality setting to the 10 levels:
			switch(dummyInt) {
				case 0:
					pwriterRec->CodecQuality = codecMinQuality;
				break;

				case 1:
					pwriterRec->CodecQuality = codecMinQuality;
				break;

				case 2:
					pwriterRec->CodecQuality = codecLowQuality;
				break;

				case 3:
					pwriterRec->CodecQuality = codecLowQuality;
				break;

				case 4:
					pwriterRec->CodecQuality = codecNormalQuality;
				break;

				case 5:
					pwriterRec->CodecQuality = codecNormalQuality;
				break;

				case 6:
					pwriterRec->CodecQuality = codecNormalQuality;
				break;

				case 7:
					pwriterRec->CodecQuality = codecHighQuality;
				break;

				case 8:
					pwriterRec->CodecQuality = codecHighQuality;
				break;

				case 9:
					pwriterRec->CodecQuality = codecMaxQuality;
				break;

				case 10:
					pwriterRec->CodecQuality = codecLosslessQuality;
				break;
				
			}
			
			if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Encoding quality level %i selected for encoding of movie %i [%s].\n", dummyInt, moviehandle, moviefile);
		}
		else PsychErrorExitMsg(PsychError_user, "Invalid EncodingQuality= parameter provided in movieoptions parameter. Parse error!");
	}
*/

	// Check for valid parameters. Also warn if some parameters are borderline for certain codecs:
	if ((framerate < 1) && (PsychPrefStateGet_Verbosity() > 1)) printf("PTB-WARNING:In CreateMovie: Negative or zero 'framerate' %f units for moviehandle %i provided! Sounds like trouble ahead.\n", (float) framerate, moviehandle);
	if (width < 1) PsychErrorExitMsg(PsychError_user, "In CreateMovie: Invalid zero or negative 'width' for video frame size provided!");
	if ((width < 4) && (PsychPrefStateGet_Verbosity() > 1)) printf("PTB-WARNING:In CreateMovie: 'width' of %i pixels for moviehandle %i provided! Some video codecs may malfunction with such a small width.\n", width, moviehandle);
	if ((width % 4 != 0) && (PsychPrefStateGet_Verbosity() > 1)) printf("PTB-WARNING:In CreateMovie: 'width' of %i pixels for moviehandle %i provided! Some video codecs may malfunction with a width which is not a multiple of 4 or 16.\n", width, moviehandle);
	if (height < 1) PsychErrorExitMsg(PsychError_user, "In CreateMovie: Invalid zero or negative 'height' for video frame size provided!");
	if ((height < 4) && (PsychPrefStateGet_Verbosity() > 1)) printf("PTB-WARNING:In CreateMovie: 'height' of %i pixels for moviehandle %i provided! Some video codecs may malfunction with such a small height.\n", height, moviehandle);

	// Find the gst-launch style string for audiosource, aud
	if (!PsychGetCodecLaunchLineFromString(movieoptions, &(codecString[0]))) {
		// No config for this format possible:
		if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR:In CreateMovie: Creating movie file with handle %i [%s] failed: Could not find matching codec setup.\n", moviehandle, moviefile);
		goto bail;
	}

	// Build final launch string:
	sprintf(launchString, "appsrc name=ptbappsrc do-timestamp=0 stream-type=0 max-bytes=0 block=1 is-live=0 emit-signals=0 ! capsfilter caps=\"video/x-raw-rgb, bpp=(int)32, depth=(int)32, endianess=(int)4321, red_mask=(int)16711680, green_mask=(int)65280, blue_mask=(int)255, width=(int)%i, height=(int)%i, framerate=%i/1 \" ! videorate ! ffmpegcolorspace ! %s ! filesink name=ptbfilesink async=0 location=%s ", width, height, ((int) (framerate + 0.5)), codecString, moviefile);

	// Create a movie file for the destination movie:
	if (PsychPrefStateGet_Verbosity() > 3) {
		printf("PTB-INFO: Movie writing pipeline gst-launch line is:\n");
		printf("gst-launch -e %s\n", launchString);
	}

	// Build pipeline from launch string:
	pwriterRec->Movie = gst_parse_launch((const gchar*) launchString, &myErr);
	if ((NULL == pwriterRec->Movie) || myErr) {
		if (PsychPrefStateGet_Verbosity() > 0) {
			printf("PTB-ERROR: In CreateMovie: Creating movie file with handle %i [%s] failed: Could not build pipeline.\n", moviehandle, moviefile);
			printf("PTB-ERROR: Parameters were: %s\n", movieoptions);
			printf("PTB-ERROR: Launch string was: %s\n", launchString);
			printf("PTB-ERROR: GStreamer error message was: %s\n", (char*) myErr->message);
		}
		goto bail;
	}

	// Get handle to appsrc:
	pwriterRec->appsrc = gst_bin_get_by_name(GST_BIN(pwriterRec->Movie), (const gchar *) "ptbappsrc");
	if (NULL == pwriterRec->appsrc) {
		if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: In CreateMovie: Creating movie file with handle %i [%s] failed: Could not find appsrc pipeline element.\n", moviehandle, moviefile);
		goto bail;
	}

	pwriterRec->Context = g_main_loop_new (NULL, FALSE);
	pwriterRec->bus = gst_pipeline_get_bus (GST_PIPELINE(pwriterRec->Movie));
	gst_bus_add_watch(pwriterRec->bus, (GstBusFunc) PsychMovieBusCallback, pwriterRec);
	gst_object_unref(pwriterRec->bus);

	// Start the pipeline:
	if (!PsychMoviePipelineSetState(pwriterRec->Movie, GST_STATE_PLAYING, 10)) {
		if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: In CreateMovie: Creating movie file with handle %i [%s] failed:  Failed to start movie encoding pipeline!\n", moviehandle, moviefile);
		goto bail;
	}

	PsychGSProcessMovieContext(pwriterRec->Context, FALSE);

	// Increment count of open movie writers:
	moviewritercount++;
	
	if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Moviehandle %i successfully opened for movie writing into file '%s'.\n", moviehandle, moviefile);

	// Return new handle:
	return(moviehandle);

bail:
	if (pwriterRec->appsrc) gst_object_unref(GST_OBJECT(pwriterRec->appsrc));
	pwriterRec->appsrc = NULL;

	if (pwriterRec->Movie) gst_object_unref(GST_OBJECT(pwriterRec->Movie));
	pwriterRec->Movie = NULL;

	if (pwriterRec->Context) g_main_loop_unref(pwriterRec->Context);
	pwriterRec->Context = NULL;

	// Return failure:
	return(-1);
}

int PsychFinalizeNewMovieFile(int movieHandle)
{
	int myErr = 0;
	GError *ret = NULL;

	PsychMovieWriterRecordType* pwriterRec = PsychGetMovieWriter(movieHandle, FALSE);

	if (NULL == pwriterRec->appsrc) return(0);

	// Release any pending buffers:
	if (pwriterRec->PixMap) gst_buffer_unref(pwriterRec->PixMap);
	pwriterRec->PixMap = NULL;

	PsychGSProcessMovieContext(pwriterRec->Context, FALSE);

	// Send EOS signal downstream:
	g_signal_emit_by_name(pwriterRec->appsrc, "end-of-stream", &ret);

	// Wait for eos flag to turn TRUE due to bus callback receiving the
	// downstream EOS event that we just sent out:
	while (!pwriterRec->eos) {
		PsychGSProcessMovieContext(pwriterRec->Context, FALSE);
		PsychYieldIntervalSeconds(0.010);
	}

	// Yield another 10 msecs after EOS signalled, just to be safe:
	PsychYieldIntervalSeconds(0.010);

	// Pause the encoding pipeline:
	if (!PsychMoviePipelineSetState(pwriterRec->Movie, GST_STATE_PAUSED, 10)) {
		if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed to pause movie encoding pipeline at close time!!\n");
	}

	// Stop the encoding pipeline:
	if (!PsychMoviePipelineSetState(pwriterRec->Movie, GST_STATE_READY, 10)) {
		if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed to stop movie encoding pipeline at close time!!\n");
	}

	// Shutdown and release encoding pipeline:
	if (!PsychMoviePipelineSetState(pwriterRec->Movie, GST_STATE_NULL, 10)) {
		if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed to shutdown movie encoding pipeline at close time!!\n");
	}

	gst_object_unref(GST_OBJECT(pwriterRec->Movie));
	pwriterRec->Movie = NULL;

	if (pwriterRec->appsrc) gst_object_unref(GST_OBJECT(pwriterRec->appsrc));
	pwriterRec->appsrc = NULL;

	// Delete video context:
	if (pwriterRec->Context) g_main_loop_unref(pwriterRec->Context);
	pwriterRec->Context = NULL;

	// gst_object_unref(GST_OBJECT(pwriterRec->filesink));
	// pwriterRec->filesink = NULL;

	// Decrement count of active writers:
	moviewritercount--;

	// Return success/fail status:
	if ((myErr == 0) && (PsychPrefStateGet_Verbosity() > 3)) printf("PTB-INFO: Moviehandle %i successfully closed and movie written to filesystem.\n", movieHandle);

	return(myErr == 0);
}

// End of GStreamer routines.

#else

// This is Linux, but without GStreamer support compiled in: Implement dummy functions:
void PsychMovieWritingInit(void) { return; }
void PsychExitMovieWriting(void) { return; }
void PsychDeleteAllMovieWriters(void) { return; }
unsigned char*	PsychGetVideoFrameForMoviePtr(int moviehandle, unsigned int* twidth, unsigned int* theight) { return(NULL); }
int PsychAddVideoFrameToMovie(int moviehandle, int frameDurationUnits, psych_bool isUpsideDown) { return(0); }
int PsychCreateNewMovieFile(char* moviefile, int width, int height, double framerate, char* movieoptions)
{
	PsychErrorExitMsg(PsychError_unimplemented, "Sorry, movie writing and editing support not yet implemented on this operating system.");
}

int PsychFinalizeNewMovieFile(int movieHandle)
{
	PsychErrorExitMsg(PsychError_unimplemented, "Sorry, movie writing and editing support not yet implemented on this operating system.");
}

#endif
#endif
