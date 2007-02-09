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

#include "Screen.h"

void PsychVideoCaptureInit(void);
bool PsychOpenVideoCaptureDevice(PsychWindowRecordType *win, int deviceIndex, int* capturehandle, double* capturerectangle, int reqdepth, int num_dmabuffers, int allow_lowperf_fallback, char* targetmoviefilename, unsigned int recordingflags);
void PsychCloseVideoCaptureDevice(int capturehandle);
void PsychDeleteAllCaptureDevices(void);
int PsychGetTextureFromCapture(PsychWindowRecordType *win, int capturehandle, int checkForImage, double timeindex, PsychWindowRecordType *out_texture, double *presentation_timestamp, double* summed_intensity);
int PsychVideoCaptureRate(int capturehandle, double capturerate, int dropframes, double* startattime);
double PsychVideoCaptureSetParameter(int capturehandle, const char* pname, double value);
void PsychExitVideoCapture(void);

//end include once
#endif



