/*
	Psychtoolbox3/PsychSourceGL/Source/Common/IOPort/PsychSerialGlue.h		
    
	PROJECTS: 
	
		IOPort for now.
  
	AUTHORS:
	
		mario.kleiner at tuebingen.mpg.de	mk
  
	PLATFORMS:	
	
		All Unix (aka Posix) systems: OS/X and Linux.
    
	HISTORY:

		04/10/2008	mk		Initial implementation.
 
	DESCRIPTION:
	
		This is the operating system dependent "glue code" header file for access to serial ports for the
		Unices, ie., for GNU/Linux and Apple MacOS/X. It is used by the higher-level serial port
		routines to abstract out operating system dependencies.

		The code is shared and #ifdef'ed if needed, because most of it is identical for Linux and OS/X.
*/

#ifndef PSYCH_IS_INCLUDED_SerialUnixGlue
#define PSYCH_IS_INCLUDED_SerialUnixGlue

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <paths.h>
#include <termios.h>
#include <sysexits.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>

// OS/X specific includes and structures:
#if PSYCH_SYSTEM == PSYCH_OSX

#include <AvailabilityMacros.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/serial/ioss.h>
#include <IOKit/IOBSD.h>

// End OS/X specific includes...
#endif

// Linux specific includes and structures:
#if PSYCH_SYSTEM == PSYCH_LINUX
// None yet.
#endif

typedef struct PsychSerialDeviceRecord {
	char				portSpec[1000];			// Name string of the device file.
	int					fileDescriptor;			// Device handle.
	struct termios		OriginalTTYAttrs;		// Stores original settings of device to allow restore on close.
	unsigned char*		readBuffer;				// Pointer to memory buffer for reading data.
	unsigned int		readBufferSize;			// Size of readbuffer.
} PsychSerialDeviceRecord;

#endif
