/*
 *    SCREENReadHDRImage.c
 *
 *    AUTHORS:
 *
 *    mario.kleiner.de@gmail.com      mk
 *
 *    PLATFORMS:
 *
 *    All.
 */

#include "Screen.h"
#include "tinyexr.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "[imageArray, format, errorMsg] = Screen('ReadHDRImage', filename [, errorMode=0]);";
//                          1           2       3                                  1           2
static char synopsisString[] =
"Read a high dynamic range (HDR) image file from the filesystem and return its content.\n\n"
"This function allows to read some HDR file formats and return the image data as a "
"double matrix.\n"
"'filename' is the name of the image file.\n"
"'errorMode' is the optional flag to define how failures to read the file should be "
"handled: 0 = Fail silently: Return empty 'imageArray', 1 = Like 0, but print warning "
"or error message to console, 2 = Like 1, but also abort script with error message.\n"
"In any case, 'errorMsg' is either empty on success, 'unknown-format' if the file was "
"not recognized as a supported HDR format, or a loader specific error message if the "
"file could not be loaded for some reason.\n"
"'imageArray' is a height-by-width-by-channels double matrix containing the pixel "
"data in usual Matlab format, e.g., 4 channels RGBA planes. 'imageArray' can be "
"directly passed into Screen('MakeTexture', win, imageArray); to turn it into a "
"displayable image texture.\n"
"'format' is a name string that identifies the format of the image file read.\n"
"\n"
"The following HDR file formats are currently supported:\n"
"* OpenEXR file format (file suffix \".exr\", 'format' = \"openexr\"), via use of the "
"  included open-source TinyEXR library (https://github.com/syoyo/tinyexr).\n"
"\n";
static char seeAlsoString[] = "MakeTexture";

PsychError SCREENReadHDRImage(void)
{
    size_t      ix, iy, outWidth, outHeight, rIndex, gIndex, bIndex, aIndex;
    double      *returnArrayBaseDouble;
    float       *rgba = NULL;
    const char  *err = NULL;
    char        *filename;
    int         width, height, nrchannels;
    int         errorMode, rc;

    // Provide help if needed:
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    // Cap the numbers of inputs and outputs
    PsychErrorExit(PsychCapNumInputArgs(2));        // The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1));    // Min. 1 input args required.
    PsychErrorExit(PsychCapNumOutputArgs(3));       // The maximum number of outputs

    // Get image file name:
    PsychAllocInCharArg(1, kPsychArgRequired, &filename);

    // Get optional errorMode:
    errorMode = 0;
    PsychCopyInIntegerArg(2, kPsychArgOptional, &errorMode);
    if (errorMode < 0 || errorMode > 2)
        PsychErrorExitMsg(PsychError_user, "Invalid errorMode flag provided: Must be 0, 1 or 2.");

    // OpenEXR .exr file filename?
    if (TINYEXR_SUCCESS == IsEXR(filename)) {
        // Yes: Load it via TinyEXR simple loader api:
        rc = LoadEXR(&rgba, &width, &height, filename, &err);
        if (rc) {
            if ((errorMode > 0) && (PsychPrefStateGet_Verbosity() > 0))
                printf("PTB-ERROR: ReadHDRImage: Reading OpenEXR image file '%s' failed: %s\n", filename, err);

            goto out;
        }

        // Return format id:
        PsychCopyOutCharArg(2, kPsychArgOptional, "openexr");

        outWidth = (size_t) width;
        outHeight = (size_t) height;
        nrchannels = 4;
    }
    else {
        if ((errorMode > 0) && (PsychPrefStateGet_Verbosity() > 1))
            printf("PTB-ERROR: ReadHDRImage: File '%s' does not exist, or has unknown/unsupported image format.\n", filename);

        goto out;
    }

    // Readback of standard 32bpc float pixels into a double matrix:
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, outHeight, outWidth, nrchannels, &returnArrayBaseDouble);

    // Transpose and convert from packed/interleaved float -> planar double what we got, before returning it:
    for (ix = 0; ix < outWidth; ix++) {
        for (iy = 0; iy < outHeight; iy++) {
            // Compute write-indices for returned data:
            rIndex = PsychIndexElementFrom3DArray(outHeight, outWidth, nrchannels, iy, ix, 0);
            gIndex = PsychIndexElementFrom3DArray(outHeight, outWidth, nrchannels, iy, ix, 1);
            bIndex = PsychIndexElementFrom3DArray(outHeight, outWidth, nrchannels, iy, ix, 2);
            aIndex = PsychIndexElementFrom3DArray(outHeight, outWidth, nrchannels, iy, ix, 3);

            if (nrchannels > 0) returnArrayBaseDouble[rIndex] = rgba[(ix + iy * outWidth) * (size_t) nrchannels + 0];
            if (nrchannels > 1) returnArrayBaseDouble[gIndex] = rgba[(ix + iy * outWidth) * (size_t) nrchannels + 1];
            if (nrchannels > 2) returnArrayBaseDouble[bIndex] = rgba[(ix + iy * outWidth) * (size_t) nrchannels + 2];
            if (nrchannels > 3) returnArrayBaseDouble[aIndex] = rgba[(ix + iy * outWidth) * (size_t) nrchannels + 3];
        }
    }

    // Clean up:
    if (rgba)
        free(rgba);

    // No error:
    FreeEXRErrorMessage(err);

    // Return empty errorMsg for "success":
    PsychCopyOutCharArg(3, kPsychArgOptional, "");

    return(PsychError_none);

out: // Error out:

    // Return empty return arguments:
    PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 0, 0, 0, &returnArrayBaseDouble);
    PsychCopyOutCharArg(2, kPsychArgOptional, "");

    // Return errorMsg with reason for the failure:
    PsychCopyOutCharArg(3, kPsychArgOptional, err ? err : "unknown-format");
    FreeEXRErrorMessage(err);

    if (errorMode > 1)
        PsychErrorExitMsg(PsychError_user, "HDR image file read failed. For reason, see above.");

    return(PsychError_none);
}
