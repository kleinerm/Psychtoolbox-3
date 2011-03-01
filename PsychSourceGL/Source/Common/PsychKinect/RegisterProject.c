/*
	PsychToolbox3/Source/Common/PsychKinect/RegisterProject.c

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
#include "Psych.h"
#include "PsychKinect.h"

PsychError PsychModuleInit(void)
{
	// Register the project exit function
	PsychErrorExit(PsychRegisterExit(&PsychKNShutdown)); 

	// Register the project function which is called when the module
	// is invoked with no named subfunction:
	PsychErrorExit(PsychRegister(NULL,  &PsychKinectDisplaySynopsis));

	PsychErrorExit(PsychRegister("Open", &PSYCHKINECTOpen));
	PsychErrorExit(PsychRegister("Close", &PSYCHKINECTClose));
	PsychErrorExit(PsychRegister("Start", &PSYCHKINECTStart));
	PsychErrorExit(PsychRegister("Stop", &PSYCHKINECTStop));
	PsychErrorExit(PsychRegister("GetStatus", &PSYCHKINECTGetStatus));
	PsychErrorExit(PsychRegister("GrabFrame", &PSYCHKINECTGrabFrame));
	PsychErrorExit(PsychRegister("ReleaseFrame", &PSYCHKINECTReleaseFrame));
	PsychErrorExit(PsychRegister("GetImage", &PSYCHKINECTGetImage));
	PsychErrorExit(PsychRegister("GetDepthImage", &PSYCHKINECTGetDepthImage));
	PsychErrorExit(PsychRegister("SetBaseCalibration", &PSYCHKINECTSetBaseCalibration));
	PsychErrorExit(PsychRegister("SetAngle", &PSYCHKINECTSetAngle));

	//report the version
	PsychErrorExit(PsychRegister("Version", &MODULEVersion));

	//register the module name & authors:
	PsychErrorExit(PsychRegister("PsychKinect", NULL));
	PsychSetModuleAuthorByInitials("mk");

	// Register synopsis and named subfunctions.
	InitializeSynopsis();
	
	// Preinit everything:
	PsychKNInit();
	
	return(PsychError_none);
}
