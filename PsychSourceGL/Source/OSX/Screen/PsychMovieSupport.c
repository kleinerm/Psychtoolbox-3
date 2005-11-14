/*
	PsychMovieSupport.c
	
	PLATFORMS:	
	
		This is the OS X Core Graphics version.  
				
	AUTHORS:
	
		Mario Kleiner           mk              mario.kleiner@tuebingen.mpg.de

	HISTORY:
	
        DESCRIPTION:
	
		Psychtoolbox functions for dealing with movies.
	
	NOTES:

*/

#include "Screen.h"

#define PSYCH_MAX_MOVIES 100

typedef struct {
    Movie   theMovie;
    QTVisualContextRef  QTMovieContext;
    QTAudioContextRef   QTAudioContext;
    int loopflag;
    double movieduration;
    int nrframes;
    double fps;
    int width;
    int height;
    double last_pts;
    int nr_droppedframes;
} PsychMovieRecordType;

static PsychMovieRecordType movieRecordBANK[PSYCH_MAX_MOVIES];
static int numMovieRecords = 0;
static Boolean firsttime = TRUE;

/*
 *      PsychCreateMovie() -- Create a movie object.
 *
 *      This function tries to open a Quicktime-Moviefile and create an
 *      associated movie object for it.
 *
 *      win = Pointer to window record of associated onscreen window.
 *      moviename = char* with the name of the moviefile.
 *      moviehandle = handle to the new movie.
 */
void PsychCreateMovie(PsychWindowRecordType *win, const char* moviename, int* moviehandle)
{
    Movie theMovie = NULL;
    QTVisualContextRef QTMovieContext = NULL;
    QTAudioContextRef  QTAudioContext = NULL;
    *moviehandle = -1;
    
    if (firsttime) {
        // First time since startup: Initialize movieRecordBANK:
        int i;
        for (i=0; i < PSYCH_MAX_MOVIES; i++) {
            movieRecordBANK[i].theMovie = NULL;
            movieRecordBANK[i].QTMovieContext = NULL;
            movieRecordBANK[i].QTAudioContext = NULL;
            movieRecordBANK[i].loopflag = 0;
        }
        
        firsttime = FALSE;
    }
    
    if (!PsychIsOnscreenWindow(win)) {
        PsychErrorExitMsg(PsychError_user, "Provided windowPtr is not an onscreen window.");
    }

    if (NULL==moviename) {
        PsychErrorExitMsg(PsychError_internal, "NULL-Ptr instead of moviename passed!");
    }

    if (numMovieRecords >= PSYCH_MAX_MOVIES) {
        PsychErrorExitMsg(PsychError_user, "Allowed maximum number of simultaneously open movies exceeded!");
    }

    // Search first free slot in movieRecordBANK:
    int i;
    for (i=0; (i < PSYCH_MAX_MOVIES) && (movieRecordBANK[i].theMovie); i++);
    if (i>=PSYCH_MAX_MOVIES) {
        PsychErrorExitMsg(PsychError_user, "Allowed maximum number of simultaneously open movies exceeded!");
    }

    // Slot slotid will contain the movie record for our new movie object:
    int slotid=i;
    
    // Create name-string for moviename:
    CFStringRef movieLocation = CFStringCreateWithCString (kCFAllocatorDefault, moviename, kCFStringEncodingASCII);
    
    // Zero-out new record in moviebank:
    movieRecordBANK[slotid].theMovie=NULL;    
    movieRecordBANK[slotid].QTMovieContext=NULL;    
    movieRecordBANK[slotid].QTAudioContext=NULL;
    
    // Initialize Quicktime-Subsystem:
    OSStatus error = EnterMovies();
    if (error!=noErr) {
        PsychErrorExitMsg(PsychError_internal, "Quicktime EnterMovies() failed!!!");
    }
    
    // Create QTGLTextureContext:
    error = QTOpenGLTextureContextCreate (kCFAllocatorDefault,
                                           win->targetSpecific.contextObject,
                                           win->targetSpecific.pixelFormatObject,
                                           NULL,
                                           &QTMovieContext);
    if (error!=noErr) {
        PsychErrorExitMsg(PsychError_internal, "OpenGL Quicktime visual context creation failed!!!");
    }
        
    // Create QTAudioContext for default CoreAudio device:
    CFStringRef coreAudioDeviceUID = NULL; // Use default audio-output device.
    error =QTAudioContextCreateForAudioDevice (kCFAllocatorDefault,
                                                        coreAudioDeviceUID,
                                                        NULL,
                                                        &QTAudioContext);
    if (error!=noErr) {
        PsychErrorExitMsg(PsychError_internal, "Quicktime audio context creation failed!!!");
    }
    
    boolean trueValue = TRUE;
    QTNewMoviePropertyElement newMovieProperties[4] = {0};

    // The Movie location 
    newMovieProperties[0].propClass = kQTPropertyClass_DataLocation;
    newMovieProperties[0].propID = kQTDataLocationPropertyID_CFStringPosixPath;
    newMovieProperties[0].propValueSize = sizeof(CFStringRef);
    newMovieProperties[0].propValueAddress = &movieLocation;
    
    // The Movie visual context
    newMovieProperties[1].propClass = kQTPropertyClass_Context;
    newMovieProperties[1].propID = kQTContextPropertyID_VisualContext;
    newMovieProperties[1].propValueSize = sizeof(QTVisualContextRef);
    newMovieProperties[1].propValueAddress = &QTMovieContext;
    
    // The Movie audio context
    newMovieProperties[2].propClass = kQTPropertyClass_Context;
    newMovieProperties[2].propID = kQTContextPropertyID_AudioContext;
    newMovieProperties[2].propValueSize = sizeof(QTAudioContextRef);
    newMovieProperties[2].propValueAddress = &QTAudioContext;

    // The Movie active
    newMovieProperties[3].propClass = kQTPropertyClass_NewMovieProperty;
    newMovieProperties[3].propID = kQTNewMoviePropertyID_Active;
    newMovieProperties[3].propValueSize = sizeof(trueValue);
    newMovieProperties[3].propValueAddress = &trueValue;
    
    // Instantiate the Movie
    error = NewMovieFromProperties(4, newMovieProperties, 0, NULL, &theMovie);
    if (error!=noErr) {
        QTVisualContextRelease(QTMovieContext);
        QTAudioContextRelease(QTAudioContext);
        char msgerr[10000];
        char errdesc[1000];
        switch(error) {
            case -2000:
                sprintf(errdesc, "File not found.");
            break;
            
            case -2048:
                sprintf(errdesc, "This is not a file that Quicktime understands.");
            break;
            
            case -2003:
                sprintf(errdesc, "Can't find media handler (codec) for this movie.");
            break;
            
            default:
                sprintf(errdesc, "Unknown: Check http://developer.apple.com/documentation/QuickTime/APIREF/ErrorCodes.htm#//apple_ref/doc/constant_group/Error_Codes");
        }
        
        sprintf(msgerr, "Couldn't load movie %s! Quicktime error code %i [%s]", moviename, (int) error, errdesc);
        PsychErrorExitMsg(PsychError_user, msgerr);
    }
    
    SetMovieAudioContext(theMovie, QTAudioContext);
    SetMovieAudioMute(theMovie, FALSE, 0);

    // Assign new record in moviebank:
    movieRecordBANK[slotid].theMovie=theMovie;    
    movieRecordBANK[slotid].QTMovieContext=QTMovieContext;    
    movieRecordBANK[slotid].QTAudioContext=QTAudioContext;
    movieRecordBANK[slotid].loopflag = 0;
    *moviehandle = slotid;

    // Increase counter:
    numMovieRecords++;

    // Compute basic movie properties - total framecount, duration and fps:
    
    // Compute duration in seconds:
    movieRecordBANK[slotid].movieduration = (double) GetMovieDuration(theMovie) / (double) GetMovieTimeScale(theMovie);
    
    // Count total number of videoframes:
    long		myCount = -1;
    short		myFlags;
    TimeValue           myTime = 0;
    OSType		myTypes[1];
    myTypes[0] = VisualMediaCharacteristic;		// We want video samples.
    // We want to begin with the first frame in the movie:
    myFlags = nextTimeStep + nextTimeEdgeOK;
    
    while (myTime >= 0) {
        myCount++;        
        // look for the next frame in the track; when there are no more frames,
        // myTime is set to -1, so we'll exit the while loop
        GetMovieNextInterestingTime(theMovie, myFlags, 1, myTypes, myTime, FloatToFixed(1), &myTime, NULL);        
        // after the first interesting time, don't include the time we're currently at
        myFlags = nextTimeStep;
    }    
    movieRecordBANK[slotid].nrframes = (int) myCount;

    // Compute expected framerate, assuming a linear spacing between frames:
    movieRecordBANK[slotid].fps = (double) myCount / movieRecordBANK[slotid].movieduration;
    
    // Determine size of images in movie:
    Rect movierect;
    GetMovieBox(theMovie, &movierect);
    movieRecordBANK[slotid].width = movierect.right - movierect.left;
    movieRecordBANK[slotid].height = movierect.bottom - movierect.top;
    
    return;
}

/*
 *  PsychGetMovieInfo() - Return basic information about a movie.
 *
 *  framecount = Total number of video frames in the movie, determined by counting.
 *  durationsecs = Total playback duration of the movie, in seconds.
 *  framerate = Estimated video playback framerate in frames per second (fps).
 *  width = Width of movie images in pixels.
 *  height = Height of movie images in pixels.
 *  nrdroppedframes = Total count of videoframes that had to be dropped during last movie playback,
 *                    in order to keep the movie synced with the realtime clock.
 */
void PsychGetMovieInfos(int moviehandle, int* width, int* height, int* framecount, double* durationsecs, double* framerate, int* nrdroppedframes)
{
    if (moviehandle < 0 || moviehandle >= PSYCH_MAX_MOVIES) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. Valid handles are between 0 and !!!");
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
 *  PsychDeleteMovie() -- Delete a movie object and release all associated ressources.
 */
void PsychDeleteMovie(int moviehandle)
{
    if (moviehandle < 0 || moviehandle >= PSYCH_MAX_MOVIES) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. Valid handles are between 0 and !!!");
    }
    
    if (movieRecordBANK[moviehandle].theMovie == NULL) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. No movie associated with this handle !!!");
    }
    
    MoviesTask(NULL, 0);
    
    // Stop movie playback immediately:
    StopMovie(movieRecordBANK[moviehandle].theMovie);
    
    MoviesTask(NULL, 0);

    SetMovieVisualContext(movieRecordBANK[moviehandle].theMovie, NULL);
    SetMovieAudioContext(movieRecordBANK[moviehandle].theMovie, NULL);
    
    // Delete visual context for this movie:
    QTVisualContextRelease(movieRecordBANK[moviehandle].QTMovieContext);
    movieRecordBANK[moviehandle].QTMovieContext = NULL;

    // Delete audio context for this movie:
    QTAudioContextRelease(movieRecordBANK[moviehandle].QTAudioContext);
    movieRecordBANK[moviehandle].QTAudioContext = NULL;

    // Delete movieobject for this handle:
    DisposeMovie(movieRecordBANK[moviehandle].theMovie);
    movieRecordBANK[moviehandle].theMovie=NULL;    
    
    // Decrease counter:
    if (numMovieRecords>0) numMovieRecords--;
        
    return;
}

/*
 *  PsychDeleteAllMovies() -- Delete all movie objects and release all associated ressources.
 */
void PsychDeleteAllMovies(void)
{
    int i;
    for (i=0; i<PSYCH_MAX_MOVIES; i++) {
        if (movieRecordBANK[i].theMovie) PsychDeleteMovie(i);
    }
    return;
}


/*
 *  PsychGetTextureFromMovie() -- Create an OpenGL texture map from a specific videoframe from given movie object.
 *
 *  win = Window pointer of onscreen window for which a OpenGL texture should be created.
 *  moviehandle = Handle to the movie object.
 *  checkForImage = true == Just check if new image available, false == really retrieve the image, blocking if necessary.
 *  timeindex = When not in playback mode, this allows specification of a requested frame by presentation time.
 *              If set to -1, or if in realtime playback mode, this parameter is ignored and the next video frame is returned.
 *  out_texture = Pointer to the Psychtoolbox texture-record where the new texture should be stored.
 *  presentation_timestamp = A ptr to a double variable, where the presentation timestamp of the returned frame should be stored.
 */
bool PsychGetTextureFromMovie(PsychWindowRecordType *win, int moviehandle, int checkForImage, double timeindex, PsychWindowRecordType *out_texture, double *presentation_timestamp)
{
    TimeValue		myCurrTime;
    TimeValue		myNextTime;
    short		myFlags;
    OSType		myTypes[1];
    OSErr		myErr = noErr;
    
    CVOpenGLTextureRef newImage = NULL;

    if (!PsychIsOnscreenWindow(win)) {
        PsychErrorExitMsg(PsychError_user, "Need onscreen window ptr!!!");
    }
    
    // Activate OpenGL context of target window:
    PsychSetGLContext(win);

    // Explicitely disable Apple's Client storage extensions. For now they are not really useful to us.
    glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_FALSE);
    
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
    Movie   theMovie = movieRecordBANK[moviehandle].theMovie;
    QTVisualContextRef  theMoviecontext = movieRecordBANK[moviehandle].QTMovieContext;

    if (theMovie == NULL) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. No movie associated with this handle.");
    }

    // Check if end of movie is reached. Rewind, if so...
    if (IsMovieDone(theMovie) && movieRecordBANK[moviehandle].loopflag > 0) {
        if (GetMovieRate(theMovie)>0) {
            GoToBeginningOfMovie(theMovie);
        } else {
            GoToEndOfMovie(theMovie);
        }
    }
    
    // Is movie actively playing (automatic async playback, possibly with synced sound)?
    // If so, then we ignore the 'timeindex' parameter, because the automatic playback
    // process determines which frames should be delivered to PTB when. This function will
    // simply wait or poll for arrival/presence of a new frame that hasn't been fetched
    // in previous calls.
    if (0 == GetMovieRate(theMovie)) {
        // Movie playback inactive. We are in "manual" mode: No automatic async playback,
        // no synced audio output. The user just wants to manually fetch movie frames into
        // textures for manual playback in a standard Matlab-loop.

        // Image for specific point in time requested?
        if (timeindex >= 0) {
            // Yes. We try to retrieve the next possible image for requested timeindex.
            myCurrTime = (TimeValue) ((timeindex * (double) GetMovieTimeScale(theMovie)) + 0.5f);
        }
        else {
            // No. We just retrieve the next frame, given the current movie time.
            myCurrTime = GetMovieTime(theMovie, NULL);
        }
            
        // Retrieve timeindex of the closest image sample after myCurrTime:
        myFlags = nextTimeStep + nextTimeEdgeOK;	// We want the next frame in the movie's media.
        myTypes[0] = VisualMediaCharacteristic;		// We want video samples.
        GetMovieNextInterestingTime(theMovie, myFlags, 1, myTypes, myCurrTime, FloatToFixed(1), &myNextTime, NULL);
        myErr = GetMoviesError();
        if (myErr != noErr) {
            PsychErrorExitMsg(PsychError_internal, "Failed to fetch texture from movie for given timeindex!");
        }
        
        // Set movies current time to myNextTime, so the next frame will be fetched from there:
        SetMovieTimeValue(theMovie, myNextTime);
    }
    
    if (presentation_timestamp) {
        // Retrieve the exact presentation timestamp of the retrieved frame (in movietime):
        myFlags = nextTimeStep + nextTimeEdgeOK;            // We want the next frame in the movie's media.
        myTypes[0] = VisualMediaCharacteristic;		// We want video samples.
        // We search backward for the closest available image for the current time. Either we get the current time
        // if we happen to fetch a frame exactly when it becomes ready, or we get a bit earlier timestamp, which is
        // the optimal presentation timestamp for this frame:
        GetMovieNextInterestingTime(theMovie, myFlags, 1, myTypes, GetMovieTime(theMovie, NULL), FloatToFixed(-1), &myNextTime, NULL);
        *presentation_timestamp = (double) myNextTime / (double) GetMovieTimeScale(theMovie);
    }

    // Allow quicktime visual context task to do its internal bookkeeping and cleanup work:
    QTVisualContextTask(theMoviecontext);

    // Perform decompress-operation:
    if (checkForImage) MoviesTask(theMovie, 0);
    
    // Should we just check for new image? If so, just return availability status:
    if (checkForImage) return(QTVisualContextIsNewImageAvailable(theMoviecontext, NULL));
    
    // Try up to 1000 iterations for arrival of requested image data in wait-mode:
    unsigned int failcount=0;
    while ((failcount < 1000) && !QTVisualContextIsNewImageAvailable(theMoviecontext, NULL)) {
        MoviesTask(theMovie, 0);
        PsychWaitIntervalSeconds(0.001);
        failcount++;
    }
        
    // Fetch new OpenGL texture with the new movie image frame:
    OSErr error = QTVisualContextCopyImageForTime(theMoviecontext, kCFAllocatorDefault, NULL, &newImage);
    if ((error!=noErr) || newImage == NULL) {
        PsychErrorExitMsg(PsychError_internal, "OpenGL<->Quicktime texture fetch failed!!!");
    }
    
    // Build a standard PTB texture record:    
    float lowerLeft[2];
    float lowerRight[2];    
    float upperRight[2];    
    float upperLeft[2];
    CVOpenGLTextureGetCleanTexCoords (newImage, lowerLeft, lowerRight, upperRight, upperLeft);
    int texid = CVOpenGLTextureGetName(newImage);
    
    // Assign texture rectangle:
    PsychMakeRect(out_texture->rect, upperLeft[0], upperLeft[1], lowerRight[0], lowerRight[1]);    
    
    // Assign OpenGL texture id:
    out_texture->textureNumber = texid;

    // Store special texture object as part of the PTB texture record:
    out_texture->targetSpecific.QuickTimeGLTexture = newImage;
    
    float rate = FixedToFloat(GetMovieRate(theMovie));
    
    // Detection of dropped frames: This is a heuristic. We'll see how well it works out...
    if (rate) {
        // Try to check for dropped frames in playback mode:

        // Expected delta between successive presentation timestamps:
        double targetdelta = 1.0f / (movieRecordBANK[moviehandle].fps * rate);

        // Compute real delta, given rate and playback direction:
        double realdelta;
        if (rate>0) {
            realdelta = *presentation_timestamp - movieRecordBANK[moviehandle].last_pts;
            if (realdelta<0) realdelta = 0;
        }
        else {
            realdelta = -1.0 * (*presentation_timestamp - movieRecordBANK[moviehandle].last_pts);
            if (realdelta<0) realdelta = 0;
        }
        
        double frames = realdelta / targetdelta;
        // Dropped frames?
        if (frames > 1 && movieRecordBANK[moviehandle].last_pts>=0) {
            movieRecordBANK[moviehandle].nr_droppedframes += (int) (frames - 1 + 0.5);
        }

        movieRecordBANK[moviehandle].last_pts = *presentation_timestamp;
    }
    
    // Manually advance movie time, if in fetch mode:
    if (0 == GetMovieRate(theMovie)) {
        // We are in manual fetch mode: Need to manually advance movie time to next
        // media sample:
        myFlags = nextTimeStep;                         // We want the next frame in the movie's media.
        myTypes[0] = VisualMediaCharacteristic;		// We want video samples.
        GetMovieNextInterestingTime(theMovie, myFlags, 1, myTypes, myCurrTime, FloatToFixed(1), &myNextTime, NULL);
        myErr = GetMoviesError();
        if (myErr != noErr) {
            PsychErrorExitMsg(PsychError_internal, "Failed to advance movie timeindex!");
        }
        
        // Set movies current time to myNextTime, so the next frame will be fetched from there:
        SetMovieTimeValue(theMovie, myNextTime);        
    }

    // Check if end of movie is reached. Rewind, if so...
    if (IsMovieDone(theMovie) && movieRecordBANK[moviehandle].loopflag > 0) {
        if (GetMovieRate(theMovie)>0) {
            GoToBeginningOfMovie(theMovie);
        } else {
            GoToEndOfMovie(theMovie);
        }
    }

    return(TRUE);
}

/*
 *  PsychFreeMovieTexture() - Release texture memory for a Quicktime texture.
 *
 *  This routine is called by PsychDeleteTexture() in PsychTextureSupport.c
 *  It performs the special cleanup necessary for Quicktime created textures.
 *
 */
void PsychFreeMovieTexture(PsychWindowRecordType *win)
{
    // Fetch special Quicktime texture handle...
    CVOpenGLTextureRef theTexture = win->targetSpecific.QuickTimeGLTexture;

    // ...is this a quicktime movietexture? If not, just skip this routine.
    if (win->windowType!=kPsychTexture || NULL == theTexture) return;
    
    // Quicktime movie texture: Release it via special CoreVideo release function:
    CVOpenGLTextureRelease(theTexture);
    win->targetSpecific.QuickTimeGLTexture = NULL;
    
    // 0-out the textureNumber so our standard cleanup routine (glDeleteTextures) gets
    // skipped - if we don't do this, Quicktime & CoreVideo will leak memory like hell!!!
    win->textureNumber = 0;

    return;
}

/*
 *  PsychPlaybackRate() - Start- and stop movieplayback, set playback parameters.
 *
 *  moviehandle = Movie to start-/stop.
 *  playbackrate = zero == Stop playback, non-zero == Play movie with spec. rate,
 *                 e.g., 1 = forward, 2 = double speed forward, -1 = backward, ...
 *  loop = 0 = Play once. 1 = Loop, aka rewind at end of movie and restart.
 *  soundvolume = 0 == Mute sound playback, between 0.0 and 1.0 == Set volume to 0 - 100 %.
 *
 */
void PsychPlaybackRate(int moviehandle, double playbackrate, int loop, double soundvolume)
{
    if (moviehandle < 0 || moviehandle >= PSYCH_MAX_MOVIES) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. Valid handles are between 0 and !!!");
    }
        
    // Fetch references to objects we need:
    Movie   theMovie = movieRecordBANK[moviehandle].theMovie;    
    if (theMovie == NULL) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. No movie associated with this handle !!!");
    }
    
    if (playbackrate != 0) {
        // Start playback of movie:
        SetMovieAudioMute(theMovie, (soundvolume==0) ? TRUE : FALSE, 0);
        SetMovieVolume(theMovie, (short) (soundvolume * 255.0));
        movieRecordBANK[moviehandle].loopflag = loop;
        movieRecordBANK[moviehandle].last_pts = -1.0;
        movieRecordBANK[moviehandle].nr_droppedframes = 0;
        SetMoviePreferredRate(theMovie, FloatToFixed(playbackrate));
        StartMovie(theMovie);
        
    }
    else {
        // Stop playback of movie:
        StopMovie(theMovie);
        
        // Output count of dropped frames:
        if (movieRecordBANK[moviehandle].nr_droppedframes > 0) {
            printf("PTB-INFO: Movie playback had to drop %i frames of movie %i to keep playback in sync.\n", movieRecordBANK[moviehandle].nr_droppedframes, moviehandle); 
        }
    }
    
    return;
}

/*
 *  void PsychExitMovies() - Shutdown handler.
 *
 *  This routine is called by Screen('CloseAll') and on clear Screen time to
 *  do final cleanup. It deletes all Quicktime textures and releases all Quicktime
 *  movie objects.
 *
 */
void PsychExitMovies(void)
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
    PsychDeleteAllMovies();
    
    // Shutdown Quicktime toolbox: We skip this, because according to Apple its not necessary,
    // and for some reason it reliably hangs Matlab, so one has to force-quit it :-(
    // Don't do this: ExitMovies();

    return;
}

/*
 *  PsychGetMovieTimeIndex()  -- Return current playback time of movie.
 */
double PsychGetMovieTimeIndex(int moviehandle)
{
    if (moviehandle < 0 || moviehandle >= PSYCH_MAX_MOVIES) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. Valid handles are between 0 and !!!");
    }
    
    // Fetch references to objects we need:
    Movie   theMovie = movieRecordBANK[moviehandle].theMovie;    
    if (theMovie == NULL) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. No movie associated with this handle !!!");
    }

    // Retrieve timeindex:
    return((double) GetMovieTime(theMovie, NULL) / (double) GetMovieTimeScale(theMovie));
}

/*
 *  PsychSetMovieTimeIndex()  -- Set current playback time of movie.
 */
double PsychSetMovieTimeIndex(int moviehandle, double timeindex)
{
    if (moviehandle < 0 || moviehandle >= PSYCH_MAX_MOVIES) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. Valid handles are between 0 and !!!");
    }
    
    // Fetch references to objects we need:
    Movie   theMovie = movieRecordBANK[moviehandle].theMovie;    
    if (theMovie == NULL) {
        PsychErrorExitMsg(PsychError_user, "Invalid moviehandle provided. No movie associated with this handle !!!");
    }
    
    // Retrieve current timeindex:
    double oldtime = (double) GetMovieTime(theMovie, NULL) / (double) GetMovieTimeScale(theMovie);
    
    // Set new timeindex:
    SetMovieTimeValue(theMovie, (TimeValue) (((timeindex * (double) GetMovieTimeScale(theMovie))) + 0.5f));

    // Check if end of movie is reached. Rewind, if so...
    if (IsMovieDone(theMovie) && movieRecordBANK[moviehandle].loopflag > 0) {
        if (GetMovieRate(theMovie)>0) {
            GoToBeginningOfMovie(theMovie);
        } else {
            GoToEndOfMovie(theMovie);
        }
    }
        
    // Return old value:
    return(oldtime);
}
