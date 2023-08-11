/*
	PsychToolbox3/Source/Common/PsychCV/PsychCVARToolkit.c
	
	PLATFORMS:	All

	AUTHORS:
	
	Mario Kleiner   mk      mario.kleiner at tuebingen.mpg.de
	
	HISTORY:
	
	19.04.09		mk		Initial implementation.  
	
	DESCRIPTION:
	
	Contains the implementation of our interface to the ARToolkit and all our
	convenience- and higher-level routines using that toolkit.
	
	Usually user code won't call this module directly, but it will call high level
	M-File code which in turn "outsources" parts of computationally very expensive,
	ARToolkit related tasks to this module.

	TODO:
	
	* Support for 2-Camera stereo processing.
	* Input image format/color conversion to cope with different input formats.
	* Use of other nice bits inside toolkit?

*/

// Include AR toolkit specific routines and includes, but also PsychCV.h
#include "PsychCVARToolkit.h"

// ARToolkit includes:
#include <AR/gsub.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/arMulti.h>
#include <AR/gsub_lite.h>

// Declare variables local to this file.  

// Level of verbosity: Defined in PsychCV.c, read-only accessed here:
extern unsigned int  verbosity;

// Status: PsychCVARToolkit Initialized or not?
static psych_bool psychCVARInitialized = FALSE;

static psych_bool arglInitialized = FALSE;

// Threshold for binarization of input image via intensity thresholding:
int imgBinarizationThreshold;

// Pointer to input image buffer:
static	ARUint8* arImagebuffer = NULL;

// Pointer to processing image buffer:
static  ARUint8* arTrackBuffer = NULL;

// Fornat of image buffer:
int		imgWidth, imgHeight, imgChannels, imgFormat;

// Camera calibration parameters:
ARParam         cameraParams;

// Scaling factor AR units [millimeters] -> OpenGl drawing units.
// Start with one to one mapping by default:
static double view_scalefactor = 1.0;

// Clip near and clip far distance for frustums compute by ARToolkit
// when returning a gl projection matrix:
static double view_distance_min = 0.1;
static double view_distance_max = 1000.0;

// Settings for ARGL library:
static ARGL_CONTEXT_SETTINGS_REF gArglSettings = NULL;

#define PSYCHCVAR_MAX_MARKERCOUNT 100

// Multimarker definition array:
typedef struct PsychCVARMarkerInfoStruct {
	int		isMultiMarker;
	double	matchError;
	double	oldTrans[3][4];	
	double	patt_width;
	double	patt_center[2];
	
	union {
		ARMultiMarkerInfoT*	multiMarker;
		int					singleMarker;
	} marker;
} PsychCVARMarkerInfoStruct;

struct PsychCVARMarkerInfoStruct	arMarkers[PSYCHCVAR_MAX_MARKERCOUNT];
static int							markerCount = 0;

// Internal helper: Perform image data conversion if required:
void PsychCVARConvertInputImage(void)
{
	ARUint8* src = arImagebuffer;
	ARUint8* dst = arTrackBuffer;
	ARUint8  dummy;
	int i, count;
	
	// Conversion needed at all? If input matches requested channelcount and image format
	// of ARToolkit, then there ain't nothing to do and we can return immediately:
	if (imgChannels == AR_PIX_SIZE_DEFAULT && imgFormat == AR_DEFAULT_PIXEL_FORMAT) return;

	count = imgWidth * imgHeight;
	
	// Matching channel count from input to output? This would be 3 or 4 channels,
	// as our target platforms don't use anything else as AR_PIX_SIZE_DEFAULT.
	if (src == dst) {
		// Yes: 3 -> 3 or 4 -> 4. So we just need to swizzle...
		if (AR_PIX_SIZE_DEFAULT == 3) {
			// RGB -> BGR or BGR -> RGB conversion. In any case, need to
			// switch 1st and 3rd component:
			for (i = 0; i < count; i++) {
				dummy = src[0];
				src[0] = src[2];
				src[2] = dummy;
				src+=3;
			}

			// Done:
			return;
		}
		else {
			// ARGB -> BGRA or vice versa. Switch 1st with 4th, 2nd with 3rd:
			for (i = 0; i < count; i++) {
				dummy = src[0];
				src[0] = src[3];
				src[3] = dummy;
				dummy = src[2];
				src[2] = src[1];
				src[1] = dummy;
				src+=4;
			}

			// Done:
			return;
		}
	}
	else {
		// Separate buffers: 1->3, 1->4, 3->4 or 4->3:
		if (imgChannels == 1) {
			if (AR_PIX_SIZE_DEFAULT == 3) {
				// Luminance -> RGB expansion:
				for (i = 0; i < count; i++) {
					dummy = *(src++);
					*(dst++) = dummy;
					*(dst++) = dummy;
					*(dst++) = dummy;
				}

				// Done:
				return;
			}
			else {
				// Luminance -> RGBA (or ABGR) expansion: As
				// A channel will be ignored, just replicate
				// luminance into all 4 channels:
				for (i = 0; i < count; i++) {
					dummy = *(src++);
					*(dst++) = dummy;
					*(dst++) = dummy;
					*(dst++) = dummy;
					*(dst++) = dummy;
				}			

				// Done:
				return;
			}
		}
		
		if (imgChannels == 3) {
			// RGB --> ARGB or BGRA expansion: All our video capture engines
			// deliver RGB if 3 channel data is requested.
			if (AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_ARGB) {
				for (i = 0; i < count; i++) {
					// Zero-fill first byte (the A part of ARGB):
					*(dst++) = 0;
					// Copy last 3 bytes (RGB part):
					*(dst++) = *(src++);
					*(dst++) = *(src++);
					*(dst++) = *(src++);
				}

				// Done:
				return;				
			}
			
			if (AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_BGRA) {
				for (i = 0; i < count; i++) {
					// Copy and swizzle first 3 bytes (RGB part to BGR):
					*(dst++) = src[2];
					*(dst++) = src[1];
					*(dst++) = src[0];
					src+=3;
					// Zero-fill last byte (the A part of BGRA):
					*(dst++) = 0;
				}

				// Done:
				return;
			}
		
			// Other target formats etc. are not relevant to our platforms...
		}

		if (imgChannels == 4) {
			// ARGB or BGRA --> RGB contraction: Target is always RGB,
			// source depends on input image format
			if (imgFormat == AR_PIXEL_FORMAT_ARGB) {
				for (i = 0; i < count; i++) {
					// Copy first 3 bytes (RGB part):
					*(dst++) = *(src++);
					*(dst++) = *(src++);
					*(dst++) = *(src++);
					// Skip last byte (the A part of ARGB):
					src++;
				}

				// Done:
				return;
			}
			
			if (imgFormat == AR_PIXEL_FORMAT_BGRA) {
				for (i = 0; i < count; i++) {
					// Skip first byte (the A part of BGRA):
					src++;
					// Copy and swizzle last 3 bytes (RGB part to BGR):
					*(dst++) = src[2];
					*(dst++) = src[1];
					*(dst++) = src[0];
					src+=3;
				}

				// Done:
				return;
			}
		
			// Other target formats etc. are not relevant to our platforms...
		}
	}

	// If we reach this point, then some unsupported input -> output conversion
	// was requested!
	PsychErrorExitMsg(PsychError_user, "Unknown or unsupported input image format settings 'imgChannels' and/or 'imgFormat' encountered! Check your settings in PsychCV('ARInitialize')!");
	
	return;
}

void PsychCVARExit(void)
{
	int i;
	
	// Perform Shutdown operation, if needed. Called from PsychCVExit routine
	// at PsychCV shutdown/flush time, or explicitely via subfunction 'ARShutdown':
	if (psychCVARInitialized) {
		// Release buffer memory, if any:
		if ((arTrackBuffer) && (arTrackBuffer != arImagebuffer)) free(arTrackBuffer);
		arTrackBuffer = NULL;

		if (arImagebuffer) free(arImagebuffer);
		arImagebuffer = NULL;
	
		// Release all marker and pattern definitions:
		for (i = 0; i < markerCount; i++) {
			if (arMarkers[i].isMultiMarker) {
				arMultiFreeConfig(arMarkers[i].marker.multiMarker);
				arMarkers[i].marker.multiMarker = NULL;
			}
			else {
				arFreePatt(arMarkers[i].marker.singleMarker);
				arMarkers[i].marker.singleMarker = -1;
			}
		}
		
		// Reset count of loaded marker definitions:
		markerCount = 0;
		
		// Does ARGL need cleanup?
		if (arglInitialized) {
			arglCleanup(gArglSettings);
			gArglSettings = NULL;
			arglInitialized = FALSE;
		}

		psychCVARInitialized = FALSE;
	}
}

/* PsychCV('ARInitialize') - Initialize a new tracking session with ARToolkit:
 *
 * Allocate internal image memory buffers and data structures,
 * load camera calibration files and customize it for given
 * video image format.
 *
 * No handle is returned, as we currently only allow one instance of the ARToolkit.
 *
 */
PsychError PSYCHCVARInitialize(void)
{
 	static char useString[] = "[SceneImageMemBuffer, glProjectionMatrix, DebugImageMemBuffer] = PsychCV('ARInitialize', cameraCalibFilename, imgWidth, imgHeight, imgChannels [, imgFormat]);";
	//							1					 2					 3												1					 2		   3		  4              5
	static char synopsisString[] = 
		"Initialize ARToolkit subsystem prior to first use.\n\n"
		"Camera calibration info for the camera used for tracking is loaded from the "
		"file given by 'cameraCalibFilename'. Internal video image memory buffers "
		"are setup for input images of size 'imgWidth' x 'imgHeight' pixels, with "
		"'imgChannels' color channels (1, 3 or 4 are valid settings) and input "
		"color format 'imgFormat' (or a default setting if 'imgFormat' is omitted). "
		"imgFormat can be one of:\n"
		"RGB = 1, BGR = 2, BGRA = 4, ARGB = 7, MONO = 6. Other formats are not "
		"supported for input images, but these are the ones provided by Psychtoolboxs "
		"various video capture engines for different video sources and settings.\n"
		"Then the camera calibration is adapted to the given image size, ARToolkit "
		"is initialized, and a memory buffer handle 'SceneImageMemBuffer' to the "
		"internal video memory buffer is returned.\n\n"
		"You should pass this handle to Psychtoolbox functions for videocapture etc. "
		"to acquire video images from the scene to track and to store that input "
		"images inside ARToolkits video buffer.\n\n"
		"After this step, you can define the markers to detect and track via the "
		"subfunction PsychCV('ARLoadMarker') and then commence the actual tracking "
		"operations by calls to Screen()'s video capture engine and PsychCV's "
		"'DetectMarkers' subfunction.\n"
		"'glProjectionMatrix' is a 4x4 matrix that can be directly loaded into "
		"OpenGL's projection matrix to define optimal camera parameters for AR apps.\n"
		"'DebugImageMemBuffer' is a memory pointer to an image buffer that "
		"contains debug images (results of AR's image segmentation) at least "
		"if the PsychCV('Verbosity') level is set to 7 or higher.\n\n";

	static char seeAlsoString[] = "";	 

	double			*projmatrixGL;
    ARParam			wparam;
	char*			cameraCalibFilename = NULL;

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(5));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(4)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(3));	 // The maximum number of outputs

	if (psychCVARInitialized) PsychErrorExitMsg(PsychError_user, "ARToolkit already initialized! Call PsychCV('ARShutdown') first and retry!");

	// Get name of camera calibration file:
	PsychAllocInCharArg(1, TRUE, &cameraCalibFilename);

	PsychCopyInIntegerArg(2, TRUE, &imgWidth);
	if (imgWidth < 1 || imgWidth > 1024) PsychErrorExitMsg(PsychError_invalidRectArg, "Invalid image width provided. Must be between 1 and 1024 pixels!");

	PsychCopyInIntegerArg(3, TRUE, &imgHeight);
	if (imgHeight < 1 || imgHeight > 1024) PsychErrorExitMsg(PsychError_invalidRectArg, "Invalid image height provided. Must be between 1 and 1024 pixels!");

	PsychCopyInIntegerArg(4, TRUE, &imgChannels);
	if (imgChannels < 1 || imgChannels > 4 || imgChannels == 2) PsychErrorExitMsg(PsychError_invalidRectArg, "Invalid imgChannles provided. Must be 1, 3 or 4!");

	// Setup default imgFormat, depending on number of input channels:
	imgFormat = AR_DEFAULT_PIXEL_FORMAT;
	if (imgChannels == 1) imgFormat = AR_PIXEL_FORMAT_MONO;
	if (imgChannels == 3) imgFormat = AR_PIXEL_FORMAT_RGB;
	
	// Get optional imgFormat:
	PsychCopyInIntegerArg(5, kPsychArgOptional, &imgFormat);
	if (imgFormat < 0) PsychErrorExitMsg(PsychError_invalidRectArg, "Invalid image format provided!");

    // Load the initial camera parameters:
    if(arParamLoad(cameraCalibFilename, 1, &wparam) < 0 ) {
		// Failed.
		PsychErrorExitMsg(PsychError_user, "Failed to load camera calibration file! Wrong filename or file inaccessible?");
    }

	// Adapt size of calibration data to our input image size and
	// fill our global camera struct with it:
    arParamChangeSize(&wparam, imgWidth, imgHeight, &cameraParams);
	
	// Final init, the camera parameters are ready after this:
    arInitCparam(&cameraParams);

	// Compute a suitable OpenGL projection matrix for these intrinsic
	// camera parameters: Return it as 4 x 4 matrix in 2nd optional
	// return argument:
	PsychAllocOutDoubleMatArg(2, FALSE, 4, 4, 1, &projmatrixGL);
	
	// Let ARToolkit do the actual job:
	arglCameraFrustumRH(&cameraParams, view_distance_min, view_distance_max, projmatrixGL);

	// Allocate internal memory buffer of sufficient size:
	// We always allocate for 4 channels, even if less are specified! This is
	// a "better safe than sorry" measure against out-of-bounds memory writes
	// under certain conditions - After all this wastes at most 4 MB or RAM,
	// so what?
	arImagebuffer = (ARUint8*) malloc(imgWidth * imgHeight * 4);
	if (NULL == arImagebuffer) PsychErrorExitMsg(PsychError_outofMemory, "Out of memory when trying to initialize ARToolkit subsystem!");

	// Return double-encoded void* memory pointer to video image input buffer:
	PsychCopyOutDoubleArg(1, kPsychArgRequired, PsychPtrToDouble(arImagebuffer));

	// Check if we need conversion for image data:
	if (AR_PIX_SIZE_DEFAULT != imgChannels) {
		// Need conversion and therefore intermediate conversion buffer:
		arTrackBuffer = (ARUint8*) malloc(imgWidth * imgHeight * AR_PIX_SIZE_DEFAULT);
	}
	else {
		arTrackBuffer = arImagebuffer;
	}

	// Return double-encoded void* memory pointer to video processing debug buffer:
	// TODO: Set properly. For now identical to input buffer...
	PsychCopyOutDoubleArg(3, FALSE, PsychPtrToDouble(arImagebuffer));

	// Init threshold to 128 == 50% max intensity on 8 bit input values:
	imgBinarizationThreshold = 128;

	// We're online!
	psychCVARInitialized = TRUE;
	
	// Ready.
	return(PsychError_none);
}

PsychError PSYCHCVARShutdown(void)
{
 	static char useString[] = "PsychCV('ARShutdown');";
	static char synopsisString[] = 
		"Shutdown ARToolkit subsystem after use, release all ressources.\n"
		"The memory buffer handle 'SceneImageMemBuffer' returned by a prior call "
		"to SceneImageMemBuffer = PsychCV('ARInitialize') will be invalid after "
		"this shutdown call and must not be used anymore, or Psychtoolbox will crash!\n";
				
	static char seeAlsoString[] = "";	 

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(0));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));	 // The maximum number of outputs

	// Cleanup and shutdown:
	PsychCVARExit();

	// Ready.
	return(PsychError_none);
}


PsychError PSYCHCVARLoadMarker(void)
{
 	static char useString[] = "[markerId] = PsychCV('ARLoadMarker', markerFilename [, isMultiMarker][, patt_width][, patt_center_x][, patt_center_y]);";
	//																1				  2				   3			 4				  5
	static char synopsisString[] = 
		"Load a marker definition into ARToolkit subsystem prior to first use.\n"
		"The marker definition used for tracking is loaded from the file given "
		"by 'markerFilename'. If the optional argument 'isMultiMarker' is set to "
		"1, then a multi marker definition file is loaded instead of a single "
		"marker definition file. Single marker load is the default. \n"
		"For single markers, you can optionally provide the 'patt_width' width "
		"of the pattern in millimeters (default is 80 mm), and the center of "
		"the pattern (x,y) coordinate 'patt_center_x' and 'patt_center_y' with "
		"a default center of (0,0).\n"
		"The function returns a unique handle 'markerId' on success. The marker "
		"will be identified by that handle in all future function calls.\n\n"
		"After this step, you can commence the actual tracking operations "
		"by calls to Screen()'s video capture engine and PsychCV's "
		"'DetectMarkers' subfunction.\n";

	static char seeAlsoString[] = "";	 

	char*			markerFilename = NULL;
	int				isMultiMarker = 0;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(5));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	if (!psychCVARInitialized) PsychErrorExitMsg(PsychError_user, "ARToolkit not yet initialized! Call PsychCV('ARInitialize') first and retry!");

	// Get name of marker definition file:
	PsychAllocInCharArg(1, TRUE, &markerFilename);

	// Get optional multimarker load flag:
	PsychCopyInIntegerArg(2, FALSE, &isMultiMarker);
	if (isMultiMarker < 0 || isMultiMarker > 1) PsychErrorExitMsg(PsychError_invalidRectArg, "Invalid 'isMultiMarker' flag provided. Must be 0 or 1!");
	
	// Find next slot in our array:
	if (markerCount >= PSYCHCVAR_MAX_MARKERCOUNT) PsychErrorExitMsg(PsychError_user, "Cannot load new marker definition, as maximum allowable markercount exceeded!");

	// Multimarker load?
	if (isMultiMarker) {
		// Load multi marker:
		arMarkers[markerCount].isMultiMarker = 1;
		if((arMarkers[markerCount].marker.multiMarker = arMultiReadConfigFile(markerFilename)) == NULL) {
			printf("PsychCV: ERROR: Failed to load multi-markerfile %s .\n", markerFilename);
			PsychErrorExitMsg(PsychError_user, "Failed to load multimarker definition file! Invalid filename or file inaccessible?");
		}

		if (-1 == arMultiActivate(arMarkers[markerCount].marker.multiMarker)) PsychErrorExitMsg(PsychError_user, "Failed to activate multimarker!");
	}
	else {
		// Load single marker:
		arMarkers[markerCount].isMultiMarker = 0;
		if((arMarkers[markerCount].marker.singleMarker = arLoadPatt(markerFilename)) < 0) {
			printf("PsychCV: ERROR: Failed to load single-markerfile %s.\n", markerFilename);
			PsychErrorExitMsg(PsychError_user, "Failed to load single marker definition file! Invalid filename or file inaccessible?");
		}
		
		// Static pattern properties for single marker matching:
		
		// Init pattern width to 80 mm:
		arMarkers[markerCount].patt_width = 80.0;

		// Optional override:
		PsychCopyInDoubleArg(3, FALSE, &(arMarkers[markerCount].patt_width));
		
		// Init pattern center to (0,0)
		arMarkers[markerCount].patt_center[0] = 0.0;
		arMarkers[markerCount].patt_center[1] = 0.0;

		// Optional (x,y) override:
		PsychCopyInDoubleArg(4, FALSE, &(arMarkers[markerCount].patt_center[0]));
		PsychCopyInDoubleArg(5, FALSE, &(arMarkers[markerCount].patt_center[1]));

		if (-1 == arActivatePatt(arMarkers[markerCount].marker.singleMarker)) PsychErrorExitMsg(PsychError_user, "Failed to activate pattern!");
	}

	// Initialize matchError to infinite:
	arMarkers[markerCount].matchError = DBL_MAX;

	// Return markerhandle:
	PsychCopyOutDoubleArg(1, kPsychArgRequired, markerCount);

	// Mark this slot as used:
	markerCount++;

	// Ready.
	return(PsychError_none);
}

PsychError PSYCHCVARDetectMarkers(void)
{
 	static char useString[] = "[detectedMarkers] = PsychCV('ARDetectMarkers'[, markerSubset][, threshold] [, infoType]);";
	//																		1				2			  3
	static char synopsisString[] = 
		"Detect markers in the current video image, return information about them.\n\n"
		"Analyzed the current video image stored in the internal input image buffer and "
		"tries to detect the markers or multimarkers previously loaded. For all detected "
		"markers, their 3D position and orientation is computed, using the loaded camera "
		"calibration. Return argument 'detectedMarkers' is an array of structs, with one "
		"slot/struct for each successfully detected marker. The struct contains info about "
		"identity of the marker or multimarker (its handle), a confidence value for the "
		"reliability of detection, misc other info, and a 4x4 OpenGL modelview matrix with "
		"the rigid position and orientation of the marker in 3D space.\n\n"
		"If you don't want to detect all markers, but only a subset, pass a list of "
		"candidate marker handles via 'markerSubset'. Provide an optional greylevel "
		"threshold value for image processing in 'threshold'. Ask only for a subset of "
		"information by providing 'infoType'.\n\n";

	static char seeAlsoString[] = "";	 
	double*		markerSubset = NULL;
	int			i,m,n,p;
	double		threshold;
	int			infoType;
	int			candHandle;

    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             j, k;

	double*		xformMatrix;
	double*		ModelviewMatrixGL;
	
	PsychGenericScriptType 	*detectedMarkers, *myMatrix;
	const char *FieldNames[]={	"Id", "MatchError", "MultiMarker", "TransformMatrix", "ModelViewMatrix"};
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(3));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	if (!psychCVARInitialized) PsychErrorExitMsg(PsychError_user, "ARToolkit not yet initialized! Call PsychCV('ARInitialize') first and retry!");
	if (markerCount < 1) PsychErrorExitMsg(PsychError_user, "No markers loaded for detection! Call PsychCV('ARLoadMarker') first to load at least one marker and retry!");

	// Perform image data conversion if required:
	PsychCVARConvertInputImage();

	// Update AR's debugging level:
	arDebug = (verbosity > 5) ? 1 : 0;

	// Get optional markerSubset list:
	if (PsychAllocInDoubleMatArg(1, FALSE, &m, &n, &p, &markerSubset)) {
		// List provided: Sanity check!
		if (p != 1 || (m*n < 1)) PsychErrorExitMsg(PsychError_user, "Invalid 'markerSubset' specified: Must be a 1D or 2D vector or matrix with handles!");
		n = m * n;

		// Validate...
		for (i = 0; i < n; i++) {
			if (markerSubset[i] < 0 || markerSubset[i] >= markerCount) {
				printf("PsychCV-ERROR: Invalid markerhandle %i passed in 'markerSubset' argument! No such marker available!\n", markerSubset[i]);
				PsychErrorExitMsg(PsychError_user, "Invalid 'markerSubset' specified: Must be a 1D or 2D vector or matrix with handles!");
			}
		}
	}
	else {
		// No list provided: Create our default match list which contains all current markers:
		markerSubset = (double*) PsychMallocTemp(sizeof(double) * markerCount);
		n = markerCount;
		for (i = 0; i < n; i++) markerSubset[i] = (double) i;
	}
	
	// Get optional threshold value, default to our startup default:
	threshold = (double) imgBinarizationThreshold;
	PsychCopyInDoubleArg(2, FALSE, &threshold);
	if (threshold < 1 || threshold > 254) PsychErrorExitMsg(PsychError_user, "Invalid 'threshold' provided. Must be integer between 1 and 254!");

	// Update our default threshold with new value:
	imgBinarizationThreshold = (int) (threshold + 0.5);

	// Get optional infoType flag:
	infoType = 0xffffff;
	PsychCopyInIntegerArg(3, FALSE, &infoType);
	if (infoType < 0) PsychErrorExitMsg(PsychError_user, "Invalid 'infoType' provided. Must be positive integer!");
	
	// Ok, we got the user arguments. Let's do the actual detection:
	if(arDetectMarker(arTrackBuffer, imgBinarizationThreshold, &marker_info, &marker_num) < 0) {
		PsychErrorExitMsg(PsychError_user, "Marker detection failed for some reason. [arDetectMarker() failed]!");
	}

	if (verbosity > 4) {
		printf("PsychCV-INFO: ARDetectMarkers: Detected %i markers in image.\n", marker_num);
		for (i = 0; i< marker_num; i++) {
			printf("Marker %i: id = %i, cf = %f\n", i, marker_info[i].id, marker_info[i].cf);
		}
	}
	
	// Any markers detected?
	// Create our fixed size return array with one slot per requested candidate marker:
	PsychAllocOutStructArray(1, TRUE, n, 5, FieldNames, &detectedMarkers);

	// Process all our 'n' candidate markers against detected markers:
	for (i = 0; i < n; i++) {
		// Retrieve handle of i'th candidate:
		candHandle = (int) markerSubset[i];
		
		// Assign marker id to output slot:
		PsychSetStructArrayDoubleElement("Id", i, candHandle, detectedMarkers);
		
		// Assign multimarker flag:
		PsychSetStructArrayDoubleElement("MultiMarker", i, arMarkers[candHandle].isMultiMarker, detectedMarkers);
		
		// Allocate and assign 4x4 xform matrix: xformmatrix must be filled with the 16 values
		// of the actual matrix later on, we init it with all-zeros:
		xformMatrix = NULL;
		PsychAllocateNativeDoubleMat(4, 4, 1, &xformMatrix, &myMatrix);
		PsychSetStructArrayNativeElement("TransformMatrix", i, myMatrix, detectedMarkers);
		memset(xformMatrix, 0, sizeof(double) * 4 * 4);
		ModelviewMatrixGL = NULL;
		PsychAllocateNativeDoubleMat(4, 4, 1, &ModelviewMatrixGL, &myMatrix);
		PsychSetStructArrayNativeElement("ModelViewMatrix", i, myMatrix, detectedMarkers);
		memset(ModelviewMatrixGL, 0, sizeof(double) * 4 * 4);

		// Multimarker candidate?
		if (arMarkers[candHandle].isMultiMarker) {
			// Multimarker candidate:
			if ( (arMarkers[candHandle].matchError = (double) arMultiGetTransMat(marker_info, marker_num, arMarkers[candHandle].marker.multiMarker)) >= 0) {
				// Got it! Assign transform Matrix:
				if (verbosity > 4) printf("PsychCV-INFO: ARDetectMarkers: Multimarker %i has matchError %f\n", candHandle, (float) arMarkers[candHandle].matchError);
				argConvGlpara(arMarkers[candHandle].marker.multiMarker->trans, xformMatrix);  
				arglCameraViewRH(arMarkers[candHandle].marker.multiMarker->trans, ModelviewMatrixGL, view_scalefactor);
			}
			else {
				// This one not detected, transform matrix will be all-zero:
				if (verbosity > 4) printf("PsychCV-INFO: ARDetectMarkers: Non-Matched MultiMarker %i has matchError %f\n", candHandle, (float) arMarkers[candHandle].matchError);
				arMarkers[candHandle].matchError = DBL_MAX;
			}
		}
		else {
			// Singlemarker candidate: Search for best matching pattern:
			k = -1;
			for( j = 0; j < marker_num; j++ ) {
				if( arMarkers[candHandle].marker.singleMarker == marker_info[j].id ) {
					if( k == -1 ) k = j;
					else if( marker_info[k].cf < marker_info[j].cf ) k = j;
				}
			}
			if (k == -1) {
				arMarkers[candHandle].matchError = DBL_MAX;
				if (verbosity > 4) printf("PsychCV-INFO: ARDetectMarkers: Non-Matched Marker %i with pattern id %i has matchError %f\n", candHandle, arMarkers[candHandle].marker.singleMarker, (float) arMarkers[candHandle].matchError);
			}
			else {
				if (arMarkers[candHandle].matchError == DBL_MAX) {
					// Previous iteration didn't detect this marker: Previous trans
					// matrix is invalid:
					arGetTransMat(&marker_info[k], arMarkers[candHandle].patt_center, arMarkers[candHandle].patt_width, arMarkers[candHandle].oldTrans);
				}
				else {
					// Previous iteration delivered valid result: Use old trans matrix
					// to stabilize reconstruction in this cycle:
					arGetTransMatCont(&marker_info[k], arMarkers[candHandle].oldTrans, arMarkers[candHandle].patt_center, arMarkers[candHandle].patt_width, arMarkers[candHandle].oldTrans);
				}
				
				// Update reliability:
				arMarkers[candHandle].matchError = 1.0 - marker_info[k].cf;
				
				// Extract useful matrices for OpenGL:
				argConvGlpara(arMarkers[candHandle].oldTrans, xformMatrix);
				arglCameraViewRH(arMarkers[candHandle].oldTrans, ModelviewMatrixGL, view_scalefactor);

				if (verbosity > 4) printf("PsychCV-INFO: ARDetectMarkers: Marker %i with pattern id %i has matchError %f\n", candHandle, arMarkers[candHandle].marker.singleMarker, (float) arMarkers[candHandle].matchError);
			}			
		}

		// Assign final matchError
		PsychSetStructArrayDoubleElement("MatchError", i, arMarkers[candHandle].matchError, detectedMarkers);
	}
	
	// Ouput binarized debug image?
	if (verbosity > 6) {
		if (NULL != arImage) {
			// Copy ARToolkits internal binary image back to our output buffer:
			memcpy(arImagebuffer, arImage, imgWidth * imgHeight * AR_PIX_SIZE_DEFAULT);
		}
		else printf("PsychCV-DEBUG: arImage == NULL! arDebug = %i\n", arDebug);
	}

	// Ready.
	return(PsychError_none);
}

PsychError PSYCHCVARRenderImage(void)
{
 	static char useString[] = "PsychCV('ARRenderImage');";
	static char synopsisString[] = 
		"Render current scene video image from internal buffer into currently"
		"active OpenGL context. Perform proper camera distortion correction. \n"
		"The context must be in 3D mode, ie., this call "
		"must happen inside a Screen('BeginOpenGL'); - Screen('EndOpenGL') clause!\n";

	static char seeAlsoString[] = "";	 

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(0));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));	 // The maximum number of outputs

	if (!psychCVARInitialized) PsychErrorExitMsg(PsychError_user, "ARToolkit not yet initialized! Call PsychCV('ARInitialize') first and retry!");

	// ARGL helper library already initialized?
	if (!arglInitialized) {
		// No: Do it now, assuming the current OpenGL context is the proper
		// target rendering context:
		if ((gArglSettings = arglSetupForCurrentContext()) == NULL) {
			PsychErrorExitMsg(PsychError_user, "PsychCV-ERROR: ARGL libray setup failed: arglSetupForCurrentContext() returned an error!");
		}
		arglInitialized = TRUE;
	}

	// Perform scene render from internal image buffer, hopefully with the proper context bound:
	arglDispImage(arTrackBuffer, &cameraParams, view_scalefactor, gArglSettings);

	// Ready.
	return(PsychError_none);	
}

PsychError PSYCHCVARRenderSettings(void)
{
 	static char useString[] = "[scale, minDist, maxDist] = PsychCV('ARRenderSettings' [, scale][, minDist][, maxDist]);";
	static char synopsisString[] = 
		"Return current rendering parameters, optionally change rendering parameters.\n"
		"Both, for backdrop video image rendering in PsychCV('ARRenderImage'); and for "
		"computation and return of OpenGL compliant projection and modelview matrices "
		"in PsychCV('ARInitialize') and PsychCV('ARDetectMarkers'), some info about the "
		"relationship between AR toolkits distance units and OpenGL's distance units "
		"are needed. These settings are set to reasonable defaults at startup, but "
		"can be changed anytime with this subfunction. Following settings are available:\n"
		"'scale' scaling factor from AR's distance units [millimeters] OpenGL units.\n"
		"'minDist' Near clipping distance for OpenGL frustum computation - Affects projection matrices.\n"
		"'maxDist' Far clipping distance for OpenGL frustum computation - Affects projection matrices.\n"
		"The optionally returned 'glProjectionMatrix' reflects the changes in these parameters. \n";

	static char seeAlsoString[] = "";	 

	double			*projmatrixGL;

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(3));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(4));	 // The maximum number of outputs

	// Copy out old settings:
	PsychCopyOutDoubleArg(1, FALSE, view_scalefactor);
	PsychCopyOutDoubleArg(2, FALSE, view_distance_min);
	PsychCopyOutDoubleArg(3, FALSE, view_distance_max);

	// Copy in optional new settings:
	PsychCopyInDoubleArg(1, FALSE, &view_scalefactor);
	PsychCopyInDoubleArg(2, FALSE, &view_distance_min);
	PsychCopyInDoubleArg(3, FALSE, &view_distance_max);

	//  argDrawMode     = AR_DRAW_BY_TEXTURE_MAPPING;

	// Recompute GL_PROJECTION_MATRIX from new settings, if any:

	// Compute a suitable OpenGL projection matrix for these intrinsic
	// camera parameters: Return it as 4 x 4 matrix in 2nd optional
	// return argument:
	PsychAllocOutDoubleMatArg(4, FALSE, 4, 4, 1, &projmatrixGL);
	
	// Let ARToolkit do the actual job:
	arglCameraFrustumRH(&cameraParams, view_distance_min, view_distance_max, projmatrixGL);

	// Ready.
	return(PsychError_none);	
}

PsychError PSYCHCVARTrackerSettings(void)
{
 	static char useString[] = "[templateMatchingInColor, imageProcessingFullSized, imageProcessingIdeal, trackingWithPCA] = PsychCV('ARTrackerSettings' [, templateMatchingInColor][, imageProcessingFullSized][, imageProcessingIdeal][, trackingWithPCA]);";
	static char synopsisString[] = 
		"Return current tracker parameters, optionally change tracker parameters.\n"
		"These settings are set to reasonable defaults at startup, but can be changed "
		"anytime with this subfunction. Most of these settings define tradeoffs between "
		"computation time aka tracking speed and quality/robustness of tracking.\n"
		"Following settings are available:\n"
		"'templateMatchingInColor' 1 = Use color template matching. 0 = Use intensity only.\n"
		"'imageProcessingFullSized' 1 = Process full image. 0 = Only work on half-resolution image.\n"
		"'imageProcessingIdeal' 1 = Undistort image (camera undistortion) before processing. 0 = Work on input image as is.\n"
		"'trackingWithPCA' 1 = Use PCA for tracking (expensive), 0 = Use simpler strategy.\n";

	static char seeAlsoString[] = "";	 

	int	templateMatchingInColor, imageProcessingFullSized, imageProcessingIdeal, trackingWithPCA;

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(4));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(4));	 // The maximum number of outputs

	// Copy out old settings:
	PsychCopyOutDoubleArg(1, FALSE, (arTemplateMatchingMode == AR_TEMPLATE_MATCHING_COLOR) ? 1 : 0);
	PsychCopyOutDoubleArg(2, FALSE, (arImageProcMode == AR_IMAGE_PROC_IN_FULL) ? 1 : 0); 
	PsychCopyOutDoubleArg(3, FALSE, (arFittingMode == AR_FITTING_TO_IDEAL) ? 1 : 0);
	PsychCopyOutDoubleArg(4, FALSE, (arMatchingPCAMode == AR_MATCHING_WITH_PCA) ? 1 : 0);

	// Copy in optional new settings:
	PsychCopyInIntegerArg(1, FALSE, &templateMatchingInColor);
	arTemplateMatchingMode = (templateMatchingInColor) ? AR_TEMPLATE_MATCHING_COLOR : AR_TEMPLATE_MATCHING_BW;
	PsychCopyInIntegerArg(2, FALSE, &imageProcessingFullSized);
	arImageProcMode = (imageProcessingFullSized) ? AR_IMAGE_PROC_IN_FULL : AR_IMAGE_PROC_IN_HALF;
	PsychCopyInIntegerArg(3, FALSE, &imageProcessingIdeal);
	arFittingMode = (imageProcessingIdeal) ? AR_FITTING_TO_IDEAL : AR_FITTING_TO_INPUT;
	PsychCopyInIntegerArg(4, FALSE, &trackingWithPCA);
	arMatchingPCAMode = (trackingWithPCA) ? AR_MATCHING_WITH_PCA : AR_MATCHING_WITHOUT_PCA;

	// Ready.
	return(PsychError_none);	
}
