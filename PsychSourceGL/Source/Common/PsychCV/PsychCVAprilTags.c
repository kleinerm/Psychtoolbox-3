/*
    PsychToolbox-3/PsychSourceGL/Source/Common/PsychCV/PsychCVAprilTags.c

    PLATFORMS:  All

    AUTHORS:

    Mario Kleiner     mk  mario.kleiner.de@gmail.com

    HISTORY:

    12.03.2024        mk  Wrote it.

    DESCRIPTION:

    Contains the implementation of our interface to libapriltags.
*/

#ifdef PSYCHCV_USE_APRILTAGS

// Include libapriltags specific routines and includes, but also PsychCV.h
#include "PsychCVAprilTags.h"
#include "apriltag.h"
#include "apriltag_pose.h"
#include "common/image_u8.h"
#include "common/image_u8x3.h"
#include "common/image_u8x4.h"
#include "common/zarray.h"

#include "tag36h11.h"
#include "tag25h9.h"
#include "tag16h5.h"
#include "tagCircle21h7.h"
#include "tagCircle49h12.h"
#include "tagCustom48h12.h"
#include "tagStandard41h12.h"
#include "tagStandard52h13.h"
#include "tagCustom48h12.h"

#define April_PIX_SIZE_DEFAULT  1

#define April_PIXEL_FORMAT_MONO 6
#define April_PIXEL_FORMAT_RGB  1
#define April_PIXEL_FORMAT_BGR  2
#define April_PIXEL_FORMAT_BGRA 4
#define April_PIXEL_FORMAT_ARGB 7

#define April_DEFAULT_PIXEL_FORMAT April_PIXEL_FORMAT_MONO

// Declare variables local to this file.

// Level of verbosity: Defined in PsychCV.c, read-only accessed here:
extern unsigned int verbosity;

// Status: PsychCVAprilToolkit Initialized or not?
static psych_bool psychCVAprilInitialized = FALSE;

// Pointer to input image buffer:
static image_u8_t* arImagebuffer = NULL;

// Pointer to processing image buffer:
static image_u8_t* arTrackBuffer = NULL;

// Fornat of image buffer:
static int imgWidth = 0;
static int imgHeight = 0;
static int imgChannels = 0;
static int imgFormat = 0;

static apriltag_family_t *tagFamily = NULL;
static apriltag_detector_t *tagDetector = NULL;

// Clip near and clip far distance for frustums computed by apriltags
// when returning a gl projection matrix:
static double view_distance_min = 0.1;
static double view_distance_max = 1000.0;

// Camera intrinsic parameters for 3D pose reconstruction and rendering:
static double cam_cx = 1, cam_cy = 1, cam_fx = 1, cam_fy = 1;
static double tagSize = 1;

// Internal helper: Perform image data conversion if required:
void PsychCVAprilConvertInputImage(void)
{
    psych_uint8* src = arImagebuffer->buf;
    psych_uint8* dst = arTrackBuffer->buf;
    int i, count, gray;

    // Conversion needed at all? If input matches requested channelcount and image format
    // of apriltags, then there ain't nothing to do and we can return immediately:
    if (imgChannels == April_PIX_SIZE_DEFAULT && imgFormat == April_DEFAULT_PIXEL_FORMAT)
        return;

    // Identical buffers? Then nothing to do:
    if (src == dst)
        return;

    count = imgWidth * imgHeight;

    if (imgChannels == 3) {
        // RGB --> MONO: All our video capture engines deliver RGB if 3 channel data is requested:
        for (i = 0; i < count; i++) {
            // Sum first 3 bytes (RGB part), use G twice to weight it higher:
            gray = *(src++);                            // B
            gray+= *(src);                              // G
            gray+= *(src++);                            // G
            gray+= *(src++);                            // R

            // Write out "weighted sum" with gray value:
            *(dst++) = gray >> 2;
        }

        // Done:
        return;
    }

    if (imgChannels == 4) {
        // ARGB or BGRA --> MONO: Source depends on input image format
        if (imgFormat == April_PIXEL_FORMAT_ARGB) {
            for (i = 0; i < count; i++) {
                // Sum first 3 bytes (RGB part), use G twice to weight it higher:
                gray = *(src++);                            // B
                gray+= *(src);                              // G
                gray+= *(src++);                            // G
                gray+= *(src++);                            // R

                // Skip last byte (the A part of ARGB):
                src++;

                // Write out "weighted sum" with gray value:
                *(dst++) = gray >> 2;
            }

            // Done:
            return;
        }

        if (imgFormat == April_PIXEL_FORMAT_BGRA) {
            for (i = 0; i < count; i++) {
                // Skip first byte (the A part of BGRA):
                src++;

                // Sum first 3 bytes (RGB part), use G twice to weight it higher:
                gray = *(src++);                            // R
                gray+= *(src);                              // G
                gray+= *(src++);                            // G
                gray+= *(src++);                            // B

                // Write out "weighted sum" with gray value:
                *(dst++) = gray >> 2;
            }

            // Done:
            return;
        }
    }

    // If we reach this point, then some unsupported input -> output conversion was requested:
    PsychErrorExitMsg(PsychError_user,
                      "Unknown or unsupported input image format settings 'imgChannels' and/or 'imgFormat' encountered! Check your settings in PsychCV('AprilInitialize')!");

    return;
}

void PsychCVAprilExit(void)
{
    // Perform Shutdown operation, if needed. Called from PsychCVExit routine
    // at PsychCV shutdown/flush time, or explicitely via subfunction 'AprilShutdown':
    if (psychCVAprilInitialized) {
        // Release buffer memory, if any:
        if ((arTrackBuffer) && (arTrackBuffer != arImagebuffer))
            image_u8_destroy(arTrackBuffer);

        arTrackBuffer = NULL;

        if (arImagebuffer) {
            switch(imgChannels) {
                case 1:
                    image_u8_destroy(arImagebuffer);
                    break;

                case 3:
                    image_u8x3_destroy((image_u8x3_t *) arImagebuffer);
                    break;

                case 4:
                    image_u8x4_destroy((image_u8x4_t *) arImagebuffer);
                    break;
            }
        }

        arImagebuffer = NULL;

        // Destroy our detector instance:
        if (tagDetector)
            apriltag_detector_destroy(tagDetector);

        tagDetector = NULL;

        // Destroy our tag family instance:
        if (tagFamily) {
            if (!strcmp(tagFamily->name, "tag36h11")) {
                tag36h11_destroy(tagFamily);
            } else if (!strcmp(tagFamily->name, "tag25h9")) {
                tag25h9_destroy(tagFamily);
            } else if (!strcmp(tagFamily->name, "tag16h5")) {
                tag16h5_destroy(tagFamily);
            } else if (!strcmp(tagFamily->name, "tagCircle21h7")) {
                tagCircle21h7_destroy(tagFamily);
            } else if (!strcmp(tagFamily->name, "tagCircle49h12")) {
                tagCircle49h12_destroy(tagFamily);
            } else if (!strcmp(tagFamily->name, "tagStandard41h12")) {
                tagStandard41h12_destroy(tagFamily);
            } else if (!strcmp(tagFamily->name, "tagStandard52h13")) {
                tagStandard52h13_destroy(tagFamily);
            } else if (!strcmp(tagFamily->name, "tagCustom48h12")) {
                tagCustom48h12_destroy(tagFamily);
            }
            else
                printf("PsychCV:CRITICAL: In shutdown, unknown tagFamily?!? Can't destroy - memory leak!\n");
        }

        tagFamily = NULL;

        psychCVAprilInitialized = FALSE;
    }
}

/* PsychCV('AprilInitialize') - Initialize a new tracking session with apriltags:
 *
 * No handle is returned, as we currently only allow one instance of the apriltags.
 *
 */
PsychError PSYCHCVAprilInitialize(void)
{
    static char useString[] =
    "[inputImageMemBuffer] = PsychCV('AprilInitialize', tagFamilyName, imgWidth, imgHeight, imgChannels [, imgFormat]);";
    //1                                                 1              2         3          4              5

    static char synopsisString[] =
        "Initialize apriltag prior to first use.\n\n"
        "Apriltag markers are loaded for the given apriltag family 'tagFamilyName'.\n"
        "The following tag families are currently supported:\n"
        "    tag36h11\n"
        "    tag25h9\n"
        "    tag16h5\n"
        "    tagCircle21h7\n"
        "    tagCircle49h12\n"
        "    tagCustom48h12\n"
        "    tagStandard41h12\n"
        "    tagStandard52h13\n"
        " \n"
        "Internal video image memory buffers are set up for input images of size "
        "'imgWidth' x 'imgHeight' pixels, with 'imgChannels' mono or color channels "
        "(1 MONO, 3 RGB, or 4 RGBA are valid settings, but 1 for MONO is the most "
        "efficient choice for minimal computation time). For 3 or 4 channels, the input "
        "color format 'imgFormat' (or a default setting if 'imgFormat' is omitted) defines "
        "color channel ordering for the input pixel bytes. 'imgFormat' can be one of:\n"
        "RGB = 1, BGR = 2, BGRA = 4 (default for 4 channels), ARGB = 7, MONO = 6. Other "
        "pixel formats are not supported for input images, but these are the ones provided "
        "by Psychtoolbox's various video capture engines for different video sources and "
        "settings. For 1 channel mono/grayscale or 3 channel RGB color content, you don't "
        "need to specify 'imgFormat' as the chosen default will always work, but for "
        "4 channel content with alpha channel, you may have to specify 'imgFormat' if "
        "your machine does not return BGRA ordered pixels, but ARGB ordered pixels, or "
        "marker detection on 4 channel content may fail or perform poorly.\n"
        "Then apriltags is initialized, and a memory buffer handle 'inputImageMemBuffer' "
        "to the internal video memory input buffer is returned.\n\n"
        "You should pass this handle to Psychtoolbox functions for videocapture "
        "to acquire video images from the scene containing the tags, and to store that "
        "input image inside PsychCV's video buffer.\n\n"
        "After this step, you can commence the actual tracking operations by calls to "
        "Screen()'s video capture engine and the PsychCV('AprilDetectMarkers', ...); "
        "subfunction.\n";
    static char seeAlsoString[] = "AprilShutdown AprilDetectMarkers AprilSettings April3DSettings";

    char* tagFamilyName;

    // Setup online help:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    PsychErrorExit(PsychCapNumInputArgs(5));        // The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(4));    // The required number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(1));       // The maximum number of outputs

    if (psychCVAprilInitialized)
        PsychErrorExitMsg(PsychError_user, "apriltags already initialized! Call PsychCV('AprilShutdown') first and retry!");

    // Get name of the apriltag family to use:
    PsychAllocInCharArg(1, TRUE, &tagFamilyName);

    PsychCopyInIntegerArg(2, TRUE, &imgWidth);
    if (imgWidth < 1 || imgWidth > 32768)
        PsychErrorExitMsg(PsychError_user, "Invalid image width provided. Must be between 1 and 32768 pixels!");

    PsychCopyInIntegerArg(3, TRUE, &imgHeight);
    if (imgHeight < 1 || imgHeight > 32768)
        PsychErrorExitMsg(PsychError_user, "Invalid image height provided. Must be between 1 and 32768 pixels!");

    PsychCopyInIntegerArg(4, TRUE, &imgChannels);
    if (imgChannels < 1 || imgChannels > 4 || imgChannels == 2)
        PsychErrorExitMsg(PsychError_user, "Invalid imgChannels provided. Must be 1, 3 or 4!");

    // Setup default imgFormat, depending on number of input channels:
    imgFormat = April_DEFAULT_PIXEL_FORMAT;
    if (imgChannels == 1) imgFormat = April_PIXEL_FORMAT_MONO;
    if (imgChannels == 3) imgFormat = April_PIXEL_FORMAT_RGB;
    if (imgChannels == 4) imgFormat = April_PIXEL_FORMAT_BGRA;

    // Get optional imgFormat:
    PsychCopyInIntegerArg(5, kPsychArgOptional, &imgFormat);
    if (imgFormat < 0 || (imgChannels == 1 && imgFormat != April_PIXEL_FORMAT_MONO) ||
        (imgChannels == 3 && imgFormat != April_PIXEL_FORMAT_RGB && imgFormat != April_PIXEL_FORMAT_BGR) ||
        (imgChannels == 4 && imgFormat != April_PIXEL_FORMAT_BGRA && imgFormat != April_PIXEL_FORMAT_ARGB))
        PsychErrorExitMsg(PsychError_user, "Invalid image format provided!");

    // Load and create the requested tag family:
    if (!strcmp(tagFamilyName, "tag36h11")) {
        tagFamily = tag36h11_create();
    } else if (!strcmp(tagFamilyName, "tag25h9")) {
        tagFamily = tag25h9_create();
    } else if (!strcmp(tagFamilyName, "tag16h5")) {
        tagFamily = tag16h5_create();
    } else if (!strcmp(tagFamilyName, "tagCircle21h7")) {
        tagFamily = tagCircle21h7_create();
    } else if (!strcmp(tagFamilyName, "tagCircle49h12")) {
        tagFamily = tagCircle49h12_create();
    } else if (!strcmp(tagFamilyName, "tagStandard41h12")) {
        tagFamily = tagStandard41h12_create();
    } else if (!strcmp(tagFamilyName, "tagStandard52h13")) {
        tagFamily = tagStandard52h13_create();
    } else if (!strcmp(tagFamilyName, "tagCustom48h12")) {
        tagFamily = tagCustom48h12_create();
    } else {
        printf("PsychCV-ERROR: Unrecognized tag family name '%s'. Use e.g. \"tag36h11\".\n", tagFamilyName);
        PsychErrorExitMsg(PsychError_user, "Startup failed due to invalid/unsupported tagFamilyName.");
    }

    if (tagFamily == NULL)
        PsychErrorExitMsg(PsychError_user, "Startup failed due to failure to create requested tag family.");

    // Create apriltag detector and assign our selected tag family:
    tagDetector = apriltag_detector_create();
    if (tagDetector == NULL) {
        // Note: Can't happen, as apriltag_detector_create can not fail without crashing the whole app.
        psychCVAprilInitialized = TRUE;
        PsychCVAprilExit();
        PsychErrorExitMsg(PsychError_user, "Startup failed due to failure to create apriltag detector.");
    }

    apriltag_detector_add_family(tagDetector, tagFamily);

    // Allocate internal memory buffer of sufficient size:
    switch(imgChannels) {
        case 1:
            arImagebuffer = image_u8_create_alignment(imgWidth, imgHeight, 1);
            break;

        case 3:
            arImagebuffer = (image_u8_t *) image_u8x3_create_alignment(imgWidth, imgHeight, 3);
            break;

        case 4:
            arImagebuffer = (image_u8_t *) image_u8x4_create_alignment(imgWidth, imgHeight, 4);
            break;
    }

    if (NULL == arImagebuffer) {
        psychCVAprilInitialized = TRUE;
        PsychCVAprilExit();
        PsychErrorExitMsg(PsychError_outofMemory, "Out of memory when trying to initialize apriltags subsystem!");
    }

    // Return double-encoded void* memory pointer to video image input buffer:
    PsychCopyOutDoubleArg(1, kPsychArgRequired, PsychPtrToDouble(arImagebuffer->buf));

    // Check if we need conversion for image data:
    if (April_PIX_SIZE_DEFAULT != imgChannels) {
        // Need conversion and therefore intermediate conversion buffer:
        arTrackBuffer = image_u8_create_alignment(imgWidth, imgHeight, 1);

        if (NULL == arTrackBuffer) {
            psychCVAprilInitialized = TRUE;
            PsychCVAprilExit();
            PsychErrorExitMsg(PsychError_outofMemory, "Out of memory when trying to initialize apriltags subsystem!");
        }
    }
    else {
        arTrackBuffer = arImagebuffer;
    }

    // We're online!
    psychCVAprilInitialized = TRUE;

    return(PsychError_none);
}

PsychError PSYCHCVAprilShutdown(void)
{
    static char useString[] = "PsychCV('AprilShutdown');";
    static char synopsisString[] =
        "Shut down apriltag after use, release all resources.\n"
        "The memory buffer handle 'inputImageMemBuffer' returned by a prior call "
        "to inputImageMemBuffer = PsychCV('AprilInitialize', ...); will be invalid after "
        "this shutdown call and must not be used anymore, or Psychtoolbox will crash!\n";
    static char seeAlsoString[] = "AprilInitialize";

    // Setup online help:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    PsychErrorExit(PsychCapNumInputArgs(0));        // The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(0));    // The required number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(0));       // The maximum number of outputs

    // Cleanup and shutdown:
    PsychCVAprilExit();

    return(PsychError_none);
}

PsychError PSYCHCVAprilDetectMarkers(void)
{
    static char useString[] = "[detectedMarkers] = PsychCV('AprilDetectMarkers'[, markerSubset=all][, infoType=all]);";
    //                          1                                                 1                   2
    static char synopsisString[] =
        "Detect apriltags in the current video image, return information about them.\n\n"
        "Analyzes the current video image, stored in the internal input image buffer, and "
        "tries to detect the apriltag markers in them. For all detected tags, their 2D center, "
        "their 2D corners, and their 3D position and orientation is computed, using the provided "
        "camera calibration. The return argument 'detectedMarkers' is an array of structs, with "
        "one struct for each successfully detected tag. The struct contains info about the "
        "identity of the tag, confidence values for the reliability of detection, and the "
        "estimated 6-DoF 3D position and 3D pose of the marker tag in 3D space, relative "
        "to the cameras reference frame and origin. Please note that, in general, 3D pose "
        "estimates are not as reliable and accurate as the 2D detection of markers. Furthermore, "
        "the 3D orientation of the marker is way less well defined and accurate than the 3D position "
        "of the markers center. Often the estimated 3D orientation may be outright rubbish!\n"
        "You can use PsychCV('AprilSettings'); to tune various parameters related to the 2D marker "
        "detection, including the use of multiple processing threads for higher performance.\n"
        "For 6-DoF 3D estimation, you need to provide camera intrinsic parameters and the size of "
        "the tags via PsychCV('April3DSettings').\n"
        "If you don't want to detect all tags, but only a subset, then pass a list of candidate "
        "tag id's via the list 'markerSubset', to reduce your codes complexity and computation time.\n"
        "To further reduce computation time, you can ask for only a subset of information by providing 'infoType'. "
        "By default all information is returned at highest quality and robustness with longest computation time:\n"
        "- 2D marker detection data is always returned.\n"
        "- A value of +1 will return 3D pose.\n"
        "Omitting the value +1 from 'infoType' will avoid 3D pose estimation.\n\n"
        "The returned structs contain the following fields:\n"
        "'Id' The decoded apriltag id. Hamming code error correction is used for decoding.\n"
        "'MatchQuality' A measure of the quality of the binary decoding process. This is what "
        "the apriltag library calls decision_margin. Higher numbers roughly indicate better "
        "decodes. It is a reasonable measure of detection quality only for small tags, mostly "
        "meaningless for bigger tags.\n"
        "'HammingErrorBits' Number of error bits corrected. Smaller numbers are better.\n"
        "'Corners2D' A 2-by-4 matrix with the 2D pixel coordinates of the detected corners "
        "of the tag in the input image, each column representing one corner [x ; y]. These "
        "always wrap counter-clock wise around the tag.\n"
        "'Center2D' A vector with the 2D pixel coordinates of the estimated center of the "
        "tag in the input image.\n"
        "'PoseError' If 3D pose estimation was used, the object space error of the returned pose.\n"
        "'T' A 3-component [x ; y; z] translation vector which encodes the estimated 3D location "
        "of the center of the tag in space, in units of meters, relative to the origin of the camera.\n"
        "'R' A 3x3 rotation matrix, encoding the estimated pose of the tag, relative to the cameras "
        "reference frame. Convention is that the tag itself lies in the x-y plane of its local reference "
        "frame, and the positive z-axis sticks out of the tags surface like a surface normal vector.\n"
        "'TransformMatrix' A 4x4 transformation matrix representing position and orientation all in one, "
        "for convenience. Simply the product TransformMatrix = T * R, extended to a 4x4 format. "
        "This represents pose relative to the cameras origin, x-axis to the right, y-axis down, z-axis "
        "along the looking direction aka optical axis.\n"
        "'ModelViewMatrix' A 4x4 RHS transformation matrix, directly usable for 3D OpenGL rendering of "
        "objects in the tags local reference frame. It can be used directly as GL_MODELVIEW_MATRIX "
        "for rendering 3D content on top of the tag in the video image, or right-multiplied to the "
        "active GL_MODELVIEW_MATRIX to represent the tags 6 DoF pose relative to the 3D OpenGL cameras "
        "origin. You need to use the GL_PROJECTION_MATRIX returned by matrix = PsychCV('April3DSettings'); "
        "for rendering superimposed to images from the camera that captured the april tags. This matrix "
        "is a rotated version of 'TransformMatrix', rotated 180 degrees around the x-axis for OpenGL "
        "compatibility, as apriltag has x-axis to the right, y-axis down, z-axis along optical looking "
        "direction axis, whereas OpenGL has its x-axis to the right, y-axis up, and the negative z-axis "
        "along optical looking direction axis / viewing direction, ie. 180 degrees rotated.\n";

    static char seeAlsoString[] = "AprilInitialize AprilSettings April3DSettings";
    double* markerSubset = NULL;
    int i, j, m, n, p;
    int infoType;
    int candHandle;
    int marker_num;
    zarray_t *marker_info;
    apriltag_detection_t *detcand;
    apriltag_detection_t *detected;
    apriltag_detection_info_t detinfo;
    apriltag_pose_t pose;
    double matchQuality;
    double poseError;
    int hammingErrorBits;
    double* xformMatrix;
    double* modelViewMatrixGL;
    double* R;
    double* T;
    double* center2D;
    double* corners2D;

    PsychGenericScriptType *detectedMarkers, *myMatrix;
    const char *FieldNames[] = { "Id", "MatchQuality", "HammingErrorBits", "PoseError", "Center2D", "Corners2D", "R", "T",
                                 "TransformMatrix", "ModelViewMatrix"};

    // Setup online help:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    PsychErrorExit(PsychCapNumInputArgs(2));        // The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(0));    // The required number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(1));       // The maximum number of outputs

    if (!psychCVAprilInitialized)
        PsychErrorExitMsg(PsychError_user, "apriltags not yet initialized! Call PsychCV('AprilInitialize') first and retry!");

    if (tagFamily->ncodes < 1)
        PsychErrorExitMsg(PsychError_user, "No markers from tag family loaded for detection!");

    // Perform image data conversion if required:
    PsychCVAprilConvertInputImage();

    // Get optional markerSubset list:
    if (PsychAllocInDoubleMatArg(1, FALSE, &m, &n, &p, &markerSubset)) {
        // List provided: Sanity check!
        if (p != 1 || (m * n < 1))
            PsychErrorExitMsg(PsychError_user, "Invalid 'markerSubset' specified: Must be a 1D or 2D vector or matrix with handles!");

        n = m * n;

        // Validate...
        for (i = 0; i < n; i++) {
            if (markerSubset[i] < 0 || markerSubset[i] >= tagFamily->ncodes) {
                printf("PsychCV-ERROR: Invalid markerhandle %i passed in 'markerSubset' argument! No such marker available!\n", markerSubset[i]);
                PsychErrorExitMsg(PsychError_user, "Invalid 'markerSubset' specified: Must be a 1D or 2D vector or matrix with handles!");
            }
        }
    }
    else {
        // No list provided: Create our default match list which contains all current markers:
        markerSubset = (double*) PsychMallocTemp(sizeof(double) * tagFamily->ncodes);
        n = tagFamily->ncodes;
        for (i = 0; i < n; i++)
            markerSubset[i] = (double) i;
    }

    // Get optional infoType flag:
    infoType = 0xffffff & 0x1;
    PsychCopyInIntegerArg(2, FALSE, &infoType);
    if (infoType < 0 || infoType > 1)
        PsychErrorExitMsg(PsychError_user, "Invalid 'infoType' provided. Must be positive integer <= 1!");

    // Ok, we got the user arguments. Let's do the actual detection:
    tagDetector->debug = (verbosity > 5) ? 1 : 0;
    marker_info = apriltag_detector_detect(tagDetector, arTrackBuffer);
    marker_num = zarray_size(marker_info);

    if (verbosity > 4) {
        printf("PsychCV-INFO: AprilDetectMarkers: Detected %i markers in image.\n", marker_num);

        for (i = 0; i < marker_num; i++) {
            apriltag_detection_t *det;
            zarray_get(marker_info, i, &det);

            printf("Marker %3d: id (%2dx%2d)-%-4d, hamming %d, margin %8.3f\n", i, det->family->nbits, det->family->h, det->id,
                   det->hamming, det->decision_margin);
        }

        printf("PsychCV-INFO: AprilDetectMarkers: -----------------------------\n");
    }

    // Create our fixed size return array with one slot per requested candidate marker:
    PsychAllocOutStructArray(1, TRUE, n, 10, FieldNames, &detectedMarkers);

    // Process all our 'n' candidate markers against detected markers:
    for (i = 0; i < n; i++) {
        // Retrieve handle of i'th candidate:
        candHandle = (int) markerSubset[i];

        // Assign marker id to output slot:
        PsychSetStructArrayDoubleElement("Id", i, candHandle, detectedMarkers);

        R = NULL;
        PsychAllocateNativeDoubleMat(3, 3, 1, &R, &myMatrix);
        PsychSetStructArrayNativeElement("R", i, myMatrix, detectedMarkers);
        memset(R, 0, sizeof(double) * 3 * 3);

        T = NULL;
        PsychAllocateNativeDoubleMat(3, 1, 1, &T, &myMatrix);
        PsychSetStructArrayNativeElement("T", i, myMatrix, detectedMarkers);
        memset(T, 0, sizeof(double) * 3 * 1);

        // Allocate and assign 4x4 xform matrix: xformmatrix must be filled with the 16 values
        // of the actual matrix later on, we init it with all-zeros:
        xformMatrix = NULL;
        PsychAllocateNativeDoubleMat(4, 4, 1, &xformMatrix, &myMatrix);
        PsychSetStructArrayNativeElement("TransformMatrix", i, myMatrix, detectedMarkers);
        memset(xformMatrix, 0, sizeof(double) * 4 * 4);

        // Allocate and assign 4x4 modelViewMatrixGL matrix: modelViewMatrixGL must be filled with
        // the 16 values of the actual matrix later on, we init it with all-zeros:
        modelViewMatrixGL = NULL;
        PsychAllocateNativeDoubleMat(4, 4, 1, &modelViewMatrixGL, &myMatrix);
        PsychSetStructArrayNativeElement("ModelViewMatrix", i, myMatrix, detectedMarkers);
        memset(modelViewMatrixGL, 0, sizeof(double) * 4 * 4);

        center2D = NULL;
        PsychAllocateNativeDoubleMat(2, 1, 1, &center2D, &myMatrix);
        PsychSetStructArrayNativeElement("Center2D", i, myMatrix, detectedMarkers);
        memset(center2D, 0, sizeof(double) * 2);

        corners2D = NULL;
        PsychAllocateNativeDoubleMat(2, 4, 1, &corners2D, &myMatrix);
        PsychSetStructArrayNativeElement("Corners2D", i, myMatrix, detectedMarkers);
        memset(corners2D, 0, sizeof(double) * 2 * 4);

        // Search for best matching pattern:
        detected = NULL;
        for (j = 0; j < marker_num; j++) {
            zarray_get(marker_info, j, &detcand);
            if ((candHandle == detcand->id) && (detected == NULL))
                detected = detcand;
        }

        matchQuality = 0;
        hammingErrorBits = 1000;
        if (detected) {
            matchQuality = detected->decision_margin;
            hammingErrorBits = detected->hamming;

            // Store detected 2D [x ; y] image location of the tag center:
            center2D[0] = detected->c[0];
            center2D[1] = detected->c[1];

            // Store detected 2D [x ; y] image locations of the tags corners
            // in a counter-clock wise wrapping around the tag:
            corners2D[0] = detected->p[0][0];
            corners2D[1] = detected->p[0][1];
            corners2D[2] = detected->p[1][0];
            corners2D[3] = detected->p[1][1];
            corners2D[4] = detected->p[2][0];
            corners2D[5] = detected->p[2][1];
            corners2D[6] = detected->p[3][0];
            corners2D[7] = detected->p[3][1];

            poseError = 0;
            if (infoType & 0x1) {
                // Compute pose:
                detinfo.det = detected;
                detinfo.tagsize = tagSize;  // In meters.
                detinfo.cx = cam_cx;        // In pixels.
                detinfo.cy = cam_cy;        // In pixels.
                detinfo.fx = cam_fx;        // In pixels.
                detinfo.fy = cam_fy;        // In pixels.

                // Estimate 3D pose from monocular video, ie. 6 DoF position and orientation:
                poseError = estimate_tag_pose(&detinfo, &pose);

                // Return translation column vector:
                T[0] = MATD_EL(pose.t, 0, 0);
                T[1] = MATD_EL(pose.t, 1, 0);
                T[2] = MATD_EL(pose.t, 2, 0);

                // Return 3x3 rotation matrix:
                j=0;
                R[j++] = MATD_EL(pose.R, 0, 0);
                R[j++] = MATD_EL(pose.R, 1, 0);
                R[j++] = MATD_EL(pose.R, 2, 0);

                R[j++] = MATD_EL(pose.R, 0, 1);
                R[j++] = MATD_EL(pose.R, 1, 1);
                R[j++] = MATD_EL(pose.R, 2, 1);

                R[j++] = MATD_EL(pose.R, 0, 2);
                R[j++] = MATD_EL(pose.R, 1, 2);
                R[j++] = MATD_EL(pose.R, 2, 2);

                // Return 4x4 pose xformMatrix as T * R
                // Paste R into top 3x3, T as 4th column,
                // add a 4th [0,0,0,1] row to complete.
                j=0;
                xformMatrix[j++] = MATD_EL(pose.R, 0, 0);
                xformMatrix[j++] = MATD_EL(pose.R, 1, 0);
                xformMatrix[j++] = MATD_EL(pose.R, 2, 0);
                xformMatrix[j++] = 0.0;

                xformMatrix[j++] = MATD_EL(pose.R, 0, 1);
                xformMatrix[j++] = MATD_EL(pose.R, 1, 1);
                xformMatrix[j++] = MATD_EL(pose.R, 2, 1);
                xformMatrix[j++] = 0.0;

                xformMatrix[j++] = MATD_EL(pose.R, 0, 2);
                xformMatrix[j++] = MATD_EL(pose.R, 1, 2);
                xformMatrix[j++] = MATD_EL(pose.R, 2, 2);
                xformMatrix[j++] = 0.0;

                xformMatrix[j++] = T[0];
                xformMatrix[j++] = T[1];
                xformMatrix[j++] = T[2];
                xformMatrix[j++] = 1.0;

                // Return 4x4 pose modelViewMatrixGL as a tweaked
                // version of xformMatrix aka 'TransformMatrix',
                // for direct use for OpenGL rendering, ie. either
                // to load, as or post-multiply / right-multiply
                // with the GL_MODELVIEW_MATRIX, so tracked objects
                // are rendered at the proper place, superimposed
                // onto video from the tracking camera, if also the
                // GL_PROJECTION_MATRIX returned by us is used.
                //
                // Compared to xformMatrix, rows 1 and 2 are multiplied
                // by -1, flipping all signs for the y and z axis, rotating
                // the reference frame by 180 degrees around the x axis, to
                // convert from apriltag y-down, z-out of camera convention
                // to OpenGL y-up, negative z-out of camera convention:
                j=0;
                modelViewMatrixGL[j++] = MATD_EL(pose.R, 0, 0);
                modelViewMatrixGL[j++] = -MATD_EL(pose.R, 1, 0);
                modelViewMatrixGL[j++] = -MATD_EL(pose.R, 2, 0);
                modelViewMatrixGL[j++] = 0.0;

                modelViewMatrixGL[j++] = MATD_EL(pose.R, 0, 1);
                modelViewMatrixGL[j++] = -MATD_EL(pose.R, 1, 1);
                modelViewMatrixGL[j++] = -MATD_EL(pose.R, 2, 1);
                modelViewMatrixGL[j++] = 0.0;

                modelViewMatrixGL[j++] = MATD_EL(pose.R, 0, 2);
                modelViewMatrixGL[j++] = -MATD_EL(pose.R, 1, 2);
                modelViewMatrixGL[j++] = -MATD_EL(pose.R, 2, 2);
                modelViewMatrixGL[j++] = 0.0;

                modelViewMatrixGL[j++] = T[0];
                modelViewMatrixGL[j++] = -T[1];
                modelViewMatrixGL[j++] = -T[2];
                modelViewMatrixGL[j++] = 1.0;

                // Release matrices: Should do matd_destroy(), but this gives undefined symbol error
                // on Ubuntu 20.04, so just do what that function does anyway to get on with life:
                free(pose.R);
                free(pose.t);
            }

            if (verbosity > 4)
                printf("PsychCV-INFO: AprilDetectMarkers: Marker %i has match quality %f. Hamming error %i. Pose error %f.\n",
                       candHandle, matchQuality, hammingErrorBits, poseError);
        }

        // Assign final matchQuality:
        PsychSetStructArrayDoubleElement("MatchQuality", i, matchQuality, detectedMarkers);
        PsychSetStructArrayDoubleElement("HammingErrorBits", i, hammingErrorBits, detectedMarkers);
        PsychSetStructArrayDoubleElement("PoseError", i, poseError, detectedMarkers);
    }

    // Release array of detected markers and all single marker elements:
    apriltag_detections_destroy(marker_info);

    return(PsychError_none);
}

PsychError PSYCHCVApril3DSettings(void)
{
    static char useString[] = "[glProjectionMatrix, camCalib, tagSize, minD, maxD] = PsychCV('April3DSettings' [, camCalib][, tagSize][, minD][, maxD]);";
    //                          1                   2         3        4     5                                    1           2          3       4
    static char synopsisString[] =
        "Return current 3D marker pose reconstruction and 3D rendering parameters, optionally change them.\n"
        "For 6-DoF 3D marker tag pose computation, and for return of OpenGL compliant rendering matrices, "
        "information about the physical size of april tag markers, OpenGL near and far clipping planes, and"
        "the intrinsic optical parameters of the camera that captures the marker images are needed.\n"
        "These parameters can be changed anytime with this subfunction. Following settings are available:\n"
        "'camCalib' Intrinsic camera parameters vector: [cx, cy, fx, fy]. (cx,cy) is sensor center in pixels, "
        "(fx,fy) is focal length in pixels.\n"
        "'tagSize' Size of an April tag in meters, ie. length of one side of the square tag.\n"
        "'minD' Near clipping distance for OpenGL frustum computation - Affects 'glProjectionMatrix' matrix only.\n"
        "'maxD' Far clipping distance for OpenGL frustum computation - Affects 'glProjectionMatrix' matrix only.\n\n"
        "The optionally returned 'glProjectionMatrix', based on these parameters, can be used as OpenGL "
        "GL_PROJECTION_MATRIX to render 3D objects superimposed and aligned with the markers in the video input "
        "image from the camera, for debugging, diagnostics, or AR / mixed reality applications.\n";
    static char seeAlsoString[] = "AprilDetectMarkers";

    double *pGL;
    double *outCamCalib;
    double *inCamCalib;
    int i, m, n, p;

    // Setup online help:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    PsychErrorExit(PsychCapNumInputArgs(4));        // The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(0));    // The required number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(5));       // The maximum number of outputs

    // Copy out old settings:
    PsychAllocOutDoubleMatArg(2, FALSE, 1, 4, 1, &outCamCalib);
    outCamCalib[0] = cam_cx;
    outCamCalib[1] = cam_cy;
    outCamCalib[2] = cam_fx;
    outCamCalib[3] = cam_fy;

    PsychCopyOutDoubleArg(3, FALSE, tagSize);
    PsychCopyOutDoubleArg(4, FALSE, view_distance_min);
    PsychCopyOutDoubleArg(5, FALSE, view_distance_max);

    // Copy in optional new settings:
    if (PsychAllocInDoubleMatArg(1, FALSE, &m, &n, &p, &inCamCalib)) {
        if (p != 1 || m * n != 4)
            PsychErrorExitMsg(PsychError_user, "Invalid 'camCalib' parameter. Must be a 4 element vector.");

        if (inCamCalib[0] < 1 || inCamCalib[1] < 1)
            PsychErrorExitMsg(PsychError_user, "Invalid 'camCalib' parameter. cx or cy < 1, this is forbidden.");

        if (inCamCalib[2] <= 0 || inCamCalib[3] <= 0)
            PsychErrorExitMsg(PsychError_user, "Invalid 'camCalib' parameter. fx or fy <= 0, this is forbidden.");

        cam_cx = inCamCalib[0];
        cam_cy = inCamCalib[1];
        cam_fx = inCamCalib[2];
        cam_fy = inCamCalib[3];
    }

    PsychCopyInDoubleArg(2, FALSE, &tagSize);
    if (tagSize <= 0)
        PsychErrorExitMsg(PsychError_user, "Invalid 'tagSize': Must be greater than zero.");

    PsychCopyInDoubleArg(3, FALSE, &view_distance_min);
    if (view_distance_min <= 0)
        PsychErrorExitMsg(PsychError_user, "Invalid 'minD': Must be greater than zero.");

    PsychCopyInDoubleArg(4, FALSE, &view_distance_max);
    if (view_distance_max <= 0)
        PsychErrorExitMsg(PsychError_user, "Invalid 'maxD': Must be greater than zero.");

    if (view_distance_max <= view_distance_min)
        PsychErrorExitMsg(PsychError_user, "Invalid 'minD' or 'maxD': maxD must be greater than minD.");

    // Recompute and return GL_PROJECTION_MATRIX from new settings, if any:
    PsychAllocOutDoubleMatArg(1, FALSE, 4, 4, 1, &pGL);

    // Derived from http://kgeorge.github.io/2014/03/08/calculating-opengl-perspective-matrix-from-opencv-intrinsic-matrix
    i = 0;
    pGL[i++] = cam_fx / cam_cx;
    pGL[i++] = 0;
    pGL[i++] = 0;
    pGL[i++] = 0;
    pGL[i++] = 0;
    pGL[i++] = cam_fy / cam_cy;
    pGL[i++] = 0;
    pGL[i++] = 0;
    pGL[i++] = 0;
    pGL[i++] = 0;
    pGL[i++] = -(view_distance_max + view_distance_min) / (view_distance_max - view_distance_min);
    pGL[i++] = -1;
    pGL[i++] = 0;
    pGL[i++] = 0;
    pGL[i++] = -2 * view_distance_max * view_distance_min / (view_distance_max - view_distance_min);
    pGL[i++] = 0;

    // Ready.
    return(PsychError_none);
}

PsychError PSYCHCVAprilSettings(void)
{
    static char useString[] =
    "[nrThreads, imageDecimation, quadSigma, refineEdges, decodeSharpening] = PsychCV('AprilSettings' [, nrThreads][, imageDecimation][, quadSigma][, refineEdges][, decodeSharpening]);";
    //1          2                3          4            5                                              1            2                  3            4              5
    static char synopsisString[] =
        "Return current tracker parameters, optionally change tracker parameters.\n"
        "These settings are set to reasonable defaults at startup, but can be changed "
        "anytime with this subfunction. Most of these settings define tradeoffs between "
        "computation time aka tracking speed and quality/robustness of tracking.\n"
        "Following settings are available:\n"
        "'nrThreads' Number of processing threads to use for speeding up operation. Default is 1 for single-threaded operation.\n"
        "'imageDecimation' 1 = Process full image. > 1 = Only work on resolution decimated image for higher speed at lower precision. Default is 2.\n"
        "'quadSigma' How much blurring (values > 0) or sharpening (values < 0) to apply to input images to reduce noise. Default is 0 for none.\n"
        "'refineEdges' 1 = Perform edge refinement on detected edges (cheap, and the default), 0 = Use simpler strategy.\n"
        "'decodeSharpening' How much sharpening should be done to decoded images? Can help small tags. Default is 0.25.\n";
    static char seeAlsoString[] = "AprilDetectMarkers";

    int nrThreads, refineEdges;
    double quadSigma, imageDecimation, decodeSharpening;

    // Setup online help:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    PsychErrorExit(PsychCapNumInputArgs(5));        // The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(0));    // The required number of inputs
    PsychErrorExit(PsychCapNumOutputArgs(5));       // The maximum number of outputs

    if (!psychCVAprilInitialized)
        PsychErrorExitMsg(PsychError_user, "apriltags not yet initialized! Call PsychCV('AprilInitialize') first and retry!");

    // Copy out old settings:
    PsychCopyOutDoubleArg(1, FALSE, tagDetector->nthreads);
    PsychCopyOutDoubleArg(2, FALSE, tagDetector->quad_decimate);
    PsychCopyOutDoubleArg(3, FALSE, tagDetector->quad_sigma);
    PsychCopyOutDoubleArg(4, FALSE, tagDetector->refine_edges ? 1 : 0);
    PsychCopyOutDoubleArg(5, FALSE, tagDetector->decode_sharpening);

    // Copy in optional new settings:
    if (PsychCopyInIntegerArg(1, FALSE, &nrThreads) && (nrThreads > 0))
        tagDetector->nthreads = nrThreads;

    if (PsychCopyInDoubleArg(2, FALSE, &imageDecimation) && (imageDecimation >= 1))
        tagDetector->quad_decimate = imageDecimation;

    if (PsychCopyInDoubleArg(3, FALSE, &quadSigma))
        tagDetector->quad_sigma = quadSigma;

    if (PsychCopyInIntegerArg(4, FALSE, &refineEdges))
        tagDetector->refine_edges = (refineEdges > 0) ? true : false;

    if (PsychCopyInDoubleArg(5, FALSE, &decodeSharpening) && (decodeSharpening >= 0))
        tagDetector->decode_sharpening = decodeSharpening;

    // TODO further potential tunables:
    // td->qtp.max_nmaxima = 10;
    // t d->qtp.min_cluster_pixels = 5;
    // td->qtp.max_line_fit_mse = 10.0;
    // td->qtp.cos_critical_rad = cos(10 * M_PI / 180);
    // td->qtp.deglitch = false;
    // td->qtp.min_white_black_diff = 5;

    return(PsychError_none);
}

#endif
