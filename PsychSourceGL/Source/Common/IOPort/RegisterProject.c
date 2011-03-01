/*
	RegisterProject.c
  
	PROJECTS: 
	
		IOPort only.  
  
	AUTHORS:
	
		Allen.Ingling@nyu.edu				awi
		mario.kleiner at tuebingen.mpg.de	mk
  
	PLATFORMS:	
	
		All.
    
	HISTORY:

		12/20/2004	awi		Wrote it.
		04/10/2008	mk		Started to extend/rewrite it to become a full-fledged generic I/O driver (serial port, parallel port, etc...).
 
	DESCRIPTION:
	
		Originally controlled the Code Mercenaries IO Warrior 40 device.
	
		It will hopefully become a generic I/O driver for driving any connection to
		external Input/Output devices that can somehow act as ports:
		
		* Serial ports (or emulated serial ports over USB/Firewire whatever...)
		* Parallel ports
		* Maybe network connections? (UDP/IP or TCP/IP)
		
		Initial target is reliable serial port support.
*/

//begin include once 
#include "RegisterProject.h"

/* PsychModuleInit()
 *
 * Called at first invocation of module - at module load and startup time.
 *
 */
PsychError PsychModuleInit(void)
{
	// Register the project exit function: Will close all open IO port connections etc.
	PsychErrorExit(PsychRegisterExit(&PsychExitIOPort)); 
	
	// register the project function which is called when the module
	// is invoked with no arguments: This will output usage information...
	PsychErrorExit(PsychRegister(NULL,  &IOPORTDisplaySynopsis));
        
	// Report the version:
	PsychErrorExit(PsychRegister("Version",  &MODULEVersion));

	// Register the module name
	PsychErrorExit(PsychRegister("IOPort", NULL));
	
	// Register subfunctions:
	
	// Support for IO-Warrior --> Disabled for now...
	// PsychErrorExit(PsychRegister("Init",  &IOPORTInit));
	// PsychErrorExit(PsychRegister("SetPort",  &IOPORTSetPort));

	// Generic port support:
	PsychErrorExit(PsychRegister("Verbosity", &IOPORTVerbosity));
	PsychErrorExit(PsychRegister("Close",  &IOPORTClose));
	PsychErrorExit(PsychRegister("CloseAll", &IOPORTCloseAll));
	PsychErrorExit(PsychRegister("Read", &IOPORTRead));
	PsychErrorExit(PsychRegister("Write", &IOPORTWrite));
	PsychErrorExit(PsychRegister("BytesAvailable", &IOPORTBytesAvailable));
	PsychErrorExit(PsychRegister("Purge", &IOPORTPurge));
	PsychErrorExit(PsychRegister("Flush", &IOPORTFlush));
	// PsychErrorExit(PsychRegister("Status", &IOPORTStatus));
	// PsychErrorExit(PsychRegister("Break", &IOPORTBreak));

	// Support for operating system managed serial ports:
	PsychErrorExit(PsychRegister("OpenSerialPort",  &IOPORTOpenSerialPort));
	PsychErrorExit(PsychRegister("ConfigureSerialPort",  &IOPORTConfigureSerialPort));
	
	// Initialize synopsis help strings:
	InitializeSynopsis();

	// Initialize internal I/O subsystem:
	PsychInitIOPort();
	
	// Set module authors:
	PsychSetModuleAuthorByInitials("awi");
	PsychSetModuleAuthorByInitials("mk");
 
	// Ready.
	return(PsychError_none);
}
