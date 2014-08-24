/*
    PsychToolbox3/Source/Common/Screen/PsychVideoCaptureSupport.h

    PLATFORMS:	This is the OS independent version.

    AUTHORS:
        Mario Kleiner   mk  mario.kleiner at tuebingen.mpg.de

    HISTORY:
    02/07/06		mk		Wrote it.

    DESCRIPTION:

    Psychtoolbox functions for dealing with video capture devices.

*/




//include once
#ifndef PSYCH_IS_INCLUDED_PsychVideoCaptureSupport
#define PSYCH_IS_INCLUDED_PsychVideoCaptureSupport

// Maximum number of allowed simultaneously open capture devices. As structures are allocated
// statically, this should not be increased beyond reasonable limits. 100 is always a nice number:
#define PSYCH_MAX_CAPTUREDEVICES 100

#include "Screen.h"

typedef struct rawcapimgdata {
    int w;
    int h;
    int depth;
    int bitdepth;
    void* data;
} rawcapimgdata;

// These are the generic entry points, to be called by SCREENxxxx videocapture functions and
// other parts of screen. They dispatch into API specific versions, depending on users choice
// of capture system and support by OS:
void PsychVideoCaptureInit(void);
psych_bool PsychOpenVideoCaptureDevice(int engineId, PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle, int reqdepth, int num_dmabuffers, int allow_lowperf_fallback, char* targetmoviefilename, unsigned int recordingflags, int bitdepth);
void PsychCloseVideoCaptureDevice(int capturehandle);
int PsychGetTextureFromCapture(PsychWindowRecordType *win, int capturehandle, int checkForImage, double timeindex, PsychWindowRecordType *out_texture, double *presentation_timestamp, double* summed_intensity, rawcapimgdata* outrawbuffer);
int PsychVideoCaptureRate(int capturehandle, double capturerate, int dropframes, double* startattime);
double PsychVideoCaptureSetParameter(int capturehandle, const char* pname, double value);
void PsychEnumerateVideoSources(int engineId, int outPos);
void PsychExitVideoCapture(void);

// These are the prototypes for LibDC1394 V2 versions, supported on GNU/Linx, OS/X and in a experimental version on MS-Windows:
#ifdef PTBVIDEOCAPTURE_LIBDC
void PsychDCVideoCaptureInit(void);
psych_bool PsychDCOpenVideoCaptureDevice(int slotid, PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle, int reqdepth, int num_dmabuffers, int allow_lowperf_fallback, char* targetmoviefilename, unsigned int recordingflags, int bitdepth);
void PsychDCCloseVideoCaptureDevice(int capturehandle);
int PsychDCGetTextureFromCapture(PsychWindowRecordType *win, int capturehandle, int checkForImage, double timeindex, PsychWindowRecordType *out_texture, double *presentation_timestamp, double* summed_intensity, rawcapimgdata* outrawbuffer);
int PsychDCVideoCaptureRate(int capturehandle, double capturerate, int dropframes, double* startattime);
double PsychDCVideoCaptureSetParameter(int capturehandle, const char* pname, double value);
void PsychDCEnumerateVideoSources(int outPos);
void PsychDCExitVideoCapture(void);

// Helper for GStreamer movie playback: Debayers a raw bayer input image into a RGB output image: Caller has to free() outRGBImage after use!
unsigned char* PsychDCDebayerFrame(unsigned char* inBayerImage, unsigned int width, unsigned int height, unsigned int bitdepth);
#endif

// These are the prototypes for the GStreamer capture library, supported on GNU/Linx, OS/X and MS-Windows:
#ifdef PTB_USE_GSTREAMER
#include <gst/gst.h>

typedef struct PsychVideosourceRecordType {
    int deviceIndex;
    int classIndex;
    int inputIndex;
    psych_uint64 deviceURI;
    unsigned int flags;
    char deviceClassName[100];
    char deviceSelectorProperty[100];
    char deviceVideoPlugin[100];
    char deviceHandle[1000];
    char devicePath[1000];
    char deviceName[1000];
    char device[1000];
    void* gstdevice; // GstDevice* for this device as enumerated by GstDeviceProvider/GstDeviceMonitor if using GStreamer 1.4.0+
} PsychVideosourceRecordType;

void PsychGSCheckInit(const char* engineName);
void PsychGSVideoCaptureInit(void);
psych_bool PsychGSOpenVideoCaptureDevice(int slotid, PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle, int reqdepth, int num_dmabuffers, int allow_lowperf_fallback, char* targetmoviefilename, unsigned int recordingflags, int bitdepth);
void PsychGSCloseVideoCaptureDevice(int capturehandle);
int PsychGSGetTextureFromCapture(PsychWindowRecordType *win, int capturehandle, int checkForImage, double timeindex, PsychWindowRecordType *out_texture, double *presentation_timestamp, double* summed_intensity, rawcapimgdata* outrawbuffer);
int PsychGSVideoCaptureRate(int capturehandle, double capturerate, int dropframes, double* startattime);
double PsychGSVideoCaptureSetParameter(int capturehandle, const char* pname, double value);
PsychVideosourceRecordType* PsychGSEnumerateVideoSources(int outPos, int deviceIndex, GstElement **videocaptureplugin);
void PsychGSExitVideoCapture(void);

// PsychGetCodecLaunchLineFromString() - Helper function for GStreamer based movie writing.
psych_bool PsychGetCodecLaunchLineFromString(char* codecSpec, char* launchString);

#endif

//end include once
#endif
