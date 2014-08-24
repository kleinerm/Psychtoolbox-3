/*
    Common/Screen/PsychMovieWritingSupportGStreamer010.c

    PLATFORMS:

        All.

    AUTHORS:

        Mario Kleiner           mk              mario.kleiner@tuebingen.mpg.de

    HISTORY:

        06/06/11                mk              Wrote it.

    DESCRIPTION:

        THIS IS THE LEGACY VERSION OF GStreamer-0.10 -- It's dead Jim!

        Psychtoolbox functions for dealing with GStreamer movie editing.

    NOTES:

*/

#include "Screen.h"

#ifdef PTB_USE_GSTREAMER

// GStreamer includes:
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#if !GST_CHECK_VERSION(1,0,0)

// PsychGetCodecLaunchLineFromString() - Helper function for GStreamer based movie writing.
// Defined in PsychVideoCaptureSupport.h: psych_bool PsychGetCodecLaunchLineFromString(char* codecSpec, char* launchString);

// GStreamer implementation of movie writing support:

// Record which defines all state for a capture device:
typedef struct {
    volatile psych_bool                             eos;
    GMainLoop*                                      Context;
    GstElement*                                     Movie;
    GstElement*                                     ptbvideoappsrc;
    GstElement*                                     ptbaudioappsrc;
    GstElement*                                     ptbvideoappsink;
    GstBus*                                         bus;
    GstBuffer*                                      PixMap;
    guint32                                         CodecType;
    char                                            File[FILENAME_MAX];
    int                                             height;
    int                                             width;
    unsigned int                                    numChannels;
    unsigned int                                    bitdepth;
    psych_bool                                      useVariableFramerate;
} PsychMovieWriterRecordType;

static PsychMovieWriterRecordType moviewriterRecordBANK[PSYCH_MAX_MOVIEWRITERDEVICES];
static int moviewritercount = 0;
static psych_bool firsttime = TRUE;

// Use direct method of checking GStreamer bus, which doesn't interfere with Octave + QT-GUI:
static const psych_bool useNewBusCheck = TRUE;

// Forward declaration:
static gboolean PsychMovieBusCallback(GstBus *bus, GstMessage *msg, gpointer dataptr);

/* Perform context loop iterations (for bus message handling) if doWait == false,
 * as long as there is work to do, or at least two seconds worth of iterations
 * if doWait == true. This drives the message-bus callback, so needs to be
 * performed to get any error reporting etc.
 */
static int PsychGSProcessMovieContext(PsychMovieWriterRecordType *movie, psych_bool doWait)
{
    GstBus* bus;
    GstMessage *msg;
    psych_bool workdone = FALSE;
    double tdeadline, tnow;
    GMainLoop *loop;
    PsychGetAdjustedPrecisionTimerSeconds(&tdeadline);
    tnow = tdeadline;
    tdeadline+=2.0;

    if (useNewBusCheck) {
        // New style:
        bus = gst_pipeline_get_bus(GST_PIPELINE(movie->Movie));
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
            PsychMovieBusCallback(bus, msg, movie);
            gst_message_unref(msg);
            msg = gst_bus_pop(bus);
        }

        gst_object_unref(bus);
    }
    else {
        // Old style: Doesn't work with Octave 3.8 + GUI on Linux:
        loop = movie->Context;
        if (NULL == loop) return(0);

        // If doWait, try to perform iterations until 2 seconds elapsed or at least one event handled:
        while (doWait && (tnow < tdeadline)) {
            // Perform non-blocking work iteration:
            if (g_main_context_iteration(g_main_loop_get_context(loop), false)) {
                workdone = TRUE;
                break;
            }

            // Update time:
            PsychYieldIntervalSeconds(0.010);
            PsychGetAdjustedPrecisionTimerSeconds(&tnow);
        }

        // Perform work iterations of the event context as long as events are available, but don't block:
        while (g_main_context_iteration(g_main_loop_get_context(loop), false)) { workdone = TRUE; }
    }

    return(workdone);
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

// Pulls next GStreamer videobuffer from appsink, if any, and copies its image data into a new malloc'd buffer. Caller has to free() the returned buffer.
// Used mostly by the libdc1394 video capture engine for retrieval of feedback data:
unsigned char* PsychMovieCopyPulledPipelineBuffer(int moviehandle, unsigned int* twidth, unsigned int* theight, unsigned int* numChannels, unsigned int* bitdepth, double* timestamp)
{
    unsigned char* imgdata;
    unsigned int count;

    // Retrieve movie record:
    PsychMovieWriterRecordType* pwriterRec = PsychGetMovieWriter(moviehandle, FALSE);

    // Pull next buffer from appsink, if any:
    GstBuffer *videoBuffer = NULL;

    // Return NULL if appsink doesn't exist or is end-of-stream already:
    if ((pwriterRec->ptbvideoappsink == NULL) || !GST_IS_APP_SINK(pwriterRec->ptbvideoappsink) || gst_app_sink_is_eos(GST_APP_SINK(pwriterRec->ptbvideoappsink))) return(NULL);

    // Pull next buffer from appsink:
    videoBuffer = gst_app_sink_pull_buffer(GST_APP_SINK(pwriterRec->ptbvideoappsink));
    
    // Double-check: Buffer valid with data?
    if ((NULL == videoBuffer) || (NULL == GST_BUFFER_DATA(videoBuffer))) return(NULL);

    // Valid assign properties:
    *twidth  = pwriterRec->width;
    *theight = pwriterRec->height;
    *numChannels = pwriterRec->numChannels;
    *bitdepth = pwriterRec->bitdepth;
    *timestamp = (double) GST_BUFFER_TIMESTAMP(videoBuffer) / (double) 1e9;

    // Copy data into new malloc'ed buffer:
    count = (pwriterRec->width * pwriterRec->height * pwriterRec->numChannels * ((pwriterRec->bitdepth > 8) ? 2 : 1));
    
    // Allocate target buffer for most recent captured frame from video recorder thread:
    imgdata = (unsigned char*) malloc(count);
    
    // Copy image into it:
    memcpy(imgdata, GST_BUFFER_DATA(videoBuffer), count);

    // Release GstBuffer to appsink for recycling:
    gst_buffer_unref(videoBuffer);
    videoBuffer = NULL;

    return(imgdata);
}

unsigned char* PsychGetVideoFrameForMoviePtr(int moviehandle, unsigned int* twidth, unsigned int* theight, unsigned int* numChannels, unsigned int* bitdepth)
{
    PsychMovieWriterRecordType* pwriterRec = PsychGetMovieWriter(moviehandle, FALSE);

    // Buffer already created?
    if (NULL == pwriterRec->PixMap) {
        // No. Let's create a suitable one:
        pwriterRec->PixMap = gst_buffer_try_new_and_alloc(pwriterRec->width * pwriterRec->height * pwriterRec->numChannels * (pwriterRec->bitdepth / 8));

        // Out of memory condition!
        if (NULL == pwriterRec->PixMap) return(NULL);
    }

    // Double-check:
    if (NULL == GST_BUFFER_DATA(pwriterRec->PixMap)) return(NULL);

    *twidth  = pwriterRec->width;
    *theight = pwriterRec->height;
    *numChannels = pwriterRec->numChannels;
    *bitdepth = pwriterRec->bitdepth;

    return((unsigned char*) GST_BUFFER_DATA(pwriterRec->PixMap));
}

int PsychAddVideoFrameToMovie(int moviehandle, int frameDurationUnits, psych_bool isUpsideDown, double frameTimestamp)
{
    PsychMovieWriterRecordType* pwriterRec = PsychGetMovieWriter(moviehandle, FALSE);
    GstBuffer*          refBuffer = NULL;
    GstBuffer*          curBuffer = NULL;
    GstFlowReturn       ret;
    int                 x, y, w, h;
    unsigned char*      pixptr;
    unsigned int*       wordptr;
    unsigned int        *wordptr2, *wordptr1;
    unsigned char       *byteptr2, *byteptr1;
    unsigned int        dummy;
    unsigned char       dummyb;
    int                 bframeDurationUnits = frameDurationUnits;

    if (NULL == pwriterRec->ptbvideoappsrc) return(0);
    if (NULL == pwriterRec->PixMap) return(0);

    if ((frameDurationUnits < 1) && (PsychPrefStateGet_Verbosity() > 1)) printf("PTB-WARNING:In AddFrameToMovie: Negative or zero 'frameduration' %i units for moviehandle %i provided! Sounds like trouble ahead.\n", frameDurationUnits, moviehandle);

    // Assign frameTimestamp (if valid aka greater than zero) as video buffer timestamp, after conversion into nanoseconds:
    // We can only timestamp if variable framerate recording is enabled, ie., the "videorate" converter element isn't used,
    // as that element chokes on many frameTimestamp's.
    if (pwriterRec->useVariableFramerate && (frameTimestamp >= 0)) GST_BUFFER_TIMESTAMP(pwriterRec->PixMap) = (psych_uint64) (frameTimestamp * 1e9);
    
    pixptr   = (unsigned char*) GST_BUFFER_DATA(pwriterRec->PixMap);
    wordptr  = (unsigned int*)  GST_BUFFER_DATA(pwriterRec->PixMap);

    // Is Imagebuffer upside-down? If so, need to flip it vertically:
    if (isUpsideDown) {
        // RGBA8 format?
        if ((pwriterRec->numChannels == 4) && (pwriterRec->bitdepth == 8)) {
            // Yes. Can use optimized copy of uint32 units:
            h = pwriterRec->height;
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
        else {
            // No. Could be 1, 2, 3, 6 or 8 bytes per pixel. Just use a
            // robust but slightly less efficient byte-wise copy:
            h = pwriterRec->height;
            w = pwriterRec->width;
            w = w * pwriterRec->numChannels * pwriterRec->bitdepth / 8;
            byteptr1 = pixptr;
            for (y = 0; y < h/2; y++) {
                byteptr2 = pixptr;
                byteptr2 += ((h - 1 - y) * w);
                for (x = 0; x < w; x++) {
                    dummyb = *byteptr1;
                    *(byteptr1++) = *byteptr2;
                    *(byteptr2++) = dummyb;
                }
            }
        }
    }
    
    // Make backup copy of buffer for replication if needed:
    if (frameDurationUnits > 1) {
        refBuffer = gst_buffer_copy(pwriterRec->PixMap);
    }

    // Add encoded buffer to movie:
    g_signal_emit_by_name(pwriterRec->ptbvideoappsrc, "push-buffer", pwriterRec->PixMap, &ret);

    // Unref it - it is now owned and memory managed by the pipeline:
    gst_buffer_unref(pwriterRec->PixMap);

    // Drop our handle to it, so we can allocate a new one on demand:
    pwriterRec->PixMap = NULL;

    // A dumb way to let this buffer last frameDurationUnits > 1 instead of
    // the default typical frameDurationUnits == 1. We simply create and
    // push frameDurationUnits-1 extra identicaly copies of the buffer. This
    // will not win the "computational efficiency award 2011", but should be robust.
    if (frameDurationUnits > 1) {
        // One already done:
        frameDurationUnits--;
        
        // Repeat for remaining ones:
        while ((frameDurationUnits > 0) && (ret == GST_FLOW_OK)) {
            // Create a new copy:
            curBuffer = gst_buffer_copy(refBuffer);
            
            // Add copied buffer to movie:
            g_signal_emit_by_name(pwriterRec->ptbvideoappsrc, "push-buffer", curBuffer, &ret);
    
            // Unref it - it is now owned and memory managed by the pipeline:
            gst_buffer_unref(curBuffer);
            curBuffer = NULL;

            // One less...
            frameDurationUnits--;
        }
    }
    
    if (refBuffer) gst_buffer_unref(refBuffer);
    refBuffer = NULL;

    if (ret != GST_FLOW_OK) {
        // Oopsie! Error encountered - Abort.
        if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR:In AddFrameToMovie: Adding current frame to moviehandle %i failed [push-buffer returned error code %i]!\n", moviehandle, (int) ret);
        return((int) ret);
    }

    PsychGSProcessMovieContext(pwriterRec, FALSE);

    if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG:In AddFrameToMovie: Added new videoframe with %i units duration and upsidedown = %i to moviehandle %i.\n", bframeDurationUnits, (int) isUpsideDown, moviehandle);

    // Return success:
    return((int) ret);
}

psych_bool PsychAddAudioBufferToMovie(int moviehandle, unsigned int nrChannels, unsigned int nrSamples, double* buffer)
{
    PsychMovieWriterRecordType* pwriterRec = PsychGetMovieWriter(moviehandle, FALSE);

    GstFlowReturn       ret;
    float*              fwordptr;
    float               v;
    unsigned int        n, i;
    GstBuffer*          pushBuffer;

    // Child protection: Audio writing enabled for this movie?
    if (NULL == pwriterRec->ptbaudioappsrc) {
        PsychErrorExitMsg(PsychError_user, "Tried to add audio data to a movie which was created without an audio track.");
    }

    // nrChannels and nrSamples are already validated by high level code.
    // Just calculate total sample count and required buffer size:
    n = nrChannels * nrSamples;

    // Create GstBuffer for audio data:
    pushBuffer = gst_buffer_try_new_and_alloc(n * sizeof(float));

    // Out of memory condition!
    if (NULL == pushBuffer) {
        PsychErrorExitMsg(PsychError_outofMemory, "Out of memory when trying to add audio data to movie! (Part I)");
        return(FALSE);
    }

    // Double-check:
    if (NULL == GST_BUFFER_DATA(pushBuffer)) {
        PsychErrorExitMsg(PsychError_outofMemory, "Out of memory when trying to add audio data to movie! (Part II)");
        return(FALSE);
    }

    // Convert and copy sample data:
    fwordptr = (float*) GST_BUFFER_DATA(pushBuffer);
    for (i = 0; i < n; i++) {
        // Fetch and convert from double to float:
        v = (float) *(buffer++);;

        // Clip:
        if (v < -1.0) v = -1.0;
        if (v > +1.0) v = +1.0;

        // Push to float buffer:
        *(fwordptr++) = v;
    }

    // Add encoded buffer to movie:
    g_signal_emit_by_name(pwriterRec->ptbaudioappsrc, "push-buffer", pushBuffer, &ret);

    // Unref it - it is now owned and memory managed by the pipeline:
    gst_buffer_unref(pushBuffer);
    pushBuffer = NULL;

    if (ret != GST_FLOW_OK) {
        // Oopsie! Error encountered - Abort.
        if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR:In AddAudioBufferToMovie: Adding current audio buffer to moviehandle %i failed [push-buffer returned error code %i]!\n", moviehandle, (int) ret);
        return(FALSE);
    }

    // Do a bit of event processing for handling of potential GStreamer messages:
    PsychGSProcessMovieContext(pwriterRec, FALSE);

    if (PsychPrefStateGet_Verbosity() > 5) printf("PTB-DEBUG:In AddAudioBufferToMovie: Added new audio buffer to moviehandle %i.\n", moviehandle);

    // Return success:
    return(TRUE);
}

/* Initiate pipeline state changes: Startup, Preroll, Playback, Pause, Standby, Shutdown. */
static psych_bool PsychMoviePipelineSetState(GstElement* camera, GstState state, double timeoutSecs)
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
  if (PsychPrefStateGet_Verbosity() > 11) printf("PTB-DEBUG: PsychMovieWriterBusCallback: Msg source name and type: %s : %s\n", GST_MESSAGE_SRC_NAME(msg), GST_MESSAGE_TYPE_NAME(msg));

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
      if (PsychPrefStateGet_Verbosity() > 4) printf("PTB-DEBUG: Moviewriter bus: Message EOS received.\n");
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

            // Special tips for the challenged:
            if (strstr(error->message, "property") || (debug && strstr(debug, "property"))) {
                // Bailed due to unsupported x264enc parameter "speed-preset". Can be solved by upgrading
                // GStreamer or the OS or the VideoCodec= override:
                printf("PTB-TIP: One reason this may have failed is because your GStreamer codec installation is too outdated.\n");
                printf("PTB-TIP: Either upgrade your GStreamer (plugin) installation to a more recent version,\n");
                printf("PTB-TIP: or upgrade your operating system (e.g., Ubuntu 10.10 'Maverick Meercat' and later are fine).\n");
                printf("PTB-TIP: A recent GStreamer installation is required to use all features and get optimal performance.\n");
                printf("PTB-TIP: As a workaround, you can manually specify all codec settings, leaving out the unsupported\n");
                printf("PTB-TIP: option. See 'help VideoRecording' on how to do that.\n\n");
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

int PsychCreateNewMovieFile(char* moviefile, int width, int height, double framerate, int numChannels, int bitdepth, char* movieoptions, char* feedbackString)
{
    PsychMovieWriterRecordType*             pwriterRec = NULL;
    int                                     moviehandle = 0;
    GError                                  *myErr = NULL;
    char*                                   poption;
    char                                    codecString[1000];
    char                                    capsString[1000];
    char                                    capsForCodecString[1000];
    char                                    videorateString[100];
    char                                    launchString[10000];
    int                                     dummyInt;
    float                                   dummyFloat;
    char                                    myfourcc[5];
    psych_bool                              doAudio = FALSE;
    psych_bool                              useOwn16bpc = FALSE;

    // Validate number of color channels: We support 1, 3 or 4:
    if (numChannels != 1 && numChannels != 3 && numChannels != 4) PsychErrorExitMsg(PsychError_internal, "Invalid number of channels parameter provided. Not 1, 3 or 4!");

    // Validate number of bits per component: We support 8 bpc or 16 bpc:
    if (bitdepth != 8 && bitdepth != 16) PsychErrorExitMsg(PsychError_internal, "Invalid number of bits per channel bpc parameter provided. Not 8 or 16!");

    // Still capacity left?
    if (moviewritercount >= PSYCH_MAX_MOVIEWRITERDEVICES) PsychErrorExitMsg(PsychError_user, "Maximum number of movie writers exceeded. Please close some first!");

    // Find first free (i.e., NULL) slot and assign moviehandle:
    while ((pwriterRec = PsychGetMovieWriter(moviehandle, TRUE)) && pwriterRec->Movie) moviehandle++;

    if (firsttime) {
        // Make sure GStreamer is ready:
        PsychGSCheckInit("movie writing");
        firsttime = FALSE;
    }

    // Store movie filename:
    strcpy(pwriterRec->File, (moviefile) ? moviefile : "");

    // Store width, height, numChannels, bitdepth:
    pwriterRec->height  = height;
    pwriterRec->width   = width;
    pwriterRec->numChannels = (unsigned int) numChannels;
    pwriterRec->bitdepth = (unsigned int) bitdepth;
    pwriterRec->eos     = FALSE;
    pwriterRec->useVariableFramerate = FALSE;

    // If no movieoptions specified, create default string for default
    // codec selection and configuration:
    if (strlen(movieoptions) == 0) {
        // No options provided. Select default encoder with default settings:
        movieoptions = strdup("DEFAULTenc");
    } else if ((poption = strstr(movieoptions, ":CodecSettings="))) {
        // Replace ':' with a zero in movieoptions, so it gets null-terminated:
        movieoptions = poption;
        *movieoptions = 0;

        // Move after null-terminator:
        movieoptions++;

        // Replace the ':CodecSettings=' with the special keyword 'DEFAULTenc', so
        // so the default video codec is chosen, but the given settings override its
        // default parameters.
        strncpy(movieoptions, "DEFAULTenc    ", strlen("DEFAULTenc    "));

        if (strlen(movieoptions) == 0) PsychErrorExitMsg(PsychError_user, "Invalid (empty) :CodecSettings= parameter specified. Aborted.");
    } else if ((poption = strstr(movieoptions, ":CodecType="))) {
        // Replace ':' with a zero in movieoptions, so it gets null-terminated
        // and only points to the actual movie filename:
        movieoptions = poption;
        *movieoptions = 0;

        // Advance movieoptions to point to the actual codec spec string:
        movieoptions+= 11;

        if (strlen(movieoptions) == 0) PsychErrorExitMsg(PsychError_user, "Invalid (empty) :CodecType= parameter specified. Aborted.");
    }

    // Assign numeric 32-bit FOURCC equivalent code to select codec:
    // This is optional. We default to kH264CodecType:
    if ((poption = strstr(movieoptions, "CodecFOURCCId="))) {
        if (sscanf(poption, "CodecFOURCCId=%i", &dummyInt) == 1) {
            pwriterRec->CodecType = dummyInt;
            if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Codec with FOURCC numeric id %i [%" GST_FOURCC_FORMAT "] requested for encoding of movie %i [%s].\n", dummyInt, GST_FOURCC_ARGS(dummyInt), moviehandle, moviefile);
            if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Codec selection by FOURCC not yet supported. FOURCC code ignored!\n");
        }
        else PsychErrorExitMsg(PsychError_user, "Invalid CodecFOURCCId= parameter provided in movieoptions parameter. Parse error!");
    }

    // Assign 4 character string FOURCC code to select codec:
    if ((poption = strstr(movieoptions, "CodecFOURCC="))) {
        if (sscanf(poption, "CodecFOURCC=%c%c%c%c", &myfourcc[0], &myfourcc[1], &myfourcc[2], &myfourcc[3]) == 4) {
            myfourcc[4] = 0;
            dummyInt = (int) GST_STR_FOURCC (myfourcc);
            pwriterRec->CodecType = dummyInt;
            if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Codec with FOURCC numeric id %i [%" GST_FOURCC_FORMAT "] requested for encoding of movie %i [%s].\n", dummyInt, GST_FOURCC_ARGS(dummyInt), moviehandle, moviefile);
            if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Codec selection by FOURCC not yet supported. FOURCC code ignored!\n");
        }
        else PsychErrorExitMsg(PsychError_user, "Invalid CodecFOURCC= parameter provided in movieoptions parameter. Must be exactly 4 characters! Parse error!");
    }

    // Assign numeric encoding quality level:
    // This is optional. We default to "normal quality":
    if ((poption = strstr(movieoptions, "EncodingQuality="))) {
        if ((sscanf(poption, "EncodingQuality=%f", &dummyFloat) == 1) && (dummyFloat >= 0) && (dummyFloat <= 1)) {
            // Map floating point quality level between 0.0 and 1.0 to 10 discrete levels:
            if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: Encoding quality level %f selected for encoding of movie %i [%s].\n", dummyFloat, moviehandle, moviefile);

            // Rewrite "EncodingQuality=" string into "VideoQuality=" string, with proper
            // padding:      "EncodingQuality="
            // This way EncodingQuality in Quicktime lingo corresponds to
            // VideoQuality in GStreamer lingo:
            strncpy(poption, "   Videoquality=", strlen("   Videoquality="));
        }
        else PsychErrorExitMsg(PsychError_user, "Invalid EncodingQuality= parameter provided in movieoptions parameter. Parse error or out of valid 0 - 1 range!");
    }

    // Check for valid parameters. Also warn if some parameters are borderline for certain codecs:
    if ((framerate < 1) && (PsychPrefStateGet_Verbosity() > 1)) printf("PTB-WARNING:In CreateMovie: Negative or zero 'framerate' %f units for moviehandle %i provided! Sounds like trouble ahead.\n", (float) framerate, moviehandle);
    if (width < 1) PsychErrorExitMsg(PsychError_user, "In CreateMovie: Invalid zero or negative 'width' for video frame size provided!");
    if ((width < 4) && (PsychPrefStateGet_Verbosity() > 1)) printf("PTB-WARNING:In CreateMovie: 'width' of %i pixels for moviehandle %i provided! Some video codecs may malfunction with such a small width.\n", width, moviehandle);
    if ((width % 4 != 0) && (PsychPrefStateGet_Verbosity() > 1)) printf("PTB-WARNING:In CreateMovie: 'width' of %i pixels for moviehandle %i provided! Some video codecs may malfunction with a width which is not a multiple of 4 or 16.\n", width, moviehandle);
    if (height < 1) PsychErrorExitMsg(PsychError_user, "In CreateMovie: Invalid zero or negative 'height' for video frame size provided!");
    if ((height < 4) && (PsychPrefStateGet_Verbosity() > 1)) printf("PTB-WARNING:In CreateMovie: 'height' of %i pixels for moviehandle %i provided! Some video codecs may malfunction with such a small height.\n", height, moviehandle);

    // Use of variable framerate requested? If so, we use the GstBuffer timestamps instead
    // of synthetic timestamps generated by the videorate converter and derived from fps:
    if (strstr(movieoptions, "UseVFR")) {
        videorateString[0] = 0;
        pwriterRec->useVariableFramerate = TRUE;
    }
    else {
        sprintf(videorateString, "videorate ! ");
        pwriterRec->useVariableFramerate = FALSE;
    }

    // Full GStreamer launch line a la gst-launch command provided?
    if (strstr(movieoptions, "gst-launch")) {
        // Yes: We use movieoptions directly as launch line:
        movieoptions = strstr(movieoptions, "gst-launch");

        // Move string pointer behind the "gst-launch" word (plus a blank):
        movieoptions+= strlen("gst-launch ");

        // Can directly use this:
        sprintf(launchString, "%s", movieoptions);

        // With audio track?
        if (strstr(movieoptions, "name=ptbaudioappsrc")) doAudio = TRUE;
    }
    else {
        // No: Do our own parsing and setup:

        // No special capsfilter string for Codec by default, just a ffmpecolorspace converter
        // which will determine proper src/sink caps automagically:
        sprintf(capsForCodecString, "ffmpegcolorspace ! ");

        // Find the gst-launch style string for codecs and muxers:
        codecString[0] = 0;
        if (moviefile && (strlen(moviefile) > 0) && !PsychGetCodecLaunchLineFromString(movieoptions, &(codecString[0]))) {
            // No config for this format possible:
            if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR:In CreateMovie: Creating movie file with handle %i [%s] failed: Could not find matching codec setup.\n", moviehandle, moviefile);
            goto bail;
        }

        // With audio track?
        if (strstr(movieoptions, "AddAudioTrack")) doAudio = TRUE;

        // With our own pseudo 16 bpc format which squeezes 16 bpc content into 8 bpc encodings?
        if (strstr(movieoptions, "UsePTB16BPC")) useOwn16bpc = TRUE;

        // Define filter-caps aka capsfilter for appsrc, to tell the encoding pipeline what kind of
        // video format is delivered by the appsrc:
        if (bitdepth == 8) {
            // 8 bpc format: We handle Luminance8/Raw8, RGB8 or RGBA8:
            switch (numChannels) {
                case 1:
                    // 8 bpc gray or raw:
                    sprintf(capsString, "video/x-raw-gray, bpp=(int)8, depth=(int)8");
                    // If the ffenc_huffyuv or ffenc_ljpeg encoder is in use, we need some special caps after the colorspace converter to get lossless
                    // encoding of 8 bpc gray/raw images. See case 3 below for explanation.
                    if (strstr(codecString, "ffenc_huffyuv") || strstr(codecString, "ffenc_ljpeg")) sprintf(capsForCodecString, "ffmpegcolorspace ! capsfilter caps=\"video/x-raw-rgb, bpp=(int)32, depth=(int)32, endianess=(int)4321, red_mask=(int)65280, green_mask=(int)16711680, blue_mask=(int)-16777216\" ! ");
                    break;
                case 3:
                    // 8 bpc RGB8:
                    sprintf(capsString, "video/x-raw-rgb, bpp=(int)24, depth=(int)24, endianess=(int)4321, red_mask=(int)16711680, green_mask=(int)65280, blue_mask=(int)255");
                    
                    // If the ffenc_huffyuv or ffenc_ljpeg encoder is in use, we need some special caps after the colorspace converter. These make sure that
                    // the huffyuv encoder actually performs RGB8 24 bpp lossless encoding of the video, instead of YUV422p encoding with near-lossless
                    // luminance, but lossy chrominance encoding (spatial subsampling). This is derived from the code of libavcodec's huffyuv
                    // element aka ffenc_huffyuv and testing on GStreamer command line. We only care about lossless encoding if huffyuv or ffenc_ljpeg is in
                    // use, because we assume one would only use huffyuv if the intention would be to get lossless encoding:
                    if (strstr(codecString, "ffenc_huffyuv") || strstr(codecString, "ffenc_ljpeg")) sprintf(capsForCodecString, "ffmpegcolorspace ! capsfilter caps=\"video/x-raw-rgb, bpp=(int)32, depth=(int)32, endianess=(int)4321, red_mask=(int)65280, green_mask=(int)16711680, blue_mask=(int)-16777216\" ! ");
                    break;
                case 4:
                    // 8 bpc RGBA8:
                    sprintf(capsString, "video/x-raw-rgb, bpp=(int)32, depth=(int)32, endianess=(int)4321, alpha_mask=(int)-16777216, red_mask=(int)16711680, green_mask=(int)65280, blue_mask=(int)255");
                    // Note: At least if lossless encoding via ffenc_huffyuv is requested, this will actually cause a lossless encoding
                    // of the RGB8 color channels, but complete loss of the A8 alpha channel! Why? Because libavcodec's huffyuv encoder
                    // only supports RGB32 or RGB24, but GStreamer's ffenc_huffyuv plugin maps RGBA8 to RGBA, which is not supported by huffyuv,
                    // so fallback to RGB24 encoding results. Essentially case 4 reverts to case 3 above, but accepts RGBA8 input at least.
                    
                    // ffenc_ljpeg needs manual setup of component swizzling, so it actually encodes RGB8 lossless instead of using I420 encoding.
                    // Note: The alpha channel is accepted as input, but lost/thrown away during encoding, just as with huffyuv!
                    if (strstr(codecString, "ffenc_ljpeg")) sprintf(capsForCodecString, "ffmpegcolorspace ! capsfilter caps=\"video/x-raw-rgb, bpp=(int)32, depth=(int)32, endianess=(int)4321, red_mask=(int)65280, green_mask=(int)16711680, blue_mask=(int)-16777216\" ! ");
                    
                    break;
                default:
                    printf("PTB-ERROR: Unsupported number of color channels %i for video encoding!\n", numChannels);
                    goto bail;
            }
            
            // y4menc as codec/muxer? This is essentially YUV raw encoding. Assume user wants near-lossless operation. This only accepts YUV
            // input, so try to convert to Y42B format so encoding loses as few information as possible:
            if (strstr(codecString, "y4menc")) sprintf(capsForCodecString, "ffmpegcolorspace ! capsfilter caps=\"video/x-raw-yuv, format=(fourcc)Y42B\" ! ");
        }
        else {
            // 16 bpc format:
            
            // Use our own proprietary encoding for squeezing 16 bpc content into 8 bpc codecs?
            if (useOwn16bpc) {
                // We always accept pixeldata from our provider - and feed it via appsrc into GStreamer - as RGB 24 bpp, 8 bpc:
                // For 3 channel RGB this is natural, and the layout is:
                // [red1,green1,blue1][red2,green2,blue2] == [RH,RL, GH][GL,BH,BL] -- Abuse two neighbour pixels to encode one 16 bpc RGB pixel.
                // For 1 channel 16 bpp gray, the layout is:
                // [red1,green1,blue1][red2,green2,blue2] == [GH1, GL1, GH2][GL2, GH3, GL3] == Abuse two neighbour pixels to encode 3 grayscale pixels.
                sprintf(capsString, "video/x-raw-rgb, bpp=(int)24, depth=(int)24, endianess=(int)4321, red_mask=(int)16711680, green_mask=(int)65280, blue_mask=(int)255");
                
                // If the ffenc_huffyuv or ffenc_ljpeg encoder is in use, we need some special caps after the colorspace converter. These make sure that
                // the huffyuv encoder actually performs RGB8 24 bpp lossless encoding of the video, instead of YUV422p encoding with near-lossless
                // luminance, but lossy chrominance encoding (spatial subsampling). This is derived from the code of libavcodec's huffyuv
                // element aka ffenc_huffyuv and testing on GStreamer command line. We only care about lossless encoding if huffyuv or ffenc_ljpeg is in
                // use, because we assume one would only use huffyuv if the intention would be to get lossless encoding:
                if (strstr(codecString, "ffenc_huffyuv") || strstr(codecString, "ffenc_ljpeg")) sprintf(capsForCodecString, "ffmpegcolorspace ! capsfilter caps=\"video/x-raw-rgb, bpp=(int)32, depth=(int)32, endianess=(int)4321, red_mask=(int)65280, green_mask=(int)16711680, blue_mask=(int)-16777216\" ! ");

                switch (numChannels) {
                    case 1:
                        // 16 bpc gray or raw: 3 grayscale pixels in two rgb pixels. Only 2/3 of RGB8 pixels needed to
                        // encode these GRAY16 pixels. Image width of typical video encoding formats oesn't end up as
                        // integer when multiplied by 2/3, but height often does, e.g., for 640x480, 768x576, 1600x1200,
                        // so do this. As height is an integer, this will truncate to smaller integer if it doesn't fit,
                        // so worst case we cut off a few scanlines at the bottom or top of the image if usercode selects an
                        // unlucky resolution. This loop tries different height cutoffs until one satisfies our constraint:
                        for (height = pwriterRec->height; (height > pwriterRec->height - 3) && (height > 0); height--) {
                            if (height == (height * 2 / 3) * 3 / 2) break;
                        }
                        
                        // Managed to do without cutting information?
                        if ((height != pwriterRec->height) && (PsychPrefStateGet_Verbosity() > 1)) {
                            printf("PTB-WARNING: Can't store complete image in 1 channel 16 bpc 'UsePTB16BPC' proprietary encoding as height %i * 2/3 \n", pwriterRec->height);
                            printf("PTB-WARNING: doesn't end up as integral value! %i scanlines will be removed from top or bottom of image during movie\n", pwriterRec->height - height);
                            printf("PTB-WARNING: encoding for resulting final valid height of %i scanlines.\n", height);
                        }

                        // Define final height of input images:
                        pwriterRec->height = height;
                        
                        // Define height for encoding:
                        height = height * 2 / 3;
                        break;
                        
                    case 3:
                        // 16 bpc RGB16: 1 RGB16 pixel in two RGB8 pixels -> double-wide image:
                        width = width * 2;
                        if ((width > 4096) && (PsychPrefStateGet_Verbosity() > 1)) {
                            printf("PTB-WARNING: Video image for 16 bpc RGB encoding is wider than 2048 pixels. This may cause encoding failure with various codecs!\n");
                        }
                        break;

                    default:
                        printf("PTB-ERROR: Unsupported number of color channels %i for 16 bpc video encoding in Psychtoolbox proprietary format! Only 1 and 3 are supported.\n", numChannels);
                        goto bail;
                }
                
                // None of the known lossless codecs in use? May want to warn user about possible royal screwups:
                if (!strstr(codecString, "ffenc_huffyuv") && !strstr(codecString, "ffenc_ljpeg") && !strstr(codecString, "ffenc_sgi") &&
                    (PsychPrefStateGet_Verbosity() > 1)) {
                    printf("PTB-WARNING: You don't use one of the known good lossless video codecs huffyuv, ffenc_sgi or maybe ffenc_ljpeg? Note that\n");
                    printf("PTB-WARNING: use of even a slightly lossy codec for 16 bpc UsePTB16BPC format will royally screw up your movie!\n");
                }
                
                if (PsychPrefStateGet_Verbosity() > 2) {
                    printf("PTB-INFO: 16 bpc %i-channels Psychtoolbox proprietary video encoding via keyword 'UsePTB16BPC' selected.\n", numChannels);
                    printf("PTB-INFO: You can read such movie files later if you add a 'specialFlags1' setting of 512 in Screen('OpenMovie') and a\n");
                    printf("PTB-INFO: 'pixelFormat' of %i channels. Other 'pixelFormats' are not supported for this proprietary encoding.\n", numChannels);
                }
            }
            else {
                // Proper standardized 16 bpc encodings on GStreamer-0.10, which is severely limited in
                // what it can do:
                // We only handle Luminance16/Raw16, aka 16 bit grayscale. This is due to limitations of the
                // ffmpegcolorspace converter we use. In practice, as of end of 2013 and GStreamer-0.10 i don't
                // know of any feasible way to actually encode content with 16 bpc, so this is quite pointless...
                if (PsychPrefStateGet_Verbosity() > 1) {
                    printf("PTB-WARNING: As of GStreamer-0.10 the PTB developers do not know of any codec format which could actually store 16 bpc content!\n");
                    printf("PTB-WARNING: Most likely your content will be downgraded to 8 bpc with your current settings. To avoid this you can use a special\n");
                    printf("PTB-WARNING: Psychtoolbox proprietary encoding, which only Psychtoolbox can read. This encoding sqeezes 16 bpc content into 8 bpc\n");
                    printf("PTB-WARNING: encodings. Specify the keyword UsePTB16BPC in the encoding options / movieoptions when creating a movie file with\n");
                    printf("PTB-WARNING: Screen('CreateMovie') or opening a video capture device for recording with the libdc1394 pro-class capture engine.\n");
                    printf("PTB-WARNING: Also select the number of channels during encoding/video recording to be 1 or 3 for grayscale or RGB, other channels are\n");
                    printf("PTB-WARNING: not supported. You must provide a 'bitdepth' of 16 and you must choose a perfectly lossless codec for encoding.\n");
                    printf("PTB-WARNING: You can read such movie files if you add a 'specialFlags1' setting of 512 in Screen('OpenMovie') and a\n");
                    printf("PTB-WARNING: 'pixelFormat' of 1 for grayscale movies or of 3 for RGB color movies. Other 'pixelFormats' are not supported.\n\n");
                }
                
                if (numChannels == 1) {
                    // 16 bit gray encoding of luminance16 or raw16 sensor data:
                    sprintf(capsString, "video/x-raw-gray, bpp=(int)16, depth=(int)16, endianness=(int)4321");
                }
                else {
                    printf("PTB-ERROR: Unsupported number of color channels %i for standard compliant 16 bpc video encoding! Only 1-channel encoding of 16 bpc luminance or raw data is supported.\n", numChannels);
                    goto bail;
                }
            }
        }

        // Build final launch string:
        if (moviefile && (strlen(moviefile) > 0)) {
            // Actual recording into moviefile requested:
            sprintf(launchString, "appsrc name=ptbvideoappsrc do-timestamp=0 stream-type=0 max-bytes=0 block=1 is-live=0 emit-signals=0 ! capsfilter caps=\"%s, width=(int)%i, height=(int)%i, framerate=%i/10000 \" ! %s%s%s%s ! filesink name=ptbfilesink async=0 location=%s ", capsString, width, height, ((int) (framerate * 10000 + 0.5)), (feedbackString) ? feedbackString : "", videorateString, capsForCodecString, codecString, moviefile);
        }
        else {
            // No writing of moviefile, just (ab)use GStreamer "recording" pipeline for image processing on behalf
            // of, e.g., the libdc1394 video capture engine:
            sprintf(launchString, "appsrc name=ptbvideoappsrc do-timestamp=0 stream-type=0 max-bytes=0 block=1 is-live=0 emit-signals=0 ! capsfilter caps=\"%s, width=(int)%i, height=(int)%i, framerate=%i/10000 \" ! %s ", capsString, width, height, ((int) (framerate * 10000 + 0.5)), (feedbackString) ? feedbackString : "");
        }
    }

    // Create a movie file for the destination movie:
    if (PsychPrefStateGet_Verbosity() > 3) {
        printf("PTB-INFO: Movie writing / GStreamer processing pipeline gst-launch line (without the -e option required on the command line!) is:\n");
        printf("gst-launch %s\n", launchString);
    }

    // Build pipeline from launch string:
    pwriterRec->Movie = gst_parse_launch((const gchar*) launchString, &myErr);
    if ((NULL == pwriterRec->Movie) || myErr) {
        if (PsychPrefStateGet_Verbosity() > 0) {
            if (strlen(moviefile) > 0) {
                printf("PTB-ERROR: In CreateMovie: Creating movie file with handle %i [%s] failed: Could not build pipeline.\n", moviehandle, moviefile);
            }
            else {
                printf("PTB-ERROR: In CreateMovie: Creating GStreamer processing pipeline with handle %i failed: Could not build pipeline.\n", moviehandle);
            }
            printf("PTB-ERROR: Parameters were: %s\n", movieoptions);
            printf("PTB-ERROR: Launch string was: %s\n", launchString);
            printf("PTB-ERROR: GStreamer error message was: %s\n", (char*) myErr->message);
        }

        goto bail;
    }

    // Get handle to ptbvideoappsrc:
    pwriterRec->ptbvideoappsrc = gst_bin_get_by_name(GST_BIN(pwriterRec->Movie), (const gchar *) "ptbvideoappsrc");
    if (NULL == pwriterRec->ptbvideoappsrc) {
        if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: In CreateMovie: Creating movie file with handle %i [%s] failed: Could not find ptbvideoappsrc pipeline element.\n", moviehandle, moviefile);
        goto bail;
    }

    // Get handle to ptbaudioappsrc:
    pwriterRec->ptbaudioappsrc = gst_bin_get_by_name(GST_BIN(pwriterRec->Movie), (const gchar *) "ptbaudioappsrc");
    if (doAudio && (NULL == pwriterRec->ptbaudioappsrc)) {
        if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: In CreateMovie: Creating movie file with handle %i [%s] failed: Could not find ptbaudioappsrc pipeline element.\n", moviehandle, moviefile);
        goto bail;
    }

    // feedbackString provided? If so, then get handle to its appsink:
    if (feedbackString) {
        pwriterRec->ptbvideoappsink = gst_bin_get_by_name(GST_BIN(pwriterRec->Movie), (const gchar *) "ptbvideoappsink");
        if (NULL == pwriterRec->ptbvideoappsink) {
            if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: In CreateMovie: Creating movie file with handle %i [%s] failed: Could not find ptbvideoappsink pipeline element.\n", moviehandle, moviefile);
            goto bail;
        }
    }

    if (!useNewBusCheck) {
        pwriterRec->Context = g_main_loop_new (NULL, FALSE);
        pwriterRec->bus = gst_pipeline_get_bus (GST_PIPELINE(pwriterRec->Movie));
        gst_bus_add_watch(pwriterRec->bus, (GstBusFunc) PsychMovieBusCallback, pwriterRec);
        gst_object_unref(pwriterRec->bus);
    }

    // Start the pipeline:
    if (!PsychMoviePipelineSetState(pwriterRec->Movie, GST_STATE_PLAYING, 10)) {
        if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: In CreateMovie: Creating movie file with handle %i [%s] failed:  Failed to start movie encoding pipeline!\n", moviehandle, moviefile);
        goto bail;
    }

    PsychGSProcessMovieContext(pwriterRec, FALSE);

    // Increment count of open movie writers:
    moviewritercount++;

    if ((strlen(moviefile) > 0) && (PsychPrefStateGet_Verbosity() > 3)) printf("PTB-INFO: Moviehandle %i successfully opened for movie writing into file '%s'.\n", moviehandle, moviefile);
    if ((strlen(moviefile) == 0) && (PsychPrefStateGet_Verbosity() > 3)) printf("PTB-INFO: Handle %i successfully opened for GStreamer video processing.\n", moviehandle);
    
    // Should we dump the whole encoding pipeline graph to a file for visualization
    // with GraphViz? This can be controlled via PsychTweak('GStreamerDumpFilterGraph' dirname);
    if (getenv("GST_DEBUG_DUMP_DOT_DIR")) {
        // Dump complete encoding filter graph to a .dot file for later visualization with GraphViz:
        printf("PTB-DEBUG: Dumping movie encoder graph pre-negotiation for movie %s to directory %s.\n", moviefile, getenv("GST_DEBUG_DUMP_DOT_DIR"));
        GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(pwriterRec->Movie), GST_DEBUG_GRAPH_SHOW_ALL, "PsychMovieWritingGraph-PreNeg");
    }

    // Return new handle:
    return(moviehandle);

bail:
    if (pwriterRec->ptbvideoappsrc) gst_object_unref(GST_OBJECT(pwriterRec->ptbvideoappsrc));
    pwriterRec->ptbvideoappsrc = NULL;

    if (pwriterRec->ptbaudioappsrc) gst_object_unref(GST_OBJECT(pwriterRec->ptbaudioappsrc));
    pwriterRec->ptbaudioappsrc = NULL;

    if (pwriterRec->ptbvideoappsink) gst_object_unref(GST_OBJECT(pwriterRec->ptbvideoappsink));
    pwriterRec->ptbvideoappsink = NULL;
    
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

    if (NULL == pwriterRec->ptbvideoappsrc) return(0);

    // Release any pending buffers:
    if (pwriterRec->PixMap) gst_buffer_unref(pwriterRec->PixMap);
    pwriterRec->PixMap = NULL;

    PsychGSProcessMovieContext(pwriterRec, FALSE);

    // Send EOS signal downstream:
    g_signal_emit_by_name(pwriterRec->ptbvideoappsrc, "end-of-stream", &ret);
    if (pwriterRec->ptbaudioappsrc) g_signal_emit_by_name(pwriterRec->ptbaudioappsrc, "end-of-stream", &ret);

    // Wait for eos flag to turn TRUE due to bus callback receiving the
    // downstream EOS event that we just sent out:
    while (!pwriterRec->eos) {
        PsychGSProcessMovieContext(pwriterRec, FALSE);
        PsychYieldIntervalSeconds(0.010);
    }

    // Yield another 10 msecs after EOS signalled, just to be safe:
    PsychYieldIntervalSeconds(0.010);
    PsychGSProcessMovieContext(pwriterRec, FALSE);

    // Pause the encoding pipeline:
    if (!PsychMoviePipelineSetState(pwriterRec->Movie, GST_STATE_PAUSED, 10)) {
        if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed to pause movie encoding pipeline at close time!!\n");
    }

    PsychGSProcessMovieContext(pwriterRec, FALSE);

    // Should we dump the whole encoding pipeline graph to a file for visualization
    // with GraphViz? This can be controlled via PsychTweak('GStreamerDumpFilterGraph' dirname);
    if (getenv("GST_DEBUG_DUMP_DOT_DIR")) {
        // Dump complete encoding filter graph to a .dot file for later visualization with GraphViz:
        printf("PTB-DEBUG: Dumping movie encoder graph post-encoding for moviehandle %i to directory %s.\n", movieHandle, getenv("GST_DEBUG_DUMP_DOT_DIR"));
        GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS(GST_BIN(pwriterRec->Movie), GST_DEBUG_GRAPH_SHOW_ALL, "PsychMovieWritingGraph-Actual");
    }

    // Stop the encoding pipeline:
    if (!PsychMoviePipelineSetState(pwriterRec->Movie, GST_STATE_READY, 10)) {
        if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed to stop movie encoding pipeline at close time!!\n");
    }

    PsychGSProcessMovieContext(pwriterRec, FALSE);

    // Shutdown and release encoding pipeline:
    if (!PsychMoviePipelineSetState(pwriterRec->Movie, GST_STATE_NULL, 10)) {
        if (PsychPrefStateGet_Verbosity() > 0) printf("PTB-ERROR: Failed to shutdown movie encoding pipeline at close time!!\n");
    }

    PsychGSProcessMovieContext(pwriterRec, FALSE);

    gst_object_unref(GST_OBJECT(pwriterRec->Movie));
    pwriterRec->Movie = NULL;

    if (pwriterRec->ptbvideoappsrc) gst_object_unref(GST_OBJECT(pwriterRec->ptbvideoappsrc));
    pwriterRec->ptbvideoappsrc = NULL;

    if (pwriterRec->ptbaudioappsrc) gst_object_unref(GST_OBJECT(pwriterRec->ptbaudioappsrc));
    pwriterRec->ptbaudioappsrc = NULL;

    if (pwriterRec->ptbvideoappsink) gst_object_unref(GST_OBJECT(pwriterRec->ptbvideoappsink));
    pwriterRec->ptbvideoappsink = NULL;

    // Delete video context:
    if (pwriterRec->Context) g_main_loop_unref(pwriterRec->Context);
    pwriterRec->Context = NULL;

    // Decrement count of active writers:
    moviewritercount--;

    // Return success/fail status:
    if ((myErr == 0) && (PsychPrefStateGet_Verbosity() > 3)) printf("PTB-INFO: Moviehandle %i successfully closed and movie written to filesystem.\n", movieHandle);

    return(myErr == 0);
}

#endif // #if !GST_CHECK_VERSION(1,0,0)

// End of GStreamer routines.
#endif
