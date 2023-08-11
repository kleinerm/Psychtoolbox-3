/*
	PsychToolbox3/Source/Common/PsychKinect/PsychKinect.h

	PROJECTS: PsychKinect only.

	AUTHORS:

	mario.kleiner@tuebingen.mpg.de	mk

	PLATFORMS:	All.

	HISTORY:

	24.11.2010  mk		Created.

	DESCRIPTION:

	A Psychtoolbox driver for Microsoft's Kinect 3D-camera, based
	on the free software code of the http://openkinect.org project.

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychKinect
#define PSYCH_IS_INCLUDED_PsychKinect

//project includes
#include "Psych.h" 
#include "PsychTimeGlue.h"

//function prototypes
PsychError MODULEVersion(void);			//MODULEVersion.c
void InitializeSynopsis(void);
PsychError PsychKinectDisplaySynopsis(void);

void PsychKNInit(void);
PsychError PsychKNShutdown(void);

PsychError PSYCHKINECTOpen(void);
PsychError PSYCHKINECTClose(void);
PsychError PSYCHKINECTStart(void);
PsychError PSYCHKINECTStop(void);
PsychError PSYCHKINECTGetStatus(void);
PsychError PSYCHKINECTGrabFrame(void);
PsychError PSYCHKINECTReleaseFrame(void);
PsychError PSYCHKINECTGetImage(void);
PsychError PSYCHKINECTGetDepthImage(void);
PsychError PSYCHKINECTSetBaseCalibration(void);
PsychError PSYCHKINECTSetAngle(void);

//end include once
#endif
