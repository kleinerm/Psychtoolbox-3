/*
	Psychtoolbox3/PsychSourceGL/Source/Windows/IOPort/PsychSerialWindowsGlue.h		
    
	PROJECTS: 
	
		IOPort for now.
  
	AUTHORS:
	
		mario.kleiner at tuebingen.mpg.de	mk
  
	PLATFORMS:	
	
		All Windows (aka Win32) systems: Windows 2000, Windows-XP, Windows-Vista, ...
    
	HISTORY:

		05/10/2008	mk		Initial implementation.
 
	DESCRIPTION:
	
		This is the operating system dependent "glue code" layer for access to serial ports for the
		Microsoft Windows platforms. It is used by the higher-level serial port
		routines to abstract out operating system dependencies.
*/

#ifndef PSYCH_IS_INCLUDED_SerialWindowsGlue
#define PSYCH_IS_INCLUDED_SerialWindowsGlue

#include <time.h>

typedef struct PsychSerialDeviceRecord {
	char				portSpec[1000];			// Name string of the device file - typically "COM0:", but we alloc large space for virtual device files etc.
	HANDLE				fileDescriptor;			// Device handle.
	DCB					OriginalTTYAttrs;		// Stores original settings of device to allow restore on close.
	COMMTIMEOUTS		Originaltimeouts;		// dto.
	COMMTIMEOUTS		timeouts;				// Structure with Win-specific timeout settings.
	unsigned char*		readBuffer;				// Pointer to memory buffer for reading data.
	unsigned int		readBufferSize;			// Size of readbuffer.
	double				readTimeout;			// Backup copy of current read timeout value.
} PsychSerialDeviceRecord;

#endif
