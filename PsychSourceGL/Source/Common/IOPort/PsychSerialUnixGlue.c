/*
	Psychtoolbox3/PsychSourceGL/Source/Common/IOPort/PsychSerialUnixGlue.c		
    
	PROJECTS: 
	
		IOPort for now.
  
	AUTHORS:
	
		mario.kleiner at tuebingen.mpg.de	mk
  
	PLATFORMS:	
	
		All Unix (aka Posix) systems: OS/X and Linux.
    
	HISTORY:

		04/10/2008	mk		Initial implementation.
 
	DESCRIPTION:
	
		This is the operating system dependent "glue code" layer for access to serial ports for the
		Unices, ie., for GNU/Linux and Apple MacOS/X. It is used by the higher-level serial port
		routines to abstract out operating system dependencies.

		The code is shared and #ifdef'ed if needed, because most of it is identical for Linux and OS/X.
*/

#include "IOPort.h"

// Externally defined level of verbosity:
extern int verbosity;

// Map numeric baud rate to Posix constant:
static int BaudToConstant(int inint)
{
	// Need to map from numeric values to predefined constants. On OS/X the
	// constants are identical to the input values, but on Linux they aren't, so better safe
	// than sorry, although it looks as if Linux accepts the "raw" numbers properly as well...
	switch(inint) {
		case 50: inint = B50; break;
		case 75: inint = B75; break;
		case 110: inint = B110; break;
		case 134: inint = B134; break;
		case 150: inint = B150; break;
		case 200: inint = B200; break;
		case 300: inint = B300; break;
		case 600: inint = B600; break;
		case 1200: inint = B1200; break;
		case 1800: inint = B1800; break;
		case 2400: inint = B2400; break;
		case 4800: inint = B4800; break;
		case 9600: inint = B9600; break;
		case 19200: inint = B19200; break;
		case 38400: inint = B38400; break;
		case 57600: inint = B57600; break;
		case 115200: inint = B115200; break;
		case 230400: inint = B230400; break;
		#if PSYCH_SYSTEM == PSYCH_OSX
		// Only defined on OS/X:
		case 7200: inint = B7200; break;
		case 14400: inint = B14400; break;
		case 28800: inint = B28800; break;
		case 76800: inint = B76800; break;
		#endif

		default:
			// Pass inint unchanged in the hope that the OS will accept it:
			if (verbosity > 1) printf("IOPort: Non-standard BaudRate %i specified. We'll see if the OS likes this. Double-check the settings!\n", inint);
	}
	
	return(inint);
}

// Map Posix constant to numeric baud rate:
static int ConstantToBaud(int inint)
{
	// Need to map predefined constants to numeric values On OS/X the
	// constants are identical to the input values, but on Linux they aren't, so better safe
	// than sorry, although it looks as if Linux accepts the "raw" numbers properly as well...
	switch(inint) {
		case B50: inint = 50; break;
		case B75: inint = 75; break;
		case B110: inint = 110; break;
		case B134: inint = 134; break;
		case B150: inint = 150; break;
		case B200: inint = 200; break;
		case B300: inint = 300; break;
		case B600: inint = 600; break;
		case B1200: inint = 1200; break;
		case B1800: inint = 1800; break;
		case B2400: inint = 2400; break;
		case B4800: inint = 4800; break;
		case B9600: inint = 9600; break;
		case B19200: inint = 19200; break;
		case B38400: inint = 38400; break;
		case B57600: inint = 57600; break;
		case B115200: inint = 115200; break;
		case B230400: inint = 230400; break;
		#if PSYCH_SYSTEM == PSYCH_OSX
		// Only defined on OS/X:
		case B7200: inint = 7200; break;
		case B14400: inint = 14400; break;
		case B28800: inint = 28800; break;
		case B76800: inint = 76800; break;
		#endif
		default:
			// Pass inint unchanged in the hope that the OS will accept it:
			if (verbosity > 1) printf("IOPort: Non-standard BaudRate %i detected. Let's see if this makes sense...\n", inint);
	}
	
	return(inint);
}

/* PsychIOOSOpenSerialPort()
 *
 * Open a serial port device and configure it.
 *
 * portSpec - String with the port name / device name of the serial port device.
 * configString - String with port configuration parameters.
 *
 * On success, allocate a PsychSerialDeviceRecord with all relevant settings,
 * return a pointer to it.
 *
 * Otherwise abort with error message.
 */
PsychSerialDeviceRecord* PsychIOOSOpenSerialPort(const char* portSpec, const char* configString)
{
    int				fileDescriptor = -1;
    struct termios	options;
    char			errmsg[1000];
	PsychSerialDeviceRecord* device = NULL;
	bool			usererr = FALSE;
	
    // Open the serial port read/write, with no controlling terminal, and don't wait for a connection.
    // The O_NONBLOCK flag also causes subsequent I/O on the device to be non-blocking.
    // See open(2) ("man 2 open") for details.
    fileDescriptor = open(portSpec, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fileDescriptor == -1)
    {
		if (errno == EBUSY) {
			sprintf(errmsg, "Error opening serial port device %s - The serial port is already open, close it first! %s(%d).\n", portSpec, strerror(errno), errno);
			usererr = TRUE;
		}
		else {
			sprintf(errmsg, "Error opening serial port device %s - %s(%d).\n", portSpec, strerror(errno), errno);
		}
        goto error;
    }

    // Note that open() follows POSIX semantics: multiple open() calls to the same file will succeed
    // unless the TIOCEXCL ioctl is issued. This will prevent additional opens except by root-owned
    // processes.
    // See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
    if (ioctl(fileDescriptor, TIOCEXCL) == -1)
    {
        printf("IOPort-Warning: Error setting exclusive access (TIOCEXCL) on device %s - %s(%d).\n", portSpec, strerror(errno), errno);
        // goto error;
    }
    
    // Create the device struct and init it:
	device = calloc(1, sizeof(PsychSerialDeviceRecord));
	device->fileDescriptor = -1;
	device->readBuffer = NULL;
	device->readBufferSize = 0;
	
    // Get the current options and save them so we can restore the default settings later.
    if (tcgetattr(fileDescriptor, &(device->OriginalTTYAttrs)) == -1) {
        sprintf(errmsg, "Error getting original device settings for device %s - %s(%d).\n", portSpec, strerror(errno), errno);
        goto error;
    }

	strncpy(device->portSpec, portSpec, sizeof(device->portSpec));
	device->fileDescriptor = fileDescriptor;
    options = device->OriginalTTYAttrs;

	// Ok, the device is basically open. Call the reconfiguration routine with the setting
	// string. It will do all further setup work:
	if (PsychError_none != PsychIOOSConfigureSerialPort(device, configString)) {
		// Something failed. Error handling...
        sprintf(errmsg, "Error changing device settings for device %s - %s(%d).\n", portSpec, strerror(errno), errno);
		usererr = TRUE;
        goto error;
	}

	if (device->readBuffer == NULL) {
        sprintf(errmsg, "Error for device %s - No InputBuffer allocated! You must specify the 'InputBuffer' size in the configuration.\n", portSpec);
		usererr = TRUE;
        goto error;
	}

    // Success! Return Pointer to new device structure:
    return(device);
    
    // Failure path: Called on error with error message in errmsg:
error:

    if (fileDescriptor != -1)
    {
		// File open.
		if (device) {
			// Settings retrieved or made?
			if (device->fileDescriptor != -1) {
				// Have original settings available: Try to restore them:
				if (tcsetattr(fileDescriptor, TCSANOW, &options) == -1) {
					printf("WARNING: In error handling: Error restoring tty attributes %s - %s(%d).\n", portSpec, strerror(errno), errno);
				}
			}

			// Release read buffer, if any:
			if (device->readBuffer) free(device->readBuffer);

			// Release device struct:
			free(device);
		}

		// Close file:
        close(fileDescriptor);
    }
    
	// Return with error message:
	PsychErrorExitMsg(((usererr) ? PsychError_user : PsychError_system), errmsg);
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
    if (tcdrain(device->fileDescriptor) == -1)
    {
        printf("IOPort: WARNING: While trying to close serial port: Error waiting for drain - %s(%d).\n", strerror(errno), errno);
    }
    
    // Traditionally it is good practice to reset a serial port back to
    // the state in which you found it. This is why the original termios struct
    // was saved.
    if (tcsetattr(device->fileDescriptor, TCSANOW, &(device->OriginalTTYAttrs)) == -1)
    {
        printf("IOPort: WARNING: While trying to close serial port: Could not restore original port settings - %s(%d).\n", strerror(errno), errno);
    }

	// Close device:
    close(device->fileDescriptor);

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
    struct termios	options;
    int				handshake;
	char*			p;
	float			infloat;
	int				inint;
	unsigned long	mics = 0UL;
	bool			updatetermios = FALSE;

    // The serial port attributes such as timeouts and baud rate are set by modifying the termios
    // structure and then calling tcsetattr() to cause the changes to take effect. Note that the
    // changes will not become effective without the tcsetattr() call.
    // See tcsetattr(4) ("man 4 tcsetattr") for details.

	// Retrieve current settings:
    if (tcgetattr(device->fileDescriptor, &options) == -1)
    {
        printf("Error getting current serial port device settings for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
		return(PsychError_system);
    }
	
    // Print the current input and output baud rates.
    // See tcsetattr(4) ("man 4 tcsetattr") for details.
    if (verbosity > 3) {
		printf("IOPort-Info: Configuration for device %s:\n", device->portSpec);
		printf("IOPort-Info: Current input baud rate is %d\n", (int) cfgetispeed(&options));
		printf("IOPort-Info: Current output baud rate is %d\n", (int) cfgetospeed(&options));
    }
	
    // Set raw input (non-canonical) mode: No line-based processing or editing of
	// received data via special control characters, unless a processing mode of
	// cooked was explicitely requested:
    if (strstr(configString, "ProcessingMode=Cooked") == NULL) { cfmakeraw(&options); updatetermios = TRUE; }
	
	// No minimum number of bytes to receive:
	options.c_cc[VMIN] = 0;

	if ((p = strstr(configString, "ReceiveTimeout="))) {
		// Set timeouts for read operations: We set VMIN to zero and VTIME to zero,
		// which means: Don't block if no data available, just return with zero read
		// bytes of data.
		// See tcsetattr(4) ("man 4 tcsetattr") and termios(4) ("man 4 termios") for details.		
		if ((1!=sscanf(p, "ReceiveTimeout=%f", &infloat)) || (infloat < 0)) {
			printf("Invalid parameter for ReceiveTimeout set! Typo, or negative value provided.\n");
			return(PsychError_user);
		}
		else {
			// Set timeout: It is in granularity of 1/10th seconds, so we need to quantize to 10th of seconds:
			options.c_cc[VTIME] = (int) (infloat * 10 + 0.5);
			updatetermios = TRUE;
		}
	}
	
	// Set common baud rate for send and receive:
	if ((p = strstr(configString, "BaudRate="))) {
		if (1!=sscanf(p, "BaudRate=%i", &inint)) {
			printf("Invalid parameter for BaudRate set!\n");
			return(PsychError_invalidIntegerArg);
		}
		else {
			// Set common speed for input- and output queues:
			inint = BaudToConstant(inint);
			if (-1==cfsetspeed(&options, inint)) {
				printf("Invalid BaudRate %i not accepted! (%s)", inint, strerror(errno));
				return(PsychError_invalidIntegerArg);
			}
			updatetermios = TRUE;
		}
	}

	// Generation and handling of parity bits:
	if ((p = strstr(configString, "Parity="))) {
		// Clear all parity settings:
		options.c_cflag &= ~(PARENB | PARODD);
		options.c_iflag &= ~INPCK;
		
		if (strstr(configString, "Parity=Even")) {
			options.c_cflag |= PARENB;
			options.c_iflag |= INPCK;
		}
		else 
		if (strstr(configString, "Parity=Odd")) {
			options.c_cflag |= (PARENB | PARODD);
			options.c_iflag |= INPCK;
		}
		else
		if (strstr(configString, "Parity=None")) {
			// Nothing to do.
		}
		else {
			// Invalid parity spec:
			printf("Invalid parity setting %s not accepted! (Valid are None, Even and Odd", p);
			return(PsychError_user);
		}
		updatetermios = TRUE;		
	}
	
	// Handling of Break conditions:
	if ((p = strstr(configString, "BreakBehaviour="))) {
		// Clear all break settings:
		options.c_iflag &= ~(IGNBRK | BRKINT);
		
		if (strstr(configString, "BreakBehaviour=Ignore")) {
			// Break signals will be ignored:
			options.c_iflag |= IGNBRK;
		}
		else 
		if (strstr(configString, "BreakBehaviour=Flush")) {
			// A break signal will flush the input- and output-queues:
			options.c_iflag |= BRKINT;
		}
		else
		if (strstr(configString, "BreakBehaviour=Zero")) {
			// Don't change anything, ie. IGNBRK and BRKINT not set:
			// A break signal will be output as single zero byte.
		}
		else {
			// Invalid spec:
			printf("Invalid break behaviour %s not accepted (Valid: Ignore, Flush, or Zero)!", p);
			return(PsychError_user);
		}
		updatetermios = TRUE;
	}

	// Handling of data bits:
	if ((p = strstr(configString, "DataBits="))) {
		// Clear all databit settings:
		options.c_cflag &= ~(CS5 | CS6 | CS7 | CS8);
		
		if (strstr(configString, "DataBits=5")) {
			options.c_cflag |= CS5;
		}
		else 
		if (strstr(configString, "DataBits=6")) {
			options.c_cflag |= CS6;
		}
		else
		if (strstr(configString, "DataBits=7")) {
			options.c_cflag |= CS7;
		}
		else
		if (strstr(configString, "DataBits=8")) {
			options.c_cflag |= CS8;
		}
		else {
			// Invalid spec:
			printf("Invalid setting for data bits %s not accepted! (Valid: 5, 6, 7 or 8 databits)", p);
			return(PsychError_user);
		}
		updatetermios = TRUE;	
	}
	
	// Handling of flow control:
	if ((p = strstr(configString, "FlowControl="))) {
		// Clear all flow control settings:
		options.c_cflag &= ~CRTSCTS;
		options.c_iflag &= ~(IXON | IXOFF);
		
		if (strstr(configString, "FlowControl=None")) {
			// Set above already...
		}
		else 
		if (strstr(configString, "FlowControl=Software")) {
			options.c_iflag |= (IXON | IXOFF);
		}
		else
		if (strstr(configString, "FlowControl=Hardware")) {
			options.c_cflag |= CRTSCTS;
		}
		else {
			// Invalid spec:
			printf("Invalid setting for flow control %s not accepted! (Valid: None, Software, Hardware)", p);
			return(PsychError_user);
		}
		updatetermios = TRUE;		
	}
			
	//#if defined(MAC_OS_X_VERSION_10_4) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4)
	//	// Starting with Tiger, the IOSSIOSPEED ioctl can be used to set arbitrary baud rates
	//	// other than those specified by POSIX. The driver for the underlying serial hardware
	//	// ultimately determines which baud rates can be used. This ioctl sets both the input
	//	// and output speed. 
	//	
	//	speed_t speed = 14400; // Set 14400 baud
	//    if (ioctl(fileDescriptor, IOSSIOSPEED, &speed) == -1)
	//    {
	//        printf("Error calling ioctl(..., IOSSIOSPEED, ...) %s - %s(%d).\n",
	//            bsdPath, strerror(errno), errno);
	//    }
	//#endif
    
    // Print the new input and output baud rates. Note that the IOSSIOSPEED ioctl interacts with the serial driver 
	// directly bypassing the termios struct. This means that the following two calls will not be able to read
	// the current baud rate if the IOSSIOSPEED ioctl was used but will instead return the speed set by the last call
	// to cfsetspeed.
    if (verbosity > 3) {
			// Retrieve baudrate and check for equal rate on input- and output queue:
			inint = cfgetispeed(&options);
			if ((inint != cfgetospeed(&options)) && (verbosity > 1)) printf("IOPort: Warning: Hmm, new input- and output baudrates %i vs. %i don't match!? May or may not be a problem...\n", inint, (int) cfgetospeed(&options)); 

			// Output new baud rate:
			printf("IOPort-Info: Baud rate changed to %d\n", (int) ConstantToBaud(inint));
    }
    
    // Cause the new options to take effect immediately.
    if (updatetermios && (tcsetattr(device->fileDescriptor, TCSANOW, &options) == -1))
    {
        printf("Error setting new serial port configuration attributes for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
        return(PsychError_system);
    }

    // To set the modem handshake lines, use the following ioctls.
    // See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.

	// Retrieve current handshake state aka DTR, RTS, CTS, DSR et al.:
    if (ioctl(device->fileDescriptor, TIOCMGET, &handshake) == -1) {
		printf("Error getting lines status for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
		return(PsychError_system);
    }
    
    	if (verbosity > 3) {
		printf("IOPort-Info: Handshake lines currently set to %d : ", handshake);
		printf("DTR=%i : DSR=%i : RTS=%i : CTS=%i\n", (handshake & TIOCM_DTR) ? 1:0, (handshake & TIOCM_DSR) ? 1:0, (handshake & TIOCM_RTS) ? 1:0, (handshake & TIOCM_CTS) ? 1:0);
	}

	// Handling of DTR :
	if ((p = strstr(configString, "DTR="))) {
		if (strstr(configString, "DTR=0")) {
			handshake&= ~TIOCM_DTR;
		}
		else
		if (strstr(configString, "DTR=1")) {
			handshake|= TIOCM_DTR;
		}
		else {
			// Invalid spec:
			printf("Invalid setting for DTR %s not accepted! (Valid: 1 or 0)", p);
			return(PsychError_user);
		}		
	}

	// Handling of DSR :
	if ((p = strstr(configString, "DSR="))) {
		if (strstr(configString, "DSR=0")) {
			handshake&= ~TIOCM_DSR;
		}
		else
		if (strstr(configString, "DSR=1")) {
			handshake|= TIOCM_DSR;
		}
		else {
			// Invalid spec:
			printf("Invalid setting for DSR %s not accepted! (Valid: 1 or 0)", p);
			return(PsychError_user);
		}		
	}
	
	// Handling of RTS :
	if ((p = strstr(configString, "RTS="))) {
		if (strstr(configString, "RTS=0")) {
			handshake&= ~TIOCM_RTS;
		}
		else
		if (strstr(configString, "RTS=1")) {
			handshake|= TIOCM_RTS;
		}
		else {
			// Invalid spec:
			printf("Invalid setting for RTS %s not accepted! (Valid: 1 or 0)", p);
			return(PsychError_user);
		}		
	}

	// Handling of CTS :
	if ((p = strstr(configString, "CTS="))) {
		if (strstr(configString, "CTS=0")) {
			handshake&= ~TIOCM_CTS;
		}
		else
		if (strstr(configString, "CTS=1")) {
			handshake|= TIOCM_CTS;
		}
		else {
			// Invalid spec:
			printf("Invalid setting for CTS %s not accepted! (Valid: 1 or 0)", p);
			return(PsychError_user);
		}		
	}

    if (ioctl(device->fileDescriptor, TIOCMSET, &handshake) == -1) {
		printf("Error setting handshake lines status for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
		return(PsychError_system);
    }
	
	// This function is OS/X specific, silently ignored on Linux...
#if PSYCH_SYSTEM == PSYCH_OSX	
	if ((p = strstr(configString, "ReceiveLatency="))) {
		// Set receive latency for low-level driver: Minimum latency between dequeue and notify
		// operation on reception of new input data. The granularity is microseconds...
		if ((1!=sscanf(p, "ReceiveLatency=%f", &infloat)) || (infloat < 0)) {
			printf("Invalid parameter for ReceiveLatency set! Typo, or negative value provided.\n");
			return(PsychError_user);
		}
		else {
			// Set timeout: It is in granularity microseconds, so we need to quantize to microseconds:
			mics = (unsigned long) (infloat * 1000000 + 0.5);
			
			// A value of zero would revert to operating system default, so we need to make sure
			// the mics value is at least 1 microsecond:
			if (mics == 0) mics = 1;
			
			// Set the receive latency in microseconds. Serial drivers use this value to determine how often to
			// dequeue characters received by the hardware. Most applications don't need to set this value: if an
			// app reads lines of characters, the app can't do anything until the line termination character has been
			// received anyway. The most common applications which are sensitive to read latency are MIDI and IrDA
			// applications.			
			if (ioctl(device->fileDescriptor, IOSSDATALAT, &mics) == -1) {
				printf("Error setting receive latency for device %s to %d microseconds - %s(%d).\n", device->portSpec, mics, strerror(errno), errno);
				return(PsychError_system);
			}
		}
	}
#endif

	// TODO: Try if TIOCTIMESTAMP is useful to get extra accurate receive timestamps on OS/X...

	// Set input buffer size for receive ops:
	if ((p = strstr(configString, "InputBufferSize="))) {
		if ((1!=sscanf(p, "InputBufferSize=%i", &inint)) || (inint < 1)) {
			printf("Invalid parameter for InputBufferSize set! Typo or requested buffer size smaller than 1 byte.\n");
			return(PsychError_invalidIntegerArg);
		}
		else {
			// Set InputBufferSize:
			if (device->readBuffer) {
				// Buffer already exists. Try to realloc:
				p = (char*) realloc(device->readBuffer, inint);
				if (p == NULL) {
					// Realloc failed:
					printf("Reallocation of Inputbuffer of device %s for new size %i failed! (%s)", device->portSpec, inint, strerror(errno));
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
					printf("Allocation of Inputbuffer of device %s of size %i failed! (%s)", device->portSpec, inint, strerror(errno));
					return(PsychError_outofMemory);
				}
				
				// Worked. Assign new values:
				device->readBuffer = (unsigned char*) p;
				device->readBufferSize = (unsigned int) inint;
			}
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
 * nonblocking = 0 --> Async, 1 --> Flush buffer and wait for write completion.
 * errmsg = Pointer to char array where error messages should be put to.
 * timestamp = Pointer to a double value where write-completion timestamps should be put to.
 *
 * Returns number of bytes written, or -1 on error.
 */
int PsychIOOSWriteSerialPort(PsychSerialDeviceRecord* device, void* writedata, unsigned int amount, int nonblocking, char* errmsg, double* timestamp)
{
	int nwritten;
	
	// Nonblocking mode?
	if (nonblocking > 0) {
		// Yep. Set filedescriptor to non-blocking mode:
		// Set the O_NONBLOCK flag so subsequent I/O will not block.
		// See fcntl(2) ("man 2 fcntl") for details.
		if (fcntl(device->fileDescriptor, F_SETFL, O_NONBLOCK) == -1)
		{
			sprintf(errmsg, "Error setting O_NONBLOCK on device %s for non-blocking writes - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}
		
		// Write the data:
		if ((nwritten = write(device->fileDescriptor, writedata, amount)) == -1)
		{
			sprintf(errmsg, "Error during write to device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}
	}
	else {
		// Nope. Set filedescriptor to blocking mode:
		// Clear the O_NONBLOCK flag so subsequent I/O will not block.
		// See fcntl(2) ("man 2 fcntl") for details.
		if (fcntl(device->fileDescriptor, F_SETFL, 0) == -1)
		{
			sprintf(errmsg, "Error clearing O_NONBLOCK on device %s for blocking writes - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}
		
		// Write the data:
		if ((nwritten = write(device->fileDescriptor, writedata, amount)) == -1)
		{
			sprintf(errmsg, "Error during write to device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}
		
		// Flush the write buffer and wait for write completion on physical hardware:
		if (tcdrain(device->fileDescriptor) == -1) {
			sprintf(errmsg, "Error during write to device %s while draining the write buffers - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}
	}
	
	// Write successfully completed if we reach this point. Take timestamp, clear error message, return:
	PsychGetAdjustedPrecisionTimerSeconds(timestamp);
	errmsg[0] = 0;
	
	return(nwritten);
}

int PsychIOOSReadSerialPort(PsychSerialDeviceRecord* device, void** readdata, unsigned int amount, int nonblocking, char* errmsg, double* timestamp)
{
    	struct termios	options;

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
	if (nonblocking > 0) {
		// Yep. Set filedescriptor to non-blocking mode:
		// Set the O_NONBLOCK flag so subsequent I/O will not block.
		// See fcntl(2) ("man 2 fcntl") for details.
		if (fcntl(device->fileDescriptor, F_SETFL, O_NONBLOCK) == -1)
		{
			sprintf(errmsg, "Error setting O_NONBLOCK on device %s for non-blocking read - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}
		
		// Read the data, at most 'amount' bytes, nonblocking:
		if ((nread = read(device->fileDescriptor, device->readBuffer, amount)) == -1)
		{
			// Nothing to read? In nonblocking mode we simply return zero bytes read:
			if (errno == EAGAIN) return(0);
			
			// Some other error:
			sprintf(errmsg, "Error during non-blocking read from device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}
	}
	else {
		// Nope. Set filedescriptor to blocking mode:
		// Clear the O_NONBLOCK flag so subsequent I/O will not block.
		// See fcntl(2) ("man 2 fcntl") for details.
		if (fcntl(device->fileDescriptor, F_SETFL, 0) == -1)
		{
			sprintf(errmsg, "Error clearing O_NONBLOCK on device %s for blocking read - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}

		// Retrieve current termios settings:
    		if (tcgetattr(device->fileDescriptor, &options) == -1)
    		{
        		sprintf(errmsg, "Error getting current serial port device settings for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
    		}

		// Change the number of bytes that must be received for blocking read()'s before read() returns.
		// read() will return if it can fetch at least that many bytes (we want 'amount' bytes at least),
		// or if the user specified timeout occurs (as set in Open or Configure routine):
		options.c_cc[VMIN] = amount;

		// Cause the new options to take effect immediately.
		if (tcsetattr(device->fileDescriptor, TCSANOW, &options) == -1)
		{
			sprintf(errmsg, "Error setting new serial port configuration attributes for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}
		
		// Read the data, at most (and at least, unless timeout occurs) 'amount' bytes, blocking:
		if ((nread = read(device->fileDescriptor, device->readBuffer, amount)) == -1)
		{
			sprintf(errmsg, "Error during blocking read from device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}		

		// Reset minbytes to zero:
		options.c_cc[VMIN] = 0;

		// Cause the new options to take effect immediately.
		if (tcsetattr(device->fileDescriptor, TCSANOW, &options) == -1)
		{
			sprintf(errmsg, "Error resetting new serial port configuration attributes for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}

	}
	
	// Read successfully completed if we reach this point. Take timestamp, clear error message, return:
	PsychGetAdjustedPrecisionTimerSeconds(timestamp);
	errmsg[0] = 0;

	*readdata = device->readBuffer;	
	return(nread);
}

int PsychIOOSBytesAvailableSerialPort(PsychSerialDeviceRecord* device)
{
	int navail = 0;
	
	if (ioctl(device->fileDescriptor, FIONREAD, &navail)!=0) {
		if (verbosity > 0) printf("Error during 'BytesAvailable': ioctl - FIONREAD byte available query on device %s returned %s(%d)\n", device->portSpec, strerror(errno), errno);
		return(-1);
	}
	
	return(navail);
}

void PsychIOOSFlushSerialPort(PsychSerialDeviceRecord* device)
{	
	if (tcdrain(device->fileDescriptor)!=0) {
		if (verbosity > 0) printf("Error during 'Flush': tcdrain() on device %s returned %s(%d)\n", device->portSpec, strerror(errno), errno);
	}
	
	return;
}

void PsychIOOSPurgeSerialPort(PsychSerialDeviceRecord* device)
{
	if (tcflush(device->fileDescriptor, TCIFLUSH)!=0) {
		if (verbosity > 0) printf("Error during 'Purge': tcflush(TCIFLUSH) on device %s returned %s(%d)\n", device->portSpec, strerror(errno), errno);
	}
	
	return;
}
