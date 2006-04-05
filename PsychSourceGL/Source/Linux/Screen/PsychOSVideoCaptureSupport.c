/*
	Linux/Screen/PsychOSVideoCaptureSupport.c
	
	PLATFORMS:	
	
		This is the GNU/Linux specific version.
		The header file with the API is shared with the OS-X and Windows versions and
		located in Common/Screen/PsychVideoCaptureSupport.h

		--> Linux version has same API but substantially different implementation.
				
	AUTHORS:
	
		Mario Kleiner           mk              mario.kleiner@tuebingen.mpg.de

	HISTORY:
	
        DESCRIPTION:
	
	Psychtoolbox functions for dealing with video capture devices on GNU/Linux.

	For now, we only support machine vision cameras that are connected via the IEEE-1394
	Firewire-bus and that conform to the IDDC-1.0 (and later) standard for firewire
	machine vision cameras. These cameras are handled on Linux via the (statically linked)
	libdc1394 in combination with libraw1394. They provide high performance streaming of
	uncompressed camera data over firewire and a lot of features (e.g., external sync.
	triggers) not useful for consumer cameras, but very useful for computer vision
	applications and things like eye-trackers, ...
	
	NOTES:

*/


#include "Screen.h"

#include <libraw1394/raw1394.h>
#include <dc1394/dc1394_control.h>
#include <syslog.h>

// Set DROP_FRAMES 1  for framedropping enabled. Set to 0 for queueing.
#define DROP_FRAMES 1
// Number of DMA ringbuffers to use:
#define DC1394_BUFFERS 8

#define PSYCH_MAX_CAPTUREDEVICES 10

// Record which defines all state for a capture device:
typedef struct {
  int valid;                        // Is this a valid device record? zero == Invalid.
  dc1394camera_t *camera;           // Ptr to a DC1394 camera object that holds the internal state for such cams.
  int dc_imageformat;               // Encodes image size and pixelformat.
  int pixeldepth;                   // Depth of single pixel in bits.
  int nrframes;                     // Total count of decompressed images.
  double fps;                       // Acquisition framerate of capture device.
  int width;                        // Width x height of captured images.
  int height;
  double last_pts;                  // Capture timestamp of previous frame.
  double current_pts;               // Capture timestamp of current frame
  int nr_droppedframes;             // Counter for dropped frames.
  int frame_ready;                  // Signals availability of new frames for conversion into GL-Texture.
  int grabber_active;               // Grabber running?
  //  Rect roirect;                     // Region of interest rectangle - denotes subarea of full video capture area.
  double avg_decompresstime;        // Average time spent in Quicktime/Sequence Grabber decompressor.
  double avg_gfxtime;               // Average time spent in GWorld --> OpenGL texture conversion and statistics.
  int nrgfxframes;                  // Count of fetched textures.
} PsychVidcapRecordType;

static PsychVidcapRecordType vidcapRecordBANK[PSYCH_MAX_CAPTUREDEVICES];
static int numCaptureRecords = 0;
static Boolean firsttime = TRUE;

/*    PsychGetVidcapRecord() -- Given a handle, return ptr to video capture record.
 *    --> Internal helper function of PsychVideoCaptureSupport.
 */
PsychVidcapRecordType* PsychGetVidcapRecord(int deviceIndex)
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
    vidcapRecordBANK[i].valid = 0;
  }    
  numCaptureRecords = 0;
  
  return;
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
  
  // Reset firsttime flag to get a cold restart on next invocation of Screen:
  firsttime = TRUE;
  return;
}

/*
 *  PsychDeleteAllCaptureDevices() -- Delete all capture objects and release all associated ressources.
 */
void PsychDeleteAllCaptureDevices(void)
{
  int i;
  for (i=0; i<PSYCH_MAX_CAPTUREDEVICES; i++) {
    if (vidcapRecordBANK[i].valid) PsychCloseVideoCaptureDevice(i);
  }
  return;
}

/*
 *  PsychCloseVideoCaptureDevice() -- Close a capture device and release all associated ressources.
 */
void PsychCloseVideoCaptureDevice(int capturehandle)
{
  // Retrieve device record for handle:
  PsychVidcapRecordType* capdev = PsychGetVidcapRecord(capturehandle);
        
  // Stop capture immediately if it is still running:
  PsychVideoCaptureRate(capturehandle, 0, 0);

  // Close & Shutdown camera, release ressources:
  dc1394_free_camera(capdev->camera);
  capdev->camera = NULL;

  // Invalidate device record to free up this slot in the array:
  capdev->valid = 0;
    
  // Decrease counter of open capture devices:
  if (numCaptureRecords>0) numCaptureRecords--;
  
  // Done.
  return;
}

/*
 *      PsychOpenVideoCaptureDevice() -- Create a video capture object.
 *
 *      This function tries to open and initialize a connection to a IEEE1394
 *      Firewire machine vision camera and return the associated captureHandle for it.
 *
 *      win = Pointer to window record of associated onscreen window.
 *      deviceIndex = Index of the grabber device. (Currently ignored)
 *      capturehandle = handle to the new capture object.
 *      capturerectangle = If non-NULL a ptr to a PsychRectangle which contains the ROI for capture.
 */
bool PsychOpenVideoCaptureDevice(PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle)
{
    PsychVidcapRecordType* capdev = NULL;
    dc1394camera_t **cameras=NULL;
    uint_t numCameras;
    dc1394featureset_t features;
    int err;

    int i, slotid;
    char msgerr[10000];
    // char errdesc[1000];
    //    Rect movierect, newrect;
    *capturehandle = -1;

    
    if (firsttime) {
      // Nothing to do for now...
      firsttime = FALSE;
    }
    
    // Sanity checks:
    if (!PsychIsOnscreenWindow(win)) {
      PsychErrorExitMsg(PsychError_user, "Provided windowPtr is not an onscreen window.");
    }
    
    if (deviceIndex < -1) {
      PsychErrorExitMsg(PsychError_internal, "Invalid (negative) deviceIndex passed!");
    }

    if (numCaptureRecords >= PSYCH_MAX_CAPTUREDEVICES) {
      PsychErrorExitMsg(PsychError_user, "Allowed maximum number of simultaneously open capture devices exceeded!");
    }

    // Search first free slot in vidcapRecordBANK:
    for (i=0; (i < PSYCH_MAX_CAPTUREDEVICES) && (vidcapRecordBANK[i].valid); i++);
    if (i>=PSYCH_MAX_CAPTUREDEVICES) {
      PsychErrorExitMsg(PsychError_user, "Allowed maximum number of simultaneously open capture devices exceeded!");
    }
    
    // Slot slotid will contain the record for our new capture object:
    slotid=i;
    
    // Initialize new record:
    vidcapRecordBANK[slotid].valid = 1;
    
    // Retrieve device record for slotid:
    capdev = PsychGetVidcapRecord(slotid);

    capdev->camera = NULL;
    capdev->grabber_active = 0;
        
    // Query a list of all available (connected) Firewire cameras:
    err=dc1394_find_cameras(&cameras, &numCameras);

    if (err!=DC1394_SUCCESS) {
      // Failed to detect any cameras: Invalidate our record.
      capdev->valid = 0;
      PsychErrorExitMsg(PsychError_user, "Unable to detect Firewire cameras: Please make sure that you have read/write access to\n"
			"/dev/raw1394 and that the kernel modules `ieee1394',`raw1394' and `ohci1394' are successfully loaded.\n"
			"Ask your system administrator for assistance or read 'help LinuxFirewire'.");
    }

    // Any cameras?
    if (numCameras<1) {
      // Failed to find a camera: Invalidate our record.
      capdev->valid = 0;
      PsychErrorExitMsg(PsychError_user, "Unable to find any Firewire camera: Please make sure that there's actually one connected.\n"
			"Please note that we only support IDDC compliant machine vision cameras, not standard consumer DV cameras!");
    }

    // Specific cam requested?
    if (deviceIndex==-1) {
      // Nope. We just use the first one.
      capdev->camera=cameras[0];
      printf("PTB-INFO: Opening the first Firewire camera on the IEEE1394 bus.\n");
    }
    else {
      // Does a camera with requested index exist?
      if (deviceIndex>=numCameras) {
	// No such cam.
	capdev->valid = 0;
	sprintf(msgerr, "You wanted me to open the %i th camera (deviceIndex: %i), but there are only %i cameras available!",
		deviceIndex + 1, deviceIndex, numCameras);
	PsychErrorExitMsg(PsychError_user, msgerr);
      }

      // Ok, valid device index: Assign cam:
      capdev->camera=cameras[deviceIndex];
      printf("PTB-INFO: Opening the %i. Firewire camera (deviceIndex=%i) out of %i cams on the IEEE1394 bus.\n",
	     deviceIndex + 1, deviceIndex, numCameras);
    }

    fflush(NULL);

    // Free the unused cameras:
    for (i=0; i<numCameras; i++) if (cameras[i]!=capdev->camera) dc1394_free_camera(cameras[i]);
    free(cameras);
    cameras=NULL;
    
    // Hack to fix a bug in libdc: This pointer doesn't get NULL initialized, so
    // it can be accidentally non-NULL due to garbage in memory!
    //    capdev->camera->capture.dma_device_file = NULL;

    // Report cameras features:
    if (dc1394_get_camera_feature_set(capdev->camera, &features) !=DC1394_SUCCESS) {
      printf("PTB-WARNING: Unable to query feature set of camera.\n");
    }
    else {
      printf("PTB-INFO: The camera provides the following feature set:\n");
      dc1394_print_feature_set(&features);
      fflush(NULL);
    }
    
    //     vidcapRecordBANK[slotid].roirect = movierect;
    //     movierect.right-=movierect.left;
    //     movierect.bottom-=movierect.top;
    //     movierect.left=0;
    //     movierect.top=0;
    
    // Our camera should be ready: Assign final handle.
    *capturehandle = slotid;

    // Increase counter of open capture devices:
    numCaptureRecords++;
    
    // Set dummy value for framerate for now:
    capdev->fps = 25;

    // Set image size:
    capdev->width = 640;
    capdev->height = 480;

    // Reset framecounter:
    capdev->nrframes = 0;
    capdev->grabber_active = 0;
    
    printf("W x h = %i x  %i at %lf fps...\n", capdev->width, capdev->height, capdev->fps);
    fflush(NULL);

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
  int framerate = 0;
  unsigned int speed;
  int mode;

  // Retrieve device record for handle:
  PsychVidcapRecordType* capdev = PsychGetVidcapRecord(capturehandle);

  // Start- or stop capture?
  if (capturerate != 0) {
    // Start capture:
    if (capdev->grabber_active) PsychErrorExitMsg(PsychError_user, "You tried to start video capture, but capture is already started!");

    // Reset statistics:
    capdev->last_pts = -1.0;
    capdev->nr_droppedframes = 0;
    capdev->frame_ready = 0;
    
    // Set capture framerate:
    framerate = (int) capturerate;

    // MK: TODO!!!

    // Setup capture hardware and DMA engine:

    // Select mode for requested image size and pixel format:
/*     switch (palette) { */
/*     case VIDEO_PALETTE_RGB24: */
/*       mode = DC1394_VIDEO_MODE_640x480_RGB8; */
/*       break; */
      
/*     case VIDEO_PALETTE_YUV422: */
/*     case VIDEO_PALETTE_YUV422P: */
/*     case VIDEO_PALETTE_YUV420P: */
/*       mode = DC1394_VIDEO_MODE_640x480_YUV422; */
/*       break; */
      
/*     default: */
/*       return 0; */
/*     } */

    // For now we hardcode to 640x480 pixels greyscale:
    mode = DC1394_VIDEO_MODE_640x480_MONO8;
    capdev->dc_imageformat = mode;
    capdev->pixeldepth = 8;

    if (dc1394_video_get_iso_speed(capdev->camera, &speed)!=DC1394_SUCCESS) {
      PsychErrorExitMsg(PsychError_internal, "Unable to query bus-speed - Start of video capture failed!");
    }
	
    // Setup DMA engine:
    // We use a framerate of 60 fps with frame dropping enabled:
    if (dc1394_dma_setup_capture(capdev->camera, mode, speed, DC1394_FRAMERATE_60,
				 DC1394_BUFFERS, DROP_FRAMES) != DC1394_SUCCESS) {
      PsychErrorExitMsg(PsychError_internal, "Unable to setup and start DMA capture engine - Start of video capture failed!");
    }

    // Start DMA driven isochronous data transfer:
    if (dc1394_video_set_transmission(capdev->camera, DC1394_ON) !=DC1394_SUCCESS) {
      PsychErrorExitMsg(PsychError_internal, "Unable to start isochronous data transfer from camera - Start of video capture failed!");
    }
    
    capdev->fps = (double) framerate;

    // Ok, capture is now started:
    capdev->grabber_active = 1;
    
    printf("PTB-INFO: Capture started on device %i - Framerate: %f fps.\n", capturehandle, capdev->fps);
  }
  else {
    // Stop capture:
    if (capdev->grabber_active) {

      // Wait for 2 seconds before really stopping capture. This - for some reason -
      // prevents Kernel Ooops or even crashes under Linux 2.4.19.
      // PsychWaitIntervalSeconds(2);

      // Stop isochronous data transfer from camera:
      if (dc1394_video_set_transmission(capdev->camera, DC1394_OFF) !=DC1394_SUCCESS) {
	PsychErrorExitMsg(PsychError_internal, "Unable to stop video transfer on camera! (dc1394_video_set_transmission(DC_OFF) failed)!");
      }

      // Wait for 500 msecs so data-transfer is really off.
      // PsychWaitIntervalSeconds(0.5);

      // Shutdown DMA engine:
      dc1394_dma_unlisten(capdev->camera);

      // Wait for 500 msecs so engine is really stopped.
      // PsychWaitIntervalSeconds(0.5);

      // Release DMA engine:
      dc1394_dma_release_camera(capdev->camera);

      // Ok, capture is now stopped.
      capdev->frame_ready = 0;
      capdev->grabber_active = 0;
    
      // Output count of dropped frames:
      if ((dropped=capdev->nr_droppedframes) > 0) {
	printf("PTB-INFO: Video capture dropped %i frames on device %i to keep capture running in sync with realtime.\n", dropped, capturehandle); 
      }
      
      if (capdev->nrframes>0) capdev->avg_decompresstime/= (double) capdev->nrframes;
      printf("PTB-INFO: Average time spent in video decompressor (waiting/polling for new frames) was %lf milliseconds.\n", capdev->avg_decompresstime * 1000.0f);
      if (capdev->nrgfxframes>0) capdev->avg_gfxtime/= (double) capdev->nrgfxframes;
      printf("PTB-INFO: Average time spent in GetCapturedImage (intensity calculation and/or Video->OpenGL texture conversion) was %lf milliseconds.\n", capdev->avg_gfxtime * 1000.0f);
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
 *  summed_intensity = An optional ptr to a double variable. If non-NULL, then sum of intensities over all channels is calculated and returned.
 *  Returns true (1) on success, false (0) if no new image available, -1 if no new image available and there won't be any in future.
 */
int PsychGetTextureFromCapture(PsychWindowRecordType *win, int capturehandle, int checkForImage, double timeindex,
			       PsychWindowRecordType *out_texture, double *presentation_timestamp, double* summed_intensity)
{
    GLuint texid;
    int w, h, padding;
    double targetdelta, realdelta, frames;
    unsigned int intensity = 0;
    unsigned int count, i;
    unsigned char* pixptr;
    Boolean newframe = FALSE;
    double tstart, tend;
    unsigned int pixval, alphacount;

    int waitforframe = (timeindex > 0) ? 1:0; // We misuse the timeindex as flag for blocking- non-blocking mode.

    // Retrieve device record for handle:
    PsychVidcapRecordType* capdev = PsychGetVidcapRecord(capturehandle);

    // Take start timestamp for timing stats:
    PsychGetAdjustedPrecisionTimerSeconds(&tstart);
        
    if ((timeindex!=-1) && (timeindex < 0 || timeindex >= 10000.0)) {
      PsychErrorExitMsg(PsychError_user, "Invalid timeindex provided.");
    }
    
    // Should we just check for new image?
    if (checkForImage) {
      if (capdev->grabber_active == 0) {
	// Grabber stopped. We'll never get a new image:
	return(-1);
      }

      // Grabber active: Polling mode or wait for new frame mode?
      if (waitforframe) {
	// Check for image in blocking mode: We actually try to capture a frame in
	// blocking mode, so we will wait here until a new frame arrives.
      	if (dc1394_dma_capture(&(capdev->camera), 1, DC1394_VIDEO1394_WAIT) == DC1394_SUCCESS) {
	  // if (dc1394_capture(&(capdev->camera), 1) == DC1394_SUCCESS) {
	  // Ok, new frame ready and dequeued from DMA ringbuffer. We'll return it on next non-poll invocation.
	  capdev->frame_ready = 1;
	}
	else {
	  // Blocking wait failed!
	  PsychErrorExitMsg(PsychError_internal, "Blocking wait for new frame failed!!!");
	}
      }
      else {
	// Check for image in polling mode: We capture in non-blocking mode:
	if (dc1394_dma_capture(&(capdev->camera), 1, DC1394_VIDEO1394_POLL) == DC1394_SUCCESS) {
	  // Ok, new frame ready and dequeued from DMA ringbuffer. We'll return it on next non-poll invocation.
	  capdev->frame_ready = 1;
	}
	else {
	  // No new frame ready yet...
	  capdev->frame_ready = 0;
	}
      }

      if (capdev->frame_ready) {
	// Update stats for decompression:
	PsychGetAdjustedPrecisionTimerSeconds(&tend);

	// Increase counter of decompressed frames:
	capdev->nrframes++;
	// Update avg. decompress time:
	capdev->avg_decompresstime+=(tend - tstart);
	// Query capture timestamp from Firewire subsystem and convert to seconds:
	capdev->current_pts = ((double) capdev->camera->capture.filltime.tv_sec) + (((double) capdev->camera->capture.filltime.tv_usec) / 1000000.0f);
      }

      // Return availability status:
      return(capdev->frame_ready);
    }
    
    // This point is only reached if checkForImage == FALSE, which only happens
    // if a new frame is available in our buffer:
    
    // Presentation timestamp requested?
    if (presentation_timestamp) {
      // Return it:
      *presentation_timestamp = capdev->current_pts;
    }

    // Synchronous texture fetch: Copy content of capture buffer into a texture:
    // =========================================================================

    // Build a standard PTB texture record:    
    
    // Assign texture rectangle:
    w=capdev->width;
    h=capdev->height;
    
    // Hack: Need to extend rect by 4 pixels, because GWorlds are 4 pixels-aligned via
    // image row padding:
#if PSYCH_SYSTEM == PSYCH_OSX
    padding = 4 + (4 - (w % 4)) % 4;
#else
    padding= 0;
#endif
    
    // Only setup if really a texture is requested (non-benchmarking mode):
    if (out_texture) {
      PsychMakeRect(out_texture->rect, 0, 0, w+padding, h);    
      
      // Set NULL - special texture object as part of the PTB texture record:
      out_texture->targetSpecific.QuickTimeGLTexture = NULL;
      
      // Set textureNumber to zero, which means "Not cached, don't recycle"
      // Todo: Texture recycling like in PsychMovieSupport for higher efficiency!
      out_texture->textureNumber = 0;
      
      // Set texture orientation as if it were an inverted Offscreen window: Upside-down.
      out_texture->textureOrientation = 3;
      
      // Setup a pointer to our GWorld as texture data pointer: Setting memsize to zero
      // prevents unwanted free() operation in PsychDeleteTexture...
      out_texture->textureMemorySizeBytes = 0;

      // Set texture depth: Could be 8 bits for mono formats, or 24 for RGB formats.
      out_texture->depth = capdev->pixeldepth;
    
      // This will retrieve an OpenGL compatible pointer to the pixel data and assign it to our texmemptr:
      out_texture->textureMemory = (GLuint*) (capdev->camera->capture.capture_buffer);
      
      // Let PsychCreateTexture() do the rest of the job of creating, setting up and
      // filling an OpenGL texture with content:
      PsychCreateTexture(out_texture);
      
      // Undo hack from above after texture creation: Now we need the real width of the
      // texture for proper texture coordinate assignments in drawing code et al.
      PsychMakeRect(out_texture->rect, 0, 0, w-padding, h);    
      // Ready to use the texture...
    }
    
    // Sum of pixel intensities requested?
    if(summed_intensity) {
      pixptr = (unsigned char*) (capdev->camera->capture.capture_buffer);
      count = (w*h*((capdev->pixeldepth == 24) ? 3 : 1));
      for (i=0; i<count; i++) intensity+=(unsigned int) pixptr[i];
      *summed_intensity = ((double) intensity) / w / h / ((capdev->pixeldepth == 24) ? 3 : 1);
    }

    // Release the capture buffer. Return it to the DMA ringbuffer pool:
    dc1394_dma_done_with_buffer(capdev->camera);

    // Detection of dropped frames: This is a heuristic. We'll see how well it works out...
    if (1) {
      // Old style: Heuristic based on comparison of capture timestamps:
      // Expected delta between successive presentation timestamps:
      targetdelta = 1.0f / capdev->fps;
    
      // Compute real delta:
      realdelta = *presentation_timestamp - capdev->last_pts;
      if (realdelta<0) realdelta = 0;
      frames = realdelta / targetdelta;
      
      // Dropped frames?
      if (frames > 1 && capdev->last_pts>=0) {
        capdev->nr_droppedframes += (int) (frames - 1 + 0.5);
      }

      // Record timestamp as reference for next check:    
      capdev->last_pts = *presentation_timestamp;
    }
    else {
      // New style: Just take values from Firewire subsystem:
      // MK: This wont work - look at framesbehind instead! capdev->nr_droppedframes += (int) capdev->camera->capture.drop_frames;
    }
    
    // Timestamping:
    PsychGetAdjustedPrecisionTimerSeconds(&tend);
    // Increase counter of retrieved textures:
    capdev->nrgfxframes++;
    // Update average time spent in texture conversion:
    capdev->avg_gfxtime+=(tend - tstart);
    
    // We're successfully done!
    return(TRUE);
}
