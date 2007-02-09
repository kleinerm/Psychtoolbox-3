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
	Firewire-bus and that conform to the IIDC-1.0 (and later) standard for firewire
	machine vision cameras. These cameras are handled on Linux via the (statically linked)
	libdc1394-2.0 in combination with libraw1394. They provide high performance streaming of
	uncompressed camera data over firewire and a lot of features (e.g., external sync.
	triggers) not useful for consumer cameras, but very useful for computer vision
	applications and things like eye-trackers, ...
	
	NOTES:

*/


#include "Screen.h"
#include <float.h>

#include <libraw1394/raw1394.h>
#include <dc1394/dc1394_control.h>
#include <dc1394/dc1394_utils.h>
#include <dc1394/dc1394_conversions.h>
#include <syslog.h>

dc1394error_t dc1394_free_iso_channel_and_bandwidth(dc1394camera_t *);

#define PSYCH_MAX_CAPTUREDEVICES 10

// Record which defines all state for a capture device:
typedef struct {
  int valid;                        // Is this a valid device record? zero == Invalid.
  dc1394camera_t *camera;           // Ptr to a DC1394 camera object that holds the internal state for such cams.
  int dma_mode;                     // 0 == Non-DMA fallback path. 1 == DMA-Transfers.
  int allow_nondma_fallback;        // Use of Non-DMA fallback path allowed?
  dc1394video_mode_t dc_imageformat;// Encodes image size and pixelformat.
  dc1394framerate_t dc_framerate;   // Encodes framerate.
  dc1394color_coding_t colormode;   // Encodes color encoding of cameras data.
  unsigned char* scratchbuffer;     // Scratch buffer for YUV->RGB conversion.
  int reqpixeldepth;                // Requested depth of single pixel in output texture.
  int pixeldepth;                   // Depth of single pixel from grabber in bits.
  int num_dmabuffers;               // Number of DMA ringbuffers to use in DMA capture.
  int nrframes;                     // Total count of decompressed images.
  double fps;                       // Acquisition framerate of capture device.
  int width;                        // Width x height of captured images.
  int height;
  double last_pts;                  // Capture timestamp of previous frame.
  double current_pts;               // Capture timestamp of current frame
  int nr_droppedframes;             // Counter for dropped frames.
  int frame_ready;                  // Signals availability of new frames for conversion into GL-Texture.
  int grabber_active;               // Grabber running?
  PsychRectType roirect;            // Region of interest rectangle - denotes subarea of full video capture area.
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
  PsychVideoCaptureRate(capturehandle, 0, 0, NULL);

  // Initiate a power-down cycle to bring camera into standby mode:
  if (dc1394_set_camera_power(capdev->camera, DC1394_OFF)!=DC1394_SUCCESS) {
    printf("PTB-WARNING: Tried to power down camera %i, but powerdown-cycle failed for some reason!\n", capturehandle); fflush(NULL);
  }

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
 *      reqdepth = Number of layers for captured output textures. (0=Don't care, 1=LUMINANCE8, 2=LUMINANCE8_ALPHA8, 3=RGB8, 4=RGBA8)
 *      num_dmabuffers = Number of buffers in the ringbuffer queue (e.g., DMA buffers) - This is OS specific. Zero = Don't care.
 *      allow_lowperf_fallback = If set to 1 then PTB can use a slower, low-performance fallback path to get nasty devices working.
 *		targetmoviefilename and recordingflags are currently ignored, they would refer to video harddics recording capabilities.
 */
bool PsychOpenVideoCaptureDevice(PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle,
				 int reqdepth, int num_dmabuffers, int allow_lowperf_fallback, char* targetmoviefilename, unsigned int recordingflags)
{
    PsychVidcapRecordType* capdev = NULL;
    dc1394camera_t **cameras=NULL;
    uint_t numCameras;
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
    capdev->scratchbuffer = NULL;        

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
    
    // ROI rectangle specified?
    if (capturerectangle) {
      PsychCopyRect(capdev->roirect, capturerectangle);
    }
    else {
      // Create empty pseudo-rect, which means "don't care":
      PsychMakeRect(capdev->roirect, 0, 0, 1 , 1);
    }

    // Our camera should be ready: Assign final handle.
    *capturehandle = slotid;

    // Increase counter of open capture devices:
    numCaptureRecords++;
    
    // Set zero framerate:
    capdev->fps = 0;
    
    // Set image size:
    capdev->width = 0;
    capdev->height = 0;
    
    // Requested output texture pixel depth in layers:
    capdev->reqpixeldepth = reqdepth;
    
    // Number of DMA ringbuffers to use in DMA capture mode:
    capdev->num_dmabuffers = (num_dmabuffers>0) ? num_dmabuffers : 8;
    
    // Use of low-performance non-DMA fallback path allowed in case of trouble with DMA engine?
    capdev->allow_nondma_fallback = allow_lowperf_fallback;

    // Reset framecounter:
    capdev->nrframes = 0;
    capdev->grabber_active = 0;
    
    fflush(NULL);

    // Initiate a power-up cycle in case the camera is in standby mode:
    if (dc1394_set_camera_power(capdev->camera, DC1394_ON)!=DC1394_SUCCESS) {
      printf("PTB-WARNING: Tried to power up camera %i, but powerup-cycle failed for some reason!\n", deviceIndex); fflush(NULL);
    }

    // Initiate a reset-cycle on the camera to bring it into a clean state to start with:
    if (dc1394_reset_camera(capdev->camera)!=DC1394_SUCCESS) {
      printf("PTB-WARNING: Tried to reset camera %i, but reset cycle failed for some reason!\n", deviceIndex); fflush(NULL);
    }

    return(TRUE);
}

/* Internal function: Find best matching non-Format 7 mode:
 */
int PsychVideoFindNonFormat7Mode(PsychVidcapRecordType* capdev, double capturerate)
{
  int maximgarea = 0;
  dc1394video_mode_t maximgmode, mode;
  int i, j, w, h;
  unsigned int mw, mh;
  float framerate;
  dc1394framerate_t dc1394_framerate;
  dc1394framerates_t supported_framerates;
  dc1394video_modes_t video_modes;
  dc1394color_coding_t color_code;
  int nonyuvbonus;
  float bpp;
  int framerate_matched = false;
  int roi_matched = false;
  int mode_found = false;

  // Query supported video modes for this camera:
  dc1394_video_get_supported_modes(capdev->camera,  &video_modes);
  w = (int) PsychGetWidthFromRect(capdev->roirect);
  h = (int) PsychGetHeightFromRect(capdev->roirect);
  maximgmode = DC1394_VIDEO_MODE_MIN;

  for (i = 0; i < video_modes.num; i++) {
    // Query properties of this mode and match them against our requirements:
    mode = video_modes.modes[i];
    
    // We first check non-format 7 types: Skip format-7 types...
    if (mode >= DC1394_VIDEO_MODE_FORMAT7_MIN) continue;
    
    // Pixeldepth supported? We reject anything except RAW8 or MONO8 for luminance formats
    // and RGB8, YUV444, YUV422, YUV411 for color formats.
    dc1394_get_color_coding_from_video_mode(capdev->camera, mode, &color_code);
    if (capdev->reqpixeldepth > 0) {
      // Specific pixelsize requested:
      if (capdev->reqpixeldepth < 3 && color_code!=DC1394_COLOR_CODING_RAW8 && color_code!=DC1394_COLOR_CODING_MONO8) continue;
      if (capdev->reqpixeldepth > 2 && color_code!=DC1394_COLOR_CODING_RGB8 && color_code!=DC1394_COLOR_CODING_YUV444 &&
	  color_code!=DC1394_COLOR_CODING_YUV422 && color_code!=DC1394_COLOR_CODING_YUV411) continue;
    }
    else {
      // No specific pixelsize req. check our minimum requirements:
      if (color_code!=DC1394_COLOR_CODING_RGB8 && color_code!=DC1394_COLOR_CODING_RAW8 && color_code!=DC1394_COLOR_CODING_MONO8
	  && color_code!=DC1394_COLOR_CODING_YUV444 && color_code!=DC1394_COLOR_CODING_YUV422 && color_code!=DC1394_COLOR_CODING_YUV411) continue;
    }
    
    // ROI specified?
    dc1394_get_image_size_from_video_mode(capdev->camera, mode, &mw, &mh);
    if (capdev->roirect[kPsychLeft]==0 && capdev->roirect[kPsychTop]==0 && w==1 && h==1) {
      // No. Just find biggest one:
      if (mw*mh < maximgarea) continue;
      maximgarea = mw * mh;
      maximgmode = mode;
      mode_found = true;
      roi_matched = true;
    }
    else {
      // Yes. Check for exact match, reject everything else:
      if (capdev->roirect[kPsychLeft]!=0 || capdev->roirect[kPsychTop]!=0 || w!=mw || h!=mh) continue;	
      roi_matched = true;
      
      // Ok, this is a valid mode wrt. reqpixeldepth and exact image size. Check for matching framerate:
      dc1394_video_get_supported_framerates(capdev->camera, mode, &supported_framerates);
      for (j = 0; j < supported_framerates.num; j++) {
	dc1394_framerate = supported_framerates.framerates[j];
	dc1394_framerate_as_float(dc1394_framerate, &framerate);
	if (framerate >= capturerate) break;
      }
      dc1394_framerate_as_float(dc1394_framerate, &framerate);

      // nonyuvbonus is true, if a color capture mode is requested and the given mode
      // allows for RGB8 transfer instead of a YUV format. We try to prefer non-YUV
      // modes in selection of final mode, because YUV modes need a computationally
      // expensive conversion YUVxxx --> RGB8, whereas RGB8 doesn't need that.
      nonyuvbonus = (capdev->reqpixeldepth == 0 || capdev->reqpixeldepth > 2) && (color_code == DC1394_COLOR_CODING_RGB8);

      // Compare whatever framerate we've got as closest match against current fastest one:
      if ((framerate > maximgarea) ||
	  (framerate == capturerate && nonyuvbonus) ||
	  (framerate == maximgarea  && nonyuvbonus)) {
	maximgarea = (int) framerate;
	maximgmode = mode;
	mode_found = true;
      }
    }
  }
  
  // Sanity check: Any valid mode found?
  if (!mode_found) {
    // None found!
    PsychErrorExitMsg(PsychError_user, "Couldn't find any capture mode settings for your camera which satisfy your minimum requirements! Aborted.");
  }
  
  // maximgmode contains the best matching non-format-7 mode for our specs:
  mode = maximgmode;
  capdev->dc_imageformat = mode;
  
  // Query final color format and therefore pixel-depth:
  dc1394_get_color_coding_from_video_mode(capdev->camera, mode, &color_code);
  
  // This is the pixeldepth delivered by the capture engine:
  capdev->pixeldepth = (color_code == DC1394_COLOR_CODING_MONO8 || color_code == DC1394_COLOR_CODING_RAW8) ? 8 : 24;
  
  // Match this against requested pixeldepth:
  if (capdev->reqpixeldepth == 0) {
    // No specific depth requested: Just use native depth of captured image:
    capdev->reqpixeldepth = capdev->pixeldepth;
  }
  else {
    // Specific depth requested: Match it against native format:
    switch (capdev->reqpixeldepth) {
    case 1:
      // Pure LUMINANCE8 requested:
    case 2:
      // LUMINANCE8+ALPHA8 requested: This is not yet supported.
      if (capdev->pixeldepth != 8*capdev->reqpixeldepth && PsychPrefStateGet_Verbosity()>1) {
	printf("PTB-WARNING: Wanted a depth of %i layers (%s) for captured images, but capture device delivers\n"
	       "PTB-WARNING: %i layers! Adapted to capture device native format for performance reasons.\n",
	       capdev->reqpixeldepth, (capdev->reqpixeldepth==1) ? "LUMINANCE - 8 bpc":"LUMINANCE+ALPHA - 8 bpc", capdev->pixeldepth/8);
      }
      capdev->reqpixeldepth = capdev->pixeldepth;
      break;
    case 3:
      // RGB8 requested:
    case 4:
      // RGBA8 requested: This is not yet supported.
      if (capdev->pixeldepth != 8*capdev->reqpixeldepth && PsychPrefStateGet_Verbosity()>1) {
	printf("PTB-WARNING: Wanted a depth of %i layers (%s) for captured images, but capture device delivers\n"
	       "PTB-WARNING: %i layers! Adapted to capture device native format for performance reasons.\n",
	       capdev->reqpixeldepth, (capdev->reqpixeldepth==3) ? "RGB - 8 bpc":"RGB+ALPHA - 8 bpc", capdev->pixeldepth/8);
      }
      capdev->reqpixeldepth = capdev->pixeldepth;
      break;
    default:
      capdev->reqpixeldepth = 0;
      PsychErrorExitMsg(PsychError_user, "You requested a invalid capture image format (more than 4 layers). Aborted.");
    }
  }
  
  if (capdev->reqpixeldepth > 8 && color_code != DC1394_COLOR_CODING_RGB8 && PsychPrefStateGet_Verbosity()>1) {
    // Color capture with a non RGB8 mode aka a YUV mode -- expensive.
    printf("PTB-INFO: Using a YUV color format instead of a RGB color format. This requires expensive YUV->RGB conversion and\n");
    printf("PTB-INFO: can lead to higher cpu load and longer latencies. You may be able to avoid this with different settings\n");
    printf("PTB-INFO: for ROI, color depth and framerate...\n"); fflush(NULL);
  }

  // Query final image size and therefore ROI:
  dc1394_get_image_size_from_video_mode(capdev->camera, mode, &mw, &mh);
  capdev->roirect[kPsychLeft] = 0;
  capdev->roirect[kPsychTop] = 0;
  capdev->roirect[kPsychRight] = mw;
  capdev->roirect[kPsychBottom] = mh;
  
  // Recheck capture framerate:
  // We probe all available non mode-7 framerates of camera for the best match, aka
  // the slowest framerate equal or faster to the requested framerate:
  dc1394_video_get_supported_framerates(capdev->camera, mode, &supported_framerates);
  for (i = 0; i < supported_framerates.num; i++) {
    dc1394_framerate = supported_framerates.framerates[i];
    dc1394_framerate_as_float(dc1394_framerate, &framerate);
    if (framerate >= capturerate) break;
  }
  dc1394_framerate_as_float(dc1394_framerate, &framerate);
  
  // Ok, we've got the closest match we could get. Good enough?
  if (fabs(framerate - capturerate) < 0.5) {
    // Perfect match of delivered and requested framerate. Nothing to do so far...
    framerate_matched=true;
  }
  else {
    // No perfect match :(.
    framerate_matched=false;
    if(framerate < capturerate) {
      printf("PTB-WARNING: Camera does not support requested capture framerate of %f fps. Using maximum of %f fps instead.\n",
	     (float) capturerate, framerate);
      fflush(NULL);
    }
  }

  // Return framerate:
  capdev->dc_framerate = dc1394_framerate;

  if(PsychPrefStateGet_Verbosity()>1){
    printf("PTB-INFO: Will use non-Format7 mode %i: Width x Height = %i x %i, fps=%f, colormode=%i ...\n",
	   (int) mode, mw, mh, framerate, (int) color_code); fflush(NULL);
  }

  // Success! 
  return(true);
}

/* Internal function: Find best matching Format 7 mode:
 * Returns calculated optimal iso-packet size.
 */
int PsychVideoFindFormat7Mode(PsychVidcapRecordType* capdev, double capturerate)
{
  float mindiff = 1000000;
  float mindifframerate = 0;
  int minpacket_size = 0;
  dc1394video_mode_t minimgmode, mode;
  int i, j, w, h, numF7Available=0;
  dc1394speed_t speed;
  unsigned int mw, mh, pbmin, pbmax, depth;
  int num_packets, packet_size;
  float framerate;
  dc1394framerate_t dc1394_framerate;
  dc1394framerates_t supported_framerates;
  dc1394video_modes_t video_modes;
  dc1394color_coding_t color_code;
  float bpp;
  int framerate_matched = false;
  int roi_matched = false;
  float bus_period;

  // Query IEEE1394 bus speed code and map it to bus_period:
  if (dc1394_video_get_iso_speed(capdev->camera, &speed)!=DC1394_SUCCESS) {
    PsychErrorExitMsg(PsychError_user, "Unable to query bus-speed - Start of video capture failed!");
  }

  // Special hack for Unibrain Fire-i: This camera can do 400 Megabit/second, but reports
  // a speed of 100 MBit after a cold-start! We enforce a 400 Megabit speed if this is a
  // Unibrain Fire-i:
  if (strstr(capdev->camera->vendor, "Unibrain") && strstr(capdev->camera->model, "Fire-i")) {
    // Unibrain Fire-i: Enforce correct speed:
    speed = DC1394_ISO_SPEED_400;
  }

  switch(speed) {
    case DC1394_ISO_SPEED_100:
      bus_period = 0.000500f;
      break;
    case DC1394_ISO_SPEED_200:
      bus_period = 0.000250f;
      break;
    case DC1394_ISO_SPEED_400:
      bus_period = 0.000125f;
      break;
    case DC1394_ISO_SPEED_800:
      bus_period = 0.0000625f;
      break;
    case DC1394_ISO_SPEED_1600:
      bus_period = 0.00003125f;
      break;
    case DC1394_ISO_SPEED_3200:
      bus_period = 0.000015625f;
      break;
    default:
    PsychErrorExitMsg(PsychError_user, "Unknown bus speed specification! Start of video capture failed!");
  }

  if(PsychPrefStateGet_Verbosity()>1){ 
    printf("PTB-INFO: IEEE-1394 Firewire bus speed is %i Megabit/second --> Bus period is %f usecs.\n",
	   (int) (100 << speed), bus_period * 1000000.0f);
  }
  
  // Query supported video modes for this camera:
  dc1394_video_get_supported_modes(capdev->camera,  &video_modes);
  minimgmode = DC1394_VIDEO_MODE_MIN;

  for (i = 0; i < video_modes.num; i++) {
    // Query properties of this mode and match them against our requirements:
    mode = video_modes.modes[i];
    
    // Skip non-format-7 types...
    if (mode < DC1394_VIDEO_MODE_FORMAT7_MIN || mode > DC1394_VIDEO_MODE_FORMAT7_MAX) continue;

    // Increment count of available Format-7 modes:
    numF7Available++;

    if(PsychPrefStateGet_Verbosity()>1){
      printf("PTB-Info: Probing Format-7 mode %i ...\n", mode);
    }

    // Pixeldepth supported? We reject anything except RAW8 or MONO8 for luminance formats
    // and RGB8 for color formats.
    dc1394_format7_get_color_coding(capdev->camera, mode, &color_code);
    if (capdev->reqpixeldepth > 0) {
      // Specific pixelsize requested:
      if (capdev->reqpixeldepth < 3 && color_code!=DC1394_COLOR_CODING_RAW8 && color_code!=DC1394_COLOR_CODING_MONO8) continue;
      if (capdev->reqpixeldepth > 2 && color_code!=DC1394_COLOR_CODING_RGB8) continue;
    }
    else {
      // No specific pixelsize req. check our minimum requirements:
      if (color_code!=DC1394_COLOR_CODING_RGB8 && color_code!=DC1394_COLOR_CODING_RAW8 && color_code!=DC1394_COLOR_CODING_MONO8) continue;
    }
    
    // ROI specified?
    w = (int) PsychGetWidthFromRect(capdev->roirect);
    h = (int) PsychGetHeightFromRect(capdev->roirect);

    if (capdev->roirect[kPsychLeft]==0 && capdev->roirect[kPsychTop]==0 && w==1 && h==1) {
      // No. Just set biggest one for this mode:

      // Query maximum size for mode:
      if(dc1394_format7_get_max_image_size(capdev->camera, mode, &mw, &mh)!=DC1394_SUCCESS) continue;
      // Set zero position offset:
      if (dc1394_format7_set_image_position(capdev->camera, mode, 0, 0)!=DC1394_SUCCESS) continue;
      // Set maximum size:
      if (dc1394_format7_set_image_size(capdev->camera, mode, mw, mh)!=DC1394_SUCCESS) continue;
      w=mw; h=mh;
      roi_matched = true;
    }
    else {
      // Yes. Check for exact match, reject everything else:
      if(dc1394_format7_get_max_image_size(capdev->camera, mode, &mw, &mh)!=DC1394_SUCCESS) continue;
      if (w > mw || h > mh) continue;

      // This mode allows for a ROI as big as the one we request. Try to set it up:

      // First set zero position offset:
      if (dc1394_format7_set_image_position(capdev->camera, mode, 0, 0)!=DC1394_SUCCESS) continue;
      
      // Reject mode if size isn't supported:
      if (dc1394_format7_set_image_size(capdev->camera, mode, (unsigned int) w, (unsigned int) h)!=DC1394_SUCCESS) continue;
      
      // Now set real position:
      if (dc1394_format7_set_image_position(capdev->camera, mode, (unsigned int) capdev->roirect[kPsychLeft], (unsigned int) capdev->roirect[kPsychTop])!=DC1394_SUCCESS) continue;

      // If we reach this point, then we should have exactly the ROI we wanted.
      roi_matched = true;
    }

    // Try to set the requested framerate as well:
    // We need to calculate the ISO packet size depending on wanted framerate, Firewire bus speed,
    // image size and image depth + some IIDC spec. restrictions...

    // First we query the range of available packet sizes:
    if (dc1394_format7_get_packet_para(capdev->camera, mode, &pbmin, &pbmax)!=DC1394_SUCCESS) continue;
    // Special case handling:
    if (pbmin==0) pbmin = pbmax;
    
    // Compute number of ISO-Packets, assuming a 400 MBit bus (125 usec cycle time):
    num_packets = (int) (1.0/(bus_period * capturerate) + 0.5);
    if (num_packets < 1 || num_packets > 4095) {
      // Invalid num_packets. Adapt it to fit IIDC constraints:
      if (num_packets < 1) {
	num_packets = 1;
      }
      else {
	num_packets = 4095;
      }
    }
    num_packets*=8;
    if (dc1394_format7_get_data_depth(capdev->camera, mode, &depth)!=DC1394_SUCCESS) continue;

    packet_size = (int)((w * h * depth + num_packets - 1) /  num_packets);
    
    // Make sure that packet_size is an integral multiple of pbmin (IIDC constraint):
    if (packet_size < pbmin) packet_size = pbmin;
    if (packet_size % pbmin != 0) {
      packet_size = packet_size - (packet_size % pbmin);
    }
    
    // Make sure that packet size is smaller than pbmax:
    while (packet_size > pbmax) packet_size=packet_size - pbmin;
    
    // Ok, we should now have the closest valid packet size for the given ROI and framerate:
    // Inverse compute framerate for this packetsize:
    num_packets = (int) ((w * h * depth + (packet_size*8) - 1)/(packet_size*8));
    framerate = 1.0/(bus_period * (float) num_packets);
      
    // Compare whatever framerate we've got as closest match against current fastest one:
    if (fabs(capturerate - framerate) < mindiff) {
      mindiff = fabs(capturerate - framerate);
      mindifframerate = framerate;
      minimgmode = mode;
      minpacket_size = packet_size;
    }

    if(PsychPrefStateGet_Verbosity()>1){
      if (capdev->roirect[kPsychLeft]!=0 || capdev->roirect[kPsychTop]!=0 || capdev->roirect[kPsychRight]!=1 || capdev->roirect[kPsychBottom]!=1) {
	printf("PTB-INFO: Checking Format-7 mode %i: ROI = [l=%f t=%f r=%f b=%f] , FPS = %f\n", mode, (float) capdev->roirect[kPsychLeft], (float) capdev->roirect[kPsychTop],
	       (float) capdev->roirect[kPsychRight], (float) capdev->roirect[kPsychBottom], framerate);
      }
      else {
	printf("PTB-INFO: Checking Format-7 mode %i: ROI = [l=0 t=0 r=%i b=%i] , FPS = %f\n", mode, w, h, framerate);
      }
    }

    // Test next mode...
  }
  
  // Sanity check: Any valid mode found?
  if (minimgmode == DC1394_VIDEO_MODE_MIN || numF7Available == 0) {
    // None found!
    if (numF7Available > 0) {
      printf("PTB-INFO: Couldn't find any Format-7 capture mode settings for your camera which satisfy your minimum requirements!\n");
    }
    else {
      printf("PTB-INFO: This camera does not support *any* Format-7 capture modes.\n");
    }
    printf("PTB-INFO: Will now try standard (non Format-7) capture modes for the best match and try to use that...\n");
    return(0);
  }

  // Success (more or less...):
  
  // minimgmode contains the best matching Format-7 mode for our specs:
  mode = minimgmode;
  capdev->dc_imageformat = mode;
  capdev->dc_framerate = DC1394_FRAMERATE_MIN;
  packet_size = minpacket_size;
  framerate = mindifframerate;

  // Query final color format and therefore pixel-depth:
  dc1394_get_color_coding_from_video_mode(capdev->camera, mode, &color_code);
  
  // This is the pixeldepth delivered by the capture engine:
  capdev->pixeldepth = (color_code == DC1394_COLOR_CODING_RGB8) ? 24 : 8;
  
  // Match this against requested pixeldepth:
  if (capdev->reqpixeldepth == 0) {
    // No specific depth requested: Just use native depth of captured image:
    capdev->reqpixeldepth = capdev->pixeldepth;
  }
  else {
    // Specific depth requested: Match it against native format:
    switch (capdev->reqpixeldepth) {
    case 1:
      // Pure LUMINANCE8 requested:
    case 2:
      // LUMINANCE8+ALPHA8 requested: This is not yet supported.
      if (capdev->pixeldepth != 8*capdev->reqpixeldepth && PsychPrefStateGet_Verbosity()>1) {
	printf("PTB-WARNING: Wanted a depth of %i layers (%s) for captured images, but capture device delivers\n"
	       "PTB-WARNING: %i layers! Adapted to capture device native format for performance reasons.\n",
	       capdev->reqpixeldepth, (capdev->reqpixeldepth==1) ? "LUMINANCE - 8 bpc":"LUMINANCE+ALPHA - 8 bpc", capdev->pixeldepth/8);
      }
      capdev->reqpixeldepth = capdev->pixeldepth;
      break;
    case 3:
      // RGB8 requested:
    case 4:
      // RGBA8 requested: This is not yet supported.
      if (capdev->pixeldepth != 8*capdev->reqpixeldepth && PsychPrefStateGet_Verbosity()>1) {
	printf("PTB-WARNING: Wanted a depth of %i layers (%s) for captured images, but capture device delivers\n"
	       "PTB-WARNING: %i layers! Adapted to capture device native format for performance reasons.\n",
	       capdev->reqpixeldepth, (capdev->reqpixeldepth==3) ? "RGB - 8 bpc":"RGB+ALPHA - 8 bpc", capdev->pixeldepth/8);
      }
      capdev->reqpixeldepth = capdev->pixeldepth;
      break;
    default:
      capdev->reqpixeldepth = 0;
      PsychErrorExitMsg(PsychError_user, "You requested a invalid capture image format (more than 4 layers). Aborted.");
    }
  }
  
  // Query final image size and therefore ROI:
  dc1394_get_image_size_from_video_mode(capdev->camera, mode, &mw, &mh);
  capdev->roirect[kPsychRight]  = capdev->roirect[kPsychLeft] + mw;
  capdev->roirect[kPsychBottom] = capdev->roirect[kPsychTop]  + mh;
  
  // Ok, we've got the closest match we could get. Good enough?
  if (mindiff < 0.5) {
    // Perfect match of delivered and requested framerate. Nothing to do so far...
    framerate_matched=true;
  }
  else {
    // No perfect match :(.
    framerate_matched=false;
    if(framerate < capturerate) {
      printf("PTB-WARNING: Camera does not support requested capture framerate of %f fps at given ROI setting. Using %f fps instead.\n",
	     (float) capturerate, framerate);
      fflush(NULL);
    }
  }

  // Assign computed framerate as best guess for real framerate, in case frame-interval query fails...
  capdev->fps = framerate;

  // Return packet_size:
  return(packet_size);
}


/*
 *  PsychVideoCaptureRate() - Start- and stop video capture.
 *
 *  capturehandle = Grabber to start-/stop.
 *  playbackrate = zero == Stop capture, non-zero == Capture
 *  dropframes = 0 - Always deliver oldest frame in DMA ringbuffer. 1 - Always deliver newest frame.
 *               --> 1 == drop frames in ringbuffer if behind -- low-latency capture.
 *  startattime = Deadline (in system time) for which to wait before real start of capture.
 *  Returns Number of dropped frames during capture.
 */
int PsychVideoCaptureRate(int capturehandle, double capturerate, int dropframes, double* startattime)
{
  int dropped = 0;
  float framerate = 0;
  dc1394speed_t speed;
  dc1394video_mode_t maximgmode, mode;
  int i, j, w, h, packetsize;
  unsigned int mw, mh;
  dc1394framerate_t dc1394_framerate;
  dc1394framerates_t supported_framerates;
  dc1394video_modes_t video_modes;
  dc1394color_coding_t color_code;
  float bpp;
  int framerate_matched = false;
  int roi_matched = false;
  dc1394error_t err;

  // Retrieve device record for handle:
  PsychVidcapRecordType* capdev = PsychGetVidcapRecord(capturehandle);

  // Start- or stop capture?
  if (capturerate > 0) {
    // Start capture:
    if (capdev->grabber_active) PsychErrorExitMsg(PsychError_user, "You tried to start video capture, but capture is already started!");

    // Reset statistics:
    capdev->last_pts = -1.0;
    capdev->nr_droppedframes = 0;
    capdev->frame_ready = 0;
    
    // Select best matching mode for requested image size and pixel format:
    // ====================================================================

    w = (int) PsychGetWidthFromRect(capdev->roirect);
    h = (int) PsychGetHeightFromRect(capdev->roirect);

    // Can we (potentially) get along with a non-Format-7 mode?
    // Check minimum requirements for non-Format-7 mode:
    if (!((capdev->roirect[kPsychLeft]==0 && capdev->roirect[kPsychTop]==0) &&
	((capdev->roirect[kPsychRight]==1 && capdev->roirect[kPsychBottom]==1) || (w==640 && h==480) ||
	 (w==800 && h==600) || (w==1024 && h==768) || (w==1280 && h==960) || (w==1600 && h==1200) ||
	 (w==320 && h==240) || (w==160 && h==120)) &&
	(capturerate==1.875 || capturerate==3.75 || capturerate==7.5 || capturerate==15 || capturerate==30 ||
	 capturerate==60 || capturerate==120 || capturerate==240))) {

      // Ok, the requested ROI and/or framerate is not directly supported by non-Format7 capture modes.
      // Try to find a good format-7 mode, fall back to NF-7 if format 7 doesn't work out:
      if ((packetsize=PsychVideoFindFormat7Mode(capdev, capturerate))==0) {
	// Could not find good Format-7 mode! Try NF-7: This function will exit if we don't find
	// a useful match at all:
	PsychVideoFindNonFormat7Mode(capdev, capturerate);
      }
      // Ok either we have a format-7 mode ready to go (packetsize>0) or we have a default
      // non-format-7 mode ready (packetsize==0)...
    }
    else {
      // The requested combo of ROI and framerate should be supported by standard non-format-7 capture:
      // Try it and exit in case of non-match:
      PsychVideoFindNonFormat7Mode(capdev, capturerate);
      packetsize = 0;
    }

    // Setup capture hardware and DMA engine:
    if (dc1394_video_get_iso_speed(capdev->camera, &speed)!=DC1394_SUCCESS) {
      PsychErrorExitMsg(PsychError_user, "Unable to query bus-speed - Start of video capture failed!");
    }

    // Special hack for Unibrain Fire-i: This camera can do 400 Megabit/second, but reports
    // a speed of 100 MBit after a cold-start! We enforce a 400 Megabit speed if this is a
    // Unibrain Fire-i:
    if (strstr(capdev->camera->vendor, "Unibrain") && strstr(capdev->camera->model, "Fire-i")) {
      // Unibrain Fire-i: Enforce correct speed:
      speed = DC1394_ISO_SPEED_400;
    }

    // Assign final mode and framerate:
    dc1394_framerate = capdev->dc_framerate;
    mode = capdev->dc_imageformat;

    // Query final color format of captured data:
    dc1394_get_color_coding_from_video_mode(capdev->camera, mode, &color_code);
    capdev->colormode = color_code;

    // Setup DMA engine:
    // =================

    // Format-7 capture?
    if (packetsize > 0) {
      // Format-7 capture DMA setup:
      err = dc1394_dma_setup_format7_capture(capdev->camera, mode, color_code, speed, packetsize,
					     (unsigned int) capdev->roirect[kPsychLeft],
					     (unsigned int) capdev->roirect[kPsychTop],
					     (unsigned int) PsychGetWidthFromRect(capdev->roirect),
					     (unsigned int) PsychGetHeightFromRect(capdev->roirect),
					     capdev->num_dmabuffers, dropframes);
    }
    else {
      // Non-Format-7 capture DMA setup:
      err = dc1394_dma_setup_capture(capdev->camera, mode, speed, dc1394_framerate, capdev->num_dmabuffers, dropframes);
    }

    if (err != DC1394_SUCCESS) {      
      // Failed! We clean up and either fail or retry in non-DMA mode:

      // Shutdown DMA engine:
      dc1394_dma_unlisten(capdev->camera);

      // Release DMA engine:
      dc1394_dma_release_camera(capdev->camera);

      // We release the cams iso channel and bandwidth allocation in the hope that this
      // will get us ready again...
      dc1394_free_iso_channel_and_bandwidth(capdev->camera);

      // Are we allowed to use non-DMA capture if DMA capture doesn't work?
      if (capdev->allow_nondma_fallback) {
	// Yes. Try to activate non-DMA capture. Less efficient...
	printf("PTB-WARNING: Could not setup DMA capture engine! Trying non-DMA capture engine as a slow, inefficient and limited fallback.\n");
	if (dc1394_setup_capture(capdev->camera, mode, speed, dc1394_framerate) != DC1394_SUCCESS) {
	  // Non-DMA setup failed as well. That's the end my friend...

	  // We release the cams iso channel and bandwidth allocation in the hope that this
	  // will get us ready again...
	  dc1394_free_iso_channel_and_bandwidth(capdev->camera);
	  PsychErrorExitMsg(PsychError_user, "Unable to setup and start non-DMA capture engine as well - Start of video capture failed!");
	}
	else {
	  // Signal use of non-DMA mode:
	  capdev->dma_mode = 0;
	}
      }
      else {
	// Nope! Exit with error-message:
	PsychErrorExitMsg(PsychError_user, "Unable to setup and start DMA capture engine and not allowed to use non-DMA fallback path - Start of video capture failed!");
      }
    }
    else {
      // Signal use of DMA engine:
      capdev->dma_mode = 1;
    }

    // Ready to go! Now we just need to tell the camera to start its capture cycle:

    // Wait until start deadline reached:
    PsychWaitUntilSeconds(*startattime);

    // Start DMA driven isochronous data transfer:
    if (dc1394_video_set_transmission(capdev->camera, DC1394_ON) !=DC1394_SUCCESS) {
      if (capdev->dma_mode > 0) {
	// Shutdown DMA engine:
	dc1394_dma_unlisten(capdev->camera);
	// Release DMA engine:
	dc1394_dma_release_camera(capdev->camera);
      }
      else {
	dc1394_release_camera(capdev->camera);
      }

      // We release the cams iso channel and bandwidth allocation in the hope that this
      // will get us ready again...
      dc1394_free_iso_channel_and_bandwidth(capdev->camera);
      
      PsychErrorExitMsg(PsychError_user, "Unable to start isochronous data transfer from camera - Start of video capture failed!");
    }
    
    // Record real start time:
    PsychGetAdjustedPrecisionTimerSeconds(startattime);

    // Map framerate enum to floating point value and assign it:
    if (packetsize == 0) {
      dc1394_framerate_as_float(dc1394_framerate, &framerate);
    }
    else {
      dc1394_format7_get_frame_interval(capdev->camera, mode, &framerate);
      if (framerate == 0) framerate = capdev->fps;
    }

    capdev->fps = (double) framerate;

    // Setup size and position:
    capdev->width  = (int) PsychGetWidthFromRect(capdev->roirect);
    capdev->height = (int) PsychGetHeightFromRect(capdev->roirect);

    // Ok, capture is now started:
    capdev->grabber_active = 1;
    
    // Allocate conversion buffer if needed for YUV->RGB conversions.
    if (capdev->pixeldepth == 24 && color_code!=DC1394_COLOR_CODING_RGB8) {
      // Software conversion of YUV -> RGB needed. Allocate a proper scratch-buffer:
      capdev->scratchbuffer = malloc(capdev->width * capdev->height * 3);
    }

    if(PsychPrefStateGet_Verbosity()>1){
      printf("PTB-INFO: Capture started on device %i - Width x Height = %i x %i - Framerate: %f fps.\n", capturehandle,
	     capdev->width, capdev->height, capdev->fps);
    }
  }
  else {
    // Stop capture:
    if (capdev->grabber_active) {
      // Stop isochronous data transfer from camera:
      if (dc1394_video_set_transmission(capdev->camera, DC1394_OFF) !=DC1394_SUCCESS) {
	PsychErrorExitMsg(PsychError_user, "Unable to stop video transfer on camera! (dc1394_video_set_transmission(DC_OFF) failed)!");
      }

      if (capdev->dma_mode > 0) {
	// Shutdown DMA engine:
	dc1394_dma_unlisten(capdev->camera);
	
	// Release DMA engine:
	dc1394_dma_release_camera(capdev->camera);
      }
      else {
	dc1394_release_camera(capdev->camera);
      }

      // Ok, capture is now stopped.
      capdev->frame_ready = 0;
      capdev->grabber_active = 0;
    
      if (capdev->scratchbuffer) {
	// Release scratch-buffer:
	free(capdev->scratchbuffer);
	capdev->scratchbuffer = NULL;
      }

      // MK: FIXME: Need to undo reqpixeldepth = reqpixeldepth * 8 in start capture!
      // Need to rethink definition of reqpixeldepth...
      capdev->reqpixeldepth = capdev->reqpixeldepth / 8;

      if(PsychPrefStateGet_Verbosity()>1){
	// Output count of dropped frames:
	if ((dropped=capdev->nr_droppedframes) > 0) {
	  printf("PTB-INFO: Video capture dropped %i frames on device %i to keep capture running in sync with realtime.\n", dropped, capturehandle); 
	}
	
	if (capdev->nrframes>0) capdev->avg_decompresstime/= (double) capdev->nrframes;
	printf("PTB-INFO: Average time spent in video decompressor (waiting/polling for new frames) was %lf milliseconds.\n", capdev->avg_decompresstime * 1000.0f);
	if (capdev->nrgfxframes>0) capdev->avg_gfxtime/= (double) capdev->nrgfxframes;
	printf("PTB-INFO: Average time spent in GetCapturedImage (intensity calculation Video->OpenGL texture conversion) was %lf milliseconds.\n", capdev->avg_gfxtime * 1000.0f);
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


/*
 *  PsychGetTextureFromCapture() -- Create an OpenGL texturemap from a specific videoframe from given capture object.
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
 *  Returns Number of pending or dropped frames after fetch on success (>=0), -1 if no new image available yet, -2 if no new image available and there won't be any in future.
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
    dc1394error_t error;
    int nrdropped;
    unsigned char* input_image = NULL;

    int waitforframe = (checkForImage > 1) ? 1:0; // Blocking wait for new image requested?

    // Retrieve device record for handle:
    PsychVidcapRecordType* capdev = PsychGetVidcapRecord(capturehandle);

    // Take start timestamp for timing stats:
    PsychGetAdjustedPrecisionTimerSeconds(&tstart);
        
    // Should we just check for new image?
    if (checkForImage) {
      if (capdev->grabber_active == 0) {
	// Grabber stopped. We'll never get a new image:
	return(-2);
      }

      // Grabber active: Polling mode or wait for new frame mode?
      if (waitforframe) {
	// Check for image in blocking mode: We actually try to capture a frame in
	// blocking mode, so we will wait here until a new frame arrives.
	if (capdev->dma_mode > 0) {
	  // DMA wait & capture:
	  error = dc1394_dma_capture(&(capdev->camera), 1, DC1394_VIDEO1394_WAIT);
	}
	else {
	  // Non-DMA wait & capture:
	  error = dc1394_capture(&(capdev->camera), 1);
	}

	if (error == DC1394_SUCCESS) {	  
	  // Ok, new frame ready and dequeued from DMA ringbuffer. We'll return it on next non-poll invocation.
	  capdev->frame_ready = 1;
	}
	else {
	  // Blocking wait failed! Somethings seriously wrong:
	  PsychErrorExitMsg(PsychError_internal, "Blocking wait for new frame failed!!!");
	}
      }
      else {
	// Check for image in polling mode: We capture in non-blocking mode:	
	if (capdev->dma_mode <= 0) {
	  // Oops. Tried to use polling mode in non-DMA capture. This is not supported.
	  printf("PTB-ERROR: Tried to call Screen('GetCapturedImage') in polling mode during non-DMA capture\n");
	  printf("PTB-ERROR: This is not supported. Will return error code -2...\n");
	  fflush(NULL);
	  return(-2);
	}

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

	if (capdev->dma_mode > 0) {
	  // Query capture timestamp from Firewire subsystem and convert to seconds:
	  capdev->current_pts = ((double) capdev->camera->capture.filltime.tv_sec) + (((double) capdev->camera->capture.filltime.tv_usec) / 1000000.0f);
	}
	else {
	  // We do not get a timestamp from firewire subsystem in non-DMA mode: Use the best we have, which should still be pretty good:
	  capdev->current_pts = tend;
	}
      }

      // Return availability status: 0 = new frame ready for retrieval. -1 = No new frame ready yet.
      return((capdev->frame_ready) ? 0 : -1);
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
    
    padding= 0;

    // input_image points to the image buffer in our cam:
    input_image = (unsigned char*) (capdev->camera->capture.capture_buffer);

    // Do we want to do something with the image data and have a
    // scratch buffer for color conversion alloc'ed?
    if ((capdev->scratchbuffer) && ((out_texture) || (summed_intensity))) {
      // Yes. Perform color-conversion YUV->RGB from cameras DMA buffer
      // into the scratch buffer and set scratch buffer as source for
      // all further operations:

      dc1394_convert_to_RGB8(input_image, capdev->scratchbuffer, capdev->width,
			     capdev->height, DC1394_BYTE_ORDER_UYVY, capdev->colormode, 8);

      // Ok, at this point we should have a RGB8 texture image ready in scratch_buffer.
      // Set scratch buffer as our new image source for all further processing:
      input_image = (unsigned char*) capdev->scratchbuffer;
    }

    // Only setup if really a texture is requested (non-benchmarking mode):
    if (out_texture) {
      PsychMakeRect(out_texture->rect, 0, 0, w+padding, h);    
      
      // Set NULL - special texture object as part of the PTB texture record:
      out_texture->targetSpecific.QuickTimeGLTexture = NULL;
      
      // Set texture orientation as if it were an inverted Offscreen window: Upside-down.
      out_texture->textureOrientation = 3;
      
      // Setup a pointer to our GWorld as texture data pointer: Setting memsize to zero
      // prevents unwanted free() operation in PsychDeleteTexture...
      out_texture->textureMemorySizeBytes = 0;

      // Set texture depth: Could be 8, 16, 24 or 32 bpp.
      out_texture->depth = capdev->reqpixeldepth;
    
      // This will retrieve an OpenGL compatible pointer to the pixel data and assign it to our texmemptr:
      out_texture->textureMemory = (GLuint*) input_image;

      // Let PsychCreateTexture() do the rest of the job of creating, setting up and
      // filling an OpenGL texture with content:
      PsychCreateTexture(out_texture);
      
      // Undo hack from above after texture creation: Now we need the real width of the
      // texture for proper texture coordinate assignments in drawing code et al.
      PsychMakeRect(out_texture->rect, 0, 0, w, h);    
      // Ready to use the texture...
    }
    
    // Sum of pixel intensities requested?
    if(summed_intensity) {
      pixptr = (unsigned char*) input_image;
      count = (w*h*((capdev->pixeldepth == 24) ? 3 : 1));
      for (i=0; i<count; i++) intensity+=(unsigned int) pixptr[i];
      *summed_intensity = ((double) intensity) / w / h / ((capdev->pixeldepth == 24) ? 3 : 1);
    }

    // Release the capture buffer. Return it to the DMA ringbuffer pool:
    if (capdev->dma_mode > 0) dc1394_dma_done_with_buffer(capdev->camera);

    // Detection of dropped frames: This is a heuristic. We'll see how well it works out...
    if (capdev->dma_mode < 1) {
      // Old style: Heuristic based on comparison of capture timestamps:
      // Expected delta between successive presentation timestamps:
      targetdelta = 1.0f / capdev->fps;
    
      // Compute real delta:
      realdelta = capdev->current_pts - capdev->last_pts;
      if (realdelta<0) realdelta = 0;
      frames = realdelta / targetdelta;
      
      // Dropped frames?
      if (frames > 1 && capdev->last_pts>=0) {
	nrdropped = (int) (frames - 1 + 0.5);
      }
      else {
	nrdropped = 0;
      }

      // Record timestamp as reference for next check:    
      capdev->last_pts = capdev->current_pts;
    }
    else {
      // New style - Only works with DMA capture engine. Just take values from Firewire subsystem:
      nrdropped = (int) capdev->camera->capture.num_dma_buffers_behind;
    }
    
    // Update total count of dropped (or pending) frames:
    capdev->nr_droppedframes += nrdropped;

    // Timestamping:
    PsychGetAdjustedPrecisionTimerSeconds(&tend);

    // Increase counter of retrieved textures:
    capdev->nrgfxframes++;

    // Update average time spent in texture conversion:
    capdev->avg_gfxtime+=(tend - tstart);
    
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
double PsychVideoCaptureSetParameter(int capturehandle, const char* pname, double value)
{
  dc1394featureset_t features;
  dc1394feature_t feature;
  dc1394bool_t present;
  unsigned int minval, maxval, intval, oldintval;
  unsigned int lowadr, hiadr;
  octlet_t triggercounteraddress;
  int triggercount;

  double oldvalue = DBL_MAX; // Initialize return value to the "unknown/unsupported" default.
  boolean assigned = false;

  // Retrieve device record for handle:
  PsychVidcapRecordType* capdev = PsychGetVidcapRecord(capturehandle);

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
    if (strstr(capdev->camera->vendor, "Basler")==NULL) {
      // Non Basler cam :( We have to give up for now...
      return(-1);
    }

    // It is a Basler cam. Try to get a handle to the proper register:
    //capdev->camera->advanced_features_csr = 0xf2f00000;
    hiadr=lowadr=0xDEADBEEF;
    if(GetCameraAdvControlRegister(capdev->camera, 0x00, &lowadr)!=DC1394_SUCCESS) return(-11);
    if(GetCameraAdvControlRegister(capdev->camera, 0x04, &hiadr)!=DC1394_SUCCESS) return(-12);
    printf("Hi = %x Lo = %x  == ", hiadr, lowadr);

    
    // Step 0: Unlock Advanced features by writing Baslers unlock - key into access control register:
    if(SetCameraAdvControlRegister(capdev->camera, 0x00, 0x0030533B)!=DC1394_SUCCESS) return(-2);
    if(SetCameraAdvControlRegister(capdev->camera, 0x04, 0x73C3F000)!=DC1394_SUCCESS) return(-3);
    if(GetCameraAdvControlRegister(capdev->camera, 0x00, &lowadr)!=DC1394_SUCCESS) return(-31);
    if(GetCameraAdvControlRegister(capdev->camera, 0x04, &hiadr)!=DC1394_SUCCESS) return(-32);
    if(lowadr == 0xFFFFFFFF && hiadr == 0xFFFFFFFF) {
      // Failed! Advanced Features not supported on this Basler camera:
      return(-33);
    }
    printf("TIMEOUT %i\n", (int) (hiadr & 0xFFFF));

    // Step 1: Write GUID of Trigger counter register to "Smart Features Inquiry Register"
    if(SetCameraAdvControlRegister(capdev->camera, 0x10, 0x16C31A78)!=DC1394_SUCCESS) return(-4);
    if(SetCameraAdvControlRegister(capdev->camera, 0x14, 0x11D83F75)!=DC1394_SUCCESS) return(-5);
    if(SetCameraAdvControlRegister(capdev->camera, 0x18, 0x1000EC94)!=DC1394_SUCCESS) return(-6);
    if(SetCameraAdvControlRegister(capdev->camera, 0x1C, 0x55AE5B5A)!=DC1394_SUCCESS) return(-7);
    // Step 2: Query address of Trigger counter reg. from "Smart Features Address Register"
    if(GetCameraAdvControlRegister(capdev->camera, 0x20, &lowadr)!=DC1394_SUCCESS) return(-8);
    if(GetCameraAdvControlRegister(capdev->camera, 0x24, &hiadr)!=DC1394_SUCCESS) return(-9);
    if(lowadr == 0 && hiadr == 0) {
      // Failed! Feature not supported, just return -1:
      return(-10);
    }
    printf("Hi = %i Lo = %i  == ", hiadr, lowadr);

    // Step 3: Assemble final address of trigger counter reg.
    triggercounteraddress = (octlet_t) (((octlet_t) hiadr << 32) | (octlet_t) lowadr);

    // Step 4: Read counter:
    lowadr = 0xDEADBEEF;
    if(GetCameraROMValue(capdev->camera, triggercounteraddress, &lowadr)!=DC1394_SUCCESS) return(-11);
    triggercount = (int)(lowadr >> 16);
    printf("%p\n", (void*) lowadr);
    return((int) lowadr);
  }

  if (strcmp(pname, "PrintParameters")==0) {
    // Special command: List and print all features...
    if (dc1394_get_camera_info(capdev->camera) !=DC1394_SUCCESS) {
      printf("PTB-WARNING: Unable to query general information about camera.\n");
    }
    else {
      printf("PTB-INFO: The camera provides the following generic information:\n");
      dc1394_print_camera_info(capdev->camera);
    }

    if (dc1394_get_camera_feature_set(capdev->camera, &features) !=DC1394_SUCCESS) {
      printf("PTB-WARNING: Unable to query feature set of camera.\n");
    }
    else {
      printf("PTB-INFO: The camera provides the following feature set:\n");
      dc1394_print_feature_set(&features);
    }

    fflush(NULL);    
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
    PsychCopyOutCharArg(1, FALSE, capdev->camera->vendor);
    return(0);
  }

  // Return model name string:
  if (strcmp(pname, "GetModelname")==0) {
    PsychCopyOutCharArg(1, FALSE, capdev->camera->model);
    return(0);
  }

  if (strstr(pname, "Brightness")!=0) {
    assigned = true;
    feature = DC1394_FEATURE_BRIGHTNESS;    
  }

  if (strstr(pname, "Gain")!=0) {
    assigned = true;
    feature = DC1394_FEATURE_GAIN;    
  }

  if (strstr(pname, "Exposure")!=0) {
    assigned = true;
    feature = DC1394_FEATURE_EXPOSURE;    
  }

  if (strstr(pname, "Shutter")!=0) {
    assigned = true;
    feature = DC1394_FEATURE_SHUTTER;    
  }

  if (strstr(pname, "Sharpness")!=0) {
    assigned = true;
    feature = DC1394_FEATURE_SHARPNESS;    
  }

  if (strstr(pname, "Saturation")!=0) {
    assigned = true;
    feature = DC1394_FEATURE_SATURATION;    
  }

  if (strstr(pname, "Gamma")!=0) {
    assigned = true;
    feature = DC1394_FEATURE_GAMMA;    
  }

  // Check if feature is present on this camera:
  if (dc1394_feature_is_present(capdev->camera, feature, &present)!=DC1394_SUCCESS) {
    if(PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Failed to query presence of feature %s on camera %i! Ignored.\n", pname, capturehandle);
    fflush(NULL);
  }
  else if (present) {
    // Feature is available:

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

