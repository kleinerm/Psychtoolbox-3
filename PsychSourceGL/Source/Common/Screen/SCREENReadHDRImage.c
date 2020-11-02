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
static char useString[] = "[imageArray, format, errorMsg, auxInfo] = Screen('ReadHDRImage', filename [, errorMode=0]);";
//                          1           2       3         4                                 1           2
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
"'auxInfo' is a struct with additional properties and information about an image if "
"the image file supports such additional meta information. If an image file format "
"does not support useful additional information then 'auxInfo' will be an empty [] "
"matrix. The fields of the struct are dependent on the file format and specific "
"image file, so don't assume a fixed set of fields and fieldNames for a returned "
"'auxInfo' struct.\n"
"\n"
"The following HDR file formats are currently supported:\n"
"* OpenEXR file format (file suffix \".exr\", 'format' = \"openexr\"), via use of the "
"included open-source TinyEXR library (https://github.com/syoyo/tinyexr). Only "
"single-part RGB(A) images are supported at the moment, no multi-part images or "
"deep images. Only color channels are supported, no integer id channels.\n"
"The 'auxInfo' struct for OpenEXR images contains the following struct fields:\n"
"'ColorGamut' a 2-by-4 matrix defining the CIE 1931 2D chromaticity coordinates "
"of the red, green and blue primaries and white-point of the gamut / color-space "
"in which the image content is represented. 'GamutFromFile' is 1 if the "
"'ColorGamut' was actually read from the 'chromaticities' OpenEXR optional image "
"attribute, in which case the 'chromaticities' field will contain exactly the "
"same information. 'GamutFromFile' is 0 if the image file does not contain a "
"'chromaticities' attribute, in which case the spec requires to assume the gamut "
"to be that of BT-709-3, and the 'ColorGamut' matrix will contain the gamut info "
"for BT-709.\n"
"'sampleToNits' is always present and describes the conversion factor from sample "
"values to absolute units of nits. The 'sampToNits' attribute/field is either "
"provided by the exr file, or it will default to zero, to mark the info unavailable.\n"
"Additionally, many of the mandatory OpenEXR attributes are part of 'auxInfo', e.g., "
"'dataWindow', 'displayWindow', 'pixelAspectRatio'. Additional optional custom "
"attributes may be present. The current implementation can handle attributes of "
"type 'string', 'float', and 'chromaticities'. Other attributes will be listed, "
"but returned as empty [] struct fields.\n"
"You can find technical background info about the OpenEXR file format under this link:\n"
"https://www.openexr.com/documentation/TechnicalIntroduction.pdf\n"
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
    PsychErrorExit(PsychCapNumOutputArgs(4));       // The maximum number of outputs

    // Get image file name:
    PsychAllocInCharArg(1, kPsychArgRequired, &filename);

    // Get optional errorMode:
    errorMode = 0;
    PsychCopyInIntegerArg(2, kPsychArgOptional, &errorMode);
    if (errorMode < 0 || errorMode > 2)
        PsychErrorExitMsg(PsychError_user, "Invalid errorMode flag provided: Must be 0, 1 or 2.");

    // OpenEXR .exr file filename?
    if (TINYEXR_SUCCESS == IsEXR(filename)) {
        PsychGenericScriptType *s;
        PsychGenericScriptType *outMat;
        double *v;
        EXRVersion exrVersion;
        EXRHeader exrHeader;
        const char *fixedFieldNames[] = { "dataWindow", "displayWindow", "pixelAspectRatio", "screenWindowWidth", "screenWindowCenter",
                                          "lineOrder", "compression", "GamutFromFile", "ColorGamut", "sampleToNits" };
        #define fixedFieldCount 10
        const char *fieldNames[TINYEXR_MAX_CUSTOM_ATTRIBUTES + fixedFieldCount];
        int i, fieldCount;
        int hasChroma = 0;
        double sampToNits = 0;

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

        rc = ParseEXRVersionFromFile(&exrVersion, filename);
        if (rc) {
            if ((errorMode > 0) && (PsychPrefStateGet_Verbosity() > 0))
                printf("PTB-ERROR: ReadHDRImage: Reading version of OpenEXR image file '%s' failed: %s\n", filename, err);

            goto out;
        }

        // Parse EXR file header for optional attributes:
        InitEXRHeader(&exrHeader);
        rc = ParseEXRHeaderFromFile(&exrHeader, &exrVersion, filename, &err);
        if (rc) {
            if ((errorMode > 0) && (PsychPrefStateGet_Verbosity() > 0))
                printf("PTB-ERROR: ReadHDRImage: Reading header of OpenEXR image file '%s' failed: %s\n", filename, err);

            goto out;
        }

        // Got the header. Parse attributes we care about:

        // Build array with attribute names aka struct field names:
        fieldCount = fixedFieldCount + exrHeader.num_custom_attributes;
        for (i = 0; i < fieldCount; i++)
            fieldNames[i] = (i < fixedFieldCount) ? fixedFieldNames[i] : exrHeader.custom_attributes[i - fixedFieldCount].name;

        // Build struct with all fields, assign as 4th return argument:
        PsychAllocOutStructArray(4, kPsychArgOptional, -1, fieldCount, fieldNames, &s);

        // First fixed attributes which matter:

        // Return dataWindow as typical PTB rect:
        PsychAllocateNativeDoubleMat(1, 4, 1, &v, &outMat);
        *(v++) = exrHeader.data_window.min_x;
        *(v++) = exrHeader.data_window.min_y;
        *(v++) = exrHeader.data_window.max_x;
        *(v++) = exrHeader.data_window.max_y;
        PsychSetStructArrayNativeElement("dataWindow", 0, outMat, s);

        // Return displayWindow as typical PTB rect:
        PsychAllocateNativeDoubleMat(1, 4, 1, &v, &outMat);
        *(v++) = exrHeader.display_window.min_x;
        *(v++) = exrHeader.display_window.min_y;
        *(v++) = exrHeader.display_window.max_x;
        *(v++) = exrHeader.display_window.max_y;
        PsychSetStructArrayNativeElement("displayWindow", 0, outMat, s);

        // Return screenWindowCenter as 2D vector:
        PsychAllocateNativeDoubleMat(1, 2, 1, &v, &outMat);
        *(v++) = exrHeader.screen_window_center[0];
        *(v++) = exrHeader.screen_window_center[1];
        PsychSetStructArrayNativeElement("screenWindowCenter", 0, outMat, s);

        PsychSetStructArrayDoubleElement("screenWindowWidth", 0, exrHeader.screen_window_width, s);
        PsychSetStructArrayDoubleElement("pixelAspectRatio", 0, exrHeader.pixel_aspect_ratio, s);
        PsychSetStructArrayDoubleElement("lineOrder", 0, exrHeader.line_order, s);
        PsychSetStructArrayDoubleElement("compression", 0, exrHeader.compression_type, s);

        for (i = 0; i < exrHeader.num_custom_attributes; i++) {
            if (!strcmp(exrHeader.custom_attributes[i].type, "float"))
                PsychSetStructArrayDoubleElement(exrHeader.custom_attributes[i].name, 0, (double) *((float*) exrHeader.custom_attributes[i].value), s);

            if (!strcmp(exrHeader.custom_attributes[i].type, "string"))
                PsychSetStructArrayStringElement(exrHeader.custom_attributes[i].name, 0, (char*) exrHeader.custom_attributes[i].value, s);

            // Mark existence of optional sampToNits attribute for translating sample values to absolute nits:
            if (!strcmp(exrHeader.custom_attributes[i].name, "sampToNits") && !strcmp(exrHeader.custom_attributes[i].type, "float")) {
                sampToNits = (double) *((float*) exrHeader.custom_attributes[i].value);
            }

            if (!strcmp(exrHeader.custom_attributes[i].type, "chromaticities") && (exrHeader.custom_attributes[i].size == 32)) {
                int j;
                float *cv = (float*) exrHeader.custom_attributes[i].value;

                // Create color gamut and white point matrix and assign to OpenEXR standard attribute 'chromaticities':
                PsychAllocateNativeDoubleMat(2, 4, 1, &v, &outMat);
                for (j = 0; j < 8; j++)
                    *(v++) = cv[j];

                PsychSetStructArrayNativeElement(exrHeader.custom_attributes[i].name, 0, outMat, s);

                // If this is really the standardized 'chromaticities' attribute, then store its gamut
                // information in our standard struct field for gamut information:
                if (!strcmp(exrHeader.custom_attributes[i].name, "chromaticities")) {
                    // Again: Create color gamut and white point matrix and assign to our 'ColorGamut' standard struct field:
                    PsychAllocateNativeDoubleMat(2, 4, 1, &v, &outMat);
                    for (j = 0; j < 8; j++)
                        *(v++) = cv[j];

                    PsychSetStructArrayNativeElement("ColorGamut", 0, outMat, s);
                    hasChroma = 1;
                }
            }
        }

        // If the optional sampToNits attribute was not provided by the file then add
        // a dummy one with the obviously invalid value 0:
        PsychSetStructArrayDoubleElement("sampleToNits", 0, sampToNits, s);

        // Mark 'ColorGamut' as coming from the file, or not, depending if we found a
        // chromaticities attribute or not:
        PsychSetStructArrayDoubleElement("GamutFromFile", 0, hasChroma, s);
        if (!hasChroma) {
            // No chromaticities attribute in file, so we don't know the true
            // color gamut / color space of the image file. OpenEXR spec says
            // we should assume BT-709 color space and gamut in this case, so
            // manually assign the color primaries and white point of BT-709:
            // Create color gamut and white point matrix:
            PsychAllocateNativeDoubleMat(2, 4, 1, &v, &outMat);

            *(v++) = 0.6400; // Red
            *(v++) = 0.3300;

            *(v++) = 0.3000; // Green
            *(v++) = 0.6000;

            *(v++) = 0.1500; // Blue
            *(v++) = 0.0600;

            *(v++) = 0.3127; // White point
            *(v++) = 0.3290;

            PsychSetStructArrayNativeElement("ColorGamut", 0, outMat, s);
        }

        // Done with the header:
        FreeEXRHeader(&exrHeader);
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

    PsychAllocOutDoubleMatArg(4, kPsychArgOptional, 0, 0, 0, &returnArrayBaseDouble);

    if (errorMode > 1)
        PsychErrorExitMsg(PsychError_user, "HDR image file read failed. For reason, see above.");

    return(PsychError_none);
}
