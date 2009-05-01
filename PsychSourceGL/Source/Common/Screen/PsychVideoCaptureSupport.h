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

// Unconditionally enable Quicktime Sequencegrabber capture support on OS/X and Windows:
#if PSYCH_SYSTEM != PSYCH_LINUX
#define PTBVIDEOCAPTURE_QT 1
#endif

#include "Screen.h"

typedef struct rawcapimgdata {
	int w;
	int h;
	int depth;
	void* data;
} rawcapimgdata;

// These are the generic entry points, to be called by SCREENxxxx videocapture functions and
// other parts of screen. They dispatch into API specific versions, depending on users choice
// of capture system and support by OS:
void PsychVideoCaptureInit(void);
bool PsychOpenVideoCaptureDevice(int engineId, PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle, int reqdepth, int num_dmabuffers, int allow_lowperf_fallback, char* targetmoviefilename, unsigned int recordingflags);
void PsychCloseVideoCaptureDevice(int capturehandle);
int PsychGetTextureFromCapture(PsychWindowRecordType *win, int capturehandle, int checkForImage, double timeindex, PsychWindowRecordType *out_texture, double *presentation_timestamp, double* summed_intensity, rawcapimgdata* outrawbuffer);
int PsychVideoCaptureRate(int capturehandle, double capturerate, int dropframes, double* startattime);
double PsychVideoCaptureSetParameter(int capturehandle, const char* pname, double value);
void PsychEnumerateVideoSources(int engineId, int outPos);
void PsychExitVideoCapture(void);

// These are the prototypes for Quicktime/SequenceGrabber versions, supported on OS/X and Windows:
#ifdef PTBVIDEOCAPTURE_QT
void PsychQTVideoCaptureInit(void);
bool PsychQTOpenVideoCaptureDevice(int slotid, PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle, int reqdepth, int num_dmabuffers, int allow_lowperf_fallback, char* targetmoviefilename, unsigned int recordingflags);
void PsychQTCloseVideoCaptureDevice(int capturehandle);
int PsychQTGetTextureFromCapture(PsychWindowRecordType *win, int capturehandle, int checkForImage, double timeindex, PsychWindowRecordType *out_texture, double *presentation_timestamp, double* summed_intensity, rawcapimgdata* outrawbuffer);
int PsychQTVideoCaptureRate(int capturehandle, double capturerate, int dropframes, double* startattime);
double PsychQTVideoCaptureSetParameter(int capturehandle, const char* pname, double value);
void PsychQTEnumerateVideoSources(int outPos);
void PsychQTExitVideoCapture(void);
#endif

// These are the prototypes for LibDC1394 V2 versions, supported on GNU/Linx, OS/X and in a experimental version on MS-Windows:
#ifdef PTBVIDEOCAPTURE_LIBDC
void PsychDCVideoCaptureInit(void);
bool PsychDCOpenVideoCaptureDevice(int slotid, PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle, int reqdepth, int num_dmabuffers, int allow_lowperf_fallback, char* targetmoviefilename, unsigned int recordingflags);
void PsychDCCloseVideoCaptureDevice(int capturehandle);
int PsychDCGetTextureFromCapture(PsychWindowRecordType *win, int capturehandle, int checkForImage, double timeindex, PsychWindowRecordType *out_texture, double *presentation_timestamp, double* summed_intensity, rawcapimgdata* outrawbuffer);
int PsychDCVideoCaptureRate(int capturehandle, double capturerate, int dropframes, double* startattime);
double PsychDCVideoCaptureSetParameter(int capturehandle, const char* pname, double value);
void PsychDCExitVideoCapture(void);
#endif

// These are the prototypes for the ARVideo capture library, supported on GNU/Linx, OS/X and MS-Windows:
#ifdef PTBVIDEOCAPTURE_ARVIDEO
void PsychARVideoCaptureInit(void);
bool PsychAROpenVideoCaptureDevice(int slotid, PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle, int reqdepth, int num_dmabuffers, int allow_lowperf_fallback, char* targetmoviefilename, unsigned int recordingflags);
void PsychARCloseVideoCaptureDevice(int capturehandle);
int PsychARGetTextureFromCapture(PsychWindowRecordType *win, int capturehandle, int checkForImage, double timeindex, PsychWindowRecordType *out_texture, double *presentation_timestamp, double* summed_intensity, rawcapimgdata* outrawbuffer);
int PsychARVideoCaptureRate(int capturehandle, double capturerate, int dropframes, double* startattime);
double PsychARVideoCaptureSetParameter(int capturehandle, const char* pname, double value);
void PsychARExitVideoCapture(void);
#endif

//end include once
#endif
