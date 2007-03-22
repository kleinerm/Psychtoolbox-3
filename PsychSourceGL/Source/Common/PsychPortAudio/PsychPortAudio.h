/*
	PsychToolbox3/Source/Common/PsychPortAudio/PsychPortAudio.h
	
	PLATFORMS:	All
			
	
	AUTHORS:
	Mario Kleiner   mk      mario.kleiner at tuebingen.mpg.de
	
	HISTORY:
	21.03.07		mk		wrote it.  
	
	NOTES:
	
	This needs the portaudio.h header file, which should be located in /usr/include/portaudio.h
	after a standard ./configure; make; make install installation on Linux & OS/X.

*/

//begin include once 
#ifndef PSYCH_IS_INCLUDED_PsychPortAudio
#define PSYCH_IS_INCLUDED_PsychPortAudio

//project includes
#include "Psych.h" 
#include "PsychTimeGlue.h"
#include "portaudio.h"

// Internal helper functions:

// Module exit function: Stop and close all audio streams, terminate PortAudio...
PsychError PsychPortAudioExit(void);

// Show command overview:
PsychError PSYCHPORTAUDIODisplaySynopsis(void);

//function prototypes:

// Version:
PsychError MODULEVersion(void); 

// Open audio device:
PsychError PSYCHPORTAUDIOOpen(void);
// Close audio device, shutdown PortAudio if last device is closed:
PsychError PSYCHPORTAUDIOClose(void);
PsychError PSYCHPORTAUDIOFillAudioBuffer(void);
PsychError PSYCHPORTAUDIOStartAudioDevice(void);
PsychError PSYCHPORTAUDIOStopAudioDevice(void);
//end include once
#endif

	
