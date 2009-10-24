/*
	PsychSourceGL/Source/Common/Screen/PsychVideoCaptureSupportLibARVideo.c
 
	PLATFORMS:	
	
	GNU/Linux, Apple MacOS/X and MS-Windows
 
	AUTHORS:
	
	Mario Kleiner           mk              mario.kleiner@tuebingen.mpg.de
 
	HISTORY:
	
	18.04.2009				Created initial version.
	
	DESCRIPTION:
	
	 This is the videocapture engine based on the free, open-source (GPL'ed)
	 LibARVideo library, part of the ARToolkit. It only supports video capture,
	 no sound capture and no recording of video or sound.
	 
	 On MacOS/X it is implemented via the Quicktime Sequencegrabber-API, ie., it
	 uses the same engine as our original Quicktime video capture engine. Therefore
	 it doesn't provide any additional functionality or advantages over our default
	 engine. It is only implemented here as a reference to compare against our own
	 engine and for simpler cross-platform development. Maybe we can learn something
	 here?
	 
	 On GNU/Linux it is implemented via the free LGPL'ed GStreamer-Multimedia plugin
	 framework. As such, it should be able to support any video source and video
	 preprocessing operation provided by GStreamers plugins: We have at least support
	 for video4linux and video4linux2 video devices (webcams, DVB, TV tuners, analog
													 cameras and framegrabbers and whatnot), DV firewire consumer cameras, IIDC machine
	 vision cameras, network video (GigE etc in the future), decoded video from video
	 files...
	 
	 On MS-Windows, it is implemented as a DirectShow filtergraph and should therefore
	 be able to access any device supported by a DirectShow Videofilter, ie., pretty
	 much any device on MS-Windows.
 
	NOTES:
 
	TODO:
 
 */


#include "Screen.h"
#include <float.h>

#ifdef PTBVIDEOCAPTURE_ARVIDEO

// These are the includes for ARToolkit and ARVideo:
#include <AR/video.h>

#ifndef DC1394_SUCCESS
#define DC1394_SUCCESS 0
#endif

// Record which defines all state for a capture device:
typedef struct {
	int valid;                        // Is this a valid device record? zero == Invalid.
	AR2VideoParamT	*camera;		// Ptr to a ARVideo camera object that holds the internal state for such cams.
	ARUint8			*frame;			// Ptr to a psych_uint8 matrix which contains the most recently captured/dequeued frame.
	int dropframes;					// 1 == Always deliver most recent frame in FIFO, even if dropping of frames is neccessary.
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
	PsychRectType roirect;            // Region of interest rectangle - denotes subarea of full video capture area.
	double avg_decompresstime;        // Average time spent in Quicktime/Sequence Grabber decompressor.
	double avg_gfxtime;               // Average time spent in GWorld --> OpenGL texture conversion and statistics.
	int nrgfxframes;                  // Count of fetched textures.
} PsychVidcapRecordType;

static PsychVidcapRecordType vidcapRecordBANK[PSYCH_MAX_CAPTUREDEVICES];
static int numCaptureRecords = 0;
static psych_bool firsttime = TRUE;

// Forward declaration of internal helper function:
void PsychARDeleteAllCaptureDevices(void);


/*    PsychGetARVidcapRecord() -- Given a handle, return ptr to video capture record.
*    --> Internal helper function of PsychVideoCaptureSupport.
*/
PsychVidcapRecordType* PsychGetARVidcapRecord(int deviceIndex)
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

/* CHECKED
*     PsychARVideoCaptureInit() -- Initialize video capture subsystem.
*     This routine is called by Screen's RegisterProject.c PsychModuleInit()
*     routine at Screen load-time. It clears out the vidcapRecordBANK to
*     bring the subsystem into a clean initial state.
*/
void PsychARVideoCaptureInit(void)
{
	// Initialize vidcapRecordBANK with NULL-entries:
	int i;
	for (i=0; i < PSYCH_MAX_CAPTUREDEVICES; i++) {
		vidcapRecordBANK[i].valid = 0;
	}    
	numCaptureRecords = 0;
	
	return;
}

/* CHECKED
*  void PsychARExitVideoCapture() - Shutdown handler.
*
*  This routine is called by Screen('CloseAll') and on clear Screen time to
*  do final cleanup. It deletes all capture objects
*
*/
void PsychARExitVideoCapture(void)
{
	// Release all capture devices
	PsychARDeleteAllCaptureDevices();
	
	// Reset firsttime flag to get a cold restart on next invocation of Screen:
	firsttime = TRUE;
	return;
}

/*  CHECKED
*  PsychARDeleteAllCaptureDevices() -- Delete all capture objects and release all associated ressources.
*/
void PsychARDeleteAllCaptureDevices(void)
{
	int i;
	for (i=0; i<PSYCH_MAX_CAPTUREDEVICES; i++) {
		if (vidcapRecordBANK[i].valid) PsychARCloseVideoCaptureDevice(i);
	}
	return;
}

/* CHECKED
*  PsychARCloseVideoCaptureDevice() -- Close a capture device and release all associated ressources.
*/
void PsychARCloseVideoCaptureDevice(int capturehandle)
{
	// Retrieve device record for handle:
	PsychVidcapRecordType* capdev = PsychGetARVidcapRecord(capturehandle);
	
	// Stop capture immediately if it is still running:
	PsychARVideoCaptureRate(capturehandle, 0, 0, NULL);
	
	// Close & Shutdown camera, release ressources:
	ar2VideoClose(capdev->camera);	
	capdev->camera = NULL;
	
	// Invalidate device record to free up this slot in the array:
	capdev->valid = 0;
    
	// Decrease counter of open capture devices:
	if (numCaptureRecords>0) numCaptureRecords--;
	
	// Done.
	return;
}

/* CHECKED TODO
*      PsychAROpenVideoCaptureDevice() -- Create a video capture object.
*
*      This function tries to open and initialize a connection to a camera
*      and returns the associated captureHandle for it.
*
*	   slotid = Number of slot in vidcapRecordBANK[] array to use for this camera.
*      win = Pointer to window record of associated onscreen window.
*      deviceIndex = Index of the grabber device. (Currently ignored)
*      capturehandle = handle to the new capture object.
*      capturerectangle = If non-NULL a ptr to a PsychRectangle which contains the ROI for capture.
*      reqdepth = Number of layers for captured output textures. (0=Don't care, 1=LUMINANCE8, 2=LUMINANCE8_ALPHA8, 3=RGB8, 4=RGBA8)
*      num_dmabuffers = Number of buffers in the ringbuffer queue (e.g., DMA buffers) - This is OS specific. Zero = Don't care.
*      allow_lowperf_fallback = If set to 1 then PTB can use a slower, low-performance fallback path to get nasty devices working.
*	   targetmoviefilename and recordingflags are currently ignored, they would refer to video harddics recording capabilities.
*/
psych_bool PsychAROpenVideoCaptureDevice(int slotid, PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle,
								   int reqdepth, int num_dmabuffers, int allow_lowperf_fallback, char* targetmoviefilename, unsigned int recordingflags)
{
    PsychVidcapRecordType	*capdev = NULL;
	int						w, h;
    char					msgerr[10000];
	char					config[1000];
	char					tmpstr[1000];

	config[0] = 0;
	tmpstr[0] = 0;
	
	// Default camera config:
	#if PSYCH_SYSTEM == PSYCH_WINDOWS
    //strcat(config, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><dsvl_input><camera show_format_dialog=\"false\" friendly_name=\"\"><pixel_format><RGB32 flip_h=\"false\" flip_v=\"true\"/></pixel_format></camera></dsvl_input>");

	// Prefix:
    strcat(config, "<?xml version=\"1.0\" encoding=\"UTF-8\"?><dsvl_input><camera show_format_dialog=\"false\" ");

	// Specific deviceIndex requested, instead of auto-select?
    if (deviceIndex >= 1 && deviceIndex <= 3) {
		// Fetch optional moviename parameter as name spec string:
		if (targetmoviefilename == NULL) PsychErrorExitMsg(PsychError_user, "You set 'deviceIndex' to a value of 1, 2 or 3, but didn't provide the required device name string in the 'moviename' argument! Aborted.");
		switch(deviceIndex) {
			case 1:
				sprintf(tmpstr, "friendly_name=\"%s\" ", targetmoviefilename);
				break;
				
			case 2:
				sprintf(tmpstr, "device_name=\"%s\" ", targetmoviefilename);
				break;
				
			case 3:
				sprintf(tmpstr, "ieee1394id=\"%s\" ", targetmoviefilename);
				break;
		}
		
		strcat(config, tmpstr);
	}
	else {
		// Default device index: Just pass through as default device:
		strcat(config, "friendly_name=\"\" ");
	}	
	#endif


	#if PSYCH_SYSTEM == PSYCH_OSX
	char *defaultcamconfig = "";
	#endif

	#if PSYCH_SYSTEM == PSYCH_LINUX
	char *defaultcamconfig = "-dev=/dev/video0 -channel=0 -palette=YUV420P -width=320 -height=240";
	#endif

	// Init capturehandle to none:
    *capturehandle = -1;
    
    if (firsttime) {
		// First time invocation:
        
        #if PSYCH_SYSTEM == PSYCH_WINDOWS
        // On Windows, we need to delay-load the libARvideo.dll DLL. This loading
        // and linking will automatically happen downstream. However, if delay loading
        // would fail, we would end up with a crash! For that reason, we try here to
        // load the DLL, just to probe if the real load/link/bind op later on will
        // likely succeed. If the following LoadLibrary() call fails and returns NULL,
        // then we know we would end up crashing. Therefore we'll output some helpful
        // error-message instead:
        if (NULL == LoadLibrary("libARvideo.dll")) {
            // Failed:
            printf("\n\nPTB-ERROR: Tried to startup video capture engine type 2 (ARVideo). This didn't work,\n");
            printf("PTB-ERROR: because one of the required helper DLL libraries failed to load. Probably because they\n");
            printf("PTB-ERROR: could not be found or could not be accessed (e.g., due to permission problems).\n\n");
            printf("PTB-ERROR: Please read the online help by typing 'help ARVideoCapture' for troubleshooting instructions.\n\n");
			PsychErrorExitMsg(PsychError_user, "Unable to start Videocapture engine ARVideo due to DLL loading problems. Aborted.");
        }
        #endif
        
		firsttime = FALSE;
    }

    // Slot 'slotid' will contain the record for our new capture object:

    // Initialize new record:
    vidcapRecordBANK[slotid].valid = 1;
    
    // Retrieve device record for slotid:
    capdev = PsychGetARVidcapRecord(slotid);
	
    capdev->camera = NULL;
    capdev->grabber_active = 0;
    capdev->scratchbuffer = NULL;        

    // ROI rectangle specified?
    if (capturerectangle) {
		// Extract wanted width and height:
		w = (int) PsychGetWidthFromRect(capturerectangle);
		h = (int) PsychGetHeightFromRect(capturerectangle);

		#if PSYCH_SYSTEM == PSYCH_OSX
		sprintf(tmpstr, " -width=%i -height=%i", w, h);
		#endif
		
		#if PSYCH_SYSTEM == PSYCH_WINDOWS		
		sprintf(tmpstr, " frame_width=\"%i\" frame_height=\"%i\" ", w, h);
		#endif

		#if PSYCH_SYSTEM == PSYCH_LINUX
		// TODO
		#endif		

		strcat(config, tmpstr);
    }

	if (num_dmabuffers > 0) {
		#if PSYCH_SYSTEM == PSYCH_WINDOWS
		// Get framerate from num_dmabuffers argument:
		sprintf(tmpstr, " frame_rate=\"%i\" ", num_dmabuffers);
		strcat(config, tmpstr);
		#endif
	}

#if PSYCH_SYSTEM == PSYCH_OSX
	// Disable setup dialog:
	strcat(config, " -nodialog");

	// Specific deviceIndex requested, instead of auto-select?
    if (deviceIndex > 0) {
		sprintf(tmpstr, " -grabber=%i", deviceIndex + 1);
		strcat(config, tmpstr);
	}
	else {
		deviceIndex = 0;
	}

	switch (reqdepth) {
		case 2:
			// A no-go: Instead we use 1 channel luminance8:
			if (PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Video capture engine doesn't support requested Luminance+Alpha format. Will revert to pure Luminance instead...\n");
		case 1:
			reqdepth = 1;
			sprintf(tmpstr, " -pixelformat=40");
			break;
			
		case 3:
			reqdepth = 3;
			sprintf(tmpstr, " -pixelformat=24");
			break;

		case 5:
			reqdepth = 4;
			sprintf(tmpstr, "");
			break;
		case 4:
		case 0:
			reqdepth = 4;
			sprintf(tmpstr, " -pixelformat=ARGB");
			break;
			
		default:
			// Unknown format:
			PsychErrorExitMsg(PsychError_user, "You requested an invalid image depths (not one of 0, 1, 2, 3 or 4). Aborted.");
	}

	strcat(config, tmpstr);
#endif

#if PSYCH_SYSTEM == PSYCH_WINDOWS
	if (reqdepth == 4 || reqdepth == 0) {
		// Default is RGB32 bit:
		reqdepth = 4;
		sprintf(tmpstr, "><pixel_format><RGB32 flip_h=\"false\" flip_v=\"true\"/></pixel_format></camera></dsvl_input>");	
	}
	else {
		// Only other supported format is RGB24 bit:
		switch (reqdepth) {
			case 1:
				if (PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Video capture engine doesn't support requested Luminance format. Will revert to RGB color instead...\n");
			break;
			
			case 2:
				// A no-go: Instead we use 1 channel luminance8:
				if (PsychPrefStateGet_Verbosity()>1) printf("PTB-WARNING: Video capture engine doesn't support requested Luminance+Alpha format. Will revert to RGB color instead...\n");
			break;
			
			case 3:
			break;
				
			default:
				// Unknown format:
				PsychErrorExitMsg(PsychError_user, "You requested an invalid image depths (not one of 0, 1, 2, 3 or 4). Aborted.");
		}
		
		reqdepth = 3;
		sprintf(tmpstr, "><pixel_format><RGB24 flip_h=\"false\" flip_v=\"true\"/></pixel_format></camera></dsvl_input>");	
	}

	strcat(config, tmpstr);
	
    if (deviceIndex == 4) {
		// Fetch optional moviename parameter as override configuration string:
		if (targetmoviefilename == NULL) PsychErrorExitMsg(PsychError_user, "You set 'deviceIndex' to a value of 4, but didn't provide the required override configuration string in the 'moviename' argument! Aborted.");

		// Reset config string:
		config[0] = 0;

		// And load the moviename argument as override string:
		strcat(config, targetmoviefilename);
	}
	
	// End of MS-Windows specific setup.
#endif

#if PSYCH_SYSTEM == PSYCH_LINUX
	// Specific deviceIndex requested, instead of auto-select?
    if (deviceIndex!=-1) {
		sprintf(tmpstr, " -dev=/dev/video%i", deviceIndex);
		strcat(config, tmpstr);
	}
	else {
		deviceIndex = 0;
	}

	reqdepth = 3;
#endif		

    // Requested output texture pixel depth in layers:
    capdev->reqpixeldepth = reqdepth;
	capdev->pixeldepth = reqdepth * 8;
	
    // Number of DMA ringbuffers to use in DMA capture mode: If no number provided (==0), set it to 8 buffers...
#if PSYCH_SYSTEM == PSYCH_OSX
	if (num_dmabuffers == 1) {
		// Use single-buffering instead of triple buffering:
		strcat(config, " -singlebuffer");
	}
	else {
		num_dmabuffers = 3;
	}

    capdev->num_dmabuffers = num_dmabuffers;
#else
    capdev->num_dmabuffers = num_dmabuffers;
#endif
	
	if (PsychPrefStateGet_Verbosity()>4) printf("PTB-INFO: Final configuration string passed to ARVideo library is:\n%s\n", config);

	// Prepare error message in case its needed below:
	sprintf(msgerr, "PTB-ERROR: Opening the %i. camera (deviceIndex=%i) failed!\n", deviceIndex + 1, deviceIndex);

	// Try to open and initialize camera according to given settings:
    capdev->camera = ar2VideoOpen(config);

	// Error abort if camera init failed:
	if(capdev->camera == NULL) {
		// Error abort here:
		capdev->valid = 0;
		PsychErrorExitMsg(PsychError_user, msgerr);
    }

    // Our camera should be ready: Assign final handle.
    *capturehandle = slotid;
	
    // Increase counter of open capture devices:
    numCaptureRecords++;
    
    // Set zero framerate:
    capdev->fps = 0;
    
    // Set image size:
	ar2VideoInqSize(capdev->camera, &(capdev->width), &(capdev->height));
    
	// Create capture ROI corresponding to width and height of video image:
	PsychMakeRect(capdev->roirect, 0, 0, capdev->width, capdev->height);

    // Reset framecounter:
    capdev->nrframes = 0;
    capdev->grabber_active = 0;
    
    printf("PTB-INFO: Camera successfully opened...\n");

    return(TRUE);
}

/* CHECKED
*  PsychARVideoCaptureRate() - Start- and stop video capture.
*
*  capturehandle = Grabber to start-/stop.
*  playbackrate = zero == Stop capture, non-zero == Capture
*  dropframes = 0 - Always deliver oldest frame in DMA ringbuffer. 1 - Always deliver newest frame.
*               --> 1 == drop frames in ringbuffer if behind -- low-latency capture.
*  startattime = Deadline (in system time) for which to wait before real start of capture.
*  Returns Number of dropped frames during capture.
*/
int PsychARVideoCaptureRate(int capturehandle, double capturerate, int dropframes, double* startattime)
{
	int dropped = 0;
	float framerate = 0;
	
	// Retrieve device record for handle:
	PsychVidcapRecordType* capdev = PsychGetARVidcapRecord(capturehandle);
	
	// Start- or stop capture?
	if (capturerate > 0) {
		// Start capture:
		if (capdev->grabber_active) PsychErrorExitMsg(PsychError_user, "You tried to start video capture, but capture is already started!");
		
		// Reset statistics:
		capdev->last_pts = -1.0;
		capdev->nr_droppedframes = 0;
		capdev->frame_ready = 0;
		
		// Framedropping is not supported by libARVideo, so we implement it ourselves.
		// Store the 'dropframes' flag in our capdev struct, so the PsychARGetTextureFromCapture()
		// knows how to handle this:
		capdev->dropframes = (dropframes > 0) ? 1 : 0;

		// Ready to go! Now we just need to tell the camera to start its capture cycle:
		
		// Wait until start deadline reached:
		if (*startattime != 0) PsychWaitUntilSeconds(*startattime);
		
		// Start DMA driven isochronous data transfer:
		if(PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Starting capture...\n"); fflush(NULL);

		// Start the video capture for this camera.
		if (ar2VideoCapStart(capdev->camera) !=DC1394_SUCCESS) {
			// Failed!
			PsychErrorExitMsg(PsychError_user, "Unable to start capture on camera via ar2VideoCapStart() - Start of video capture failed!");
		}
		
		// Record real start time:
		PsychGetAdjustedPrecisionTimerSeconds(startattime);
		
		if(PsychPrefStateGet_Verbosity()>5) printf("PTB-DEBUG: Capture engine fully running...\n"); fflush(NULL);
		
		// Query framerate and convert to floating point value and assign it:
		#if PSYCH_SYSTEM == PSYCH_WINDOWS
		ar2VideoInqFreq(capdev->camera, &framerate);
		#else
		// TODO: Implement for non-Win32:
		framerate = (float) capturerate;
		#endif

		capdev->fps = (double) framerate;

		// Ok, capture is now started:
		capdev->grabber_active = 1;
		
		// Allocate conversion buffer if needed for YUV->RGB conversions.
		if (capdev->pixeldepth == -1) {
			// Not used at the moment!!
			// Software conversion of YUV -> RGB needed. Allocate a proper scratch-buffer:
			capdev->scratchbuffer = malloc(capdev->width * capdev->height * 3);
		}
		
		if(PsychPrefStateGet_Verbosity()>1) {
			printf("PTB-INFO: Capture started on device %i - Width x Height = %i x %i - Framerate: %f fps.\n", capturehandle, capdev->width, capdev->height, capdev->fps);
		}
	}
	else {
		// Stop capture:
		if (capdev->grabber_active) {
			// Stop isochronous data transfer from camera:
			if (ar2VideoCapStop(capdev->camera) !=DC1394_SUCCESS) {
				PsychErrorExitMsg(PsychError_user, "Unable to stop video transfer on camera! (ar2VideoCapStop() failed)!");
			}
			
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
*  PsychARGetTextureFromCapture() -- Create an OpenGL texturemap from a specific videoframe from given capture object.
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
int PsychARGetTextureFromCapture(PsychWindowRecordType *win, int capturehandle, int checkForImage, double timeindex,
								 PsychWindowRecordType *out_texture, double *presentation_timestamp, double* summed_intensity, rawcapimgdata* outrawbuffer)
{
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
	
    // Retrieve device record for handle:
    PsychVidcapRecordType* capdev = PsychGetARVidcapRecord(capturehandle);
	
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
	
    // int waitforframe = (checkForImage > 1) ? 1:0; // Blocking wait for new image requested?
	
	// A checkForImage 4 means "no op" with the ARVideo capture engine: This is meant to drive
	// a movie recording engine, ie., grant processing time to it. Our ARVideo engine doesn't
	// support movie recording, so this is a no-op:
	if (checkForImage == 4) return(0);

    // Take start timestamp for timing stats:
    PsychGetAdjustedPrecisionTimerSeconds(&tstart);
	
    // Should we just check for new image?
    if (checkForImage) {
		// Reset current dropped count to zero:
		capdev->current_dropped = 0;
        
		if (capdev->grabber_active == 0) {
			// Grabber stopped. We'll never get a new image:
			return(-2);
		}
		
		// Check for image in polling mode: We capture in non-blocking mode:			
		capdev->frame = ar2VideoGetImage(capdev->camera);

		// Ok, call succeeded. If the 'frame' pointer is non-NULL then there's a new frame ready and dequeued from DMA
		// ringbuffer. We'll return it on next non-poll invocation. Otherwise no new video data ready yet:
		capdev->frame_ready = (capdev->frame != NULL) ? 1 : 0;

		
		if (capdev->frame_ready) {
			// Store count of currently queued frames (in addition to the one just fetched).
			// This is an indication of how well the users script is keeping up with the video stream,
			// technically the number of frames that would need to be dropped to keep in sync with the
			// stream.
			// TODO: Think about this. ARVideo doesn't support a query for pending/dropped frames, so
			// we either need to live without this feature or think up something clever...
			capdev->current_dropped = (int) 0;
			
			// Ok, at least one new frame ready. If more than one frame has queued up and
			// we are in 'dropframes' mode, ie. we should always deliver the most recent available
			// frame, then we quickly fetch & discard all queued frames except the last one.
			while((capdev->dropframes) && ((int) capdev->current_dropped > 0)) {
				// We just poll - fetch the frames. As we know there are some queued frames, it
				// doesn't matter if we poll or block, but polling sounds like a bit less overhead
				// at the OS level:
				
				// First enqueue the recently dequeued buffer...
				if (ar2VideoCapNext(capdev->camera) != DC1394_SUCCESS) {
					PsychErrorExitMsg(PsychError_system, "Requeuing of discarded video frame failed while dropping frames (dropframes=1)!!!");
				}
				
				// Then fetch the next one:
				if ((capdev->frame = ar2VideoGetImage(capdev->camera)) == NULL) {
					// Polling failed for some reason...
					PsychErrorExitMsg(PsychError_system, "Polling for new video frame failed while dropping frames (dropframes=1)!!!");
				}
				
			}
			
			// Update stats for decompression:
			PsychGetAdjustedPrecisionTimerSeconds(&tend);
			
			// Increase counter of decompressed frames:
			capdev->nrframes++;
			
			// Update avg. decompress time:
			capdev->avg_decompresstime+=(tend - tstart);
			
			// Query capture timestamp in seconds:
			// TODO: ARVideo doesn't provide such a timestamp. For now we just return the current
			// system time as a lame replacement...
			// On Windows there would be uint64 capdev->camera->g_Timestamp
			PsychGetAdjustedPrecisionTimerSeconds(&(capdev->current_pts));
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
	
    // input_image points to the image buffer in our cam:
    input_image = (unsigned char*) (capdev->frame);
	
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
		PsychMakeRect(out_texture->rect, 0, 0, w, h);    
		
		// Set NULL - special texture object as part of the PTB texture record:
		out_texture->targetSpecific.QuickTimeGLTexture = NULL;
		
		// Set texture orientation as if it were an inverted Offscreen window: Upside-down.
		out_texture->textureOrientation = 3;
		
		#if PSYCH_SYSTEM == PSYCH_WINDOWS
		// On Windows in non RGB32 bit modes, set orientation to Upright:
		out_texture->textureOrientation = (capdev->reqpixeldepth == 4) ? 3 : 2;
		#endif

		// Setup a pointer to our buffer as texture data pointer: Setting memsize to zero
		// prevents unwanted free() operation in PsychDeleteTexture...
		out_texture->textureMemorySizeBytes = 0;
		
		// Set texture depth: Could be 8, 16, 24 or 32 bpp.
		out_texture->depth = capdev->reqpixeldepth * 8;
		
		// This will retrieve an OpenGL compatible pointer to the pixel data and assign it to our texmemptr:
		out_texture->textureMemory = (GLuint*) input_image;
		
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
	if (ar2VideoCapNext(capdev->camera) != DC1394_SUCCESS) {
		PsychErrorExitMsg(PsychError_system, "Re-Enqueuing processed video frame failed.");
	}

    // Update total count of dropped (or pending) frames:
    capdev->nr_droppedframes += capdev->current_dropped;
    nrdropped = capdev->current_dropped;
    capdev->current_dropped = 0;
	
    // Timestamping:
    PsychGetAdjustedPrecisionTimerSeconds(&tend);
	
    // Increase counter of retrieved textures:
    capdev->nrgfxframes++;
	
    // Update average time spent in texture conversion:
    capdev->avg_gfxtime+=(tend - tstart);
    
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
double PsychARVideoCaptureSetParameter(int capturehandle, const char* pname, double value)
{
	unsigned int minval, maxval, intval, oldintval;
	int triggercount;
	
	double oldvalue = DBL_MAX; // Initialize return value to the "unknown/unsupported" default.
	psych_bool assigned = false;
	psych_bool present  = false;
	
	// Retrieve device record for handle:
	PsychVidcapRecordType* capdev = PsychGetARVidcapRecord(capturehandle);
	
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
		ar2VideoDispOption();
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
