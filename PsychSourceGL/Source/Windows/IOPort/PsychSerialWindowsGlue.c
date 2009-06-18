/*
	Psychtoolbox3/PsychSourceGL/Source/Windows/IOPort/PsychSerialWindowsGlue.c		
    
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

#include "IOPort.h"

// Internal function:
int PsychIOOSCheckError(PsychSerialDeviceRecord* device, char* inerrmsg);

// Externally defined level of verbosity:
extern int verbosity;

// Map numeric baud rate to Posix constant:
//static int BaudToConstant(int inint)
//{
//	// Need to map from numeric values to predefined constants. On OS/X the
//	// constants are identical to the input values, but on Linux they aren't, so better safe
//	// than sorry, although it looks as if Linux accepts the "raw" numbers properly as well...
//	switch(inint) {
//		case 50: inint = B50; break;
//		case 75: inint = B75; break;
//		case 110: inint = B110; break;
//		case 134: inint = B134; break;
//		case 150: inint = B150; break;
//		case 200: inint = B200; break;
//		case 300: inint = B300; break;
//		case 600: inint = B600; break;
//		case 1200: inint = B1200; break;
//		case 1800: inint = B1800; break;
//		case 2400: inint = B2400; break;
//		case 4800: inint = B4800; break;
//		case 9600: inint = B9600; break;
//		case 19200: inint = B19200; break;
//		case 38400: inint = B38400; break;
//		case 57600: inint = B57600; break;
//		case 115200: inint = B115200; break;
//		case 230400: inint = B230400; break;
//		case 7200: inint = B7200; break;
//		case 14400: inint = B14400; break;
//		case 28800: inint = B28800; break;
//		case 76800: inint = B76800; break;
//
//		default:
//			// Pass inint unchanged in the hope that the OS will accept it:
//			if (verbosity > 1) printf("IOPort: Non-standard BaudRate %i specified. We'll see if the OS likes this. Double-check the settings!\n", inint);
//	}
//	
//	return(inint);
//}
//
// Map Posix constant to numeric baud rate:
//static int ConstantToBaud(int inint)
//{
//	// Need to map predefined constants to numeric values On OS/X the
//	// constants are identical to the input values, but on Linux they aren't, so better safe
//	// than sorry, although it looks as if Linux accepts the "raw" numbers properly as well...
//	switch(inint) {
//		case B50: inint = 50; break;
//		case B75: inint = 75; break;
//		case B110: inint = 110; break;
//		case B134: inint = 134; break;
//		case B150: inint = 150; break;
//		case B200: inint = 200; break;
//		case B300: inint = 300; break;
//		case B600: inint = 600; break;
//		case B1200: inint = 1200; break;
//		case B1800: inint = 1800; break;
//		case B2400: inint = 2400; break;
//		case B4800: inint = 4800; break;
//		case B9600: inint = 9600; break;
//		case B19200: inint = 19200; break;
//		case B38400: inint = 38400; break;
//		case B57600: inint = 57600; break;
//		case B115200: inint = 115200; break;
//		case B230400: inint = 230400; break;
//		#if PSYCH_SYSTEM == PSYCH_OSX
//		// Only defined on OS/X:
//		case B7200: inint = 7200; break;
//		case B14400: inint = 14400; break;
//		case B28800: inint = 28800; break;
//		case B76800: inint = 76800; break;
//		#endif
//		default:
//			// Pass inint unchanged in the hope that the OS will accept it:
//			if (verbosity > 1) printf("IOPort: Non-standard BaudRate %i detected. Let's see if this makes sense...\n", inint);
//	}
//	
//	return(inint);
//}

/* PsychIOOSOpenSerialPort()
 *
 * Open a serial port device and configure it.
 *
 * portSpec - String with the port name / device name of the serial port device.
 * configString - String with port configuration parameters.
 * errmsg - Pointer to char[] buffer in which error messages should be returned, if any.
 *
 * On success, allocate a PsychSerialDeviceRecord with all relevant settings,
 * return a pointer to it.
 *
 * Otherwise abort with error message.
 */
PsychSerialDeviceRecord* PsychIOOSOpenSerialPort(const char* portSpec, const char* configString, char* errmsg)
{
    HANDLE			fileDescriptor = INVALID_HANDLE_VALUE;
    DCB				options;
	PsychSerialDeviceRecord* device = NULL;
	psych_bool			usererr = FALSE;
	unsigned int	myerrno;
	
	// Init errmsg error message to empty == no error:
	errmsg[0] = 0;

    // Open the serial port read/write, for exclusive access with normal attributes and default other settings:
    fileDescriptor = CreateFile(portSpec, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileDescriptor == INVALID_HANDLE_VALUE)
    {
		myerrno = GetLastError();
		if (myerrno == ERROR_FILE_NOT_FOUND) {
			sprintf(errmsg, "Error opening serial port device %s - No such serial port device exists! (%d) [ENOENT].\n", portSpec, myerrno);
			usererr = TRUE;
		}
		else if (myerrno == ERROR_SHARING_VIOLATION || myerrno == ERROR_ACCESS_DENIED) {
			sprintf(errmsg, "Error opening serial port device %s - The serial port is already open, close it first! (%d) [EBUSY EPERM]. Could be a permission problem as well.\n", portSpec, myerrno);
			usererr = TRUE;
		}
		else {
			sprintf(errmsg, "Error opening serial port device %s - Errorcode (%d).\n", portSpec, myerrno);
		}
        goto error;
    }
    
    // Create the device struct and init it:
	device = calloc(1, sizeof(PsychSerialDeviceRecord));
	device->fileDescriptor = INVALID_HANDLE_VALUE;
	device->readBuffer = NULL;
	device->readBufferSize = 0;
	
    // Get the current options and save them so we can restore the default settings later.
	device->OriginalTTYAttrs.DCBlength = sizeof(DCB);
    if (GetCommState(fileDescriptor, &(device->OriginalTTYAttrs)) == 0) {
        sprintf(errmsg, "Error getting original device settings for device %s - (%d).\n", portSpec, GetLastError());
        goto error;
    }

	// Get and save the current timeout settings as well:
    if (GetCommTimeouts(fileDescriptor, &(device->timeouts)) == 0) {
        sprintf(errmsg, "Error getting current device comm timeouts for device %s - (%d).\n", portSpec, GetLastError());
		goto error;
    }
	
    if (GetCommTimeouts(fileDescriptor, &(device->Originaltimeouts)) == 0) {
        sprintf(errmsg, "Error getting original device comm timeouts for device %s - (%d).\n", portSpec, GetLastError());
		goto error;
    }

	strncpy(device->portSpec, portSpec, sizeof(device->portSpec));
	device->fileDescriptor = fileDescriptor;
    options = device->OriginalTTYAttrs;

	// Ok, the device is basically open. Call the reconfiguration routine with the setting
	// string. It will do all further setup work:
	if (PsychError_none != PsychIOOSConfigureSerialPort(device, configString)) {
		// Something failed. Error handling...
        sprintf(errmsg, "Error changing device settings for device %s - (%d).\n", portSpec, GetLastError());
		usererr = TRUE;
        goto error;
	}

	if (device->readBuffer == NULL) {
        sprintf(errmsg, "Error for device %s - No InputBuffer allocated! You must specify the 'InputBuffer' size in the configuration.\n", portSpec);
		usererr = TRUE;
        goto error;
	}

	// Setup for blocking serial port write operations: Tell that a transmit-buffer-empty event shall be signalled:
	// This will set the EV_TXEMPTY whenever a write operation completes (and resets it at start of each new write
	// operation!). The WaitCommEvent() routine in PsychIOOSWriteSerialPort() will wait for such events when a
	// blocking write is requested by usercode.
	//
	// N.B.: This used to be part of the blocking write routine in PsychIOOSWriteSerialPort(), but we
	// learned that this call is awfully slow for unknown reasons, and it seems to sync us to the USB
	// duty-cycle on Serial-over-USB ports, which is not what we want! As switching this on and off on
	// demand would be also awfully slow, and as it doesn't do any harm for non-blocking writes AFAIK,
	// we now always unconditionally enable it already here during device setup, where extra delays of
	// 1-2 msecs don't matter.
	if (SetCommMask(device->fileDescriptor, EV_TXEMPTY)==0) {
		sprintf(errmsg, "Error during setup for blocking write operations to device %s during call to SetCommMask(EV_TXEMPTY) - (%d).\n", portSpec, GetLastError());
		goto error;
	}

    // Success! Return Pointer to new device structure:
    return(device);
    
    // Failure path: Called on error with error message in errmsg:
error:

		if (fileDescriptor != INVALID_HANDLE_VALUE)
		{
			// File open.
			if (device) {
				// Settings retrieved or made?
				if (device->fileDescriptor != INVALID_HANDLE_VALUE) {
					// Have original settings available: Try to restore them:
					if (SetCommState(fileDescriptor, &options) == 0) {
						if (verbosity > 1) printf("WARNING: In error handling: Error restoring tty attributes %s - (%d).\n", portSpec, GetLastError());
					}
					
					// Restore original timeout settings as well:
					if (SetCommTimeouts(fileDescriptor, &(device->Originaltimeouts)) == 0) {
						if (verbosity > 1) printf("WARNING: In error handling: Error restoring comm timeouts for device %s - (%d).\n", portSpec, GetLastError());
					}				
				}
				
				// Release read buffer, if any:
				if (device->readBuffer) free(device->readBuffer);
				
				// Release device struct:
				free(device);
			}
			
			// Close file:
			CloseHandle(fileDescriptor);
		}
    
	// Return with error message:
	if (verbosity > 0) {
		PsychErrorExitMsg(((usererr) ? PsychError_user : PsychError_system), errmsg);
	}
	
	return(NULL);
}

/* PsychIOOSCloseSerialPort()
 *
 * Close serial port connection/device referenced by given 'device' record.
 * Free the device record afterwards.
 */
void PsychIOOSCloseSerialPort(PsychSerialDeviceRecord* device)
{
	if (device == NULL) PsychErrorExitMsg(PsychError_internal, "NULL-Ptr instead of valid device pointer!");
	
	// Drain all send-buffers:
    // Block until all written output has been sent from the device.
    // Note that this call is simply passed on to the serial device driver. 
	// See tcsendbreak(3) ("man 3 tcsendbreak") for details.
    if (FlushFileBuffers(device->fileDescriptor) == 0)
    {
		if (verbosity > 1) printf("IOPort: WARNING: While trying to close serial port %s : Error waiting for drain - (%d).\n", device->portSpec, GetLastError());
    }
    
    // Traditionally it is good practice to reset a serial port back to
    // the state in which you found it. This is why the original settings
    // were saved.
    if (SetCommState(device->fileDescriptor, &(device->OriginalTTYAttrs)) == 0)
    {
		if (verbosity > 1) printf("IOPort: WARNING: While trying to close serial port %s : Could not restore original port settings - (%d).\n", device->portSpec, GetLastError());
    }

	// Restore original timeout settings as well:
	if (SetCommTimeouts(device->fileDescriptor, &(device->Originaltimeouts)) == 0) {
		if (verbosity > 1) printf("IOPort: WARNING: While trying to close serial port %s : Could not restore original timeout settings - (%d).\n", device->portSpec, GetLastError());
	}
				
	// Close device:
    CloseHandle(device->fileDescriptor);

	// Release read buffer, if any:
	if (device->readBuffer) free(device->readBuffer);

	// Release memory for device struct:
	free(device);
	
	// Done.
	return;
}

/* PsychIOOSConfigureSerialPort()
 *
 * (Re-)configure serial port connection/device referenced by given 'device' record.
 * The relevant parameters are stored in 'configString'.
 *
 * Returns success- or failure status.
 */
PsychError PsychIOOSConfigureSerialPort(PsychSerialDeviceRecord* device, const char* configString)
{
    DCB				options;
	char*			p;
	float			infloat;
	int				inint, inint2;
	psych_bool			updatetermios = FALSE;

	// Init DCBlength field to size of structure! Fixed as of 31.12.2008 -- May have caused
	// trouble in some setups with earlier releases!
	memset(&options, 0, sizeof(DCB));
	options.DCBlength = sizeof(DCB);
	
	// Retrieve current settings:
    if (GetCommState(device->fileDescriptor, &options) == 0)
    {
        if (verbosity > 0) printf("Error getting current serial port device settings for device %s - (%d).\n", device->portSpec, GetLastError());
		return(PsychError_system);
    }
	
	// Set opmode to binary mode, give a warning if 'Cooked' mode is requested, as only binary mode is supported on Windows:
	options.fBinary = 1;
	if (strstr(configString, "ProcessingMode=Cooked") && (verbosity > 1)) printf("IOPort: Warning: Requested 'ProcessingMode=Cooked' not supported on Windows. Will use binary mode aka raw mode.\n");

    // Print the current input and output baud rates.
    // See tcsetattr(4) ("man 4 tcsetattr") for details.
    if (verbosity > 3) {
		printf("IOPort-Info: Configuration for device %s:\n", device->portSpec);
		printf("IOPort-Info: Current baud rate is %d\n", (int) options.BaudRate);
    }
	
	if ((p = strstr(configString, "ReceiveTimeout="))) {
		// Set timeouts for read operations:
		if ((1!=sscanf(p, "ReceiveTimeout=%f", &infloat)) || (infloat < 0)) {
			if (verbosity > 0) printf("Invalid parameter for ReceiveTimeout set! Typo, or negative value provided.\n");
			return(PsychError_user);
		}
		else {
			// Set timeout: It is in granularity of milliseconds.
			// Clamp to a minimum of 1 millisecond if a non-zero timeout is requested:
			if ((infloat < 0.001) && (infloat > 0)) {
				if (verbosity > 1) printf("IOPort: Warning: Requested per-byte 'ReceiveTimeout' value %f secs is positive but smaller than supported minimum of 1 millisecond on Windows. Changed value to 0.001 secs.\n", infloat);
				infloat = 0.001f;
			}
			device->readTimeout = infloat;
		}
	}
	
	if ((p = strstr(configString, "SendTimeout="))) {
		// Set timeouts for write operations:
		if ((1!=sscanf(p, "SendTimeout=%f", &infloat)) || (infloat < 0)) {
			if (verbosity > 0) printf("Invalid parameter for SendTimeout set! Typo, or negative value provided.\n");
			return(PsychError_user);
		}
		else {
			// Set timeout: It is in granularity of milliseconds.
			// Clamp to a minimum of 1 millisecond if a non-zero timeout is requested:
			if ((infloat < 0.001) && (infloat > 0)) {
				if (verbosity > 1) printf("IOPort: Warning: Requested per-byte 'SendTimeout' value %f secs is positive but smaller than supported minimum of 1 millisecond on Windows. Changed value to 0.001 secs.\n", infloat);
				infloat = 0.001f;
			}

			device->timeouts.WriteTotalTimeoutMultiplier = (int) (infloat * 1000 + 0.5);
			device->timeouts.WriteTotalTimeoutConstant = 0;
			
			if (SetCommTimeouts(device->fileDescriptor, &(device->timeouts)) == 0) {
				if (verbosity > 0) printf("IOPort: Error: Failed to set requested write timeout %f seconds on serial port %s : SetCommTimeouts() failed with error code %d.\n", infloat, device->portSpec, GetLastError());
				return(PsychError_system);
			}
		}
	}

	// Set common baud rate for send and receive:
	if ((p = strstr(configString, "BaudRate="))) {
		if ((1!=sscanf(p, "BaudRate=%i", &inint)) || (inint < 1)) {
			if (verbosity > 0) printf("Invalid parameter %i for BaudRate set!\n", inint);
			return(PsychError_invalidIntegerArg);
		}
		else {
			// Set common speed for input- and output queues:
			options.BaudRate = inint;
			updatetermios = TRUE;
		}
	}

	// Generation and handling of parity bits:
	if ((p = strstr(configString, "Parity="))) {
		// Clear all parity settings:
		options.fParity = 0;
		options.Parity  = NOPARITY;
		
		if (strstr(p, "Parity=Even")) {
			options.fParity = 1;
			options.Parity  = EVENPARITY;
		}
		else 
		if (strstr(p, "Parity=Odd")) {
			options.fParity = 1;
			options.Parity  = ODDPARITY;
		}
		else
		if (strstr(p, "Parity=None")) {
			// Nothing to do.
			options.fParity = 0;
			options.Parity  = NOPARITY;
		}
		else {
			// Invalid parity spec:
			if (verbosity > 0) printf("Invalid parity setting %s not accepted! (Valid are None, Even and Odd", p);
			return(PsychError_user);
		}
		updatetermios = TRUE;		
	}
	
	// Handling of Break conditions: Not reproducible in a OS/X & Linux compatible way on MS-Windows to my knowledge,
	// so we simply ignore it for now. Imho even on OS/X and Linux this parameter is not very useful...
	//	if ((p = strstr(configString, "BreakBehaviour="))) {
	//		// Clear all break settings:
	//		options.c_iflag &= ~(IGNBRK | BRKINT);
	//		
	//		if (strstr(configString, "BreakBehaviour=Ignore")) {
	//			// Break signals will be ignored:
	//			options.c_iflag |= IGNBRK;
	//		}
	//		else 
	//		if (strstr(configString, "BreakBehaviour=Flush")) {
	//			// A break signal will flush the input- and output-queues:
	//			options.c_iflag |= BRKINT;
	//		}
	//		else
	//		if (strstr(configString, "BreakBehaviour=Zero")) {
	//			// Don't change anything, ie. IGNBRK and BRKINT not set:
	//			// A break signal will be output as single zero byte.
	//		}
	//		else {
	//			// Invalid spec:
	//			printf("Invalid break behaviour %s not accepted (Valid: Ignore, Flush, or Zero)!", p);
	//			return(PsychError_user);
	//		}
	//		updatetermios = TRUE;
	//	}

	// Handling of data bits:
	if ((p = strstr(configString, "DataBits="))) {
		// Clear all databit settings:
		options.ByteSize = 0;
		
		if (strstr(p, "DataBits=5")) {
			options.ByteSize = 5;
		}
		else 
		if (strstr(p, "DataBits=6")) {
			options.ByteSize = 6;
		}
		else
		if (strstr(p, "DataBits=7")) {
			options.ByteSize = 7;
		}
		else
		if (strstr(p, "DataBits=8")) {
			options.ByteSize = 8;
		}
		else
		if (strstr(p, "DataBits=16")) {
			options.ByteSize = 16;
		}
		else {
			// Invalid spec:
			if (verbosity > 0) printf("Invalid setting for data bits %s not accepted! (Valid: 5, 6, 7, 8 or 16 databits)", p);
			return(PsychError_user);
		}
		updatetermios = TRUE;	
	}
	
	// Handling of stop bits:
	if ((p = strstr(configString, "StopBits="))) {
		// Clear all stopbit settings:
		options.StopBits = 0;
		
		if (strstr(p, "StopBits=1")) {
			options.StopBits = ONESTOPBIT;
		}
		else 
		if (strstr(p, "StopBits=2")) {
			options.StopBits = TWOSTOPBITS;
		}
		else {
			// Invalid spec:
			if (verbosity > 0) printf("Invalid setting for stop bits %s not accepted! (Valid: 1 or 2 stopbits)", p);
			return(PsychError_user);
		}
		updatetermios = TRUE;	
	}
	
	// Handling of flow control:
	if ((p = strstr(configString, "FlowControl="))) {
		// Clear all flow control settings:
		options.fOutX = 0;
		options.fInX = 0;
		options.fRtsControl = 0;
		options.fDtrControl = 0;
		options.fOutxCtsFlow = 0;
		options.fOutxDsrFlow = 0;

		if (strstr(p, "FlowControl=None")) {
			// Set above already...
		}
		else 
		if (strstr(p, "FlowControl=Software")) {
			options.fOutX = 1;
			options.fInX = 1;
		}
		else
		if (strstr(p, "FlowControl=Hardware")) {
			options.fRtsControl = RTS_CONTROL_HANDSHAKE;
			options.fOutxCtsFlow = 1;
		}
		else {
			// Invalid spec:
			if (verbosity > 0) printf("Invalid setting for flow control %s not accepted! (Valid: None, Software, Hardware)", p);
			return(PsychError_user);
		}
		updatetermios = TRUE;		
	}

    // Print the new input and output baud rates.
    if (verbosity > 3) {
		// Output new baud rate:
		printf("IOPort-Info: Baud rate changed to %d\n", (int) options.BaudRate);
    }
    
	// Handling of DTR :
	if ((p = strstr(configString, "DTR="))) {
		if (strstr(p, "DTR=0")) {
			options.fDtrControl = DTR_CONTROL_DISABLE;
		}
		else
		if (strstr(p, "DTR=1")) {
			options.fDtrControl = DTR_CONTROL_ENABLE;
		}
		else {
			// Invalid spec:
			if (verbosity > 0) printf("Invalid setting for DTR %s not accepted! (Valid: 1 or 0)", p);
			return(PsychError_user);
		}		
		updatetermios = TRUE;		
	}
	
	// Handling of RTS :
	if ((p = strstr(configString, "RTS="))) {
		if (strstr(p, "RTS=0")) {
			options.fRtsControl = RTS_CONTROL_DISABLE;
		}
		else
		if (strstr(p, "RTS=1")) {
			options.fRtsControl = RTS_CONTROL_ENABLE;
		}
		else {
			// Invalid spec:
			if (verbosity > 0) printf("Invalid setting for RTS %s not accepted! (Valid: 1 or 0)", p);
			return(PsychError_user);
		}		
		updatetermios = TRUE;		
	}

    // Cause the new options to take effect immediately.
    if (updatetermios && (SetCommState(device->fileDescriptor, &options) == 0))
    {
        if (verbosity > 0) printf("Error setting new serial port configuration attributes for device %s - (%d).\n", device->portSpec, GetLastError());
        return(PsychError_system);
    }

	// Set input buffer size for receive ops:
	if ((p = strstr(configString, "InputBufferSize="))) {
		if ((1!=sscanf(p, "InputBufferSize=%i", &inint)) || (inint < 1)) {
			if (verbosity > 0) printf("Invalid parameter for InputBufferSize set! Typo or requested buffer size smaller than 1 byte.\n");
			return(PsychError_invalidIntegerArg);
		}
		else {
			// Set InputBufferSize:
			if (device->readBuffer) {
				// Buffer already exists. Try to realloc:
				p = (char*) realloc(device->readBuffer, inint);
				if (p == NULL) {
					// Realloc failed:
					if (verbosity > 0) printf("Reallocation of Inputbuffer of device %s for new size %i failed!", device->portSpec, inint);
					return(PsychError_outofMemory);
				}
				// Worked. Assign new values:
				device->readBuffer = (unsigned char*) p;
				device->readBufferSize = (unsigned int) inint;
			}
			else {
				// No Buffer yet. Allocate
				p = malloc(inint);
				if (p == NULL) {
					// Realloc failed:
					if (verbosity > 0) printf("Allocation of Inputbuffer of device %s of size %i failed!", device->portSpec, inint);
					return(PsychError_outofMemory);
				}
				
				// Worked. Assign new values:
				device->readBuffer = (unsigned char*) p;
				device->readBufferSize = (unsigned int) inint;
			}
		}
	}

	// Set buffer sizes of the hardware itself, ie., the driver-internal buffers:
	if ((p = strstr(configString, "HardwareBufferSizes="))) {
		if ((2!=sscanf(p, "HardwareBufferSizes=%i,%i", &inint, &inint2)) || (inint < 1) || (inint2 < 1)) {
			if (verbosity > 0) printf("Invalid parameter for HardwareBufferSizes set! Typo or at least one of the requested buffer sizes smaller than 1 byte.\n");
			return(PsychError_invalidIntegerArg);
		}
		else {
			// Set HardwareBufferSizes: This functionality is only available on the Windows platform.
			// It is only a hint or polite request to the underlying driver. The driver is free to
			// ignore the request and choose whatever size or buffering strategy it finds appropriate:
			if (!SetupComm(device->fileDescriptor, inint, inint2)) {
				// Failed:
				if (verbosity > 0) printf("Setup of explicit HardwareBufferSizes for device %s of inputsize %i and outputsize %i failed with error code %d!\n", device->portSpec, inint, inint2, GetLastError());
				return(PsychError_system);
			}
		}
	}

	if ((p = strstr(configString, "PollLatency="))) {
		// Set polling latency for busy-waiting read operations:
		if ((1!=sscanf(p, "PollLatency=%f", &infloat)) || (infloat < 0)) {
			if (verbosity > 0) printf("Invalid parameter for PollLatency set! Typo, or negative value provided.\n");
			return(PsychError_user);
		}
		else {
			device->pollLatency = infloat;
		}
	}

	// Done.
	return(PsychError_none);
}

/* PsychIOOSWriteSerialPort()
 *
 * Write data to serial port:
 * writedata = Ptr. to data buffer of uint8 values to write.
 * amount = Buffersize aka amount of bytes to write.
 * blocking = 0 --> Async, 1 --> Flush buffer and wait for write completion.
 * errmsg = Pointer to char array where error messages should be put to.
 * timestamp = Pointer to a 4 element vectorof double values where write-completion timestamps should be put to.
 * [0] = Final write completion timestamp.
 * [1] = Timestamp immediately before write() submission.
 * [2] = Timestamp immediately after write() submission.
 * [3] = Timestamp of last check for write completion.
 *
 * Returns number of bytes written, or -1 on error.
 */
int PsychIOOSWriteSerialPort(PsychSerialDeviceRecord* device, void* writedata, unsigned int amount, int blocking, char* errmsg, double* timestamp)
{
	int nwritten;
	DWORD EvtMask;
	
	// Nonblocking mode?
	if (blocking <= 0) {
		// Non-blocking write:

		// Ok, this is problematic: There isn't a simple way to make the WriteFile non-blocking, so for now, our
		// "non-blocking write" is a (probably blocking under certain conditions) WriteFile() op, just with the
		// FlushFileBuffer call missing.

		// Write the data: Take pre- and postwrite timestamps.
		PsychGetAdjustedPrecisionTimerSeconds(&timestamp[1]);
		if (WriteFile(device->fileDescriptor, writedata, amount, &nwritten, NULL) == 0)
		{
			sprintf(errmsg, "Error during non-blocking write to device %s - (%d).\n", device->portSpec, GetLastError());
			return(-1);
		}
		PsychGetAdjustedPrecisionTimerSeconds(&timestamp[2]);
	}
	else {
		// Blocking write:

		// Write the data: Take pre- and postwrite timestamps.
		PsychGetAdjustedPrecisionTimerSeconds(&timestamp[1]);
		if (WriteFile(device->fileDescriptor, writedata, amount, &nwritten, NULL) == 0)
		{
			sprintf(errmsg, "Error during blocking write to device %s - (%d).\n", device->portSpec, GetLastError());
			return(-1);
		}
		PsychGetAdjustedPrecisionTimerSeconds(&timestamp[2]);
		
		// Flush the write buffer and wait for write completion on physical hardware:
		if (FlushFileBuffers(device->fileDescriptor)==0) {
			sprintf(errmsg, "Error during blocking write to device %s while draining the write buffers - (%d).\n", device->portSpec, GetLastError());
			return(-1);
		}
		
		// Special polling mode to wait for transmit completion instead of WaitCommEvent()?
		if (blocking == 2) {
			// Yes. Use a tight polling loop which spin-waits until the driver output queue is empty (contains zero pending bytes):
			device->num_output_pending = nwritten;
			while (device->num_output_pending > 0) {
				// Take timestamp for this iteration:
				PsychGetAdjustedPrecisionTimerSeconds(&timestamp[3]);

				// Poll: This updates num_output_pending as side effect:
				PsychIOOSCheckError(device, errmsg);
			}
		}
		else {
			// Take timestamp for completeness although it doesn't make much sense in the blocking case:
			PsychGetAdjustedPrecisionTimerSeconds(&timestamp[3]);

			// Really wait for write completion, ie., until the last byte has left the transmitter:
			// (N.B.: No need to clear or init EvtMask, it is a pure return parameter)
			if (WaitCommEvent(device->fileDescriptor, (LPDWORD) &EvtMask, NULL)==0) {
				sprintf(errmsg, "Error during blocking write to device %s during call to WaitCommEvent(EV_TXEMPTY) - (%d).\n", device->portSpec, GetLastError());
				return(-1);
			}
		}
	}
	
	// Take write completion timestamp:
	PsychGetAdjustedPrecisionTimerSeconds(timestamp);

	// Check for communication errors, acknowledge them, clear the error state and return error message, if any:
	PsychIOOSCheckError(device, errmsg);
	
	// Write successfully completed if we reach this point. Clear error message, return:
	errmsg[0] = 0;
	
	return(nwritten);
}

int PsychIOOSReadSerialPort(PsychSerialDeviceRecord* device, void** readdata, unsigned int amount, int blocking, char* errmsg, double* timestamp)
{
	double timeout;
	int nread = 0;	
	*readdata = NULL;

	// Clamp 'amount' of data to be read to receive buffer size:
	if (amount > device->readBufferSize) {
		// Too much. Is amount unspecified aka INT_MAX? In that case,
		// we'll clamp it to readbuffers size. Otherwise we abort with
		// error.
		if (amount == INT_MAX) {
			// Clamp to buffersize:
			amount = device->readBufferSize;
		}
		else {
			// amount specified and impossible to satisfy request at
			// current readbuffer size. Abort with error and tell
			// user to resize readbuffer:
			sprintf(errmsg, "Amount of requested data %i is more than device %s can satisfy, as its input buffer is too small (%i bytes).\nSet a bigger readbuffer size please.\n", amount, device->portSpec, device->readBufferSize);
			return(-1);			
		}		
	}
	
	// Nonblocking mode?
	if (blocking <= 0) {
		// Yep. Set COMMTIMEOUT for non-blocking read mode:		
		device->timeouts.ReadIntervalTimeout = MAXDWORD;
		device->timeouts.ReadTotalTimeoutConstant = 0;
		device->timeouts.ReadTotalTimeoutMultiplier = 0;
		
		if (SetCommTimeouts(device->fileDescriptor, &(device->timeouts)) == 0)
		{
			sprintf(errmsg, "Error calling SetCommTimeouts on device %s for non-blocking read - (%d).\n", device->portSpec, GetLastError());
			return(-1);
		}
		
		// Read the data, at most 'amount' bytes, nonblocking:
		if (ReadFile(device->fileDescriptor, device->readBuffer, amount, &nread, NULL) == 0)
		{			
			// Some error:
			sprintf(errmsg, "Error during non-blocking read from device %s - (%d).\n", device->portSpec, GetLastError());
			return(-1);
		}
	}
	else {
		// Nope. Setup timeouts for blocking read with user code spec'd timeout parameters:
		// We set all fields to the same value, so reception of a single byte should not take longer than device->readTimeout
		// seconds, neither absolute, nor on average. ReadIntervalTimeout is equivalent to VTIME on Unix, its an inter-byte
		// timer. As this only gets triggered after receipt of first byte, we need to use ReadTotalTimeoutXXX as well, so
		// even if nothing is received, we time out after (readTimeout * amount) time units.
		// All these timeouts are per byte, not absolute for the whole read op, so the real maximum duration for the whole
		// read is the given readTimeout multiplied by the total number of requested bytes.
		// This is conformant to the Unix behaviour where the timeout is also per byte, so it scales with the total
		// number of requested bytes worst case.
		//
		// Granularity of setting is milliseconds, so we need to convert our seconds value to milliseconds, with proper
		// rounding applied:
		device->timeouts.ReadIntervalTimeout = (int) (device->readTimeout * 1000 + 0.5);
		device->timeouts.ReadTotalTimeoutMultiplier = device->timeouts.ReadIntervalTimeout;
		device->timeouts.ReadTotalTimeoutConstant = 0;
		
		if (SetCommTimeouts(device->fileDescriptor, &(device->timeouts)) == 0)
		{
			sprintf(errmsg, "Error calling SetCommTimeouts on device %s for blocking read - (%d).\n", device->portSpec, GetLastError());
			return(-1);
		}
		
		if (amount > 1) {
			// Need to poll for arrival of first byte, as the interbyte ReadIntervalTimeout timer is only armed after
			// reception of first byte:
			PsychGetAdjustedPrecisionTimerSeconds(&timeout);
			*timestamp = timeout;
			timeout+=device->readTimeout;
			
			while((*timestamp < timeout) && (PsychIOOSBytesAvailableSerialPort(device) < 1)) {
				PsychGetAdjustedPrecisionTimerSeconds(timestamp);
				// Due to the shoddy Windows scheduler, we can't use a nice sleep duration of 500 microseconds like
				// on Unices, but need to use a rather large 5 msecs so the system actually puts us to sleep
				// and we don't overload the machine in realtime scheduling:
				PsychWaitIntervalSeconds(device->pollLatency);
			}
			
			if (PsychIOOSBytesAvailableSerialPort(device) < 1) {
				// Ok, first byte didn't arrive within one timeout period:
				return(0);
			}
		}
		
		// Read the data, exactly 'amount' bytes, blocking, unless timeout occurs:
		if (ReadFile(device->fileDescriptor, device->readBuffer, amount, &nread, NULL) == 0)
		{			
			// Some error:
			sprintf(errmsg, "Error during blocking read from device %s - (%d).\n", device->portSpec, GetLastError());
			return(-1);
		}
	}
	
	// Take read-completion timestamp:
	PsychGetAdjustedPrecisionTimerSeconds(timestamp);

	// Check for communication errors, acknowledge them, clear the error state and return error message:
	PsychIOOSCheckError(device, errmsg);

	// Read successfully completed if we reach this point. Clear error message, assign read return data, return:
	errmsg[0] = 0;

	*readdata = device->readBuffer;	
	return(nread);
}

int PsychIOOSCheckError(PsychSerialDeviceRecord* device, char* inerrmsg)
{
	COMSTAT dstatus;
	DWORD   estatus;
	char errmsg[100];
	errmsg[0]=0;
	
	if (ClearCommError(device->fileDescriptor, &estatus, &dstatus)==0) {
		if (verbosity > 0) printf("Error during ClearCommError() on device %s returned (%d)\n", device->portSpec, GetLastError());
		if (inerrmsg) sprintf(inerrmsg, "Error during ClearCommError() on device %s returned (%d)\n", device->portSpec, GetLastError());
		return(0xdeadbeef);
	}
	
	if (estatus > 0 && inerrmsg) {
		if (estatus & CE_BREAK) { sprintf(errmsg, "IOPort: Break condition on receive line detected.\n"); strcat(inerrmsg, errmsg); }
		if (estatus & CE_FRAME) { sprintf(errmsg, "IOPort: Data packet framing error detected.\n"); strcat(inerrmsg, errmsg); }
		if (estatus & CE_OVERRUN) { sprintf(errmsg, "IOPort: Character buffer overrun detected.\n"); strcat(inerrmsg, errmsg); }
		if (estatus & CE_RXOVER) { sprintf(errmsg, "IOPort: Receive buffer overflow detected.\n"); strcat(inerrmsg, errmsg); }
		if (estatus & CE_RXPARITY) { sprintf(errmsg, "IOPort: Parity error detected.\n"); strcat(inerrmsg, errmsg); }
	}
	
	// Store number of pending characters to write out:
	device->num_output_pending = (unsigned int) dstatus.cbOutQue;
	
	return((int) estatus);
}

int PsychIOOSBytesAvailableSerialPort(PsychSerialDeviceRecord* device)
{
	COMSTAT dstatus;
	DWORD   dummy;
	
	if (ClearCommError(device->fileDescriptor, &dummy, &dstatus)==0) {
		if (verbosity > 0) printf("Error during 'BytesAvailable': ClearCommError() bytes available query on device %s returned (%d)\n", device->portSpec, GetLastError());
		return(-1);
	}
	
	return((int) dstatus.cbInQue);
}

void PsychIOOSFlushSerialPort(PsychSerialDeviceRecord* device)
{	
	if (FlushFileBuffers(device->fileDescriptor)==0) {
		if (verbosity > 0) printf("Error during 'Flush': FlushFileBuffers() on device %s returned (%d)\n", device->portSpec, GetLastError());
	}
	
	return;
}

void PsychIOOSPurgeSerialPort(PsychSerialDeviceRecord* device)
{
	if (PurgeComm(device->fileDescriptor, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR)==0) {
		if (verbosity > 0) printf("Error during 'Purge': PurgeComm() on device %s returned (%d)\n", device->portSpec, GetLastError());
	}
	
	return;
}
