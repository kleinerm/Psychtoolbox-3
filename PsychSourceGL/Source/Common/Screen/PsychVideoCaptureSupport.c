/*
	Common/Screen/PsychVideoCaptureSupport.c
	
	PLATFORMS:	
	
		This is the OS independent version (for now: Should work on OS-X and Windows)  
				
	AUTHORS:
	
		Mario Kleiner           mk              mario.kleiner@tuebingen.mpg.de

	HISTORY:
	
        DESCRIPTION:
	
		Psychtoolbox functions for dealing with video capture devices.
	
	NOTES:

*/

#include "Screen.h"

#include <Quicktime/QuickTimeComponents.h>

//#if PSYCH_SYSTEM == PSYCH_WINDOWS
// We typedef all missing functions on Windows away...
//typedef void* QTVisualContextRef;
//Boolean QTVisualContextIsNewImageAvailable(void* a, void* b) { return(false); }
//OSErr QTVisualContextCopyImageForTime(void* a, void* b, void* c) { return(noErr) };
//void QTVisualContextRelease(void* a) { return; }
//void QTVisualContextTask(void *a) { return; }
//void CVOpenGLTextureRelease(void* a) { return; }
//GLuint CVOpenGLTextureGetName(void* a) { return(0); }
//void CVOpenGLTextureGetCleanTexCoords(void* a, float* b, float* c, float* d, float* e) { return; }
//#endif 

#define PSYCH_MAX_CAPTUREDEVICES 100

typedef struct {
    GWorldPtr           gworld;
    SeqGrabComponent 	seqGrab;	// Sequence grabber.
    SGChannel           sgchanVideo;    // Video channel of sequence grabber.
    ImageSequence 	decomSeq;	// unique identifier for our decompression sequence
    int nrframes;
    double fps;
    int width;
    int height;
    double last_pts;
    double current_pts;
    int nr_droppedframes;
    int frame_ready;
    int grabber_active;
} PsychVidcapRecordType;

static PsychVidcapRecordType vidcapRecordBANK[PSYCH_MAX_CAPTUREDEVICES];
static int numCaptureRecords = 0;
static Boolean firsttime = TRUE;

OSErr PsychVideoCaptureDataProc(SGChannel c, Ptr p, long len, long *offset, long chRefCon, TimeValue time, short writeType, long refCon)
{
    int handle;
    static unsigned int count = 0;
    OSErr err = noErr;
    CodecFlags	ignore;
    TimeScale 	timeScale;
    
    // Retrieve handle to our capture data structure:
    handle = (int) chRefCon;
    
    // Compute capture timestamp:
    err = SGGetChannelTimeScale(c, &timeScale);
    vidcapRecordBANK[handle].current_pts = (double) time / (double) timeScale;
    
    // GWorld for offscreen rendering available?
    if (vidcapRecordBANK[handle].gworld) {
        // Yes.
        
        // First time invocation for this sequence grabber?
        if (vidcapRecordBANK[handle].decomSeq == 0) {
            // Need to do one-time setup of decompression sequence:
            Rect		sourceRect = { 0, 0 };
            MatrixRecord        scaleMatrix;
            ImageDescriptionHandle imageDesc = (ImageDescriptionHandle) NewHandle(0);
            
            // retrieve a channel’s current sample description, the channel returns a sample description that is
            // appropriate to the type of data being captured
            err = SGGetChannelSampleDescription(c, (Handle)imageDesc);
            //BailErr(err);
            
            /***** IMPORTANT NOTE *****
                
                Previous versions of this sample code made an incorrect decompression
                request.  Intending to draw the DV frame at quarter-size into a quarter-size
                offscreen GWorld, it made the call
                
                err = DecompressSequenceBegin(..., &rect, nil, ...);
            
            passing a quarter-size rectangle as the source rectangle.  The correct
                interpretation of this request is to draw the top-left corner of the DV
                frame cropped at normal size.  Unfortunately, a DV-specific bug in QuickTime
                5 caused it to misinterpret this request and scale the frame to fit.
                
                This bug will be fixed in QuickTime 6.  If your code behaves as intended
                because of the bug, you should fix your code to pass a matrix scaling the
                frame to fit the offscreen gworld:
                
                RectMatrix( & scaleMatrix, &dvFrameRect, &gworldBounds );
            err = DecompressSequenceBegin(..., nil, &scaleMatrix, ...);
            
            This approach will work in all versions of QuickTime.
                
                **************************/
            
            // make a scaling matrix for the sequence
            //sourceRect.right = (**imageDesc).width;
            //sourceRect.bottom = (**imageDesc).height;
            vidcapRecordBANK[handle].width = (**imageDesc).width;
            vidcapRecordBANK[handle].height = (**imageDesc).height;
            
            
            //RectMatrix(&scaleMatrix, &sourceRect, &gMungData->boundsRect);
            
            // begin the process of decompressing a sequence of frames
            // this is a set-up call and is only called once for the sequence - the ICM will interrogate different codecs
            // and construct a suitable decompression chain, as this is a time consuming process we don't want to do this
            // once per frame (eg. by using DecompressImage)
            // for more information see Ice Floe #8 http://developer.apple.com/quicktime/icefloe/dispatch008.html
            // the destination is specified as the GWorld
            err = DecompressSequenceBegin(&(vidcapRecordBANK[handle].decomSeq),	// pointer to field to receive unique ID for sequence
                                          imageDesc,			// handle to image description structure
                                          vidcapRecordBANK[handle].gworld,   // port for the DESTINATION image
                                          NULL,					// graphics device handle, if port is set, set to NULL
                                          NULL,					// source rectangle defining the portion of the image to decompress 
                                          NULL, //&scaleMatrix,			// transformation matrix
                                          srcCopy,				// transfer mode specifier
                                          (RgnHandle)NULL,		// clipping region in dest. coordinate system to use as a mask
                                          NULL,					// flags
                                          codecNormalQuality, 	// accuracy in decompression
                                          bestSpeedCodec);		// compressor identifier or special identifiers ie. bestSpeedCodec
            //BailErr(err);
            
            DisposeHandle((Handle)imageDesc);         
            
            // One-Time setup of decompression engine done.
        }
        
        // Decompress new frame into our offscreen GWorld:
        err = DecompressSequenceFrameS(vidcapRecordBANK[handle].decomSeq,	// sequence ID returned by DecompressSequenceBegin
                                       p,					// pointer to compressed image data
                                       len,					// size of the buffer
                                       0,					// in flags
                                       &ignore,				// out flags
                                       NULL);				// async completion proc
        
        if (err!=noErr) {
            printf("PTB-ERROR: Error in Video capture callback!!!");
            fflush(NULL);
        }
        
        // Now we should have the required texture data in our GWorld...
        // Increment the newimage - flag:
        vidcapRecordBANK[handle].frame_ready++;
    }
    
    
    return(noErr);
}

/*
 *     PsychVideoCaptureInit() -- Initialize video capture subsystem.
 *     This routine is called by Screen's RegisterProject.c PsychModuleInit()
 *     routine at Screen load-time. It clears out the vidcapRecordBANK to
 *     bring the subsystem into a clean initial state.
 */
void PsychVideoCaptureInit(void)
{
    // Initialize vidcapRecordBANK with NULL-entries:
    int i;
    for (i=0; i < PSYCH_MAX_CAPTUREDEVICES; i++) {
        vidcapRecordBANK[i].gworld = (GWorldPtr) NULL;
        vidcapRecordBANK[i].seqGrab = (SeqGrabComponent) NULL;
        vidcapRecordBANK[i].decomSeq = NULL;
        vidcapRecordBANK[i].grabber_active = 0;
    }    
    numCaptureRecords = 0;
    
    return;
}

/*
 *      PsychOpenVideoCaptureDevice() -- Create a video capture object.
 *
 *      This function tries to open a Quicktime-Sequencegrabber
 *      and return the associated captureHandle for it.
 *
 *      win = Pointer to window record of associated onscreen window.
 *      deviceIndex = Index of the grabber device. (Currently ignored)
 *      capturehandle = handle to the new capture object.
 */
bool PsychOpenVideoCaptureDevice(PsychWindowRecordType *win, int deviceIndex, int* capturehandle)
{
    #define BailErr(x) {error = x; if(error != noErr) goto bail;}    
    *capturehandle = -1;
    int i, slotid;
    OSErr error;
    char msgerr[10000];
    char errdesc[1000];
    Rect movierect;
    SeqGrabComponent seqGrab = NULL;
    SGChannel *sgchanptr = NULL;
    error=noErr;
    ImageDescriptionHandle imageDesc;
    Fixed framerate;
 
    // We startup the Quicktime subsystem only on first invocation.
    if (firsttime) {
#if PSYCH_SYSTEM == PSYCH_WINDOWS
        // Initialize Quicktime for Windows compatibility layer: This will fail if
        // QT isn't installed on the Windows machine...
        error = InitializeQTML(0);
        if (error!=noErr) {
            PsychErrorExitMsg(PsychError_internal, "Quicktime Media Layer initialization failed: Quicktime not properly installed?!?");
        }
#endif

        // Initialize Quicktime-Subsystem:
        error = EnterMovies();
        if (error!=noErr) {
            PsychErrorExitMsg(PsychError_internal, "Quicktime EnterMovies() failed!!!");
        }
        firsttime = FALSE;
    }
    
    if (!PsychIsOnscreenWindow(win)) {
        PsychErrorExitMsg(PsychError_user, "Provided windowPtr is not an onscreen window.");
    }

    if (deviceIndex < 0) {
        PsychErrorExitMsg(PsychError_internal, "Invalid (negative) deviceIndex passed!");
    }

    if (numCaptureRecords >= PSYCH_MAX_CAPTUREDEVICES) {
        PsychErrorExitMsg(PsychError_user, "Allowed maximum number of simultaneously open capture devices exceeded!");
    }

    // Search first free slot in vidcapRecordBANK:
    for (i=0; (i < PSYCH_MAX_CAPTUREDEVICES) && (vidcapRecordBANK[i].gworld); i++);
    if (i>=PSYCH_MAX_CAPTUREDEVICES) {
        PsychErrorExitMsg(PsychError_user, "Allowed maximum number of simultaneously open capture devices exceeded!");
    }

    // Slot slotid will contain the record for our new capture object:
    slotid=i;
    
    // Zero-out new record:
    vidcapRecordBANK[slotid].gworld=NULL;
        
    // Open sequence grabber:
    // ======================
    
    // Open the default sequence grabber: The deviceIndex is currently ignored.
    seqGrab = OpenDefaultComponent(SeqGrabComponentType, 0);
    if (seqGrab == NULL) {
        PsychErrorExitMsg(PsychError_internal, "Failed to open the default sequence grabber for video capture!");
    }
    
    // Initialize the sequence grabber component:
    error=noErr;
    error = SGInitialize(seqGrab);
    
    if (error != noErr) {
        if (seqGrab) CloseComponent(seqGrab);
        PsychErrorExitMsg(PsychError_internal, "SGInitialize() for capture device failed!"); 
    }

    // specify the destination data reference for a record operation
    // tell it we're not making a movie
    // if the flag seqGrabDontMakeMovie is used, the sequence grabber still calls
    // your data function, but does not write any data to the movie file
    // writeType will always be set to seqGrabWriteAppend
    error = SGSetDataRef(seqGrab, 0, 0, seqGrabDontMakeMovie);
    if (error !=noErr) {
        DisposeGWorld(vidcapRecordBANK[slotid].gworld);
        vidcapRecordBANK[slotid].gworld = NULL;
        if (seqGrab) CloseComponent(seqGrab);
        PsychErrorExitMsg(PsychError_internal, "SGSetDataRef for capture device failed!");            
    }

    sgchanptr = &(vidcapRecordBANK[slotid].sgchanVideo);
    // Create and setup video channel on sequence grabber:
    error = SGNewChannel(seqGrab, VideoMediaType, sgchanptr);
    if (error == noErr) {
        SGGetSrcVideoBounds(*sgchanptr, &movierect);
        error = SGSetChannelBounds(*sgchanptr, &movierect);
        if (error == noErr) {
            // set usage for new video channel to avoid playthrough
            // note we don't set seqGrabPlayDuringRecord
            error = SGSetChannelUsage(*sgchanptr, seqGrabRecord );
        }
        
        if (error != noErr) {
            // clean up on failure
            SGDisposeChannel(seqGrab, *sgchanptr);
            *sgchanptr = NULL;
            DisposeGWorld(vidcapRecordBANK[slotid].gworld);
            vidcapRecordBANK[slotid].gworld = NULL;
            if (seqGrab) CloseComponent(seqGrab);
            PsychErrorExitMsg(PsychError_internal, "SGSetChannelBounds() or SGSetChannelUsage for capture device failed!");            
        }
    }
    else {
bail:
        // clean up on failure
        *sgchanptr = NULL;
        DisposeGWorld(vidcapRecordBANK[slotid].gworld);
        vidcapRecordBANK[slotid].gworld = NULL;
        if (seqGrab) CloseComponent(seqGrab);
        PsychErrorExitMsg(PsychError_internal, "SGNewChannel() for capture device failed!");            
    }

    // Retrieve a channel’s current sample description, the channel returns a sample description that is
    // appropriate to the type of data being captured
//    imageDesc = (ImageDescriptionHandle) NewHandle(0);
//    error = SGGetChannelSampleDescription(vidcapRecordBANK[slotid].sgchanVideo, (Handle)imageDesc);
    
//    movierect.left = 0;
//    movierect.top = 0;
//    movierect.right = (**imageDesc).width;
//    movierect.bottom = (**imageDesc).height;
    
//    DisposeHandle((Handle)imageDesc);
    
    // Create GWorld for this grabber object:
    error = QTNewGWorld(&vidcapRecordBANK[slotid].gworld, 0, &movierect,  NULL, NULL, 0);
    if (error!=noErr) {
        if (seqGrab) CloseComponent(seqGrab);
        PsychErrorExitMsg(PsychError_internal, "Quicktime GWorld creation for capture device failed!");
    }
    
    // Set grabbers graphics world to our GWorld:
    error = SGSetGWorld(seqGrab, vidcapRecordBANK[slotid].gworld, NULL );
    
    if (error !=noErr) {
        DisposeGWorld(vidcapRecordBANK[slotid].gworld);
        vidcapRecordBANK[slotid].gworld = NULL;
        if (seqGrab) CloseComponent(seqGrab);
        PsychErrorExitMsg(PsychError_internal, "Assignment of GWorld to capture device failed!");            
    }
    
    // Specify a data callback function:
    error = SGSetDataProc(seqGrab, NewSGDataUPP(PsychVideoCaptureDataProc), NULL);
    BailErr(error);
    
    // Store a reference to our slotid for this channel. This gets passed to the
    // videocapture callback fcn. so it knows to which capture object to relate to...
    SGSetChannelRefCon(vidcapRecordBANK[slotid].sgchanVideo, slotid);
    
    // Get ready!
    error = SGPrepare(seqGrab, false, true);
    BailErr(error); 
    
    // Grabber should be ready now.
    
    // Assign new record:
    vidcapRecordBANK[slotid].seqGrab=seqGrab;    
    vidcapRecordBANK[slotid].decomSeq=NULL;    
    vidcapRecordBANK[slotid].grabber_active = 0;

    // Assign final handle:
    *capturehandle = slotid;

    // Increase counter:
    numCaptureRecords++;

    // Query capture framerate:
    SGGetFrameRate(vidcapRecordBANK[slotid].sgchanVideo, &framerate);
    vidcapRecordBANK[slotid].fps = (double) FixedToFloat(framerate);

    // Determine size of images in movie:
    vidcapRecordBANK[slotid].width = movierect.right - movierect.left;
    vidcapRecordBANK[slotid].height = movierect.bottom - movierect.top;
    
    // We set nrframes == -1 to indicate that this value is not yet available.
    // Will do counting on first query for this parameter as it is very time-consuming:
    vidcapRecordBANK[slotid].nrframes = -1;
    printf("W x h = %i x  %i at %lf fps...", vidcapRecordBANK[slotid].width, vidcapRecordBANK[slotid].height, vidcapRecordBANK[slotid].fps);
    return(TRUE);
}

/*
 *  PsychCloseVideoCaptureDevice() -- Close a capture device and release all associated ressources.
 */
void PsychCloseVideoCaptureDevice(int capturehandle)
{
    if (capturehandle < 0 || capturehandle >= PSYCH_MAX_CAPTUREDEVICES) {
        PsychErrorExitMsg(PsychError_user, "Invalid capturehandle provided!");
    }
    
    if (vidcapRecordBANK[capturehandle].gworld == NULL) {
        PsychErrorExitMsg(PsychError_user, "Invalid capturehandle provided. No capture device associated with this handle !!!");
    }
        
    // Stop capture immediately:
    SGStop(vidcapRecordBANK[capturehandle].seqGrab);
    
    // Delete GWorld if any:
    if (vidcapRecordBANK[capturehandle].gworld) DisposeGWorld(vidcapRecordBANK[capturehandle].gworld);
    vidcapRecordBANK[capturehandle].gworld = NULL;
    
    // Release grabber:
    CloseComponent(vidcapRecordBANK[capturehandle].seqGrab);
    vidcapRecordBANK[capturehandle].seqGrab = NULL;
    vidcapRecordBANK[capturehandle].grabber_active = 0;
    
    // Decrease counter:
    if (numCaptureRecords>0) numCaptureRecords--;
        
    return;
}

/*
 *  PsychDeleteAllCaptureDevices() -- Delete all capture objects and release all associated ressources.
 */
void PsychDeleteAllCaptureDevices(void)
{
    int i;
    for (i=0; i<PSYCH_MAX_CAPTUREDEVICES; i++) {
        if (vidcapRecordBANK[i].gworld) PsychCloseVideoCaptureDevice(i);
    }
    return;
}


/*
 *  PsychGetTextureFromCapture() -- Create an OpenGL texturemap from a specific videoframe from given capture object.
 *
 *  win = Window pointer of onscreen window for which a OpenGL texture should be created.
 *  capturehandle = Handle to the capture object.
 *  checkForImage = true == Just check if new image available, false == really retrieve the image, blocking if necessary.
 *  timeindex = When not in playback mode, this allows specification of a requested frame by presentation time.
 *              If set to -1, or if in realtime playback mode, this parameter is ignored and the next video frame is returned.
 *  out_texture = Pointer to the Psychtoolbox texture-record where the new texture should be stored.
 *  presentation_timestamp = A ptr to a double variable, where the presentation timestamp of the returned frame should be stored.
 *
 *  Returns true (1) on success, false (0) if no new image available, -1 if no new image available and there won't be any in future.
 */
int PsychGetTextureFromCapture(PsychWindowRecordType *win, int capturehandle, int checkForImage, double timeindex, PsychWindowRecordType *out_texture, double *presentation_timestamp)
{
    TimeValue		myCurrTime;
    TimeValue		myNextTime;
    TimeValue           nextFramesTime=0;
    OSErr		error = noErr;
    GLuint texid;
    Rect rect;
    int w, h;
    double targetdelta, realdelta, frames;
    Boolean newframe = FALSE;
        
    // Activate OpenGL context of target window: We'll need it for texture fetch...
    PsychSetGLContext(win);
    
    // Sanity checks:
    if (capturehandle < 0 || capturehandle >= PSYCH_MAX_CAPTUREDEVICES) {
        PsychErrorExitMsg(PsychError_user, "Invalid capturehandle provided.");
    }
    
    if ((timeindex!=-1) && (timeindex < 0 || timeindex >= 10000.0)) {
        PsychErrorExitMsg(PsychError_user, "Invalid timeindex provided.");
    }
    
    if (NULL == out_texture && !checkForImage) {
        PsychErrorExitMsg(PsychError_internal, "NULL-Ptr instead of out_texture ptr passed!!!");
    }
    
    // Grant some processing time to the sequence grabber engine:
    SGIdle(vidcapRecordBANK[capturehandle].seqGrab);
    
    // Check if a new captured frame is ready for retrieval...
    newframe = (Boolean) vidcapRecordBANK[capturehandle].frame_ready;
    // ...and clear out the ready flag immediately:
    vidcapRecordBANK[capturehandle].frame_ready = 0;

    // Presentation timestamp requested?
    if (presentation_timestamp) {
        // Already available? Return it:
        *presentation_timestamp =vidcapRecordBANK[capturehandle].current_pts;
    }
    
    // Should we just check for new image? If so, just return availability status:
    if (checkForImage) {
        if (vidcapRecordBANK[capturehandle].grabber_active == 0) {
            // Grabber stopped. We'll never get a new image:
            return(-1);
        }

        // Grabber active. Just return availability status:
        return(newframe);
    }
    
    // This point is only reached if checkForImage == FALSE, which only happens
    // if a new frame is available in our GWorld:

    // Synchronous texture fetch code for GWorld rendering mode:
        
    // Disable client storage, if it is enabled for some reason:
    glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_FALSE);
    
    // Build a standard PTB texture record:    
    
    // Assign texture rectangle:
    w=vidcapRecordBANK[capturehandle].width;
    h=vidcapRecordBANK[capturehandle].height;
    SetRect(&rect, 0, 0, w, h);
    
    // Hack: Need to extend rect by 4 pixels, because GWorlds are 4 pixels-aligned via
    // image row padding:
    rect.right = rect.right + 4;
    PsychMakeRect(out_texture->rect, rect.left, rect.top, rect.right, rect.bottom);    
    
    // Set NULL - special texture object as part of the PTB texture record:
    out_texture->targetSpecific.QuickTimeGLTexture = NULL;
    
    // Set texture orientation as if it were an inverted Offscreen window: Upside-down.
    out_texture->textureOrientation = 3;
    
    // Setup a pointer to our GWorld as texture data pointer:
    out_texture->textureMemorySizeBytes = 0;
    
    // Lock GWorld:
    if(!LockPixels(GetGWorldPixMap(vidcapRecordBANK[capturehandle].gworld))) {
        // Locking surface failed! We abort.
        PsychErrorExitMsg(PsychError_internal, "PsychGetTextureFromCapture(): Locking GWorld pixmap surface failed!!!");
    }
    
    // This will retrieve an OpenGL compatible pointer to the GWorlds pixel data and assign it to our texmemptr:
    out_texture->textureMemory = (GLuint*) GetPixBaseAddr(GetGWorldPixMap(vidcapRecordBANK[capturehandle].gworld));
    
    // Let PsychCreateTexture() do the rest of the job of creating, setting up and
    // filling an OpenGL texture with GWorlds content:
    PsychCreateTexture(out_texture);
    
    // Undo hack from above after texture creation: Now we need the real width of the
    // texture for proper texture coordinate assignments in drawing code et al.
    //rect.right = rect.right - 4;
    PsychMakeRect(out_texture->rect, rect.left, rect.top, rect.right, rect.bottom);    
    
    // Unlock GWorld surface.
    UnlockPixels(GetGWorldPixMap(vidcapRecordBANK[capturehandle].gworld));
    
    // Ready to use the texture... We're done.
    
    // Detection of dropped frames: This is a heuristic. We'll see how well it works out...
    
    // Expected delta between successive presentation timestamps:
    targetdelta = 1.0f / 25; // vidcapRecordBANK[capturehandle].fps;
    
    // Compute real delta, given rate and playback direction:
    realdelta = *presentation_timestamp - vidcapRecordBANK[capturehandle].last_pts;
    if (realdelta<0) realdelta = 0;
    
    frames = realdelta / targetdelta;

    // Dropped frames?
    if (frames > 1 && vidcapRecordBANK[capturehandle].last_pts>=0) {
        vidcapRecordBANK[capturehandle].nr_droppedframes += (int) (frames - 1 + 0.5);
    }
    
    vidcapRecordBANK[capturehandle].last_pts = *presentation_timestamp;
    
    // We're successfully done!
    return(TRUE);
}

/*
 *  PsychVideoCaptureRate() - Start- and stop video capture.
 *
 *  capturehandle = Grabber to start-/stop.
 *  playbackrate = zero == Stop capture, non-zero == Capture
 *  Returns Number of dropped frames during capture.
 */
int PsychVideoCaptureRate(int capturehandle, double capturerate, int loop)
{
    int dropped = 0;
    OSErr error = noErr;
    Fixed framerate;
    
    if (capturehandle < 0 || capturehandle >= PSYCH_MAX_CAPTUREDEVICES) {
        PsychErrorExitMsg(PsychError_user, "Invalid capturehandle provided!");
    }
        
    // Fetch references to objects we need:
    if (vidcapRecordBANK[capturehandle].seqGrab == NULL) {
        PsychErrorExitMsg(PsychError_user, "Invalid capturehandle provided. No movie associated with this handle !!!");
    }
    
    if (capturerate != 0) {
        // Start capture:
        error = SGStartRecord(vidcapRecordBANK[capturehandle].seqGrab);
        vidcapRecordBANK[capturehandle].last_pts = -1.0;
        vidcapRecordBANK[capturehandle].nr_droppedframes = 0;
        vidcapRecordBANK[capturehandle].frame_ready = 0;
        vidcapRecordBANK[capturehandle].grabber_active = 1;
        
        SGGetFrameRate(vidcapRecordBANK[capturehandle].sgchanVideo, &framerate);
        vidcapRecordBANK[capturehandle].fps = (double) FixedToFloat(framerate);


    }
    else {
        // Stop capture:
        error = SGStop(vidcapRecordBANK[capturehandle].seqGrab);
        vidcapRecordBANK[capturehandle].frame_ready = 0;
        vidcapRecordBANK[capturehandle].grabber_active = 0;

        // Output count of dropped frames:
        if ((dropped=vidcapRecordBANK[capturehandle].nr_droppedframes) > 0) {
            printf("PTB-INFO: Video capture dropped %i frames on device %i to keep pipe running.\n", vidcapRecordBANK[capturehandle].nr_droppedframes, capturehandle); 
        }
    }
    
    return(dropped);
}

/*
 *  void PsychExitVideoCapture() - Shutdown handler.
 *
 *  This routine is called by Screen('CloseAll') and on clear Screen time to
 *  do final cleanup. It deletes all capture objects
 *
 */
void PsychExitVideoCapture(void)
{
    // Release all capture devices
    PsychDeleteAllCaptureDevices();
    
    // Shutdown Quicktime toolbox: We skip this, because according to Apple its not necessary,
    // and for some reason it reliably hangs Matlab, so one has to force-quit it :-(
    // Don't do this: ExitMovies();
#if PSYCH_SYSTEM == PSYCH_WINDOWS
    // Shutdown Quicktime core system:
    ExitMovies();
    
    // Shutdown Quicktime for Windows compatibility layer:
    TerminateQTML();
#endif
    
    return;
}
